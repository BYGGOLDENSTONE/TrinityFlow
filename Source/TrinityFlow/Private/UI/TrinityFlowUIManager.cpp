#include "UI/TrinityFlowUIManager.h"
#include "UI/TrinityFlowStyle.h"
#include "UI/Slate/STrinityFlowMainMenu.h"
#include "UI/Slate/STrinityFlowPauseMenu.h"
#include "UI/Slate/STrinityFlowHUD.h"
#include "UI/Slate/STrinityFlowShardAltar.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SWeakWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Core/StateComponent.h"
#include "Core/HealthComponent.h"
#include "Enemy/EnemyBase.h"
#include "World/ShardAltar.h"

void UTrinityFlowUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize the style set
    FTrinityFlowStyle::Initialize();
    
    CreateWidgets();
}

void UTrinityFlowUIManager::Deinitialize()
{
    HideAllUI();
    
    MainMenuWidget.Reset();
    PauseMenuWidget.Reset();
    HUDWidget.Reset();
    ShardAltarWidget.Reset();
    
    // Shutdown the style set
    FTrinityFlowStyle::Shutdown();
    
    Super::Deinitialize();
}

void UTrinityFlowUIManager::CreateWidgets()
{
    // Create all widgets upfront for better performance
    MainMenuWidget = SNew(STrinityFlowMainMenu)
        .UIManager(this);
    
    PauseMenuWidget = SNew(STrinityFlowPauseMenu)
        .UIManager(this);
    
    HUDWidget = SNew(STrinityFlowHUD)
        .UIManager(this);
    
    ShardAltarWidget = SNew(STrinityFlowShardAltar)
        .UIManager(this);
}

void UTrinityFlowUIManager::SetUIState(EUIState NewState)
{
    if (CurrentUIState == NewState)
    {
        return;
    }
    
    EUIState OldState = CurrentUIState;
    CurrentUIState = NewState;
    
    // Handle state transitions
    RemoveCurrentWidget();
    
    switch (NewState)
    {
        case EUIState::MainMenu:
            ShowWidget(MainMenuWidget);
            SetInputModeUI();
            break;
            
        case EUIState::InGame:
            ShowWidget(HUDWidget);
            SetInputModeGame();
            break;
            
        case EUIState::PauseMenu:
            ShowWidget(PauseMenuWidget);
            SetInputModeUI();
            break;
            
        case EUIState::ShardAltar:
            ShowWidget(ShardAltarWidget);
            // Use GameAndUI mode for altar to allow keyboard input
            if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                FInputModeGameAndUI InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                InputMode.SetWidgetToFocus(ShardAltarWidget);
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
            break;
            
        case EUIState::None:
        default:
            break;
    }
    
    OnUIStateChanged.Broadcast(NewState);
}

void UTrinityFlowUIManager::ShowMainMenu()
{
    SetUIState(EUIState::MainMenu);
}

void UTrinityFlowUIManager::ShowPauseMenu()
{
    SetUIState(EUIState::PauseMenu);
}

void UTrinityFlowUIManager::ShowInGameHUD()
{
    SetUIState(EUIState::InGame);
}

void UTrinityFlowUIManager::ShowShardAltarUI(AShardAltar* Altar)
{
    if (ShardAltarWidget.IsValid())
    {
        ShardAltarWidget->SetAltar(Altar);
    }
    SetUIState(EUIState::ShardAltar);
    
    // Set focus to the widget after state change
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        // Give focus to the altar widget
        FSlateApplication::Get().SetUserFocus(0, ShardAltarWidget);
    }
}

void UTrinityFlowUIManager::HideAllUI()
{
    RemoveCurrentWidget();
    CurrentUIState = EUIState::None;
}

void UTrinityFlowUIManager::ShowWidget(TSharedPtr<SWidget> Widget)
{
    if (!Widget.IsValid())
    {
        return;
    }
    
    UGameViewportClient* GameViewport = GetGameInstance()->GetGameViewportClient();
    if (!GameViewport)
    {
        return;
    }
    
    CurrentWidgetContainer = SNew(SWeakWidget)
        .PossiblyNullContent(Widget.ToSharedRef());
    
    GameViewport->AddViewportWidgetContent(CurrentWidgetContainer.ToSharedRef());
}

void UTrinityFlowUIManager::RemoveCurrentWidget()
{
    if (!CurrentWidgetContainer.IsValid())
    {
        return;
    }
    
    UGameViewportClient* GameViewport = GetGameInstance()->GetGameViewportClient();
    if (GameViewport)
    {
        GameViewport->RemoveViewportWidgetContent(CurrentWidgetContainer.ToSharedRef());
    }
    
    CurrentWidgetContainer.Reset();
}

void UTrinityFlowUIManager::SetInputModeUI()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
        // Set focus to the current widget if it's a menu
        if (CurrentUIState == EUIState::MainMenu && MainMenuWidget.IsValid())
        {
            InputMode.SetWidgetToFocus(MainMenuWidget);
        }
        else if (CurrentUIState == EUIState::PauseMenu && PauseMenuWidget.IsValid())
        {
            InputMode.SetWidgetToFocus(PauseMenuWidget);
        }
        
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void UTrinityFlowUIManager::SetInputModeGame()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}

