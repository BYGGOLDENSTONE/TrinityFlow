#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowHUD.generated.h"

USTRUCT()
struct FFloatingDamageNumber
{
    GENERATED_BODY()

    FVector WorldLocation;
    float Damage;
    float LifeTime;
    FLinearColor Color;
    bool bIsEcho;

    FFloatingDamageNumber()
    {
        WorldLocation = FVector::ZeroVector;
        Damage = 0.0f;
        LifeTime = 0.0f;
        Color = FLinearColor::White;
        bIsEcho = false;
    }
};

UCLASS()
class TRINITYFLOW_API ATrinityFlowHUD : public AHUD
{
    GENERATED_BODY()

public:
    ATrinityFlowHUD();

    virtual void DrawHUD() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType);

    void AddFloatingDamageNumber(const FVector& Location, float Damage, bool bIsEcho = false);

protected:
    void DrawHealthBar();
    void DrawCrosshair();
    void DrawEnemyInfoAboveHeads();
    void DrawEnemyInfo(class AEnemyBase* Enemy, float ScreenX, float ScreenY);
    void DrawPlayerInfo();
    void DrawCombatState();
    void DrawWeaponInfo();
    void DrawNearbyEnemies();
    void DrawFloatingDamageNumbers();

    void DrawBar(float X, float Y, float Width, float Height, float Percentage, FLinearColor Color);
    FString GetTagsString(ECharacterTag InTags);
    FString GetStatesString(ECharacterState InStates);

    UPROPERTY()
    class ATrinityFlowCharacter* PlayerCharacter;

    UPROPERTY()
    FVector2D ViewportSize;

    // HUD Configuration
    float HealthBarWidth = 300.0f;
    float HealthBarHeight = 20.0f;
    float CrosshairSize = 10.0f;
    float InfoPanelWidth = 300.0f;
    float LineHeight = 20.0f;
    
    // Floating damage numbers
    TArray<FFloatingDamageNumber> FloatingDamageNumbers;
};