#include "AI/AIStateMachine.h"
#include "AI/AIState.h"
#include "Enemy/EnemyBase.h"
#include "AI/EnemyAIController.h"

UAIStateMachine::UAIStateMachine()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIStateMachine::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerEnemy = Cast<AEnemyBase>(GetOwner());
	if (OwnerEnemy)
	{
		OwnerAIController = Cast<AEnemyAIController>(OwnerEnemy->GetController());
		if (!OwnerAIController)
		{
			UE_LOG(LogTemp, Warning, TEXT("AIStateMachine: Owner %s has no AI Controller!"), *OwnerEnemy->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AIStateMachine: Owner %s has AI Controller %s"), *OwnerEnemy->GetName(), *OwnerAIController->GetName());
		}
	}
}

void UAIStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState)
	{
		CurrentState->Update(DeltaTime);
	}
}

void UAIStateMachine::Initialize(TSubclassOf<UAIState> InitialStateClass)
{
	if (!InitialStateClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AIStateMachine: InitialStateClass is null"));
		return;
	}

	CreateAndEnterState(InitialStateClass);
}

void UAIStateMachine::ChangeState(TSubclassOf<UAIState> NewStateClass)
{
	if (!NewStateClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AIStateMachine: NewStateClass is null"));
		return;
	}

	if (CurrentStateClass == NewStateClass)
	{
		return;
	}


	if (CurrentState)
	{
		CurrentState->Exit();
		CurrentState = nullptr;
	}

	CreateAndEnterState(NewStateClass);
}

void UAIStateMachine::CreateAndEnterState(TSubclassOf<UAIState> StateClass)
{
	if (!StateClass || !OwnerEnemy)
	{
		return;
	}

	CurrentStateClass = StateClass;
	CurrentState = NewObject<UAIState>(this, StateClass);
	
	if (CurrentState)
	{
		CurrentState->Enter(OwnerEnemy, OwnerAIController);
	}
}