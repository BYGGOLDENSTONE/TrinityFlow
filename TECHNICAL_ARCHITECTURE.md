# TrinityFlow - Technical Architecture Document

## Architecture Overview
TrinityFlow uses Unreal Engine 5's component-based architecture with a focus on modularity, reusability, and clean C++ implementation. The project minimizes Blueprint usage in favor of maintainable C++ code.

## Core Systems Architecture

### Game Instance Subsystems
**UTrinityFlowUIManager** (GameInstanceSubsystem)
- Centralized UI state management
- Persistent across level transitions
- Handles all UI widget creation/destruction
- Events: OnUIStateChanged, OnDamageNumber
- Manages input mode switching (UI/Game/Mixed)

### Component-Based Design

**Character Components**
- **UHealthComponent**: Health/damage management, death handling
- **UStateComponent**: Character state machine (Idle/Alert/Combat/Dead)
- **UStanceComponent**: Dynamic stance flow system, damage modifiers
- **UShardComponent**: Shard collection/activation, damage bonuses
- **UAbilityComponent**: Ability cooldowns, special move execution
- **UAnimationComponent**: Centralized animation management
- **UCameraShakeComponent**: Procedural camera effects

**Enemy Components**
- **UEnemyAnimationComponent**: Enemy-specific animation handling
- **UAIControllerBase**: State machine AI with behavior trees
- Reuses player components: Health, State, Animation

### Inheritance Hierarchy
```
AActor
├── AWeaponBase (Abstract)
│   ├── AOverrideKatana (Soul damage)
│   └── APhysicalKatana (Physical damage)
├── AShardPickup (Collectible)
└── AShardAltar (Interaction point)

ACharacter
├── ATrinityFlowCharacter (Player)
└── AEnemyBase (All enemies)
    ├── AStandardEnemy
    ├── ATankEnemy
    └── APhaseEnemy
```

## UI System (Slate)

### Widget Hierarchy
```
SCompoundWidget
├── STrinityFlowMainMenu
├── STrinityFlowPauseMenu
├── STrinityFlowHUD
│   ├── STrinityFlowHealthBar
│   ├── STrinityFlowWeaponPanel (x2)
│   ├── STrinityFlowStanceBar
│   └── STrinityFlowDamageNumber
├── STrinityFlowShardAltar
└── STrinityFlowEnemyInfoPanel
    └── STrinityFlowDefenseTimingBar
```

### UI Management Flow
1. UIManager creates all widgets on initialization
2. Widgets hidden/shown based on game state
3. Event-driven updates (no polling)
4. Proper cleanup on state transitions

## Data Management

### Stats System
**Data Assets** (UPrimaryDataAsset)
- Character stats configuration
- Weapon stats and abilities
- Enemy stats per type
- Runtime modifiable, no recompilation

**Configuration Storage**
- Game Instance holds weapon stats
- Components cache relevant data
- Hot-reloadable for testing

## Combat System Architecture

### Damage Pipeline
1. Weapon triggers damage event
2. HealthComponent processes damage
3. StateComponent updates combat state
4. UIManager displays damage numbers
5. AnimationComponent plays hit reactions

### Stance Flow Implementation
- Attack input → StanceComponent updates position
- Position determines current stance
- Stance provides damage modifiers
- UI reflects changes in real-time

## AI System Design

### State Machine
- Pure C++ implementation
- States: Idle, Alert, Chase, Attack, Return
- Smooth transitions with validation
- No behavior trees (simplified approach)

### Enemy Behavior
- Line-of-sight detection
- NavMesh pathfinding
- Attack telegraphing (1.5s)
- State persistence across frames

## Animation System

### Centralized Management
**AnimationComponent** handles:
- Montage playback/interruption
- Animation locking
- Combo system timing
- State-based restrictions

### Montage Organization
- Attack: Left/Right slash variations
- Defense: Perfect/Moderate/Failed
- Utility: Interaction, Idle behaviors
- Combat: Hit reactions by damage type

## Performance Optimizations

### Update Strategies
- Component tick only when necessary
- UI updates batched (0.1s intervals)
- Event-driven over polling
- Efficient slate widget reuse

### Memory Management
- Shared pointers for UI widgets
- Weak references for cross-references
- Proper cleanup in destructors
- No circular dependencies

## Networking Preparation
- Components designed for replication
- State changes through authoritative functions
- Client prediction ready architecture
- Server validation points identified

## Build Configuration

### Module Structure
```
Source/TrinityFlow/
├── Core/          (Health, State, Stance, Shards)
├── Combat/        (Weapons, Damage)
├── Player/        (Character, Input)
├── Enemy/         (AI, Enemy types)
├── UI/            (All Slate widgets)
│   └── Slate/     (Widget implementations)
├── World/         (Altars, Pickups)
└── Data/          (Stats, Types)
```

### Dependencies
- Enhanced Input System
- Slate UI Framework
- Navigation System
- No third-party plugins

## Best Practices Implemented

### Code Standards
- UPROPERTY/UFUNCTION for reflection
- Proper null checking
- Extensive logging for debugging
- Clear component boundaries

### Architecture Principles
- Single Responsibility
- Dependency Injection
- Event-driven communication
- Minimal Blueprint coupling

### Extension Points
- New enemy types: Inherit AEnemyBase
- New weapons: Inherit AWeaponBase
- New UI: Add to UIManager states
- New abilities: Extend AbilityComponent

## Development Workflow

### Adding Features
1. Create component or extend existing
2. Add to relevant actor/character
3. Wire up UI if needed
4. Test in isolation

### Debugging Tools
- Extensive console logging
- Visual debug displays
- State inspection commands
- Performance profiling points

## Performance Optimizations

### Memory Management
- Timer handles properly cleaned up in EndPlay/BeginDestroy
- Circular buffer implementation for attack tracking (O(1) vs O(n))
- Pre-allocated arrays for frequently used collections

### Rendering Optimizations
- Debug rendering wrapped in `#if !UE_BUILD_SHIPPING` macros
- No debug overhead in production builds
- Conditional compilation for development-only features

### AI Performance
- Visibility traces cached for 0.2 seconds per enemy
- Reduces expensive line trace operations
- Scales better with high enemy counts

### UI Performance
- Event-driven updates instead of tick-based polling
- UI only updates when values actually change
- Removed unnecessary 10Hz update loops

### Code Safety
- Comprehensive null pointer checks in critical paths
- Atomic state transitions for thread safety
- Defensive programming patterns throughout

## Future Considerations
- Multiplayer replication paths
- Save system integration points
- Modular ability system expansion
- Enhanced AI behavior trees
- Migration to diegetic UI elements
- Behavior tree AI implementation
- Performance scaling options