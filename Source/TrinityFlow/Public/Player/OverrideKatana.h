#pragma once

#include "CoreMinimal.h"
#include "Combat/WeaponBase.h"
#include "OverrideKatana.generated.h"

UCLASS()
class TRINITYFLOW_API AOverrideKatana : public AWeaponBase
{
    GENERATED_BODY()

public:
    AOverrideKatana();

    virtual void BeginPlay() override;

    virtual void AbilityQ(AActor* Target) override; // Echoes of Data
    virtual void AbilityE(AActor* Target) override; // Code Break
    virtual void DefensiveAbility() override; // Scripted Dodge

    UFUNCTION()
    void OnPerfectDodge();

protected:
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