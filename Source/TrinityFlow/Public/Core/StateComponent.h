#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrinityFlowTypes.h"
#include "StateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, ECharacterState, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRINITYFLOW_API UStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStateComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void AddState(ECharacterState State);

    UFUNCTION()
    void RemoveState(ECharacterState State);

    UFUNCTION()
    bool HasState(ECharacterState State) const;

    UFUNCTION()
    ECharacterState GetStates() const { return States; }

    UFUNCTION()
    void SetMarked(float Duration);

    UFUNCTION()
    bool IsMarked() const { return HasState(ECharacterState::Marked); }

    UFUNCTION()
    float GetMarkedTimeRemaining() const { return MarkedTimer; }

    UPROPERTY()
    FOnStateChanged OnStateChanged;

protected:
    UPROPERTY()
    ECharacterState States = ECharacterState::NonCombat;

    UPROPERTY()
    float MarkedTimer = 0.0f;

    UPROPERTY()
    float VulnerableTimer = 0.0f;

public:
    UFUNCTION()
    void SetVulnerable(float Duration);
};