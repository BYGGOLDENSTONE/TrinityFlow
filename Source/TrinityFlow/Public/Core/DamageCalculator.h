#pragma once

#include "CoreMinimal.h"
#include "TrinityFlowTypes.h"

class TRINITYFLOW_API FDamageCalculator
{
public:
    static float CalculateDamage(const FDamageInfo& DamageInfo, const FCharacterResources& TargetResources, ECharacterTag TargetTags, const FVector& DamageDirection, const FVector& TargetForward);
    
private:
    static bool IsShieldBlocking(ECharacterTag Tags, const FVector& DamageDirection, const FVector& TargetForward);
};