#include "Enemy/PhaseEnemy.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"

APhaseEnemy::APhaseEnemy()
{
    bIsAreaDamage = false;
    EnemyStatsID = "PhaseEnemy";
}

void APhaseEnemy::SetupEnemy()
{
    Super::SetupEnemy();
}