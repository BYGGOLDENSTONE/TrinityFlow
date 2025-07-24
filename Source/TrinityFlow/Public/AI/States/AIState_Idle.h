#pragma once

#include "CoreMinimal.h"
#include "AI/AIState.h"
#include "AIState_Idle.generated.h"

UCLASS()
class TRINITYFLOW_API UAIState_Idle : public UAIState
{
	GENERATED_BODY()

public:
	UAIState_Idle();

	virtual void Enter(AEnemyBase* Enemy, AEnemyAIController* AIController) override;
	virtual void Update(float DeltaTime) override;
	virtual void Exit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	float DetectionCheckInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	TSubclassOf<UAIState> ChaseStateClass;

private:
	float TimeSinceLastCheck;
	
	bool CheckForPlayer();
};