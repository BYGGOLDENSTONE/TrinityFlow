#include "Enemy/EnemyAnimationComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UEnemyAnimationComponent::UEnemyAnimationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsPlayingAnimation = false;
}

void UEnemyAnimationComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache owner character
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyAnimationComponent: Owner is not a Character!"));
        return;
    }

    // Cache anim instance
    if (OwnerCharacter->GetMesh())
    {
        AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            // Bind to montage end delegate
            AnimInstance->OnMontageEnded.AddDynamic(this, &UEnemyAnimationComponent::OnMontageEnded);
        }
    }
}

void UEnemyAnimationComponent::PlayHitResponse(EDamageType DamageType, bool bIsLeftWeapon)
{
    // Choose appropriate hit response montage based on which weapon hit
    UAnimMontage* MontageToPlay = bIsLeftWeapon ? LeftHitResponseMontage : RightHitResponseMontage;
    
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyAnimationComponent: No hit response montage for %s weapon!"), 
            bIsLeftWeapon ? TEXT("left") : TEXT("right"));
        return;
    }

    // Stop any current animation to play hit response
    if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
    {
        AnimInstance->StopAllMontages(0.1f);
    }

    PlayMontage(MontageToPlay);
    
    UE_LOG(LogTemp, Display, TEXT("Enemy playing %s hit response for %s damage"), 
        bIsLeftWeapon ? TEXT("left") : TEXT("right"),
        DamageType == EDamageType::Soul ? TEXT("soul") : TEXT("physical"));
}

void UEnemyAnimationComponent::PlayAttackAnimation()
{
    if (!AttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyAnimationComponent: No attack montage set!"));
        return;
    }

    // Don't interrupt hit responses with attacks
    if (IsPlayingCombatAnimation())
    {
        return;
    }

    PlayMontage(AttackMontage);
    UE_LOG(LogTemp, Display, TEXT("Enemy playing attack animation"));
}

void UEnemyAnimationComponent::PlayParryResponse()
{
    if (!ParryResponseMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyAnimationComponent: No parry response montage set!"));
        return;
    }

    // Stop any current animation to play parry response
    if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
    {
        AnimInstance->StopAllMontages(0.1f);
    }

    PlayMontage(ParryResponseMontage);
    UE_LOG(LogTemp, Display, TEXT("Enemy playing parry response animation"));
}

bool UEnemyAnimationComponent::IsPlayingCombatAnimation() const
{
    if (!AnimInstance)
    {
        return false;
    }

    // Check if any of our combat montages are playing
    if (AnimInstance->IsAnyMontagePlaying())
    {
        UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
        return CurrentMontage == LeftHitResponseMontage || 
               CurrentMontage == RightHitResponseMontage || 
               CurrentMontage == AttackMontage || 
               CurrentMontage == ParryResponseMontage;
    }

    return false;
}

bool UEnemyAnimationComponent::PlayMontage(UAnimMontage* Montage, float PlayRate, FName StartSection)
{
    if (!Montage || !AnimInstance)
    {
        return false;
    }

    float Duration = AnimInstance->Montage_Play(Montage, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f);
    
    if (Duration > 0.0f)
    {
        bIsPlayingAnimation = true;
        
        if (StartSection != NAME_None)
        {
            AnimInstance->Montage_JumpToSection(StartSection, Montage);
        }
        
        return true;
    }

    return false;
}

void UEnemyAnimationComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == LeftHitResponseMontage || 
        Montage == RightHitResponseMontage || 
        Montage == AttackMontage || 
        Montage == ParryResponseMontage)
    {
        bIsPlayingAnimation = false;
        UE_LOG(LogTemp, Display, TEXT("Enemy combat animation ended"));
    }
}