#include "Core/AnimationComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/World.h"

UAnimationComponent::UAnimationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAnimationComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get character mesh reference
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        CharacterMesh = Character->GetMesh();
        if (CharacterMesh)
        {
            AnimInstance = CharacterMesh->GetAnimInstance();
        }
    }
}

void UAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update idle animation
    if (!bIsAnimationLocked)
    {
        UpdateIdleAnimation();
    }
}

bool UAnimationComponent::PlayAttackAnimation(bool bIsLeftHand)
{
    if (!CanPlayNewAnimation() || !AnimInstance)
    {
        return false;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeSinceLastAttack = CurrentTime - LastAttackTime;
    
    // Determine which montage to play based on state and timing
    UAnimMontage* MontageToPlay = nullptr;
    EAttackState NewState = EAttackState::None;
    
    if (CurrentAttackState == EAttackState::None || CurrentAttackState == EAttackState::SecondAttack)
    {
        // Check if this is a spam combo
        if (TimeSinceLastAttack < SpamComboThreshold && LastAttackTime > 0)
        {
            MontageToPlay = bIsLeftHand ? LeftSlashCombo : RightSlashCombo;
            NewState = EAttackState::ComboAttack;
            UE_LOG(LogTemp, Warning, TEXT("Playing spam combo (time since last: %f)"), TimeSinceLastAttack);
        }
        else
        {
            MontageToPlay = bIsLeftHand ? LeftSlash1 : RightSlash1;
            NewState = EAttackState::FirstAttack;
        }
    }
    else if (CurrentAttackState == EAttackState::WaitingForCombo)
    {
        // Player is in wait state, play second attack
        MontageToPlay = bIsLeftHand ? LeftSlash2 : RightSlash2;
        NewState = EAttackState::SecondAttack;
        
        // Stop the wait montage
        if (CurrentWaitMontage && AnimInstance->Montage_IsPlaying(CurrentWaitMontage))
        {
            AnimInstance->Montage_Stop(0.1f, CurrentWaitMontage);
        }
    }
    else
    {
        // Already in attack, can't start new one
        return false;
    }
    
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("No montage available for attack"));
        return false;
    }
    
    // Stop idle animation
    StopIdleMontage();
    
    // Play the attack montage
    LockAnimation(MontageToPlay);
    float MontageLength = AnimInstance->Montage_Play(MontageToPlay, AttackMontagePlayRate);
    
    if (MontageLength > 0.0f)
    {
        CurrentAttackState = NewState;
        LastAttackTime = CurrentTime;
        bLastAttackWasLeft = bIsLeftHand;
        
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnAttackMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
        
        UE_LOG(LogTemp, Warning, TEXT("Playing attack montage %s, State: %d"), 
            *MontageToPlay->GetName(), (int32)NewState);
        
        return true;
    }
    else
    {
        UnlockAnimation();
        return false;
    }
}

void UAnimationComponent::PlayIdleMontage()
{
    UAnimMontage* CurrentIdleMontage = GetCurrentIdleMontage();
    
    if (!CurrentIdleMontage || !AnimInstance || bIsAnimationLocked)
    {
        return;
    }

    // Check if idle is already playing
    if (!AnimInstance->Montage_IsPlaying(CurrentIdleMontage))
    {
        AnimInstance->Montage_Play(CurrentIdleMontage);

        // Set up looping timer
        float MontageLength = CurrentIdleMontage->GetPlayLength();
        GetWorld()->GetTimerManager().SetTimer(
            IdleLoopTimerHandle,
            this,
            &UAnimationComponent::RestartIdleAnimation,
            MontageLength,
            true
        );
    }
}

void UAnimationComponent::StopIdleMontage()
{
    // Stop any idle montage that might be playing
    if (AnimInstance)
    {
        if (NonCombatIdleMontage && AnimInstance->Montage_IsPlaying(NonCombatIdleMontage))
        {
            AnimInstance->Montage_Stop(0.2f, NonCombatIdleMontage);
        }
        if (CombatIdleMontage && AnimInstance->Montage_IsPlaying(CombatIdleMontage))
        {
            AnimInstance->Montage_Stop(0.2f, CombatIdleMontage);
        }
    }

    // Clear loop timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(IdleLoopTimerHandle);
    }
}

bool UAnimationComponent::CanPlayNewAnimation() const
{
    return !bIsAnimationLocked;
}

void UAnimationComponent::PlayWaitMontage(bool bIsLeftHand)
{
    if (!AnimInstance)
    {
        return;
    }
    
    UAnimMontage* WaitMontage = bIsLeftHand ? LeftSlash1Wait : RightSlash1Wait;
    
    if (!WaitMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("No wait montage available"));
        CurrentAttackState = EAttackState::None;
        UnlockAnimation();
        return;
    }
    
    // Play wait montage
    float MontageLength = AnimInstance->Montage_Play(WaitMontage, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        CurrentAttackState = EAttackState::WaitingForCombo;
        CurrentWaitMontage = WaitMontage;
        
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnWaitMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, WaitMontage);
        
        UE_LOG(LogTemp, Warning, TEXT("Playing wait montage %s"), *WaitMontage->GetName());
    }
    else
    {
        CurrentAttackState = EAttackState::None;
        UnlockAnimation();
    }
}

