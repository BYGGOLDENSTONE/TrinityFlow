#include "Enemy/EnemyBase.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Core/CombatComponent.h"
#include "Core/CombatStateManager.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowCharacterStats.h"
#include "../../TrinityFlowCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AI/AIStateMachine.h"
#include "AI/EnemyAIController.h"
#include "AI/AIState.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Create components
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    SetRootComponent(CapsuleComponent);
    CapsuleComponent->SetCapsuleHalfHeight(88.0f);
    CapsuleComponent->SetCapsuleRadius(34.0f);
    
    // Set collision properties for proper detection
    CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);  // Allow pawns to overlap

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComponent);
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    
    // Ensure mesh also blocks visibility for line traces
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    TagComponent = CreateDefaultSubobject<UTagComponent>(TEXT("TagComponent"));
    StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    
    // AI Components
    AIStateMachine = CreateDefaultSubobject<UAIStateMachine>(TEXT("AIStateMachine"));
    
    // Movement Component
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = CapsuleComponent;
    MovementComponent->MaxSpeed = MovementSpeed;
    MovementComponent->bConstrainToPlane = false;  // Don't constrain to plane, let navmesh handle it
    MovementComponent->Acceleration = 800.0f;
    MovementComponent->Deceleration = 800.0f;
    
    // Set AI Controller class
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // Default initial state will be set in blueprints
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    
    SetupEnemy();

    // Bind events
    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnHealthChanged);
        HealthComponent->OnDeath.AddDynamic(this, &AEnemyBase::OnDeathEvent);
    }

    // Attack speed and range are set in SetupEnemy() from stats

    // Find player
    PlayerTarget = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    // Ensure movement component is properly configured
    if (MovementComponent)
    {
        // Make sure UpdatedComponent is still set
        if (!MovementComponent->UpdatedComponent)
        {
            MovementComponent->SetUpdatedComponent(CapsuleComponent);
        }
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s: Movement component configured with speed %.0f, UpdatedComponent=%s"), 
            *GetName(), 
            MovementComponent->MaxSpeed,
            MovementComponent->UpdatedComponent ? TEXT("Valid") : TEXT("NULL"));
            
        // Ensure movement component is active
        MovementComponent->SetComponentTickEnabled(true);
        MovementComponent->SetActive(true);
    }
    
    // Initialize AI State Machine
    if (AIStateMachine)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s: Initializing AI State Machine"), *GetName());
        
        // Use the specified initial state or default to Idle
        if (InitialStateClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy %s: Using InitialStateClass: %s"), *GetName(), *InitialStateClass->GetName());
            AIStateMachine->Initialize(InitialStateClass);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Enemy %s: No InitialStateClass set!"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Enemy %s: No AIStateMachine component found!"), *GetName());
    }

    // Register with combat state manager
    if (UCombatStateManager* CombatManager = GetWorld()->GetSubsystem<UCombatStateManager>())
    {
        CombatManager->RegisterEnemy(this);
    }
    
    // Register damage events with player for echo system
    // Do this with a small delay to ensure everything is initialized
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        if (PlayerTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy %s attempting to register with player"), *GetName());
            if (ATrinityFlowCharacter* TrinityCharacter = Cast<ATrinityFlowCharacter>(PlayerTarget))
            {
                TrinityCharacter->RegisterEnemyDamageEvents(this);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy %s could not find player target"), *GetName());
        }
    }, 0.1f, false);
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // AI State Machine handles combat logic now
    // Remove old hardcoded behavior
    
    // Debug velocity for animation
    if (MovementComponent && !MovementComponent->Velocity.IsZero())
    {
        float Speed = MovementComponent->Velocity.Size();
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s: Velocity = %s, Speed = %.0f"), 
            *GetName(), *MovementComponent->Velocity.ToString(), Speed);
    }
}

