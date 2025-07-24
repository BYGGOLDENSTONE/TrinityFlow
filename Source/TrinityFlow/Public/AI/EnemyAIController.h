#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class TRINITYFLOW_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	AEnemyAIController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PathfindingAcceptanceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bEnableDebugDrawing = false;

private:
	void SetupAIComponents();
};