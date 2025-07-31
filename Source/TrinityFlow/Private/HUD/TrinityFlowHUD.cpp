#include "HUD/TrinityFlowHUD.h"
#include "../../TrinityFlowCharacter.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Player/OverrideKatana.h"
#include "Player/PhysicalKatana.h"
#include "Enemy/EnemyBase.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "World/ShardAltar.h"
#include "Core/ShardComponent.h"
#include "Core/StanceComponent.h"

ATrinityFlowHUD::ATrinityFlowHUD()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATrinityFlowHUD::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<ATrinityFlowCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    
    // Subscribe to damage events from all actors
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (UHealthComponent* HealthComp = Actor->FindComponentByClass<UHealthComponent>())
        {
            HealthComp->OnDamageDealt.AddDynamic(this, &ATrinityFlowHUD::OnDamageDealt);
        }
    }
}

void ATrinityFlowHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update floating damage numbers
    for (int32 i = FloatingDamageNumbers.Num() - 1; i >= 0; i--)
    {
        FloatingDamageNumbers[i].LifeTime -= DeltaTime;
        
        // Move damage numbers up
        FloatingDamageNumbers[i].WorldLocation.Z += 50.0f * DeltaTime;
        
        // Remove expired damage numbers
        if (FloatingDamageNumbers[i].LifeTime <= 0.0f)
        {
            FloatingDamageNumbers.RemoveAt(i);
        }
    }
    
    // Handle shard activation UI input
    if (ShardActivationUIState != EShardActivationUIState::Closed)
    {
        if (APlayerController* PC = GetOwningPlayerController())
        {
            // Check for Q key to close
            if (PC->WasInputKeyJustPressed(EKeys::Q))
            {
                CloseShardActivationUI();
                return;
            }
            
            // Check for A/D to switch shard types
            if (PC->WasInputKeyJustPressed(EKeys::A))
            {
                bSelectingSoulShards = true;
                CurrentInputString = &SoulShardInput;
            }
            else if (PC->WasInputKeyJustPressed(EKeys::D))
            {
                bSelectingSoulShards = false;
                CurrentInputString = &PowerShardInput;
            }
            
            // Check for W/S to increment/decrement shard amount
            if (CurrentInputString)
            {
                int32 CurrentValue = FCString::Atoi(**CurrentInputString);
                int32 MaxValue = bSelectingSoulShards ? CachedSoulInactive : CachedPowerInactive;
                
                if (PC->WasInputKeyJustPressed(EKeys::W))
                {
                    // Increment
                    CurrentValue = FMath::Clamp(CurrentValue + 1, 0, MaxValue);
                    *CurrentInputString = FString::FromInt(CurrentValue);
                }
                else if (PC->WasInputKeyJustPressed(EKeys::S))
                {
                    // Decrement
                    CurrentValue = FMath::Clamp(CurrentValue - 1, 0, MaxValue);
                    *CurrentInputString = FString::FromInt(CurrentValue);
                }
            }
            
            // Check for E key to activate
            if (PC->WasInputKeyJustPressed(EKeys::E))
            {
                ProcessShardActivation();
            }
        }
    }
}

void ATrinityFlowHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !PlayerCharacter)
    {
        return;
    }

    ViewportSize = FVector2D(Canvas->SizeX, Canvas->SizeY);

    // Draw all HUD elements
    DrawHealthBar();
    DrawCrosshair();
    DrawFloatingDamageNumbers();
    DrawEnemyInfoAboveHeads();
    DrawPlayerInfo();
    DrawCombatState();
    DrawWeaponInfo();
    DrawNearbyEnemies();
    
    // Draw appropriate UI based on state
    if (ShardActivationUIState != EShardActivationUIState::Closed)
    {
        DrawShardActivationPanel();
    }
    else
    {
        DrawAltarInteractionUI();
    }
}

void ATrinityFlowHUD::DrawHealthBar()
{
    if (UHealthComponent* HealthComp = PlayerCharacter->GetHealthComponent())
    {
        float HealthPercentage = HealthComp->GetHealthPercentage();
        
        // Position above player info at bottom left
        float X = 20.0f;
        float Y = ViewportSize.Y - 350.0f;  // Moved even higher to avoid collision

        // Background
        DrawRect(FLinearColor::Black, X - 2, Y - 2, HealthBarWidth + 4, HealthBarHeight + 4);
        
        // Health bar
        DrawBar(X, Y, HealthBarWidth, HealthBarHeight, HealthPercentage, FLinearColor::Red);
        
        // Health text
        FString HealthText = FString::Printf(TEXT("Health: %.0f/%.0f"), 
            HealthComp->GetHealth(), HealthComp->GetMaxHealth());
        DrawText(HealthText, FLinearColor::White, X, Y - 25, nullptr, 1.0f);
    }
}

