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
#include "HUD/TrinityFlowHUD.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Core/AnimationComponent.h"
#include "Core/StanceComponent.h"
#include "Core/ShardComponent.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowCharacterStats.h"
#include "Combat/AbilityComponent.h"
#include "Player/OverrideKatana.h"
#include "Player/PhysicalKatana.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/EnemyAnimationComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "World/ShardAltar.h"
#include "TrinityFlowGameMode.h"
#include "HUD/TrinityFlowHUD.h"

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
	AnimationComponent = CreateDefaultSubobject<UAnimationComponent>(TEXT("AnimationComponent"));
	StanceComponent = CreateDefaultSubobject<UStanceComponent>(TEXT("StanceComponent"));
	ShardComponent = CreateDefaultSubobject<UShardComponent>(TEXT("ShardComponent"));

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

		// UI
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Started, this, &ATrinityFlowCharacter::PauseGame); // Escape - Pause menu
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATrinityFlowCharacter::GlobalDefensiveAbility()
{
	// Check if UI is blocking input
	if (IsUIBlockingInput())
	{
		return;
	}
	
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use defensive ability: Animation is locked"));
		return;
	}
	
	// Only works in combat
	if (!StateComponent || !StateComponent->HasState(ECharacterState::Combat))
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Global defensive ability only works in combat"));
		return;
	}
	
	// Check if already active
	if (bDefensiveAbilityActive)
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Defensive ability already active"));
		return;
	}
	
	// Get current stance from StanceComponent
	EStanceType CurrentStance = EStanceType::Balanced;
	if (StanceComponent)
	{
		CurrentStance = StanceComponent->GetCurrentStance();
	}
	
	// Determine which defensive ability to use based on stance
	switch (CurrentStance)
	{
		case EStanceType::Soul:
			// Soul Stance - Use Scripted Dodge (left defensive)
			UE_LOG(LogTemplateCharacter, Log, TEXT("Global Defense: Using Scripted Dodge (Soul Stance)"));
			if (LeftKatana)
			{
				LeftKatana->DefensiveAbility();
			}
			break;
			
		case EStanceType::Power:
			// Power Stance - Use Order (right defensive)
			UE_LOG(LogTemplateCharacter, Log, TEXT("Global Defense: Using Order (Power Stance)"));
			if (RightKatana)
			{
				RightKatana->DefensiveAbility();
			}
			break;
			
		case EStanceType::Balanced:
			// Balanced Stance - Use Flow Guard (both weapons)
			UE_LOG(LogTemplateCharacter, Log, TEXT("Global Defense: Using Flow Guard (Balanced Stance)"));
			// Activate both defensive abilities for balanced effect
			if (LeftKatana && RightKatana)
			{
				// Flow Guard provides combined benefits
				bDefensiveAbilityActive = true;
				DefensiveDamageMultiplier = 0.4f; // 60% damage reduction
				DefensiveAbilityType = TEXT("Flow Guard");
				DefensiveAbilityWindow = 1.5f; // 1.5 second window
				
				// Visual feedback for Flow Guard
				UE_LOG(LogTemplateCharacter, Log, TEXT("Flow Guard activated! 60%% damage reduction for 1.5 seconds"));
				
				// Set timer to deactivate
				GetWorldTimerManager().SetTimer(DefensiveAbilityTimer, [this]()
				{
					bDefensiveAbilityActive = false;
					DefensiveDamageMultiplier = 1.0f;
					DefensiveAbilityType = TEXT("");
					UE_LOG(LogTemplateCharacter, Log, TEXT("Flow Guard deactivated"));
				}, DefensiveAbilityWindow, false);
			}
			break;
	}
}

void ATrinityFlowCharacter::Move(const FInputActionValue& Value)
{
	// Check if UI is blocking input
	if (IsUIBlockingInput())
	{
		return;
	}
	
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

		// Notify AnimationComponent of movement
		if (AnimationComponent)
		{
			FVector Movement3D(MovementVector.X, MovementVector.Y, 0.0f);
			AnimationComponent->OnMovementInput(Movement3D);
		}
	}
}

