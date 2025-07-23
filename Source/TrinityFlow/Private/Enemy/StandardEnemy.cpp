#include "Enemy/StandardEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AStandardEnemy::AStandardEnemy()
{
    bIsAreaDamage = false;
}

void AStandardEnemy::SetupEnemy()
{
    // Tags: havesoul
    if (TagComponent)
    {
        TagComponent->SetTags(ECharacterTag::HaveSoul);
    }

    // Resources: health: 100, attackpoint: 10, defencepoint: 0
    if (HealthComponent)
    {
        FCharacterResources Resources(100.0f, 0.0f, 10.0f);
        HealthComponent->SetResources(Resources);
    }
}