void UTrinityFlowUIManager::AddDamageNumber(const FVector& WorldLocation, float Damage, bool bIsEcho, EDamageType DamageType)
{
    if (HUDWidget.IsValid())
    {
        HUDWidget->AddDamageNumber(WorldLocation, Damage, bIsEcho, DamageType);
    }
}

void UTrinityFlowUIManager::UpdatePlayerHealth(float HealthPercentage)
{
    if (HUDWidget.IsValid())
    {
        HUDWidget->UpdatePlayerHealth(HealthPercentage);
    }
}

void UTrinityFlowUIManager::UpdateWeaponCooldowns(float QCooldown, float TabCooldown, float ECooldown, float RCooldown)
{
    if (HUDWidget.IsValid())
    {
        HUDWidget->UpdateWeaponCooldowns(QCooldown, TabCooldown, ECooldown, RCooldown);
    }
}

void UTrinityFlowUIManager::UpdatePlayerStats(int32 SoulActive, int32 PowerActive, int32 SoulInactive, int32 PowerInactive, float SoulBonus, float PhysicalBonus)
{
    if (HUDWidget.IsValid())
    {
        HUDWidget->UpdatePlayerStats(SoulActive, PowerActive, SoulInactive, PowerInactive, SoulBonus, PhysicalBonus);
    }
}

void UTrinityFlowUIManager::UpdateCombatState(bool bInCombat)
{
    if (HUDWidget.IsValid())
    {
        HUDWidget->UpdateCombatState(bInCombat);
    }
}

void UTrinityFlowUIManager::RegisterEnemy(AEnemyBase* Enemy)
{
    if (Enemy)
    {
        RegisteredEnemies.AddUnique(Enemy);
        
        // Subscribe to enemy's damage events
        if (UHealthComponent* HealthComp = Enemy->FindComponentByClass<UHealthComponent>())
        {
            HealthComp->OnDamageDealt.AddDynamic(this, &UTrinityFlowUIManager::OnDamageDealt);
        }
    }
}

void UTrinityFlowUIManager::UnregisterEnemy(AEnemyBase* Enemy)
{
    if (Enemy)
    {
        RegisteredEnemies.Remove(Enemy);
        
        // Unsubscribe from enemy's damage events
        if (UHealthComponent* HealthComp = Enemy->FindComponentByClass<UHealthComponent>())
        {
            HealthComp->OnDamageDealt.RemoveDynamic(this, &UTrinityFlowUIManager::OnDamageDealt);
        }
    }
}

const TArray<AEnemyBase*>& UTrinityFlowUIManager::GetRegisteredEnemies() const
{
    return RegisteredEnemies;
}

void UTrinityFlowUIManager::OnDamageDealt(AActor* DamagedActor, float ActualDamage, AActor* DamageInstigator, EDamageType DamageType)
{
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
    {
        if (DamageInstigator == PlayerPawn && DamagedActor)
        {
            bool bIsEcho = false;
            if (UStateComponent* StateComp = DamagedActor->FindComponentByClass<UStateComponent>())
            {
                bIsEcho = StateComp->IsMarked() && DamageType == EDamageType::Soul;
            }

            UE_LOG(LogTemp, Warning, TEXT("UIManager OnDamageDealt: Damage=%.1f, Type=%s, IsEcho=%s"), 
                ActualDamage, 
                DamageType == EDamageType::Soul ? TEXT("Soul") : TEXT("Physical"),
                bIsEcho ? TEXT("Yes") : TEXT("No"));

            AddDamageNumber(DamagedActor->GetActorLocation(), ActualDamage, bIsEcho, DamageType);
        }
    }
}

void UTrinityFlowUIManager::OpenShardActivationUI(AShardAltar* Altar)
{
    if (!Altar)
    {
        return;
    }
    
    ActiveAltar = Altar;
    ShowShardAltarUI(Altar);
    
    // Subscribe to altar interaction end event
    Altar->OnAltarInteractionEnded.AddDynamic(this, &UTrinityFlowUIManager::CloseShardActivationUI);
}

void UTrinityFlowUIManager::CloseShardActivationUI()
{
    if (ActiveAltar)
    {
        ActiveAltar->OnAltarInteractionEnded.RemoveDynamic(this, &UTrinityFlowUIManager::CloseShardActivationUI);
        ActiveAltar = nullptr;
    }
    
    ShowInGameHUD();
}


void UTrinityFlowUIManager::AddFloatingText(const FVector& WorldLocation, const FString& Text, const FLinearColor& Color)
{
    // For now, we'll just add this as a damage number with 0 damage
    // In the future, we can create a dedicated floating text widget
    AddDamageNumber(WorldLocation, 0.0f, false, EDamageType::Physical);
}