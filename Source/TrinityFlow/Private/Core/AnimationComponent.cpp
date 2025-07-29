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
    
    // Debug: Track attack montage progress
    if (bIsAnimationLocked && CurrentLockedMontage && AnimInstance)
    {
        if (CurrentLockedMontage == LeftSlash1 || CurrentLockedMontage == RightSlash1)
        {
            float CurrentTime = AnimInstance->Montage_GetPosition(CurrentLockedMontage);
            float PlayRate = AnimInstance->Montage_GetPlayRate(CurrentLockedMontage);
            bool bIsPlaying = AnimInstance->Montage_IsPlaying(CurrentLockedMontage);
            
            // Log every 5 frames (roughly)
            static float LastLogTime = 0.0f;
            if (GetWorld()->GetTimeSeconds() - LastLogTime > 0.08f)
            {
                UE_LOG(LogTemp, Warning, TEXT("Montage Progress - Time: %.3f, Position: %.3f, Playing: %s"), 
                    GetWorld()->GetTimeSeconds(), CurrentTime, bIsPlaying ? TEXT("Yes") : TEXT("No"));
                LastLogTime = GetWorld()->GetTimeSeconds();
            }
        }
    }
}

float UAnimationComponent::PlayAttackAnimation(bool bIsLeftHand)
{
    if (!AnimInstance || bIsAnimationLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayAttackAnimation: Cannot play - AnimInstance:%s, Locked:%s"), 
            AnimInstance ? TEXT("Valid") : TEXT("NULL"), 
            bIsAnimationLocked ? TEXT("Yes") : TEXT("No"));
        return 0.0f;
    }

    // Check if any montage is currently playing
    if (AnimInstance->IsAnyMontagePlaying())
    {
        UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
        UE_LOG(LogTemp, Warning, TEXT("PlayAttackAnimation: Another montage is playing: %s"), 
            CurrentMontage ? *CurrentMontage->GetName() : TEXT("Unknown"));
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
        UE_LOG(LogTemp, Warning, TEXT("No montage available for attack"));
        return 0.0f;
    }
    
    // Lock animation and play the attack montage
    LockAnimation(MontageToPlay);
    
    // Try to find the default slot name (usually "DefaultSlot")
    FName SlotName = NAME_None;
    if (MontageToPlay->SlotAnimTracks.Num() > 0)
    {
        SlotName = MontageToPlay->SlotAnimTracks[0].SlotName;
        UE_LOG(LogTemp, Warning, TEXT("Montage slot name: %s"), *SlotName.ToString());
    }
    
    // Get the montage's base play rate
    float MontagePlayRate = MontageToPlay->RateScale;
    UE_LOG(LogTemp, Warning, TEXT("Montage base rate scale: %.2f"), MontagePlayRate);
    
    // Apply the configurable speed scale
    float CompensatedPlayRate = AttackAnimationSpeedScale;
    
    // If the montage has a non-standard rate scale, compensate for it
    if (FMath::Abs(MontagePlayRate - 1.0f) > 0.01f)
    {
        CompensatedPlayRate = AttackAnimationSpeedScale / MontagePlayRate;
    }
    
    // Ensure we're not blending out too quickly and use specific slot
    float MontageLength = AnimInstance->Montage_Play(MontageToPlay, CompensatedPlayRate, EMontagePlayReturnType::MontageLength, 0.0f, true);
    
    // Force the montage to play at correct speed
    if (MontageLength > 0.0f)
    {
        // Ensure position is at start
        AnimInstance->Montage_SetPosition(MontageToPlay, 0.0f);
        
        // Log the actual play rate being used
        float ActualPlayRate = AnimInstance->Montage_GetPlayRate(MontageToPlay);
        UE_LOG(LogTemp, Warning, TEXT("Actual montage play rate after setting: %.2f"), ActualPlayRate);
    }
    
    if (MontageLength > 0.0f)
    {
        // Set up completion callback
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UAnimationComponent::OnMontageComplete);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
        
        // Get detailed montage info
        float CurrentPosition = AnimInstance->Montage_GetPosition(MontageToPlay);
        float PlayRate = AnimInstance->Montage_GetPlayRate(MontageToPlay);
        bool bIsPlaying = AnimInstance->Montage_IsPlaying(MontageToPlay);
        int32 NumSections = MontageToPlay->GetNumSections();
        
        UE_LOG(LogTemp, Warning, TEXT("Playing attack montage %s:"), *MontageToPlay->GetName());
        UE_LOG(LogTemp, Warning, TEXT("  - Length: %.3f seconds"), MontageLength);
        UE_LOG(LogTemp, Warning, TEXT("  - Current Position: %.3f"), CurrentPosition);
        UE_LOG(LogTemp, Warning, TEXT("  - Play Rate: %.2f"), PlayRate);
        UE_LOG(LogTemp, Warning, TEXT("  - Is Playing: %s"), bIsPlaying ? TEXT("Yes") : TEXT("No"));
        UE_LOG(LogTemp, Warning, TEXT("  - Number of Sections: %d"), NumSections);
        UE_LOG(LogTemp, Warning, TEXT("  - Start Time: %.3f"), GetWorld()->GetTimeSeconds());
        
        // Check for AnimNotifies that might stop the montage
        if (MontageToPlay->Notifies.Num() > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("  - Montage has %d notifies"), MontageToPlay->Notifies.Num());
            for (const FAnimNotifyEvent& Notify : MontageToPlay->Notifies)
            {
                UE_LOG(LogTemp, Warning, TEXT("    - Notify at time %.3f"), Notify.GetTriggerTime());
            }
        }
        
        // Log the compensated play rate we're using
        UE_LOG(LogTemp, Warning, TEXT("  - Compensated Play Rate: %.2f (base rate: %.2f)"), CompensatedPlayRate, MontagePlayRate);
        
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
    float EndTime = GetWorld()->GetTimeSeconds();
    float CurrentPosition = AnimInstance ? AnimInstance->Montage_GetPosition(Montage) : -1.0f;
    
    UE_LOG(LogTemp, Warning, TEXT("Montage complete callback:"));
    UE_LOG(LogTemp, Warning, TEXT("  - Montage: %s"), Montage ? *Montage->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("  - Interrupted: %s"), bInterrupted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  - End Time: %.3f"), EndTime);
    UE_LOG(LogTemp, Warning, TEXT("  - Final Position: %.3f"), CurrentPosition);
    
    if (bInterrupted)
    {
        UE_LOG(LogTemp, Error, TEXT("MONTAGE WAS INTERRUPTED! Something stopped it early."));
    }
    
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



