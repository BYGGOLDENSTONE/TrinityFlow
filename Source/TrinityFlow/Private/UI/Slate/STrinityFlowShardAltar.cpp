#include "UI/Slate/STrinityFlowShardAltar.h"
#include "UI/TrinityFlowUIManager.h"
#include "World/ShardAltar.h"
#include "Core/ShardComponent.h"
#include "../../../TrinityFlowCharacter.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "TrinityFlowShardAltar"

void STrinityFlowShardAltar::Construct(const FArguments& InArgs)
{
    UIManager = InArgs._UIManager;
    
    // Create animation
    OpenAnimation = FCurveSequence();
    OpenCurve = OpenAnimation.AddCurve(0.f, 0.5f, ECurveEaseFunction::QuadOut);

    ChildSlot
    [
        SNew(SOverlay)
        + SOverlay::Slot() // Background overlay
        [
            SAssignNew(BackgroundBorder, SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f))
            .RenderOpacity(0.0f)
        ]
        
        // Main panel
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(MainPanel, SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.95f))
            .Padding(40)
            .RenderTransform(FSlateRenderTransform(FScale2D(1.0f)))
            .RenderTransformPivot(FVector2D(0.5f, 0.5f))
            .RenderOpacity(0.0f)
            [
                SNew(SBox)
                .WidthOverride(800)
                .HeightOverride(600)
                [
                    SNew(SVerticalBox)
                    
                    // Title
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(0, 0, 0, 30)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("AltarTitle", "SHARD ACTIVATION ALTAR"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 36))
                        .ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
                        .ShadowOffset(FVector2D(2, 2))
                        .ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    
                    // Instructions
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(0, 0, 0, 30)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("Instructions", "A/D: Select Type | W/S: Adjust Amount | E: Activate | Q: Cancel"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    ]
                    
                    // Shard panels
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    [
                        SNew(SHorizontalBox)
                        
                        // Soul panel
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(10)
                        [
                            SAssignNew(SoulPanel, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                            .BorderBackgroundColor(FSlateColor(FLinearColor(0.2f, 0.4f, 0.8f, 0.8f)))
                            .Padding(20)
                            [
                                SNew(SVerticalBox)
                                
                                // Soul title
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                .Padding(0, 0, 0, 20)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("SoulShards", "SOUL SHARDS"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
                                    .ColorAndOpacity(FSlateColor(FLinearColor(0.0f, 0.5f, 1.0f)))
                                ]
                                
                                // Soul count
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                [
                                    SAssignNew(SoulAvailableText, STextBlock)
                                    .Text(FText::Format(LOCTEXT("AvailableFormat", "Available: {0}"), 0))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 18))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::Gray))
                                ]
                                
                                // Soul amount
                                + SVerticalBox::Slot()
                                .FillHeight(1.0f)
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SAssignNew(SoulAmountText, STextBlock)
                                    .Text(FText::AsNumber(0))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 72))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                                
                                // W/S indicators
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                [
                                    SAssignNew(SoulControlsText, STextBlock)
                                    .Text(LOCTEXT("WSControls", "W ↑  S ↓"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
                                ]
                            ]
                        ]
                        
                        // Power panel
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        .Padding(10)
                        [
                            SAssignNew(PowerPanel, SBorder)
                            .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                            .BorderBackgroundColor(FSlateColor(FLinearColor(0.4f, 0.2f, 0.1f, 0.5f)))
                            .Padding(20)
                            [
                                SNew(SVerticalBox)
                                
                                // Power title
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                .Padding(0, 0, 0, 20)
                                [
                                    SNew(STextBlock)
                                    .Text(LOCTEXT("PowerShards", "POWER SHARDS"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
                                    .ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.5f, 0.0f)))
                                ]
                                
                                // Power count
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                [
                                    SAssignNew(PowerAvailableText, STextBlock)
                                    .Text(FText::Format(LOCTEXT("AvailableFormat", "Available: {0}"), 0))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 18))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::Gray))
                                ]
                                
                                // Power amount
                                + SVerticalBox::Slot()
                                .FillHeight(1.0f)
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SAssignNew(PowerAmountText, STextBlock)
                                    .Text(FText::AsNumber(0))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 72))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::White))
                                ]
                                
                                // W/S indicators
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Center)
                                [
                                    SAssignNew(PowerControlsText, STextBlock)
                                    .Text(LOCTEXT("WSControls", "W ↑  S ↓"))
                                    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16))
                                    .ColorAndOpacity(FSlateColor(FLinearColor::Gray))
                                ]
                            ]
                        ]
                    ]
                    
                    // Damage bonus preview
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(0, 30, 0, 0)
                    [
                        SAssignNew(StancePreviewText, STextBlock)
                        .Text(LOCTEXT("DamageBonus", "Damage Bonus: +0%"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
                        .ColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.2f)))
                        .ShadowOffset(FVector2D(1, 1))
                        .ShadowColorAndOpacity(FLinearColor::Black)
                    ]
                    
                    // Activation prompt
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(0, 20, 0, 0)
                    [
                        SAssignNew(ActivationPromptText, STextBlock)
                        .Text(LOCTEXT("SelectShards", "Select shards to activate"))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 18))
                        .ColorAndOpacity(FSlateColor(FLinearColor::Gray))
                    ]
                ]
            ]
        ]
        
        // Progress overlay
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(ProgressOverlay, SOverlay)
            .Visibility(EVisibility::Collapsed)
            
            + SOverlay::Slot()
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
                .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f))
                .Padding(50)
                [
                    SNew(SVerticalBox)
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(0, 0, 0, 20)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("Activating", "Activating Shards..."))
                        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24))
                        .ColorAndOpacity(FSlateColor(FLinearColor::White))
                    ]
                    
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBox)
                        .WidthOverride(400)
                        .HeightOverride(30)
                        [
                            SAssignNew(ActivationProgressBar, SProgressBar)
                            .Percent(0.0f)
                            .FillColorAndOpacity(FLinearColor::Green)
                        ]
                    ]
                ]
            ]
        ]
    ];
}

