#include "UI/Slate/STrinityFlowHUD.h"
#include "UI/TrinityFlowUIManager.h"
#include "UI/Slate/STrinityFlowHealthBar.h"
#include "UI/Slate/STrinityFlowWeaponPanel.h"
#include "UI/Slate/STrinityFlowDamageNumber.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Engine/World.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowHUD"

void STrinityFlowHUD::Construct(const FArguments& InArgs)
{
    UIManager = InArgs._UIManager;

    ChildSlot
    [
        SNew(SOverlay)
        
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
                .WeaponColor(FLinearColor(0.0f, 0.5f, 1.0f))
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
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    .BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f))
                ]
            ]
            
            // Right Weapon (Physical)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(10, 0)
            [
                SAssignNew(RightWeaponPanel, STrinityFlowWeaponPanel)
                .WeaponName(FText::FromString("Physical Katana"))
                .WeaponColor(FLinearColor(1.0f, 0.5f, 0.0f))
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
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
            .Padding(20, 10)
            [
                SAssignNew(CombatStateText, STextBlock)
                .Text(LOCTEXT("NonCombat", "NON-COMBAT"))
                .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
                .ColorAndOpacity(FSlateColor(FLinearColor::Green))
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
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
            .Padding(15)
            [
                SAssignNew(PlayerStatsBox, SVerticalBox)
                // Stance
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SAssignNew(StanceText, STextBlock)
                    .Text(LOCTEXT("BalancedStance", "Stance: Balanced"))
                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.2f, 1.0f)))
                ]
                
                // Shards
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 5, 0, 0)
                [
                    SAssignNew(ShardsText, STextBlock)
                    .Text(LOCTEXT("Shards", "Shards: 0 Soul / 0 Power"))
                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                ]
                
                // Damage Bonuses
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 5, 0, 0)
                [
                    SAssignNew(DamageBonusText, STextBlock)
                    .Text(LOCTEXT("DamageBonus", "Damage: +0% Soul / +0% Physical"))
                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                    .ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
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
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .BorderBackgroundColor(FLinearColor::White)
            ]
        ]
    ];
}

void STrinityFlowHUD::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
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

void STrinityFlowHUD::UpdatePlayerStats(int32 SoulActive, int32 PowerActive, float SoulBonus, float PhysicalBonus)
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
        FText ShardsTextValue = FText::Format(LOCTEXT("ShardsFormat", "Shards: {0} Soul / {1} Power"), SoulActive, PowerActive);
        ShardsText->SetText(ShardsTextValue);
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

#undef LOCTEXT_NAMESPACE