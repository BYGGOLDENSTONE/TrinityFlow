#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/TrinityFlowTypes.h"
#include "EnemyAnimationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UEnemyAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyAnimationComponent();

    virtual void BeginPlay() override;

    // Animation Montages
    UPROPERTY(EditDefaultsOnly, Category = "Animation|Hit Response")
    class UAnimMontage* LeftHitResponseMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation|Hit Response")
    class UAnimMontage* RightHitResponseMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation|Combat")
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation|Combat")
    class UAnimMontage* ParryResponseMontage;

    // Play hit response based on damage type
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayHitResponse(EDamageType DamageType, bool bIsLeftWeapon);

    // Play attack animation
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayAttackAnimation();

    // Play parry response when player successfully parries
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayParryResponse();

    // Check if any combat animation is playing
    UFUNCTION(BlueprintPure, Category = "Animation")
    bool IsPlayingCombatAnimation() const;

protected:
    // Cache references
    UPROPERTY()
    class ACharacter* OwnerCharacter;

    UPROPERTY()
    class UAnimInstance* AnimInstance;

    // Play montage with safety checks
    bool PlayMontage(UAnimMontage* Montage, float PlayRate = 1.0f, FName StartSection = NAME_None);

    // Called when any montage ends
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
    // Track if we're currently playing a combat animation
    bool bIsPlayingAnimation;
};