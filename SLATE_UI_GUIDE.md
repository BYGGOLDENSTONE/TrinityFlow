# Slate UI System Guide

## Overview
TrinityFlow now features a modern Slate-based UI system that replaces the old Canvas-based HUD rendering. This system provides better performance, smoother animations, and a more professional appearance.

## Features

### 1. Main Menu
- Clean, animated main menu with fade-in effects
- Start Game, Settings, and Quit options
- Styled with TrinityFlow theme colors

### 2. Pause Menu
- Semi-transparent overlay when paused
- Resume, Settings, Main Menu, and Quit options
- Proper input mode switching

### 3. In-Game HUD
- **Health Bar**: Animated with damage flash effects
- **Weapon Panels**: Shows cooldowns for both katanas
- **Combat State**: Visual indicator (IN COMBAT / NON-COMBAT)
- **Player Stats**: Stance, shards, and damage bonuses
- **Floating Damage Numbers**: Slate-based with proper scaling
- **Crosshair**: Simple, clean design

### 4. Shard Altar UI
- Full-screen interface with animations
- Visual shard selection with D-Pad/WASD controls
- Real-time stance preview
- Animated activation progress

## Setup Instructions

### Enable Slate UI

1. **In Blueprint**:
   - Open `BP_TrinityFlowHUD` (or create it from `ATrinityFlowHUD`)
   - In Class Defaults, find the UI category
   - Check "Use Slate UI" (enabled by default)

2. **Game Mode Setup**:
   - Create `BP_TrinityFlowGameMode` from `ATrinityFlowGameMode`
   - Set it as the default game mode in Project Settings
   - Or set it per-level in World Settings

3. **Game Instance**:
   - Ensure `UTrinityFlowGameInstance` is set in Project Settings
   - This is required for the UI Manager subsystem

## Architecture

### UI Manager (`UTrinityFlowUIManager`)
- Central subsystem that manages all UI states
- Handles transitions between menus
- Provides unified API for UI updates

### Widget Structure
```
STrinityFlowMainMenu      - Main menu
STrinityFlowPauseMenu     - Pause menu
STrinityFlowHUD           - In-game HUD container
  ├── STrinityFlowHealthBar    - Health display
  ├── STrinityFlowWeaponPanel  - Weapon info (x2)
  └── STrinityFlowDamageNumber - Floating damage
STrinityFlowShardAltar    - Altar interaction UI
```

## Adding Pause Functionality

To enable pause menu with ESC key:

1. **In Character Blueprint**:
   ```cpp
   // Add to SetupPlayerInputComponent
   InputComponent->BindAction("Pause", IE_Pressed, this, &ATrinityFlowCharacter::RequestPause);
   ```

2. **Add RequestPause Method**:
   ```cpp
   void ATrinityFlowCharacter::RequestPause()
   {
       if (ATrinityFlowGameMode* GameMode = Cast<ATrinityFlowGameMode>(GetWorld()->GetAuthGameMode()))
       {
           GameMode->OnPauseRequested();
       }
   }
   ```

3. **Create Pause Input Action**:
   - Create new Input Action: `IA_Pause`
   - Map to ESC key
   - Add to Input Mapping Context

## Customization

### Colors
The UI uses TrinityFlow's color scheme:
- **Soul (Blue)**: #0080FF
- **Power (Orange)**: #FF8000  
- **Balanced (Purple)**: #CC33FF

To change colors, modify the color values in the respective widget constructors.

### Fonts
Default font: Roboto (from Engine content)
To change: Update font paths in widget constructors

### Animations
- Main menu fade-in: 0.5s quadratic ease-out
- Shard altar scale-in: 0.5s quadratic ease-out
- Damage numbers: 2s lifetime with upward float

## Switching Between Canvas and Slate

The HUD supports both rendering modes:

1. **Use Slate** (default):
   - Set `bUseSlateUI = true` in HUD
   - Modern, animated UI

2. **Use Canvas** (legacy):
   - Set `bUseSlateUI = false` in HUD
   - Original immediate-mode rendering

## Performance Considerations

- Slate widgets are created once and reused
- Damage numbers are pooled and recycled
- UI updates only when values change
- Efficient batching of draw calls

## Troubleshooting

### UI Not Appearing
1. Check that `bUseSlateUI` is enabled
2. Verify Game Mode is set correctly
3. Ensure Game Instance has UI Manager subsystem

### Input Issues
1. Check input mode is set correctly
2. Verify focus is on the correct widget
3. Ensure mouse cursor visibility matches UI state

### Damage Numbers Not Showing
1. Verify damage events are firing
2. Check that player is the damage instigator
3. Ensure UI Manager is receiving events

## Future Enhancements

1. **Settings Menu**: Graphics and audio options
2. **Enemy Info Panels**: Slate-based enemy health bars
3. **Inventory UI**: If inventory system is added
4. **Quest Log**: For quest tracking
5. **Mini-map**: Top-down view of area

## Example: Adding a New HUD Element

```cpp
// 1. Create widget class
class SMyWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyWidget) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);
};

// 2. Add to HUD
TSharedPtr<SMyWidget> MyWidget;
MyWidget = SNew(SMyWidget);

// 3. Add to HUD layout
+ SOverlay::Slot()
.HAlign(HAlign_Right)
.VAlign(VAlign_Top)
[
    MyWidget.ToSharedRef()
]
```