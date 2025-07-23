#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
    Standard,
    Tank,
    ShieldedTank,
    Phase,
    ShieldedTankRobot
};

UCLASS()
class TRINITYFLOW_API AEnemySpawner : public AActor
{
    GENERATED_BODY()
    
public:    
    AEnemySpawner();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    EEnemyType EnemyTypeToSpawn = EEnemyType::Standard;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    bool bAutoSpawn = true;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    float SpawnDelay = 2.0f;

    UFUNCTION()
    void SpawnEnemy();

protected:
    TSubclassOf<class AEnemyBase> GetEnemyClassForType(EEnemyType Type);
};