void ATrinityFlowCharacter::Look(const FInputActionValue& Value)
{
	// Check if UI is blocking input
	if (IsUIBlockingInput())
	{
		return;
	}
	
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

	// Spawn weapons
	SpawnWeapons();
	
	// Register player's own health component for echo system (in case of self-damage or special abilities)
	if (HealthComponent)
	{
		HealthComponent->OnDamageDealt.AddDynamic(this, &ATrinityFlowCharacter::OnAnyDamageDealt);
	}

	// Setup AnimationComponent
	if (AnimationComponent)
	{
		// Set initial combat state based on StateComponent
		if (StateComponent)
		{
			AnimationComponent->SetCombatState(StateComponent->HasState(ECharacterState::Combat));
			
			// Register for state changes
			StateComponent->OnStateChanged.AddDynamic(this, &ATrinityFlowCharacter::OnStateChanged);
		}
	}
}

void ATrinityFlowCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Check if we need to cancel altar activation due to movement or combat
	if (CurrentAltar && CurrentAltar->IsActivating())
	{
		// Cancel if player entered combat
		if (StateComponent && StateComponent->HasState(ECharacterState::Combat))
		{
			CurrentAltar->CancelActivation();
			CurrentAltar = nullptr;
			UE_LOG(LogTemplateCharacter, Log, TEXT("Altar activation cancelled - entered combat"));
		}
		// Cancel if player moved away from altar
		else if (!CurrentAltar->IsPlayerInZone(this))
		{
			CurrentAltar->CancelActivation();
			CurrentAltar = nullptr;
			UE_LOG(LogTemplateCharacter, Log, TEXT("Altar activation cancelled - moved away"));
		}
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
	UE_LOG(LogTemplateCharacter, Warning, TEXT("=== LeftKatanaAttack Called ==="));
	
	if (IsUIBlockingInput())
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Attack blocked by UI"));
		return;
	}
	
	if (!LeftKatana)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("No LeftKatana weapon found!"));
		return;
	}
	
	if (bIsAttacking)
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Already attacking"));
		return;
	}
	
	// Try to play animation if component exists
	float MontageLength = 0.0f;
	if (AnimationComponent)
	{
		MontageLength = AnimationComponent->PlayAttackAnimation(true);
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Animation montage length: %.2f"), MontageLength);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No AnimationComponent found - attacking without animation"));
	}
	
	// If no animation or animation failed, use default duration
	if (MontageLength <= 0.0f)
	{
		MontageLength = 0.5f; // Default attack duration
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Using default attack duration: %.2f"), MontageLength);
	}
	
	// Execute attack regardless of animation
	bIsAttacking = true;
	CurrentAttackingWeapon = LeftKatana;
	AttackEndTime = GetWorld()->GetTimeSeconds() + MontageLength;
	
	AActor* Target = GetTargetInSight();
	LeftKatana->BasicAttack(Target);
	
	// Set timer to reset attack state
	GetWorld()->GetTimerManager().SetTimer(AttackResetTimer, this, &ATrinityFlowCharacter::OnAttackComplete, MontageLength, false);
	
	UE_LOG(LogTemplateCharacter, Warning, TEXT("Left katana attack executed! Target: %s"), Target ? *Target->GetName() : TEXT("None"));
}

