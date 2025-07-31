#include "Core/AnimationComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../TrinityFlowCharacter.h"

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

    // Update wondering timer
    UpdateWonderingTimer();
    
    // Montages will play naturally without tracking
}

float UAnimationComponent::PlayAttackAnimation(bool bIsLeftHand)
{
    UE_LOG(LogTemp, Warning, TEXT("PlayAttackAnimation called - bIsLeftHand: %s"), bIsLeftHand ? TEXT("true") : TEXT("false"));
    
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("No AnimInstance found!"));
        return 0.0f;
    }
    
    if (bIsAnimationLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("Animation is locked!"));
        return 0.0f;
    }

    // Stop any wondering animation
    StopWonderingTimer();
    if (bIsWondering && WonderingMontage)
    {
        AnimInstance->Montage_Stop(0.2f, WonderingMontage);
        bIsWondering = false;
    }

    // Get the appropriate montage
    UAnimMontage* MontageToPlay = bIsLeftHand ? LeftSlash1 : RightSlash1;
    
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Error, TEXT("No montage found! %s is null. Make sure to set the montages in the Character Blueprint!"), 
            bIsLeftHand ? TEXT("LeftSlash1") : TEXT("RightSlash1"));
        return 0.0f;
    }
    
    // Lock animation and play the attack montage at natural speed
    LockAnimation(MontageToPlay);
    
    // Play montage at its natural speed (1.0)
    float MontageLength = AnimInstance->Montage_Play(MontageToPlay, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
        
        UE_LOG(LogTemp, Log, TEXT("Playing attack montage %s (Length: %.3f seconds)"), *MontageToPlay->GetName(), MontageLength);
        
        return MontageLength;
    }
    else
    {
        UnlockAnimation();
        return 0.0f;
    }
}

void UAnimationComponent::StartWonderingTimer()
{
    if (!bIsInCombat && !bIsAnimationLocked && !bIsWondering)
    {
        GetWorld()->GetTimerManager().SetTimer(
            WonderingTimerHandle,
            this,
            &UAnimationComponent::PlayWonderingAnimation,
            WonderingDelay,
            false
        );
    }
}

void UAnimationComponent::StopWonderingTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(WonderingTimerHandle);
}

void UAnimationComponent::PlayWonderingAnimation()
{
    if (!WonderingMontage || !AnimInstance || bIsAnimationLocked || bIsInCombat)
    {
        return;
    }

    // Play wondering montage
    float MontageLength = AnimInstance->Montage_Play(WonderingMontage, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        bIsWondering = true;
        LockAnimation(WonderingMontage);
        
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, WonderingMontage);
        
        UE_LOG(LogTemp, Log, TEXT("Playing wondering montage"));
    }
}

void UAnimationComponent::UpdateWonderingTimer()
{
    // Start wondering timer if conditions are met
    if (!bIsInCombat && !bIsAnimationLocked && !bIsWondering && 
        LastMovementInput.IsNearlyZero() && 
        !GetWorld()->GetTimerManager().IsTimerActive(WonderingTimerHandle))
    {
        StartWonderingTimer();
    }
}

bool UAnimationComponent::CanPlayNewAnimation() const
{
    return !bIsAnimationLocked;
}


void UAnimationComponent::OnMovementInput(const FVector& MovementVector)
{
    LastMovementInput = MovementVector;

    // Stop wondering timer and animation if player moves
    if (MovementVector.SizeSquared() > 0.01f)
    {
        StopWonderingTimer();
        
        if (bIsWondering && WonderingMontage && AnimInstance)
        {
            AnimInstance->Montage_Stop(0.2f, WonderingMontage);
            bIsWondering = false;
            UnlockAnimation();
        }
    }
}

