#include "World/ShardPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Core/ShardComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

AShardPickup::AShardPickup()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetSphereRadius(50.0f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    RootComponent = CollisionSphere;

    // Create mesh component
    ShardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShardMesh"));
    ShardMesh->SetupAttachment(RootComponent);
    ShardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ShardMesh->SetRelativeScale3D(FVector(0.5f)); // Default scale

    // Create rotating movement component
    RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
    RotatingMovement->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
}

void AShardPickup::BeginPlay()
{
    Super::BeginPlay();
    
    InitialLocation = GetActorLocation();
    
    // Bind overlap event
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AShardPickup::OnOverlapBegin);
}

void AShardPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Float up and down
    FloatTimer += DeltaTime;
    float ZOffset = FMath::Sin(FloatTimer * FloatSpeed) * FloatAmplitude;
    FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, ZOffset);
    SetActorLocation(NewLocation);
}

void AShardPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<APawn>())
    {
        HandlePickup(OtherActor);
    }
}

void AShardPickup::HandlePickup(AActor* Collector)
{
    if (!Collector)
    {
        return;
    }

    // Find ShardComponent on the collector
    if (UShardComponent* ShardComp = Collector->FindComponentByClass<UShardComponent>())
    {
        // Collect the shard
        ShardComp->CollectShard(ShardType);
        
        // Fire Blueprint event for effects
        OnShardCollected();
        
        // Hide the shard
        SetShardVisibility(false);
        
        // Handle respawn or destruction
        if (bRespawns)
        {
            GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, 
                &AShardPickup::RespawnShard, RespawnTime, false);
        }
        else
        {
            // Small delay for effects to play
            FTimerHandle DestroyTimer;
            GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this]()
            {
                Destroy();
            }, 0.5f, false);
        }
    }
}

void AShardPickup::RespawnShard()
{
    SetShardVisibility(true);
    FloatTimer = 0.0f;
}

void AShardPickup::SetShardVisibility(bool bVisible)
{
    SetActorHiddenInGame(!bVisible);
    SetActorEnableCollision(bVisible);
    SetActorTickEnabled(bVisible);
}