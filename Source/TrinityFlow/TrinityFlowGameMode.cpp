// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrinityFlowGameMode.h"
#include "TrinityFlowCharacter.h"
#include "Player/TrinityFlowPlayerController.h"
#include "UI/TrinityFlowUIManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ATrinityFlowGameMode::ATrinityFlowGameMode()
{
	// Set default classes
	DefaultPawnClass = ATrinityFlowCharacter::StaticClass();
	PlayerControllerClass = ATrinityFlowPlayerController::StaticClass();
	// No HUD class - we're using UIManager for all UI
	HUDClass = nullptr;
}

void ATrinityFlowGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeUI();
}

void ATrinityFlowGameMode::InitGameState()
{
	Super::InitGameState();
}

void ATrinityFlowGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	// Don't set input mode here - let UI Manager handle it based on current UI state
}

void ATrinityFlowGameMode::InitializeUI()
{
	// Get UI Manager
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UIManager = GameInstance->GetSubsystem<UTrinityFlowUIManager>();
		if (UIManager)
		{
			// Always show main menu at game start
			// The menu will handle transitioning to gameplay
			UIManager->ShowMainMenu();
		}
	}
}

void ATrinityFlowGameMode::RestartGame()
{
	// Get the current level name
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	
	// Reload the current level to fully reset the game
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}

void ATrinityFlowGameMode::OnPauseRequested()
{
	SetPauseState(true);
}

void ATrinityFlowGameMode::SetPauseState(bool bPaused)
{
	if (UIManager)
	{
		// Set game pause state
		UGameplayStatics::SetGamePaused(GetWorld(), bPaused);
		
		if (bPaused)
		{
			// Show pause menu
			UIManager->ShowPauseMenu();
			
			// Set input mode to UI
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(nullptr);
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
		}
		else
		{
			// Resume game
			UIManager->ShowInGameHUD();
			
			// Set input mode back to game
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = false;
			}
		}
	}
}
