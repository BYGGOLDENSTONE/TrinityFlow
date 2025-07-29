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

	/** Ability Tab Input Action (Left Katana Ability 2) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityTabAction;

	/** Ability R Input Action (Right Katana Ability 2) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityRAction;

	/** Left Defensive Input Action (Shift) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftDefensiveAction;

	/** Right Defensive Input Action (Space) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightDefensiveAction;

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
	class AOverrideKatana* GetLeftKatana() const { return LeftKatana; }

	UFUNCTION()
	class APhysicalKatana* GetRightKatana() const { return RightKatana; }

	UFUNCTION()
	bool IsAttacking() const { return bIsAttacking; }
	
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

	UFUNCTION()
	void OnStateChanged(ECharacterState NewState);

	/** Called when attack completes */
	UFUNCTION()
	void OnAttackComplete();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for attack input */
	void LeftKatanaAttack();   // LMB - Soul damage
	void RightKatanaAttack();  // RMB - Physical damage

	/** Called for ability inputs */
	void AbilityQ();      // Left katana ability 1 (Code Break)
	void AbilityE();      // Right katana ability 1
	void AbilityTab();    // Left katana ability 2 (Echoes of Data)
	void AbilityR();      // Right katana ability 2

	/** Called for defensive abilities */
	void LeftDefensiveAbility();   // Shift - Scripted Dodge
	void RightDefensiveAbility();  // Space - Order

	/** Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTagComponent* TagComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStateComponent* StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAbilityComponent* AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAnimationComponent* AnimationComponent;

	/** Weapons */
	UPROPERTY()
	class AOverrideKatana* LeftKatana;  // Soul damage katana

	UPROPERTY()
	class APhysicalKatana* RightKatana;  // Physical damage katana

	// Attack coordination system
	UPROPERTY()
	bool bIsAttacking = false;

	UPROPERTY()
	class AWeaponBase* CurrentAttackingWeapon = nullptr;

	UPROPERTY()
	float AttackEndTime = 0.0f;
	
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

