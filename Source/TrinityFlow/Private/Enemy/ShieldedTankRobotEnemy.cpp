#include "Enemy/ShieldedTankRobotEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AShieldedTankRobotEnemy::AShieldedTankRobotEnemy()
{
    bIsAreaDamage = false;
}

void AShieldedTankRobotEnemy::SetupEnemy()
{
    // Tags: mechanical, armored, shielded
    if (TagComponent)
    {
        TagComponent->SetTags(ECharacterTag::Mechanical | ECharacterTag::Armored | ECharacterTag::Shielded);
    }

    // Resources: health: 300, attackpoint: 30, defencepoint: 40
    if (HealthComponent)
    {
        FCharacterResources Resources(300.0f, 40.0f, 30.0f);
        HealthComponent->SetResources(Resources);
    }
}