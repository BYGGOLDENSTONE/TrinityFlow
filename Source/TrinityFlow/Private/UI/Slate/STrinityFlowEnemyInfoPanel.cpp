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
                SNew(SBox)
                .HeightOverride(8)
                [
                    SAssignNew(HealthBar, SProgressBar)
                    .Percent(1.0f)
                    .BarFillType(EProgressBarFillType::LeftToRight)
                    .FillColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                    .FillImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                ]
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
        float HealthPercent = HealthComp->GetHealthPercentage();
        HealthBar->SetPercent(HealthPercent);
        
        // Hide health bar if health is 0
        if (HealthPercent <= 0.0f)
        {
            HealthBar->SetVisibility(EVisibility::Hidden);
        }
        else
        {
            HealthBar->SetVisibility(EVisibility::Visible);
        }
        
        const FCharacterResources& Resources = HealthComp->GetResources();
        StatsText->SetText(FText::Format(FText::FromString("ATK: {0} | DEF: {1}"), FText::AsNumber(Resources.AttackPoint), FText::AsNumber(Resources.DefencePoint)));
    }
}
