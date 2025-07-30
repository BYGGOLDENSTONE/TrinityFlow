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
}