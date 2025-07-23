#include "Player/DivineAnchor.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

ADivineAnchor::ADivineAnchor()
{
    BasicAttackRange = 400.0f; // Area damage range
    BasicAttackSpeed = 0.5f; // 0.5 attacks per second
    BasicDamageType = EDamageType::Physical;
    
    AbilityQCooldown = 6.0f; // Gravity Pull
    AbilityECooldown = 10.0f; // Holy Gravity
}

void ADivineAnchor::BasicAttack(AActor* Target)
{
    // Smash - Area damage with knockback
    if (!OwnerHealthComponent)
    {
        return;
    }

    // Find all enemies in range
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(this);

    GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(BasicAttackRange),
        QueryParams
    );

    for (const FOverlapResult& Result : OverlapResults)
    {
        if (AActor* HitActor = Result.GetActor())
        {
            if (UHealthComponent* TargetHealth = HitActor->FindComponentByClass<UHealthComponent>())
            {
                // Deal damage
                FDamageInfo DamageInfo;
                DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
                DamageInfo.Type = BasicDamageType;
                DamageInfo.Instigator = OwnerPawn;
                DamageInfo.bIsAreaDamage = true;

                FVector DamageDirection = (HitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                TargetHealth->TakeDamage(DamageInfo, DamageDirection);

                // Apply knockback
                FVector KnockbackDirection = DamageDirection;
                KnockbackDirection.Z = 0.3f; // Add slight upward component
                KnockbackDirection.Normalize();
                
                HitActor->SetActorLocation(HitActor->GetActorLocation() + KnockbackDirection * 100.0f);

                // Visual feedback
                DrawDebugLine(GetWorld(), GetActorLocation(), HitActor->GetActorLocation(), FColor::Orange, false, 0.5f, 0, 3.0f);
            }
        }
    }

    // Area effect visual
    DrawDebugSphere(GetWorld(), GetActorLocation(), BasicAttackRange, 16, FColor::Orange, false, 0.5f);
}

void ADivineAnchor::AbilityQ(AActor* Target)
{
    // Gravity Pull
    if (!IsAbilityQReady() || !Target || !OwnerHealthComponent)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > 1000.0f) // Gravity Pull range
    {
        return;
    }

    // Pull enemy to player
    FVector PullDirection = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
    FVector PullDestination = GetActorLocation() + (-PullDirection * 150.0f); // Stop 150 units away
    PullDestination.Z = Target->GetActorLocation().Z + 200.0f; // Pull up in the air

    Target->SetActorLocation(PullDestination);

    // Make enemy vulnerable while in air
    if (UStateComponent* StateComp = Target->FindComponentByClass<UStateComponent>())
    {
        StateComp->SetVulnerable(1.0f); // Vulnerable for 1 second while falling
    }

    // Deal damage
    if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
        DamageInfo.Type = EDamageType::Physical;
        DamageInfo.Instigator = OwnerPawn;

        TargetHealth->TakeDamage(DamageInfo, PullDirection);
    }

    StartCooldown(AbilityQCooldownTimer, AbilityQCooldown);

    // Visual feedback
    DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Blue, false, 1.0f, 0, 5.0f);
    DrawDebugSphere(GetWorld(), PullDestination, 50.0f, 12, FColor::Blue, false, 1.0f);
}

