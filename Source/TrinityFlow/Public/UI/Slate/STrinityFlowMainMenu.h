#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UTrinityFlowUIManager;

/**
 * Main Menu Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowMainMenu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowMainMenu) {}
        SLATE_ARGUMENT(UTrinityFlowUIManager*, UIManager)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    
    // Input handling
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
    // Button Callbacks
    FReply OnStartGameClicked();
    FReply OnSettingsClicked();
    FReply OnQuitClicked();

    // Animation
    void PlayFadeInAnimation();
    
    // Widget References
    TSharedPtr<class STextBlock> TitleText;
    TSharedPtr<class SVerticalBox> ButtonContainer;
    TSharedPtr<class SButton> StartButton;
    TSharedPtr<class SButton> SettingsButton;
    TSharedPtr<class SButton> QuitButton;

    // UI Manager reference
    UTrinityFlowUIManager* UIManager = nullptr;

    // Animation
    FCurveSequence FadeInAnimation;
    FCurveHandle FadeInCurve;

    // Styles
    FButtonStyle* GetButtonStyle() const;
    FTextBlockStyle* GetTitleStyle() const;
    
    // Keyboard navigation
    int32 SelectedButtonIndex = 0;
    TArray<TSharedPtr<SButton>> NavigableButtons;
    void UpdateButtonSelection();
    void SelectButton(int32 Index);
    void ActivateSelectedButton();
};