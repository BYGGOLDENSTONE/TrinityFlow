#include "UI/Slate/STrinityFlowHUD.h"
#include "UI/TrinityFlowUIManager.h"
#include "UI/TrinityFlowStyle.h"
#include "UI/Slate/STrinityFlowHealthBar.h"
#include "UI/Slate/STrinityFlowWeaponPanel.h"
#include "UI/Slate/STrinityFlowDamageNumber.h"
#include "UI/Slate/STrinityFlowEnemyInfoPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Enemy/EnemyBase.h"

#define LOCTEXT_NAMESPACE "TrinityFlowHUD"

void STrinityFlowHUD::Construct(const FArguments& InArgs)
{
    UIManager = InArgs._UIManager;
    const ISlateStyle* Style = &FTrinityFlowStyle::Get();

    ChildSlot
    [
        SNew(SOverlay)
        
        // Enemy Info Panels Layer
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(EnemyInfoOverlay, SOverlay)
        ]

        // Damage Numbers Layer
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(DamageNumberOverlay, SOverlay)
        ]
        
        // Health Bar (Bottom Left)
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Bottom)
        .Padding(20, 0, 0, 100)
        [
            SNew(SBox)
            .WidthOverride(300)
            .HeightOverride(30)
            [
                SAssignNew(HealthBar, STrinityFlowHealthBar)
            ]
        ]
        
        // Weapon Panels (Bottom Center)
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Padding(0, 0, 0, 50)
        [
            SNew(SHorizontalBox)
            // Left Weapon (Soul)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SAssignNew(LeftWeaponPanel, STrinityFlowWeaponPanel)
                .WeaponName(FText::FromString("Override Katana"))
                .WeaponColor(Style->GetColor("TrinityFlow.Color.Soul"))
                .AbilityKey1(FText::FromString("Q"))
                .AbilityKey2(FText::FromString("Tab"))
            ]
            
            // Separator
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(2)
                .HeightOverride(80)
                [
                    SNew(SBorder)
                    .BorderImage(Style->GetBrush("TrinityFlow.Brush.Separator"))
                ]
            ]
            
            // Right Weapon (Physical)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SAssignNew(RightWeaponPanel, STrinityFlowWeaponPanel)
                .WeaponName(FText::FromString("Physical Katana"))
                .WeaponColor(Style->GetColor("TrinityFlow.Color.Power"))
                .AbilityKey1(FText::FromString("E"))
                .AbilityKey2(FText::FromString("R"))
            ]
        ]
        
        // Combat State (Top Center)
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Top)
        .Padding(0, 50, 0, 0)
        [
            SNew(SBorder)
            .BorderImage(Style->GetBrush("TrinityFlow.Brush.ToolPanel"))
            .Padding(20, 10)
            [
                SAssignNew(CombatStateText, STextBlock)
                .Text(LOCTEXT("NonCombat", "NON-COMBAT"))
                .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Bold"))
                .ColorAndOpacity(Style->GetColor("TrinityFlow.Color.Positive"))
                .ShadowOffset(FVector2D(1, 1))
                .ShadowColorAndOpacity(FLinearColor::Black)
            ]
        ]
        
        // Player Stats (Bottom Left, above health)
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Bottom)
        .Padding(20, 0, 0, 140)
        [
            SNew(SBorder)
            .BorderImage(Style->GetBrush("TrinityFlow.Brush.ToolPanel"))
            .Padding(15)
            [
                SAssignNew(PlayerStatsBox, SVerticalBox)
                // Stance
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SAssignNew(StanceText, STextBlock)
                    .Text(LOCTEXT("BalancedStance", "Stance: Balanced"))
                    .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                    .ColorAndOpacity(Style->GetColor("TrinityFlow.Color.Balanced"))
                ]
                
                // Active Shards
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 5, 0, 0)
                [
                    SAssignNew(ShardsText, STextBlock)
                    .Text(LOCTEXT("ActiveShards", "Active: 0 Soul / 0 Power"))
                    .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                    .ColorAndOpacity(Style->GetColor("TrinityFlow.Color.Neutral"))
                ]
                
                // Inactive Shards
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 2, 0, 0)
                [
                    SAssignNew(InactiveShardsText, STextBlock)
                    .Text(LOCTEXT("InactiveShards", "Inactive: 0 Soul / 0 Power"))
                    .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f)))
                ]
                
                // Damage Bonuses
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 5, 0, 0)
                [
                    SAssignNew(DamageBonusText, STextBlock)
                    .Text(LOCTEXT("DamageBonus", "Damage: +0% Soul / +0% Physical"))
                    .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                    .ColorAndOpacity(FLinearColor::Yellow)
                ]
            ]
        ]
        
        // Crosshair (Center)
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(10)
            .HeightOverride(10)
            [
                SNew(SBorder)
                .BorderImage(Style->GetBrush("TrinityFlow.Brush.White"))
            ]
        ]
    ];
}

void STrinityFlowHUD::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    UpdateEnemyInfoPanels(AllottedGeometry);

    // Update damage numbers
    for (int32 i = DamageNumbers.Num() - 1; i >= 0; i--)
    {
        if (DamageNumbers[i].IsValid())
        {
            if (DamageNumbers[i]->IsExpired())
            {
                DamageNumberOverlay->RemoveSlot(DamageNumbers[i].ToSharedRef());
                DamageNumbers.RemoveAt(i);
            }
        }
    }
}

void STrinityFlowHUD::UpdatePlayerHealth(float HealthPercentage)
{
    if (HealthBar.IsValid())
    {
        HealthBar->SetHealthPercentage(HealthPercentage);
    }
}

