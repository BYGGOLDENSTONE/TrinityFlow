#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Core/TrinityFlowTypes.h"

class UTrinityFlowUIManager;
class AShardAltar;

/**
 * Shard Altar UI Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowShardAltar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowShardAltar) {}
        SLATE_ARGUMENT(UTrinityFlowUIManager*, UIManager)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
    
    void SetAltar(AShardAltar* InAltar);
    void PlayOpenAnimation();
    void PlayCloseAnimation();

private:
    // UI Manager reference
    UTrinityFlowUIManager* UIManager = nullptr;
    
    // Altar reference
    AShardAltar* CurrentAltar = nullptr;
    
    // Widget References
    TSharedPtr<class SBorder> BackgroundBorder;
    TSharedPtr<class SBorder> MainPanel;
    TSharedPtr<class SBorder> SoulPanel;
    TSharedPtr<class SBorder> PowerPanel;
    TSharedPtr<class STextBlock> SoulAmountText;
    TSharedPtr<class STextBlock> PowerAmountText;
    TSharedPtr<class STextBlock> SoulAvailableText;
    TSharedPtr<class STextBlock> PowerAvailableText;
    TSharedPtr<class STextBlock> SoulControlsText;
    TSharedPtr<class STextBlock> PowerControlsText;
    TSharedPtr<class STextBlock> StancePreviewText;
    TSharedPtr<class STextBlock> ActivationPromptText;
    TSharedPtr<class SProgressBar> ActivationProgressBar;
    TSharedPtr<class SOverlay> ProgressOverlay;
    
    // Shard Selection
    bool bSelectingSoul = true;
    int32 SoulAmount = 0;
    int32 PowerAmount = 0;
    int32 MaxSoulAmount = 0;
    int32 MaxPowerAmount = 0;
    
    // Animation
    FCurveSequence OpenAnimation;
    FCurveHandle OpenCurve;
    float AnimationAlpha = 0.0f;
    
    // Activation Progress
    bool bIsActivating = false;
    float ActivationProgress = 0.0f;
    
    // Input Handling
    void HandleInput(const FKey& Key);
    
    // UI Updates
    void UpdateShardDisplay();
    void UpdateStancePreview();
    void UpdateSelectionDisplay();
    void UpdateActivationDisplay();
    void StartActivation();
    void UpdateActivationProgress(float Progress);
    
    EStanceType CalculateResultingStance() const;
    FLinearColor GetStanceColor(EStanceType Stance) const;
    FText GetStanceName(EStanceType Stance) const;
};