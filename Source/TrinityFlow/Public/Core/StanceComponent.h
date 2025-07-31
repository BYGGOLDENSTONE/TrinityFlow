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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance")
    EStanceType CurrentStance;

    // Placeholder for future implementation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PowerStancePhysicalBonus = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SoulStanceSoulBonus = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BalancedStanceBothBonus = 0.10f;

private:
    void BroadcastStanceChange(EStanceType NewStance);
};