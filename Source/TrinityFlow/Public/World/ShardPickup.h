#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrinityFlowTypes.h"
#include "ShardPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;

UCLASS()
class TRINITYFLOW_API AShardPickup : public AActor
{
    GENERATED_BODY()

public:
    AShardPickup();

    virtual void BeginPlay() override;

    // Called when player overlaps with pickup
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
        const FHitResult& SweepResult);

    // Shard Type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shard")
    EShardType ShardType = EShardType::Power;

    // Visual Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShardMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    URotatingMovementComponent* RotatingMovement;

    // Respawn Settings (for testing)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
    bool bRespawns = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn", meta = (EditCondition = "bRespawns"))
    float RespawnTime = 10.0f;

    // Visual Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatAmplitude = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float FloatSpeed = 2.0f;

    // Collection Effects (implement in Blueprint)
    UFUNCTION(BlueprintImplementableEvent, Category = "Shard")
    void OnShardCollected();

protected:
    virtual void Tick(float DeltaTime) override;

private:
    FVector InitialLocation;
    float FloatTimer = 0.0f;

    void HandlePickup(AActor* Collector);
    void RespawnShard();
    void SetShardVisibility(bool bVisible);

    FTimerHandle RespawnTimerHandle;
};