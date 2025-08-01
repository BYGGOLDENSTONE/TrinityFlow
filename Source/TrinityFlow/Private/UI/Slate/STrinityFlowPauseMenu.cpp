#include "UI/Slate/STrinityFlowPauseMenu.h"
#include "UI/TrinityFlowUIManager.h"
#include "TrinityFlowGameMode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "TrinityFlowPauseMenu"

void STrinityFlowPauseMenu::Construct(const FArguments& InArgs)
{
    UIManager = InArgs._UIManager;

    ChildSlot
    [
        SNew(SOverlay)
        // Semi-transparent background
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f))
        ]
        
        // Menu Content
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.95f))
            .Padding(50)
            [
                SNew(SBox)
                .WidthOverride(400.0f)
                [
                    SNew(SVerticalBox)
                    // Title
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 0, 0, 40)
                    .HAlign(HAlign_Center)
                    [
                        SAssignNew(TitleText, STextBlock)
                        .Text(LOCTEXT("PauseTitle", "PAUSED"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 48))
                        .ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f)))
                        .ShadowOffset(FVector2D(2, 2))
                        .ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    
                    // Buttons
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SAssignNew(ButtonContainer, SVerticalBox)
                        
                        // Resume Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(50)
                            [
                                SAssignNew(ResumeButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowPauseMenu::OnResumeClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Resume", "Resume"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                            ]
                        ]
                        
                        // Settings Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(50)
                            [
                                SAssignNew(SettingsButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowPauseMenu::OnSettingsClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Settings", "Settings"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                            ]
                        ]
                        
                        // Main Menu Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(50)
                            [
                                SAssignNew(MainMenuButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowPauseMenu::OnMainMenuClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("MainMenu", "Main Menu"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                            ]
                        ]
                        
                        // Quit Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(50)
                            [
                                SAssignNew(QuitButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowPauseMenu::OnQuitClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("QuitToDesktop", "Quit to Desktop"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ];
    
    // Set up navigable buttons array
    NavigableButtons.Empty();
    NavigableButtons.Add(ResumeButton);
    NavigableButtons.Add(SettingsButton);
    NavigableButtons.Add(MainMenuButton);
    NavigableButtons.Add(QuitButton);
    
    // Select first button by default
    SelectButton(0);
}

FReply STrinityFlowPauseMenu::OnResumeClicked()
{
    if (UIManager)
    {
        // Resume the game
        UGameplayStatics::SetGamePaused(UIManager->GetGameInstance()->GetWorld(), false);
        UIManager->ShowInGameHUD();
    }
    
    return FReply::Handled();
}

FReply STrinityFlowPauseMenu::OnSettingsClicked()
{
    // TODO: Show settings menu
    UE_LOG(LogTemp, Log, TEXT("Settings clicked - not yet implemented"));
    return FReply::Handled();
}

FReply STrinityFlowPauseMenu::OnMainMenuClicked()
{
    if (UIManager)
    {
        // Restart the game which will reload the level and show main menu
        if (ATrinityFlowGameMode* GameMode = Cast<ATrinityFlowGameMode>(UGameplayStatics::GetGameMode(UIManager->GetGameInstance()->GetWorld())))
        {
            GameMode->RestartGame();
        }
    }
    
    return FReply::Handled();
}

FReply STrinityFlowPauseMenu::OnQuitClicked()
{
    // Quit the game
    UKismetSystemLibrary::QuitGame(UIManager->GetGameInstance()->GetWorld(), nullptr, EQuitPreference::Quit, false);
    return FReply::Handled();
}

FReply STrinityFlowPauseMenu::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();
    
    // Navigate up with W or Up Arrow
    if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up)
    {
        SelectButton(SelectedButtonIndex - 1);
        return FReply::Handled();
    }
    // Navigate down with S or Down Arrow
    else if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down)
    {
        SelectButton(SelectedButtonIndex + 1);
        return FReply::Handled();
    }
    // Activate with E, Enter, or Gamepad A
    else if (Key == EKeys::E || Key == EKeys::Enter || Key == EKeys::Gamepad_FaceButton_Bottom)
    {
        ActivateSelectedButton();
        return FReply::Handled();
    }
    // Also allow Escape to resume the game
    else if (Key == EKeys::Escape)
    {
        OnResumeClicked();
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

void STrinityFlowPauseMenu::SelectButton(int32 Index)
{
    // Wrap around the selection
    if (Index < 0)
    {
        Index = NavigableButtons.Num() - 1;
    }
    else if (Index >= NavigableButtons.Num())
    {
        Index = 0;
    }
    
    SelectedButtonIndex = Index;
    UpdateButtonSelection();
}

void STrinityFlowPauseMenu::UpdateButtonSelection()
{
    // Update visual state of all buttons
    for (int32 i = 0; i < NavigableButtons.Num(); i++)
    {
        if (NavigableButtons[i].IsValid())
        {
            if (i == SelectedButtonIndex)
            {
                // Highlight selected button
                NavigableButtons[i]->SetBorderBackgroundColor(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f));
                
                // Set keyboard focus to this button
                FSlateApplication::Get().SetKeyboardFocus(NavigableButtons[i]);
            }
            else
            {
                // Reset non-selected buttons
                NavigableButtons[i]->SetBorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));
            }
        }
    }
}

void STrinityFlowPauseMenu::ActivateSelectedButton()
{
    if (SelectedButtonIndex >= 0 && SelectedButtonIndex < NavigableButtons.Num())
    {
        if (NavigableButtons[SelectedButtonIndex].IsValid())
        {
            // Trigger the appropriate callback based on which button is selected
            switch (SelectedButtonIndex)
            {
                case 0: // Resume
                    OnResumeClicked();
                    break;
                case 1: // Settings
                    OnSettingsClicked();
                    break;
                case 2: // Main Menu
                    OnMainMenuClicked();
                    break;
                case 3: // Quit
                    OnQuitClicked();
                    break;
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE