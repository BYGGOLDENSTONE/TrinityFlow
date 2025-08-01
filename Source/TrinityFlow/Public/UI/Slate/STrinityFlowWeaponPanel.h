#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Weapon Panel Widget for TrinityFlow
 * Displays weapon info and ability cooldowns
 */
class TRINITYFLOW_API STrinityFlowWeaponPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowWeaponPanel) {}
        SLATE_ARGUMENT(FText, WeaponName)
        SLATE_ARGUMENT(FLinearColor, WeaponColor)
        SLATE_ARGUMENT(FText, AbilityKey1)
        SLATE_ARGUMENT(FText, AbilityKey2)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetCooldowns(float Cooldown1, float Cooldown2);

private:
    // Widget References
    TSharedPtr<class STextBlock> Ability1Text;
    TSharedPtr<class STextBlock> Ability2Text;
    TSharedPtr<class SProgressBar> Ability1CooldownBar;
    TSharedPtr<class SProgressBar> Ability2CooldownBar;
    
    // Cooldown values
    float Ability1Cooldown = 0.0f;
    float Ability2Cooldown = 0.0f;
};