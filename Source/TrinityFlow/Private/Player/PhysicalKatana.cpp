#include "Player/PhysicalKatana.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowPhysicalKatanaStats.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

APhysicalKatana::APhysicalKatana()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Set default stats
    BasicAttackRange = 300.0f;
    BasicAttackSpeed = 1.0f;
    BasicDamageType = EDamageType::Physical;  // Physical damage for right katana
    bIsLeftHandWeapon = false;  // This is the right hand weapon
    AbilityECooldown = 6.0f;  // Placeholder cooldowns
}

void APhysicalKatana::BeginPlay()
{
    Super::BeginPlay();
    
    // Don't try to load stats in editor/simulate mode
    if (!GetWorld() || !GetWorld()->IsGameWorld())
    {
        return;
    }
    
    // Load weapon stats from subsystem
    if (GetGameInstance())
    {
        if (UTrinityFlowStatsSubsystem* StatsSubsystem = GetGameInstance()->GetSubsystem<UTrinityFlowStatsSubsystem>())
    {
        WeaponStats = StatsSubsystem->GetPhysicalKatanaStats();
        
        if (WeaponStats)
        {
            // Apply basic stats
            BasicAttackRange = WeaponStats->BasicAttackRange;
            BasicAttackSpeed = WeaponStats->BasicAttackSpeed;
            BasicDamageType = WeaponStats->BasicDamageType;
            AbilityECooldown = WeaponStats->AbilityECooldown;
            BasicAttackDamageDelay = WeaponStats->PhysicalKatanaStats.BasicAttackDamageDelay;
            
            // Cache ability stats
            PhysicalKatanaStats = &WeaponStats->PhysicalKatanaStats;
            
            UE_LOG(LogTemp, Log, TEXT("PhysicalKatana loaded stats - Range: %.1f, Speed: %.1f, OrderWindow: %.1f"), 
                BasicAttackRange, BasicAttackSpeed, PhysicalKatanaStats->OrderWindowDuration);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PhysicalKatana: No weapon stats found in subsystem"));
        }
        }
    }
}

void APhysicalKatana::BasicAttack(AActor* Target)
{
    Super::BasicAttack(Target);
    // Basic attack implementation handled by parent class with delay system
}

void APhysicalKatana::AbilityE(AActor* Target)
{
    // Placeholder for first ability
    if (!IsAbilityEReady())
    {
        UE_LOG(LogTemp, Warning, TEXT("PhysicalKatana Ability E on cooldown: %.1f seconds remaining"), GetAbilityECooldownRemaining());
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("PhysicalKatana Ability E - Not yet implemented"));
    StartCooldown(AbilityECooldownTimer, AbilityECooldown);
}

void APhysicalKatana::AbilityR(AActor* Target)
{
    // Placeholder for second ability
    UE_LOG(LogTemp, Log, TEXT("PhysicalKatana Ability R - Not yet implemented"));
}

void APhysicalKatana::DefensiveAbility()
{
    // Order defensive ability
    // This is called by the character when Space is pressed
    if (!bOrderWindowActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Order: No incoming attack to defend against"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Order: Defensive ability activated, window timer: %.2f"), OrderWindowTimer);
    }
}

void APhysicalKatana::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Handle Order window timing
    if (bOrderWindowActive)
    {
        OrderWindowTimer += DeltaTime;
        
        // Check if stats are loaded
        if (PhysicalKatanaStats)
        {
            // Visual feedback for timing window
            if (OrderWindowTimer <= PhysicalKatanaStats->PerfectOrderStart)
            {
                // Moderate window - orange
                DrawDebugSphere(GetWorld(), GetActorLocation(), 80.0f, 12, FColor::Orange, false, 0.1f);
            }
            else if (OrderWindowTimer <= PhysicalKatanaStats->OrderWindowDuration)
            {
                // Perfect window - green
                DrawDebugSphere(GetWorld(), GetActorLocation(), 100.0f, 12, FColor::Green, false, 0.1f);
            }
            else
            {
                // Window expired
                bOrderWindowActive = false;
                OrderWindowTimer = 0.0f;
            }
        }
        else
        {
            // Use default values if stats not loaded
            const float DefaultPerfectOrderStart = 0.75f;
            const float DefaultOrderWindowDuration = 1.5f;
            
            if (OrderWindowTimer <= DefaultPerfectOrderStart)
            {
                DrawDebugSphere(GetWorld(), GetActorLocation(), 80.0f, 12, FColor::Orange, false, 0.1f);
            }
            else if (OrderWindowTimer <= DefaultOrderWindowDuration)
            {
                DrawDebugSphere(GetWorld(), GetActorLocation(), 100.0f, 12, FColor::Green, false, 0.1f);
            }
            else
            {
                bOrderWindowActive = false;
                OrderWindowTimer = 0.0f;
            }
        }
    }
}