void ATrinityFlowCharacter::RightKatanaAttack()
{
	UE_LOG(LogTemplateCharacter, Warning, TEXT("=== RightKatanaAttack Called ==="));
	
	if (IsUIBlockingInput())
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Attack blocked by UI"));
		return;
	}
	
	if (!RightKatana)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("No RightKatana weapon found!"));
		return;
	}
	
	if (bIsAttacking)
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Already attacking"));
		return;
	}
	
	// Try to play animation if component exists
	float MontageLength = 0.0f;
	if (AnimationComponent)
	{
		MontageLength = AnimationComponent->PlayAttackAnimation(false);
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Animation montage length: %.2f"), MontageLength);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No AnimationComponent found - attacking without animation"));
	}
	
	// If no animation or animation failed, use default duration
	if (MontageLength <= 0.0f)
	{
		MontageLength = 0.5f; // Default attack duration
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Using default attack duration: %.2f"), MontageLength);
	}
	
	// Execute attack regardless of animation
	bIsAttacking = true;
	CurrentAttackingWeapon = RightKatana;
	AttackEndTime = GetWorld()->GetTimeSeconds() + MontageLength;
	
	AActor* Target = GetTargetInSight();
	RightKatana->BasicAttack(Target);
	
	// Set timer to reset attack state
	GetWorld()->GetTimerManager().SetTimer(AttackResetTimer, this, &ATrinityFlowCharacter::OnAttackComplete, MontageLength, false);
	
	UE_LOG(LogTemplateCharacter, Warning, TEXT("Right katana attack executed! Target: %s"), Target ? *Target->GetName() : TEXT("None"));
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
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use ability: Animation is locked"));
		return;
	}
	
	// Left katana ability 1 - Code Break
	if (LeftKatana)
	{
		AActor* Target = GetTargetInSight();
		LeftKatana->AbilityQ(Target);
	}
}

void ATrinityFlowCharacter::AbilityE()
{
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use ability: Animation is locked"));
		return;
	}
	
	// First check if shard activation UI is open (handles both E and Triangle)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ATrinityFlowHUD* HUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
		{
			if (HUD->IsShardActivationUIOpen())
			{
				// Process shard activation
				HUD->ProcessShardActivation();
				return;
			}
		}
	}
	
	// Check if we're in non-combat state
	if (StateComponent && !StateComponent->HasState(ECharacterState::Combat))
	{
		// In non-combat state - check for altar interaction first
		CheckForNearbyAltar();
		
		if (CurrentAltar && CurrentAltar->CanActivate(this))
		{
			// Open the shard activation UI instead of auto-activating
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				if (ATrinityFlowHUD* HUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
				{
					HUD->OpenShardActivationUI(CurrentAltar);
					UE_LOG(LogTemplateCharacter, Log, TEXT("Opening shard activation UI"));
				}
			}
		}
		else if (AnimationComponent)
		{
			// No altar nearby - just play interaction animation
			if (AnimationComponent->PlayInteractionMontage())
			{
				UE_LOG(LogTemplateCharacter, Log, TEXT("Playing interaction montage"));
			}
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
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use ability: Animation is locked"));
		return;
	}
	
	// Left katana ability 2 - Echoes of Data
	if (LeftKatana)
	{
		AActor* Target = GetTargetInSight();
		LeftKatana->AbilityTab(Target);
	}
}

void ATrinityFlowCharacter::AbilityR()
{
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use ability: Animation is locked"));
		return;
	}
	
	// Right katana ability 2
	if (RightKatana)
	{
		AActor* Target = GetTargetInSight();
		RightKatana->AbilityR(Target);
	}
}

