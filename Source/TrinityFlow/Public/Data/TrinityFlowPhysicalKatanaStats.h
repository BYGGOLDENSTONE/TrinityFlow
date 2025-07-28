#pragma once

#include "CoreMinimal.h"
#include "Data/TrinityFlowWeaponStatsBase.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowPhysicalKatanaStats.generated.h"

/**
 * Stats for Physical Katana abilities
 */
USTRUCT(BlueprintType)
struct FPhysicalKatanaAbilityStats
{
    GENERATED_BODY()

    // Basic Attack Timing
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0", ClampMax = "2"))
    float BasicAttackDamageDelay = 0.6f; // Delay before damage is dealt (in seconds)

    // Order Defensive Ability (from Divine Anchor)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order Defense")
    float OrderWindowDuration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order Defense")
    float PerfectOrderStart = 0.75f; // When perfect window starts

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order Defense", meta = (ClampMin = "0", ClampMax = "1"))
    float ModerateOrderDamageMultiplier = 0.5f; // Half damage

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order Defense", meta = (ClampMin = "0", ClampMax = "1"))
    float CounterArmorReduction = 0.25f; // 25% armor reduction on counter

    // Placeholder for future abilities
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability E")
    float AbilityERange = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability R")
    float AbilityRRange = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability R")
    float AbilityRCooldown = 8.0f;
};

/**
 * Data asset for Physical Katana weapon stats configuration
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowPhysicalKatanaStats : public UTrinityFlowWeaponStatsBase
{
    GENERATED_BODY()

public:
    // Basic Attack Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "50", ClampMax = "2000"))
    float BasicAttackRange = 350.0f; // Slightly longer range than soul katana

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float BasicAttackSpeed = 0.8f; // Slightly slower than soul katana

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack")
    EDamageType BasicDamageType = EDamageType::Physical;

    // Ability Cooldowns
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityECooldown = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityRCooldown = 8.0f;

    // Physical katana-specific ability stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    FPhysicalKatanaAbilityStats PhysicalKatanaStats;
};