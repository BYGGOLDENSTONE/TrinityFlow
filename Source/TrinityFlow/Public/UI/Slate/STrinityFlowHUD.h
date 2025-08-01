#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Core/TrinityFlowTypes.h"

class UTrinityFlowUIManager;
class STrinityFlowHealthBar;
class STrinityFlowWeaponPanel;
class STrinityFlowDamageNumber;
class STrinityFlowEnemyInfoPanel;
class AEnemyBase;

/**
 * Main HUD Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowHUD : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowHUD) {}
        SLATE_ARGUMENT(UTrinityFlowUIManager*, UIManager)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    // HUD Updates
    void UpdatePlayerHealth(float HealthPercentage);
    void UpdateWeaponCooldowns(float QCooldown, float TabCooldown, float ECooldown, float RCooldown);
    void UpdatePlayerStats(int32 SoulActive, int32 PowerActive, int32 SoulInactive, int32 PowerInactive, float SoulBonus, float PhysicalBonus);
    void UpdateCombatState(bool bInCombat);
    
    // Damage Numbers
    void AddDamageNumber(const FVector& WorldLocation, float Damage, bool bIsEcho, EDamageType DamageType);
    
    // Defense timing bar
    void ShowDefenseTiming(float Duration, float PerfectStart, float PerfectEnd);
    void HideDefenseTiming();
    void ShowEnemyDefenseTiming(AEnemyBase* Enemy, float Duration, float PerfectStart, float PerfectEnd);
    void HideEnemyDefenseTiming(AEnemyBase* Enemy);
    
    // Stance flow bar
    void ShowStanceBar();
    void HideStanceBar();
    void UpdateStanceBar(float FlowPosition);

private:
    void UpdateEnemyInfoPanels(const FGeometry& AllottedGeometry);

    // Widget References
    TSharedPtr<STrinityFlowHealthBar> HealthBar;
    TSharedPtr<STrinityFlowWeaponPanel> LeftWeaponPanel;
    TSharedPtr<STrinityFlowWeaponPanel> RightWeaponPanel;
    TSharedPtr<class STextBlock> CombatStateText;
    TSharedPtr<class SVerticalBox> PlayerStatsBox;
    TSharedPtr<class SOverlay> DamageNumberOverlay;
    TSharedPtr<class SOverlay> EnemyInfoOverlay;
    TSharedPtr<class STrinityFlowDefenseTimingBar> DefenseTimingBar;
    TSharedPtr<class STrinityFlowStanceBar> StanceBar;
    
    // Player stats text blocks
    TSharedPtr<class STextBlock> StanceText;
    TSharedPtr<class STextBlock> ShardsText;
    TSharedPtr<class STextBlock> InactiveShardsText;
    TSharedPtr<class STextBlock> DamageBonusText;
    
    // Damage Numbers
    TArray<TSharedPtr<STrinityFlowDamageNumber>> DamageNumbers;

    // Enemy Info Panels
    TMap<AEnemyBase*, TSharedPtr<STrinityFlowEnemyInfoPanel>> EnemyInfoPanels;
    
    // UI Manager reference
    UTrinityFlowUIManager* UIManager = nullptr;
};