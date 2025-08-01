#include "UI/Slate/STrinityFlowMainMenu.h"
#include "UI/TrinityFlowUIManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowMainMenu"

void STrinityFlowMainMenu::Construct(const FArguments& InArgs)
{
    UIManager = InArgs._UIManager;

    // Create fade-in animation
    FadeInAnimation = FCurveSequence();
    FadeInCurve = FadeInAnimation.AddCurve(0.f, 0.5f, ECurveEaseFunction::QuadOut);

    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        .Padding(0)
        [
            SNew(SOverlay)
            // Background
            + SOverlay::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SImage)
                .Image(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                .ColorAndOpacity(FLinearColor(0.02f, 0.02f, 0.02f, 0.9f))
            ]
            
            // Content
            + SOverlay::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SNew(SBox)
                .WidthOverride(600.0f)
                [
                    SNew(SVerticalBox)
                    // Title
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 0, 0, 60)
                    .HAlign(HAlign_Center)
                    [
                        SAssignNew(TitleText, STextBlock)
                        .Text(LOCTEXT("GameTitle", "TRINITY FLOW"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 72))
                        .ColorAndOpacity(FSlateColor(FLinearColor(0.0f, 0.5f, 1.0f)))
                        .ShadowOffset(FVector2D(2, 2))
                        .ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    
                    // Buttons
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SAssignNew(ButtonContainer, SVerticalBox)
                        
                        // Start Game Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(60)
                            [
                                SAssignNew(StartButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowMainMenu::OnStartGameClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("StartGame", "Start Game"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                    .ShadowOffset(FVector2D(1, 1))
                                    .ShadowColorAndOpacity(FLinearColor::Black)
                                ]
                            ]
                        ]
                        
                        // Settings Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(60)
                            [
                                SAssignNew(SettingsButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowMainMenu::OnSettingsClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Settings", "Settings"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                    .ShadowOffset(FVector2D(1, 1))
                                    .ShadowColorAndOpacity(FLinearColor::Black)
                                ]
                            ]
                        ]
                        
                        // Quit Button
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(0, 10)
                        [
                            SNew(SBox)
                            .HeightOverride(60)
                            [
                                SAssignNew(QuitButton, SButton)
                                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                .OnClicked(this, &STrinityFlowMainMenu::OnQuitClicked)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("Quit", "Quit Game"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                    .ShadowOffset(FVector2D(1, 1))
                                    .ShadowColorAndOpacity(FLinearColor::Black)
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
    NavigableButtons.Add(StartButton);
    NavigableButtons.Add(SettingsButton);
    NavigableButtons.Add(QuitButton);
    
    // Select first button by default
    SelectButton(0);
    
    PlayFadeInAnimation();
}

FReply STrinityFlowMainMenu::OnStartGameClicked()
{
    if (UIManager)
    {
        // We're already in the game level, just transition to gameplay
        UIManager->ShowInGameHUD();
        
        // Unpause in case the game was paused
        UGameplayStatics::SetGamePaused(UIManager->GetGameInstance()->GetWorld(), false);
    }
    
    return FReply::Handled();
}

FReply STrinityFlowMainMenu::OnSettingsClicked()
{
    // TODO: Show settings menu
    UE_LOG(LogTemp, Log, TEXT("Settings clicked - not yet implemented"));
    return FReply::Handled();
}

FReply STrinityFlowMainMenu::OnQuitClicked()
{
    // Quit the game
    UKismetSystemLibrary::QuitGame(UIManager->GetGameInstance()->GetWorld(), nullptr, EQuitPreference::Quit, false);
    return FReply::Handled();
}

void STrinityFlowMainMenu::PlayFadeInAnimation()
{
    if (!FadeInAnimation.IsPlaying())
    {
        FadeInAnimation.Play(this->AsShared());
    }
}

FReply STrinityFlowMainMenu::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
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
    
    return FReply::Unhandled();
}

void STrinityFlowMainMenu::SelectButton(int32 Index)
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

void STrinityFlowMainMenu::UpdateButtonSelection()
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

void STrinityFlowMainMenu::ActivateSelectedButton()
{
    if (SelectedButtonIndex >= 0 && SelectedButtonIndex < NavigableButtons.Num())
    {
        if (NavigableButtons[SelectedButtonIndex].IsValid())
        {
            // Trigger the appropriate callback based on which button is selected
            switch (SelectedButtonIndex)
            {
                case 0: // Start Game
                    OnStartGameClicked();
                    break;
                case 1: // Settings
                    OnSettingsClicked();
                    break;
                case 2: // Quit
                    OnQuitClicked();
                    break;
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE