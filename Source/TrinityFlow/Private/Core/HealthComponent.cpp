#include "Core/HealthComponent.h"
#include "Core/DamageCalculator.h"
#include "Core/TagComponent.h"
#include "Core/AnimationComponent.h"
#include "Enemy/EnemyAnimationComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (AActor* Owner = GetOwner())
    {
        TagComponent = Owner->FindComponentByClass<UTagComponent>();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthComponent: No owner found during BeginPlay"));
    }
}

void UHealthComponent::TakeDamage(const FDamageInfo& DamageInfo, const FVector& DamageDirection)
{
    if (!IsAlive())
    {
        return;
    }
    
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("HealthComponent: No owner found when taking damage"));
        return;
    }
    
    ECharacterTag Tags = TagComponent ? TagComponent->GetTags() : ECharacterTag::None;
    FVector OwnerForward = Owner->GetActorForwardVector();
    
    float FinalDamage = FDamageCalculator::CalculateDamage(DamageInfo, Resources, Tags, DamageDirection, OwnerForward);
    
    if (FinalDamage > 0.0f)
    {
        float HealthBefore = Resources.Health;
        Resources.Health = FMath::Max(0.0f, Resources.Health - FinalDamage);
        float ActualDamageDealt = HealthBefore - Resources.Health;
        
        OnHealthChanged.Broadcast(Resources.Health);
        OnDamageDealt.Broadcast(GetOwner(), ActualDamageDealt, DamageInfo.Instigator, DamageInfo.Type);
        
        // Play hit response animation
        // Check for player animation component
        if (UAnimationComponent* AnimComp = Owner->FindComponentByClass<UAnimationComponent>())
        {
            AnimComp->PlayHitResponse();
        }
        // Check for enemy animation component
        else if (UEnemyAnimationComponent* EnemyAnimComp = Owner->FindComponentByClass<UEnemyAnimationComponent>())
        {
            EnemyAnimComp->PlayHitResponse(DamageInfo.Type, DamageInfo.bIsLeftWeapon);
        }
        
        if (!IsAlive())
        {
            OnDeath.Broadcast();
        }
    }
}

void UHealthComponent::SetResources(const FCharacterResources& NewResources)
{
    Resources = NewResources;
    OnHealthChanged.Broadcast(Resources.Health);
}