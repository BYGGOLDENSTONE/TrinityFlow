#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Core/TrinityFlowTypes.h"

/**
 * Floating Damage Number Widget for TrinityFlow
 */
class TRINITYFLOW_API STrinityFlowDamageNumber : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STrinityFlowDamageNumber) {}
        SLATE_ARGUMENT(FVector, WorldLocation)
        SLATE_ARGUMENT(float, Damage)
        SLATE_ARGUMENT(bool, bIsEcho)
        SLATE_ARGUMENT(EDamageType, DamageType)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, 
        FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    bool IsExpired() const { return LifeTime <= 0.0f; }

private:
    FVector WorldLocation;
    float Damage;
    bool bIsEcho;
    EDamageType DamageType;
    
    float LifeTime = 2.0f;
    float InitialLifeTime = 2.0f;
    
    // Animation
    float VerticalOffset = 0.0f;
    float HorizontalOffset = 0.0f;
    
    // Color
    FLinearColor GetDamageColor() const;
    
    // Text
    FText GetDamageText() const;
};