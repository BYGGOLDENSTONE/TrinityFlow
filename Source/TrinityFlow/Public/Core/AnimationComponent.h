#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "AnimationComponent.generated.h"

class UAnimMontage;
class UAnimInstance;

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
    float PlayAttackAnimation(bool bIsLeftHand);

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StartWonderingTimer();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StopWonderingTimer();

    UFUNCTION(BlueprintPure, Category = "Animation")
    bool CanPlayNewAnimation() const;

    UFUNCTION(BlueprintPure, Category = "Animation")
    bool IsAnimationLocked() const { return bIsAnimationLocked; }

    // Movement Detection
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void OnMovementInput(const FVector& MovementVector);

    // Interaction
    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool PlayInteractionMontage();

    // Combat State
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCombatState(bool bInCombat);

protected:
    // Animation State
    UPROPERTY()
    bool bIsAnimationLocked = false;

    UPROPERTY()
    UAnimMontage* CurrentLockedMontage = nullptr;

    // Montages - Only 4 essential montages
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (DisplayName = "Right Slash"))
    UAnimMontage* RightSlash1 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (DisplayName = "Left Slash"))
    UAnimMontage* LeftSlash1 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (DisplayName = "Interaction"))
    UAnimMontage* InteractionMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (DisplayName = "Wondering"))
    UAnimMontage* WonderingMontage = nullptr;

    // Wondering System
    UPROPERTY(EditDefaultsOnly, Category = "Timing", meta = (DisplayName = "Wondering Delay", ClampMin = "1.0", ClampMax = "10.0"))
    float WonderingDelay = 5.0f;
    
    // Attack Animation Settings
    UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (DisplayName = "Attack Animation Speed Scale", ClampMin = "0.1", ClampMax = "2.0"))
    float AttackAnimationSpeedScale = 1.0f;

    UPROPERTY()
    FTimerHandle WonderingTimerHandle;

    UPROPERTY()
    bool bIsWondering = false;

    // Combat State
    UPROPERTY()
    bool bIsInCombat = false;

    // Movement Detection
    UPROPERTY()
    FVector LastMovementInput = FVector::ZeroVector;

    // Component References
    UPROPERTY()
    class USkeletalMeshComponent* CharacterMesh = nullptr;

    UPROPERTY()
    UAnimInstance* AnimInstance = nullptr;

private:
    // Internal Functions
    void LockAnimation(UAnimMontage* Montage);
    void UnlockAnimation();

    UFUNCTION()
    void OnMontageComplete(UAnimMontage* Montage, bool bInterrupted);

    UFUNCTION()
    void PlayWonderingAnimation();

    // Wondering System
    void UpdateWonderingTimer();
};