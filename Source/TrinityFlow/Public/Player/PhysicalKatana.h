#pragma once

#include "CoreMinimal.h"
#include "Combat/WeaponBase.h"
#include "PhysicalKatana.generated.h"

class UTrinityFlowPhysicalKatanaStats;
struct FPhysicalKatanaAbilityStats;

UCLASS()
class TRINITYFLOW_API APhysicalKatana : public AWeaponBase
{
    GENERATED_BODY()

public:
    APhysicalKatana();

    virtual void BeginPlay() override;

    virtual void BasicAttack(AActor* Target) override;
    virtual void AbilityE(AActor* Target) override; // First ability (TBD)
    virtual void AbilityR(AActor* Target) override; // Second ability (TBD)
    virtual void DefensiveAbility() override; // Order (from Divine Anchor)

    UFUNCTION()
    void OnPerfectOrder();

protected:
    // Cached stats reference
    UPROPERTY()
    UTrinityFlowPhysicalKatanaStats* WeaponStats;
    
    const FPhysicalKatanaAbilityStats* PhysicalKatanaStats;

    // Order defensive ability state (from Divine Anchor)
    UPROPERTY()
    bool bOrderWindowActive = false;

    UPROPERTY()
    float OrderWindowTimer = 0.0f;

    UPROPERTY()
    float IncomingDamage = 0.0f;

    UPROPERTY()
    AActor* OrderAttacker = nullptr;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void StartOrderWindow(float Damage, AActor* Attacker);

    UFUNCTION()
    bool ProcessOrder();

    UFUNCTION()
    void PerformCounterAttack(AActor* Target);
};