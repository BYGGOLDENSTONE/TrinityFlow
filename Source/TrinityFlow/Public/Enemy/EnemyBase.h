#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Core/TrinityFlowTypes.h"
#include "EnemyBase.generated.h"

UCLASS(Abstract)
class TRINITYFLOW_API AEnemyBase : public APawn
{
    GENERATED_BODY()

public:
    AEnemyBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UHealthComponent* HealthComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UTagComponent* TagComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UStateComponent* StateComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UCombatComponent* CombatComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UCapsuleComponent* CapsuleComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class USkeletalMeshComponent* MeshComponent;

    // Stats Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    FName EnemyStatsID = "StandardEnemy";

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    class UTrinityFlowCharacterStats* OverrideStats;

    // Runtime stats (loaded from data)
    UPROPERTY()
    float SightRange = 1500.0f;

    UPROPERTY()
    float AttackRange = 300.0f;

    UPROPERTY()
    bool bIsAreaDamage = false;

    UPROPERTY()
    class ATrinityFlowCharacter* PlayerTarget;

    UPROPERTY()
    bool bHasSeenPlayer = false;

    virtual void SetupEnemy();
    virtual void OnDeath();
    
    bool CanSeePlayer();
    void UpdateCombatState();
    void AttackPlayer();

    UFUNCTION()
    void OnHealthChanged(float NewHealth);

    UFUNCTION()
    void OnDeathEvent();
};