#include "Combat/WeaponBase.h"
#include "Core/HealthComponent.h"
#include "Core/ShardComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(Root);
    
    // Disable all collision to prevent glitches
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    WeaponMesh->SetGenerateOverlapEvents(false);
    WeaponMesh->CanCharacterStepUpOn = ECB_No;
    WeaponMesh->SetCanEverAffectNavigation(false);
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    
    OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        OwnerHealthComponent = OwnerPawn->FindComponentByClass<UHealthComponent>();
    }
}

void AWeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update cooldowns
    if (AbilityQCooldownTimer > 0.0f)
    {
        AbilityQCooldownTimer -= DeltaTime;
    }

    if (AbilityECooldownTimer > 0.0f)
    {
        AbilityECooldownTimer -= DeltaTime;
    }
}

void AWeaponBase::BasicAttack(AActor* Target)
{
    if (!Target || !OwnerHealthComponent)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > BasicAttackRange)
    {
        return;
    }

    // Store the target for delayed damage
    PendingAttackTarget = Target;

    // Don't clear timer if one is already active - this prevents interrupting animations
    if (!GetWorld()->GetTimerManager().IsTimerActive(AttackTimerHandle))
    {
        // Set timer to execute the actual attack after delay
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, FName("ExecuteBasicAttack"), Target);
        GetWorld()->GetTimerManager().SetTimer(
            AttackTimerHandle,
            TimerDelegate,
            BasicAttackDamageDelay,
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BasicAttack: Attack timer already active, not resetting"));
    }

    // Draw debug for attack wind-up
#if !UE_BUILD_SHIPPING
    DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Orange, false, BasicAttackDamageDelay, 0, 2.0f);
#endif
}

void AWeaponBase::ExecuteBasicAttack(AActor* Target)
{
    // Use the pending target if no target is provided
    if (!Target && PendingAttackTarget)
    {
        Target = PendingAttackTarget;
    }

    if (!Target || !OwnerHealthComponent)
    {
        PendingAttackTarget = nullptr;
        return;
    }

    // Check if target is still valid and in range
    if (!IsValid(Target))
    {
        PendingAttackTarget = nullptr;
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > BasicAttackRange * 1.5f) // Give a bit more leeway for moving targets
    {
        PendingAttackTarget = nullptr;
        return;
    }

    if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
        
        // Apply shard damage bonuses
        if (OwnerPawn)
        {
            if (UShardComponent* ShardComp = OwnerPawn->FindComponentByClass<UShardComponent>())
            {
                float DamageMultiplier = 1.0f;
                
                if (BasicDamageType == EDamageType::Soul)
                {
                    DamageMultiplier += ShardComp->GetSoulDamageBonus();
                }
                else if (BasicDamageType == EDamageType::Physical)
                {
                    DamageMultiplier += ShardComp->GetPhysicalDamageBonus();
                }
                
                DamageInfo.Amount *= DamageMultiplier;
                
                UE_LOG(LogTemp, Warning, TEXT("Applied shard damage bonus: %.1f%% (Multiplier: %.2f)"), 
                    (DamageMultiplier - 1.0f) * 100.0f, DamageMultiplier);
            }
        }
        
        DamageInfo.Type = BasicDamageType;
        DamageInfo.Instigator = OwnerPawn;
        DamageInfo.bIsLeftWeapon = bIsLeftHandWeapon;
        
        UE_LOG(LogTemp, Warning, TEXT("WeaponBase ExecuteBasicAttack: Instigator=%s, Target=%s, Damage=%.1f, IsLeftWeapon=%s"), 
            OwnerPawn ? *OwnerPawn->GetName() : TEXT("NULL"),
            Target ? *Target->GetName() : TEXT("NULL"),
            DamageInfo.Amount,
            bIsLeftHandWeapon ? TEXT("Yes") : TEXT("No"));

        FVector DamageDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        TargetHealth->TakeDamage(DamageInfo, DamageDirection);

        // Draw debug for successful hit
#if !UE_BUILD_SHIPPING
        DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 0.5f, 0, 3.0f);
#endif
    }

    // Clear pending target
    PendingAttackTarget = nullptr;
}

void AWeaponBase::StartCooldown(float& Timer, float Cooldown)
{
    Timer = Cooldown;
}

void AWeaponBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clear all active timers
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AttackTimerHandle);
    }
    
    Super::EndPlay(EndPlayReason);
}

void AWeaponBase::BeginDestroy()
{
    // Additional cleanup in case EndPlay wasn't called
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AttackTimerHandle);
    }
    
    Super::BeginDestroy();
}