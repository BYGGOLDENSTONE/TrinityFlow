#include "Enemy/ShieldedTankRobotEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AShieldedTankRobotEnemy::AShieldedTankRobotEnemy()
{
    bIsAreaDamage = false;
    EnemyStatsID = "ShieldedTankRobotEnemy";
}

void AShieldedTankRobotEnemy::SetupEnemy()
{
    Super::SetupEnemy();
}