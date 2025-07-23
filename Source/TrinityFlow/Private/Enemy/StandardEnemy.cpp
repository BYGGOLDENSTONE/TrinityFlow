#include "Enemy/StandardEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

AStandardEnemy::AStandardEnemy()
{
    EnemyStatsID = "StandardEnemy";
}

void AStandardEnemy::SetupEnemy()
{
    // Call parent to load stats from data
    Super::SetupEnemy();
}