void STrinityFlowShardAltar::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Update animation
    if (OpenAnimation.IsPlaying())
    {
        AnimationAlpha = OpenCurve.GetLerp();
        
        // Update widget opacities
        if (BackgroundBorder.IsValid())
        {
            BackgroundBorder->SetRenderOpacity(AnimationAlpha);
        }
        if (MainPanel.IsValid())
        {
            MainPanel->SetRenderOpacity(AnimationAlpha);
            float Scale = FMath::Lerp(0.8f, 1.0f, AnimationAlpha);
            MainPanel->SetRenderTransform(FSlateRenderTransform(FScale2D(Scale)));
        }
    }
    
    // Update activation progress
    if (bIsActivating && CurrentAltar)
    {
        float Progress = CurrentAltar->GetActivationProgress();
        UpdateActivationProgress(Progress);
        
        // Check if activation is complete or if altar is no longer activating
        if (Progress >= 1.0f || !CurrentAltar->IsActivating())
        {
            bIsActivating = false;
            ActivationProgress = 0.0f;
            
            if (ProgressOverlay.IsValid())
            {
                ProgressOverlay->SetVisibility(EVisibility::Collapsed);
            }
            
            // Close the UI after a short delay to show completion
            if (Progress >= 1.0f)
            {
                FTimerHandle CloseTimer;
                if (UIManager && UIManager->GetWorld())
                {
                    UIManager->GetWorld()->GetTimerManager().SetTimer(CloseTimer, [this]()
                    {
                        if (UIManager)
                        {
                            UIManager->ShowInGameHUD();
                        }
                    }, 0.5f, false);
                }
            }
        }
    }
}

void STrinityFlowShardAltar::SetAltar(AShardAltar* InAltar)
{
    CurrentAltar = InAltar;
    
    if (!CurrentAltar)
    {
        return;
    }
    
    // Get player character
    ATrinityFlowCharacter* PlayerCharacter = Cast<ATrinityFlowCharacter>(
        UGameplayStatics::GetPlayerCharacter(CurrentAltar->GetWorld(), 0));
    
    if (!PlayerCharacter)
    {
        return;
    }
    
    // Get shard component
    UShardComponent* ShardComp = PlayerCharacter->GetShardComponent();
    if (!ShardComp)
    {
        return;
    }
    
    // Get available shards
    MaxSoulAmount = ShardComp->GetInactiveShardCount(EShardType::Soul);
    MaxPowerAmount = ShardComp->GetInactiveShardCount(EShardType::Power);
    
    // Reset amounts
    SoulAmount = 0;
    PowerAmount = 0;
    bSelectingSoul = true;
    
    UpdateShardDisplay();
    UpdateStancePreview();
    PlayOpenAnimation();
}

void STrinityFlowShardAltar::PlayOpenAnimation()
{
    if (!OpenAnimation.IsPlaying())
    {
        AnimationAlpha = 0.0f;
        OpenAnimation.Play(this->AsShared());
    }
}

void STrinityFlowShardAltar::PlayCloseAnimation()
{
    // For now, just close immediately
    // TODO: Implement close animation
    AnimationAlpha = 0.0f;
}

FReply STrinityFlowShardAltar::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    HandleInput(InKeyEvent.GetKey());
    return FReply::Handled();
}

