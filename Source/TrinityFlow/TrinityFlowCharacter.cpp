// Copyright Epic Games, Inc. All Rights Reserved.

#include "TrinityFlowCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowCharacterStats.h"
#include "Combat/AbilityComponent.h"
#include "Player/OverrideKatana.h"
#include "Player/PhysicalKatana.h"
#include "Enemy/EnemyBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATrinityFlowCharacter

ATrinityFlowCharacter::ATrinityFlowCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Set up shoulder camera offset (right and up)
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 60.0f); // Forward, Right, Up offset
	
	// Additional camera boom settings for better shoulder view
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 10.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Create components
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	TagComponent = CreateDefaultSubobject<UTagComponent>(TEXT("TagComponent"));
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));

	PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATrinityFlowCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATrinityFlowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrinityFlowCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrinityFlowCharacter::Look);

		// Combat - Dual Katana attacks
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::LeftKatanaAttack);     // LMB - Soul damage
		EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::RightKatanaAttack); // RMB - Physical damage

		// Abilities
		EnhancedInputComponent->BindAction(AbilityQAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::AbilityQ);       // Q - Code Break
		EnhancedInputComponent->BindAction(AbilityEAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::AbilityE);       // E - Right katana ability / Interaction
		EnhancedInputComponent->BindAction(AbilityTabAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::AbilityTab);   // Tab - Echoes of Data
		EnhancedInputComponent->BindAction(AbilityRAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::AbilityR);       // R - Right katana ability 2

		// Defensive abilities
		EnhancedInputComponent->BindAction(LeftDefensiveAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::LeftDefensiveAbility);   // Shift - Scripted Dodge
		EnhancedInputComponent->BindAction(RightDefensiveAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::RightDefensiveAbility); // Space - Order / Jump
		EnhancedInputComponent->BindAction(RightDefensiveAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATrinityFlowCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATrinityFlowCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATrinityFlowCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Load player stats from subsystem
	UTrinityFlowCharacterStats* PlayerStats = nullptr;
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UTrinityFlowStatsSubsystem* StatsSubsystem = GameInstance->GetSubsystem<UTrinityFlowStatsSubsystem>())
		{
			PlayerStats = StatsSubsystem->GetPlayerStats();
		}
	}
	
	// Apply stats if found
	if (PlayerStats)
	{
		// Apply base stats
		if (HealthComponent)
		{
			FCharacterResources Resources = PlayerStats->GetCharacterResources();
			HealthComponent->SetResources(Resources);
		}
		
		// Apply tags
		if (TagComponent)
		{
			TagComponent->SetTags(PlayerStats->GetCharacterTags());
		}
		
		UE_LOG(LogTemplateCharacter, Log, TEXT("Player loaded stats from %s"), *PlayerStats->CharacterName);
	}
	else
	{
		// Fallback to default values if stats not found
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Player could not find stats, using defaults"));
		
		if (HealthComponent)
		{
			FCharacterResources PlayerResources(200.0f, 20.0f, 20.0f);
			HealthComponent->SetResources(PlayerResources);
		}

		if (TagComponent)
		{
			TagComponent->SetTags(ECharacterTag::HaveSoul | ECharacterTag::Armored);
		}
	}

	// Spawn dual katanas
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	// Spawn left katana (soul damage)
	LeftKatana = GetWorld()->SpawnActor<AOverrideKatana>(AOverrideKatana::StaticClass(), GetActorLocation(), GetActorRotation(), SpawnParams);
	if (LeftKatana)
	{
		LeftKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_l"));
	}

	// Spawn right katana (physical damage)
	RightKatana = GetWorld()->SpawnActor<APhysicalKatana>(APhysicalKatana::StaticClass(), GetActorLocation(), GetActorRotation(), SpawnParams);
	if (RightKatana)
	{
		RightKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r"));
	}
	
	// Register player's own health component for echo system (in case of self-damage or special abilities)
	if (HealthComponent)
	{
		HealthComponent->OnDamageDealt.AddDynamic(this, &ATrinityFlowCharacter::OnAnyDamageDealt);
	}
}

void ATrinityFlowCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if attack has completed
	if (bIsAttacking && GetWorld()->GetTimeSeconds() >= AttackEndTime)
	{
		OnAttackComplete();
	}

	// Update defensive window
	if (bDefensiveAbilityActive)
	{
		DefensiveWindowTimer += DeltaTime;
		
		// Check if window expired (1.5 seconds)
		if (DefensiveWindowTimer >= 1.5f)
		{
			// Window expired, take full damage
			bDefensiveAbilityActive = false;
			
			if (HealthComponent && PendingDamage > 0.0f)
			{
				FDamageInfo DamageInfo;
				DamageInfo.Amount = PendingDamage;
				DamageInfo.Type = PendingDamageType;
				DamageInfo.Instigator = PendingAttacker;
				
				FVector DamageDirection = PendingAttacker ? (GetActorLocation() - PendingAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
				HealthComponent->TakeDamage(DamageInfo, DamageDirection);
			}
			
			DefensiveWindowTimer = 0.0f;
			PendingDamage = 0.0f;
			PendingAttacker = nullptr;
		}
	}
}

void ATrinityFlowCharacter::LeftKatanaAttack()
{
	// Check if any attack is in progress
	if (bIsAttacking)
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot attack: Another attack in progress"));
		return;
	}
	
	if (LeftKatana)
	{
		bIsAttacking = true;
		CurrentAttackingWeapon = LeftKatana;
		AttackEndTime = GetWorld()->GetTimeSeconds() + LeftKatana->GetAttackDuration();
		
		AActor* Target = GetTargetInSight();
		LeftKatana->BasicAttack(Target);
		
		// Play left katana animation
		if (LeftKatanaAttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(LeftKatanaAttackMontage);
		}
	}
}

void ATrinityFlowCharacter::RightKatanaAttack()
{
	// Check if any attack is in progress
	if (bIsAttacking)
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot attack: Another attack in progress"));
		return;
	}
	
	if (RightKatana)
	{
		bIsAttacking = true;
		CurrentAttackingWeapon = RightKatana;
		AttackEndTime = GetWorld()->GetTimeSeconds() + RightKatana->GetAttackDuration();
		
		AActor* Target = GetTargetInSight();
		RightKatana->BasicAttack(Target);
		
		// Play right katana animation
		if (RightKatanaAttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(RightKatanaAttackMontage);
		}
	}
}

void ATrinityFlowCharacter::OnAttackComplete()
{
	bIsAttacking = false;
	CurrentAttackingWeapon = nullptr;
	AttackEndTime = 0.0f;
	UE_LOG(LogTemplateCharacter, VeryVerbose, TEXT("Attack completed, ready for next attack"));
}

void ATrinityFlowCharacter::AbilityQ()
{
	// Left katana ability 1 - Code Break
	if (LeftKatana)
	{
		AActor* Target = GetTargetInSight();
		LeftKatana->AbilityQ(Target);
	}
}

void ATrinityFlowCharacter::AbilityE()
{
	// Check if we're in non-combat state
	if (StateComponent && !StateComponent->HasState(ECharacterState::Combat))
	{
		// In non-combat state - play interaction animation
		if (InteractionMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(InteractionMontage);
			UE_LOG(LogTemplateCharacter, Log, TEXT("Playing interaction montage"));
		}
	}
	else if (RightKatana)
	{
		// In combat state - use right katana ability
		AActor* Target = GetTargetInSight();
		RightKatana->AbilityE(Target);
	}
}

void ATrinityFlowCharacter::AbilityTab()
{
	// Left katana ability 2 - Echoes of Data
	if (LeftKatana)
	{
		AActor* Target = GetTargetInSight();
		LeftKatana->AbilityTab(Target);
	}
}

void ATrinityFlowCharacter::AbilityR()
{
	// Right katana ability 2
	if (RightKatana)
	{
		AActor* Target = GetTargetInSight();
		RightKatana->AbilityR(Target);
	}
}

