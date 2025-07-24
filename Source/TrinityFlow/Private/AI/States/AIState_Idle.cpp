#include "AI/States/AIState_Idle.h"
#include "Enemy/EnemyBase.h"
#include "AI/EnemyAIController.h"
#include "Core/StateComponent.h"
#include "Core/TrinityFlowTypes.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UAIState_Idle::UAIState_Idle()
{
	StateName = "Idle";
	TimeSinceLastCheck = 0.0f;
	
	// Default state transitions will be set in blueprints or via data assets
}

void UAIState_Idle::Enter(AEnemyBase* Enemy, AEnemyAIController* AIController)
{
	Super::Enter(Enemy, AIController);
	
	TimeSinceLastCheck = 0.0f;
	
	if (CachedEnemy && CachedEnemy->GetStateComponent())
	{
		CachedEnemy->GetStateComponent()->RemoveState(ECharacterState::Combat);
		CachedEnemy->GetStateComponent()->AddState(ECharacterState::NonCombat);
	}
}

void UAIState_Idle::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (!CachedEnemy || !CachedAIController)
	{
		return;
	}

	TimeSinceLastCheck += DeltaTime;
	
	if (TimeSinceLastCheck >= DetectionCheckInterval)
	{
		TimeSinceLastCheck = 0.0f;
		
		if (CheckForPlayer())
		{
			if (ChaseStateClass)
			{
				TransitionToState(ChaseStateClass);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Idle State: No ChaseStateClass set!"));
			}
		}
	}
}

void UAIState_Idle::Exit()
{
	Super::Exit();
}

bool UAIState_Idle::CheckForPlayer()
{
	if (!CachedEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckForPlayer: No CachedEnemy"));
		return false;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(CachedEnemy->GetWorld(), 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckForPlayer: No PlayerController"));
		return false;
	}

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckForPlayer: No PlayerPawn"));
		return false;
	}

	float DistanceToPlayer = FVector::Dist(CachedEnemy->GetActorLocation(), PlayerPawn->GetActorLocation());
	float SightRange = CachedEnemy->GetSightRange();
	
	if (DistanceToPlayer <= SightRange)
	{
		FVector StartLocation = CachedEnemy->GetActorLocation() + FVector(0, 0, 50);
		FVector EndLocation = PlayerPawn->GetActorLocation() + FVector(0, 0, 50);
		
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

		DrawDebugLine(CachedEnemy->GetWorld(), StartLocation, EndLocation, 
			!bHit || HitResult.GetActor() == PlayerPawn ? FColor::Green : FColor::Red, 
			false, 0.5f);
		
		DrawDebugSphere(CachedEnemy->GetWorld(), CachedEnemy->GetActorLocation(), SightRange, 24, FColor::Yellow, false, 0.5f);

		if (!bHit || HitResult.GetActor() == PlayerPawn)
		{
			CachedEnemy->SetTargetPlayer(PlayerPawn);
			CachedEnemy->bHasSeenPlayer = true;  // Set the flag
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CheckForPlayer: Player blocked by %s"), HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("Unknown"));
		}
	}

	return false;
}