// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrinityFlowGameMode.h"
#include "TrinityFlowCharacter.h"
#include "Player/TrinityFlowPlayerController.h"
#include "HUD/TrinityFlowHUD.h"
#include "UObject/ConstructorHelpers.h"

ATrinityFlowGameMode::ATrinityFlowGameMode()
{
	// Set default classes
	DefaultPawnClass = ATrinityFlowCharacter::StaticClass();
	PlayerControllerClass = ATrinityFlowPlayerController::StaticClass();
	HUDClass = ATrinityFlowHUD::StaticClass();
}
