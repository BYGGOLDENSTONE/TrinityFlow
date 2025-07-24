#include "AI/EnemyAIController.h"
#include "Enemy/EnemyBase.h"
#include "AI/AIStateMachine.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/PawnMovementComponent.h"

AEnemyAIController::AEnemyAIController()
{
	bWantsPlayerState = false;
	
	SetGenericTeamId(FGenericTeamId(1));
	
	// Enable pathfinding
	bSetControlRotationFromPawnOrientation = false;
}

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	bWantsPlayerState = false;
	
	SetGenericTeamId(FGenericTeamId(1));
	
	// Enable pathfinding
	bSetControlRotationFromPawnOrientation = false;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupAIComponents();
	
	// Check navigation component
	if (GetPathFollowingComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController %s has PathFollowingComponent of type %s"), 
			*GetName(), 
			*GetPathFollowingComponent()->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController %s has NO PathFollowingComponent!"), *GetName());
	}
	
	// Check if navigation system exists
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Navigation System is available"));
		
		// Check if nav mesh is built
		FNavLocation TestLocation;
		bool bHasNavMesh = NavSys->ProjectPointToNavigation(GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector, 
			TestLocation, FVector(500, 500, 500));
		UE_LOG(LogTemp, Warning, TEXT("NavMesh at controller location: %s"), bHasNavMesh ? TEXT("YES") : TEXT("NO"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO Navigation System found!"));
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
		// Check PathFollowingComponent
		if (GetPathFollowingComponent())
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyAIController: Has PathFollowingComponent %s"), 
				*GetPathFollowingComponent()->GetClass()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EnemyAIController: NO PathFollowingComponent!"));
		}
		
		// Log movement component status
		if (Enemy->GetMovementComponent())
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyAIController: Possessed enemy %s has movement component %s"), 
				*Enemy->GetName(), 
				*Enemy->GetMovementComponent()->GetClass()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EnemyAIController: Possessed enemy %s has NO movement component!"), 
				*Enemy->GetName());
		}
		
		UAIStateMachine* StateMachine = Enemy->FindComponentByClass<UAIStateMachine>();
		if (StateMachine)
		{
			UE_LOG(LogTemp, Log, TEXT("EnemyAIController: Found StateMachine on %s"), *Enemy->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyAIController: No StateMachine found on %s"), *Enemy->GetName());
		}
	}
}

void AEnemyAIController::OnUnPossess()
{
	StopMovement();
	
	Super::OnUnPossess();
}

void AEnemyAIController::SetupAIComponents()
{
	// No blackboard needed for state machine based AI
}