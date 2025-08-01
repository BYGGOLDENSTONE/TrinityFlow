#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Defense timing bar widget that shows perfect/moderate/failure zones
 */
class TRINITYFLOW_API STrinityFlowDefenseTimingBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowDefenseTimingBar) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    // Start showing the timing bar
    void StartTiming(float Duration, float PerfectStart, float PerfectEnd);
    
    // Stop and hide the timing bar
    void StopTiming();
    
    // Check if timing bar is active
    bool IsActive() const { return bIsActive; }
    
    // Get current timing zone
    enum class ETimingZone : uint8
    {
        None,
        Failure,
        Moderate,
        Perfect
    };
    ETimingZone GetCurrentZone() const;

private:
    // Widget references
    TSharedPtr<class SBorder> BackgroundBorder;
    TSharedPtr<class SOverlay> TimingOverlay;
    TSharedPtr<class SBorder> FailureZone;
    TSharedPtr<class SBorder> ModerateZone;
    TSharedPtr<class SBorder> PerfectZone;
    TSharedPtr<class SBorder> TimingCursor;
    TSharedPtr<class STextBlock> InstructionText;
    TSharedPtr<class STextBlock> ZoneText;
    
    // Timing state
    bool bIsActive = false;
    float CurrentTime = 0.0f;
    float TotalDuration = 1.5f;
    float PerfectStartTime = 0.5f;
    float PerfectEndTime = 1.0f;
    
    // Animation
    float FadeAlpha = 0.0f;
    bool bFadingIn = false;
    bool bFadingOut = false;
    
    void UpdateTimingDisplay();
    FLinearColor GetZoneColor(ETimingZone Zone) const;
    FText GetZoneText(ETimingZone Zone) const;
};