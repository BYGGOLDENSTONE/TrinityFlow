#include "HUD/TrinityFlowHUD.h"
#include "../../TrinityFlowCharacter.h"
#include "Core/HealthComponent.h"
#include "Core/TagComponent.h"
#include "Core/StateComponent.h"
#include "Player/OverrideKatana.h"
#include "Player/DivineAnchor.h"
#include "Enemy/EnemyBase.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ATrinityFlowHUD::ATrinityFlowHUD()
{
}

void ATrinityFlowHUD::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<ATrinityFlowCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void ATrinityFlowHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !PlayerCharacter)
    {
        return;
    }

    ViewportSize = FVector2D(Canvas->SizeX, Canvas->SizeY);

    // Update current target
    CurrentTarget = PlayerCharacter->GetTargetInSight();

    // Draw all HUD elements
    DrawHealthBar();
    DrawCrosshair();
    DrawTargetInfo();
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
        
        float X = ViewportSize.X * 0.5f - HealthBarWidth * 0.5f;
        float Y = ViewportSize.Y - 100.0f;

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

void ATrinityFlowHUD::DrawTargetInfo()
{
    if (!CurrentTarget)
    {
        return;
    }

    float X = ViewportSize.X - InfoPanelWidth - 20.0f;
    float Y = 100.0f;

    // Background
    DrawRect(FLinearColor(0, 0, 0, 0.7f), X - 10, Y - 10, InfoPanelWidth + 20, 200);

    DrawText(TEXT("=== TARGET INFO ==="), FLinearColor::Yellow, X, Y);
    Y += LineHeight;

    // Health info
    if (UHealthComponent* HealthComp = CurrentTarget->FindComponentByClass<UHealthComponent>())
    {
        FString HealthText = FString::Printf(TEXT("Health: %.0f/%.0f"), 
            HealthComp->GetHealth(), HealthComp->GetMaxHealth());
        DrawText(HealthText, FLinearColor::Red, X, Y);
        Y += LineHeight;

        // Health bar
        float HealthPercentage = HealthComp->GetHealthPercentage();
        DrawBar(X, Y, InfoPanelWidth - 20, 10, HealthPercentage, FLinearColor::Red);
        Y += 15;

        // Resources
        const FCharacterResources& Resources = HealthComp->GetResources();
        DrawText(FString::Printf(TEXT("Attack: %.0f"), Resources.AttackPoint), FLinearColor(1.0f, 0.5f, 0.0f), X, Y);
        Y += LineHeight;
        DrawText(FString::Printf(TEXT("Defence: %.0f"), Resources.DefencePoint), FLinearColor::Blue, X, Y);
        Y += LineHeight;
    }

    // Tags
    if (UTagComponent* TagComp = CurrentTarget->FindComponentByClass<UTagComponent>())
    {
        FString TagsText = TEXT("Tags: ") + GetTagsString(TagComp->GetTags());
        DrawText(TagsText, FLinearColor::Green, X, Y);
        Y += LineHeight;
    }

    // States
    if (UStateComponent* StateComp = CurrentTarget->FindComponentByClass<UStateComponent>())
    {
        FString StatesText = TEXT("States: ") + GetStatesString(StateComp->GetStates());
        DrawText(StatesText, FLinearColor(0.0f, 1.0f, 1.0f), X, Y);
        Y += LineHeight;

        if (StateComp->IsMarked())
        {
            FString MarkedText = FString::Printf(TEXT("Marked: %.1fs"), StateComp->GetMarkedTimeRemaining());
            DrawText(MarkedText, FLinearColor(0.5f, 0.0f, 1.0f), X, Y);
        }
    }
}

void ATrinityFlowHUD::DrawPlayerInfo()
{
    float X = 20.0f;
    float Y = 100.0f;

    // Background
    DrawRect(FLinearColor(0, 0, 0, 0.7f), X - 10, Y - 10, InfoPanelWidth + 20, 150);

    DrawText(TEXT("=== PLAYER INFO ==="), FLinearColor::Yellow, X, Y);
    Y += LineHeight;

    if (UHealthComponent* HealthComp = PlayerCharacter->GetHealthComponent())
    {
        const FCharacterResources& Resources = HealthComp->GetResources();
        DrawText(FString::Printf(TEXT("Health: %.0f/%.0f"), Resources.Health, Resources.MaxHealth), FLinearColor::Red, X, Y);
        Y += LineHeight;
        DrawText(FString::Printf(TEXT("Attack: %.0f"), Resources.AttackPoint), FLinearColor(1.0f, 0.5f, 0.0f), X, Y);
        Y += LineHeight;
        DrawText(FString::Printf(TEXT("Defence: %.0f"), Resources.DefencePoint), FLinearColor::Blue, X, Y);
        Y += LineHeight;
    }

    if (UTagComponent* TagComp = PlayerCharacter->GetTagComponent())
    {
        FString TagsText = TEXT("Tags: ") + GetTagsString(TagComp->GetTags());
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

    FString WeaponName = PlayerCharacter->IsKatanaActive() ? TEXT("Override Katana") : TEXT("Divine Anchor");
    DrawText(TEXT("Current Weapon: ") + WeaponName, FLinearColor::White, X, Y);

    Y += LineHeight;

    // Ability cooldowns
    if (AWeaponBase* Weapon = PlayerCharacter->GetCurrentWeapon())
    {
        float QCooldown = Weapon->GetAbilityQCooldownRemaining();
        float ECooldown = Weapon->GetAbilityECooldownRemaining();

        FString QText = QCooldown > 0 ? FString::Printf(TEXT("Q: %.1fs"), QCooldown) : TEXT("Q: Ready");
        FString EText = ECooldown > 0 ? FString::Printf(TEXT("E: %.1fs"), ECooldown) : TEXT("E: Ready");

        FLinearColor QColor = QCooldown > 0 ? FLinearColor::Red : FLinearColor::Green;
        FLinearColor EColor = ECooldown > 0 ? FLinearColor::Red : FLinearColor::Green;

        DrawText(QText, QColor, X, Y);
        DrawText(EText, EColor, X + 100, Y);
    }
}

void ATrinityFlowHUD::DrawNearbyEnemies()
{
    float X = 20.0f;
    float Y = ViewportSize.Y - 300.0f;

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