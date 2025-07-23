#include "Enemy/TankEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

ATankEnemy::ATankEnemy()
{
    bIsAreaDamage = true; // Tank enemy has area damage
}

void ATankEnemy::SetupEnemy()
{
    // Tags: havesoul, armored
    if (TagComponent)
    {
        TagComponent->SetTags(ECharacterTag::HaveSoul | ECharacterTag::Armored);
    }

    // Resources: health: 200, attackpoint: 40, defencepoint: 20
    if (HealthComponent)
    {
        FCharacterResources Resources(200.0f, 20.0f, 40.0f);
        HealthComponent->SetResources(Resources);
    }
}