#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    bool CanAttack() const;

    UFUNCTION()
    void StartAttack(AActor* Target, float Range, EDamageType DamageType, bool bIsAreaDamage = false);

    UFUNCTION()
    float GetAttackCooldownRemaining() const { return AttackCooldownTimer; }

    UFUNCTION()
    void SetAttackSpeed(float AttacksPerSecond);

    UFUNCTION()
    float GetAttackRange() const { return AttackRange; }

    UFUNCTION()
    void SetAttackRange(float NewRange) { AttackRange = NewRange; }

    UPROPERTY()
    FOnAttack OnAttack;

protected:
    UPROPERTY()
    float AttackCooldown = 1.5f;

    UPROPERTY()
    float AttackCooldownTimer = 0.0f;

    UPROPERTY()
    float AttackRange = 300.0f;

    UPROPERTY()
    float CastingTime = 1.5f;

    UPROPERTY()
    float CastingTimer = 0.0f;

    UPROPERTY()
    bool bIsCasting = false;

    UPROPERTY()
    AActor* CurrentTarget = nullptr;

    UPROPERTY()
    EDamageType PendingDamageType = EDamageType::Physical;

    UPROPERTY()
    bool bPendingAreaDamage = false;

private:
    void ExecuteAttack();
    class UHealthComponent* OwnerHealthComponent;
};