void ATrinityFlowCharacter::LeftDefensiveAbility()
{
	// Shift key - Scripted Dodge for left katana
	if (StateComponent && StateComponent->HasState(ECharacterState::Combat))
	{
		if (bDefensiveAbilityActive && LeftKatana)
		{
			// Let the katana handle its defensive ability
			LeftKatana->StartDodgeWindow(PendingDamage, PendingAttacker);
			
			// Process the dodge
			float DamageMultiplier = 1.0f;
			if (LeftKatana->ProcessDodge(DamageMultiplier))
			{
				// Perfect dodge
				LeftKatana->OnPerfectDodge();
			}
			
			// Apply damage if any
			if (HealthComponent && DamageMultiplier > 0.0f && PendingDamage > 0.0f)
			{
				FDamageInfo DamageInfo;
				DamageInfo.Amount = PendingDamage * DamageMultiplier;
				DamageInfo.Type = PendingDamageType;
				DamageInfo.Instigator = PendingAttacker;
				
				FVector DamageDirection = PendingAttacker ? (GetActorLocation() - PendingAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
				HealthComponent->TakeDamage(DamageInfo, DamageDirection);
			}
			
			// Reset defensive state
			bDefensiveAbilityActive = false;
			DefensiveWindowTimer = 0.0f;
			PendingDamage = 0.0f;
			PendingAttacker = nullptr;
		}
	}
	else
	{
		// Not in combat - no action
		UE_LOG(LogTemplateCharacter, Log, TEXT("Left defensive ability only works in combat"));
	}
}

void ATrinityFlowCharacter::RightDefensiveAbility()
{
	// Space key - Order for right katana
	if (StateComponent && StateComponent->HasState(ECharacterState::Combat))
	{
		if (bDefensiveAbilityActive && RightKatana)
		{
			// Let the katana handle its defensive ability
			RightKatana->StartOrderWindow(PendingDamage, PendingAttacker);
			
			// Process the order
			if (RightKatana->ProcessOrder())
			{
				// Perfect order executed
				RightKatana->OnPerfectOrder();
			}
			
			// Damage is handled inside ProcessOrder
			
			// Reset defensive state
			bDefensiveAbilityActive = false;
			DefensiveWindowTimer = 0.0f;
			PendingDamage = 0.0f;
			PendingAttacker = nullptr;
		}
	}
	else
	{
		// Not in combat - jump
		Jump();
	}
}

void ATrinityFlowCharacter::OnIncomingAttack(AActor* Attacker, float Damage, EDamageType DamageType)
{
	// Start defensive window
	bDefensiveAbilityActive = true;
	DefensiveWindowTimer = 0.0f;
	PendingAttacker = Attacker;
	PendingDamage = Damage;
	PendingDamageType = DamageType;
	DefensiveDamageMultiplier = 1.0f;
}

void ATrinityFlowCharacter::OnAnyDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnAnyDamageDealt: DamagedActor=%s, Damage=%.1f, Instigator=%s"), 
		DamagedActor ? *DamagedActor->GetName() : TEXT("NULL"),
		ActualDamage,
		DamageInstigator ? *DamageInstigator->GetName() : TEXT("NULL"));
	
	// Check if this damage was dealt by the player (could be this character or its pawn)
	bool bIsPlayerDamage = (DamageInstigator == this) || 
	                      (DamageInstigator && DamageInstigator->GetOwner() == this) ||
	                      (Cast<APawn>(DamageInstigator) && Cast<APawn>(DamageInstigator)->GetController() && 
	                       Cast<APawn>(DamageInstigator)->GetController()->GetPawn() == this);
	
	if (bIsPlayerDamage && AbilityComponent && DamagedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Forwarding to AbilityComponent for echo processing"));
		AbilityComponent->OnActualDamageDealt(DamagedActor, ActualDamage, this, DamageType);
	}
}

void ATrinityFlowCharacter::RegisterEnemyDamageEvents(AEnemyBase* Enemy)
{
	if (Enemy)
	{
		if (UHealthComponent* HealthComp = Enemy->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->OnDamageDealt.AddDynamic(this, &ATrinityFlowCharacter::OnAnyDamageDealt);
			UE_LOG(LogTemp, Warning, TEXT("Registered damage events for enemy: %s"), *Enemy->GetName());
		}
	}
}

AActor* ATrinityFlowCharacter::GetTargetInSight()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * 10000.0f);

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	// Use visibility channel for better hit detection
	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();
		
		// Check if it's an enemy
		if (HitActor && HitActor->FindComponentByClass<UHealthComponent>() && HitActor != this)
		{
			return HitActor;
		}
	}

	return nullptr;
}
