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

private:
    AEnemyBase* TargetEnemy = nullptr;

    TSharedPtr<class STextBlock> NameText;
    TSharedPtr<class SProgressBar> HealthBar;
    TSharedPtr<class STextBlock> StatsText;
};