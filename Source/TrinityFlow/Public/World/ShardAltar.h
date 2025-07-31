#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/TrinityFlowTypes.h"
#include "ShardAltar.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UShardComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAltarActivated, int32, SoulShardsActivated, int32, PowerShardsActivated, AActor*, Activator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAltarInteractionStarted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAltarInteractionEnded);

UENUM(BlueprintType)
enum class EAltarPuzzleType : uint8
{
    None UMETA(DisplayName = "None - Instant Activation"),
    HoldToActivate UMETA(DisplayName = "Hold To Activate"),
    TimingChallenge UMETA(DisplayName = "Timing Challenge"),
    PatternMatch UMETA(DisplayName = "Pattern Match")
};

UCLASS()
class TRINITYFLOW_API AShardAltar : public AActor
{
    GENERATED_BODY()

public:
    AShardAltar();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Altar Configuration - Now accepts both shard types
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Altar")
    bool bAcceptsBothTypes = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Altar")
    int32 MaxShardsPerActivation = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Altar")
    int32 MinShardsToActivate = 1;

    // Puzzle Configuration
    // Simplified to always use 2-second hold after interaction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    EAltarPuzzleType PuzzleType = EAltarPuzzleType::HoldToActivate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    float HoldDuration = 2.0f;

    // Guardian Enemies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guardians")
    TArray<AActor*> GuardianEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guardians")
    bool bRequireGuardiansDefeated = true;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* AltarMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* InteractionZone;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAltarActivated OnAltarActivated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAltarInteractionStarted OnAltarInteractionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAltarInteractionEnded OnAltarInteractionEnded;

    // Interaction Interface
    UFUNCTION(BlueprintCallable, Category = "Altar")
    bool CanActivate(AActor* Interactor) const;
    
    UFUNCTION(BlueprintCallable, Category = "Altar")
    bool IsPlayerInZone(AActor* Player) const;

    UFUNCTION(BlueprintCallable, Category = "Altar")
    int32 GetAvailableShards(AActor* Interactor) const;

    UFUNCTION(BlueprintCallable, Category = "Altar")
    void StartActivation(AActor* Interactor, int32 ShardsToActivate);
    
    UFUNCTION(BlueprintCallable, Category = "Altar")
    void StartSelectiveActivation(AActor* Interactor, int32 SoulShardsToActivate, int32 PowerShardsToActivate);

    UFUNCTION(BlueprintCallable, Category = "Altar")
    void CancelActivation();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Altar")
    void OnActivationStarted(AActor* Activator);

    UFUNCTION(BlueprintImplementableEvent, Category = "Altar")
    void OnActivationCompleted(AActor* Activator, int32 ShardsActivated);

    UFUNCTION(BlueprintImplementableEvent, Category = "Altar")
    void OnActivationFailed(AActor* Activator);

    // State Queries
    UFUNCTION(BlueprintCallable, Category = "Altar")
    bool IsActivating() const { return bIsActivating; }

    UFUNCTION(BlueprintCallable, Category = "Altar")
    float GetActivationProgress() const;

protected:
    // Overlap events
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY()
    AActor* CurrentInteractor = nullptr;

    UPROPERTY()
    UShardComponent* InteractorShardComponent = nullptr;

    bool bIsActivating = false;
    float ActivationTimer = 0.0f;
    int32 PendingShardsToActivate = 0;
    
    // Selective activation tracking
    int32 PendingSoulShardsToActivate = 0;
    int32 PendingPowerShardsToActivate = 0;
    bool bIsSelectiveActivation = false;

    TSet<AActor*> OverlappingActors;

    void CompleteActivation();
    void CompleteSelectiveActivation();
    bool AreGuardiansDefeated() const;
    void UpdateActivationProgress(float DeltaTime);
};