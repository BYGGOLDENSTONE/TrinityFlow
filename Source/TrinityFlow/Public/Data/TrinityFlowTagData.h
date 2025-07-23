#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowTagData.generated.h"

/**
 * Tag effect types for gameplay implications
 */
UENUM(BlueprintType)
enum class ETagEffectType : uint8
{
    None = 0,
    DamageImmunity,     // Ghost (physical), Mechanical (soul)
    DamageReduction,    // Armored
    DirectionalBlock,   // Shielded
    DamageModifier,     // HaveSoul
    Custom              // For special cases
};

/**
 * Data structure for a single character tag
 */
USTRUCT(BlueprintType)
struct FCharacterTagData : public FTableRowBase
{
    GENERATED_BODY()

    /** Internal tag name (used in code) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName TagName;

    /** Display name shown in UI */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DisplayName;

    /** Tag description for tooltips */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Description;

    /** Tag category for organization */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Category = "General";

    /** UI Color for this tag */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FLinearColor TagColor = FLinearColor::White;

    /** Gameplay effect type */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ETagEffectType EffectType = ETagEffectType::None;

    /** Effect parameters based on EffectType */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == ETagEffectType::DamageImmunity"))
    EDamageType ImmunityType = EDamageType::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == ETagEffectType::DamageReduction", ClampMin = "0", ClampMax = "100"))
    float ReductionPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == ETagEffectType::DirectionalBlock"))
    bool bBlockFromFront = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == ETagEffectType::DamageModifier"))
    float DamageMultiplier = 1.0f;

    /** Tags that conflict with this one (can't have both) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FName> ConflictingTags;

    /** Tags that are required for this one to work */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FName> RequiredTags;

    /** Priority for tag effects (higher = applied first) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Priority = 0;

    FCharacterTagData()
    {
        TagName = NAME_None;
        DisplayName = "New Tag";
        Description = "Tag description";
        Category = "General";
        TagColor = FLinearColor::White;
        EffectType = ETagEffectType::None;
        Priority = 0;
    }
};

/**
 * Manager class for tag data access
 * This could be integrated into the stats subsystem
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowTagManager : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Tags")
    static FCharacterTagData GetTagData(UDataTable* TagDataTable, FName TagName);

    UFUNCTION(BlueprintCallable, Category = "Tags")
    static TArray<FName> GetAllTagNames(UDataTable* TagDataTable);

    UFUNCTION(BlueprintCallable, Category = "Tags")
    static bool ValidateTagCombination(UDataTable* TagDataTable, const TArray<FName>& Tags);

    UFUNCTION(BlueprintCallable, Category = "Tags")
    static FString GetTagDisplayString(UDataTable* TagDataTable, const TArray<FName>& Tags);
};