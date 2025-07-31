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
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AIStateMachine.h"
#include "AI/EnemyAIController.h"
#include "AI/AIState.h"
#include "Enemy/EnemyAnimationComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Configure inherited capsule component
    GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
    GetCapsuleComponent()->SetCapsuleRadius(34.0f);
    
    // Set collision properties for proper detection
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);  // Allow pawns to overlap

    // Configure inherited mesh component
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    
    // Ensure mesh also blocks visibility for line traces
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // Create custom components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    TagComponent = CreateDefaultSubobject<UTagComponent>(TEXT("TagComponent"));
    StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    
    // AI Components
    AIStateMachine = CreateDefaultSubobject<UAIStateMachine>(TEXT("AIStateMachine"));
    AnimationComponent = CreateDefaultSubobject<UEnemyAnimationComponent>(TEXT("AnimationComponent"));
    
    // Set AI Controller class
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    
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
    
    // Initialize AI State Machine
    if (AIStateMachine)
    {
        // Use the specified initial state or default to Idle
        if (InitialStateClass)
        {
            AIStateMachine->Initialize(InitialStateClass);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Enemy %s: No InitialStateClass set!"), *GetName());
        }
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
        if (StatsToUse->MovementSpeed > 0.0f)
        {
            MovementSpeed = StatsToUse->MovementSpeed;
            GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
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