#include "Combat/AbilityComponent.h"
#include "Core/HealthComponent.h"
#include "Core/StateComponent.h"
#include "DrawDebugHelpers.h"

UAbilityComponent::UAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update Echoes of Data timer
    if (EchoesData.MarkedEnemy && EchoesData.RemainingTime > 0.0f)
    {
        EchoesData.RemainingTime -= DeltaTime;
        
        if (EchoesData.RemainingTime <= 0.0f)
        {
            // Remove marked state
            if (UStateComponent* StateComp = EchoesData.MarkedEnemy->FindComponentByClass<UStateComponent>())
            {
                StateComp->RemoveState(ECharacterState::Marked);
            }
            EchoesData.MarkedEnemy = nullptr;
        }
    }
}

void UAbilityComponent::SetEchoesTarget(AActor* Target)
{
    // Clear previous target
    if (EchoesData.MarkedEnemy)
    {
        if (UStateComponent* StateComp = EchoesData.MarkedEnemy->FindComponentByClass<UStateComponent>())
        {
            StateComp->RemoveState(ECharacterState::Marked);
        }
    }

    EchoesData.MarkedEnemy = Target;
    EchoesData.RemainingTime = 5.0f;

    if (Target)
    {
        if (UStateComponent* StateComp = Target->FindComponentByClass<UStateComponent>())
        {
            StateComp->SetMarked(5.0f);
        }
    }
}

void UAbilityComponent::OnDamageDealt(AActor* DamagedActor, const FDamageInfo& DamageInfo)
{
    ProcessEchoesDamage(DamagedActor, DamageInfo);
}

void UAbilityComponent::ProcessEchoesDamage(AActor* DamagedActor, const FDamageInfo& DamageInfo)
{
    if (!EchoesData.MarkedEnemy || EchoesData.MarkedEnemy == DamagedActor)
    {
        return;
    }

    // Check if marked enemy is still alive
    if (UHealthComponent* MarkedHealth = EchoesData.MarkedEnemy->FindComponentByClass<UHealthComponent>())
    {
        if (!MarkedHealth->IsAlive())
        {
            EchoesData.MarkedEnemy = nullptr;
            return;
        }

        // Calculate echo damage (75% for single, 30% for AoE)
        float EchoMultiplier = DamageInfo.bIsAreaDamage ? 0.3f : 0.75f;
        float EchoDamageAmount = DamageInfo.Amount * EchoMultiplier;

        // Echo damage is always Soul type
        FDamageInfo EchoDamage;
        EchoDamage.Amount = EchoDamageAmount;
        EchoDamage.Type = EDamageType::Soul;
        EchoDamage.Instigator = DamageInfo.Instigator;

        FVector DamageDirection = (EchoesData.MarkedEnemy->GetActorLocation() - DamagedActor->GetActorLocation()).GetSafeNormal();
        MarkedHealth->TakeDamage(EchoDamage, DamageDirection);

        // Visual feedback
        DrawDebugLine(GetWorld(), DamagedActor->GetActorLocation(), EchoesData.MarkedEnemy->GetActorLocation(), 
                     FColor::Purple, false, 0.5f, 0, 1.0f);
    }
}