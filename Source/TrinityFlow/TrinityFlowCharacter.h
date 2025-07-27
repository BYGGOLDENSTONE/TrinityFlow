// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Core/TrinityFlowTypes.h"
#include "TrinityFlowCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATrinityFlowCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack Input Action (Left Mouse Button) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Right Attack Input Action (Right Mouse Button) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightAttackAction;

	/** Ability Q Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityQAction;

	/** Ability E Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityEAction;

	/** Switch Weapon Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchWeaponAction;

public:
	ATrinityFlowCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION()
	class UStateComponent* GetStateComponent() const { return StateComponent; }

	UFUNCTION()
	class UTagComponent* GetTagComponent() const { return TagComponent; }

	UFUNCTION()
	AActor* GetTargetInSight();

	UFUNCTION()
	bool IsKatanaActive() const { return bIsKatanaActive; }

	UFUNCTION()
	class AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
	
	// Defensive ability handling
	UFUNCTION()
	void OnIncomingAttack(AActor* Attacker, float Damage, EDamageType DamageType);

	UFUNCTION()
	bool IsDefensiveAbilityActive() const { return bDefensiveAbilityActive; }

	UFUNCTION()
	float GetDefensiveDamageMultiplier() const { return DefensiveDamageMultiplier; }
	
	UFUNCTION()
	void OnAnyDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType);
	
	UFUNCTION()
	void RegisterEnemyDamageEvents(class AEnemyBase* Enemy);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for attack input */
	void Attack();
	void RightAttack();

	/** Called for ability inputs */
	void AbilityQ();
	void AbilityE();

	/** Called for weapon switch */
	void SwitchWeapon();

	/** Called for defensive ability (contextual jump/defense) */
	void DefensiveAbility();

	/** Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTagComponent* TagComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAbilityComponent* AbilityComponent;

	/** Weapons */
	UPROPERTY()
	class AWeaponBase* CurrentWeapon;

	UPROPERTY()
	class AOverrideKatana* OverrideKatana;

	UPROPERTY()
	class ADivineAnchor* DivineAnchor;

	bool bIsKatanaActive = true;

	// Combat Animation Montages (to be set in Blueprint)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	class UAnimMontage* LeftAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	class UAnimMontage* RightAttackMontage;
	
	// Defensive ability state
	bool bDefensiveAbilityActive = false;
	float DefensiveDamageMultiplier = 1.0f;
	float DefensiveWindowTimer = 0.0f;
	AActor* PendingAttacker = nullptr;
	float PendingDamage = 0.0f;
	EDamageType PendingDamageType = EDamageType::Physical;
			

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

