#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "StandardEnemy.generated.h"

UCLASS()
class TRINITYFLOW_API AStandardEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AStandardEnemy();

protected:
    virtual void SetupEnemy() override;
};