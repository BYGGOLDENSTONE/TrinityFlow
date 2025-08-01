#include "UI/Slate/STrinityFlowDamageNumber.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"

#define LOCTEXT_NAMESPACE "TrinityFlowDamageNumber"

void STrinityFlowDamageNumber::Construct(const FArguments& InArgs)
{
    WorldLocation = InArgs._WorldLocation;
    Damage = InArgs._Damage;
    bIsEcho = InArgs._bIsEcho;
    DamageType = InArgs._DamageType;
    
    // Add random horizontal offset
    HorizontalOffset = FMath::RandRange(-30.0f, 30.0f);
    
    ChildSlot
    [
        SNullWidget::NullWidget
    ];
}

void STrinityFlowDamageNumber::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    // Update lifetime
    LifeTime -= InDeltaTime;
    
    // Update vertical offset (float upward)
    VerticalOffset += 50.0f * InDeltaTime;
}

int32 STrinityFlowDamageNumber::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Get viewport
    if (!GEngine || !GEngine->GameViewport)
    {
        return LayerId;
    }
    
    FSceneViewport* Viewport = GEngine->GameViewport->GetGameViewport();
    if (!Viewport)
    {
        return LayerId;
    }
    
    // Get player controller for projection
    APlayerController* PC = GEngine->GetFirstLocalPlayerController(GEngine->GameViewport->GetWorld());
    if (!PC)
    {
        return LayerId;
    }
    
    // Project world location to screen
    FVector2D ScreenLocation;
    FVector AdjustedWorldLocation = WorldLocation + FVector(HorizontalOffset, 0, 100.0f + VerticalOffset);
    
    if (!PC->ProjectWorldLocationToScreen(AdjustedWorldLocation, ScreenLocation))
    {
        return LayerId;
    }
    
    // Calculate alpha based on lifetime
    float Alpha = FMath::Clamp(LifeTime / 0.5f, 0.0f, 1.0f);
    
    // Get damage text and color
    FText DamageText = GetDamageText();
    FLinearColor DamageColor = GetDamageColor();
    DamageColor.A = Alpha;
    
    // Calculate font size based on damage amount
    float FontSize = FMath::Clamp(20.0f + (Damage / 50.0f) * 10.0f, 20.0f, 60.0f);
    FSlateFontInfo FontInfo = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), FontSize);
    
    // Draw shadow
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2f(1, 1), FSlateLayoutTransform(FVector2f(ScreenLocation.X + 2, ScreenLocation.Y + 2))),
        DamageText,
        FontInfo,
        ESlateDrawEffect::None,
        FLinearColor(0, 0, 0, Alpha)
    );
    
    // Draw text
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(FVector2f(1, 1), FSlateLayoutTransform(FVector2f(ScreenLocation.X, ScreenLocation.Y))),
        DamageText,
        FontInfo,
        ESlateDrawEffect::None,
        DamageColor
    );
    
    return LayerId + 2;
}

FLinearColor STrinityFlowDamageNumber::GetDamageColor() const
{
    if (bIsEcho)
    {
        return FLinearColor(0.0f, 0.5f, 1.0f); // Blue for echo
    }
    else if (DamageType == EDamageType::Soul)
    {
        return FLinearColor(0.0f, 0.5f, 1.0f); // Blue for soul
    }
    else
    {
        return FLinearColor(1.0f, 0.5f, 0.0f); // Orange for physical
    }
}

FText STrinityFlowDamageNumber::GetDamageText() const
{
    if (bIsEcho)
    {
        return FText::Format(LOCTEXT("EchoDamage", "ECHO {0}"), FMath::RoundToInt(Damage));
    }
    else
    {
        return FText::AsNumber(FMath::RoundToInt(Damage));
    }
}

#undef LOCTEXT_NAMESPACE