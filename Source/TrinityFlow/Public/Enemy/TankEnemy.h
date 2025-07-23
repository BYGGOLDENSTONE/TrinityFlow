#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "TankEnemy.generated.h"

UCLASS()
class TRINITYFLOW_API ATankEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    ATankEnemy();

protected:
    virtual void SetupEnemy() override;
};