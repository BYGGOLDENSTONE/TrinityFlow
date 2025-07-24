#include "AI/States/AIState_Chase.h"
#include "Enemy/EnemyBase.h"
#include "AI/EnemyAIController.h"
#include "Core/StateComponent.h"
#include "Core/TrinityFlowTypes.h"
#include "Core/CombatStateManager.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UAIState_Chase::UAIState_Chase()
{
	StateName = "Chase";
	TimeSinceLastPathUpdate = 0.0f;
	
	// Default state transitions will be set in blueprints or via data assets
}

void UAIState_Chase::Enter(AEnemyBase* Enemy, AEnemyAIController* AIController)
{
	Super::Enter(Enemy, AIController);
	
	TimeSinceLastPathUpdate = 0.0f;
	
	
	if (CachedEnemy && CachedEnemy->GetStateComponent())
	{
		CachedEnemy->GetStateComponent()->RemoveState(ECharacterState::NonCombat);
		CachedEnemy->GetStateComponent()->AddState(ECharacterState::Combat);
	}
	
	// Notify combat state manager
	if (UWorld* World = CachedEnemy->GetWorld())
	{
		if (UCombatStateManager* CombatManager = World->GetSubsystem<UCombatStateManager>())
		{
			CombatManager->OnEnemySeesPlayer(CachedEnemy);
		}
	}
	
	UpdatePath();
}

void UAIState_Chase::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (!CachedEnemy || !CachedAIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chase State: No enemy or controller"));
		return;
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		if (IdleStateClass)
		{
			TransitionToState(IdleStateClass);
		}
		return;
	}
	
	// Check movement component status
	if (UPawnMovementComponent* MoveComp = CachedEnemy->GetMovementComponent())
	{
		FVector Velocity = MoveComp->Velocity;
		if (Velocity.SizeSquared() < 1.0f)
		{
			static int32 StuckCounter = 0;
			StuckCounter++;
			if (StuckCounter % 60 == 0) // Log every second if stuck
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy %s appears stuck! Velocity: %s, MaxSpeed: %.1f"), 
					*CachedEnemy->GetName(), 
					*Velocity.ToString(),
					MoveComp->GetMaxSpeed());
			}
		}
	}

	if (HasLostTarget())
	{
		CachedEnemy->SetTargetPlayer(nullptr);
		if (IdleStateClass)
		{
			TransitionToState(IdleStateClass);
		}
		return;
	}

	if (IsInAttackRange())
	{
		if (AttackStateClass)
		{
			TransitionToState(AttackStateClass);
		}
		return;
	}

	CachedEnemy->FaceTarget(Target);

	TimeSinceLastPathUpdate += DeltaTime;
	if (TimeSinceLastPathUpdate >= PathUpdateInterval)
	{
		TimeSinceLastPathUpdate = 0.0f;
		UpdatePath();
	}
}

void UAIState_Chase::Exit()
{
	Super::Exit();
	
	if (CachedAIController)
	{
		CachedAIController->StopMovement();
	}
}