void UAnimationComponent::ResetCombo()
{
    CurrentAttackState = EAttackState::None;
    CurrentWaitMontage = nullptr;
    
    // Reset to idle if not locked
    if (!bIsAnimationLocked)
    {
        PlayIdleMontage();
    }
}

void UAnimationComponent::OnMovementInput(const FVector& MovementVector)
{
    LastMovementInput = MovementVector;

    // Cancel wait state if movement detected
    if (CurrentAttackState == EAttackState::WaitingForCombo && bMovementCancelsCombo && MovementVector.SizeSquared() > 0.01f)
    {
        if (CurrentWaitMontage && AnimInstance && AnimInstance->Montage_IsPlaying(CurrentWaitMontage))
        {
            AnimInstance->Montage_Stop(0.2f, CurrentWaitMontage);
        }
        ResetCombo();
    }
}

bool UAnimationComponent::PlayInteractionMontage()
{
    if (!InteractionMontage || !AnimInstance || bIsAnimationLocked)
    {
        return false;
    }

    // Stop any idle animation
    StopIdleMontage();

    // Lock animation to prevent interruption
    LockAnimation(InteractionMontage);

    // Play interaction montage at normal speed
    float MontageLength = AnimInstance->Montage_Play(InteractionMontage, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        // Set up completion callback to unlock
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
        {
            UnlockAnimation();
            UE_LOG(LogTemp, Warning, TEXT("Interaction montage completed"));
        });
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, InteractionMontage);
        
        UE_LOG(LogTemp, Warning, TEXT("Playing interaction montage %s, length %f"), 
            *InteractionMontage->GetName(), MontageLength);
        
        return true;
    }
    else
    {
        UnlockAnimation();
        return false;
    }
}

void UAnimationComponent::SetCombatState(bool bInCombat)
{
    bIsInCombat = bInCombat;

    if (bInCombat)
    {
        StopIdleMontage();
    }
    else
    {
        // Reset combos when leaving combat
        ResetCombo();
        // Start idle animation after a short delay
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DelayHandle,
            this,
            &UAnimationComponent::PlayIdleMontage,
            0.5f,
            false
        );
    }
}

void UAnimationComponent::LockAnimation(UAnimMontage* Montage)
{
    bIsAnimationLocked = true;
    CurrentLockedMontage = Montage;
}

void UAnimationComponent::UnlockAnimation()
{
    bIsAnimationLocked = false;
    CurrentLockedMontage = nullptr;
}

bool UAnimationComponent::ShouldPlaySpamCombo()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float TimeSinceLastAttack = CurrentTime - LastAttackTime;
    return TimeSinceLastAttack < SpamComboThreshold && LastAttackTime > 0;
}

UAnimMontage* UAnimationComponent::GetAttackMontage(bool bIsLeftHand, EAttackState State)
{
    switch (State)
    {
        case EAttackState::FirstAttack:
            return bIsLeftHand ? LeftSlash1 : RightSlash1;
        case EAttackState::SecondAttack:
            return bIsLeftHand ? LeftSlash2 : RightSlash2;
        case EAttackState::ComboAttack:
            return bIsLeftHand ? LeftSlashCombo : RightSlashCombo;
        default:
            return nullptr;
    }
}

void UAnimationComponent::OnAttackMontageComplete(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Attack montage complete. State: %d, Interrupted: %s"), 
        (int32)CurrentAttackState, bInterrupted ? TEXT("true") : TEXT("false"));
    
    if (bInterrupted)
    {
        UnlockAnimation();
        ResetCombo();
        return;
    }
    
    // Handle based on current state
    switch (CurrentAttackState)
    {
        case EAttackState::FirstAttack:
            // Play wait montage after first attack
            PlayWaitMontage(bLastAttackWasLeft);
            break;
            
        case EAttackState::SecondAttack:
        case EAttackState::ComboAttack:
            // Combo finished, return to idle
            UnlockAnimation();
            ResetCombo();
            break;
            
        default:
            UnlockAnimation();
            ResetCombo();
            break;
    }
}

void UAnimationComponent::OnWaitMontageComplete(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Wait montage complete. Interrupted: %s"), 
        bInterrupted ? TEXT("true") : TEXT("false"));
    
    CurrentWaitMontage = nullptr;
    
    // Unlock and reset to idle
    UnlockAnimation();
    ResetCombo();
}


void UAnimationComponent::UpdateIdleAnimation()
{
    UAnimMontage* CurrentIdleMontage = GetCurrentIdleMontage();
    
    if (!bIsAnimationLocked && CurrentIdleMontage && AnimInstance)
    {
        // Only play if not already playing
        if (!AnimInstance->Montage_IsPlaying(CurrentIdleMontage))
        {
            PlayIdleMontage();
        }
    }
}

UAnimMontage* UAnimationComponent::GetCurrentIdleMontage() const
{
    return bIsInCombat ? CombatIdleMontage : NonCombatIdleMontage;
}

void UAnimationComponent::RestartIdleAnimation()
{
    if (!bIsInCombat && !bIsAnimationLocked)
    {
        PlayIdleMontage();
    }
}