#pragma once

#include "CoreMinimal.h"
#include "Combat/WeaponBase.h"
#include "DivineAnchor.generated.h"

UCLASS()
class TRINITYFLOW_API ADivineAnchor : public AWeaponBase
{
    GENERATED_BODY()

public:
    ADivineAnchor();

    virtual void BasicAttack(AActor* Target) override; // Smash (area damage)
    virtual void AbilityQ(AActor* Target) override; // Gravity Pull
    virtual void AbilityE(AActor* Target) override; // Holy Gravity
    virtual void DefensiveAbility() override; // Order

protected:
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

private:
    void PerformCounterAttack(AActor* Target);
};