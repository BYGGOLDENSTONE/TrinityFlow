#include "Enemy/TankEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

ATankEnemy::ATankEnemy()
{
    EnemyStatsID = "TankEnemy";
}

void ATankEnemy::SetupEnemy()
{
    // Call parent to load stats from data
    Super::SetupEnemy();
}