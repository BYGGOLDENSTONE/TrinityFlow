#include "World/ShardAltar.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/ShardComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

AShardAltar::AShardAltar()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create interaction zone as root (like collision sphere in pickup)
    InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
    RootComponent = InteractionZone;
    InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionZone->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    // Create altar mesh as child of interaction zone
    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    AltarMesh->SetupAttachment(RootComponent);
    AltarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    AltarMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AShardAltar::BeginPlay()
{
    Super::BeginPlay();
    
    // Apply interaction zone size from editable properties
    if (InteractionZone)
    {
        InteractionZone->SetBoxExtent(InteractionZoneSize);
    }
    
    // Bind overlap events
    InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &AShardAltar::OnOverlapBegin);
    InteractionZone->OnComponentEndOverlap.AddDynamic(this, &AShardAltar::OnOverlapEnd);
}

void AShardAltar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsActivating && CurrentInteractor)
    {
        UpdateActivationProgress(DeltaTime);
    }
}

bool AShardAltar::CanActivate(AActor* Interactor) const
{
    if (!Interactor)
    {
        return false;
    }

    // Check if guardians need to be defeated
    if (bRequireGuardiansDefeated && !AreGuardiansDefeated())
    {
        return false;
    }

    // Check if interactor has any shards (either type)
    if (UShardComponent* ShardComp = Interactor->FindComponentByClass<UShardComponent>())
    {
        int32 TotalInactive = ShardComp->GetInactiveShardCount(EShardType::Soul) + 
                              ShardComp->GetInactiveShardCount(EShardType::Power);
        return TotalInactive >= MinShardsToActivate;
    }

    return false;
}

bool AShardAltar::IsPlayerInZone(AActor* Player) const
{
    return Player && OverlappingActors.Contains(Player);
}

int32 AShardAltar::GetAvailableShards(AActor* Interactor) const
{
    if (!Interactor)
    {
        return 0;
    }

    if (UShardComponent* ShardComp = Interactor->FindComponentByClass<UShardComponent>())
    {
        // Return total inactive shards of both types
        return ShardComp->GetInactiveShardCount(EShardType::Soul) + 
               ShardComp->GetInactiveShardCount(EShardType::Power);
    }

    return 0;
}

void AShardAltar::StartActivation(AActor* Interactor, int32 ShardsToActivate)
{
    if (!CanActivate(Interactor) || bIsActivating)
    {
        return;
    }

    int32 AvailableShards = GetAvailableShards(Interactor);
    PendingShardsToActivate = FMath::Clamp(ShardsToActivate, MinShardsToActivate, 
        FMath::Min(AvailableShards, MaxShardsPerActivation));

    CurrentInteractor = Interactor;
    InteractorShardComponent = Interactor->FindComponentByClass<UShardComponent>();
    bIsActivating = true;
    ActivationTimer = 0.0f;

    OnAltarInteractionStarted.Broadcast(Interactor);
    OnActivationStarted(Interactor);

    // If no puzzle, activate immediately
    if (PuzzleType == EAltarPuzzleType::None)
    {
        CompleteActivation();
    }
}

void AShardAltar::CancelActivation()
{
    if (bIsActivating)
    {
        bIsActivating = false;
        bIsSelectiveActivation = false;
        ActivationTimer = 0.0f;
        
        OnAltarInteractionEnded.Broadcast();
        OnActivationFailed(CurrentInteractor);
        
        CurrentInteractor = nullptr;
        InteractorShardComponent = nullptr;
        PendingShardsToActivate = 0;
        PendingSoulShardsToActivate = 0;
        PendingPowerShardsToActivate = 0;
    }
}

float AShardAltar::GetActivationProgress() const
{
    if (!bIsActivating || PuzzleType == EAltarPuzzleType::None)
    {
        return 0.0f;
    }

    if (PuzzleType == EAltarPuzzleType::HoldToActivate)
    {
        return FMath::Clamp(ActivationTimer / HoldDuration, 0.0f, 1.0f);
    }

    // Other puzzle types would have their own progress calculation
    return 0.0f;
}

void AShardAltar::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<APawn>())
    {
        OverlappingActors.Add(OtherActor);
    }
}

void AShardAltar::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    OverlappingActors.Remove(OtherActor);

    // Cancel activation if the interactor leaves
    if (OtherActor == CurrentInteractor)
    {
        CancelActivation();
    }
}

void AShardAltar::StartSelectiveActivation(AActor* Interactor, int32 SoulShardsToActivate, int32 PowerShardsToActivate)
{
    UE_LOG(LogTemp, Warning, TEXT("StartSelectiveActivation called - Soul: %d, Power: %d"), SoulShardsToActivate, PowerShardsToActivate);
    
    if (!Interactor || bIsActivating)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartSelectiveActivation failed - Interactor: %s, bIsActivating: %s"), 
            Interactor ? TEXT("Valid") : TEXT("NULL"), 
            bIsActivating ? TEXT("true") : TEXT("false"));
        return;
    }
    
    // Check if player has enough shards
    UShardComponent* ShardComp = Interactor->FindComponentByClass<UShardComponent>();
    if (!ShardComp)
    {
        return;
    }
    
    int32 InactiveSoul = ShardComp->GetInactiveShardCount(EShardType::Soul);
    int32 InactivePower = ShardComp->GetInactiveShardCount(EShardType::Power);
    
    // Clamp to available shards
    PendingSoulShardsToActivate = FMath::Clamp(SoulShardsToActivate, 0, InactiveSoul);
    PendingPowerShardsToActivate = FMath::Clamp(PowerShardsToActivate, 0, InactivePower);
    
    if (PendingSoulShardsToActivate == 0 && PendingPowerShardsToActivate == 0)
    {
        return;
    }
    
    CurrentInteractor = Interactor;
    InteractorShardComponent = ShardComp;
    bIsActivating = true;
    bIsSelectiveActivation = true;
    ActivationTimer = 0.0f;
    
    UE_LOG(LogTemp, Warning, TEXT("Altar activation started - PuzzleType: %d, HoldDuration: %.2f"), 
        (int32)PuzzleType, HoldDuration);
    
    OnAltarInteractionStarted.Broadcast(Interactor);
    OnActivationStarted(Interactor);
    
    // If no puzzle, activate immediately
    if (PuzzleType == EAltarPuzzleType::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("No puzzle - activating immediately"));
        CompleteSelectiveActivation();
    }
}