void ATrinityFlowHUD::DrawCrosshair()
{
    float CenterX = ViewportSize.X * 0.5f;
    float CenterY = ViewportSize.Y * 0.5f;

    // Draw crosshair dot
    DrawRect(FLinearColor::White, 
        CenterX - CrosshairSize * 0.5f, 
        CenterY - CrosshairSize * 0.5f, 
        CrosshairSize, 
        CrosshairSize);
}

void ATrinityFlowHUD::DrawEnemyInfoAboveHeads()
{
    // Find all enemies in the world
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (AEnemyBase* Enemy = Cast<AEnemyBase>(Actor))
        {
            // Get enemy location and convert to screen space
            FVector EnemyLocation = Enemy->GetActorLocation();
            FVector HeadLocation = EnemyLocation + FVector(0, 0, 200.0f); // Offset above enemy
            
            FVector2D ScreenLocation;
            APlayerController* PC = GetOwningPlayerController();
            if (PC && UGameplayStatics::ProjectWorldToScreen(PC, HeadLocation, ScreenLocation))
            {
                // Only draw if on screen
                if (ScreenLocation.X >= 0 && ScreenLocation.X <= ViewportSize.X &&
                    ScreenLocation.Y >= 0 && ScreenLocation.Y <= ViewportSize.Y)
                {
                    DrawEnemyInfo(Enemy, ScreenLocation.X, ScreenLocation.Y);
                }
            }
        }
    }
}

void ATrinityFlowHUD::DrawEnemyInfo(AEnemyBase* Enemy, float ScreenX, float ScreenY)
{
    if (!Enemy) return;

    // Calculate distance to scale text appropriately
    float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), Enemy->GetActorLocation());
    
    // Keep font size constant regardless of distance
    // Base scale of 1.0 at any distance
    float FontScale = 1.0f;
    
    float X = ScreenX - 100.0f; // Center the info
    float Y = ScreenY;
    
    // Enemy name/type with shadow
    FString EnemyType = Enemy->GetClass()->GetName();
    EnemyType = EnemyType.Replace(TEXT("BP_"), TEXT(""));
    EnemyType = EnemyType.Replace(TEXT("_C"), TEXT(""));
    DrawText(EnemyType, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale);
    DrawText(EnemyType, FLinearColor::Yellow, X, Y, nullptr, FontScale);
    Y += 18;

    // Health info
    if (UHealthComponent* HealthComp = Enemy->FindComponentByClass<UHealthComponent>())
    {
        // Health bar with outline for visibility
        float HealthPercentage = HealthComp->GetHealthPercentage();
        float BarWidth = 150.0f;
        float BarHeight = 10.0f;
        
        // Draw black outline
        DrawRect(FLinearColor::Black, X - 1, Y - 1, BarWidth + 2, BarHeight + 2);
        // Draw health bar
        DrawBar(X, Y, BarWidth, BarHeight, HealthPercentage, FLinearColor::Red);
        Y += 14;
        
        // Health text with shadow for readability
        FString HealthText = FString::Printf(TEXT("HP: %.0f/%.0f"), 
            HealthComp->GetHealth(), HealthComp->GetMaxHealth());
        // Draw shadow
        DrawText(HealthText, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale * 0.9f);
        // Draw text
        DrawText(HealthText, FLinearColor::White, X, Y, nullptr, FontScale * 0.9f);
        Y += 16;

        // Attack and Defense with shadow
        const FCharacterResources& Resources = HealthComp->GetResources();
        FString StatsText = FString::Printf(TEXT("ATK: %.0f  DEF: %.0f"), 
            Resources.AttackPoint, Resources.DefencePoint);
        DrawText(StatsText, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale * 0.9f);
        DrawText(StatsText, FLinearColor(1.0f, 0.8f, 0.0f), X, Y, nullptr, FontScale * 0.9f);
        Y += 16;
    }

    // Tags with shadow
    if (UTagComponent* TagComp = Enemy->FindComponentByClass<UTagComponent>())
    {
        FString TagsText = GetTagsString(TagComp->GetTags());
        DrawText(TagsText, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale * 0.9f);
        DrawText(TagsText, FLinearColor::Green, X, Y, nullptr, FontScale * 0.9f);
        Y += 16;
    }

    // States with shadow
    if (UStateComponent* StateComp = Enemy->FindComponentByClass<UStateComponent>())
    {
        FString StatesText = GetStatesString(StateComp->GetStates());
        DrawText(StatesText, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale * 0.9f);
        DrawText(StatesText, FLinearColor(0.0f, 1.0f, 1.0f), X, Y, nullptr, FontScale * 0.9f);
        Y += 16;

        // Mark duration if marked
        if (StateComp->IsMarked())
        {
            FString MarkedText = FString::Printf(TEXT("MARKED: %.1fs"), StateComp->GetMarkedTimeRemaining());
            DrawText(MarkedText, FLinearColor::Black, X + 1, Y + 1, nullptr, FontScale);
            DrawText(MarkedText, FLinearColor(1.0f, 0.0f, 1.0f), X, Y, nullptr, FontScale);
        }
    }
}

