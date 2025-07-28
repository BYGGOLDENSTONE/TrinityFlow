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
}

void ATrinityFlowHUD::DrawHealthBar()
{
    if (UHealthComponent* HealthComp = PlayerCharacter->GetHealthComponent())
    {
        float HealthPercentage = HealthComp->GetHealthPercentage();
        
        // Position above player info at bottom left
        float X = 20.0f;
        float Y = ViewportSize.Y - 140.0f;  // Above player info section

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
    // Position at bottom left corner
    float X = 20.0f;
    float Y = ViewportSize.Y - 120.0f;  // Start from bottom with some margin

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