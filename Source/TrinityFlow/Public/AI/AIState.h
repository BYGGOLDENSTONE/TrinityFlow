#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIState.generated.h"

class UAIStateMachine;
class AEnemyBase;
class AEnemyAIController;

UCLASS(Abstract, Blueprintable)
class TRINITYFLOW_API UAIState : public UObject
{
	GENERATED_BODY()

public:
	UAIState();

	virtual void Enter(AEnemyBase* Enemy, AEnemyAIController* AIController);
	virtual void Update(float DeltaTime);
	virtual void Exit();

	UFUNCTION(BlueprintCallable, Category = "AI State")
	void TransitionToState(TSubclassOf<UAIState> NewStateClass);

	UFUNCTION(BlueprintPure, Category = "AI State")
	AEnemyBase* GetEnemy() const { return CachedEnemy; }

	UFUNCTION(BlueprintPure, Category = "AI State")
	AEnemyAIController* GetAIController() const { return CachedAIController; }

	UFUNCTION(BlueprintPure, Category = "AI State")
	UAIStateMachine* GetStateMachine() const { return CachedStateMachine; }

protected:
	UPROPERTY()
	AEnemyBase* CachedEnemy;

	UPROPERTY()
	AEnemyAIController* CachedAIController;

	UPROPERTY()
	UAIStateMachine* CachedStateMachine;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	FString StateName;
};