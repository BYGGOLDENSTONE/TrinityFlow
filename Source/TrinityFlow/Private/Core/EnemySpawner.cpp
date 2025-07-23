#include "Core/EnemySpawner.h"
#include "Enemy/StandardEnemy.h"
#include "Enemy/TankEnemy.h"
#include "Enemy/ShieldedTankEnemy.h"
#include "Enemy/PhaseEnemy.h"
#include "Enemy/ShieldedTankRobotEnemy.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    
    if (bAutoSpawn)
    {
        FTimerHandle SpawnTimer;
        GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnDelay, false);
    }
}

void AEnemySpawner::SpawnEnemy()
{
    TSubclassOf<AEnemyBase> EnemyClass = GetEnemyClassForType(EnemyTypeToSpawn);
    
    if (EnemyClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, GetActorLocation(), GetActorRotation(), SpawnParams);
    }
}

TSubclassOf<AEnemyBase> AEnemySpawner::GetEnemyClassForType(EEnemyType Type)
{
    switch (Type)
    {
        case EEnemyType::Standard:
            return AStandardEnemy::StaticClass();
        case EEnemyType::Tank:
            return ATankEnemy::StaticClass();
        case EEnemyType::ShieldedTank:
            return AShieldedTankEnemy::StaticClass();
        case EEnemyType::Phase:
            return APhaseEnemy::StaticClass();
        case EEnemyType::ShieldedTankRobot:
            return AShieldedTankRobotEnemy::StaticClass();
        default:
            return nullptr;
    }
}