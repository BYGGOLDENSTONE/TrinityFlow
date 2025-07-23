#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowCharacterStats.generated.h"

/**
 * Data asset for character stats configuration
 * Allows designers to create and modify character stats in the editor
 */
UCLASS(BlueprintType)
class TRINITYFLOW_API UTrinityFlowCharacterStats : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Character display name */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    FString CharacterName;

    /** Character type identifier */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Info")
    FName CharacterID;

    // Base Stats
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Health", meta = (ClampMin = "1", ClampMax = "10000"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Combat", meta = (ClampMin = "0", ClampMax = "1000"))
    float AttackPoint = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Combat", meta = (ClampMin = "0", ClampMax = "100"))
    float DefencePoint = 0.0f;

    // Combat Properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float AttackSpeed = 1.0f; // Attacks per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "50", ClampMax = "2000"))
    float AttackRange = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsAreaDamage = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (EditCondition = "bIsAreaDamage"))
    float AreaDamageRadius = 400.0f;

    // AI Properties (for enemies)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "100", ClampMax = "5000"))
    float SightRange = 1500.0f;

    // Tags
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags", meta = (Bitmask, BitmaskEnum = "/Script/TrinityFlow.ECharacterTag"))
    int32 CharacterTags = 0;

    // Helper function to get tags as enum
    ECharacterTag GetCharacterTags() const 
    { 
        UE_LOG(LogTemp, Warning, TEXT("CharacterStats %s - Raw tag int value: %d"), *CharacterName, CharacterTags);
        return static_cast<ECharacterTag>(CharacterTags); 
    }

    // Get resources struct for easy application
    FCharacterResources GetCharacterResources() const
    {
        FCharacterResources Resources;
        Resources.Health = MaxHealth;
        Resources.MaxHealth = MaxHealth;
        Resources.AttackPoint = AttackPoint;
        Resources.DefencePoint = DefencePoint;
        return Resources;
    }
};