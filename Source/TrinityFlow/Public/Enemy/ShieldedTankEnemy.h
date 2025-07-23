#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "ShieldedTankEnemy.generated.h"

UCLASS()
class TRINITYFLOW_API AShieldedTankEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AShieldedTankEnemy();

protected:
    virtual void SetupEnemy() override;
};