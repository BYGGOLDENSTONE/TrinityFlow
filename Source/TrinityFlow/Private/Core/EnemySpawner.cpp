#include "Core/EnemySpawner.h"
#include "Enemy/StandardEnemy.h"
#include "Enemy/TankEnemy.h"
#include "Enemy/ShieldedTankEnemy.h"
#include "Enemy/PhaseEnemy.h"
#include "Enemy/ShieldedTankRobotEnemy.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HUD/TrinityFlowHUD.h"
#include "Core/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

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
        
        AEnemyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, GetActorLocation(), GetActorRotation(), SpawnParams);
        
        // Register damage events with HUD for damage numbers
        if (SpawnedEnemy)
        {
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                if (ATrinityFlowHUD* HUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
                {
                    if (UHealthComponent* HealthComp = SpawnedEnemy->FindComponentByClass<UHealthComponent>())
                    {
                        HealthComp->OnDamageDealt.AddDynamic(HUD, &ATrinityFlowHUD::OnDamageDealt);
                    }
                }
            }
        }
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