void STrinityFlowShardAltar::HandleInput(const FKey& Key)
{
    if (bIsActivating)
    {
        return; // No input during activation
    }
    
    // Navigation
    if (Key == EKeys::A || Key == EKeys::Gamepad_DPad_Left)
    {
        bSelectingSoul = true;
        UpdateSelectionDisplay();
    }
    else if (Key == EKeys::D || Key == EKeys::Gamepad_DPad_Right)
    {
        bSelectingSoul = false;
        UpdateSelectionDisplay();
    }
    // Amount adjustment
    else if (Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up)
    {
        if (bSelectingSoul)
        {
            SoulAmount = FMath::Clamp(SoulAmount + 1, 0, MaxSoulAmount);
        }
        else
        {
            PowerAmount = FMath::Clamp(PowerAmount + 1, 0, MaxPowerAmount);
        }
        UpdateShardDisplay();
        UpdateStancePreview();
    }
    else if (Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down)
    {
        if (bSelectingSoul)
        {
            SoulAmount = FMath::Clamp(SoulAmount - 1, 0, MaxSoulAmount);
        }
        else
        {
            PowerAmount = FMath::Clamp(PowerAmount - 1, 0, MaxPowerAmount);
        }
        UpdateShardDisplay();
        UpdateStancePreview();
    }
    // Activation
    else if (Key == EKeys::E || Key == EKeys::Gamepad_FaceButton_Top)
    {
        if (SoulAmount > 0 || PowerAmount > 0)
        {
            StartActivation();
        }
    }
    // Cancel
    else if (Key == EKeys::Q || Key == EKeys::Gamepad_FaceButton_Right)
    {
        if (UIManager)
        {
            UIManager->ShowInGameHUD();
        }
    }
}

void STrinityFlowShardAltar::UpdateShardDisplay()
{
    if (SoulAmountText.IsValid())
    {
        SoulAmountText->SetText(FText::AsNumber(SoulAmount));
    }
    
    if (PowerAmountText.IsValid())
    {
        PowerAmountText->SetText(FText::AsNumber(PowerAmount));
    }
    
    if (SoulAvailableText.IsValid())
    {
        SoulAvailableText->SetText(FText::Format(LOCTEXT("AvailableFormat", "Available: {0}"), MaxSoulAmount));
    }
    
    if (PowerAvailableText.IsValid())
    {
        PowerAvailableText->SetText(FText::Format(LOCTEXT("AvailableFormat", "Available: {0}"), MaxPowerAmount));
    }
    
    UpdateSelectionDisplay();
    UpdateActivationDisplay();
}

void STrinityFlowShardAltar::UpdateStancePreview()
{
    if (!StancePreviewText.IsValid())
    {
        return;
    }
    
    // Calculate total damage bonus from selected shards
    float SoulDamageBonus = SoulAmount * 3.0f;
    float PowerDamageBonus = PowerAmount * 3.0f;
    
    if (SoulAmount > 0 && PowerAmount > 0)
    {
        StancePreviewText->SetText(FText::Format(
            LOCTEXT("DamageBonusFormat", "Soul: +{0}%  Physical: +{1}%"), 
            FText::AsNumber((int32)SoulDamageBonus),
            FText::AsNumber((int32)PowerDamageBonus)
        ));
    }
    else if (SoulAmount > 0)
    {
        StancePreviewText->SetText(FText::Format(
            LOCTEXT("SoulBonusFormat", "Soul Damage: +{0}%"), 
            FText::AsNumber((int32)SoulDamageBonus)
        ));
    }
    else if (PowerAmount > 0)
    {
        StancePreviewText->SetText(FText::Format(
            LOCTEXT("PowerBonusFormat", "Physical Damage: +{0}%"), 
            FText::AsNumber((int32)PowerDamageBonus)
        ));
    }
    else
    {
        StancePreviewText->SetText(LOCTEXT("NoBonusFormat", "No Damage Bonus"));
    }
}

void STrinityFlowShardAltar::StartActivation()
{
    if (!CurrentAltar)
    {
        return;
    }
    
    // Get player character
    ATrinityFlowCharacter* PlayerCharacter = Cast<ATrinityFlowCharacter>(
        UGameplayStatics::GetPlayerCharacter(CurrentAltar->GetWorld(), 0));
    
    if (!PlayerCharacter)
    {
        return;
    }
    
    // Start activation
    CurrentAltar->StartSelectiveActivation(PlayerCharacter, SoulAmount, PowerAmount);
    bIsActivating = true;
    ActivationProgress = 0.0f;
    
    // Show progress overlay
    if (ProgressOverlay.IsValid())
    {
        ProgressOverlay->SetVisibility(EVisibility::Visible);
    }
}

