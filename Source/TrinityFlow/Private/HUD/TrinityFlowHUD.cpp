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
#include "UI/TrinityFlowUIManager.h"

ATrinityFlowHUD::ATrinityFlowHUD()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATrinityFlowHUD::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<ATrinityFlowCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    
    // Setup Slate UI if enabled
    if (bUseSlateUI)
    {
        SetupSlateUI();
    }
    
    // Subscribe to damage events from all actors
    if (UIManager)
    {
        TArray<AActor*> AllActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
        
        for (AActor* Actor : AllActors)
        {
            if (UHealthComponent* HealthComp = Actor->FindComponentByClass<UHealthComponent>())
            {
                HealthComp->OnDamageDealt.AddDynamic(UIManager, &UTrinityFlowUIManager::OnDamageDealt);
            }
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
            // Check for Q key or Circle button to close
            if (PC->WasInputKeyJustPressed(EKeys::Q) || PC->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Right))
            {
                CloseShardActivationUI();
                return;
            }
            
            // Check for A/D or D-Pad Left/Right to switch shard types
            if (PC->WasInputKeyJustPressed(EKeys::A) || PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Left))
            {
                bSelectingSoulShards = true;
                CurrentInputString = &SoulShardInput;
            }
            else if (PC->WasInputKeyJustPressed(EKeys::D) || PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Right))
            {
                bSelectingSoulShards = false;
                CurrentInputString = &PowerShardInput;
            }
            
            // Check for W/S or D-Pad Up/Down to increment/decrement shard amount
            if (CurrentInputString)
            {
                int32 CurrentValue = FCString::Atoi(**CurrentInputString);
                int32 MaxValue = bSelectingSoulShards ? CachedSoulInactive : CachedPowerInactive;
                
                if (PC->WasInputKeyJustPressed(EKeys::W) || PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Up))
                {
                    // Increment
                    CurrentValue = FMath::Clamp(CurrentValue + 1, 0, MaxValue);
                    *CurrentInputString = FString::FromInt(CurrentValue);
                }
                else if (PC->WasInputKeyJustPressed(EKeys::S) || PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Down))
                {
                    // Decrement
                    CurrentValue = FMath::Clamp(CurrentValue - 1, 0, MaxValue);
                    *CurrentInputString = FString::FromInt(CurrentValue);
                }
            }
            
            // E key activation is now handled through character input
            // This prevents input conflicts with Enhanced Input System
        }
    }
}

