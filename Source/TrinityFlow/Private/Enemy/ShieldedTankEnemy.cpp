#include "Enemy/ShieldedTankEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AShieldedTankEnemy::AShieldedTankEnemy()
{
    bIsAreaDamage = false;
    EnemyStatsID = "ShieldedTankEnemy";
}

void AShieldedTankEnemy::SetupEnemy()
{
    Super::SetupEnemy();
}