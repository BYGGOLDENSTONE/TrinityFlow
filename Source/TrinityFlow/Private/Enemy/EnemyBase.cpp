#include "Enemy/EnemyBase.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Core/CombatComponent.h"
#include "Core/CombatStateManager.h"
#include "../../TrinityFlowCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create components
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    SetRootComponent(CapsuleComponent);
    CapsuleComponent->SetCapsuleHalfHeight(88.0f);
    CapsuleComponent->SetCapsuleRadius(34.0f);

    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComponent);
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    TagComponent = CreateDefaultSubobject<UTagComponent>(TEXT("TagComponent"));
    StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
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

    // Set attack speed
    if (CombatComponent)
    {
        CombatComponent->SetAttackSpeed(1.0f / 1.5f); // 1 attack per 1.5 seconds
        CombatComponent->SetAttackRange(AttackRange);
    }

    // Find player
    PlayerTarget = Cast<ATrinityFlowCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    // Register with combat state manager
    if (UCombatStateManager* CombatManager = GetWorld()->GetSubsystem<UCombatStateManager>())
    {
        CombatManager->RegisterEnemy(this);
    }
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateCombatState();

    if (StateComponent && StateComponent->HasState(ECharacterState::Combat) && PlayerTarget)
    {
        // Face player
        FVector DirectionToPlayer = (PlayerTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        DirectionToPlayer.Z = 0.0f;
        SetActorRotation(DirectionToPlayer.Rotation());

        // Try to attack
        AttackPlayer();
    }
}

void AEnemyBase::SetupEnemy()
{
    // Override in derived classes
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
    if (!StateComponent || !PlayerTarget)
    {
        return;
    }

    bool bCanSeePlayerNow = CanSeePlayer();

    if (bCanSeePlayerNow && !bHasSeenPlayer)
    {
        // Enter combat
        bHasSeenPlayer = true;
        StateComponent->RemoveState(ECharacterState::NonCombat);
        StateComponent->AddState(ECharacterState::Combat);

        // Notify combat state manager
        if (UCombatStateManager* CombatManager = GetWorld()->GetSubsystem<UCombatStateManager>())
        {
            CombatManager->OnEnemySeesPlayer(this);
        }

        // Visual feedback
        DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0, 0, 100), 50.0f, 12, FColor::Red, false, 2.0f);
    }
}

void AEnemyBase::AttackPlayer()
{
    if (!CombatComponent || !PlayerTarget || !CombatComponent->CanAttack())
    {
        return;
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerTarget->GetActorLocation());
    if (DistanceToPlayer <= AttackRange)
    {
        CombatComponent->StartAttack(PlayerTarget, AttackRange, EDamageType::Physical, bIsAreaDamage);
    }
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