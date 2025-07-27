#include "Player/DivineAnchor.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Core/TrinityFlowStatsSubsystem.h"
#include "Data/TrinityFlowAnchorStats.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBase.h"

ADivineAnchor::ADivineAnchor()
{
    // Default values - will be overridden by stats subsystem
    BasicAttackRange = 400.0f; // Area damage range
    BasicAttackSpeed = 0.5f; // 0.5 attacks per second
    BasicDamageType = EDamageType::Physical;
    
    AbilityQCooldown = 6.0f; // Gravity Pull
    AbilityECooldown = 10.0f; // Holy Gravity
}

void ADivineAnchor::BeginPlay()
{
    Super::BeginPlay();
    
    // Get stats from subsystem
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UTrinityFlowStatsSubsystem* StatsSubsystem = GameInstance->GetSubsystem<UTrinityFlowStatsSubsystem>())
        {
            WeaponStats = StatsSubsystem->GetAnchorStats("DivineAnchor");
            if (WeaponStats)
            {
                // Apply base weapon stats
                BasicAttackRange = WeaponStats->BasicAttackRange;
                BasicAttackSpeed = WeaponStats->BasicAttackSpeed;
                BasicDamageType = WeaponStats->BasicDamageType;
                
                // Apply ability cooldowns
                AbilityQCooldown = WeaponStats->AbilityQCooldown;
                AbilityECooldown = WeaponStats->AbilityECooldown;
                
                // Cache anchor stats pointer
                AnchorStats = &WeaponStats->AnchorStats;
                
                // Apply basic attack timing
                BasicAttackDamageDelay = AnchorStats->BasicAttackDamageDelay;
                
                UE_LOG(LogTemp, Log, TEXT("Divine Anchor loaded stats: Range=%.0f, Speed=%.1f"), 
                    BasicAttackRange, BasicAttackSpeed);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to load Divine Anchor stats from subsystem"));
            }
        }
    }
}

void ADivineAnchor::BasicAttack(AActor* Target)
{
    // Smash - Area damage with knockback
    if (!OwnerHealthComponent)
    {
        return;
    }

    // Find all enemies in range
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(this);

    GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(BasicAttackRange),
        QueryParams
    );

    for (const FOverlapResult& Result : OverlapResults)
    {
        if (AActor* HitActor = Result.GetActor())
        {
            if (UHealthComponent* TargetHealth = HitActor->FindComponentByClass<UHealthComponent>())
            {
                // Deal damage
                FDamageInfo DamageInfo;
                DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
                DamageInfo.Type = BasicDamageType;
                DamageInfo.Instigator = OwnerPawn;
                DamageInfo.bIsAreaDamage = true;

                FVector DamageDirection = (HitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                TargetHealth->TakeDamage(DamageInfo, DamageDirection);

                // Apply knockback using velocity-based system
                FVector KnockbackDirection = DamageDirection;
                float UpwardRatio = AnchorStats ? AnchorStats->SmashKnockbackUpwardRatio : 0.5f;
                KnockbackDirection.Z = UpwardRatio; // Add upward component for knockup effect
                KnockbackDirection.Normalize();
                
                // Check if this enemy is already being affected by physics
                bool bAlreadyAffected = false;
                for (int32 i = ActivePulls.Num() - 1; i >= 0; i--)
                {
                    if (ActivePulls[i].Target == HitActor)
                    {
                        // Update existing effect instead of adding new one
                        ActivePulls[i].PullVelocity = KnockbackDirection * 500.0f;
                        ActivePulls[i].PullTime = 0.0f;
                        ActivePulls[i].TotalPullDuration = 0.5f;
                        bAlreadyAffected = true;
                        break;
                    }
                }
                
                if (!bAlreadyAffected)
                {
                    // Create knockback data
                    FPullTarget Knockback;
                    Knockback.Target = HitActor;
                    Knockback.StartLocation = HitActor->GetActorLocation();
                    
                    // Use stats if available
                    float KnockbackForce = AnchorStats ? AnchorStats->SmashKnockbackForce : 500.0f;
                    float KnockbackDuration = AnchorStats ? AnchorStats->SmashKnockbackDuration : 0.5f;
                    
                    Knockback.PullVelocity = KnockbackDirection * KnockbackForce;
                    Knockback.PullTime = 0.0f;
                    Knockback.TotalPullDuration = KnockbackDuration;
                    
                    // Add to active pulls (reusing the system for knockback)
                    ActivePulls.Add(Knockback);
                    
                    UE_LOG(LogTemp, Warning, TEXT("Knockback started on %s with velocity %s"), 
                        HitActor ? *HitActor->GetName() : TEXT("NULL"), 
                        *Knockback.PullVelocity.ToString());
                }

                // Visual feedback
                DrawDebugLine(GetWorld(), GetActorLocation(), HitActor->GetActorLocation(), FColor::Orange, false, 0.5f, 0, 3.0f);
            }
        }
    }

    // Area effect visual
    DrawDebugSphere(GetWorld(), GetActorLocation(), BasicAttackRange, 16, FColor::Orange, false, 0.5f);
}

