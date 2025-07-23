#include "Core/CombatComponent.h"
#include "Core/HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Actor.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    
    OwnerHealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update cooldown
    if (AttackCooldownTimer > 0.0f)
    {
        AttackCooldownTimer -= DeltaTime;
    }

    // Update casting
    if (bIsCasting && CurrentTarget)
    {
        CastingTimer += DeltaTime;
        
        // Draw debug line for attack casting
        FVector StartLocation = GetOwner()->GetActorLocation();
        FVector EndLocation = CurrentTarget->GetActorLocation();
        float Progress = CastingTimer / CastingTime;
        FVector CurrentEndLocation = FMath::Lerp(StartLocation, EndLocation, Progress);
        
        DrawDebugLine(GetWorld(), StartLocation, CurrentEndLocation, FColor::Red, false, DeltaTime * 2.0f, 0, 2.0f);
        
        if (CastingTimer >= CastingTime)
        {
            ExecuteAttack();
            bIsCasting = false;
            CastingTimer = 0.0f;
            CurrentTarget = nullptr;
        }
    }
}

bool UCombatComponent::CanAttack() const
{
    return AttackCooldownTimer <= 0.0f && !bIsCasting;
}

void UCombatComponent::StartAttack(AActor* Target, float Range, EDamageType DamageType, bool bIsAreaDamage)
{
    if (!CanAttack() || !Target)
    {
        return;
    }

    float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Target->GetActorLocation());
    if (Distance > Range)
    {
        return;
    }

    CurrentTarget = Target;
    PendingDamageType = DamageType;
    bPendingAreaDamage = bIsAreaDamage;
    bIsCasting = true;
    CastingTimer = 0.0f;
    AttackCooldownTimer = AttackCooldown;
}

void UCombatComponent::SetAttackSpeed(float AttacksPerSecond)
{
    AttackCooldown = 1.0f / FMath::Max(0.1f, AttacksPerSecond);
}

void UCombatComponent::ExecuteAttack()
{
    if (!CurrentTarget || !OwnerHealthComponent)
    {
        return;
    }

    FDamageInfo DamageInfo;
    DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
    DamageInfo.Type = PendingDamageType;
    DamageInfo.Instigator = GetOwner();
    DamageInfo.bIsAreaDamage = bPendingAreaDamage;

    if (bPendingAreaDamage)
    {
        // Area damage
        TArray<FOverlapResult> OverlapResults;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetOwner());

        GetWorld()->OverlapMultiByChannel(
            OverlapResults,
            CurrentTarget->GetActorLocation(),
            FQuat::Identity,
            ECC_Pawn,
            FCollisionShape::MakeSphere(400.0f),
            QueryParams
        );

        for (const FOverlapResult& Result : OverlapResults)
        {
            if (AActor* HitActor = Result.GetActor())
            {
                if (UHealthComponent* HealthComp = HitActor->FindComponentByClass<UHealthComponent>())
                {
                    FVector DamageDirection = (HitActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
                    HealthComp->TakeDamage(DamageInfo, DamageDirection);
                }
            }
        }
    }
    else
    {
        // Single target damage
        if (UHealthComponent* TargetHealth = CurrentTarget->FindComponentByClass<UHealthComponent>())
        {
            FVector DamageDirection = (CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
            TargetHealth->TakeDamage(DamageInfo, DamageDirection);
        }
    }

    OnAttack.Broadcast();
}