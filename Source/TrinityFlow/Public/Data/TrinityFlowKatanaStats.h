#pragma once

#include "CoreMinimal.h"
#include "Data/TrinityFlowWeaponStatsBase.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowKatanaStats.generated.h"

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
 * Data asset for Override Katana weapon stats configuration
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowKatanaStats : public UTrinityFlowWeaponStatsBase
{
    GENERATED_BODY()

public:
    // Basic Attack Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "50", ClampMax = "2000"))
    float BasicAttackRange = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float BasicAttackSpeed = 1.0f; // Attacks per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic Attack")
    EDamageType BasicDamageType = EDamageType::Physical;

    // Ability Cooldowns
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityQCooldown = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    float AbilityECooldown = 6.0f;

    // Katana-specific ability stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    FKatanaAbilityStats KatanaStats;
};