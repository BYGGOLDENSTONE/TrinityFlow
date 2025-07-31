#include "AI/States/AIState_Attack.h"
#include "Enemy/EnemyBase.h"
#include "AI/EnemyAIController.h"
#include "Core/CombatComponent.h"
#include "Core/StateComponent.h"
#include "Core/TrinityFlowTypes.h"
#include "Enemy/EnemyAnimationComponent.h"
#include "DrawDebugHelpers.h"

UAIState_Attack::UAIState_Attack()
{
	StateName = "Attack";
	TimeSinceLastAttack = 0.0f;
	bIsAttacking = false;
	
	// Default state transitions will be set in blueprints or via data assets
}

void UAIState_Attack::Enter(AEnemyBase* Enemy, AEnemyAIController* AIController)
{
	Super::Enter(Enemy, AIController);
	
	TimeSinceLastAttack = 0.0f;
	bIsAttacking = false;
	
	if (CachedAIController)
	{
		CachedAIController->StopMovement();
	}
	
	if (CachedEnemy && CachedEnemy->GetStateComponent())
	{
		CachedEnemy->GetStateComponent()->RemoveState(ECharacterState::NonCombat);
		CachedEnemy->GetStateComponent()->AddState(ECharacterState::Combat);
	}
}

void UAIState_Attack::Update(float DeltaTime)
{
	Super::Update(DeltaTime);

	if (!CachedEnemy || !CachedAIController)
	{
		return;
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		if (IdleStateClass)
		{
			TransitionToState(IdleStateClass);
		}
		return;
	}

	if (!IsTargetInRange())
	{
		if (ChaseStateClass)
		{
			TransitionToState(ChaseStateClass);
		}
		return;
	}

	CachedEnemy->FaceTarget(Target);

	UCombatComponent* CombatComp = CachedEnemy->GetCombatComponent();
	if (!CombatComp)
	{
		return;
	}

	if (!bIsAttacking && CombatComp->CanAttack())
	{
		PerformAttack();
		bIsAttacking = true;
		TimeSinceLastAttack = 0.0f;
	}
	else if (bIsAttacking)
	{
		TimeSinceLastAttack += DeltaTime;
		
		if (TimeSinceLastAttack >= AttackCooldown)
		{
			bIsAttacking = false;
		}
	}
}

void UAIState_Attack::Exit()
{
	Super::Exit();
	
	bIsAttacking = false;
}

void UAIState_Attack::PerformAttack()
{
	if (!CachedEnemy)
	{
		return;
	}

	// Play attack animation
	if (UEnemyAnimationComponent* AnimComp = CachedEnemy->FindComponentByClass<UEnemyAnimationComponent>())
	{
		AnimComp->PlayAttackAnimation();
	}

	UCombatComponent* CombatComp = CachedEnemy->GetCombatComponent();
	if (CombatComp)
	{
		APawn* Target = CachedEnemy->GetTargetPlayer();
		if (Target)
		{
			CombatComp->StartAttack(Target, CachedEnemy->GetAttackRange(), EDamageType::Physical, false);
		}
	}

	#if WITH_EDITOR
	if (CachedEnemy->GetWorld()->WorldType == EWorldType::Editor)
	{
		APawn* Target = CachedEnemy->GetTargetPlayer();
		if (Target)
		{
			DrawDebugLine(CachedEnemy->GetWorld(), 
				CachedEnemy->GetActorLocation() + FVector(0, 0, 50),
				Target->GetActorLocation() + FVector(0, 0, 50),
				FColor::Red, false, 1.0f, 0, 5.0f);
		}
	}
	#endif
}

bool UAIState_Attack::IsTargetInRange()
{
	if (!CachedEnemy)
	{
		return false;
	}

	APawn* Target = CachedEnemy->GetTargetPlayer();
	if (!Target)
	{
		return false;
	}

	float Distance = FVector::Dist(CachedEnemy->GetActorLocation(), Target->GetActorLocation());
	float AttackRange = FMath::Min(CachedEnemy->GetAttackRange(), MaxAttackRange);
	
	return Distance <= AttackRange;
}