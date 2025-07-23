#include "Enemy/ShieldedTankEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AShieldedTankEnemy::AShieldedTankEnemy()
{
    bIsAreaDamage = false;
}

void AShieldedTankEnemy::SetupEnemy()
{
    // Tags: havesoul, armored, shielded
    if (TagComponent)
    {
        TagComponent->SetTags(ECharacterTag::HaveSoul | ECharacterTag::Armored | ECharacterTag::Shielded);
    }

    // Resources: health: 300, attackpoint: 20, defencepoint: 20
    if (HealthComponent)
    {
        FCharacterResources Resources(300.0f, 20.0f, 20.0f);
        HealthComponent->SetResources(Resources);
    }
}