bool UAnimationComponent::PlayInteractionMontage()
{
    if (!InteractionMontage || !AnimInstance || bIsAnimationLocked)
    {
        return false;
    }

    // Stop any wondering animation
    StopWonderingTimer();
    if (bIsWondering && WonderingMontage)
    {
        AnimInstance->Montage_Stop(0.2f, WonderingMontage);
        bIsWondering = false;
    }

    // Lock animation to prevent interruption
    LockAnimation(InteractionMontage);

    // Play interaction montage at normal speed
    float MontageLength = AnimInstance->Montage_Play(InteractionMontage, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        // Set up completion callback to unlock
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, InteractionMontage);
        
        UE_LOG(LogTemp, Log, TEXT("Playing interaction montage %s"), *InteractionMontage->GetName());
        
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

    // Stop wondering if entering combat
    if (bInCombat)
    {
        StopWonderingTimer();
        if (bIsWondering && WonderingMontage && AnimInstance)
        {
            AnimInstance->Montage_Stop(0.2f, WonderingMontage);
            bIsWondering = false;
            UnlockAnimation();
        }
    }
    else
    {
        // Start wondering timer when leaving combat
        StartWonderingTimer();
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

void UAnimationComponent::OnMontageComplete(UAnimMontage* Montage, bool bInterrupted)
{
    
    // Unlock animation
    UnlockAnimation();
    
    // If it was a wondering montage, clear the flag
    if (Montage == WonderingMontage)
    {
        bIsWondering = false;
        // Restart wondering timer
        StartWonderingTimer();
    }
    
    // Notify the character that the attack is complete if it was an attack
    if (Montage == LeftSlash1 || Montage == RightSlash1)
    {
        if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
        {
            if (ATrinityFlowCharacter* TrinityCharacter = Cast<ATrinityFlowCharacter>(Character))
            {
                TrinityCharacter->OnAttackComplete();
            }
        }
    }
}

void UAnimationComponent::PlayHitResponse()
{
    if (!HitResponseMontage || !AnimInstance || bIsAnimationLocked)
    {
        return;
    }

    // Stop any wondering animation
    StopWonderingTimer();
    if (bIsWondering && WonderingMontage)
    {
        AnimInstance->Montage_Stop(0.2f, WonderingMontage);
        bIsWondering = false;
    }

    // Play hit response montage
    float MontageLength = AnimInstance->Montage_Play(HitResponseMontage, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        LockAnimation(HitResponseMontage);
        
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HitResponseMontage);
        
        UE_LOG(LogTemp, Log, TEXT("Playing hit response montage"));
    }
}

void UAnimationComponent::PlayDefensiveAnimation(bool bIsLeftHand, bool bIsPerfect)
{
    if (!AnimInstance || bIsAnimationLocked)
    {
        return;
    }

    // Stop any wondering animation
    StopWonderingTimer();
    if (bIsWondering && WonderingMontage)
    {
        AnimInstance->Montage_Stop(0.2f, WonderingMontage);
        bIsWondering = false;
    }

    // Select the appropriate montage
    UAnimMontage* MontageToPlay = nullptr;
    
    if (bIsLeftHand)
    {
        MontageToPlay = bIsPerfect ? LeftParryMontage : LeftBlockMontage;
    }
    else
    {
        MontageToPlay = bIsPerfect ? RightParryMontage : RightBlockMontage;
    }
    
    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("No defensive montage found for %s %s"), 
            bIsLeftHand ? TEXT("Left") : TEXT("Right"),
            bIsPerfect ? TEXT("Parry") : TEXT("Block"));
        return;
    }

    // Play the defensive montage
    float MontageLength = AnimInstance->Montage_Play(MontageToPlay, 1.0f);
    
    if (MontageLength > 0.0f)
    {
        LockAnimation(MontageToPlay);
        
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
        
        UE_LOG(LogTemp, Log, TEXT("Playing %s %s montage"), 
            bIsLeftHand ? TEXT("Left") : TEXT("Right"),
            bIsPerfect ? TEXT("Parry") : TEXT("Block"));
    }
}



