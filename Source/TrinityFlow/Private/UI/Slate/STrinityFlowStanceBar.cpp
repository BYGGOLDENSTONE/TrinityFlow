#include "UI/Slate/STrinityFlowStanceBar.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "UI/TrinityFlowStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowStanceBar"

void STrinityFlowStanceBar::Construct(const FArguments& InArgs)
{
    const ISlateStyle* Style = &FTrinityFlowStyle::Get();
    
    ChildSlot
    [
        SAssignNew(BackgroundBar, SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
        .Padding(0)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Bottom)
        .Visibility(EVisibility::Collapsed)
        [
            SNew(SBox)
            .WidthOverride(400)
            .HeightOverride(30)
            [
                SAssignNew(BarOverlay, SOverlay)
                
                // Background
                + SOverlay::Slot()
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                    .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
                ]
                
                // Zone visualization
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                [
                    SNew(SHorizontalBox)
                    
                    // Soul zone (0-20%)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.2f)
                    [
                        SAssignNew(SoulZone, SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor(0.2f, 0.4f, 0.8f, 0.3f))
                    ]
                    
                    // Transition zone (20-40%)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.2f)
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.2f))
                    ]
                    
                    // Balanced zone (40-60%)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.2f)
                    [
                        SAssignNew(BalancedZone, SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor(0.6f, 0.3f, 0.8f, 0.3f))
                    ]
                    
                    // Transition zone (60-80%)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.2f)
                    [
                        SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 0.2f))
                    ]
                    
                    // Power zone (80-100%)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.2f)
                    [
                        SAssignNew(PowerZone, SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.2f, 0.3f))
                    ]
                ]
                
                // Moving indicator
                + SOverlay::Slot()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Fill)
                [
                    SNew(SBox)
                    .WidthOverride(6)
                    [
                        SAssignNew(Indicator, SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor::White)
                        .RenderTransform(FSlateRenderTransform(FVector2D(197, 0))) // Start at center (400 * 0.5 - 3)
                    ]
                ]
                
                // Stance text overlay
                + SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(this, &STrinityFlowStanceBar::GetStanceText)
                    .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Bold"))
                    .ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.8f))
                    .ShadowOffset(FVector2D(1, 1))
                    .ShadowColorAndOpacity(FLinearColor::Black)
                ]
            ]
        ]
    ];
}

void STrinityFlowStanceBar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Handle fade animations
    if (bFadingIn)
    {
        FadeAlpha = FMath::Min(1.0f, FadeAlpha + InDeltaTime * 2.0f);
        if (FadeAlpha >= 1.0f)
        {
            bFadingIn = false;
        }
    }
    else if (bFadingOut)
    {
        FadeAlpha = FMath::Max(0.0f, FadeAlpha - InDeltaTime * 2.0f);
        if (FadeAlpha <= 0.0f)
        {
            bFadingOut = false;
            BackgroundBar->SetVisibility(EVisibility::Collapsed);
        }
    }
    
    if (BackgroundBar.IsValid())
    {
        BackgroundBar->SetRenderOpacity(FadeAlpha);
    }
    
    // Smoothly interpolate indicator position
    CurrentPosition = FMath::FInterpTo(CurrentPosition, TargetPosition, InDeltaTime, 10.0f);
    
    // Update indicator position
    if (Indicator.IsValid())
    {
        float IndicatorX = CurrentPosition * 400.0f - 3.0f; // Bar width * position - half indicator width
        Indicator->SetRenderTransform(FSlateRenderTransform(FVector2D(IndicatorX, 0)));
    }
    
    // Update zone highlighting based on position
    if (SoulZone.IsValid() && BalancedZone.IsValid() && PowerZone.IsValid())
    {
        float BaseAlpha = 0.3f;
        float HighlightAlpha = 0.6f;
        
        if (CurrentPosition <= 0.2f)
        {
            // Soul stance
            SoulZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.4f, 0.8f, HighlightAlpha));
            BalancedZone->SetBorderBackgroundColor(FLinearColor(0.6f, 0.3f, 0.8f, BaseAlpha));
            PowerZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.2f, BaseAlpha));
        }
        else if (CurrentPosition >= 0.4f && CurrentPosition <= 0.6f)
        {
            // Balanced stance
            SoulZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.4f, 0.8f, BaseAlpha));
            BalancedZone->SetBorderBackgroundColor(FLinearColor(0.6f, 0.3f, 0.8f, HighlightAlpha));
            PowerZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.2f, BaseAlpha));
        }
        else if (CurrentPosition >= 0.8f)
        {
            // Power stance
            SoulZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.4f, 0.8f, BaseAlpha));
            BalancedZone->SetBorderBackgroundColor(FLinearColor(0.6f, 0.3f, 0.8f, BaseAlpha));
            PowerZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.2f, HighlightAlpha));
        }
        else
        {
            // Transition zones
            SoulZone->SetBorderBackgroundColor(FLinearColor(0.2f, 0.4f, 0.8f, BaseAlpha));
            BalancedZone->SetBorderBackgroundColor(FLinearColor(0.6f, 0.3f, 0.8f, BaseAlpha));
            PowerZone->SetBorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.2f, BaseAlpha));
        }
    }
}

void STrinityFlowStanceBar::SetIndicatorPosition(float NewPosition)
{
    TargetPosition = FMath::Clamp(NewPosition, 0.0f, 1.0f);
}

void STrinityFlowStanceBar::Show()
{
    if (!bIsVisible)
    {
        bIsVisible = true;
        bFadingIn = true;
        bFadingOut = false;
        BackgroundBar->SetVisibility(EVisibility::Visible);
    }
}

void STrinityFlowStanceBar::Hide()
{
    if (bIsVisible)
    {
        bIsVisible = false;
        bFadingOut = true;
        bFadingIn = false;
    }
}

FLinearColor STrinityFlowStanceBar::GetZoneColor(float Position) const
{
    if (Position <= 0.2f)
    {
        return FLinearColor(0.2f, 0.4f, 0.8f); // Soul blue
    }
    else if (Position >= 0.4f && Position <= 0.6f)
    {
        return FLinearColor(0.6f, 0.3f, 0.8f); // Balanced purple
    }
    else if (Position >= 0.8f)
    {
        return FLinearColor(0.8f, 0.4f, 0.2f); // Power orange
    }
    else
    {
        return FLinearColor(0.5f, 0.5f, 0.5f); // Transition gray
    }
}

FText STrinityFlowStanceBar::GetStanceText() const
{
    if (CurrentPosition <= 0.2f)
    {
        return LOCTEXT("SoulStance", "SOUL");
    }
    else if (CurrentPosition >= 0.4f && CurrentPosition <= 0.6f)
    {
        return LOCTEXT("BalancedStance", "BALANCED");
    }
    else if (CurrentPosition >= 0.8f)
    {
        return LOCTEXT("PowerStance", "POWER");
    }
    else
    {
        return FText::GetEmpty();
    }
}

#undef LOCTEXT_NAMESPACE