void ATrinityFlowHUD::DrawPlayerInfo()
{
    // Position at bottom left corner, moved higher up
    float X = 20.0f;
    float Y = ViewportSize.Y - 320.0f;  // Moved much higher to show all content

    // Title with shadow
    FString Title = TEXT("=== PLAYER INFO ===");
    DrawText(Title, FLinearColor::Black, X + 1, Y + 1);
    DrawText(Title, FLinearColor::Yellow, X, Y);
    Y += LineHeight;

    if (UHealthComponent* HealthComp = PlayerCharacter->GetHealthComponent())
    {
        const FCharacterResources& Resources = HealthComp->GetResources();
        
        // Health with shadow
        FString HealthText = FString::Printf(TEXT("Health: %.0f/%.0f"), Resources.Health, Resources.MaxHealth);
        DrawText(HealthText, FLinearColor::Black, X + 1, Y + 1);
        DrawText(HealthText, FLinearColor::Red, X, Y);
        Y += LineHeight;
        
        // Attack with shadow
        FString AttackText = FString::Printf(TEXT("Attack: %.0f"), Resources.AttackPoint);
        DrawText(AttackText, FLinearColor::Black, X + 1, Y + 1);
        DrawText(AttackText, FLinearColor(1.0f, 0.5f, 0.0f), X, Y);
        Y += LineHeight;
        
        // Defence with shadow
        FString DefenceText = FString::Printf(TEXT("Defence: %.0f"), Resources.DefencePoint);
        DrawText(DefenceText, FLinearColor::Black, X + 1, Y + 1);
        DrawText(DefenceText, FLinearColor::Blue, X, Y);
        Y += LineHeight;
    }

    if (UTagComponent* TagComp = PlayerCharacter->GetTagComponent())
    {
        FString TagsText = TEXT("Tags: ") + GetTagsString(TagComp->GetTags());
        DrawText(TagsText, FLinearColor::Black, X + 1, Y + 1);
        DrawText(TagsText, FLinearColor::Green, X, Y);
        Y += LineHeight;
    }
    
    // Draw Stance Information
    if (UStanceComponent* StanceComp = PlayerCharacter->GetStanceComponent())
    {
        EStanceType CurrentStance = StanceComp->GetCurrentStance();
        FString StanceName = TEXT("Unknown Stance");
        FLinearColor StanceColor = FLinearColor::White;
        FString BonusText = TEXT("No Bonus");
        
        switch (CurrentStance)
        {
            case EStanceType::Power:
                StanceName = TEXT("Power Stance");
                StanceColor = FLinearColor::Red;
                BonusText = TEXT("+15% Physical Damage");
                break;
            case EStanceType::Soul:
                StanceName = TEXT("Soul Stance");
                StanceColor = FLinearColor(0.2f, 0.6f, 1.0f); // Light Blue
                BonusText = TEXT("+15% Soul Damage");
                break;
            case EStanceType::Balanced:
                StanceName = TEXT("Balanced Stance");
                StanceColor = FLinearColor(0.8f, 0.2f, 1.0f); // Purple
                BonusText = TEXT("+10% All Damage");
                break;
            default:
                // Default values already set above
                break;
        }
        
        // Stance name
        FString StanceText = TEXT("Stance: ") + StanceName;
        DrawText(StanceText, FLinearColor::Black, X + 1, Y + 1);
        DrawText(StanceText, StanceColor, X, Y);
        Y += LineHeight;
        
        // Stance bonus
        DrawText(BonusText, FLinearColor::Black, X + 11, Y + 1, nullptr, 0.9f);
        DrawText(BonusText, FLinearColor::Yellow, X + 10, Y, nullptr, 0.9f);
        Y += LineHeight;
    }
    
    // Draw Shard Information
    if (UShardComponent* ShardComp = PlayerCharacter->GetShardComponent())
    {
        // Separator
        Y += 5;
        DrawText(TEXT("--- SHARDS ---"), FLinearColor::Black, X + 1, Y + 1, nullptr, 0.9f);
        DrawText(TEXT("--- SHARDS ---"), FLinearColor::White, X, Y, nullptr, 0.9f);
        Y += LineHeight;
        
        // Get shard counts
        int32 SoulActive, SoulInactive, PowerActive, PowerInactive;
        ShardComp->GetShardCounts(SoulActive, SoulInactive, PowerActive, PowerInactive);
        
        // Soul shards
        FString SoulText = FString::Printf(TEXT("Soul: %d active / %d inactive"), SoulActive, SoulInactive);
        DrawText(SoulText, FLinearColor::Black, X + 1, Y + 1, nullptr, 0.9f);
        DrawText(SoulText, FLinearColor(0.2f, 0.6f, 1.0f), X, Y, nullptr, 0.9f);
        Y += LineHeight;
        
        // Power shards
        FString PowerText = FString::Printf(TEXT("Power: %d active / %d inactive"), PowerActive, PowerInactive);
        DrawText(PowerText, FLinearColor::Black, X + 1, Y + 1, nullptr, 0.9f);
        DrawText(PowerText, FLinearColor::Red, X, Y, nullptr, 0.9f);
        Y += LineHeight;
        
        // Total
        int32 TotalActive = SoulActive + PowerActive;
        int32 TotalInactive = SoulInactive + PowerInactive;
        FString TotalText = FString::Printf(TEXT("Total: %d active / %d inactive"), TotalActive, TotalInactive);
        DrawText(TotalText, FLinearColor::Black, X + 1, Y + 1, nullptr, 0.9f);
        DrawText(TotalText, FLinearColor::Green, X, Y, nullptr, 0.9f);
    }
}

