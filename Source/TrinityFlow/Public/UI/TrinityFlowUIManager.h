#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowUIManager.generated.h"

UENUM(BlueprintType)
enum class EUIState : uint8
{
    None,
    MainMenu,
    InGame,
    PauseMenu,
    ShardAltar,
    Settings
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIStateChanged, EUIState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageNumber, const FVector&, WorldLocation, float, Damage);

// Forward declarations
class AShardAltar;
class AEnemyBase;

/**
 * Central UI Manager for TrinityFlow
 * Manages all UI states and transitions
 */
UCLASS()
class TRINITYFLOW_API UTrinityFlowUIManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // UGameInstanceSubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // State Management
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetUIState(EUIState NewState);

    UFUNCTION(BlueprintCallable, Category = "UI")
    EUIState GetCurrentUIState() const { return CurrentUIState; }

    // Menu Functions
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPauseMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowInGameHUD();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowShardAltarUI(class AShardAltar* Altar);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideAllUI();

    // HUD Functions
    UFUNCTION(BlueprintCallable, Category = "UI")
    void AddDamageNumber(const FVector& WorldLocation, float Damage, bool bIsEcho = false, EDamageType DamageType = EDamageType::Physical);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdatePlayerHealth(float HealthPercentage);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateWeaponCooldowns(float QCooldown, float TabCooldown, float ECooldown, float RCooldown);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdatePlayerStats(int32 SoulActive, int32 PowerActive, int32 SoulInactive, int32 PowerInactive, float SoulBonus, float PhysicalBonus);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateCombatState(bool bInCombat);

    UFUNCTION()
    void OnDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType);

    // Enemy Registry
    void RegisterEnemy(AEnemyBase* Enemy);
    void UnregisterEnemy(AEnemyBase* Enemy);
    const TArray<AEnemyBase*>& GetRegisteredEnemies() const;
    
    // Shard Altar
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenShardActivationUI(class AShardAltar* Altar);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseShardActivationUI();
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    bool IsShardActivationUIOpen() const { return ActiveAltar != nullptr; }
    
    // Floating Text
    UFUNCTION(BlueprintCallable, Category = "UI")
    void AddFloatingText(const FVector& WorldLocation, const FString& Text, const FLinearColor& Color);
    
    // Getters

    // Events
    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnUIStateChanged OnUIStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI")
    FOnDamageNumber OnDamageNumber;

protected:
    UPROPERTY()
    EUIState CurrentUIState = EUIState::None;

    // Widget References
    TSharedPtr<class STrinityFlowMainMenu> MainMenuWidget;
    TSharedPtr<class STrinityFlowPauseMenu> PauseMenuWidget;
    TSharedPtr<class STrinityFlowHUD> HUDWidget;
    TSharedPtr<class STrinityFlowShardAltar> ShardAltarWidget;

    // Viewport slot for current widget
    TSharedPtr<class SWeakWidget> CurrentWidgetContainer;

    // Enemy Registry
    TArray<AEnemyBase*> RegisteredEnemies;
    
    // Active Altar
    UPROPERTY()
    class AShardAltar* ActiveAltar = nullptr;

    void CreateWidgets();
    void ShowWidget(TSharedPtr<SWidget> Widget);
    void RemoveCurrentWidget();
    
    // Input mode management
    void SetInputModeUI();
    void SetInputModeGame();
};