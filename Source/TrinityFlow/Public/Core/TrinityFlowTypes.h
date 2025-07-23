#pragma once

#include "CoreMinimal.h"
#include "TrinityFlowTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Soul UMETA(DisplayName = "Soul")
};

UENUM(BlueprintType)
enum class ECharacterTag : uint8
{
    None = 0x00,
    Shielded = 0x01,
    Armored = 0x02,
    Ghost = 0x04,
    Mechanical = 0x08,
    HaveSoul = 0x10
};
ENUM_CLASS_FLAGS(ECharacterTag);

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    None = 0x00,
    Vulnerable = 0x01,
    Combat = 0x02,
    NonCombat = 0x04,
    Marked = 0x08
};
ENUM_CLASS_FLAGS(ECharacterState);

USTRUCT()
struct FCharacterResources
{
    GENERATED_BODY()

    UPROPERTY()
    float Health = 100.0f;

    UPROPERTY()
    float MaxHealth = 100.0f;

    UPROPERTY()
    float DefencePoint = 0.0f;

    UPROPERTY()
    float AttackPoint = 10.0f;

    FCharacterResources() {}
    
    FCharacterResources(float InHealth, float InDefence, float InAttack)
        : Health(InHealth), MaxHealth(InHealth), DefencePoint(InDefence), AttackPoint(InAttack) {}
};

USTRUCT()
struct FDamageInfo
{
    GENERATED_BODY()

    UPROPERTY()
    float Amount = 0.0f;

    UPROPERTY()
    EDamageType Type = EDamageType::Physical;

    UPROPERTY()
    AActor* Instigator = nullptr;

    UPROPERTY()
    bool bIsAreaDamage = false;

    FDamageInfo() {}
    
    FDamageInfo(float InAmount, EDamageType InType, AActor* InInstigator = nullptr, bool bInIsAreaDamage = false)
        : Amount(InAmount), Type(InType), Instigator(InInstigator), bIsAreaDamage(bInIsAreaDamage) {}
};