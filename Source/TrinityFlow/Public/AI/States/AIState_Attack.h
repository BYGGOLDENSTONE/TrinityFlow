#pragma once

#include "CoreMinimal.h"
#include "AI/AIState.h"
#include "AIState_Attack.generated.h"

UCLASS()
class TRINITYFLOW_API UAIState_Attack : public UAIState
{
	GENERATED_BODY()

public:
	UAIState_Attack();

	virtual void Enter(AEnemyBase* Enemy, AEnemyAIController* AIController) override;
	virtual void Update(float DeltaTime) override;
	virtual void Exit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float MaxAttackRange = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	TSubclassOf<UAIState> ChaseStateClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	TSubclassOf<UAIState> IdleStateClass;

private:
	float TimeSinceLastAttack;
	bool bIsAttacking;
	
	void PerformAttack();
	bool IsTargetInRange();
};