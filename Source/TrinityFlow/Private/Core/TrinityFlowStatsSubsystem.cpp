#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowCharacterStats.h"
#include "Data/TrinityFlowWeaponStats.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"

void UTrinityFlowStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("TrinityFlow Stats Subsystem initializing..."));
    
    // Load all stats on initialization
    LoadCharacterStats();
    LoadWeaponStats();
}

void UTrinityFlowStatsSubsystem::Deinitialize()
{
    ClearCache();
    Super::Deinitialize();
}

UTrinityFlowCharacterStats* UTrinityFlowStatsSubsystem::GetCharacterStats(FName CharacterID) const
{
    if (UTrinityFlowCharacterStats* const* Found = LoadedCharacterStats.Find(CharacterID))
    {
        return *Found;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Character stats not found for ID: %s"), *CharacterID.ToString());
    return nullptr;
}

UTrinityFlowCharacterStats* UTrinityFlowStatsSubsystem::GetPlayerStats() const
{
    // First try to get from loaded stats
    if (UTrinityFlowCharacterStats* const* Found = LoadedCharacterStats.Find("Player"))
    {
        return *Found;
    }
    
    // Fall back to default
    if (DefaultPlayerStats.IsValid())
    {
        return DefaultPlayerStats.Get();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Player stats not found!"));
    return nullptr;
}

UTrinityFlowWeaponStats* UTrinityFlowStatsSubsystem::GetWeaponStats(FName WeaponID) const
{
    if (UTrinityFlowWeaponStats* const* Found = LoadedWeaponStats.Find(WeaponID))
    {
        return *Found;
    }
    
    // Try defaults
    if (WeaponID == "OverrideKatana" && DefaultKatanaStats.IsValid())
    {
        return DefaultKatanaStats.Get();
    }
    else if (WeaponID == "DivineAnchor" && DefaultAnchorStats.IsValid())
    {
        return DefaultAnchorStats.Get();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Weapon stats not found for ID: %s"), *WeaponID.ToString());
    return nullptr;
}

void UTrinityFlowStatsSubsystem::ReloadAllStats()
{
    UE_LOG(LogTemp, Log, TEXT("Reloading all stats..."));
    
    ClearCache();
    LoadCharacterStats();
    LoadWeaponStats();
}

void UTrinityFlowStatsSubsystem::LoadCharacterStats()
{
    // Load default player stats
    if (DefaultPlayerStats.IsValid())
    {
        if (UTrinityFlowCharacterStats* PlayerStats = DefaultPlayerStats.LoadSynchronous())
        {
            LoadedCharacterStats.Add("Player", PlayerStats);
        }
    }
    
    // Load from data table if available
    if (CharacterStatsTable.IsValid())
    {
        if (UDataTable* Table = CharacterStatsTable.LoadSynchronous())
        {
            TArray<FCharacterStatsTableRow*> AllRows;
            Table->GetAllRows<FCharacterStatsTableRow>(TEXT("LoadCharacterStats"), AllRows);
            
            for (const FCharacterStatsTableRow* Row : AllRows)
            {
                if (Row && Row->StatsAsset.IsValid())
                {
                    if (UTrinityFlowCharacterStats* Stats = Row->StatsAsset.LoadSynchronous())
                    {
                        LoadedCharacterStats.Add(Stats->CharacterID, Stats);
                        UE_LOG(LogTemp, Log, TEXT("Loaded character stats: %s"), *Stats->CharacterID.ToString());
                    }
                }
            }
        }
    }
}

void UTrinityFlowStatsSubsystem::LoadWeaponStats()
{
    // Load default weapon stats
    if (DefaultKatanaStats.IsValid())
    {
        if (UTrinityFlowWeaponStats* KatanaStats = DefaultKatanaStats.LoadSynchronous())
        {
            LoadedWeaponStats.Add("OverrideKatana", KatanaStats);
        }
    }
    
    if (DefaultAnchorStats.IsValid())
    {
        if (UTrinityFlowWeaponStats* AnchorStats = DefaultAnchorStats.LoadSynchronous())
        {
            LoadedWeaponStats.Add("DivineAnchor", AnchorStats);
        }
    }
    
    // Load from data table if available
    if (WeaponStatsTable.IsValid())
    {
        if (UDataTable* Table = WeaponStatsTable.LoadSynchronous())
        {
            TArray<FWeaponStatsTableRow*> AllRows;
            Table->GetAllRows<FWeaponStatsTableRow>(TEXT("LoadWeaponStats"), AllRows);
            
            for (const FWeaponStatsTableRow* Row : AllRows)
            {
                if (Row && Row->StatsAsset.IsValid())
                {
                    if (UTrinityFlowWeaponStats* Stats = Row->StatsAsset.LoadSynchronous())
                    {
                        LoadedWeaponStats.Add(Stats->WeaponID, Stats);
                        UE_LOG(LogTemp, Log, TEXT("Loaded weapon stats: %s"), *Stats->WeaponID.ToString());
                    }
                }
            }
        }
    }
}

void UTrinityFlowStatsSubsystem::ClearCache()
{
    LoadedCharacterStats.Empty();
    LoadedWeaponStats.Empty();
}