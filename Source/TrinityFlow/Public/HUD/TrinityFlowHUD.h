#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowHUD.generated.h"

UCLASS()
class TRINITYFLOW_API ATrinityFlowHUD : public AHUD
{
    GENERATED_BODY()

public:
    ATrinityFlowHUD();

    virtual void DrawHUD() override;
    virtual void BeginPlay() override;

protected:
    void DrawHealthBar();
    void DrawCrosshair();
    void DrawTargetInfo();
    void DrawPlayerInfo();
    void DrawCombatState();
    void DrawWeaponInfo();
    void DrawNearbyEnemies();

    void DrawBar(float X, float Y, float Width, float Height, float Percentage, FLinearColor Color);
    FString GetTagsString(ECharacterTag InTags);
    FString GetStatesString(ECharacterState InStates);

    UPROPERTY()
    class ATrinityFlowCharacter* PlayerCharacter;

    UPROPERTY()
    AActor* CurrentTarget;

    UPROPERTY()
    FVector2D ViewportSize;

    // HUD Configuration
    float HealthBarWidth = 300.0f;
    float HealthBarHeight = 20.0f;
    float CrosshairSize = 10.0f;
    float InfoPanelWidth = 300.0f;
    float LineHeight = 20.0f;
};