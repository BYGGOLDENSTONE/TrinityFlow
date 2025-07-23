#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatStateManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, bool, bInCombat);

UCLASS()
class TRINITYFLOW_API UCombatStateManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION()
    void RegisterEnemy(class AEnemyBase* Enemy);

    UFUNCTION()
    void UnregisterEnemy(class AEnemyBase* Enemy);

    UFUNCTION()
    void OnEnemySeesPlayer(class AEnemyBase* Enemy);

    UFUNCTION()
    bool IsInCombat() const { return bIsInCombat; }

    UPROPERTY()
    FOnCombatStateChanged OnCombatStateChanged;

protected:
    UPROPERTY()
    TArray<class AEnemyBase*> RegisteredEnemies;

    UPROPERTY()
    TArray<class AEnemyBase*> AlertedEnemies;

    UPROPERTY()
    bool bIsInCombat = false;

    void UpdateCombatState();
};