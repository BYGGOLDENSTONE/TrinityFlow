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
    // Keep this for compatibility but it won't be used
}

void UAbilityComponent::OnActualDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType)
{
    ProcessEchoesDamageActual(DamagedActor, ActualDamage, DamageInstigator);
}

void UAbilityComponent::ProcessEchoesDamageActual(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator)
{
    UE_LOG(LogTemp, Warning, TEXT("ProcessEchoesDamageActual: DamagedActor=%s, Damage=%.1f, MarkedEnemy=%s"), 
        DamagedActor ? *DamagedActor->GetName() : TEXT("NULL"),
        ActualDamage,
        EchoesData.MarkedEnemy ? *EchoesData.MarkedEnemy->GetName() : TEXT("NULL"));
    
    if (!EchoesData.MarkedEnemy || EchoesData.MarkedEnemy == DamagedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Echo skipped: No marked enemy or damaged actor is the marked enemy"));
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

        // Calculate echo damage based on actual health lost (75% for single target)
        // For now, we'll assume it's single target. In a full implementation,
        // we'd track whether this was from an AoE attack
        float EchoMultiplier = 0.75f;
        float EchoDamageAmount = ActualDamage * EchoMultiplier;
        
        // Soul damage gets multiplied by 2 in the damage calculator, so we need to divide by 2
        // to get the desired final damage amount
        float AdjustedEchoDamage = EchoDamageAmount / 2.0f;
        
        UE_LOG(LogTemp, Warning, TEXT("Echo calculation: ActualDamage=%.1f, 75%%=%.1f, Adjusted for soul=%.1f"), 
            ActualDamage, EchoDamageAmount, AdjustedEchoDamage);

        // Echo damage is always Soul type
        FDamageInfo EchoDamage;
        EchoDamage.Amount = AdjustedEchoDamage;
        EchoDamage.Type = EDamageType::Soul;
        EchoDamage.Instigator = DamageInstigator;

        FVector DamageDirection = (EchoesData.MarkedEnemy->GetActorLocation() - DamagedActor->GetActorLocation()).GetSafeNormal();
        MarkedHealth->TakeDamage(EchoDamage, DamageDirection);

#if !UE_BUILD_SHIPPING
        // Visual feedback
        DrawDebugLine(GetWorld(), DamagedActor->GetActorLocation(), EchoesData.MarkedEnemy->GetActorLocation(), 
                     FColor::Purple, false, 0.5f, 0, 1.0f);
#endif
    }
}