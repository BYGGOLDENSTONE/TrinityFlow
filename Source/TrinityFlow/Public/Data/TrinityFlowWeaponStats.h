#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowWeaponStats.generated.h"

/**
 * Stats for Override Katana abilities
 */
USTRUCT(BlueprintType)
struct FKatanaAbilityStats
{
    GENERATED_BODY()

    // Echoes of Data (Q)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echoes of Data")
    float EchoesRange = 4000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echoes of Data")
    float EchoesCooldown = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echoes of Data")
    float EchoesDuration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echoes of Data", meta = (ClampMin = "0", ClampMax = "100"))
    float EchoDamagePercent = 75.0f; // Percentage of original damage

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Echoes of Data", meta = (ClampMin = "0", ClampMax = "100"))
    float EchoAoEDamagePercent = 30.0f; // Percentage for AoE damage

    // Code Break (E)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Code Break")
    float CodeBreakRange = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Code Break")
    float CodeBreakCooldown = 6.0f;

    // Scripted Dodge
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scripted Dodge")
    float DodgeWindowDuration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scripted Dodge")
    float PerfectDodgeStart = 0.75f; // When perfect window starts

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scripted Dodge", meta = (ClampMin = "0", ClampMax = "1"))
    float ModerateDodgeDamageMultiplier = 0.5f; // Half damage
};

/**
 * Stats for Divine Anchor abilities
 */
USTRUCT(BlueprintType)
struct FAnchorAbilityStats
{
    GENERATED_BODY()

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
 * Data asset for weapon stats configuration
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowWeaponStats : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Weapon display name */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
    FString WeaponName;

    /** Weapon type identifier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
    FName WeaponID;

    // Basic Attack Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "50", ClampMax = "2000"))
    float BasicAttackRange = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float BasicAttackSpeed = 1.0f; // Attacks per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack")
    EDamageType BasicDamageType = EDamageType::Physical;

    // Weapon-specific ability stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (EditCondition = "WeaponID == 'OverrideKatana'"))
    FKatanaAbilityStats KatanaStats;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (EditCondition = "WeaponID == 'DivineAnchor'"))
    FAnchorAbilityStats AnchorStats;
};