void ADivineAnchor::AbilityQ(AActor* Target)
{
    // Gravity Pull
    if (!IsAbilityQReady() || !Target || !OwnerHealthComponent)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    float PullRange = AnchorStats ? AnchorStats->GravityPullRange : 1000.0f;
    if (Distance > PullRange)
    {
        return;
    }

    // Check if target is already being affected by physics
    bool bAlreadyAffected = false;
    for (int32 i = ActivePulls.Num() - 1; i >= 0; i--)
    {
        if (ActivePulls[i].Target == Target)
        {
            // Remove existing effect to replace with new pull
            ActivePulls.RemoveAt(i);
            break;
        }
    }
    
    // Create pull target data
    FPullTarget NewPull;
    NewPull.Target = Target;
    NewPull.StartLocation = Target->GetActorLocation();
    NewPull.PullTime = 0.0f;
    NewPull.TotalPullDuration = AnchorStats ? AnchorStats->GravityPullDuration : 0.6f;
    
    // Calculate pull velocity to reach player with arc
    FVector ToPlayer = GetActorLocation() - Target->GetActorLocation();
    ToPlayer.Z = 0; // Horizontal distance only
    float HorizontalDistance = ToPlayer.Size();
    ToPlayer.Normalize();
    
    // Calculate velocity needed to reach player - using stats
    float ForceMultiplier = AnchorStats ? AnchorStats->GravityPullForceMultiplier : 2.5f;
    float UpwardVelocity = AnchorStats ? AnchorStats->GravityPullUpwardVelocity : 600.0f;
    
    float PullSpeed = (HorizontalDistance / NewPull.TotalPullDuration) * ForceMultiplier;
    NewPull.PullVelocity = ToPlayer * PullSpeed;
    NewPull.PullVelocity.Z = UpwardVelocity;
    
    // Add to active pulls
    ActivePulls.Add(NewPull);
    
    UE_LOG(LogTemp, Warning, TEXT("Gravity Pull started on %s with velocity %s"), 
        Target ? *Target->GetName() : TEXT("NULL"), 
        *NewPull.PullVelocity.ToString());

    // Make enemy vulnerable while in air
    if (UStateComponent* StateComp = Target->FindComponentByClass<UStateComponent>())
    {
        float VulnerableDuration = AnchorStats ? AnchorStats->GravityPullVulnerableDuration : 1.5f;
        StateComp->SetVulnerable(VulnerableDuration);
    }

    // Deal initial damage
    if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
    {
        FDamageInfo DamageInfo;
        DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
        DamageInfo.Type = EDamageType::Physical;
        DamageInfo.Instigator = OwnerPawn;

        FVector DamageDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        TargetHealth->TakeDamage(DamageInfo, DamageDirection);
    }

    StartCooldown(AbilityQCooldownTimer, AbilityQCooldown);

    // Visual feedback
    DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Blue, false, 1.0f, 0, 5.0f);
    
    // Draw arc trajectory preview
    FVector MidPoint = (GetActorLocation() + Target->GetActorLocation()) * 0.5f;
    MidPoint.Z += 200.0f;
    DrawDebugLine(GetWorld(), Target->GetActorLocation(), MidPoint, FColor::Cyan, false, 1.0f, 0, 3.0f);
    DrawDebugLine(GetWorld(), MidPoint, GetActorLocation(), FColor::Cyan, false, 1.0f, 0, 3.0f);
}

