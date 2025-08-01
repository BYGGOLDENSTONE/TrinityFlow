#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "StanceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStanceChanged, EStanceType, NewStance);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UStanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStanceComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Stance Management
    UFUNCTION(BlueprintCallable, Category = "Stance")
    EStanceType GetCurrentStance() const { return CurrentStance; }

    UFUNCTION(BlueprintCallable, Category = "Stance")
    void SetStance(EStanceType NewStance);

    // Flow System
    UFUNCTION(BlueprintCallable, Category = "Stance")
    void OnAttackExecuted(bool bIsLeftAttack);
    
    UFUNCTION(BlueprintCallable, Category = "Stance")
    float GetFlowPosition() const { return FlowPosition; }
    
    UFUNCTION(BlueprintCallable, Category = "Stance")
    void ResetFlow();

    // Damage Modifiers
    UFUNCTION(BlueprintCallable, Category = "Stance")
    float GetDamageModifier(EDamageType DamageType) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Stance")
    FOnStanceChanged OnStanceChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance")
    EStanceType CurrentStance;

    // Stance bonuses
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PowerStancePhysicalBonus = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SoulStanceSoulBonus = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BalancedStanceBothBonus = 0.10f;
    
    // Flow system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flow")
    float FlowPosition = 0.5f; // 0.0 = full soul, 1.0 = full power
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
    float FlowSpeed = 0.15f; // How much the indicator moves per attack
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
    float ConsecutiveAttackMultiplier = 1.5f; // Speed multiplier for consecutive same-side attacks
    
    // Attack tracking
    TArray<bool> RecentAttacks; // Track last few attacks (true = left, false = right)
    float TimeSinceLastAttack = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flow")
    float AttackResetTime = 2.0f; // Time before attack pattern resets

private:
    void BroadcastStanceChange(EStanceType NewStance);
    void UpdateStanceFromFlow();
    bool IsAlternatingPattern() const;
};