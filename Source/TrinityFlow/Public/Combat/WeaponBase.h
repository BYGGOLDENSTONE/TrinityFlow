#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/TrinityFlowTypes.h"
#include "WeaponBase.generated.h"

UCLASS(Abstract)
class TRINITYFLOW_API AWeaponBase : public AActor
{
    GENERATED_BODY()
    
public:    
    AWeaponBase();

    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void BasicAttack(AActor* Target);

    UFUNCTION()
    virtual void ExecuteBasicAttack(AActor* Target);

    UFUNCTION()
    virtual void AbilityQ(AActor* Target) { }

    UFUNCTION()
    virtual void AbilityE(AActor* Target) { }

    UFUNCTION()
    virtual void AbilityR(AActor* Target) { }  // New ability for dual-wielding

    UFUNCTION()
    virtual void DefensiveAbility() { }

    UFUNCTION()
    bool IsAbilityQReady() const { return AbilityQCooldownTimer <= 0.0f; }

    UFUNCTION()
    bool IsAbilityEReady() const { return AbilityECooldownTimer <= 0.0f; }

    UFUNCTION()
    float GetAbilityQCooldownRemaining() const { return AbilityQCooldownTimer; }

    UFUNCTION()
    float GetAbilityECooldownRemaining() const { return AbilityECooldownTimer; }

    UFUNCTION()
    float GetAttackDuration() const { return BasicAttackDamageDelay + 0.5f; } // Attack time + recovery

    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY()
    class USceneComponent* Root;

    UPROPERTY()
    float BasicAttackRange = 300.0f;

    UPROPERTY()
    float BasicAttackSpeed = 1.0f;

    UPROPERTY()
    EDamageType BasicDamageType = EDamageType::Physical;

    UPROPERTY()
    float AbilityQCooldown = 5.0f;

    UPROPERTY()
    float AbilityQCooldownTimer = 0.0f;

    UPROPERTY()
    float AbilityECooldown = 6.0f;

    UPROPERTY()
    float AbilityECooldownTimer = 0.0f;

    UPROPERTY()
    class APawn* OwnerPawn;

    UPROPERTY()
    class UHealthComponent* OwnerHealthComponent;

    // Attack delay system
    UPROPERTY()
    float BasicAttackDamageDelay = 0.6f; // Time in seconds to wait before dealing damage

    UPROPERTY()
    FTimerHandle AttackTimerHandle;

    UPROPERTY()
    AActor* PendingAttackTarget;

    void StartCooldown(float& Timer, float Cooldown);
};