void ATrinityFlowHUD::DrawHUD()
{
    Super::DrawHUD();

    // Skip Canvas drawing if using Slate UI
    if (bUseSlateUI)
    {
        return;
    }

    if (!Canvas || !PlayerCharacter)
    {
        return;
    }

    ViewportSize = FVector2D(Canvas->SizeX, Canvas->SizeY);

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

void ATrinityFlowHUD::AddFloatingDamageNumber(const FVector& Location, float Damage, bool bIsEcho, EDamageType DamageType)
{
    FFloatingDamageNumber NewNumber;
    NewNumber.WorldLocation = Location + FVector(FMath::RandRange(-30.0f, 30.0f), FMath::RandRange(-30.0f, 30.0f), 100.0f);
    NewNumber.Damage = Damage;
    NewNumber.LifeTime = 2.0f;
    NewNumber.bIsEcho = bIsEcho;
    NewNumber.DamageType = DamageType;
    
    // Handle special defense result cases
    if (Damage < 0)
    {
        if (Damage == -1.0f)
        {
            // Perfect defense or blocked
            NewNumber.Text = TEXT("PERFECT DEFENSE!");
            NewNumber.Color = FLinearColor::Green;
        }
        else if (Damage == -2.0f)
        {
            // Failed defense
            NewNumber.Text = TEXT("FAILED!");
            NewNumber.Color = FLinearColor::Red;
        }
    }
    else
    {
        // Set color based on damage type
        if (bIsEcho)
        {
            NewNumber.Color = FLinearColor(0.0f, 0.5f, 1.0f); // Blue for echo damage
        }
        else if (DamageType == EDamageType::Soul)
        {
            NewNumber.Color = FLinearColor(0.0f, 0.5f, 1.0f); // Blue for soul damage (#0080FF)
        }
        else
        {
            NewNumber.Color = FLinearColor(1.0f, 0.5f, 0.0f); // Orange for physical damage (#FF8000)
        }
    }
    
    FloatingDamageNumbers.Add(NewNumber);
}

void ATrinityFlowHUD::AddFloatingText(const FVector& Location, const FString& Text, const FLinearColor& Color)
{
    FFloatingDamageNumber NewNumber;
    NewNumber.WorldLocation = Location + FVector(FMath::RandRange(-30.0f, 30.0f), FMath::RandRange(-30.0f, 30.0f), 100.0f);
    NewNumber.Damage = 0.0f;
    NewNumber.LifeTime = 2.0f;
    NewNumber.bIsEcho = false;
    NewNumber.Text = Text;
    NewNumber.Color = Color;
    
    FloatingDamageNumbers.Add(NewNumber);
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
    
    // Use Slate UI if enabled
    if (bUseSlateUI && UIManager)
    {
        UIManager->ShowShardAltarUI(Altar);
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
    
    // Subscribe to altar events to know when to close the UI
    Altar->OnAltarInteractionEnded.AddDynamic(this, &ATrinityFlowHUD::CloseShardActivationUI);
    
    // Don't change input mode - we'll handle input filtering in the character class
    UE_LOG(LogTemp, Log, TEXT("Shard activation UI opened"));
}

void ATrinityFlowHUD::CloseShardActivationUI()
{
    // Unsubscribe from altar events
    if (ActiveAltar)
    {
        ActiveAltar->OnAltarInteractionEnded.RemoveDynamic(this, &ATrinityFlowHUD::CloseShardActivationUI);
    }
    
    ShardActivationUIState = EShardActivationUIState::Closed;
    ActiveAltar = nullptr;
    bSelectingSoulShards = true;
    SoulShardInput = "0";
    PowerShardInput = "0";
    CurrentInputString = nullptr;
    
    UE_LOG(LogTemp, Log, TEXT("Shard activation UI closed"));
}

void ATrinityFlowHUD::SetupSlateUI()
{
    // Get UI Manager
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UIManager = GameInstance->GetSubsystem<UTrinityFlowUIManager>();
        if (!UIManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get UIManager subsystem"));
            return;
        }
        
        // The UI Manager already shows the HUD in game mode initialization
        // We just need to subscribe to updates
        
        // Subscribe to damage events to forward to Slate UI
        // The damage events are already being captured in BeginPlay
    }
}

void ATrinityFlowHUD::ProcessShardActivation()
{
    if (!ActiveAltar || !PlayerCharacter)
    {
        return;
    }
    
    // Get input values
    int32 SoulShards = FCString::Atoi(*SoulShardInput);
    int32 PowerShards = FCString::Atoi(*PowerShardInput);
    
    // Validate input
    if (SoulShards == 0 && PowerShards == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No shards selected for activation"));
        return;
    }
    
    // Activate shards through the altar
    ActiveAltar->StartSelectiveActivation(PlayerCharacter, SoulShards, PowerShards);
    CloseShardActivationUI();
}

void ATrinityFlowHUD::DrawAltarInteractionUI()
{
    // Check if player is near an altar
    NearbyAltar = GetNearbyAltar();
    if (!NearbyAltar)
    {
        return;
    }
    
    // Draw interaction prompt
    const FString InteractText = TEXT("Press E to interact with Shard Altar");
    float TextWidth, TextHeight;
    GetTextSize(InteractText, TextWidth, TextHeight, GEngine->GetLargeFont());
    
    float PosX = ViewportSize.X * 0.5f - TextWidth * 0.5f;
    float PosY = ViewportSize.Y * 0.7f;
    
    DrawText(InteractText, FLinearColor::White, PosX, PosY, GEngine->GetLargeFont());
}

void ATrinityFlowHUD::DrawShardActivationPanel()
{
    if (!Canvas || !ActiveAltar)
    {
        return;
    }
    
    // Panel dimensions
    float PanelWidth = 400.0f;
    float PanelHeight = 300.0f;
    float PanelX = (ViewportSize.X - PanelWidth) * 0.5f;
    float PanelY = (ViewportSize.Y - PanelHeight) * 0.5f;
    
    // Draw panel background
    DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f), PanelX, PanelY, PanelWidth, PanelHeight);
    
    // Draw title
    const FString TitleText = TEXT("Shard Activation");
    float TitleWidth, TitleHeight;
    GetTextSize(TitleText, TitleWidth, TitleHeight, GEngine->GetLargeFont());
    DrawText(TitleText, FLinearColor::White, PanelX + (PanelWidth - TitleWidth) * 0.5f, PanelY + 20.0f, GEngine->GetLargeFont());
    
    // Draw shard counts
    const FString SoulText = FString::Printf(TEXT("Soul Shards: %d/%d"), FCString::Atoi(*SoulShardInput), CachedSoulInactive);
    const FString PowerText = FString::Printf(TEXT("Power Shards: %d/%d"), FCString::Atoi(*PowerShardInput), CachedPowerInactive);
    
    FLinearColor SoulColor = bSelectingSoulShards ? FLinearColor::Yellow : FLinearColor::White;
    FLinearColor PowerColor = !bSelectingSoulShards ? FLinearColor::Yellow : FLinearColor::White;
    
    DrawText(SoulText, SoulColor, PanelX + 50.0f, PanelY + 80.0f, GEngine->GetMediumFont());
    DrawText(PowerText, PowerColor, PanelX + 50.0f, PanelY + 120.0f, GEngine->GetMediumFont());
    
    // Draw stance preview
    UpdateStancePreview();
    
    // Draw controls
    const FString ControlsText = TEXT("A/D: Select Type | W/S: Change Amount | E: Activate | Q: Cancel");
    float ControlsWidth, ControlsHeight;
    GetTextSize(ControlsText, ControlsWidth, ControlsHeight, GEngine->GetSmallFont());
    DrawText(ControlsText, FLinearColor::Gray, PanelX + (PanelWidth - ControlsWidth) * 0.5f, PanelY + PanelHeight - 40.0f, GEngine->GetSmallFont());
}

