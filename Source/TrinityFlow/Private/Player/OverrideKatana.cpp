#include "Player/OverrideKatana.h"
#include "Combat/AbilityComponent.h"
#include "Core/HealthComponent.h"
#include "Core/StateComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AOverrideKatana::AOverrideKatana()
{
    BasicAttackRange = 300.0f;
    BasicAttackSpeed = 1.0f;
    BasicDamageType = EDamageType::Physical;
    
    AbilityQCooldown = 5.0f; // Echoes of Data
    AbilityECooldown = 6.0f; // Code Break
    
    // Don't create AbilityComponent here - we'll get it from the owner
}

void AOverrideKatana::BeginPlay()
{
    Super::BeginPlay();
    
    // Get the ability component from the owner (player character)
    if (GetOwner())
    {
        AbilityComponent = GetOwner()->FindComponentByClass<UAbilityComponent>();
        if (!AbilityComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("OverrideKatana: Could not find AbilityComponent on owner"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OverrideKatana: Successfully found AbilityComponent on owner"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OverrideKatana: No owner found"));
    }
}

void AOverrideKatana::AbilityQ(AActor* Target)
{
    // Echoes of Data
    if (!IsAbilityQReady() || !Target)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > 4000.0f) // Echoes of Data range
    {
        return;
    }

    if (AbilityComponent)
    {
        AbilityComponent->SetEchoesTarget(Target);
        UE_LOG(LogTemp, Warning, TEXT("Echoes of Data: Marked enemy %s"), *Target->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Echoes of Data: No AbilityComponent found!"));
    }

    StartCooldown(AbilityQCooldownTimer, AbilityQCooldown);

    // Visual feedback
    DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Purple, false, 1.0f, 0, 3.0f);
}

void AOverrideKatana::AbilityE(AActor* Target)
{
    // Code Break - Enhanced slash with soul damage
    if (!IsAbilityEReady() || !Target)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > 600.0f) // Code Break range
    {
        return;
    }

    if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = OwnerHealthComponent ? OwnerHealthComponent->GetResources().AttackPoint : 20.0f;
        DamageInfo.Type = EDamageType::Soul; // Code Break deals soul damage
        DamageInfo.Instigator = OwnerPawn;

        FVector DamageDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        TargetHealth->TakeDamage(DamageInfo, DamageDirection);

        // Visual feedback
        DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Cyan, false, 0.5f, 0, 5.0f);
    }

    StartCooldown(AbilityECooldownTimer, AbilityECooldown);
}

void AOverrideKatana::DefensiveAbility()
{
    // Scripted Dodge
    // This will be called when space is pressed during combat
    if (bDodgeWindowActive)
    {
        float DamageMultiplier = 1.0f;
        if (ProcessDodge(DamageMultiplier))
        {
            // Dodge was successful
            if (DamageMultiplier == 0.0f)
            {
                // Perfect dodge
                OnPerfectDodge();
            }
        }
    }
}

void AOverrideKatana::OnPerfectDodge()
{
    // Reset Code Break cooldown on perfect dodge
    AbilityECooldownTimer = 0.0f;

    // Visual feedback
    DrawDebugSphere(GetWorld(), GetActorLocation(), 100.0f, 12, FColor::Green, false, 0.5f);
}

void AOverrideKatana::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update dodge window
    if (bDodgeWindowActive)
    {
        DodgeWindowTimer += DeltaTime;
        
        if (DodgeWindowTimer >= 1.5f)
        {
            // Attack completed, deal full damage
            bDodgeWindowActive = false;
            DodgeWindowTimer = 0.0f;
            
            if (OwnerHealthComponent && IncomingDamage > 0.0f)
            {
                // Deal the damage since dodge wasn't used
                FDamageInfo DamageInfo;
                DamageInfo.Amount = IncomingDamage;
                DamageInfo.Type = EDamageType::Physical;
                DamageInfo.Instigator = DodgeAttacker;
                
                FVector DamageDirection = DodgeAttacker ? (GetActorLocation() - DodgeAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
                OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
            }
        }
    }
}

void AOverrideKatana::StartDodgeWindow(float Damage, AActor* Attacker)
{
    bDodgeWindowActive = true;
    DodgeWindowTimer = 0.0f;
    IncomingDamage = Damage;
    DodgeAttacker = Attacker;
}

bool AOverrideKatana::ProcessDodge(float& OutDamageMultiplier)
{
    if (!bDodgeWindowActive)
    {
        OutDamageMultiplier = 1.0f;
        return false;
    }

    // Check timing window
    if (DodgeWindowTimer <= 0.75f)
    {
        // Moderate dodge (0-0.75s) - half damage
        OutDamageMultiplier = 0.5f;
    }
    else if (DodgeWindowTimer <= 1.5f)
    {
        // Perfect dodge (0.75-1.5s) - no damage
        OutDamageMultiplier = 0.0f;
    }
    else
    {
        // Too late
        OutDamageMultiplier = 1.0f;
        return false;
    }

    // Apply reduced damage
    if (OwnerHealthComponent && OutDamageMultiplier > 0.0f)
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = IncomingDamage * OutDamageMultiplier;
        DamageInfo.Type = EDamageType::Physical;
        DamageInfo.Instigator = DodgeAttacker;
        
        FVector DamageDirection = DodgeAttacker ? (GetActorLocation() - DodgeAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
        OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
    }

    bDodgeWindowActive = false;
    DodgeWindowTimer = 0.0f;
    
    return true;
}