#include "Core/CombatStateManager.h"
#include "Enemy/EnemyBase.h"
#include "../../TrinityFlowCharacter.h"
#include "Core/StateComponent.h"
#include "Kismet/GameplayStatics.h"

void UCombatStateManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UCombatStateManager::Deinitialize()
{
    Super::Deinitialize();
}

void UCombatStateManager::RegisterEnemy(AEnemyBase* Enemy)
{
    if (Enemy && !RegisteredEnemies.Contains(Enemy))
    {
        RegisteredEnemies.Add(Enemy);
    }
}

void UCombatStateManager::UnregisterEnemy(AEnemyBase* Enemy)
{
    RegisteredEnemies.Remove(Enemy);
    AlertedEnemies.Remove(Enemy);
    UpdateCombatState();
}

void UCombatStateManager::OnEnemySeesPlayer(AEnemyBase* Enemy)
{
    if (Enemy && !AlertedEnemies.Contains(Enemy))
    {
        AlertedEnemies.Add(Enemy);
        UpdateCombatState();
    }
}

void UCombatStateManager::UpdateCombatState()
{
    bool bNewCombatState = AlertedEnemies.Num() > 0;

    if (bNewCombatState != bIsInCombat)
    {
        bIsInCombat = bNewCombatState;
        OnCombatStateChanged.Broadcast(bIsInCombat);

        // Update player state
        if (ATrinityFlowCharacter* Player = Cast<ATrinityFlowCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
        {
            if (UStateComponent* StateComp = Player->GetStateComponent())
            {
                if (bIsInCombat)
                {
                    StateComp->RemoveState(ECharacterState::NonCombat);
                    StateComp->AddState(ECharacterState::Combat);
                }
                else
                {
                    StateComp->RemoveState(ECharacterState::Combat);
                    StateComp->AddState(ECharacterState::NonCombat);
                }
            }
        }
    }
}