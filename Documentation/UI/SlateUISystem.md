# TrinityFlow Slate UI System Documentation

## Overview

The TrinityFlow UI system has been migrated to use Unreal Engine's Slate framework for better performance and more advanced UI capabilities. This document outlines the new Slate-based UI architecture.

## Architecture

### Core Components

1. **UTrinityFlowUIManager** (GameInstanceSubsystem)
   - Central UI state management
   - Widget lifecycle control
   - Input mode switching
   - Enemy registration for HUD tracking

2. **FTrinityFlowStyle** (Slate Style Set)
   - Centralized styling for all UI elements
   - Color definitions (Soul, Power, Balanced, etc.)
   - Font configurations
   - Brush definitions

### Widget Hierarchy

```
UTrinityFlowUIManager
├── STrinityFlowMainMenu
├── STrinityFlowPauseMenu
├── STrinityFlowHUD
│   ├── STrinityFlowHealthBar
│   ├── STrinityFlowWeaponPanel (x2)
│   ├── STrinityFlowDamageNumber (dynamic)
│   └── STrinityFlowEnemyInfoPanel (per enemy)
└── STrinityFlowShardAltar
```

## Implementation Details

### HUD System (STrinityFlowHUD)

The main HUD widget uses an overlay structure:
- **Enemy Info Layer**: Uses SOverlay for dynamic enemy health bars
- **Damage Numbers Layer**: Floating damage text
- **Player UI Layer**: Health bar, weapon panels, combat state
- **Crosshair**: Center screen targeting reticle

Key Features:
- Dynamic enemy info panels that track world positions
- Floating damage numbers with type-based coloring
- Real-time weapon cooldown displays
- Stance and shard count visualization

### Enemy Info Panels

Enemy info panels are dynamically created and positioned:
```cpp
// Panels use RenderTransform for screen positioning
Panel->SetRenderTransform(FSlateRenderTransform(ScreenPosition));
```

### Style System

The style system provides consistent theming:
```cpp
// Color definitions
Soul: FLinearColor(0.0f, 0.5f, 1.0f)     // Blue
Power: FLinearColor(1.0f, 0.5f, 0.0f)    // Orange
Balanced: FLinearColor(0.8f, 0.2f, 1.0f) // Purple
```

## Known Issues (To Be Fixed)

1. **SCanvas Include Path**: Initial implementation used incorrect include paths
2. **Enemy Panel Positioning**: Dynamic positioning needs refinement
3. **Missing Widget Implementations**: Some referenced widgets need implementation
4. **Compile Errors**: Various linking and method call issues

## Migration from Canvas to Slate

The system maintains backward compatibility with Canvas-based rendering:
- `bUseSlateUI` flag in ATrinityFlowHUD (defaults to true)
- Canvas drawing methods remain for fallback
- Slate UI takes precedence when enabled

## Integration Points

1. **Damage Events**: 
   - Health components fire damage events
   - UI Manager subscribes and creates damage numbers

2. **Enemy Registration**:
   - Enemies register/unregister with UI Manager
   - HUD creates/removes info panels dynamically

3. **Player Stats Updates**:
   - Direct calls to update health, cooldowns, stance
   - Weapon panels show ability availability

## Build Configuration

Required modules in TrinityFlow.Build.cs:
- Slate
- SlateCore
- UMG (for future Blueprint integration)

## Future Improvements

1. Implement missing widget classes
2. Add animation support for damage numbers
3. Improve enemy panel positioning algorithm
4. Add UI scaling for different resolutions
5. Implement settings menu using Slate