void ATrinityFlowHUD::DrawCombatState()
{
    if (UStateComponent* StateComp = PlayerCharacter->GetStateComponent())
    {
        FString StateText = StateComp->HasState(ECharacterState::Combat) ? TEXT("IN COMBAT") : TEXT("NON-COMBAT");
        FLinearColor StateColor = StateComp->HasState(ECharacterState::Combat) ? FLinearColor::Red : FLinearColor::Green;
        
        DrawText(StateText, StateColor, ViewportSize.X * 0.5f - 50.0f, 50.0f, nullptr, 1.5f);
    }
}

void ATrinityFlowHUD::DrawWeaponInfo()
{
    float X = ViewportSize.X * 0.5f - 150.0f;
    float Y = ViewportSize.Y - 150.0f;

    DrawText(TEXT("Dual Katanas"), FLinearColor::White, X, Y);
    Y += LineHeight;

    // Left Katana (Soul damage) cooldowns
    if (AOverrideKatana* LeftKatana = PlayerCharacter->GetLeftKatana())
    {
        float QCooldown = LeftKatana->GetAbilityQCooldownRemaining();
        float TabCooldown = LeftKatana->GetAbilityECooldownRemaining(); // Using E cooldown for Tab
        
        FString QText = QCooldown > 0 ? FString::Printf(TEXT("Q: %.1fs"), QCooldown) : TEXT("Q: Ready");
        FString TabText = TabCooldown > 0 ? FString::Printf(TEXT("Tab: %.1fs"), TabCooldown) : TEXT("Tab: Ready");
        
        FLinearColor QColor = QCooldown > 0 ? FLinearColor::Red : FLinearColor::Green;
        FLinearColor TabColor = TabCooldown > 0 ? FLinearColor::Red : FLinearColor::Green;
        
        DrawText(TEXT("Left (Soul):"), FLinearColor(0.0f, 1.0f, 1.0f), X, Y);  // Cyan color
        DrawText(QText, QColor, X + 80, Y);
        DrawText(TabText, TabColor, X + 180, Y);
    }

    Y += LineHeight;

    // Right Katana (Physical damage) cooldowns
    if (APhysicalKatana* RightKatana = PlayerCharacter->GetRightKatana())
    {
        float ECooldown = RightKatana->GetAbilityECooldownRemaining();
        
        FString EText = ECooldown > 0 ? FString::Printf(TEXT("E: %.1fs"), ECooldown) : TEXT("E: Ready");
        FString RText = TEXT("R: Ready");
        
        FLinearColor EColor = ECooldown > 0 ? FLinearColor::Red : FLinearColor::Green;
        
        DrawText(TEXT("Right (Phys):"), FLinearColor(1.0f, 0.5f, 0.0f), X, Y);  // Orange color
        DrawText(EText, EColor, X + 80, Y);
        DrawText(RText, FLinearColor::Gray, X + 180, Y);
    }
}

void ATrinityFlowHUD::DrawNearbyEnemies()
{
    float X = 20.0f;
    float Y = 100.0f;  // Position at top left

    DrawText(TEXT("=== NEARBY ENEMIES ==="), FLinearColor::Yellow, X, Y);
    Y += LineHeight;

    // Find all enemies within range
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), FoundActors);

    int32 EnemyCount = 0;
    for (AActor* Actor : FoundActors)
    {
        if (AEnemyBase* Enemy = Cast<AEnemyBase>(Actor))
        {
            float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), Enemy->GetActorLocation());
            if (Distance < 3000.0f)
            {
                FString EnemyInfo = FString::Printf(TEXT("%s - %.0fm"), 
                    *Enemy->GetClass()->GetName(), Distance / 100.0f);
                
                if (UStateComponent* StateComp = Enemy->FindComponentByClass<UStateComponent>())
                {
                    if (StateComp->IsMarked())
                    {
                        EnemyInfo += TEXT(" [MARKED]");
                    }
                }

                DrawText(EnemyInfo, FLinearColor::White, X, Y);
                Y += LineHeight;
                EnemyCount++;

                if (EnemyCount >= 5)
                {
                    break;
                }
            }
        }
    }
}

void ATrinityFlowHUD::DrawBar(float X, float Y, float Width, float Height, float Percentage, FLinearColor Color)
{
    // Background
    DrawRect(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f), X, Y, Width, Height);
    
    // Filled portion
    DrawRect(Color, X, Y, Width * Percentage, Height);
}

FString ATrinityFlowHUD::GetTagsString(ECharacterTag InTags)
{
    TArray<FString> TagStrings;
    
    if (EnumHasAnyFlags(InTags, ECharacterTag::Shielded)) TagStrings.Add(TEXT("shielded"));
    if (EnumHasAnyFlags(InTags, ECharacterTag::Armored)) TagStrings.Add(TEXT("armored"));
    if (EnumHasAnyFlags(InTags, ECharacterTag::Ghost)) TagStrings.Add(TEXT("ghost"));
    if (EnumHasAnyFlags(InTags, ECharacterTag::Mechanical)) TagStrings.Add(TEXT("mechanical"));
    if (EnumHasAnyFlags(InTags, ECharacterTag::HaveSoul)) TagStrings.Add(TEXT("havesoul"));
    
    return FString::Join(TagStrings, TEXT(", "));
}

FString ATrinityFlowHUD::GetStatesString(ECharacterState InStates)
{
    TArray<FString> StateStrings;
    
    if (EnumHasAnyFlags(InStates, ECharacterState::Vulnerable)) StateStrings.Add(TEXT("vulnerable"));
    if (EnumHasAnyFlags(InStates, ECharacterState::Combat)) StateStrings.Add(TEXT("combat"));
    if (EnumHasAnyFlags(InStates, ECharacterState::NonCombat)) StateStrings.Add(TEXT("non-combat"));
    if (EnumHasAnyFlags(InStates, ECharacterState::Marked)) StateStrings.Add(TEXT("marked"));
    
    return FString::Join(StateStrings, TEXT(", "));
}

void ATrinityFlowHUD::OnDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType)
{
    // Only show damage numbers for damage dealt by the player
    if (DamageInstigator == PlayerCharacter && DamagedActor)
    {
        // Check if this is echo damage by seeing if the damaged actor is marked
        bool bIsEcho = false;
        if (UStateComponent* StateComp = DamagedActor->FindComponentByClass<UStateComponent>())
        {
            // If the damaged actor is marked and it's soul damage, it's likely echo damage
            bIsEcho = StateComp->IsMarked() && DamageType == EDamageType::Soul;
        }
        
        AddFloatingDamageNumber(DamagedActor->GetActorLocation(), ActualDamage, bIsEcho);
    }
}

void ATrinityFlowHUD::AddFloatingDamageNumber(const FVector& Location, float Damage, bool bIsEcho)
{
    FFloatingDamageNumber NewNumber;
    NewNumber.WorldLocation = Location + FVector(FMath::RandRange(-30.0f, 30.0f), FMath::RandRange(-30.0f, 30.0f), 100.0f);
    NewNumber.Damage = Damage;
    NewNumber.LifeTime = 2.0f;
    NewNumber.bIsEcho = bIsEcho;
    
    // Set color based on damage type
    if (bIsEcho)
    {
        NewNumber.Color = FLinearColor(0.0f, 0.5f, 1.0f); // Blue for echo damage
    }
    else
    {
        NewNumber.Color = FLinearColor(1.0f, 0.0f, 0.0f); // Red for normal damage
    }
    
    FloatingDamageNumbers.Add(NewNumber);
}

void ATrinityFlowHUD::DrawFloatingDamageNumbers()
{
    for (const FFloatingDamageNumber& DamageNumber : FloatingDamageNumbers)
    {
        FVector2D ScreenLocation;
        APlayerController* PC = GetOwningPlayerController();
        
        if (PC && UGameplayStatics::ProjectWorldToScreen(PC, DamageNumber.WorldLocation, ScreenLocation))
        {
            // Only draw if on screen
            if (ScreenLocation.X >= 0 && ScreenLocation.X <= ViewportSize.X &&
                ScreenLocation.Y >= 0 && ScreenLocation.Y <= ViewportSize.Y)
            {
                // Calculate fade based on lifetime
                float Alpha = FMath::Clamp(DamageNumber.LifeTime / 0.5f, 0.0f, 1.0f);
                FLinearColor DisplayColor = DamageNumber.Color;
                DisplayColor.A = Alpha;
                
                // Calculate size based on damage amount - increased base size
                float FontScale = FMath::Clamp(1.5f + (DamageNumber.Damage / 50.0f), 1.5f, 3.5f);
                
                // Format damage text
                FString DamageText = FString::Printf(TEXT("%.0f"), DamageNumber.Damage);
                if (DamageNumber.bIsEcho)
                {
                    DamageText = TEXT("ECHO ") + DamageText;
                }
                
                // Draw shadow
                DrawText(DamageText, FLinearColor(0, 0, 0, Alpha), ScreenLocation.X + 2, ScreenLocation.Y + 2, nullptr, FontScale);
                // Draw damage number
                DrawText(DamageText, DisplayColor, ScreenLocation.X, ScreenLocation.Y, nullptr, FontScale);
            }
        }
    }
}

