#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "ShardComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShardCollected, EShardType, ShardType, int32, NewInactiveCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShardsActivated, EShardType, ShardType, int32, ActivatedCount, int32, NewActiveCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamageBonusChanged, float, SoulBonus, float, PhysicalBonus, float, SoulStanceBonus, float, PhysicalStanceBonus);

USTRUCT(BlueprintType)
struct FShardData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Shards")
    int32 ActiveCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Shards")
    int32 InactiveCount = 0;

    FShardData() {}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UShardComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShardComponent();

    virtual void BeginPlay() override;

    // Shard Collection
    UFUNCTION(BlueprintCallable, Category = "Shards")
    void CollectShard(EShardType Type);

    // Shard Activation
    UFUNCTION(BlueprintCallable, Category = "Shards")
    bool ActivateShards(EShardType Type, int32 Count);

    // Getters
    UFUNCTION(BlueprintCallable, Category = "Shards")
    int32 GetActiveShardCount(EShardType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Shards")
    int32 GetInactiveShardCount(EShardType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Shards")
    int32 GetTotalShardCount(EShardType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Shards")
    void GetShardCounts(int32& OutSoulActive, int32& OutSoulInactive, int32& OutPowerActive, int32& OutPowerInactive) const;
    
    // Damage Bonus Calculations
    UFUNCTION(BlueprintCallable, Category = "Shards|Damage")
    float GetSoulDamageBonus() const;
    
    UFUNCTION(BlueprintCallable, Category = "Shards|Damage")
    float GetPhysicalDamageBonus() const;
    
    UFUNCTION(BlueprintCallable, Category = "Shards|Damage")
    void GetDamageBonuses(float& OutSoulBonus, float& OutPhysicalBonus, float& OutSoulStanceBonus, float& OutPhysicalStanceBonus) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Shards")
    FOnShardCollected OnShardCollected;

    UPROPERTY(BlueprintAssignable, Category = "Shards")
    FOnShardsActivated OnShardsActivated;
    
    UPROPERTY(BlueprintAssignable, Category = "Shards")
    FOnDamageBonusChanged OnDamageBonusChanged;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shards")
    TMap<EShardType, FShardData> ShardInventory;

    // Maximum shards that can be activated at once at an altar
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shards")
    int32 MaxShardsPerActivation = 5;
    
    // Damage bonus per shard (3%)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shards")
    float DamageBonusPerShard = 0.03f;

private:
    void InitializeShardData();
    void NotifyStanceComponent();
};