void ADivineAnchor::AbilityE(AActor* Target)
{
    // Holy Gravity - Damage all enemies in range and make them vulnerable
    if (!IsAbilityEReady() || !OwnerHealthComponent)
    {
        return;
    }

    // Find all enemies in range
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(this);

    GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(AnchorStats ? AnchorStats->HolyGravityRange : 1000.0f),
        QueryParams
    );

    for (const FOverlapResult& Result : OverlapResults)
    {
        if (AActor* HitActor = Result.GetActor())
        {
            // Deal damage
            if (UHealthComponent* TargetHealth = HitActor->FindComponentByClass<UHealthComponent>())
            {
                FDamageInfo DamageInfo;
                DamageInfo.Amount = OwnerHealthComponent->GetResources().AttackPoint;
                DamageInfo.Type = EDamageType::Physical;
                DamageInfo.Instigator = OwnerPawn;
                DamageInfo.bIsAreaDamage = true;

                FVector DamageDirection = (HitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                TargetHealth->TakeDamage(DamageInfo, DamageDirection);
            }

            // Make vulnerable
            if (UStateComponent* StateComp = HitActor->FindComponentByClass<UStateComponent>())
            {
                float VulnerableDuration = AnchorStats ? AnchorStats->HolyGravityVulnerableDuration : 2.0f;
                StateComp->SetVulnerable(VulnerableDuration);
            }
        }
    }

    StartCooldown(AbilityECooldownTimer, AbilityECooldown);

    // Visual feedback
    float HolyRange = AnchorStats ? AnchorStats->HolyGravityRange : 1000.0f;
    DrawDebugSphere(GetWorld(), GetActorLocation(), HolyRange, 32, FColor::Yellow, false, 1.0f);
}

void ADivineAnchor::DefensiveAbility()
{
    // Order
    if (bOrderWindowActive)
    {
        if (ProcessOrder())
        {
            // Order was successful
        }
    }
}

void ADivineAnchor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Cleanup stuck enemies every second
    static float CleanupTimer = 0.0f;
    CleanupTimer += DeltaTime;
    
    if (CleanupTimer >= 1.0f)
    {
        CleanupTimer = 0.0f;
        
        // Check all enemies for stuck state
        TArray<AActor*> AllEnemies;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), AllEnemies);
        
        for (AActor* Enemy : AllEnemies)
        {
            if (!Enemy) continue;
            
            // Check if enemy is floating (not in active pulls)
            bool bIsBeingPulled = false;
            for (const FPullTarget& Pull : ActivePulls)
            {
                if (Pull.Target == Enemy)
                {
                    bIsBeingPulled = true;
                    break;
                }
            }
            
            if (!bIsBeingPulled)
            {
                // Ground check for stuck enemies
                FHitResult GroundHit;
                FVector EnemyLoc = Enemy->GetActorLocation();
                FVector TraceStart = EnemyLoc + FVector(0, 0, 50.0f);
                FVector TraceEnd = EnemyLoc - FVector(0, 0, 2000.0f);
                
                FCollisionQueryParams QueryParams;
                QueryParams.AddIgnoredActor(Enemy);
                
                if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
                {
                    float GroundZ = GroundHit.Location.Z + 88.0f;
                    
                    // If enemy is floating WAY above ground, snap them down
                    if (EnemyLoc.Z > GroundZ + 300.0f) // Increased tolerance from 100 to 300
                    {
                        Enemy->SetActorLocation(FVector(EnemyLoc.X, EnemyLoc.Y, GroundZ), true);
                        UE_LOG(LogTemp, Warning, TEXT("Fixed stuck enemy: %s at height %.1f above ground"), 
                            *Enemy->GetName(), EnemyLoc.Z - GroundZ);
                    }
                }
            }
        }
    }

    // Update order window
    if (bOrderWindowActive)
    {
        OrderWindowTimer += DeltaTime;
        
        if (OrderWindowTimer >= 1.5f)
        {
            // Attack completed, deal full damage
            bOrderWindowActive = false;
            OrderWindowTimer = 0.0f;
            
            if (OwnerHealthComponent && IncomingDamage > 0.0f)
            {
                // Deal the damage since order wasn't used
                FDamageInfo DamageInfo;
                DamageInfo.Amount = IncomingDamage;
                DamageInfo.Type = EDamageType::Physical;
                DamageInfo.Instigator = OrderAttacker;
                
                FVector DamageDirection = OrderAttacker ? (GetActorLocation() - OrderAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
                OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
            }
        }
    }
    
    // Update active pulls
    for (int32 i = ActivePulls.Num() - 1; i >= 0; i--)
    {
        FPullTarget& Pull = ActivePulls[i];
        
        // Remove invalid or expired pulls
        if (!Pull.Target || Pull.PullTime >= Pull.TotalPullDuration || Pull.PullTime > 3.0f) // 3 second max timeout
        {
            ActivePulls.RemoveAt(i);
            continue;
        }
        
        Pull.PullTime += DeltaTime;
        
        // Calculate new position with gravity
        FVector CurrentLocation = Pull.Target->GetActorLocation();
        FVector NewLocation = CurrentLocation;
        
        // Apply velocity
        NewLocation += Pull.PullVelocity * DeltaTime;
        
        // Apply gravity to create arc
        Pull.PullVelocity.Z -= 980.0f * DeltaTime; // Gravity
        
        // Ground check - trace down to find ground
        FHitResult GroundHit;
        FVector TraceStart = CurrentLocation; // Use current location for more accurate ground check
        TraceStart.Z += 50.0f; // Start trace slightly above
        FVector TraceEnd = TraceStart - FVector(0, 0, 2000.0f);
        
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Pull.Target);
        QueryParams.AddIgnoredActor(GetOwner());
        
        bool bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);
        
        if (bHitGround)
        {
            float GroundZ = GroundHit.Location.Z + 88.0f; // Add capsule half height
            
            // If we're at or below ground, handle landing
            if (NewLocation.Z <= GroundZ + 5.0f) // 5 unit tolerance
            {
                NewLocation.Z = GroundZ;
                
                // Only stop if we're moving down
                if (Pull.PullVelocity.Z < 0)
                {
                    Pull.PullVelocity.Z = 0.0f; // Stop vertical velocity
                }
                
                // Apply friction to horizontal movement when on ground
                Pull.PullVelocity.X *= 0.85f;
                Pull.PullVelocity.Y *= 0.85f;
                
                // If velocity is very small, remove this pull
                if (Pull.PullVelocity.Size() < 30.0f)
                {
                    // Just remove from active pulls, don't force position
                    ActivePulls.RemoveAt(i);
                    continue;
                }
            }
        }
        else
        {
            // No ground found, apply stronger gravity to bring enemy down
            Pull.PullVelocity.Z -= 1500.0f * DeltaTime;
        }
        
        // Validate new location isn't too high
        if (NewLocation.Z > Pull.StartLocation.Z + 1000.0f)
        {
            // Cap maximum height to prevent infinite ascent
            NewLocation.Z = Pull.StartLocation.Z + 1000.0f;
            Pull.PullVelocity.Z = FMath::Min(Pull.PullVelocity.Z, 0.0f); // Only allow downward velocity
        }
        
        // Move the enemy - use false for sweep to ensure movement happens
        Pull.Target->SetActorLocation(NewLocation, false);
        
        // Visual feedback for active physics
        DrawDebugLine(GetWorld(), CurrentLocation, NewLocation, FColor::Magenta, false, 0.1f, 0, 2.0f);
    }
}