void ATrinityFlowHUD::DrawAltarInteractionUI()
{
    if (!PlayerCharacter)
    {
        return;
    }
    
    // Check if player is near an altar
    NearbyAltar = GetNearbyAltar();
    
    if (!NearbyAltar)
    {
        return;
    }
    
    // Check if in non-combat state
    if (UStateComponent* StateComp = PlayerCharacter->GetStateComponent())
    {
        if (StateComp->HasState(ECharacterState::Combat))
        {
            return; // Don't show altar UI in combat
        }
    }
    
    // Get shard component
    UShardComponent* ShardComp = PlayerCharacter->GetShardComponent();
    if (!ShardComp)
    {
        return;
    }
    
    // Get shard counts
    int32 SoulInactive = ShardComp->GetInactiveShardCount(EShardType::Soul);
    int32 PowerInactive = ShardComp->GetInactiveShardCount(EShardType::Power);
    
    // Position UI in center of screen
    float BaseX = ViewportSize.X * 0.5f - 150.0f;
    float BaseY = ViewportSize.Y * 0.4f;
    
    // Transparent background box
    DrawRect(FLinearColor(0, 0, 0, 0.3f), BaseX - 10, BaseY - 10, 320, 180);
    
    // Title
    FString Title = TEXT("=== SHARD ALTAR ===");
    DrawText(Title, FLinearColor::Yellow, BaseX, BaseY, nullptr, 1.2f);
    BaseY += 30;
    
    // Can activate check
    bool bCanActivate = NearbyAltar->CanActivate(PlayerCharacter);
    
    if (bCanActivate)
    {
        // Instruction
        FString Instruction = TEXT("Press E to activate altar");
        DrawText(Instruction, FLinearColor::White, BaseX, BaseY);
        BaseY += 25;
        
        // Shard counts
        FString SoulText = FString::Printf(TEXT("Soul Shards: %d (inactive)"), SoulInactive);
        DrawText(SoulText, FLinearColor(0.2f, 0.6f, 1.0f), BaseX, BaseY);
        BaseY += 20;
        
        FString PowerText = FString::Printf(TEXT("Power Shards: %d (inactive)"), PowerInactive);
        DrawText(PowerText, FLinearColor::Red, BaseX, BaseY);
        BaseY += 30;
        
        // Check if currently activating
        if (NearbyAltar->IsActivating())
        {
            // Show progress bar
            float Progress = NearbyAltar->GetActivationProgress();
            
            FString ProgressText = TEXT("Activating shards...");
            DrawText(ProgressText, FLinearColor::Green, BaseX, BaseY);
            BaseY += 25;
            
            // Progress bar background
            float BarWidth = 280.0f;
            float BarHeight = 20.0f;
            DrawRect(FLinearColor::Black, BaseX - 2, BaseY - 2, BarWidth + 4, BarHeight + 4);
            
            // Progress bar fill
            DrawBar(BaseX, BaseY, BarWidth, BarHeight, Progress, FLinearColor::Green);
            BaseY += 25;
            
            // Progress percentage
            FString PercentText = FString::Printf(TEXT("%.0f%%"), Progress * 100.0f);
            DrawText(PercentText, FLinearColor::White, BaseX + BarWidth/2 - 20, BaseY);
        }
    }
    else
    {
        // Show why can't activate
        if (SoulInactive == 0 && PowerInactive == 0)
        {
            FString NoShardsText = TEXT("No inactive shards to activate");
            DrawText(NoShardsText, FLinearColor::Red, BaseX, BaseY);
        }
        else
        {
            FString GuardianText = TEXT("Defeat guardian enemies first!");
            DrawText(GuardianText, FLinearColor::Red, BaseX, BaseY);
        }
    }
}

bool ATrinityFlowHUD::IsNearAltar() const
{
    return NearbyAltar != nullptr;
}

AShardAltar* ATrinityFlowHUD::GetNearbyAltar() const
{
    if (!PlayerCharacter)
    {
        return nullptr;
    }
    
    // Find all altars
    TArray<AActor*> FoundAltars;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShardAltar::StaticClass(), FoundAltars);
    
    // Check if player is in any altar's zone
    for (AActor* Actor : FoundAltars)
    {
        if (AShardAltar* Altar = Cast<AShardAltar>(Actor))
        {
            if (Altar->IsPlayerInZone(PlayerCharacter))
            {
                return Altar;
            }
        }
    }
    
    return nullptr;
}

void ATrinityFlowHUD::OpenShardActivationUI(AShardAltar* Altar)
{
    if (!Altar || !PlayerCharacter)
    {
        return;
    }
    
    ActiveAltar = Altar;
    ShardActivationUIState = EShardActivationUIState::SelectingShardType;
    bSelectingSoulShards = true;
    SoulShardInput = "0";
    PowerShardInput = "0";
    CurrentInputString = &SoulShardInput;
    
    // Cache current shard counts
    if (UShardComponent* ShardComp = PlayerCharacter->GetShardComponent())
    {
        ShardComp->GetShardCounts(CachedSoulActive, CachedSoulInactive, CachedPowerActive, CachedPowerInactive);
    }
    
    // Don't change input mode - we'll handle input filtering in the character class
    UE_LOG(LogTemp, Log, TEXT("Shard activation UI opened"));
}

