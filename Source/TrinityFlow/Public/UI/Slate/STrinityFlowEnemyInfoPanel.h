#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AEnemyBase;

class STrinityFlowEnemyInfoPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowEnemyInfoPanel) {}
        SLATE_ARGUMENT(AEnemyBase*, Enemy)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void Update();
    
    // Timing bar control
    void ShowTimingBar(float Duration, float PerfectStart, float PerfectEnd);
    void HideTimingBar();

private:
    AEnemyBase* TargetEnemy = nullptr;

    TSharedPtr<class STextBlock> NameText;
    TSharedPtr<class SProgressBar> HealthBar;
    TSharedPtr<class STextBlock> StatsText;
    TSharedPtr<class STrinityFlowDefenseTimingBar> TimingBar;
};