void APhysicalKatana::StartOrderWindow(float Damage, AActor* Attacker)
{
    bOrderWindowActive = true;
    OrderWindowTimer = 0.0f;
    IncomingDamage = Damage;
    OrderAttacker = Attacker;
}

bool APhysicalKatana::ProcessOrder()
{
    if (!bOrderWindowActive)
    {
        return false;
    }
    
    bool bPerfectTiming = false;
    float DamageMultiplier = 1.0f;
    
    // Use default values if stats not loaded
    const float DefaultPerfectOrderStart = 0.75f;
    const float DefaultOrderWindowDuration = 1.5f;
    const float DefaultModerateOrderDamageMultiplier = 0.5f;
    
    float PerfectOrderStart = PhysicalKatanaStats ? PhysicalKatanaStats->PerfectOrderStart : DefaultPerfectOrderStart;
    float OrderWindowDuration = PhysicalKatanaStats ? PhysicalKatanaStats->OrderWindowDuration : DefaultOrderWindowDuration;
    float ModerateOrderDamageMultiplier = PhysicalKatanaStats ? PhysicalKatanaStats->ModerateOrderDamageMultiplier : DefaultModerateOrderDamageMultiplier;
    
    if (OrderWindowTimer <= PerfectOrderStart)
    {
        // Moderate timing - half damage
        DamageMultiplier = ModerateOrderDamageMultiplier;
    }
    else if (OrderWindowTimer <= OrderWindowDuration)
    {
        // Perfect timing - no damage + counter
        DamageMultiplier = 0.0f;
        bPerfectTiming = true;
        
        if (OrderAttacker)
        {
            PerformCounterAttack(OrderAttacker);
        }
    }
    
    // Reset window
    bOrderWindowActive = false;
    OrderWindowTimer = 0.0f;
    
    // Apply damage if any
    if (DamageMultiplier > 0.0f && OwnerHealthComponent)
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = IncomingDamage * DamageMultiplier;
        DamageInfo.Type = EDamageType::Physical;
        DamageInfo.Instigator = OrderAttacker;
        
        FVector DamageDirection = OrderAttacker ? (GetActorLocation() - OrderAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
        OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
    }
    
    return bPerfectTiming;
}

void APhysicalKatana::PerformCounterAttack(AActor* Target)
{
    if (!Target) 
    {
        UE_LOG(LogTemp, Warning, TEXT("PerformCounterAttack: No target"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Order Counter Attack on %s"), *Target->GetName());
    
    // Visual feedback
    DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Yellow, false, 1.0f, 0, 3.0f);
    
    // Use default armor reduction if stats not loaded
    const float DefaultCounterArmorReduction = 0.25f;
    float ArmorReduction = PhysicalKatanaStats ? PhysicalKatanaStats->CounterArmorReduction : DefaultCounterArmorReduction;
    
    if (UTagComponent* TargetTags = Target->FindComponentByClass<UTagComponent>())
    {
        if (TargetTags->HasTag(ECharacterTag::Shielded))
        {
            // Strip shield
            TargetTags->RemoveTag(ECharacterTag::Shielded);
            UE_LOG(LogTemp, Log, TEXT("Counter stripped shield from %s"), *Target->GetName());
        }
        else
        {
            // Reduce armor by percentage
            if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
            {
                FCharacterResources Resources = TargetHealth->GetResources();
                float CurrentDefense = Resources.DefencePoint;
                float NewDefense = CurrentDefense * (1.0f - ArmorReduction);
                Resources.DefencePoint = NewDefense;
                TargetHealth->SetResources(Resources);
                UE_LOG(LogTemp, Log, TEXT("Counter reduced %s's armor from %.1f to %.1f"), 
                    *Target->GetName(), CurrentDefense, NewDefense);
            }
        }
    }
}

void APhysicalKatana::OnPerfectOrder()
{
    // Called when perfect Order timing is achieved
    // Could add special effects or bonuses here
    UE_LOG(LogTemp, Log, TEXT("Perfect Order executed!"));
}