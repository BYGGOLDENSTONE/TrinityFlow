#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Health Bar Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowHealthBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowHealthBar) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    void SetHealthPercentage(float NewHealthPercentage);
    void PlayDamageAnimation();

private:
    // Current and target health percentages
    float CurrentHealthPercentage = 1.0f;
    float TargetHealthPercentage = 1.0f;
    
    // Animation
    float DamageFlashAlpha = 0.0f;
    bool bPlayingDamageAnimation = false;
    
    // Widget References
    TSharedPtr<class SProgressBar> HealthProgressBar;
    TSharedPtr<class STextBlock> HealthText;
    TSharedPtr<class SBorder> DamageFlashBorder;
};