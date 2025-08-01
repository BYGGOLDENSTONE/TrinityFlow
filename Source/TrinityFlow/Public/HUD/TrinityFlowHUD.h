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
    FString Text; // For defense results
    EDamageType DamageType; // To determine color

    FFloatingDamageNumber()
    {
        WorldLocation = FVector::ZeroVector;
        Damage = 0.0f;
        LifeTime = 0.0f;
        Color = FLinearColor::White;
        bIsEcho = false;
        Text = "";
        DamageType = EDamageType::Physical;
    }
};

UENUM()
enum class EShardActivationUIState : uint8
{
    Closed,
    SelectingShardType,
    InputtingAmount
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
    
    // Integration with new UI system
    void SetupSlateUI();

    void AddFloatingDamageNumber(const FVector& Location, float Damage, bool bIsEcho = false, EDamageType DamageType = EDamageType::Physical);
    void AddFloatingText(const FVector& Location, const FString& Text, const FLinearColor& Color);
    
    // Shard Activation UI
    UFUNCTION()
    void OpenShardActivationUI(class AShardAltar* Altar);
    
    UFUNCTION()
    void CloseShardActivationUI();
    
    UFUNCTION()
    bool IsShardActivationUIOpen() const { return ShardActivationUIState != EShardActivationUIState::Closed; }
    
    UFUNCTION()
    void ProcessShardActivation();

protected:
    void DrawHealthBar();
    void DrawCrosshair();
    void DrawPlayerInfo();
    void DrawCombatState();
    void DrawWeaponInfo();
    void DrawFloatingDamageNumbers();
    void DrawAltarInteractionUI();
    void DrawShardActivationPanel();
    
    bool IsNearAltar() const;
    class AShardAltar* GetNearbyAltar() const;

    void DrawBar(float X, float Y, float Width, float Height, float Percentage, FLinearColor Color);
    FString GetTagsString(ECharacterTag InTags);
    FString GetStatesString(ECharacterState InStates);
    
    // Shard Activation UI Helpers
    void UpdateStancePreview();
    EStanceType CalculateStanceFromShards(int32 SoulActive, int32 PowerActive) const;

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
    
    // Altar UI
    UPROPERTY()
    class AShardAltar* NearbyAltar = nullptr;
    
    // Shard Activation UI State
    EShardActivationUIState ShardActivationUIState = EShardActivationUIState::Closed;
    UPROPERTY()
    class AShardAltar* ActiveAltar = nullptr;
    
    // Selected shard type (true = Soul, false = Power)
    bool bSelectingSoulShards = true;
    
    // Input amounts
    FString SoulShardInput = "0";
    FString PowerShardInput = "0";
    
    // Current input string
    FString* CurrentInputString = nullptr;
    
    // Cached shard counts
    int32 CachedSoulActive = 0;
    int32 CachedSoulInactive = 0;
    int32 CachedPowerActive = 0;
    int32 CachedPowerInactive = 0;
    
    // Slate UI integration
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    bool bUseSlateUI = true;
    
    class UTrinityFlowUIManager* UIManager = nullptr;
};