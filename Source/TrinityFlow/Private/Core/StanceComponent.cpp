#include "Core/StanceComponent.h"
#include "Engine/World.h"

UStanceComponent::UStanceComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default to Power stance for testing
    CurrentStance = EStanceType::Power;
}

void UStanceComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize with current stance
    BroadcastStanceChange(CurrentStance);
}

void UStanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Placeholder for future stance-related updates
}

void UStanceComponent::SetStance(EStanceType NewStance)
{
    if (CurrentStance != NewStance)
    {
        CurrentStance = NewStance;
        BroadcastStanceChange(NewStance);
    }
}

void UStanceComponent::UpdateStanceFromShards(int32 SoulShards, int32 PowerShards)
{
    EStanceType NewStance = EStanceType::Balanced;
    
    // Determine stance based on shard counts
    if (PowerShards > SoulShards)
    {
        NewStance = EStanceType::Power;
    }
    else if (SoulShards > PowerShards)
    {
        NewStance = EStanceType::Soul;
    }
    else
    {
        // Equal or within ±1 difference = Balanced
        NewStance = EStanceType::Balanced;
    }
    
    SetStance(NewStance);
}

float UStanceComponent::GetDamageModifier(EDamageType DamageType) const
{
    switch (CurrentStance)
    {
        case EStanceType::Power:
            return (DamageType == EDamageType::Physical) ? (1.0f + PowerStancePhysicalBonus) : 1.0f;
            
        case EStanceType::Soul:
            return (DamageType == EDamageType::Soul) ? (1.0f + SoulStanceSoulBonus) : 1.0f;
            
        case EStanceType::Balanced:
            return 1.0f + BalancedStanceBothBonus;
            
        default:
            return 1.0f;
    }
}

void UStanceComponent::BroadcastStanceChange(EStanceType NewStance)
{
    OnStanceChanged.Broadcast(NewStance);
    
    // Log for testing
    UE_LOG(LogTemp, Log, TEXT("Stance changed to: %s"), 
        NewStance == EStanceType::Power ? TEXT("Power") :
        NewStance == EStanceType::Soul ? TEXT("Soul") : TEXT("Balanced"));
}