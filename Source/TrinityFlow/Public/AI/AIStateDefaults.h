#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIStateDefaults.generated.h"

/**
 * Data asset to hold default AI state class references
 * This helps wire up the state transitions in blueprints
 */
UCLASS()
class TRINITYFLOW_API UAIStateDefaults : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI States")
	TSubclassOf<class UAIState> IdleStateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI States")
	TSubclassOf<class UAIState> ChaseStateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI States")
	TSubclassOf<class UAIState> AttackStateClass;
};