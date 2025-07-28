#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "AnimationComponent.generated.h"

class UAnimMontage;
class UAnimInstance;

UENUM(BlueprintType)
enum class EAttackState : uint8
{
    None,
    FirstAttack,
    WaitingForCombo,
    SecondAttack,
    ComboAttack
};

DECLARE_DELEGATE_OneParam(FOnComboWindowStarted, int32);
DECLARE_DELEGATE(FOnComboWindowEnded);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnimationComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Animation Control
    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool PlayAttackAnimation(bool bIsLeftHand);

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayIdleMontage();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StopIdleMontage();

    UFUNCTION(BlueprintPure, Category = "Animation")
    bool CanPlayNewAnimation() const;

    UFUNCTION(BlueprintPure, Category = "Animation")
    bool IsAnimationLocked() const { return bIsAnimationLocked; }

    UFUNCTION(BlueprintPure, Category = "Animation")
    EAttackState GetCurrentAttackState() const { return CurrentAttackState; }

    // Combo System
    UFUNCTION(BlueprintCallable, Category = "Combo")
    void ResetCombo();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void OnMovementInput(const FVector& MovementVector);

    // Interaction
    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool PlayInteractionMontage();

    // Combat State
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCombatState(bool bInCombat);

    // Events
    FOnComboWindowStarted OnComboWindowStarted;
    FOnComboWindowEnded OnComboWindowEnded;

protected:
    // Animation State
    UPROPERTY()
    bool bIsAnimationLocked = false;

    UPROPERTY()
    UAnimMontage* CurrentLockedMontage = nullptr;

    // Combo System
    UPROPERTY()
    bool bLastAttackWasLeft = false;

    // Montage Collections - All montages centralized here
    // Right Attack Montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Right Attack", meta = (DisplayName = "Right Slash 1"))
    UAnimMontage* RightSlash1 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Right Attack", meta = (DisplayName = "Right Slash 1 Wait"))
    UAnimMontage* RightSlash1Wait = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Right Attack", meta = (DisplayName = "Right Slash 2"))
    UAnimMontage* RightSlash2 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Right Attack", meta = (DisplayName = "Right Slash Combo"))
    UAnimMontage* RightSlashCombo = nullptr;

    // Left Attack Montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Left Attack", meta = (DisplayName = "Left Slash 1"))
    UAnimMontage* LeftSlash1 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Left Attack", meta = (DisplayName = "Left Slash 1 Wait"))
    UAnimMontage* LeftSlash1Wait = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Left Attack", meta = (DisplayName = "Left Slash 2"))
    UAnimMontage* LeftSlash2 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|Left Attack", meta = (DisplayName = "Left Slash Combo"))
    UAnimMontage* LeftSlashCombo = nullptr;

    // General Montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|General", meta = (DisplayName = "Non-Combat Idle"))
    UAnimMontage* NonCombatIdleMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|General", meta = (DisplayName = "Combat Idle"))
    UAnimMontage* CombatIdleMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages|General", meta = (DisplayName = "Interaction"))
    UAnimMontage* InteractionMontage = nullptr;

    // Timing Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Timing", meta = (DisplayName = "Combo Window Duration", ClampMin = "0.1", ClampMax = "3.0"))
    float ComboWindowDuration = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Timing", meta = (DisplayName = "Spam Combo Threshold", ClampMin = "0.1", ClampMax = "1.0"))
    float SpamComboThreshold = 0.3f; // Time window to detect spam combo

    UPROPERTY(EditDefaultsOnly, Category = "Timing", meta = (DisplayName = "Attack Montage Play Rate", ClampMin = "0.1", ClampMax = "2.0"))
    float AttackMontagePlayRate = 1.0f;

    // Combat State
    UPROPERTY()
    bool bIsInCombat = false;

    // Movement Detection
    UPROPERTY()
    FVector LastMovementInput = FVector::ZeroVector;

    UPROPERTY()
    bool bMovementCancelsCombo = true;

    // Attack State Tracking
    UPROPERTY()
    EAttackState CurrentAttackState = EAttackState::None;

    UPROPERTY()
    float LastAttackTime = 0.0f;

    UPROPERTY()
    UAnimMontage* CurrentWaitMontage = nullptr;

    // Component References
    UPROPERTY()
    class USkeletalMeshComponent* CharacterMesh = nullptr;

    UPROPERTY()
    UAnimInstance* AnimInstance = nullptr;

private:
    // Internal Functions
    void LockAnimation(UAnimMontage* Montage);
    void UnlockAnimation();
    void PlayWaitMontage(bool bIsLeftHand);
    bool ShouldPlaySpamCombo();
    UAnimMontage* GetAttackMontage(bool bIsLeftHand, EAttackState State);

    UFUNCTION()
    void OnAttackMontageComplete(UAnimMontage* Montage, bool bInterrupted);

    UFUNCTION()
    void OnWaitMontageComplete(UAnimMontage* Montage, bool bInterrupted);

    // Idle Animation
    void UpdateIdleAnimation();
    FTimerHandle IdleLoopTimerHandle;
    UAnimMontage* GetCurrentIdleMontage() const;

    UFUNCTION()
    void RestartIdleAnimation();
};