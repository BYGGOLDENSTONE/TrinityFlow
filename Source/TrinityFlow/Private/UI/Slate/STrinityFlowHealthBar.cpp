#include "UI/Slate/STrinityFlowHealthBar.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowHealthBar"

void STrinityFlowHealthBar::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SOverlay)
        
        // Background
        + SOverlay::Slot()
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ProgressBar.Background"))
            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
        ]
        
        // Health Bar
        + SOverlay::Slot()
        .Padding(2)
        [
            SAssignNew(HealthProgressBar, SProgressBar)
            .Percent(1.0f)
            .FillColorAndOpacity(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f))
            .BackgroundImage(FCoreStyle::Get().GetBrush("WhiteBox"))
            .BarFillType(EProgressBarFillType::LeftToRight)
            .Style(&FCoreStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar"))
        ]
        
        // Damage Flash
        + SOverlay::Slot()
        [
            SAssignNew(DamageFlashBorder, SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .BorderBackgroundColor(FLinearColor(1.0f, 0.0f, 0.0f, 0.0f))
        ]
        
        // Health Text
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(HealthText, STextBlock)
            .Text(LOCTEXT("FullHealth", "100/100"))
            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14))
            .ColorAndOpacity(FSlateColor(FLinearColor::White))
            .ShadowOffset(FVector2D(1, 1))
            .ShadowColorAndOpacity(FLinearColor::Black)
        ]
    ];
}

void STrinityFlowHealthBar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Smooth health bar animation
    if (!FMath::IsNearlyEqual(CurrentHealthPercentage, TargetHealthPercentage))
    {
        CurrentHealthPercentage = FMath::FInterpTo(CurrentHealthPercentage, TargetHealthPercentage, InDeltaTime, 5.0f);
        
        if (HealthProgressBar.IsValid())
        {
            HealthProgressBar->SetPercent(CurrentHealthPercentage);
        }
        
        // Update text
        if (HealthText.IsValid())
        {
            int32 CurrentHealth = FMath::RoundToInt(CurrentHealthPercentage * 100);
            FText NewHealthText = FText::Format(LOCTEXT("HealthFormat", "{0}/100"), CurrentHealth);
            HealthText->SetText(NewHealthText);
        }
    }
    
    // Damage flash animation
    if (bPlayingDamageAnimation && DamageFlashBorder.IsValid())
    {
        DamageFlashAlpha = FMath::Max(0.0f, DamageFlashAlpha - InDeltaTime * 3.0f);
        
        FLinearColor FlashColor(1.0f, 0.0f, 0.0f, DamageFlashAlpha);
        DamageFlashBorder->SetBorderBackgroundColor(FlashColor);
        
        if (DamageFlashAlpha <= 0.0f)
        {
            bPlayingDamageAnimation = false;
        }
    }
}

void STrinityFlowHealthBar::SetHealthPercentage(float NewHealthPercentage)
{
    // Play damage animation if health decreased
    if (NewHealthPercentage < TargetHealthPercentage)
    {
        PlayDamageAnimation();
    }
    
    TargetHealthPercentage = FMath::Clamp(NewHealthPercentage, 0.0f, 1.0f);
}

void STrinityFlowHealthBar::PlayDamageAnimation()
{
    bPlayingDamageAnimation = true;
    DamageFlashAlpha = 0.7f;
}

#undef LOCTEXT_NAMESPACE