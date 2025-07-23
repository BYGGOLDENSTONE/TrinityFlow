#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TrinityFlowWeaponStatsBase.generated.h"

/**
 * Base class for weapon stats configuration
 * Contains only identification properties
 * All combat-related stats are defined in weapon-specific subclasses
 */
UCLASS(Abstract, BlueprintType)
class TRINITYFLOW_API UTrinityFlowWeaponStatsBase : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Weapon display name */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
    FString WeaponName;

    /** Weapon type identifier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
    FName WeaponID;
};