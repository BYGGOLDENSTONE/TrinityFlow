#include "UI/Slate/STrinityFlowWeaponPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowWeaponPanel"

void STrinityFlowWeaponPanel::Construct(const FArguments& InArgs)
{
    FText WeaponName = InArgs._WeaponName;
    FLinearColor WeaponColor = InArgs._WeaponColor;
    FText AbilityKey1 = InArgs._AbilityKey1;
    FText AbilityKey2 = InArgs._AbilityKey2;

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f))
        .Padding(15)
        [
            SNew(SVerticalBox)
            
            // Weapon Name
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .Padding(0, 0, 0, 10)
            [
                SNew(STextBlock)
                .Text(WeaponName)
                .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
                .ColorAndOpacity(FSlateColor(WeaponColor))
                .ShadowOffset(FVector2D(1, 1))
                .ShadowColorAndOpacity(FLinearColor::Black)
            ]
            
            // Abilities
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                
                // Ability 1
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(0, 0, 5, 0)
                [
                    SNew(SOverlay)
                    
                    // Background
                    + SOverlay::Slot()
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
                    ]
                    
                    // Cooldown Bar
                    + SOverlay::Slot()
                    .VAlign(VAlign_Bottom)
                    [
                        SAssignNew(Ability1CooldownBar, SProgressBar)
                        .Percent(0.0f)
                        .FillColorAndOpacity(FLinearColor(0.8f, 0.0f, 0.0f, 0.5f))
                        .BackgroundImage(FCoreStyle::Get().GetBrush("ProgressBar.Background"))
                    ]
                    
                    // Text
                    + SOverlay::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .Padding(10, 5)
                    [
                        SAssignNew(Ability1Text, STextBlock)
                        .Text(FText::Format(LOCTEXT("AbilityReady", "{0}: Ready"), AbilityKey1))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                        .ColorAndOpacity(FSlateColor(FLinearColor::Green))
                    ]
                ]
                
                // Ability 2
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(5, 0, 0, 0)
                [
                    SNew(SOverlay)
                    
                    // Background
                    + SOverlay::Slot()
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
                    ]
                    
                    // Cooldown Bar
                    + SOverlay::Slot()
                    .VAlign(VAlign_Bottom)
                    [
                        SAssignNew(Ability2CooldownBar, SProgressBar)
                        .Percent(0.0f)
                        .FillColorAndOpacity(FLinearColor(0.8f, 0.0f, 0.0f, 0.5f))
                        .BackgroundImage(FCoreStyle::Get().GetBrush("ProgressBar.Background"))
                    ]
                    
                    // Text
                    + SOverlay::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .Padding(10, 5)
                    [
                        SAssignNew(Ability2Text, STextBlock)
                        .Text(FText::Format(LOCTEXT("AbilityReady", "{0}: Ready"), AbilityKey2))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
                        .ColorAndOpacity(FSlateColor(FLinearColor::Green))
                    ]
                ]
            ]
        ]
    ];
}

void STrinityFlowWeaponPanel::SetCooldowns(float Cooldown1, float Cooldown2)
{
    Ability1Cooldown = Cooldown1;
    Ability2Cooldown = Cooldown2;
    
    // Update Ability 1
    if (Ability1Text.IsValid() && Ability1CooldownBar.IsValid())
    {
        if (Cooldown1 > 0.0f)
        {
            FText CooldownText = FText::Format(LOCTEXT("AbilityCooldown", "{0}s"), FMath::CeilToInt(Cooldown1));
            Ability1Text->SetText(CooldownText);
            Ability1Text->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            
            // Show cooldown bar (inverted - full when ready, empty when on cooldown)
            Ability1CooldownBar->SetPercent(1.0f);
        }
        else
        {
            Ability1Text->SetText(LOCTEXT("Ready", "Ready"));
            Ability1Text->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
            Ability1CooldownBar->SetPercent(0.0f);
        }
    }
    
    // Update Ability 2
    if (Ability2Text.IsValid() && Ability2CooldownBar.IsValid())
    {
        if (Cooldown2 > 0.0f)
        {
            FText CooldownText = FText::Format(LOCTEXT("AbilityCooldown", "{0}s"), FMath::CeilToInt(Cooldown2));
            Ability2Text->SetText(CooldownText);
            Ability2Text->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            
            // Show cooldown bar
            Ability2CooldownBar->SetPercent(1.0f);
        }
        else
        {
            Ability2Text->SetText(LOCTEXT("Ready", "Ready"));
            Ability2Text->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
            Ability2CooldownBar->SetPercent(0.0f);
        }
    }
}

#undef LOCTEXT_NAMESPACE