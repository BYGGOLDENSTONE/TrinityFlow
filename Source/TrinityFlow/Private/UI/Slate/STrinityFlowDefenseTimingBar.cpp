#include "UI/Slate/STrinityFlowDefenseTimingBar.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowDefenseTimingBar"

void STrinityFlowDefenseTimingBar::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SAssignNew(BackgroundBorder, SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
        .Padding(0)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .Visibility(EVisibility::Collapsed)
        [
            SNew(SVerticalBox)
            
            // Timing bar
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .WidthOverride(300)
                .HeightOverride(20)
                [
                    SAssignNew(TimingOverlay, SOverlay)
                    
                    // Background
                    + SOverlay::Slot()
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f))
                    ]
                    
                    // Zones
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    [
                        SNew(SHorizontalBox)
                        
                        // Failure zone (early)
                        + SHorizontalBox::Slot()
                        .FillWidth(0.33f)
                        [
                            SAssignNew(FailureZone, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor(FLinearColor(0.8f, 0.2f, 0.2f, 0.3f))
                        ]
                        
                        // Moderate zone
                        + SHorizontalBox::Slot()
                        .FillWidth(0.34f)
                        [
                            SAssignNew(ModerateZone, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor(FLinearColor(0.8f, 0.8f, 0.2f, 0.3f))
                        ]
                        
                        // Perfect zone
                        + SHorizontalBox::Slot()
                        .FillWidth(0.33f)
                        [
                            SAssignNew(PerfectZone, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor(FLinearColor(0.2f, 0.8f, 0.2f, 0.3f))
                        ]
                    ]
                    
                    // Moving cursor
                    + SOverlay::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Fill)
                    [
                        SNew(SBox)
                        .WidthOverride(4)
                        [
                            SAssignNew(TimingCursor, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                            .BorderBackgroundColor(FLinearColor::White)
                            .RenderTransform(FSlateRenderTransform(FVector2D(0, 0)))
                        ]
                    ]
                    
                    // Zone labels
                    + SOverlay::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SHorizontalBox)
                        
                    ]
                ]
            ]
        ]
    ];
}

void STrinityFlowDefenseTimingBar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Handle fade animations
    if (bFadingIn)
    {
        FadeAlpha = FMath::Min(1.0f, FadeAlpha + InDeltaTime * 4.0f);
        if (FadeAlpha >= 1.0f)
        {
            bFadingIn = false;
        }
    }
    else if (bFadingOut)
    {
        FadeAlpha = FMath::Max(0.0f, FadeAlpha - InDeltaTime * 4.0f);
        if (FadeAlpha <= 0.0f)
        {
            bFadingOut = false;
            BackgroundBorder->SetVisibility(EVisibility::Collapsed);
        }
    }
    
    if (BackgroundBorder.IsValid())
    {
        BackgroundBorder->SetRenderOpacity(FadeAlpha);
    }
    
    // Update timing if active
    if (bIsActive)
    {
        CurrentTime += InDeltaTime;
        
        if (CurrentTime >= TotalDuration)
        {
            // Time's up - attack lands
            StopTiming();
        }
        else
        {
            UpdateTimingDisplay();
        }
    }
}

void STrinityFlowDefenseTimingBar::StartTiming(float Duration, float PerfectStart, float PerfectEnd)
{
    bIsActive = true;
    CurrentTime = 0.0f;
    TotalDuration = Duration;
    PerfectStartTime = PerfectStart;
    PerfectEndTime = PerfectEnd;
    
    // Start fade in
    bFadingIn = true;
    bFadingOut = false;
    BackgroundBorder->SetVisibility(EVisibility::Visible);
    
    UpdateTimingDisplay();
}

void STrinityFlowDefenseTimingBar::StopTiming()
{
    bIsActive = false;
    
    // Start fade out
    bFadingOut = true;
    bFadingIn = false;
}

STrinityFlowDefenseTimingBar::ETimingZone STrinityFlowDefenseTimingBar::GetCurrentZone() const
{
    if (!bIsActive)
    {
        return ETimingZone::None;
    }
    
    if (CurrentTime < PerfectStartTime)
    {
        return ETimingZone::Failure;
    }
    else if (CurrentTime <= PerfectEndTime)
    {
        return ETimingZone::Perfect;
    }
    else if (CurrentTime < TotalDuration)
    {
        return ETimingZone::Moderate;
    }
    else
    {
        return ETimingZone::Failure;
    }
}

void STrinityFlowDefenseTimingBar::UpdateTimingDisplay()
{
    if (!TimingCursor.IsValid())
    {
        return;
    }
    
    // Update cursor position
    float Progress = CurrentTime / TotalDuration;
    float CursorPosition = Progress * 300.0f; // Bar width
    TimingCursor->SetRenderTransform(FSlateRenderTransform(FVector2D(CursorPosition, 0)));
    
    
    // Highlight current zone
    float ZoneAlpha = 0.3f + 0.2f * FMath::Sin(CurrentTime * 10.0f); // Pulsing effect
    ETimingZone CurrentZone = GetCurrentZone();
    
    if (FailureZone.IsValid() && ModerateZone.IsValid() && PerfectZone.IsValid())
    {
        if (CurrentZone == ETimingZone::Failure && CurrentTime < PerfectStartTime)
        {
            FailureZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.2f, 0.2f, ZoneAlpha));
            ModerateZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.8f, 0.2f, 0.3f));
            PerfectZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.8f, 0.2f, 0.3f));
        }
        else if (CurrentZone == ETimingZone::Perfect)
        {
            FailureZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.2f, 0.2f, 0.3f));
            ModerateZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.8f, 0.2f, 0.3f));
            PerfectZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.8f, 0.2f, ZoneAlpha));
        }
        else if (CurrentZone == ETimingZone::Moderate)
        {
            FailureZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.2f, 0.2f, 0.3f));
            ModerateZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.8f, 0.2f, ZoneAlpha));
            PerfectZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.8f, 0.2f, 0.3f));
        }
    }
}

FLinearColor STrinityFlowDefenseTimingBar::GetZoneColor(ETimingZone Zone) const
{
    switch (Zone)
    {
        case ETimingZone::Perfect:
            return FLinearColor(0.2f, 0.8f, 0.2f);
        case ETimingZone::Moderate:
            return FLinearColor(0.8f, 0.8f, 0.2f);
        case ETimingZone::Failure:
        default:
            return FLinearColor(0.8f, 0.2f, 0.2f);
    }
}

FText STrinityFlowDefenseTimingBar::GetZoneText(ETimingZone Zone) const
{
    switch (Zone)
    {
        case ETimingZone::Perfect:
            return LOCTEXT("PerfectTiming", "PERFECT!");
        case ETimingZone::Moderate:
            return LOCTEXT("ModerateTiming", "MODERATE");
        case ETimingZone::Failure:
            return LOCTEXT("FailureTiming", "TOO EARLY!");
        default:
            return LOCTEXT("Ready", "READY");
    }
}

#undef LOCTEXT_NAMESPACE