void STrinityFlowShardAltar::UpdateActivationProgress(float Progress)
{
    ActivationProgress = Progress;
    
    if (ActivationProgressBar.IsValid())
    {
        ActivationProgressBar->SetPercent(Progress);
    }
}

EStanceType STrinityFlowShardAltar::CalculateResultingStance() const
{
    if (!CurrentAltar)
    {
        return EStanceType::Balanced;
    }
    
    // Get player character
    ATrinityFlowCharacter* PlayerCharacter = Cast<ATrinityFlowCharacter>(
        UGameplayStatics::GetPlayerCharacter(CurrentAltar->GetWorld(), 0));
    
    if (!PlayerCharacter)
    {
        return EStanceType::Balanced;
    }
    
    // Get shard component
    UShardComponent* ShardComp = PlayerCharacter->GetShardComponent();
    if (!ShardComp)
    {
        return EStanceType::Balanced;
    }
    
    // Calculate total active shards after activation
    int32 CurrentSoulActive = ShardComp->GetActiveShardCount(EShardType::Soul);
    int32 CurrentPowerActive = ShardComp->GetActiveShardCount(EShardType::Power);
    
    int32 TotalSoul = CurrentSoulActive + SoulAmount;
    int32 TotalPower = CurrentPowerActive + PowerAmount;
    
    if (TotalSoul > TotalPower)
    {
        return EStanceType::Soul;
    }
    else if (TotalPower > TotalSoul)
    {
        return EStanceType::Power;
    }
    else
    {
        return EStanceType::Balanced;
    }
}

FLinearColor STrinityFlowShardAltar::GetStanceColor(EStanceType Stance) const
{
    switch (Stance)
    {
        case EStanceType::Soul:
            return FLinearColor(0.0f, 0.5f, 1.0f);
        case EStanceType::Power:
            return FLinearColor(1.0f, 0.5f, 0.0f);
        case EStanceType::Balanced:
        default:
            return FLinearColor(0.8f, 0.2f, 1.0f);
    }
}

FText STrinityFlowShardAltar::GetStanceName(EStanceType Stance) const
{
    switch (Stance)
    {
        case EStanceType::Soul:
            return LOCTEXT("Soul", "Soul");
        case EStanceType::Power:
            return LOCTEXT("Power", "Power");
        case EStanceType::Balanced:
        default:
            return LOCTEXT("Balanced", "Balanced");
    }
}

void STrinityFlowShardAltar::UpdateSelectionDisplay()
{
    // Update panel colors based on selection
    if (SoulPanel.IsValid())
    {
        FLinearColor SoulColor = bSelectingSoul ? 
            FLinearColor(0.2f, 0.4f, 0.8f, 0.8f) : 
            FLinearColor(0.1f, 0.2f, 0.4f, 0.5f);
        SoulPanel->SetBorderBackgroundColor(FSlateColor(SoulColor));
    }
    
    if (PowerPanel.IsValid())
    {
        FLinearColor PowerColor = !bSelectingSoul ? 
            FLinearColor(0.8f, 0.4f, 0.2f, 0.8f) : 
            FLinearColor(0.4f, 0.2f, 0.1f, 0.5f);
        PowerPanel->SetBorderBackgroundColor(FSlateColor(PowerColor));
    }
    
    // Update control text colors
    if (SoulControlsText.IsValid())
    {
        SoulControlsText->SetColorAndOpacity(FSlateColor(bSelectingSoul ? FLinearColor::Yellow : FLinearColor::Gray));
    }
    
    if (PowerControlsText.IsValid())
    {
        PowerControlsText->SetColorAndOpacity(FSlateColor(!bSelectingSoul ? FLinearColor::Yellow : FLinearColor::Gray));
    }
}

void STrinityFlowShardAltar::UpdateActivationDisplay()
{
    // Update activation prompt
    if (ActivationPromptText.IsValid())
    {
        if (SoulAmount > 0 || PowerAmount > 0)
        {
            ActivationPromptText->SetText(LOCTEXT("PressToActivate", "Press E to Activate Shards"));
            ActivationPromptText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
        }
        else
        {
            ActivationPromptText->SetText(LOCTEXT("SelectShards", "Select shards to activate"));
            ActivationPromptText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
        }
    }
}

bool STrinityFlowShardAltar::GetSelectedShardInfo(EShardType& OutType, int32& OutCount) const
{
    // Return the currently selected shard type and amount
    if (bSelectingSoul && SoulAmount > 0)
    {
        OutType = EShardType::Soul;
        OutCount = SoulAmount;
        return true;
    }
    else if (!bSelectingSoul && PowerAmount > 0)
    {
        OutType = EShardType::Power;
        OutCount = PowerAmount;
        return true;
    }
    
    return false;
}

#undef LOCTEXT_NAMESPACE