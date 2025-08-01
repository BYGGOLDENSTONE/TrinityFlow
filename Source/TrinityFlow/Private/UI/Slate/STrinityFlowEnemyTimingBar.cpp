#include "UI/Slate/STrinityFlowEnemyTimingBar.h"
#include "Enemy/EnemyBase.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Styling/CoreStyle.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

void STrinityFlowEnemyTimingBar::Construct(const FArguments& InArgs)
{
    Enemy = InArgs._Enemy;
    
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(200)
        .HeightOverride(20)
        .Visibility(EVisibility::Collapsed)
        [
            SAssignNew(BackgroundBar, SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f))
        ]
    ];
}

void STrinityFlowEnemyTimingBar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Update timing if active
    if (bIsActive)
    {
        CurrentTime += InDeltaTime;
        
        if (CurrentTime >= TotalDuration)
        {
            // Time's up
            StopTiming();
        }
    }
}

int32 STrinityFlowEnemyTimingBar::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    if (!bIsActive || !Enemy || !IsValid(Enemy))
    {
        return LayerId;
    }
    
    // Get player controller for projection
    APlayerController* PC = GEngine->GetFirstLocalPlayerController(Enemy->GetWorld());
    if (!PC)
    {
        return LayerId;
    }
    
    // Project enemy location to screen (above their head)
    FVector WorldLocation = Enemy->GetActorLocation() + FVector(0, 0, 250.0f); // 250 units above enemy
    FVector2D ScreenLocation;
    
    if (!PC->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
    {
        return LayerId;
    }
    
    // Draw timing bar background
    float BarWidth = 200.0f;
    float BarHeight = 12.0f;
    float BarX = ScreenLocation.X - BarWidth * 0.5f;
    float BarY = ScreenLocation.Y;
    
    // Background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(BarWidth, BarHeight), FSlateLayoutTransform(FVector2f(BarX, BarY))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.1f, 0.1f, 0.1f, 0.8f)
    );
    
    // Draw zones
    float Progress = CurrentTime / TotalDuration;
    
    // Too early zone (0 - 0.33)
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(BarWidth * 0.33f, BarHeight - 2), FSlateLayoutTransform(FVector2f(BarX + 1, BarY + 1))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.2f, 0.2f, 0.3f)
    );
    
    // Moderate zone (0.33 - 0.67)
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(BarWidth * 0.34f, BarHeight - 2), FSlateLayoutTransform(FVector2f(BarX + 1 + BarWidth * 0.33f, BarY + 1))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.8f, 0.2f, 0.3f)
    );
    
    // Perfect zone (0.67 - 1.0)
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(BarWidth * 0.33f, BarHeight - 2), FSlateLayoutTransform(FVector2f(BarX + 1 + BarWidth * 0.67f, BarY + 1))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.2f, 0.8f, 0.2f, 0.3f)
    );
    
    // Moving cursor
    float CursorX = BarX + (Progress * BarWidth);
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(3, BarHeight), FSlateLayoutTransform(FVector2f(CursorX - 1.5f, BarY))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor::White
    );
    
    // Highlight current zone
    FLinearColor HighlightColor;
    if (Progress < 0.33f)
    {
        HighlightColor = FLinearColor(0.8f, 0.2f, 0.2f, 0.5f); // Red
    }
    else if (Progress < 0.67f)
    {
        HighlightColor = FLinearColor(0.8f, 0.8f, 0.2f, 0.5f); // Yellow
    }
    else
    {
        HighlightColor = FLinearColor(0.2f, 0.8f, 0.2f, 0.5f); // Green
    }
    
    // Draw highlight pulse on current zone
    float PulseAlpha = 0.3f + 0.2f * FMath::Sin(CurrentTime * 10.0f);
    HighlightColor.A = PulseAlpha;
    
    float HighlightX, HighlightWidth;
    if (Progress < 0.33f)
    {
        HighlightX = BarX;
        HighlightWidth = BarWidth * 0.33f;
    }
    else if (Progress < 0.67f)
    {
        HighlightX = BarX + BarWidth * 0.33f;
        HighlightWidth = BarWidth * 0.34f;
    }
    else
    {
        HighlightX = BarX + BarWidth * 0.67f;
        HighlightWidth = BarWidth * 0.33f;
    }
    
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2f(HighlightWidth, BarHeight), FSlateLayoutTransform(FVector2f(HighlightX, BarY))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        HighlightColor
    );
    
    return LayerId;
}

void STrinityFlowEnemyTimingBar::StartTiming(float Duration, float PerfectStart, float PerfectEnd)
{
    bIsActive = true;
    CurrentTime = 0.0f;
    TotalDuration = Duration;
    PerfectStartTime = PerfectStart;
    PerfectEndTime = PerfectEnd;
    
    SetVisibility(EVisibility::HitTestInvisible);
}

void STrinityFlowEnemyTimingBar::StopTiming()
{
    bIsActive = false;
    SetVisibility(EVisibility::Collapsed);
}

void STrinityFlowEnemyTimingBar::Update()
{
    // Position is updated in OnPaint based on enemy location
}