void STrinityFlowHUD::UpdateWeaponCooldowns(float QCooldown, float TabCooldown, float ECooldown, float RCooldown)
{
    if (LeftWeaponPanel.IsValid())
    {
        LeftWeaponPanel->SetCooldowns(QCooldown, TabCooldown);
    }
    
    if (RightWeaponPanel.IsValid())
    {
        RightWeaponPanel->SetCooldowns(ECooldown, RCooldown);
    }
}

void STrinityFlowHUD::UpdatePlayerStats(int32 SoulActive, int32 PowerActive, int32 SoulInactive, int32 PowerInactive, float SoulBonus, float PhysicalBonus)
{
    // Update stance based on shard counts
    FText StanceTextValue;
    FLinearColor StanceColor;
    
    if (SoulActive > PowerActive)
    {
        StanceTextValue = LOCTEXT("SoulStance", "Stance: Soul");
        StanceColor = FLinearColor(0.0f, 0.5f, 1.0f);
    }
    else if (PowerActive > SoulActive)
    {
        StanceTextValue = LOCTEXT("PowerStance", "Stance: Power");
        StanceColor = FLinearColor(1.0f, 0.5f, 0.0f);
    }
    else
    {
        StanceTextValue = LOCTEXT("BalancedStance", "Stance: Balanced");
        StanceColor = FLinearColor(0.8f, 0.2f, 1.0f);
    }
    
    // Update stats display
    if (StanceText.IsValid())
    {
        StanceText->SetText(StanceTextValue);
        StanceText->SetColorAndOpacity(FSlateColor(StanceColor));
    }
    
    if (ShardsText.IsValid())
    {
        FText ShardsTextValue = FText::Format(LOCTEXT("ActiveShardsFormat", "Active: {0} Soul / {1} Power"), SoulActive, PowerActive);
        ShardsText->SetText(ShardsTextValue);
    }
    
    if (InactiveShardsText.IsValid())
    {
        FText InactiveShardsTextValue = FText::Format(LOCTEXT("InactiveShardsFormat", "Inactive: {0} Soul / {1} Power"), SoulInactive, PowerInactive);
        InactiveShardsText->SetText(InactiveShardsTextValue);
    }
    
    if (DamageBonusText.IsValid())
    {
        FText DamageTextValue = FText::Format(LOCTEXT("DamageFormat", "Damage: +{0}% Soul / +{1}% Physical"), 
            FMath::RoundToInt(SoulBonus * 100), FMath::RoundToInt(PhysicalBonus * 100));
        DamageBonusText->SetText(DamageTextValue);
    }
}

void STrinityFlowHUD::UpdateCombatState(bool bInCombat)
{
    if (CombatStateText.IsValid())
    {
        if (bInCombat)
        {
            CombatStateText->SetText(LOCTEXT("InCombat", "IN COMBAT"));
            CombatStateText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
        }
        else
        {
            CombatStateText->SetText(LOCTEXT("NonCombat", "NON-COMBAT"));
            CombatStateText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
        }
    }
}

void STrinityFlowHUD::AddDamageNumber(const FVector& WorldLocation, float Damage, bool bIsEcho, EDamageType DamageType)
{
    // Create new damage number widget
    TSharedPtr<STrinityFlowDamageNumber> NewDamageNumber;
    
    DamageNumberOverlay->AddSlot()
    [
        SAssignNew(NewDamageNumber, STrinityFlowDamageNumber)
        .WorldLocation(WorldLocation)
        .Damage(Damage)
        .bIsEcho(bIsEcho)
        .DamageType(DamageType)
    ];
    
    DamageNumbers.Add(NewDamageNumber);
}

void STrinityFlowHUD::UpdateEnemyInfoPanels(const FGeometry& AllottedGeometry)
{
    if (!UIManager || !UIManager->GetWorld())
    {
        return;
    }

    const TArray<AEnemyBase*>& Enemies = UIManager->GetRegisteredEnemies();
    TSet<AEnemyBase*> CurrentEnemies(Enemies);
    TSet<AEnemyBase*> KnownEnemies;
    for (auto const& [Enemy, Panel] : EnemyInfoPanels)
    {
        KnownEnemies.Add(Enemy);
    }

    // Remove panels for enemies that are no longer registered
    TSet<AEnemyBase*> EnemiesToRemove = KnownEnemies.Difference(CurrentEnemies);
    for (AEnemyBase* Enemy : EnemiesToRemove)
    {
        if (EnemyInfoPanels.Contains(Enemy))
        {
            EnemyInfoOverlay->RemoveSlot(EnemyInfoPanels[Enemy].ToSharedRef());
            EnemyInfoPanels.Remove(Enemy);
        }
    }

    // Add panels for new enemies and update existing ones
    for (AEnemyBase* Enemy : Enemies)
    {
        TSharedPtr<STrinityFlowEnemyInfoPanel> Panel = EnemyInfoPanels.FindRef(Enemy);
        if (!Panel.IsValid())
        {
            SAssignNew(Panel, STrinityFlowEnemyInfoPanel).Enemy(Enemy);
            EnemyInfoOverlay->AddSlot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            [
                Panel.ToSharedRef()
            ];
            EnemyInfoPanels.Add(Enemy, Panel);
        }

        // Update panel position and content
        FVector2D ScreenPosition;
        if (APlayerController* PC = UIManager->GetWorld()->GetFirstPlayerController())
        {
            if (PC->ProjectWorldLocationToScreen(Enemy->GetActorLocation() + FVector(0, 0, 100), ScreenPosition))
            {
                Panel->SetVisibility(EVisibility::Visible);
                // Use RenderTransform to position the panel
                Panel->SetRenderTransform(FSlateRenderTransform(ScreenPosition));
                Panel->Update();
            }
            else
            {
                Panel->SetVisibility(EVisibility::Hidden);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE
