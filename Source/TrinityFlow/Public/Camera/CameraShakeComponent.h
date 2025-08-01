#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraShakeComponent.generated.h"

UENUM(BlueprintType)
enum class ECameraShakeType : uint8
{
    Light,      // Small hits, footsteps
    Medium,     // Regular attacks
    Heavy,      // Heavy attacks, explosions
    Custom      // Custom parameters
};

/**
 * Component that handles camera shake/bump effects for cinematic feel
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UCameraShakeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCameraShakeComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Trigger camera shake
    UFUNCTION(BlueprintCallable, Category = "Camera Shake")
    void TriggerCameraShake(ECameraShakeType ShakeType = ECameraShakeType::Medium);
    
    UFUNCTION(BlueprintCallable, Category = "Camera Shake")
    void TriggerCustomShake(float Intensity, float Duration, float Frequency = 10.0f);

    // Trigger bump effect (single impulse)
    UFUNCTION(BlueprintCallable, Category = "Camera Shake")
    void TriggerCameraBump(FVector BumpDirection, float BumpIntensity = 1.0f);

protected:
    virtual void BeginPlay() override;

    // Shake parameters for different types
    UPROPERTY(EditAnywhere, Category = "Shake Settings")
    float LightShakeIntensity = 0.5f;
    
    UPROPERTY(EditAnywhere, Category = "Shake Settings")
    float MediumShakeIntensity = 1.0f;
    
    UPROPERTY(EditAnywhere, Category = "Shake Settings")
    float HeavyShakeIntensity = 2.0f;
    
    UPROPERTY(EditAnywhere, Category = "Shake Settings")
    float DefaultShakeDuration = 0.3f;
    
    UPROPERTY(EditAnywhere, Category = "Shake Settings")
    float ShakeFrequency = 15.0f;

    // Bump parameters
    UPROPERTY(EditAnywhere, Category = "Bump Settings")
    float BumpDecayRate = 5.0f;
    
    UPROPERTY(EditAnywhere, Category = "Bump Settings")
    float MaxBumpOffset = 50.0f;

private:
    // Current shake state
    bool bIsShaking = false;
    float CurrentShakeTime = 0.0f;
    float ShakeDuration = 0.0f;
    float ShakeIntensity = 0.0f;
    float CurrentFrequency = 0.0f;
    
    // Current bump state
    FVector CurrentBumpOffset = FVector::ZeroVector;
    FVector BumpVelocity = FVector::ZeroVector;
    
    // Cached references
    class APlayerController* PlayerController = nullptr;
    class APlayerCameraManager* CameraManager = nullptr;
    FRotator OriginalRotation;
    
    void ApplyShake(float DeltaTime);
    void ApplyBump(float DeltaTime);
    void UpdateCameraTransform();
};