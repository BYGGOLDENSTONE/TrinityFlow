#include "Core/TrinityFlowGameInstance.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowCharacterStats.h"
#include "Data/TrinityFlowWeaponStats.h"
#include "Engine/DataTable.h"

UTrinityFlowGameInstance::UTrinityFlowGameInstance()
{
}

void UTrinityFlowGameInstance::Init()
{
    Super::Init();
    
    UE_LOG(LogTemp, Log, TEXT("TrinityFlow Game Instance initializing..."));
    
    ConfigureStatsSubsystem();
}

void UTrinityFlowGameInstance::Shutdown()
{
    Super::Shutdown();
}

void UTrinityFlowGameInstance::ConfigureStatsSubsystem()
{
    if (UTrinityFlowStatsSubsystem* StatsSubsystem = GetSubsystem<UTrinityFlowStatsSubsystem>())
    {
        // Configure the subsystem with our settings
        StatsSubsystem->ConfigureFromGameInstance(this);
        
        UE_LOG(LogTemp, Log, TEXT("Configured Stats Subsystem with Game Instance settings"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Stats Subsystem!"));
    }
}