#include "Camera/CameraShakeComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"

UCameraShakeComponent::UCameraShakeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UCameraShakeComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Get player controller
    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
        if (PlayerController)
        {
            CameraManager = PlayerController->PlayerCameraManager;
        }
    }
}

void UCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!CameraManager)
    {
        return;
    }
    
    // Apply shake effect
    if (bIsShaking)
    {
        ApplyShake(DeltaTime);
    }
    
    // Apply bump effect
    if (!CurrentBumpOffset.IsNearlyZero())
    {
        ApplyBump(DeltaTime);
    }
    
    // Update camera if any effect is active
    if (bIsShaking || !CurrentBumpOffset.IsNearlyZero())
    {
        UpdateCameraTransform();
    }
}

void UCameraShakeComponent::TriggerCameraShake(ECameraShakeType ShakeType)
{
    float Intensity = MediumShakeIntensity;
    
    switch (ShakeType)
    {
        case ECameraShakeType::Light:
            Intensity = LightShakeIntensity;
            break;
        case ECameraShakeType::Heavy:
            Intensity = HeavyShakeIntensity;
            break;
        case ECameraShakeType::Medium:
        default:
            Intensity = MediumShakeIntensity;
            break;
    }
    
    TriggerCustomShake(Intensity, DefaultShakeDuration, ShakeFrequency);
}

void UCameraShakeComponent::TriggerCustomShake(float Intensity, float Duration, float Frequency)
{
    if (!CameraManager)
    {
        return;
    }
    
    // Store original rotation if not shaking
    if (!bIsShaking)
    {
        OriginalRotation = CameraManager->GetCameraRotation();
    }
    
    bIsShaking = true;
    CurrentShakeTime = 0.0f;
    ShakeDuration = Duration;
    ShakeIntensity = Intensity;
    CurrentFrequency = Frequency;
}

void UCameraShakeComponent::TriggerCameraBump(FVector BumpDirection, float BumpIntensity)
{
    if (!CameraManager)
    {
        return;
    }
    
    // Normalize and apply intensity
    BumpDirection.Normalize();
    BumpVelocity = BumpDirection * BumpIntensity * MaxBumpOffset;
}

void UCameraShakeComponent::ApplyShake(float DeltaTime)
{
    CurrentShakeTime += DeltaTime;
    
    if (CurrentShakeTime >= ShakeDuration)
    {
        bIsShaking = false;
        CurrentShakeTime = 0.0f;
        return;
    }
    
    // Calculate shake progress (0 to 1)
    float Progress = CurrentShakeTime / ShakeDuration;
    
    // Fade out intensity towards the end
    float CurrentIntensity = ShakeIntensity * (1.0f - Progress);
    
    // Generate shake offset using perlin noise
    float Time = GetWorld()->GetTimeSeconds() * CurrentFrequency;
    
    float ShakeX = FMath::PerlinNoise1D(Time) * CurrentIntensity;
    float ShakeY = FMath::PerlinNoise1D(Time + 100.0f) * CurrentIntensity;
    float ShakeZ = FMath::PerlinNoise1D(Time + 200.0f) * CurrentIntensity * 0.5f; // Less vertical shake
    
    // Apply to camera rotation
    FRotator ShakeRotation(ShakeY, ShakeX, ShakeZ);
    CameraManager->SetActorRotation(OriginalRotation + ShakeRotation);
}

void UCameraShakeComponent::ApplyBump(float DeltaTime)
{
    // Apply decay to bump velocity
    BumpVelocity *= FMath::Exp(-BumpDecayRate * DeltaTime);
    
    // Update bump offset
    CurrentBumpOffset += BumpVelocity * DeltaTime;
    
    // Apply spring back force
    CurrentBumpOffset *= FMath::Exp(-BumpDecayRate * 0.5f * DeltaTime);
    
    // Clear if nearly zero
    if (CurrentBumpOffset.Size() < 0.1f)
    {
        CurrentBumpOffset = FVector::ZeroVector;
        BumpVelocity = FVector::ZeroVector;
    }
}

void UCameraShakeComponent::UpdateCameraTransform()
{
    if (!CameraManager)
    {
        return;
    }
    
    // Apply bump offset to camera location
    if (!CurrentBumpOffset.IsNearlyZero())
    {
        FVector CameraLocation = CameraManager->GetCameraLocation();
        CameraManager->SetActorLocation(CameraLocation + CurrentBumpOffset);
    }
}