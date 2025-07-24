#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/TrinityFlowTypes.h"
#include "EnemyBase.generated.h"

UCLASS(Abstract)
class TRINITYFLOW_API AEnemyBase : public ACharacter
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
    class UAIStateMachine* AIStateMachine;

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
    class APawn* PlayerTarget;

public:
    UPROPERTY()
    bool bHasSeenPlayer = false;

protected:

    virtual void SetupEnemy();
    virtual void OnDeath();
    
    bool CanSeePlayer();
    void UpdateCombatState();
    void AttackPlayer();

    UFUNCTION()
    void OnHealthChanged(float NewHealth);

    UFUNCTION()
    void OnDeathEvent();

public:
    // Getters for AI
    UFUNCTION(BlueprintPure, Category = "AI")
    float GetSightRange() const { return SightRange; }

    UFUNCTION(BlueprintPure, Category = "AI")
    float GetAttackRange() const { return AttackRange; }

    UFUNCTION(BlueprintPure, Category = "AI")
    APawn* GetTargetPlayer() const { return PlayerTarget; }

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetTargetPlayer(APawn* NewTarget) { PlayerTarget = NewTarget; }

    UFUNCTION(BlueprintCallable, Category = "AI")
    void FaceTarget(AActor* Target);

    UFUNCTION(BlueprintPure, Category = "Components")
    UStateComponent* GetStateComponent() const { return StateComponent; }

    UFUNCTION(BlueprintPure, Category = "Components")
    UCombatComponent* GetCombatComponent() const { return CombatComponent; }

    UFUNCTION(BlueprintPure, Category = "AI")
    UAIStateMachine* GetAIStateMachine() const { return AIStateMachine; }

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSubclassOf<class UAIState> InitialStateClass;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float MovementSpeed = 300.0f;
};