void ADivineAnchor::StartOrderWindow(float Damage, AActor* Attacker)
{
    bOrderWindowActive = true;
    OrderWindowTimer = 0.0f;
    IncomingDamage = Damage;
    OrderAttacker = Attacker;
}

bool ADivineAnchor::ProcessOrder()
{
    if (!bOrderWindowActive)
    {
        return false;
    }

    bool bPerfectOrder = false;
    
    // Check timing window
    if (OrderWindowTimer <= 0.75f)
    {
        // Moderate order - half damage
        if (OwnerHealthComponent)
        {
            FDamageInfo DamageInfo;
            DamageInfo.Amount = IncomingDamage * 0.5f;
            DamageInfo.Type = EDamageType::Physical;
            DamageInfo.Instigator = OrderAttacker;
            
            FVector DamageDirection = OrderAttacker ? (GetActorLocation() - OrderAttacker->GetActorLocation()).GetSafeNormal() : FVector::ForwardVector;
            OwnerHealthComponent->TakeDamage(DamageInfo, DamageDirection);
        }
    }
    else if (OrderWindowTimer <= 1.5f)
    {
        // Perfect order - no damage and counter attack
        bPerfectOrder = true;
        if (OrderAttacker)
        {
            PerformCounterAttack(OrderAttacker);
        }
    }
    else
    {
        // Too late
        return false;
    }

    bOrderWindowActive = false;
    OrderWindowTimer = 0.0f;
    
    return true;
}

void ADivineAnchor::PerformCounterAttack(AActor* Target)
{
    if (!Target)
    {
        return;
    }

    if (UTagComponent* TagComp = Target->FindComponentByClass<UTagComponent>())
    {
        if (TagComp->HasTag(ECharacterTag::Shielded))
        {
            // Strip shield
            TagComp->RemoveTag(ECharacterTag::Shielded);
            
            // Visual feedback
            DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 100.0f, 12, FColor::Red, false, 1.0f);
        }
        else if (TagComp->HasTag(ECharacterTag::Armored))
        {
            // Reduce armor by 25%
            if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>())
            {
                FCharacterResources Resources = HealthComp->GetResources();
                Resources.DefencePoint *= 0.75f;
                HealthComp->SetResources(Resources);
                
                // Visual feedback
                DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 80.0f, 12, FColor::Orange, false, 1.0f);
            }
        }
    }
}