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

    // Shard Tracking (placeholder for now)
    UFUNCTION(BlueprintCallable, Category = "Shards")
    void UpdateStanceFromShards(int32 SoulShards, int32 PowerShards);

    // Damage Modifiers
    UFUNCTION(BlueprintCallable, Category = "Stance")
    float GetDamageModifier(EDamageType DamageType) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Stance")
    FOnStanceChanged OnStanceChanged;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stance")
    EStanceType CurrentStance;

    // Placeholder for future implementation
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stance")
    float PowerStancePhysicalBonus = 0.15f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stance")
    float SoulStanceSoulBonus = 0.15f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stance")
    float BalancedStanceBothBonus = 0.10f;

private:
    void BroadcastStanceChange(EStanceType NewStance);
};