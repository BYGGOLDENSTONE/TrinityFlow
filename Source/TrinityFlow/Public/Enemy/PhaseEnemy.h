#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "PhaseEnemy.generated.h"

UCLASS()
class TRINITYFLOW_API APhaseEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    APhaseEnemy();

protected:
    virtual void SetupEnemy() override;
};