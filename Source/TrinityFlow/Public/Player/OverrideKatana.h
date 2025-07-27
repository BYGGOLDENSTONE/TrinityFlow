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

    // Override basic attack to handle both left and right attacks
    virtual void BasicAttack(AActor* Target) override;
    
    // Katana-specific attack methods
    UFUNCTION()
    void BasicAttackLeft(AActor* Target);
    
    UFUNCTION()
    void BasicAttackRight(AActor* Target);

    virtual void AbilityQ(AActor* Target) override; // Echoes of Data
    virtual void AbilityE(AActor* Target) override; // Code Break
    virtual void DefensiveAbility() override; // Scripted Dodge

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

    // Attack tracking for left/right attacks
    UPROPERTY()
    bool bIsLeftAttack = true;
    
    UPROPERTY()
    FTimerHandle LeftAttackTimerHandle;
    
    UPROPERTY()
    FTimerHandle RightAttackTimerHandle;
    
    UPROPERTY()
    AActor* PendingLeftTarget;
    
    UPROPERTY()
    AActor* PendingRightTarget;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void StartDodgeWindow(float Damage, AActor* Attacker);

    UFUNCTION()
    bool ProcessDodge(float& OutDamageMultiplier);
};