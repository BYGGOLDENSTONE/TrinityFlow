#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Stance flow bar that shows current stance position based on attack patterns
 */
class TRINITYFLOW_API STrinityFlowStanceBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowStanceBar) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    // Update indicator position (0.0 = full left/soul, 1.0 = full right/power)
    void SetIndicatorPosition(float NewPosition);
    
    // Show/hide the bar
    void Show();
    void Hide();
    
    // Check if bar is visible
    bool IsBarVisible() const { return bIsVisible; }

private:
    // Widget references
    TSharedPtr<class SBorder> BackgroundBar;
    TSharedPtr<class SBorder> SoulZone;
    TSharedPtr<class SBorder> BalancedZone;
    TSharedPtr<class SBorder> PowerZone;
    TSharedPtr<class SBorder> Indicator;
    TSharedPtr<class SOverlay> BarOverlay;
    
    // State
    float CurrentPosition = 0.5f; // Start at center
    float TargetPosition = 0.5f;
    bool bIsVisible = false;
    float FadeAlpha = 0.0f;
    
    // Animation
    bool bFadingIn = false;
    bool bFadingOut = false;
    
    // Get color for current position
    FLinearColor GetZoneColor(float Position) const;
    FText GetStanceText() const;
};