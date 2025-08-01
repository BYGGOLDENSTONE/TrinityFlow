#include "Core/StanceComponent.h"
#include "Engine/World.h"

UStanceComponent::UStanceComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default to Balanced stance
    CurrentStance = EStanceType::Balanced;
    FlowPosition = 0.5f; // Start at center
}

void UStanceComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize with current stance
    UpdateStanceFromFlow();
    BroadcastStanceChange(CurrentStance);
}

void UStanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update attack pattern timer
    if (RecentAttacks.Num() > 0)
    {
        TimeSinceLastAttack += DeltaTime;
        
        // Reset attack pattern if too much time has passed
        if (TimeSinceLastAttack >= AttackResetTime)
        {
            RecentAttacks.Empty();
            TimeSinceLastAttack = 0.0f;
        }
    }
}

void UStanceComponent::SetStance(EStanceType NewStance)
{
    if (CurrentStance != NewStance)
    {
        CurrentStance = NewStance;
        BroadcastStanceChange(NewStance);
    }
}

void UStanceComponent::OnAttackExecuted(bool bIsLeftAttack)
{
    // Reset timer
    TimeSinceLastAttack = 0.0f;
    
    // Add to recent attacks
    RecentAttacks.Add(bIsLeftAttack);
    
    // Keep only last 4 attacks for pattern detection
    if (RecentAttacks.Num() > 4)
    {
        RecentAttacks.RemoveAt(0);
    }
    
    // Calculate flow change
    float FlowChange = FlowSpeed;
    
    // Check for consecutive attacks
    if (RecentAttacks.Num() >= 2)
    {
        int32 ConsecutiveCount = 1;
        for (int32 i = RecentAttacks.Num() - 2; i >= 0; i--)
        {
            if (RecentAttacks[i] == bIsLeftAttack)
            {
                ConsecutiveCount++;
            }
            else
            {
                break;
            }
        }
        
        // Apply multiplier for consecutive attacks (capped at 3x)
        if (ConsecutiveCount >= 2)
        {
            FlowChange *= FMath::Min(ConsecutiveCount * ConsecutiveAttackMultiplier, 3.0f);
        }
    }
    
    // Apply flow change
    if (bIsLeftAttack)
    {
        FlowPosition -= FlowChange; // Left moves toward soul (0.0)
    }
    else
    {
        FlowPosition += FlowChange; // Right moves toward power (1.0)
    }
    
    // Clamp flow position
    FlowPosition = FMath::Clamp(FlowPosition, 0.0f, 1.0f);
    
    // Update stance based on flow position
    UpdateStanceFromFlow();
    
    UE_LOG(LogTemp, Log, TEXT("Attack: %s, Flow: %.2f, Stance: %s, Pattern: %s"), 
        bIsLeftAttack ? TEXT("Left") : TEXT("Right"), 
        FlowPosition,
        CurrentStance == EStanceType::Soul ? TEXT("Soul") : 
        CurrentStance == EStanceType::Power ? TEXT("Power") : TEXT("Balanced"),
        IsAlternatingPattern() ? TEXT("Alternating") : TEXT("Not Alternating"));
}

void UStanceComponent::ResetFlow()
{
    FlowPosition = 0.5f;
    RecentAttacks.Empty();
    TimeSinceLastAttack = 0.0f;
    UpdateStanceFromFlow();
}

void UStanceComponent::UpdateStanceFromFlow()
{
    EStanceType NewStance = CurrentStance;
    
    if (FlowPosition <= 0.2f)
    {
        NewStance = EStanceType::Soul;
    }
    else if (FlowPosition >= 0.4f && FlowPosition <= 0.6f)
    {
        // Only enter balanced stance if using alternating pattern
        if (IsAlternatingPattern())
        {
            NewStance = EStanceType::Balanced;
        }
        // If not alternating, don't change stance in middle zone
    }
    else if (FlowPosition >= 0.8f)
    {
        NewStance = EStanceType::Power;
    }
    // For transition zones (0.2-0.4 and 0.6-0.8), keep current stance
    
    if (NewStance != CurrentStance)
    {
        SetStance(NewStance);
    }
}

bool UStanceComponent::IsAlternatingPattern() const
{
    if (RecentAttacks.Num() < 2)
    {
        return false;
    }
    
    // Check if recent attacks are alternating
    for (int32 i = 1; i < RecentAttacks.Num(); i++)
    {
        if (RecentAttacks[i] == RecentAttacks[i - 1])
        {
            // Found consecutive same-side attacks
            if (i == 1 || i == RecentAttacks.Num() - 1)
            {
                // Allow one consecutive at start or end
                continue;
            }
            return false;
        }
    }
    
    return true;
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