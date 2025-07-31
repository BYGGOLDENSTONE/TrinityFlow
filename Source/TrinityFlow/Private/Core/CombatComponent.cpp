#include "Core/CombatComponent.h"
#include "Core/HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Actor.h"
#include "../../TrinityFlowCharacter.h"

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
        
        // Draw debug line for attack casting with color-coded timing windows
        // Get head position (approximate by adding height offset)
        FVector StartLocation = GetOwner()->GetActorLocation();
        StartLocation.Z += 150.0f; // Approximate head height
        
        FVector EndLocation = CurrentTarget->GetActorLocation();
        EndLocation.Z += 50.0f; // Target center mass
        
        float Progress = CastingTimer / CastingTime;
        FVector CurrentEndLocation = FMath::Lerp(StartLocation, EndLocation, Progress);
        
        // Color based on timing window
        FColor LineColor;
        float LineThickness = 3.0f;
        
        if (CastingTimer <= 0.75f)
        {
            // Moderate window (0-0.75s) - Yellow/Orange
            LineColor = FColor::Orange;
            LineThickness = 3.0f + (CastingTimer * 2.0f); // Growing thickness
        }
        else
        {
            // Perfect window (0.75-1.5s) - Green
            LineColor = FColor::Green;
            LineThickness = 4.0f + ((CastingTimer - 0.75f) * 3.0f); // Larger growing thickness
            
            // Add extra visual feedback for perfect window
            DrawDebugSphere(GetWorld(), CurrentEndLocation, 10.0f, 8, LineColor, false, DeltaTime * 2.0f);
        }
        
        DrawDebugLine(GetWorld(), StartLocation, CurrentEndLocation, LineColor, false, DeltaTime * 2.0f, 0, LineThickness);
        
        // Add text indicator above enemy
        FVector TextLocation = StartLocation + FVector(0, 0, 30);
        FString TimingText = CastingTimer <= 0.75f ? TEXT("MODERATE") : TEXT("PERFECT!");
        DrawDebugString(GetWorld(), TextLocation, TimingText, nullptr, LineColor, DeltaTime * 2.0f);
        
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
    DamageInfo.bIsLeftWeapon = false; // Enemies use right hand by default

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
                // Check if target is player with defensive ability system
                if (ATrinityFlowCharacter* PlayerTarget = Cast<ATrinityFlowCharacter>(HitActor))
                {
                    // Notify player of incoming attack for defensive ability window
                    PlayerTarget->OnIncomingAttack(GetOwner(), DamageInfo.Amount, DamageInfo.Type);
                }
                else if (UHealthComponent* HealthComp = HitActor->FindComponentByClass<UHealthComponent>())
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
        // Check if target is player with defensive ability system
        if (ATrinityFlowCharacter* PlayerTarget = Cast<ATrinityFlowCharacter>(CurrentTarget))
        {
            // Notify player of incoming attack for defensive ability window
            PlayerTarget->OnIncomingAttack(GetOwner(), DamageInfo.Amount, DamageInfo.Type);
        }
        else if (UHealthComponent* TargetHealth = CurrentTarget->FindComponentByClass<UHealthComponent>())
        {
            // Normal damage for non-player targets
            FVector DamageDirection = (CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
            TargetHealth->TakeDamage(DamageInfo, DamageDirection);
        }
    }

    OnAttack.Broadcast();
}