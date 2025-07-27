#pragma once

#include "CoreMinimal.h"
#include "Data/TrinityFlowWeaponStatsBase.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowAnchorStats.generated.h"

/**
 * Stats for Divine Anchor abilities
 */
USTRUCT(BlueprintType)
struct FAnchorAbilityStats
{
    GENERATED_BODY()

    // Basic Attack Timing
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0", ClampMax = "2"))
    float BasicAttackDamageDelay = 0.7f; // Delay before damage is dealt (in seconds) - slightly longer for heavy weapon

    // Smash (Basic Attack)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash")
    float SmashKnockbackForce = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash")
    float SmashKnockbackDuration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smash")
    float SmashKnockbackUpwardRatio = 0.5f; // Z component of knockback

    // Gravity Pull (Q)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullCooldown = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullDuration = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullForceMultiplier = 2.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullUpwardVelocity = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity Pull")
    float GravityPullVulnerableDuration = 1.5f;

    // Holy Gravity (E)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Holy Gravity")
    float HolyGravityRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Holy Gravity")
    float HolyGravityCooldown = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Holy Gravity")
    float HolyGravityVulnerableDuration = 2.0f;

    // Order
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order")
    float OrderWindowDuration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order")
    float PerfectOrderStart = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order", meta = (ClampMin = "0", ClampMax = "1"))
    float ModerateOrderDamageMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Order", meta = (ClampMin = "0", ClampMax = "1"))
    float ArmorReductionPercent = 0.25f; // 25% armor reduction on counter
};

/**
 * Data asset for Divine Anchor weapon stats configuration
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowAnchorStats : public UTrinityFlowWeaponStatsBase
{
    GENERATED_BODY()

public:
    // Basic Attack Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "50", ClampMax = "2000"))
    float BasicAttackRange = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float BasicAttackSpeed = 0.5f; // Attacks per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack")
    EDamageType BasicDamageType = EDamageType::Physical;

    // Ability Cooldowns
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityQCooldown = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityECooldown = 10.0f;

    // Anchor-specific ability stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    FAnchorAbilityStats AnchorStats;
};