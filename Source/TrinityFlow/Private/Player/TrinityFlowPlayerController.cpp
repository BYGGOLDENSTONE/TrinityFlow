#include "Player/TrinityFlowPlayerController.h"
#include "Engine/World.h"

ATrinityFlowPlayerController::ATrinityFlowPlayerController()
{
    bShowMouseCursor = false;
}

void ATrinityFlowPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // No HUD setup needed - UIManager handles all UI
}

void ATrinityFlowPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // Additional input setup if needed
}