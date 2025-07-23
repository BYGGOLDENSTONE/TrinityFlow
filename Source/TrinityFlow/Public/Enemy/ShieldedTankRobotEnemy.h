#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "ShieldedTankRobotEnemy.generated.h"

UCLASS()
class TRINITYFLOW_API AShieldedTankRobotEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AShieldedTankRobotEnemy();

protected:
    virtual void SetupEnemy() override;
};