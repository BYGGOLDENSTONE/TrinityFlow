#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/TrinityFlowTypes.h"
#include "AbilityComponent.generated.h"

USTRUCT()
struct FEchoesData
{
    GENERATED_BODY()

    UPROPERTY()
    AActor* MarkedEnemy = nullptr;

    UPROPERTY()
    float RemainingTime = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void SetEchoesTarget(AActor* Target);

    UFUNCTION()
    AActor* GetEchoesTarget() const { return EchoesData.MarkedEnemy; }

    UFUNCTION()
    void OnDamageDealt(AActor* DamagedActor, const FDamageInfo& DamageInfo);
    
    UFUNCTION()
    void OnActualDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType);

protected:
    UPROPERTY()
    FEchoesData EchoesData;

    void ProcessEchoesDamage(AActor* DamagedActor, const FDamageInfo& DamageInfo);
    
    void ProcessEchoesDamageActual(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator);
};