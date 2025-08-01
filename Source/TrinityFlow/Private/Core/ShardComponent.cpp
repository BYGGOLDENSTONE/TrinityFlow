#include "Core/ShardComponent.h"
#include "Core/StanceComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UShardComponent::UShardComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UShardComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeShardData();
}

void UShardComponent::InitializeShardData()
{
    // Initialize shard data for both types
    FShardData SoulData;
    FShardData PowerData;
    
    ShardInventory.Add(EShardType::Soul, SoulData);
    ShardInventory.Add(EShardType::Power, PowerData);
}

void UShardComponent::CollectShard(EShardType Type)
{
    if (FShardData* Data = ShardInventory.Find(Type))
    {
        Data->InactiveCount++;
        
        UE_LOG(LogTemp, Log, TEXT("Collected %s Shard. Inactive: %d, Active: %d"), 
            Type == EShardType::Soul ? TEXT("Soul") : TEXT("Power"),
            Data->InactiveCount,
            Data->ActiveCount);
        
        OnShardCollected.Broadcast(Type, Data->InactiveCount);
    }
}

bool UShardComponent::ActivateShards(EShardType Type, int32 Count)
{
    if (Count <= 0 || Count > MaxShardsPerActivation)
    {
        return false;
    }
    
    if (FShardData* Data = ShardInventory.Find(Type))
    {
        // Check if we have enough inactive shards
        if (Data->InactiveCount >= Count)
        {
            Data->InactiveCount -= Count;
            Data->ActiveCount += Count;
            
            UE_LOG(LogTemp, Log, TEXT("Activated %d %s Shards. Inactive: %d, Active: %d"), 
                Count,
                Type == EShardType::Soul ? TEXT("Soul") : TEXT("Power"),
                Data->InactiveCount,
                Data->ActiveCount);
            
            OnShardsActivated.Broadcast(Type, Count, Data->ActiveCount);
            
            // Notify stance component
            NotifyStanceComponent();
            
            return true;
        }
    }
    
    return false;
}

int32 UShardComponent::GetActiveShardCount(EShardType Type) const
{
    if (const FShardData* Data = ShardInventory.Find(Type))
    {
        return Data->ActiveCount;
    }
    return 0;
}

int32 UShardComponent::GetInactiveShardCount(EShardType Type) const
{
    if (const FShardData* Data = ShardInventory.Find(Type))
    {
        return Data->InactiveCount;
    }
    return 0;
}

int32 UShardComponent::GetTotalShardCount(EShardType Type) const
{
    if (const FShardData* Data = ShardInventory.Find(Type))
    {
        return Data->ActiveCount + Data->InactiveCount;
    }
    return 0;
}

void UShardComponent::GetShardCounts(int32& OutSoulActive, int32& OutSoulInactive, 
    int32& OutPowerActive, int32& OutPowerInactive) const
{
    OutSoulActive = GetActiveShardCount(EShardType::Soul);
    OutSoulInactive = GetInactiveShardCount(EShardType::Soul);
    OutPowerActive = GetActiveShardCount(EShardType::Power);
    OutPowerInactive = GetInactiveShardCount(EShardType::Power);
}

void UShardComponent::NotifyStanceComponent()
{
    if (AActor* Owner = GetOwner())
    {
        if (UStanceComponent* StanceComp = Owner->FindComponentByClass<UStanceComponent>())
        {
            int32 SoulShards = GetActiveShardCount(EShardType::Soul);
            int32 PowerShards = GetActiveShardCount(EShardType::Power);
            
            StanceComp->UpdateStanceFromShards(SoulShards, PowerShards);
        }
    }
    
    // Broadcast damage bonus change
    float SoulBonus, PhysicalBonus, SoulStanceBonus, PhysicalStanceBonus;
    GetDamageBonuses(SoulBonus, PhysicalBonus, SoulStanceBonus, PhysicalStanceBonus);
    OnDamageBonusChanged.Broadcast(SoulBonus, PhysicalBonus, SoulStanceBonus, PhysicalStanceBonus);
}

float UShardComponent::GetSoulDamageBonus() const
{
    // Soul shards provide soul damage bonus (3% per shard)
    int32 SoulShards = GetActiveShardCount(EShardType::Soul);
    float BaseBonus = SoulShards * DamageBonusPerShard;
    
    // Add stance bonus if in Soul stance
    if (AActor* Owner = GetOwner())
    {
        if (UStanceComponent* StanceComp = Owner->FindComponentByClass<UStanceComponent>())
        {
            if (StanceComp->GetCurrentStance() == EStanceType::Soul)
            {
                // Add the same bonus again (not double)
                return BaseBonus + BaseBonus;
            }
        }
    }
    
    return BaseBonus;
}

float UShardComponent::GetPhysicalDamageBonus() const
{
    // Power shards provide physical damage bonus (3% per shard)
    int32 PowerShards = GetActiveShardCount(EShardType::Power);
    float BaseBonus = PowerShards * DamageBonusPerShard;
    
    // Add stance bonus if in Power stance
    if (AActor* Owner = GetOwner())
    {
        if (UStanceComponent* StanceComp = Owner->FindComponentByClass<UStanceComponent>())
        {
            if (StanceComp->GetCurrentStance() == EStanceType::Power)
            {
                // Add the same bonus again (not double)
                return BaseBonus + BaseBonus;
            }
        }
    }
    
    return BaseBonus;
}

void UShardComponent::GetDamageBonuses(float& OutSoulBonus, float& OutPhysicalBonus, 
    float& OutSoulStanceBonus, float& OutPhysicalStanceBonus) const
{
    int32 SoulShards = GetActiveShardCount(EShardType::Soul);
    int32 PowerShards = GetActiveShardCount(EShardType::Power);
    
    // Base bonuses from shards (3% per shard)
    OutSoulBonus = SoulShards * DamageBonusPerShard;
    OutPhysicalBonus = PowerShards * DamageBonusPerShard;
    
    // Stance bonuses (add the same bonus again when in matching stance)
    OutSoulStanceBonus = 0.0f;
    OutPhysicalStanceBonus = 0.0f;
    
    if (AActor* Owner = GetOwner())
    {
        if (UStanceComponent* StanceComp = Owner->FindComponentByClass<UStanceComponent>())
        {
            EStanceType CurrentStance = StanceComp->GetCurrentStance();
            
            if (CurrentStance == EStanceType::Soul)
            {
                OutSoulStanceBonus = OutSoulBonus; // Add soul bonus again
            }
            else if (CurrentStance == EStanceType::Power)
            {
                OutPhysicalStanceBonus = OutPhysicalBonus; // Add physical bonus again
            }
        }
    }
}