void ADivineAnchor::AbilityE(AActor* Target)
{
    // Holy Gravity - Damage all enemies in range and make them vulnerable
    if (!IsAbilityEReady() || !OwnerHealthComponent)
    {
        return;
    }

    // Find all enemies in range
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(this);

    GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(1000.0f), // Holy Gravity range
        QueryParams
    );

    for (const FOverlapResult& Result : OverlapResults)
    {
        if (AActor* HitActor = Result.GetActor())
        {
            // Deal damage
            if (UHealthComponent* TargetHealth = HitActor->FindComponentByClass<UHealthComponent>())
            {
                FDamageInfo DamageInfo;
                DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
                DamageInfo.Type = EDamageType::Physical;
                DamageInfo.Instigator = OwnerPawn;
                DamageInfo.bIsAreaDamage = true;

                FVector DamageDirection = (HitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                TargetHealth->TakeDamage(DamageInfo, DamageDirection);
            }

            // Make vulnerable
            if (UStateComponent* StateComp = HitActor->FindComponentByClass<UStateComponent>())
            {
                StateComp->SetVulnerable(2.0f); // Vulnerable for 2 seconds
            }
        }
    }

    StartCooldown(AbilityECooldownTimer, AbilityECooldown);

    // Visual feedback
    DrawDebugSphere(GetWorld(), GetActorLocation(), 1000.0f, 32, FColor::Yellow, false, 1.0f);
}

void ADivineAnchor::DefensiveAbility()
{
    // Order
    if (bOrderWindowActive)
    {
        if (ProcessOrder())
        {
            // Order was successful
        }
    }
}

void ADivineAnchor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update order window
    if (bOrderWindowActive)
    {
        OrderWindowTimer += DeltaTime;
        
        if (OrderWindowTimer >= 1.5f)
        {
            // Attack completed, deal full damage
            bOrderWindowActive = false;
            OrderWindowTimer = 0.0f;
            
            if (OwnerHealthComponent && IncomingDamage > 0.0f)
            {
                // Deal the damage since order wasn't used
                FDamageInfo DamageInfo;
                DamageInfo.Amount = IncomingDamage;
                DamageInfo.Type = EDamageType::Physical;
                DamageInfo.Instigator = OrderAttacker;
                
                FVector DamageDirection = OrderAttacker ? (GetActorLocation() - OrderAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
                OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
            }
        }
    }
}

void ADivineAnchor::StartOrderWindow(float Damage, AActor* Attacker)
{
    bOrderWindowActive = true;
    OrderWindowTimer = 0.0f;
    IncomingDamage = Damage;
    OrderAttacker = Attacker;
}

bool ADivineAnchor::ProcessOrder()
{
    if (!bOrderWindowActive)
    {
        return false;
    }

    bool bPerfectOrder = false;
    
    // Check timing window
    if (OrderWindowTimer <= 0.75f)
    {
        // Moderate order - half damage
        if (OwnerHealthComponent)
        {
            FDamageInfo DamageInfo;
            DamageInfo.Amount = IncomingDamage * 0.5f;
            DamageInfo.Type = EDamageType::Physical;
            DamageInfo.Instigator = OrderAttacker;
            
            FVector DamageDirection = OrderAttacker ? (GetActorLocation() - OrderAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
            OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
        }
    }
    else if (OrderWindowTimer <= 1.5f)
    {
        // Perfect order - no damage and counter attack
        bPerfectOrder = true;
        if (OrderAttacker)
        {
            PerformCounterAttack(OrderAttacker);
        }
    }
    else
    {
        // Too late
        return false;
    }

    bOrderWindowActive = false;
    OrderWindowTimer = 0.0f;
    
    return true;
}

void ADivineAnchor::PerformCounterAttack(AActor* Target)
{
    if (!Target)
    {
        return;
    }

    if (UTagComponent* TagComp = Target->FindComponentByClass<UTagComponent>())
    {
        if (TagComp->HasTag(ECharacterTag::Shielded))
        {
            // Strip shield
            TagComp->RemoveTag(ECharacterTag::Shielded);
            
            // Visual feedback
            DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 100.0f, 12, FColor::Red, false, 1.0f);
        }
        else if (TagComp->HasTag(ECharacterTag::Armored))
        {
            // Reduce armor by 25%
            if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>())
            {
                FCharacterResources Resources = HealthComp->GetResources();
                Resources.DefencePoint *= 0.75f;
                HealthComp->SetResources(Resources);
                
                // Visual feedback
                DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 80.0f, 12, FColor::Orange, false, 1.0f);
            }
        }
    }
}