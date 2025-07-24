#include "AI/AIState.h"
#include "AI/AIStateMachine.h"
#include "Enemy/EnemyBase.h"
#include "AI/EnemyAIController.h"

UAIState::UAIState()
{
	StateName = "Base State";
}

void UAIState::Enter(AEnemyBase* Enemy, AEnemyAIController* AIController)
{
	CachedEnemy = Enemy;
	CachedAIController = AIController;
	
	if (Enemy)
	{
		CachedStateMachine = Enemy->FindComponentByClass<UAIStateMachine>();
	}

	UE_LOG(LogTemp, Log, TEXT("AI State: Entering %s"), *StateName);
}

void UAIState::Update(float DeltaTime)
{
	// Override in derived classes
}

void UAIState::Exit()
{
	UE_LOG(LogTemp, Log, TEXT("AI State: Exiting %s"), *StateName);
}

void UAIState::TransitionToState(TSubclassOf<UAIState> NewStateClass)
{
	if (CachedStateMachine && NewStateClass)
	{
		CachedStateMachine->ChangeState(NewStateClass);
	}
}