void UAIState_Chase::UpdatePath()
{
	if (!CachedAIController || !CachedEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: No controller or enemy"));
		return;
	}
	
	// Double-check controller is valid and matches
	if (CachedEnemy->GetController() != CachedAIController)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: Controller mismatch! Enemy controller = %s, Cached = %s"),
			CachedEnemy->GetController() ? *CachedEnemy->GetController()->GetName() : TEXT("NULL"),
			CachedAIController ? *CachedAIController->GetName() : TEXT("NULL"));
		
		// Try to update the controller
		CachedAIController = Cast<AEnemyAIController>(CachedEnemy->GetController());
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: No target"));
		return;
	}

	// Check nav system
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(CachedEnemy->GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: No Navigation System!"));
		return;
	}

	// Check if location is on nav mesh
	FNavLocation NavLocation;
	bool bOnNavMesh = NavSys->ProjectPointToNavigation(CachedEnemy->GetActorLocation(), NavLocation, FVector(500, 500, 500));
	
	if (!bOnNavMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: Enemy %s is NOT on NavMesh! Location: %s"), 
			*CachedEnemy->GetName(), *CachedEnemy->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdatePath: Enemy %s IS on NavMesh. Enemy Loc: %s, Nav Loc: %s"), 
			*CachedEnemy->GetName(), 
			*CachedEnemy->GetActorLocation().ToString(),
			*NavLocation.Location.ToString());
	}
	
	// Check if target is on nav mesh
	FNavLocation TargetNavLocation;
	bool bTargetOnNavMesh = NavSys->ProjectPointToNavigation(Target->GetActorLocation(), TargetNavLocation, FVector(500, 500, 500));
	
	if (!bTargetOnNavMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdatePath: Target is NOT on NavMesh! Location: %s"), 
			*Target->GetActorLocation().ToString());
	}

	// Try SimpleMoveToActor first - it's more forgiving
	EPathFollowingRequestResult::Type SimpleResult = CachedAIController->MoveToActor(Target, AcceptanceRadius);
	
	// Log result
	switch (SimpleResult)
	{
		case EPathFollowingRequestResult::Failed:
			{
				UE_LOG(LogTemp, Error, TEXT("UpdatePath: MoveTo FAILED for %s"), *CachedEnemy->GetName());
				
				// Additional debugging for failure
				if (!CachedAIController->GetPathFollowingComponent())
				{
					UE_LOG(LogTemp, Error, TEXT("  - Reason: NO PathFollowingComponent!"));
				}
				else if (!CachedEnemy->GetMovementComponent())
				{
					UE_LOG(LogTemp, Error, TEXT("  - Reason: NO MovementComponent!"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("  - PathFollowing: %s, Movement: %s"), 
						*CachedAIController->GetPathFollowingComponent()->GetClass()->GetName(),
						*CachedEnemy->GetMovementComponent()->GetClass()->GetName());
					
					// Check if movement component is properly linked
					if (CachedAIController->GetPawn() != CachedEnemy)
					{
						UE_LOG(LogTemp, Error, TEXT("  - Controller pawn mismatch! Controller's pawn: %s"), 
							CachedAIController->GetPawn() ? *CachedAIController->GetPawn()->GetName() : TEXT("NULL"));
					}
					
				}
			}
			break;
		case EPathFollowingRequestResult::AlreadyAtGoal:
			UE_LOG(LogTemp, Warning, TEXT("UpdatePath: %s is already at goal"), *CachedEnemy->GetName());
			break;
		case EPathFollowingRequestResult::RequestSuccessful:
			UE_LOG(LogTemp, Log, TEXT("UpdatePath: MoveTo successful for %s"), *CachedEnemy->GetName());
			break;
	}
	

	#if WITH_EDITOR
	if (CachedEnemy->GetWorld()->WorldType == EWorldType::Editor)
	{
		DrawDebugSphere(CachedEnemy->GetWorld(), Target->GetActorLocation(), 
			AcceptanceRadius, 12, FColor::Yellow, false, PathUpdateInterval);
		
		// Draw path if it exists
		if (SimpleResult == EPathFollowingRequestResult::RequestSuccessful)
		{
			DrawDebugLine(CachedEnemy->GetWorld(), CachedEnemy->GetActorLocation(), 
				Target->GetActorLocation(), FColor::Green, false, PathUpdateInterval, 0, 5.0f);
		}
	}
	#endif
}

bool UAIState_Chase::IsInAttackRange()
{
	if (!CachedEnemy)
	{
		return false;
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		return false;
	}

	float Distance = FVector::Dist(CachedEnemy->GetActorLocation(), Target->GetActorLocation());
	return Distance <= CachedEnemy->GetAttackRange();
}

bool UAIState_Chase::HasLostTarget()
{
	if (!CachedEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("HasLostTarget: No enemy"));
		return true;
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("HasLostTarget: No target"));
		return true;
	}

	float Distance = FVector::Dist(CachedEnemy->GetActorLocation(), Target->GetActorLocation());
	
	if (Distance > LostTargetDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("HasLostTarget: Distance %.0f > LostTargetDistance %.0f"), Distance, LostTargetDistance);
		return true;
	}

	FVector StartLocation = CachedEnemy->GetActorLocation() + FVector(0, 0, 50);
	FVector EndLocation = Target->GetActorLocation() + FVector(0, 0, 50);
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CachedEnemy);
	
	bool bHit = CachedEnemy->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	bool bCanSeeTarget = !bHit || HitResult.GetActor() == Target;
	if (!bCanSeeTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HasLostTarget: Line of sight blocked"));
	}
	
	return !bCanSeeTarget;
}