void AEnemyBase::SetupEnemy()
{
    // Load stats from subsystem
    UTrinityFlowCharacterStats* StatsToUse = nullptr;
    
    // First check if we have an override
    if (OverrideStats)
    {
        StatsToUse = OverrideStats;
    }
    else
    {
        // Get from stats subsystem
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UTrinityFlowStatsSubsystem* StatsSubsystem = GameInstance->GetSubsystem<UTrinityFlowStatsSubsystem>())
            {
                StatsToUse = StatsSubsystem->GetCharacterStats(EnemyStatsID);
            }
        }
    }
    
    // Apply stats if found
    if (StatsToUse)
    {
        // Apply base stats
        if (HealthComponent)
        {
            FCharacterResources Resources = StatsToUse->GetCharacterResources();
            HealthComponent->SetResources(Resources);
        }
        
        // Apply combat properties
        AttackRange = StatsToUse->AttackRange;
        SightRange = StatsToUse->SightRange;
        bIsAreaDamage = StatsToUse->bIsAreaDamage;
        
        if (CombatComponent)
        {
            CombatComponent->SetAttackSpeed(1.0f / StatsToUse->AttackSpeed); // Convert to delay
            CombatComponent->SetAttackRange(AttackRange);
        }
        
        // Update movement speed if specified in stats
        if (MovementComponent && StatsToUse->MovementSpeed > 0.0f)
        {
            MovementSpeed = StatsToUse->MovementSpeed;
            MovementComponent->MaxSpeed = MovementSpeed;
            UE_LOG(LogTemp, Warning, TEXT("Enemy %s movement speed set to %.0f"), *GetName(), MovementSpeed);
        }
        
        // Apply tags
        if (TagComponent)
        {
            ECharacterTag LoadedTags = StatsToUse->GetCharacterTags();
            TagComponent->SetTags(LoadedTags);
            
            // Debug logging for tags
            int32 TagsAsInt = static_cast<int32>(LoadedTags);
            FString TagsString = TEXT("");
            if (EnumHasAnyFlags(LoadedTags, ECharacterTag::Shielded)) TagsString += TEXT("Shielded ");
            if (EnumHasAnyFlags(LoadedTags, ECharacterTag::Armored)) TagsString += TEXT("Armored ");
            if (EnumHasAnyFlags(LoadedTags, ECharacterTag::Ghost)) TagsString += TEXT("Ghost ");
            if (EnumHasAnyFlags(LoadedTags, ECharacterTag::Mechanical)) TagsString += TEXT("Mechanical ");
            if (EnumHasAnyFlags(LoadedTags, ECharacterTag::HaveSoul)) TagsString += TEXT("HaveSoul ");
            
            UE_LOG(LogTemp, Warning, TEXT("Enemy %s tags - Raw int: %d, Enum flags: %s"), 
                *GetName(), TagsAsInt, *TagsString);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Enemy %s loaded stats from %s"), *GetName(), *StatsToUse->CharacterName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s could not find stats for ID: %s"), *GetName(), *EnemyStatsID.ToString());
    }
}

void AEnemyBase::OnDeath()
{
    // Unregister from combat state manager
    if (UCombatStateManager* CombatManager = GetWorld()->GetSubsystem<UCombatStateManager>())
    {
        CombatManager->UnregisterEnemy(this);
    }

    // Override in derived classes for specific death behavior
    Destroy();
}

bool AEnemyBase::CanSeePlayer()
{
    if (!PlayerTarget)
    {
        return false;
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerTarget->GetActorLocation());
    if (DistanceToPlayer > SightRange)
    {
        return false;
    }

    // Line trace to check visibility
    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        GetActorLocation() + FVector(0, 0, 50), // Eye height
        PlayerTarget->GetActorLocation(),
        ECC_Visibility,
        QueryParams
    );

    return !bHit || Hit.GetActor() == PlayerTarget;
}

void AEnemyBase::UpdateCombatState()
{
    // This is now handled by the AI State Machine
    // Keeping empty for compatibility
}

void AEnemyBase::AttackPlayer()
{
    // This is now handled by the AI Attack State
    // Keeping empty for compatibility
}

void AEnemyBase::OnHealthChanged(float NewHealth)
{
    // Visual feedback
    DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 150), 
                   FString::Printf(TEXT("HP: %.0f"), NewHealth), 
                   nullptr, FColor::White, 0.5f);
}

void AEnemyBase::OnDeathEvent()
{
    OnDeath();
}

void AEnemyBase::FaceTarget(AActor* Target)
{
    if (!Target)
    {
        return;
    }
    
    FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    DirectionToTarget.Z = 0.0f;
    
    FRotator NewRotation = DirectionToTarget.Rotation();
    SetActorRotation(NewRotation);
}

float AEnemyBase::GetCurrentSpeed() const
{
    return MovementComponent ? MovementComponent->Velocity.Size() : 0.0f;
}

bool AEnemyBase::IsMoving() const
{
    return MovementComponent && !MovementComponent->Velocity.IsNearlyZero();
}