#include "Combat/WeaponBase.h"
#include "Core/HealthComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
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

    if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
        DamageInfo.Type = BasicDamageType;
        DamageInfo.Instigator = OwnerPawn;

        FVector DamageDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        TargetHealth->TakeDamage(DamageInfo, DamageDirection);

        // Draw debug for basic attack
        DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Yellow, false, 0.5f, 0, 2.0f);
    }
}

void AWeaponBase::StartCooldown(float& Timer, float Cooldown)
{
    Timer = Cooldown;
}