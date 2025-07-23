#include "Enemy/PhaseEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

APhaseEnemy::APhaseEnemy()
{
    bIsAreaDamage = false;
}

void APhaseEnemy::SetupEnemy()
{
    // Tags: ghost
    if (TagComponent)
    {
        TagComponent->SetTags(ECharacterTag::Ghost);
    }

    // Resources: health: 100, attackpoint: 20, defencepoint: 0
    if (HealthComponent)
    {
        FCharacterResources Resources(100.0f, 0.0f, 20.0f);
        HealthComponent->SetResources(Resources);
    }
}