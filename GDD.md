# TrinityFlow - Complete Game Design Document

## Table of Contents
1. [Game Overview](#game-overview)
2. [Core Mechanics](#core-mechanics)
3. [Player Character](#player-character)
4. [Weapons & Abilities](#weapons--abilities)
5. [Enemy Design](#enemy-design)
6. [Combat System](#combat-system)
7. [UI/UX Design](#uiux-design)
8. [Technical Implementation](#technical-implementation)
9. [Stats Management System](#stats-management-system)
10. [Setup & Configuration](#setup--configuration)

---

## Game Overview

**Game Name**: TrinityFlow  
**Engine**: Unreal Engine 5.5 (C++ focused, component-based architecture)  
**Genre**: Third-person action combat  
**Core Hook**: Dual-damage system where physical and soul damage interact uniquely with enemy defenses

### Design Philosophy
- Fast-paced combat with meaningful player choices
- Clear visual feedback for all actions
- Weapon switching as core tactical element
- Physics-based abilities that feel impactful

---

## Core Mechanics

### Resources
1. **Health**
   - Only visible resource on HUD
   - All damage types affect this resource
   - No regeneration (design choice)

2. **Defense Point**
   - Value for armored characters defining defenses
   - Formula: `Received Physical Damage = Attack * (100 - DefensePoint)%`
   - Example: 10 attack vs 20 defense = 8 actual damage

3. **Attack Point**
   - Base damage value for characters
   - Modified by damage type and abilities

### Damage Types

#### Physical Damage
- Reduced by defense points
- Can be blocked by shields (frontal only)
- Standard damage calculation with defense reduction

#### Soul Damage
- Bypasses ALL defenses (shields and armor)
- 2x damage multiplier
- Formula: `Soul Damage = AttackPoint * 2`
- Cannot affect Mechanical enemies

### Tag System

Tags define character properties and damage interactions:

| Tag | Effect | Description |
|-----|--------|-------------|
| **Shielded** | Blocks frontal physical damage | Complete immunity from front regardless of defense |
| **Armored** | Has defense > 0 | Reduces physical damage by defense percentage |
| **Ghost** | Physical immunity | Takes no physical damage, only soul damage |
| **Mechanical** | Soul immunity | Takes no soul damage, only physical damage |
| **HaveSoul** | Can take soul damage | Standard property for living beings |

### States

Dynamic conditions affecting gameplay:

| State | Effect | Duration |
|-------|--------|----------|
| **Vulnerable** | Defense < 0, no movement/attack | Variable |
| **Combat** | In active combat | Until victory/death |
| **Non-Combat** | Exploration mode | Default |
| **Marked** | Takes echo damage | 5 seconds |

---

## Player Character

### Base Stats
- **Health**: 200
- **Attack**: 20  
- **Defense**: 20
- **Tags**: HaveSoul, Armored

### Movement
- **Combat**: No jumping, space = defensive ability
- **Non-Combat**: Standard movement + jumping
- **Weapon Switch**: Instant, no cooldown

---

## Weapons & Abilities

### Override Katana
*Fast, precise weapon with soul damage focus*

#### Basic Attack - Slash
- **Damage**: Physical
- **Range**: 300 units
- **Speed**: 1.0 attacks/second
- **Type**: Single target

#### Q - Echoes of Data
- **Damage**: Soul (to marked target)
- **Range**: 4000 units
- **Cooldown**: 5 seconds
- **Duration**: 5 seconds mark
- **Effect**: Marked enemy receives 75% of damage dealt to others as soul damage
- **AoE Rule**: 30% damage for area attacks

#### E - Code Break
- **Damage**: Soul
- **Range**: 600 units
- **Cooldown**: 6 seconds
- **Effect**: Enhanced slash dealing soul damage

#### Space - Scripted Dodge
- **Window**: 1.5 seconds total
- **Moderate (0-0.75s)**: Take 50% damage
- **Perfect (0.75-1.5s)**: No damage + Code Break cooldown reset
- **Visual**: Orange (moderate) → Green (perfect)

### Divine Anchor
*Heavy weapon with crowd control and physics*

#### Basic Attack - Smash
- **Damage**: Physical, Area
- **Range**: 400 units radius
- **Speed**: 0.5 attacks/second
- **Effect**: Knockback 100 units with physics
- **Physics**: 500 force, 0.5 upward ratio

#### Q - Gravity Pull
- **Damage**: Physical
- **Range**: 1000 units
- **Cooldown**: 6 seconds
- **Effect**: Pulls enemy with physics arc
- **Vulnerable**: 1.5 seconds while airborne
- **Physics**: 2.5x force multiplier, 600 upward velocity

#### E - Holy Gravity
- **Damage**: Physical, Area
- **Range**: 1000 units radius
- **Cooldown**: 10 seconds
- **Effect**: All enemies vulnerable for 2 seconds

#### Space - Order
- **Window**: 1.5 seconds total
- **Moderate (0-0.75s)**: Take 50% damage
- **Perfect (0.75-1.5s)**: No damage + counter attack
- **Counter Effect**: 
  - Strips shield if present
  - Reduces armor by 25% if no shield

---

## Enemy Design

### Standard Enemy
- **Health**: 100
- **Attack**: 10
- **Defense**: 0
- **Tags**: HaveSoul
- **Range**: 300
- **Behavior**: Basic melee attacker

### Tank Enemy
- **Health**: 200
- **Attack**: 40
- **Defense**: 20
- **Tags**: HaveSoul, Armored
- **Range**: 300
- **Special**: Area damage attacks

### Shielded Tank Enemy
- **Health**: 300
- **Attack**: 20
- **Defense**: 20
- **Tags**: HaveSoul, Armored, Shielded
- **Range**: 300
- **Special**: Frontal damage immunity

### Phase Enemy
- **Health**: 100
- **Attack**: 20
- **Defense**: 0
- **Tags**: Ghost
- **Range**: 300
- **Special**: Immune to physical damage

### Shielded Tank Robot Enemy
- **Health**: 300
- **Attack**: 30
- **Defense**: 40
- **Tags**: Mechanical, Armored, Shielded
- **Range**: 300
- **Special**: Immune to soul damage

### Enemy AI
- **Sight Range**: 1500 units
- **Attack Speed**: 1 attack per 1.5 seconds
- **Behavior**: Face player, attack when in range

---

## Combat System

### Attack Timing
- **Cast Time**: 1.5 seconds
- **Visual Indicator**: Growing line from enemy to player
- **Color Coding**:
  - Orange (0-0.75s): Moderate defense window
  - Green (0.75-1.5s): Perfect defense window

### Damage Calculation

#### Physical Damage
```
ActualDamage = AttackPoint * (100 - DefensePoint) / 100
```
- Blocked by shields from front
- Reduced by armor
- Affected by vulnerability state

#### Soul Damage
```
ActualDamage = AttackPoint * 2
```
- Ignores all defenses
- Cannot affect Mechanical enemies
- Ideal for high-defense targets

### Echo Damage System
When enemy is marked by Echoes of Data:
- Single Target Hit: Marked enemy takes 75% of actual damage as soul
- AoE Hit: Marked enemy takes 30% of total AoE damage as soul
- Stacks from multiple hits
- Visual: Purple "ECHO" numbers

### Physics System

#### Gravity Pull
- Force-based pulling with arc trajectory
- Configurable force multiplier (default 2.5x)
- Enemies become vulnerable while airborne
- Smooth physics simulation with gravity

#### Knockback
- Velocity-based system (not teleportation)
- Ground detection prevents floating
- Friction applied when landing
- 3-second maximum effect duration

---

## UI/UX Design

### HUD Elements

#### Player Health Bar
- **Position**: Bottom center
- **Style**: Red bar with black outline
- **Text**: "Health: Current/Max"

#### Enemy Info Panels
- **Display**: Floating above enemy heads
- **Contents**:
  - Enemy type name
  - Health bar with values
  - Attack/Defense stats
  - Active tags
  - States and durations
- **Style**: Text with shadows, no backgrounds

#### Damage Numbers
- **Direct Damage**: Red, floating upward
- **Echo Damage**: Blue "ECHO XX"
- **Size**: 1.5x-3.5x scaling based on damage
- **Duration**: 2 seconds fade

#### Combat State Indicator
- **Position**: Top center
- **States**: "IN COMBAT" (red) / "NON-COMBAT" (green)

#### Weapon Info
- **Position**: Bottom center above health
- **Shows**: Current weapon, ability cooldowns
- **Cooldown**: Red when on cooldown, green when ready

### Visual Effects

#### Attack Indicators
- Growing lines from attacker to target
- Color transitions for timing windows
- Sphere effects for perfect timing

#### Debug Visuals
- Damage calculation logs
- Physics trajectory previews
- Attack range indicators

---

## Technical Implementation

### Architecture
- **Language**: C++ (minimal Blueprints)
- **Pattern**: Component-based
- **Subsystems**: Stats management, Combat state

### Component Structure

#### Core Components
- `HealthComponent`: Resource management
- `TagComponent`: Tag system
- `StateComponent`: State management
- `CombatComponent`: Attack execution

#### Weapon System
- `WeaponBase`: Abstract base class
- `OverrideKatana`: Katana implementation
- `DivineAnchor`: Anchor implementation

#### Enemy System
- `EnemyBase`: Base enemy class
- Derived classes for each enemy type
- Modular stat configuration

### Performance Optimizations
- Component caching
- Efficient collision detection
- Optimized UI updates
- Physics effect pooling

---

## Stats Management System

### Overview
Centralized system for managing all gameplay values through the editor without code recompilation.

### Data Asset Types

#### Character Stats Asset
```cpp
UTrinityFlowCharacterStats
├── Character Info (Name, ID)
├── Base Stats (Health, Attack, Defense)
├── Combat Properties (Speed, Range, Area)
├── AI Properties (Sight Range)
└── Tags (Bitmask selection)
```

#### Weapon Stats Asset
```cpp
UTrinityFlowWeaponStats
├── Weapon Info (Name, ID)
├── Basic Attack (Range, Speed, Type)
├── Katana Stats (Echo %, Cooldowns, Timings)
└── Anchor Stats (Forces, Durations, Ranges)
```

### Stats Subsystem
- **Type**: Game Instance Subsystem
- **Features**:
  - Persistent across levels
  - Runtime reloading
  - Cached access
  - Fallback values

### Tag Data Table
| Column | Type | Description |
|--------|------|-------------|
| TagName | FName | Internal identifier |
| DisplayName | FString | UI display text |
| EffectType | Enum | Gameplay effect |
| Parameters | Various | Effect-specific values |

---

## Setup & Configuration

### Project Setup
1. Clone repository
2. Generate VS project files
3. Open in Unreal Engine 5.5
4. Build C++ code

### Stats Configuration

#### 1. Create Game Instance
```
Content → Blueprint Class → GameInstance
Name: BP_TrinityFlowGameInstance
Project Settings → Maps & Modes → Set Game Instance
```

#### 2. Create Character Stats
```
Content → Data Asset → TrinityFlowCharacterStats
Configure: Health, Attack, Defense, Tags
```

#### 3. Create Weapon Stats
```
Content → Data Asset → TrinityFlowWeaponStats
Configure: Ranges, Cooldowns, Forces
```

#### 4. Create Tag Data Table
```
Content → Data Table → FCharacterTagData
Add rows for each tag with effects
```

#### 5. Configure Subsystem
In Game Instance Blueprint:
- Set default stat assets
- Optional: Set data tables
- Configure reload settings

### Controls
| Action | Key | Context |
|--------|-----|---------|
| Move | WASD | Always |
| Look | Mouse | Always |
| Attack | LMB | Always |
| Ability Q | Q | Always |
| Ability E | E | Always |
| Switch Weapon | Tab | Always |
| Jump | Space | Non-Combat |
| Defensive | Space | Combat |

### Testing Features
- Runtime stat reloading
- Debug visualization toggles
- Damage number customization
- Physics parameter tweaking

---

## Design Considerations

### Balance Philosophy
- Soul damage counters high defense
- Physical damage better for low defense
- Weapon switching rewards adaptability
- Defensive abilities reward timing

### Future Expansion
- Additional weapon types
- New enemy variants
- Combo system
- Difficulty modes via stats

### Known Limitations
- No multiplayer support
- Limited animation system
- Basic AI behavior
- No save system

---

*Last Updated: 2025-07-23*  
*Version: 1.0*