void ATrinityFlowHUD::UpdateStancePreview()
{
    if (!Canvas || !PlayerCharacter)
    {
        return;
    }
    
    // Calculate what stance would result from activation
    int32 SoulToActivate = FCString::Atoi(*SoulShardInput);
    int32 PowerToActivate = FCString::Atoi(*PowerShardInput);
    
    int32 NewSoulActive = CachedSoulActive + SoulToActivate;
    int32 NewPowerActive = CachedPowerActive + PowerToActivate;
    
    EStanceType PreviewStance = CalculateStanceFromShards(NewSoulActive, NewPowerActive);
    
    // Draw preview text
    FString StanceText = TEXT("Stance Preview: ");
    FLinearColor StanceColor = FLinearColor::White;
    
    switch (PreviewStance)
    {
        case EStanceType::Soul:
            StanceText += TEXT("Soul");
            StanceColor = FLinearColor(0.0f, 0.5f, 1.0f);
            break;
        case EStanceType::Power:
            StanceText += TEXT("Power");
            StanceColor = FLinearColor(1.0f, 0.5f, 0.0f);
            break;
        case EStanceType::Balanced:
            StanceText += TEXT("Balanced");
            StanceColor = FLinearColor(0.8f, 0.2f, 1.0f);
            break;
    }
    
    float PanelWidth = 400.0f;
    float PanelX = (ViewportSize.X - PanelWidth) * 0.5f;
    float PanelY = (ViewportSize.Y - 300.0f) * 0.5f;
    
    DrawText(StanceText, StanceColor, PanelX + 50.0f, PanelY + 180.0f, GEngine->GetMediumFont());
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