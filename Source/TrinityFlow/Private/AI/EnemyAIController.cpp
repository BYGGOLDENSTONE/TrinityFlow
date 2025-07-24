#include "AI/EnemyAIController.h"
#include "Enemy/EnemyBase.h"
#include "AI/AIStateMachine.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AEnemyAIController::AEnemyAIController()
{
	bWantsPlayerState = false;
	
	SetGenericTeamId(FGenericTeamId(1));
	
	// Enable pathfinding
	bSetControlRotationFromPawnOrientation = false;
}

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
		UE_LOG(LogTemp, Warning, TEXT("AIController %s has PathFollowingComponent"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController %s has NO PathFollowingComponent!"), *GetName());
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if (Enemy)
	{
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