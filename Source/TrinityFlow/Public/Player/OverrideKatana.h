#pragma once

#include "CoreMinimal.h"
#include "Combat/WeaponBase.h"
#include "OverrideKatana.generated.h"

class UTrinityFlowKatanaStats;
struct FKatanaAbilityStats;

UCLASS()
class TRINITYFLOW_API AOverrideKatana : public AWeaponBase
{
    GENERATED_BODY()

public:
    AOverrideKatana();

    virtual void BeginPlay() override;

    // Override basic attack for soul damage
    virtual void BasicAttack(AActor* Target) override;

    // Remapped abilities for dual-wielding
    virtual void AbilityQ(AActor* Target) override; // Code Break (was E)
    virtual void AbilityTab(AActor* Target); // Echoes of Data (was Q)
    virtual void DefensiveAbility() override; // Scripted Dodge (now on Shift)

    UFUNCTION()
    void OnPerfectDodge();

protected:
    // Cached stats reference
    UPROPERTY()
    UTrinityFlowKatanaStats* WeaponStats;
    
    const FKatanaAbilityStats* KatanaStats;

    UPROPERTY()
    class UAbilityComponent* AbilityComponent;

    UPROPERTY()
    bool bDodgeWindowActive = false;

    UPROPERTY()
    float DodgeWindowTimer = 0.0f;

    UPROPERTY()
    float IncomingDamage = 0.0f;

    UPROPERTY()
    AActor* DodgeAttacker = nullptr;


public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void StartDodgeWindow(float Damage, AActor* Attacker);

    UFUNCTION()
    bool ProcessDodge(float& OutDamageMultiplier);
};