#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "TrinityFlowStatsSubsystem.generated.h"

class UTrinityFlowCharacterStats;
class UTrinityFlowWeaponStatsBase;
class UTrinityFlowKatanaStats;
class UTrinityFlowPhysicalKatanaStats;

/**
 * Row structure for character stats data table
 */
USTRUCT(BlueprintType)
struct FCharacterStatsTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTrinityFlowCharacterStats> StatsAsset;
};

/**
 * Row structure for weapon stats data table
 */
USTRUCT(BlueprintType)
struct FWeaponStatsTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTrinityFlowWeaponStatsBase> StatsAsset;
};

/**
 * Centralized stats management subsystem
 * Provides easy access to all character and weapon stats from a single location
 */
UCLASS()
class TRINITYFLOW_API UTrinityFlowStatsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem implementation
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Configuration from Game Instance
    void ConfigureFromGameInstance(class UTrinityFlowGameInstance* GameInstance);

    // Character Stats Access
    UFUNCTION(BlueprintCallable, Category = "Stats")
    UTrinityFlowCharacterStats* GetCharacterStats(FName CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    UTrinityFlowCharacterStats* GetPlayerStats() const;

    // Weapon Stats Access
    UFUNCTION(BlueprintCallable, Category = "Stats")
    UTrinityFlowWeaponStatsBase* GetWeaponStats(FName WeaponID) const;

    // Type-safe weapon stats getters
    UFUNCTION(BlueprintCallable, Category = "Stats")
    UTrinityFlowKatanaStats* GetLeftKatanaStats() const;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    UTrinityFlowPhysicalKatanaStats* GetPhysicalKatanaStats() const;

    // Reload stats (useful for runtime tweaking)
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Stats|Debug")
    void ReloadAllStats();

protected:
    // Data Tables containing references to stat assets
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSoftObjectPtr<UDataTable> CharacterStatsTable;

    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSoftObjectPtr<UDataTable> WeaponStatsTable;

    // Default assets for quick access
    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSoftObjectPtr<UTrinityFlowCharacterStats> DefaultPlayerStats;

    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSoftObjectPtr<UTrinityFlowKatanaStats> DefaultLeftKatanaStats;

    UPROPERTY(EditDefaultsOnly, Category = "Configuration")
    TSoftObjectPtr<UTrinityFlowPhysicalKatanaStats> DefaultRightKatanaStats;

private:
    // Cached stats for performance
    UPROPERTY()
    TMap<FName, UTrinityFlowCharacterStats*> LoadedCharacterStats;

    UPROPERTY()
    TMap<FName, UTrinityFlowWeaponStatsBase*> LoadedWeaponStats;

    void LoadCharacterStats();
    void LoadWeaponStats();
    void ClearCache();
};