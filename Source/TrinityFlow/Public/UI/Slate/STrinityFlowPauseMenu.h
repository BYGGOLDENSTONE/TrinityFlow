#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UTrinityFlowUIManager;

/**
 * Pause Menu Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowPauseMenu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowPauseMenu) {}
        SLATE_ARGUMENT(UTrinityFlowUIManager*, UIManager)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    
    // Input handling
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
    // Button Callbacks
    FReply OnResumeClicked();
    FReply OnSettingsClicked();
    FReply OnMainMenuClicked();
    FReply OnQuitClicked();

    // Widget References
    TSharedPtr<class STextBlock> TitleText;
    TSharedPtr<class SVerticalBox> ButtonContainer;
    TSharedPtr<class SButton> ResumeButton;
    TSharedPtr<class SButton> SettingsButton;
    TSharedPtr<class SButton> MainMenuButton;
    TSharedPtr<class SButton> QuitButton;

    // UI Manager reference
    UTrinityFlowUIManager* UIManager = nullptr;
    
    // Keyboard navigation
    int32 SelectedButtonIndex = 0;
    TArray<TSharedPtr<SButton>> NavigableButtons;
    void UpdateButtonSelection();
    void SelectButton(int32 Index);
    void ActivateSelectedButton();
};