void AShardAltar::CompleteActivation()
{
    if (bIsSelectiveActivation)
    {
        CompleteSelectiveActivation();
        return;
    }
    
    if (!CurrentInteractor || !InteractorShardComponent)
    {
        CancelActivation();
        return;
    }

    // Activate all inactive shards of both types
    int32 SoulShardsActivated = 0;
    int32 PowerShardsActivated = 0;
    
    // Get inactive counts
    int32 InactiveSoul = InteractorShardComponent->GetInactiveShardCount(EShardType::Soul);
    int32 InactivePower = InteractorShardComponent->GetInactiveShardCount(EShardType::Power);
    
    // Activate all soul shards
    if (InactiveSoul > 0)
    {
        InteractorShardComponent->ActivateShards(EShardType::Soul, InactiveSoul);
        SoulShardsActivated = InactiveSoul;
    }
    
    // Activate all power shards
    if (InactivePower > 0)
    {
        InteractorShardComponent->ActivateShards(EShardType::Power, InactivePower);
        PowerShardsActivated = InactivePower;
    }
    
    if (SoulShardsActivated > 0 || PowerShardsActivated > 0)
    {
        OnAltarActivated.Broadcast(SoulShardsActivated, PowerShardsActivated, CurrentInteractor);
        OnActivationCompleted(CurrentInteractor, SoulShardsActivated + PowerShardsActivated);
        
        UE_LOG(LogTemp, Log, TEXT("Altar activated! Soul: %d, Power: %d shards activated."),
            SoulShardsActivated, PowerShardsActivated);
    }
    else
    {
        OnActivationFailed(CurrentInteractor);
    }

    // Reset state
    bIsActivating = false;
    ActivationTimer = 0.0f;
    CurrentInteractor = nullptr;
    InteractorShardComponent = nullptr;
    PendingShardsToActivate = 0;
    
    OnAltarInteractionEnded.Broadcast();
}

void AShardAltar::CompleteSelectiveActivation()
{
    if (!CurrentInteractor || !InteractorShardComponent)
    {
        CancelActivation();
        return;
    }
    
    int32 SoulShardsActivated = 0;
    int32 PowerShardsActivated = 0;
    
    // Activate soul shards
    if (PendingSoulShardsToActivate > 0)
    {
        InteractorShardComponent->ActivateShards(EShardType::Soul, PendingSoulShardsToActivate);
        SoulShardsActivated = PendingSoulShardsToActivate;
    }
    
    // Activate power shards
    if (PendingPowerShardsToActivate > 0)
    {
        InteractorShardComponent->ActivateShards(EShardType::Power, PendingPowerShardsToActivate);
        PowerShardsActivated = PendingPowerShardsToActivate;
    }
    
    if (SoulShardsActivated > 0 || PowerShardsActivated > 0)
    {
        OnAltarActivated.Broadcast(SoulShardsActivated, PowerShardsActivated, CurrentInteractor);
        OnActivationCompleted(CurrentInteractor, SoulShardsActivated + PowerShardsActivated);
        
        UE_LOG(LogTemp, Log, TEXT("Selective altar activation! Soul: %d, Power: %d shards activated."),
            SoulShardsActivated, PowerShardsActivated);
    }
    else
    {
        OnActivationFailed(CurrentInteractor);
    }
    
    // Reset state
    bIsActivating = false;
    bIsSelectiveActivation = false;
    ActivationTimer = 0.0f;
    CurrentInteractor = nullptr;
    InteractorShardComponent = nullptr;
    PendingShardsToActivate = 0;
    PendingSoulShardsToActivate = 0;
    PendingPowerShardsToActivate = 0;
    
    OnAltarInteractionEnded.Broadcast();
}

bool AShardAltar::AreGuardiansDefeated() const
{
    for (AActor* Guardian : GuardianEnemies)
    {
        if (Guardian && !Guardian->IsPendingKillPending())
        {
            return false;
        }
    }
    return true;
}

void AShardAltar::UpdateActivationProgress(float DeltaTime)
{
    if (PuzzleType == EAltarPuzzleType::HoldToActivate)
    {
        ActivationTimer += DeltaTime;
        
        UE_LOG(LogTemp, Warning, TEXT("Altar activation progress: %.2f / %.2f"), ActivationTimer, HoldDuration);
        
        if (ActivationTimer >= HoldDuration)
        {
            CompleteActivation();
        }
    }
    else if (PuzzleType == EAltarPuzzleType::None)
    {
        // Should have been completed immediately in StartSelectiveActivation
        UE_LOG(LogTemp, Warning, TEXT("Altar with None puzzle type still updating?"));
    }
    // Other puzzle types would update here
}