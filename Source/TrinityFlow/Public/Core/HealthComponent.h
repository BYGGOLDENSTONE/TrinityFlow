#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    virtual void BeginPlay() override;

    UFUNCTION()
    void TakeDamage(const FDamageInfo& DamageInfo, const FVector& DamageDirection);

    UFUNCTION()
    float GetHealth() const { return Resources.Health; }

    UFUNCTION()
    float GetMaxHealth() const { return Resources.MaxHealth; }

    UFUNCTION()
    float GetHealthPercentage() const { return Resources.Health / Resources.MaxHealth; }

    UFUNCTION()
    bool IsAlive() const { return Resources.Health > 0.0f; }

    UFUNCTION()
    void SetResources(const FCharacterResources& NewResources);

    UFUNCTION()
    const FCharacterResources& GetResources() const { return Resources; }

    UPROPERTY()
    FOnHealthChanged OnHealthChanged;

    UPROPERTY()
    FOnDeath OnDeath;

protected:
    UPROPERTY()
    FCharacterResources Resources;

private:
    class UTagComponent* TagComponent;
};