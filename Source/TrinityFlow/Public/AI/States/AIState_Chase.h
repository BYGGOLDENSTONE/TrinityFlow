#pragma once

#include "CoreMinimal.h"
#include "AI/AIState.h"
#include "AIState_Chase.generated.h"

UCLASS()
class TRINITYFLOW_API UAIState_Chase : public UAIState
{
	GENERATED_BODY()

public:
	UAIState_Chase();

	virtual void Enter(AEnemyBase* Enemy, AEnemyAIController* AIController) override;
	virtual void Update(float DeltaTime) override;
	virtual void Exit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float AcceptanceRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float PathUpdateInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float LostTargetDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	TSubclassOf<UAIState> AttackStateClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	TSubclassOf<UAIState> IdleStateClass;

private:
	float TimeSinceLastPathUpdate;
	
	void UpdatePath();
	bool IsInAttackRange();
	bool HasLostTarget();
};