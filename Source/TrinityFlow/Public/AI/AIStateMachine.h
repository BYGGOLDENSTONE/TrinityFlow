#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIStateMachine.generated.h"

class UAIState;
class AEnemyBase;
class AEnemyAIController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UAIStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIStateMachine();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "AI State Machine")
	void Initialize(TSubclassOf<UAIState> InitialStateClass);

	UFUNCTION(BlueprintCallable, Category = "AI State Machine")
	void ChangeState(TSubclassOf<UAIState> NewStateClass);

	UFUNCTION(BlueprintPure, Category = "AI State Machine")
	UAIState* GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "AI State Machine")
	TSubclassOf<UAIState> GetCurrentStateClass() const { return CurrentStateClass; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UAIState* CurrentState;

	UPROPERTY()
	TSubclassOf<UAIState> CurrentStateClass;

	UPROPERTY()
	AEnemyBase* OwnerEnemy;

	UPROPERTY()
	AEnemyAIController* OwnerAIController;

	void CreateAndEnterState(TSubclassOf<UAIState> StateClass);
};