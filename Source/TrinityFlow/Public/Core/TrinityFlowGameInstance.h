#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TrinityFlowGameInstance.generated.h"

/**
 * Custom Game Instance for TrinityFlow
 * Provides configuration for subsystems and global game settings
 */
UCLASS()
class TRINITYFLOW_API UTrinityFlowGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UTrinityFlowGameInstance();

    virtual void Init() override;
    virtual void Shutdown() override;

public:
    // Stats Subsystem Configuration - Made public for subsystem access
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats Configuration")
    class UDataTable* CharacterStatsTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats Configuration")
    class UDataTable* WeaponStatsTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats Configuration")
    class UTrinityFlowCharacterStats* DefaultPlayerStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats Configuration")
    class UTrinityFlowWeaponStats* DefaultKatanaStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats Configuration")
    class UTrinityFlowWeaponStats* DefaultAnchorStats;

private:
    void ConfigureStatsSubsystem();
};