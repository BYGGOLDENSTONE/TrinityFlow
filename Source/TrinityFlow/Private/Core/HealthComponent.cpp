#include "Core/HealthComponent.h"
#include "Core/DamageCalculator.h"
#include "Core/TagComponent.h"
#include "Core/AnimationComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    TagComponent = GetOwner()->FindComponentByClass<UTagComponent>();
}

void UHealthComponent::TakeDamage(const FDamageInfo& DamageInfo, const FVector& DamageDirection)
{
    if (!IsAlive())
    {
        return;
    }
    
    ECharacterTag Tags = TagComponent ? TagComponent->GetTags() : ECharacterTag::None;
    FVector OwnerForward = GetOwner()->GetActorForwardVector();
    
    float FinalDamage = FDamageCalculator::CalculateDamage(DamageInfo, Resources, Tags, DamageDirection, OwnerForward);
    
    if (FinalDamage > 0.0f)
    {
        float HealthBefore = Resources.Health;
        Resources.Health = FMath::Max(0.0f, Resources.Health - FinalDamage);
        float ActualDamageDealt = HealthBefore - Resources.Health;
        
        OnHealthChanged.Broadcast(Resources.Health);
        OnDamageDealt.Broadcast(GetOwner(), ActualDamageDealt, DamageInfo.Instigator, DamageInfo.Type);
        
        // Play hit response animation
        if (AActor* Owner = GetOwner())
        {
            if (UAnimationComponent* AnimComp = Owner->FindComponentByClass<UAnimationComponent>())
            {
                AnimComp->PlayHitResponse();
            }
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