void ATrinityFlowHUD::CloseShardActivationUI()
{
    ShardActivationUIState = EShardActivationUIState::Closed;
    ActiveAltar = nullptr;
    bSelectingSoulShards = true;
    SoulShardInput = "0";
    PowerShardInput = "0";
    CurrentInputString = nullptr;
    
    UE_LOG(LogTemp, Log, TEXT("Shard activation UI closed"));
}

void ATrinityFlowHUD::DrawShardActivationPanel()
{
    if (!Canvas || !PlayerCharacter || !ActiveAltar)
    {
        return;
    }
    
    // Draw semi-transparent background
    float ScreenWidth = ViewportSize.X;
    float ScreenHeight = ViewportSize.Y;
    DrawRect(FLinearColor(0, 0, 0, 0.7f), 0, 0, ScreenWidth, ScreenHeight);
    
    // Main panel dimensions
    float PanelWidth = 800.0f;
    float PanelHeight = 600.0f;
    float PanelX = (ScreenWidth - PanelWidth) * 0.5f;
    float PanelY = (ScreenHeight - PanelHeight) * 0.5f;
    
    // Draw main panel background
    DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f), PanelX, PanelY, PanelWidth, PanelHeight);
    
    // Draw title
    float TitleY = PanelY + 30.0f;
    FString Title = TEXT("=== SHARD ACTIVATION ALTAR ===");
    DrawText(Title, FLinearColor::Yellow, PanelX + PanelWidth * 0.5f - 150.0f, TitleY, nullptr, 1.5f);
    
    // Draw instructions
    float InstructY = TitleY + 50.0f;
    FString Instructions = TEXT("A/D: Select Shard Type | W/S: Increase/Decrease Amount | E: Activate | Q: Cancel");
    DrawText(Instructions, FLinearColor::White, PanelX + 50.0f, InstructY);
    
    // Draw shard sections
    float SectionY = InstructY + 60.0f;
    float SectionWidth = 350.0f;
    float SectionHeight = 250.0f;
    float SoulSectionX = PanelX + 50.0f;
    float PowerSectionX = PanelX + PanelWidth - SectionWidth - 50.0f;
    
    // Soul shard section
    FLinearColor SoulBorderColor = bSelectingSoulShards ? FLinearColor::Yellow : FLinearColor(0.2f, 0.2f, 0.2f);
    DrawRect(SoulBorderColor, SoulSectionX - 5, SectionY - 5, SectionWidth + 10, SectionHeight + 10);
    DrawRect(FLinearColor(0.05f, 0.1f, 0.2f, 0.8f), SoulSectionX, SectionY, SectionWidth, SectionHeight);
    
    // Soul shard content
    FString SoulTitle = TEXT("SOUL SHARDS");
    DrawText(SoulTitle, FLinearColor(0.2f, 0.6f, 1.0f), SoulSectionX + 100.0f, SectionY + 20.0f, nullptr, 1.3f);
    
    FString SoulActive = FString::Printf(TEXT("Active: %d"), CachedSoulActive);
    FString SoulInactive = FString::Printf(TEXT("Inactive: %d"), CachedSoulInactive);
    DrawText(SoulActive, FLinearColor::White, SoulSectionX + 20.0f, SectionY + 60.0f);
    DrawText(SoulInactive, FLinearColor::Gray, SoulSectionX + 20.0f, SectionY + 85.0f);
    
    FString SoulInputLabel = TEXT("Amount to Activate:");
    DrawText(SoulInputLabel, FLinearColor::White, SoulSectionX + 20.0f, SectionY + 130.0f);
    
    // Input box for soul shards
    float InputBoxY = SectionY + 160.0f;
    DrawRect(FLinearColor::Black, SoulSectionX + 20.0f, InputBoxY, 100.0f, 30.0f);
    DrawText(SoulShardInput, FLinearColor::Green, SoulSectionX + 25.0f, InputBoxY + 5.0f, nullptr, 1.2f);
    
    // W/S indicators for soul shards
    if (bSelectingSoulShards)
    {
        DrawText(TEXT("W ↑"), FLinearColor::Yellow, SoulSectionX + 130.0f, InputBoxY - 5.0f, nullptr, 0.9f);
        DrawText(TEXT("S ↓"), FLinearColor::Yellow, SoulSectionX + 130.0f, InputBoxY + 15.0f, nullptr, 0.9f);
    }
    
    // Power shard section
    FLinearColor PowerBorderColor = !bSelectingSoulShards ? FLinearColor::Yellow : FLinearColor(0.2f, 0.2f, 0.2f);
    DrawRect(PowerBorderColor, PowerSectionX - 5, SectionY - 5, SectionWidth + 10, SectionHeight + 10);
    DrawRect(FLinearColor(0.2f, 0.05f, 0.05f, 0.8f), PowerSectionX, SectionY, SectionWidth, SectionHeight);
    
    // Power shard content
    FString PowerTitle = TEXT("POWER SHARDS");
    DrawText(PowerTitle, FLinearColor::Red, PowerSectionX + 100.0f, SectionY + 20.0f, nullptr, 1.3f);
    
    FString PowerActive = FString::Printf(TEXT("Active: %d"), CachedPowerActive);
    FString PowerInactive = FString::Printf(TEXT("Inactive: %d"), CachedPowerInactive);
    DrawText(PowerActive, FLinearColor::White, PowerSectionX + 20.0f, SectionY + 60.0f);
    DrawText(PowerInactive, FLinearColor::Gray, PowerSectionX + 20.0f, SectionY + 85.0f);
    
    FString PowerInputLabel = TEXT("Amount to Activate:");
    DrawText(PowerInputLabel, FLinearColor::White, PowerSectionX + 20.0f, SectionY + 130.0f);
    
    // Input box for power shards
    DrawRect(FLinearColor::Black, PowerSectionX + 20.0f, InputBoxY, 100.0f, 30.0f);
    DrawText(PowerShardInput, FLinearColor::Green, PowerSectionX + 25.0f, InputBoxY + 5.0f, nullptr, 1.2f);
    
    // W/S indicators for power shards
    if (!bSelectingSoulShards)
    {
        DrawText(TEXT("W ↑"), FLinearColor::Yellow, PowerSectionX + 130.0f, InputBoxY - 5.0f, nullptr, 0.9f);
        DrawText(TEXT("S ↓"), FLinearColor::Yellow, PowerSectionX + 130.0f, InputBoxY + 15.0f, nullptr, 0.9f);
    }
    
    // Draw stance preview
    float PreviewY = SectionY + SectionHeight + 40.0f;
    UpdateStancePreview();
    
    // Calculate what stance will be after activation
    int32 SoulToActivate = FCString::Atoi(*SoulShardInput);
    int32 PowerToActivate = FCString::Atoi(*PowerShardInput);
    int32 NewSoulActive = CachedSoulActive + SoulToActivate;
    int32 NewPowerActive = CachedPowerActive + PowerToActivate;
    
    EStanceType PreviewStance = CalculateStanceFromShards(NewSoulActive, NewPowerActive);
    
    FString StancePreviewLabel = TEXT("Resulting Stance: ");
    FString StanceName = TEXT("Unknown");
    FLinearColor StanceColor = FLinearColor::White;
    
    switch (PreviewStance)
    {
        case EStanceType::Power:
            StanceName = TEXT("Power Stance");
            StanceColor = FLinearColor::Red;
            break;
        case EStanceType::Soul:
            StanceName = TEXT("Soul Stance");
            StanceColor = FLinearColor(0.2f, 0.6f, 1.0f);
            break;
        case EStanceType::Balanced:
            StanceName = TEXT("Balanced Stance");
            StanceColor = FLinearColor(0.8f, 0.2f, 1.0f);
            break;
    }
    
    DrawText(StancePreviewLabel + StanceName, StanceColor, PanelX + PanelWidth * 0.5f - 100.0f, PreviewY, nullptr, 1.2f);
    
    // Draw activation button prompt
    if (SoulToActivate > 0 || PowerToActivate > 0)
    {
        FString ActivatePrompt = TEXT("Press E to Activate Shards");
        DrawText(ActivatePrompt, FLinearColor::Green, PanelX + PanelWidth * 0.5f - 100.0f, PreviewY + 40.0f, nullptr, 1.1f);
    }
}

// Removed HandleShardActivationInput - no longer needed with W/S controls

void ATrinityFlowHUD::ProcessShardActivation()
{
    if (!ActiveAltar || !PlayerCharacter)
    {
        return;
    }
    
    int32 SoulToActivate = FCString::Atoi(*SoulShardInput);
    int32 PowerToActivate = FCString::Atoi(*PowerShardInput);
    
    if (SoulToActivate > 0 || PowerToActivate > 0)
    {
        // Call the altar to activate specific amounts
        ActiveAltar->StartSelectiveActivation(PlayerCharacter, SoulToActivate, PowerToActivate);
        
        // Close the UI
        CloseShardActivationUI();
    }
}

void ATrinityFlowHUD::UpdateStancePreview()
{
    // This is called during drawing to update any preview calculations
    // Currently handled inline in DrawShardActivationPanel
}

EStanceType ATrinityFlowHUD::CalculateStanceFromShards(int32 SoulActive, int32 PowerActive) const
{
    if (SoulActive > PowerActive)
    {
        return EStanceType::Soul;
    }
    else if (PowerActive > SoulActive)
    {
        return EStanceType::Power;
    }
    else
    {
        return EStanceType::Balanced;
    }
}