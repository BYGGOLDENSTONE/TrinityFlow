#include "Player/TrinityFlowPlayerController.h"
#include "HUD/TrinityFlowHUD.h"
#include "Engine/World.h"

ATrinityFlowPlayerController::ATrinityFlowPlayerController()
{
    bShowMouseCursor = false;
}

void ATrinityFlowPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Set the HUD
    if (GetWorld())
    {
        TrinityFlowHUD = Cast<ATrinityFlowHUD>(GetHUD());
    }
}

void ATrinityFlowPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // Additional input setup if needed
}