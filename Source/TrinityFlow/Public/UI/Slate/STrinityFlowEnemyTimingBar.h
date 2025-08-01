#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Simple timing bar that appears above enemy when they attack
 */
class TRINITYFLOW_API STrinityFlowEnemyTimingBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowEnemyTimingBar) {}
        SLATE_ARGUMENT(class AEnemyBase*, Enemy)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    // Start showing the timing bar
    void StartTiming(float Duration, float PerfectStart, float PerfectEnd);
    
    // Stop and hide the timing bar
    void StopTiming();
    
    // Update position based on enemy location
    void Update();
    
    // Check if timing bar is active
    bool IsActive() const { return bIsActive; }

private:
    // Enemy reference
    class AEnemyBase* Enemy = nullptr;
    
    // Widget references
    TSharedPtr<class SBorder> BackgroundBar;
    TSharedPtr<class SBorder> TimingCursor;
    
    // Timing state
    bool bIsActive = false;
    float CurrentTime = 0.0f;
    float TotalDuration = 1.5f;
    float PerfectStartTime = 1.0f;
    float PerfectEndTime = 1.5f;
    
    // Screen position
    mutable FVector2D ScreenPosition;
};