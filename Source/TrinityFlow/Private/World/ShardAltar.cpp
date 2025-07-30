#include "World/ShardAltar.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/ShardComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

AShardAltar::AShardAltar()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create altar mesh
    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    RootComponent = AltarMesh;
    AltarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    AltarMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    // Create interaction zone
    InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
    InteractionZone->SetupAttachment(RootComponent);
    InteractionZone->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    InteractionZone->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
    InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionZone->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AShardAltar::BeginPlay()
{
    Super::BeginPlay();
    
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

    // Check if interactor has shards
    if (UShardComponent* ShardComp = Interactor->FindComponentByClass<UShardComponent>())
    {
        return ShardComp->GetInactiveShardCount(AltarType) >= MinShardsToActivate;
    }

    return false;
}

int32 AShardAltar::GetAvailableShards(AActor* Interactor) const
{
    if (!Interactor)
    {
        return 0;
    }

    if (UShardComponent* ShardComp = Interactor->FindComponentByClass<UShardComponent>())
    {
        return ShardComp->GetInactiveShardCount(AltarType);
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
        ActivationTimer = 0.0f;
        
        OnAltarInteractionEnded.Broadcast();
        OnActivationFailed(CurrentInteractor);
        
        CurrentInteractor = nullptr;
        InteractorShardComponent = nullptr;
        PendingShardsToActivate = 0;
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

void AShardAltar::CompleteActivation()
{
    if (!CurrentInteractor || !InteractorShardComponent)
    {
        CancelActivation();
        return;
    }

    // Activate the shards
    if (InteractorShardComponent->ActivateShards(AltarType, PendingShardsToActivate))
    {
        OnAltarActivated.Broadcast(AltarType, PendingShardsToActivate, CurrentInteractor);
        OnActivationCompleted(CurrentInteractor, PendingShardsToActivate);
        
        UE_LOG(LogTemp, Log, TEXT("Altar activated! %d %s shards activated."),
            PendingShardsToActivate,
            AltarType == EShardType::Soul ? TEXT("Soul") : TEXT("Power"));
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
        
        if (ActivationTimer >= HoldDuration)
        {
            CompleteActivation();
        }
    }
    // Other puzzle types would update here
}