// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TrinityFlowGameMode.generated.h"

UCLASS(minimalapi)
class ATrinityFlowGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATrinityFlowGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void InitGameState() override;
	
	// Called when player controller is ready
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	// UI Management
	void InitializeUI();
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnPauseRequested();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetPauseState(bool bPaused);
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGame();

private:
	// UI Manager reference
	class UTrinityFlowUIManager* UIManager = nullptr;
};



