#include "UI/Slate/STrinityFlowEnemyInfoPanel.h"
#include "Enemy/EnemyBase.h"
#include "Core/HealthComponent.h"
#include "UI/TrinityFlowStyle.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"

void STrinityFlowEnemyInfoPanel::Construct(const FArguments& InArgs)
{
    TargetEnemy = InArgs._Enemy;
    const ISlateStyle* Style = &FTrinityFlowStyle::Get();

    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(200)
        [
            SNew(SVerticalBox)

            // Name
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SAssignNew(NameText, STextBlock)
                .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                .ColorAndOpacity(Style->GetColor("TrinityFlow.Color.Neutral"))
            ]

            // Health Bar
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 5)
            [
                SAssignNew(HealthBar, SProgressBar)
                .Percent(1.0f)
                .FillColorAndOpacity(Style->GetColor("TrinityFlow.Color.Negative"))
                .BorderPadding(FVector2D(0, 0))
            ]

            // Stats
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SAssignNew(StatsText, STextBlock)
                .TextStyle(&Style->GetWidgetStyle<FTextBlockStyle>("TrinityFlow.Font.Regular"))
                .ColorAndOpacity(Style->GetColor("TrinityFlow.Color.Neutral"))
            ]
        ]
    ];

    Update();
}

void STrinityFlowEnemyInfoPanel::Update()
{
    if (!TargetEnemy)
    {
        return;
    }

    NameText->SetText(FText::FromString(TargetEnemy->GetName()));

    if (UHealthComponent* HealthComp = TargetEnemy->FindComponentByClass<UHealthComponent>())
    {
        HealthBar->SetPercent(HealthComp->GetHealthPercentage());
        const FCharacterResources& Resources = HealthComp->GetResources();
        StatsText->SetText(FText::Format(FText::FromString("ATK: {0} | DEF: {1}"), FText::AsNumber(Resources.AttackPoint), FText::AsNumber(Resources.DefencePoint)));
    }
}