void ATrinityFlowCharacter::LeftDefensiveAbility()
{
	// Check if animation is locked
	if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use defensive ability: Animation is locked"));
		return;
	}
	
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
	if (IsUIBlockingInput())
	{
		return;
	}
	
	// Check if animation is locked (only for combat actions)
	if (StateComponent && StateComponent->HasState(ECharacterState::Combat))
	{
		if (AnimationComponent && !AnimationComponent->CanPlayNewAnimation())
		{
			UE_LOG(LogTemplateCharacter, Log, TEXT("Cannot use defensive ability: Animation is locked"));
			return;
		}
	}
	
	// Space key - Defensive ability based on stance
	if (StateComponent && StateComponent->HasState(ECharacterState::Combat))
	{
		if (bDefensiveAbilityActive)
		{
			// Get current stance to determine which defensive ability to use
			EStanceType CurrentStance = StanceComponent ? StanceComponent->GetCurrentStance() : EStanceType::Power;
			
			// Check timing for perfect/moderate/failed defense
			float TotalWindow = AnimationComponent ? AnimationComponent->GetTotalDefenseWindow() : 1.0f;
			float PerfectWindow = AnimationComponent ? AnimationComponent->GetPerfectWindowDuration() : 0.3f;
			
			bool bIsPerfect = DefensiveWindowTimer <= PerfectWindow;
			bool bIsModerate = DefensiveWindowTimer <= TotalWindow && !bIsPerfect;
			bool bIsLeftHand = (CurrentStance == EStanceType::Soul);
			
			// For Balanced stance, use Power stance animations temporarily
			if (CurrentStance == EStanceType::Balanced)
			{
				bIsLeftHand = false;
			}
			
			if (bIsPerfect || bIsModerate)
			{
				// Play defensive animation
				if (AnimationComponent)
				{
					AnimationComponent->PlayDefensiveAnimation(bIsLeftHand, bIsPerfect);
				}
				
				// Calculate damage reduction
				float DamageReduction = bIsPerfect ? 1.0f : 0.5f; // Perfect = 100% reduction, Moderate = 50%
				float ActualDamage = PendingDamage * (1.0f - DamageReduction);
				
				// Apply damage if any
				if (ActualDamage > 0 && HealthComponent)
				{
					FDamageInfo DamageInfo;
					DamageInfo.Amount = ActualDamage;
					DamageInfo.Type = PendingDamageType;
					DamageInfo.Instigator = PendingAttacker;
					
					FVector DamageDirection = PendingAttacker ? 
						(GetActorLocation() - PendingAttacker->GetActorLocation()).GetSafeNormal() : 
						FVector::ForwardVector;
					
					HealthComponent->TakeDamage(DamageInfo, DamageDirection);
				}
				
				// Show defense result text above the attacker
				FString DefenseResult = bIsPerfect ? TEXT("PERFECT DEFENSE!") : TEXT("BLOCKED!");
				FLinearColor TextColor = bIsPerfect ? FLinearColor::Green : FLinearColor::Yellow;
				
				// If perfect defense, trigger enemy parry response animation
				if (bIsPerfect && PendingAttacker)
				{
					if (UEnemyAnimationComponent* EnemyAnimComp = PendingAttacker->FindComponentByClass<UEnemyAnimationComponent>())
					{
						EnemyAnimComp->PlayParryResponse();
					}
				}
				
				// Get HUD and add floating text above attacker
				if (PendingAttacker)
				{
					if (APlayerController* PC = Cast<APlayerController>(GetController()))
					{
						if (ATrinityFlowHUD* TrinityHUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
						{
							// Add defense result text
							FVector AttackerLocation = PendingAttacker->GetActorLocation() + FVector(0, 0, 100);
							if (bIsPerfect)
							{
								TrinityHUD->AddFloatingText(AttackerLocation, TEXT("PERFECT DEFENSE!"), FLinearColor::Green);
							}
							else
							{
								TrinityHUD->AddFloatingText(AttackerLocation, TEXT("BLOCKED!"), FLinearColor::Yellow);
							}
						}
					}
				}
				
				UE_LOG(LogTemplateCharacter, Log, TEXT("%s - Damage reduced from %.1f to %.1f"), 
					*DefenseResult, PendingDamage, ActualDamage);
			}
			else
			{
				// Failed defense - take full damage
				if (HealthComponent)
				{
					FDamageInfo DamageInfo;
					DamageInfo.Amount = PendingDamage;
					DamageInfo.Type = PendingDamageType;
					DamageInfo.Instigator = PendingAttacker;
					
					FVector DamageDirection = PendingAttacker ? 
						(GetActorLocation() - PendingAttacker->GetActorLocation()).GetSafeNormal() : 
						FVector::ForwardVector;
					
					HealthComponent->TakeDamage(DamageInfo, DamageDirection);
				}
				
				// Show failed defense text above the attacker
				if (PendingAttacker)
				{
					if (APlayerController* PC = Cast<APlayerController>(GetController()))
					{
						if (ATrinityFlowHUD* TrinityHUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
						{
							FVector AttackerLocation = PendingAttacker->GetActorLocation() + FVector(0, 0, 100);
							TrinityHUD->AddFloatingText(AttackerLocation, TEXT("FAILED!"), FLinearColor::Red);
						}
					}
				}
				
				UE_LOG(LogTemplateCharacter, Log, TEXT("Failed defense - Full damage taken: %.1f"), PendingDamage);
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

void ATrinityFlowCharacter::OnStateChanged(ECharacterState NewState)
{
	// Update AnimationComponent based on combat state
	if (AnimationComponent)
	{
		bool bInCombat = (NewState & ECharacterState::Combat) != ECharacterState::None;
		AnimationComponent->SetCombatState(bInCombat);
		
		UE_LOG(LogTemplateCharacter, Log, TEXT("Combat state changed: %s"), bInCombat ? TEXT("In Combat") : TEXT("Out of Combat"));
	}
}

void ATrinityFlowCharacter::CheckForNearbyAltar()
{
	// Clear current altar first
	CurrentAltar = nullptr;
	
	// Find all altars in the world
	TArray<AActor*> FoundAltars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShardAltar::StaticClass(), FoundAltars);
	
	// Check if player is in any altar's interaction zone
	for (AActor* Actor : FoundAltars)
	{
		if (AShardAltar* Altar = Cast<AShardAltar>(Actor))
		{
			if (Altar->IsPlayerInZone(this))
			{
				CurrentAltar = Altar;
				break;
			}
		}
	}
}

bool ATrinityFlowCharacter::IsUIBlockingInput() const
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ATrinityFlowHUD* HUD = Cast<ATrinityFlowHUD>(PC->GetHUD()))
		{
			return HUD->IsShardActivationUIOpen();
		}
	}
	return false;
}

// Removed unused altar interaction methods - now handled directly in AbilityE()

void ATrinityFlowCharacter::SpawnWeapons()
{
	// Don't spawn weapons in editor preview/simulate modes
	if (!GetWorld() || !GetWorld()->IsGameWorld())
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("SpawnWeapons: Not in game world, skipping weapon spawn"));
		return;
	}
	
	// Check if game instance exists (it won't in simulate mode)
	if (!GetGameInstance())
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("SpawnWeapons: No game instance, skipping weapon spawn"));
		return;
	}
	
	// Spawn left katana (soul damage)
	if (LeftKatanaClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		
		LeftKatana = GetWorld()->SpawnActor<AOverrideKatana>(LeftKatanaClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		
		if (LeftKatana)
		{
			// Attach to left hand socket
			LeftKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocketName);
			UE_LOG(LogTemplateCharacter, Log, TEXT("Spawned and attached LeftKatana to socket: %s"), *LeftHandSocketName.ToString());
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to spawn LeftKatana from class"));
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("LeftKatanaClass not set - weapon will not spawn. Set it in Blueprint!"));
	}
	
	// Spawn right katana (physical damage)
	if (RightKatanaClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		
		RightKatana = GetWorld()->SpawnActor<APhysicalKatana>(RightKatanaClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		
		if (RightKatana)
		{
			// Attach to right hand socket
			RightKatana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocketName);
			UE_LOG(LogTemplateCharacter, Log, TEXT("Spawned and attached RightKatana to socket: %s"), *RightHandSocketName.ToString());
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to spawn RightKatana from class"));
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("RightKatanaClass not set - weapon will not spawn. Set it in Blueprint!"));
	}
}

void ATrinityFlowCharacter::PauseGame()
{
	// Get the game mode and request pause
	if (ATrinityFlowGameMode* GameMode = Cast<ATrinityFlowGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnPauseRequested();
	}
}
