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
enum class EShardType : uint8
{
    Soul UMETA(DisplayName = "Soul"),
    Power UMETA(DisplayName = "Power")
};

UENUM(BlueprintType)
enum class EStanceType : uint8
{
    Soul UMETA(DisplayName = "Soul"),
    Power UMETA(DisplayName = "Power"),
    Balanced UMETA(DisplayName = "Balanced")
};

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECharacterTag : uint8
{
    None = 0 UMETA(Hidden),
    Shielded = 1 << 0 UMETA(DisplayName = "Shielded"),
    Armored = 1 << 1 UMETA(DisplayName = "Armored"),
    Ghost = 1 << 2 UMETA(DisplayName = "Ghost"),
    Mechanical = 1 << 3 UMETA(DisplayName = "Mechanical"),
    HaveSoul = 1 << 4 UMETA(DisplayName = "HaveSoul"),
    Duelist = 1 << 5 UMETA(DisplayName = "Duelist"),
    Void = 1 << 6 UMETA(DisplayName = "Void"),
    Mimic = 1 << 7 UMETA(DisplayName = "Mimic")
};
ENUM_CLASS_FLAGS(ECharacterTag);

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECharacterTagExtended : uint8
{
    None = 0 UMETA(Hidden),
    Berserk = 1 << 0 UMETA(DisplayName = "Berserk"),
    Ethereal = 1 << 1 UMETA(DisplayName = "Ethereal")
};
ENUM_CLASS_FLAGS(ECharacterTagExtended);

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