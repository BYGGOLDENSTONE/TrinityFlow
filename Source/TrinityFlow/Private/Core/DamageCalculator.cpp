#include "Core/DamageCalculator.h"

float FDamageCalculator::CalculateDamage(const FDamageInfo& DamageInfo, const FCharacterResources& TargetResources, ECharacterTag TargetTags, const FVector& DamageDirection, const FVector& TargetForward)
{
    float FinalDamage = 0.0f;
    
    switch (DamageInfo.Type)
    {
        case EDamageType::Physical:
        {
            // Ghost tag means no physical damage
            if (EnumHasAnyFlags(TargetTags, ECharacterTag::Ghost))
            {
                return 0.0f;
            }
            
            // Check if shield blocks the damage (frontal attacks only)
            if (IsShieldBlocking(TargetTags, DamageDirection, TargetForward))
            {
                return 0.0f;
            }
            
            // Physical damage calculation: attackpoint * (100 - defencepoint)%
            float DefenceMultiplier = FMath::Max(0.0f, 100.0f - TargetResources.DefencePoint) / 100.0f;
            FinalDamage = DamageInfo.Amount * DefenceMultiplier;
            break;
        }
        
        case EDamageType::Soul:
        {
            // Mechanical tag means no soul damage
            if (EnumHasAnyFlags(TargetTags, ECharacterTag::Mechanical))
            {
                return 0.0f;
            }
            
            // Soul damage calculation: attackpoint * 2 (bypasses defenses)
            FinalDamage = DamageInfo.Amount * 2.0f;
            break;
        }
    }
    
    return FinalDamage;
}

bool FDamageCalculator::IsShieldBlocking(ECharacterTag Tags, const FVector& DamageDirection, const FVector& TargetForward)
{
    if (!EnumHasAnyFlags(Tags, ECharacterTag::Shielded))
    {
        return false;
    }
    
    // Shield blocks if damage comes from front (dot product > 0 means frontal)
    float DotProduct = FVector::DotProduct(-DamageDirection.GetSafeNormal(), TargetForward.GetSafeNormal());
    return DotProduct > 0.0f;
}