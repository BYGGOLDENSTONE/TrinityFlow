#include "Core/StateComponent.h"

UStateComponent::UStateComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update marked timer
    if (MarkedTimer > 0.0f)
    {
        MarkedTimer -= DeltaTime;
        if (MarkedTimer <= 0.0f)
        {
            RemoveState(ECharacterState::Marked);
            MarkedTimer = 0.0f;
        }
    }

    // Update vulnerable timer
    if (VulnerableTimer > 0.0f)
    {
        VulnerableTimer -= DeltaTime;
        if (VulnerableTimer <= 0.0f)
        {
            RemoveState(ECharacterState::Vulnerable);
            VulnerableTimer = 0.0f;
        }
    }
}

void UStateComponent::AddState(ECharacterState State)
{
    ECharacterState OldStates = States;
    States = States | State;
    
    if (States != OldStates)
    {
        OnStateChanged.Broadcast(States);
    }
}

void UStateComponent::RemoveState(ECharacterState State)
{
    ECharacterState OldStates = States;
    States = States & ~State;
    
    if (States != OldStates)
    {
        OnStateChanged.Broadcast(States);
    }
}

bool UStateComponent::HasState(ECharacterState State) const
{
    return EnumHasAnyFlags(States, State);
}

void UStateComponent::SetMarked(float Duration)
{
    AddState(ECharacterState::Marked);
    MarkedTimer = Duration;
}

void UStateComponent::SetVulnerable(float Duration)
{
    AddState(ECharacterState::Vulnerable);
    VulnerableTimer = Duration;
}