#include "UI/TrinityFlowStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FTrinityFlowStyle::StyleInstance = nullptr;

void FTrinityFlowStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void FTrinityFlowStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
    ensure(StyleInstance.IsUnique());
    StyleInstance.Reset();
}

void FTrinityFlowStyle::ReloadTextures()
{
    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

const ISlateStyle& FTrinityFlowStyle::Get()
{
    return *StyleInstance;
}

FName FTrinityFlowStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("TrinityFlowStyle"));
    return StyleSetName;
}

TSharedRef<FSlateStyleSet> FTrinityFlowStyle::Create()
{
    TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
    FSlateStyleSet& Style = StyleRef.Get();

    // Fonts
    const FString EngineContentDir = FPaths::EngineContentDir();
    Style.Set("TrinityFlow.Font.Bold", FSlateFontInfo(EngineContentDir / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 24));
    Style.Set("TrinityFlow.Font.Regular", FSlateFontInfo(EngineContentDir / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14));
    Style.Set("TrinityFlow.Font.DamageNumber", FSlateFontInfo(EngineContentDir / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 18));

    // Colors
    Style.Set("TrinityFlow.Color.Soul", FLinearColor(0.0f, 0.5f, 1.0f)); // Blue
    Style.Set("TrinityFlow.Color.Power", FLinearColor(1.0f, 0.5f, 0.0f)); // Orange
    Style.Set("TrinityFlow.Color.Balanced", FLinearColor(0.8f, 0.2f, 1.0f)); // Purple
    Style.Set("TrinityFlow.Color.Positive", FLinearColor::Green);
    Style.Set("TrinityFlow.Color.Negative", FLinearColor::Red);
    Style.Set("TrinityFlow.Color.Neutral", FLinearColor::White);
    Style.Set("TrinityFlow.Color.Background", FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
    Style.Set("TrinityFlow.Color.Border", FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));

    // Brushes
    Style.Set("TrinityFlow.Brush.ToolPanel", new FSlateColorBrush(Style.GetColor("TrinityFlow.Color.Background")));
    Style.Set("TrinityFlow.Brush.White", new FSlateColorBrush(FLinearColor::White));
    Style.Set("TrinityFlow.Brush.Separator", new FSlateColorBrush(Style.GetColor("TrinityFlow.Color.Border")));
    Style.Set("TrinityFlow.Brush.HealthBar", new FSlateColorBrush(FLinearColor(0.8f, 0.0f, 0.0f, 1.0f)));

    return StyleRef;
}
