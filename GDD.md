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

### Shard System

#### Shard Types
- **Soul Shards**: Blue shards that enhance soul damage
- **Power Shards**: Orange shards that enhance physical damage

#### Shard Mechanics
- **Collection**: Pick up inactive shards scattered throughout levels
- **Activation**: Use shard altars to convert inactive shards to active ones
- **Damage Bonus**: Each active shard grants 3% damage bonus to its type
- **Stance Bonus**: Double the shard bonus when in matching stance

#### Damage Calculation Example
- 3 active Soul shards = 9% soul damage bonus
- In Soul Stance: 9% base + 9% stance bonus = 18% total soul damage
- 4 active Power shards = 12% physical damage bonus  
- In Power Stance: 12% base + 12% stance bonus = 24% total physical damage

#### Stance Determination
- **Soul Stance**: More Soul shards than Power shards
- **Power Stance**: More Power shards than Soul shards
- **Balanced Stance**: Equal shards (±1 difference)

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

### Stance System

Dynamic combat modifiers based on active shard balance:

| Stance | Condition | Effect | Visual |
|--------|-----------|--------|--------|
| **Power** | Power shards > Soul shards | +15% physical damage | Red aura |
| **Soul** | Soul shards > Power shards | +15% soul damage | Blue aura |
| **Balanced** | Equal or ±1 difference | +10% all damage | Purple aura |

### Shard System

**Collection & Activation**:
- **Shard Types**: Soul (blue) and Power (red)
- **States**: Inactive (collected) → Active (affects stance)
- **Activation**: Use altars to convert inactive to active
- **Max per Activation**: 5 shards

**Altar Mechanics**:
- **Guardian Challenges**: Defeat nearby enemies to unlock
- **Puzzle Types**:
  - Instant: No puzzle required
  - Hold-to-Activate: Channel for 3 seconds
  - Timing Challenge: Hit targets in sequence
  - Pattern Match: Replicate shown patterns

---

## Player Character

### Base Stats
- **Health**: 200
- **Attack**: 20  
- **Defense**: 20
- **Tags**: HaveSoul, Armored

### Movement
- **Combat**: No jumping, defensive abilities on Shift/Space
- **Non-Combat**: Standard movement + jumping + interaction
- **Attack Coordination**: Only one katana can attack at a time

---

## Weapons & Abilities

### Dual-Wielding Katana System
*Player wields two katanas simultaneously with coordinated attacks*

### Left Katana (Override Katana)
*Fast, precise weapon with soul damage focus*

#### Basic Attack - Soul Slash (LMB)
- **Damage**: Soul
- **Range**: 300 units
- **Speed**: 1.0 attacks/second
- **Type**: Single target
- **Coordination**: Cannot attack while right katana is attacking

#### Q - Code Break
- **Damage**: Soul
- **Range**: 600 units
- **Cooldown**: 6 seconds
- **Effect**: Enhanced slash dealing soul damage

#### Tab - Echoes of Data
- **Damage**: Soul (to marked target)
- **Range**: 4000 units
- **Cooldown**: 5 seconds
- **Duration**: 5 seconds mark
- **Effect**: Marked enemy receives 75% of damage dealt to others as soul damage
- **AoE Rule**: 30% damage for area attacks

#### Space - Unified Defensive Ability
- **Stance-Based**: Different animations/effects based on current stance
  - **Soul Stance**: Scripted Dodge (left-hand animation)
  - **Power Stance**: Order (right-hand animation)
  - **Balanced Stance**: Uses Power stance animation (temporary)
- **Timing Windows** (configurable in Blueprint):
  - **Perfect (0-0.3s)**: 100% damage reduction + parry animation
  - **Moderate (0.3-1.0s)**: 50% damage reduction + block animation
  - **Failed (>1.0s)**: 0% damage reduction + hit response animation
- **Visual Feedback**: Shows result text above attacker
  - "PERFECT DEFENSE!" (Green)
  - "BLOCKED!" (Yellow)
  - "FAILED!" (Red)

### Right Katana (Physical Katana)
*Powerful physical damage weapon with defensive capabilities*

#### Basic Attack - Physical Slash (RMB)
- **Damage**: Physical
- **Range**: 350 units
- **Speed**: 0.8 attacks/second
- **Type**: Single target
- **Coordination**: Cannot attack while left katana is attacking

#### E - (To Be Implemented)
- **Damage**: Physical
- **Range**: 400 units
- **Cooldown**: 6 seconds
- **Effect**: TBD

#### R - (To Be Implemented)
- **Damage**: Physical
- **Range**: 600 units
- **Cooldown**: 8 seconds
- **Effect**: TBD

#### Space - Order (Deprecated)
- See "Space - Unified Defensive Ability" under Left Katana section
- Order is now activated when in Power Stance
- **Non-Combat**: Functions as jump

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

### Color Theming

#### Left/Soul Elements (Blue Theme #0080FF)
- Soul damage numbers
- Left katana (Override Katana) effects
- Soul shard pickups and UI
- Soul altars and activation effects
- Soul damage bonus text in HUD
- Echoes of Data ability effects

#### Right/Physical Elements (Orange Theme #FF8000)
- Physical damage numbers
- Right katana (Physical Katana) effects
- Power shard pickups and UI
- Power altars and activation effects
- Physical damage bonus text in HUD
- Order defensive ability effects

#### Defensive Result Colors
- **Perfect Defense**: Green
- **Moderate Defense (Blocked)**: Yellow
- **Failed Defense**: Red

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

#### Color Theming
- **Soul/Left Elements**: Blue theme (#0080FF)
  - Soul damage numbers
  - Left katana abilities
  - Soul shard UI elements
  - Soul altar interactions
- **Physical/Right Elements**: Orange theme (#FF8000)
  - Physical damage numbers
  - Right katana abilities
  - Power shard UI elements
  - Power altar interactions
- **Neutral Elements**: White/Gray
  - Defense result text
  - General UI elements

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

#### 1. Create Game Instance Blueprint
```
1. Right-click in Content Browser → Blueprint Class
2. Select UTrinityFlowGameInstance as parent class
3. Name it BP_TrinityFlowGameInstance
4. Open the blueprint and set:
   - Default Player Stats (required)
   - Default Katana Stats (required)
   - Default Anchor Stats (required)
   - Character Stats Table (optional)
   - Weapon Stats Table (optional)
5. Project Settings → Maps & Modes → Game Instance Class → BP_TrinityFlowGameInstance
```

#### 2. Create Character Stats Data Assets
```
1. Right-click → Miscellaneous → Data Asset → UTrinityFlowCharacterStats
2. For Player:
   - CharacterID: "Player"
   - MaxHealth: 200
   - AttackPoint: 20
   - DefencePoint: 20
   - CharacterTags: HaveSoul + Armored
3. For Enemies (create one for each):
   - StandardEnemy: 100 HP, 10 ATK, 0 DEF
   - TankEnemy: 200 HP, 40 ATK, 20 DEF, Armored tag
   - ShieldedTankEnemy: 300 HP, 20 ATK, 20 DEF, Shielded tag
   - PhaseEnemy: 100 HP, 20 ATK, 0 DEF, Ghost tag
   - ShieldedTankRobotEnemy: 300 HP, 30 ATK, 40 DEF, Mechanical + Shielded tags
```

#### 3. Create Weapon Stats Data Assets
```
1. Right-click → Miscellaneous → Data Asset → UTrinityFlowWeaponStats
2. For Override Katana:
   - WeaponID: "OverrideKatana"
   - Configure Katana ability stats
3. For Divine Anchor:
   - WeaponID: "DivineAnchor"
   - Configure Anchor ability stats
```

#### 4. (Optional) Create Data Tables
```
1. Character Stats Table:
   - Row Structure: FCharacterStatsTableRow
   - Add rows referencing your character stat assets
2. Weapon Stats Table:
   - Row Structure: FWeaponStatsTableRow
   - Add rows referencing your weapon stat assets
3. Tag Data Table:
   - Row Structure: FCharacterTagData
   - Define tag display names and effects
```

#### 5. Tag System Notes
- Tags use bitmask values (can combine multiple tags)
- Tag values: Shielded=1, Armored=2, Ghost=4, Mechanical=8, HaveSoul=16
- Fixed editor display issues with proper enum metadata
- Tags now correctly save and load from data assets

### Controls
| Action | Key | Context |
|--------|-----|---------|
| Move | WASD | Always |
| Look | Mouse | Always |
| Left Katana Attack | LMB | Always |
| Right Katana Attack | RMB | Always |
| Left Ability 1 (Code Break) | Q | Always |
| Left Ability 2 (Echoes) | Tab | Always |
| Right Ability 1 | E | Combat |
| Interact | E | Non-Combat |
| Right Ability 2 | R | Always |
| Left Defensive (Dodge) | Shift | Combat |
| Right Defensive (Order) | Space | Combat |
| Jump | Space | Non-Combat |

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
- Animation system pending implementation
- No save system

---

## Enemy AI System

### Overview
The enemy AI system uses a state machine architecture implemented in pure C++ for optimal performance. Each enemy can transition between different behavioral states based on player proximity and combat conditions.

### Core Components

#### 1. AI State Machine (`UAIStateMachine`)
- Component-based system that manages state transitions
- Tick-based state updates
- Blueprint-compatible for easy configuration
- Automatically finds and uses the enemy's AI controller

#### 2. Base AI State (`UAIState`)
- Abstract base class for all AI states
- Implements Enter/Update/Exit pattern
- Handles state transitions through `TransitionToState()`
- Caches enemy, controller, and state machine references

#### 3. Enemy AI Controller (`AEnemyAIController`)
- Extends AAIController with CrowdFollowingComponent
- Manages pathfinding and movement requests
- Integrates with Unreal's navigation system
- Automatically possesses enemy pawns

#### 4. Enemy Base Class Updates
- Changed from APawn to ACharacter for better movement
- Uses built-in CharacterMovementComponent
- Automatic integration with navigation mesh
- Better physics and collision handling

### Implemented States

#### Idle State (`UAIState_Idle`)
- Default state when no threats detected
- Scans for players within sight range (configurable)
- Uses line-of-sight checks to detect valid targets
- Transitions to Chase when player detected

#### Chase State (`UAIState_Chase`)
- Actively pursues detected player
- Uses navigation mesh for pathfinding with MoveToActor
- Updates path periodically (configurable interval)
- Checks for line-of-sight to maintain pursuit
- Transitions to Attack when in range, or Idle if target lost
- Handles stuck detection with velocity monitoring

#### Attack State (`UAIState_Attack`)
- Executes attacks when player is within attack range
- Manages attack cooldowns through CombatComponent
- Faces target during attacks
- Returns to Chase if target moves out of range

### Setup Instructions

#### 1. Create AI State Blueprints
```
1. Right-click in Content Browser → Blueprint Class
2. Create blueprints for each state:
   - Parent: UAIState_Idle → Name: BP_AIState_Idle
   - Parent: UAIState_Chase → Name: BP_AIState_Chase
   - Parent: UAIState_Attack → Name: BP_AIState_Attack
```

#### 2. Configure State Transitions
```
1. Open BP_AIState_Idle:
   - Set Chase State Class = BP_AIState_Chase

2. Open BP_AIState_Chase:
   - Set Attack State Class = BP_AIState_Attack
   - Set Idle State Class = BP_AIState_Idle

3. Open BP_AIState_Attack:
   - Set Chase State Class = BP_AIState_Chase
   - Set Idle State Class = BP_AIState_Idle
```

#### 3. Configure Enemy Blueprints
```
1. Open each enemy blueprint (BP_StandardEnemy, etc.)
2. In Details panel:
   - Initial State Class = BP_AIState_Idle
   - Movement Speed = 300 (or as desired)
3. Compile and save
```

#### 4. Place Navigation Mesh
```
1. In level editor: Place Actors → Volumes → Nav Mesh Bounds Volume
2. Scale to cover all walkable areas
3. Press 'P' to visualize navigation mesh (should show green overlay)
```

### Enemy Stats Configuration

Enemy stats control AI behavior parameters:
- **SightRange**: Detection distance for players (default 1500)
- **AttackRange**: Distance to initiate attacks (default 300)
- **MovementSpeed**: Chase movement speed (default 300)

Configure these in the character stats data assets.

### Future AI Enhancements
- Patrol waypoint system
- Group coordination
- Advanced attack patterns
- Environmental awareness
- Fleeing/retreat behavior

---

## Enemy Animation System

### Overview
The enemy animation system provides visual feedback for all combat interactions, differentiating between damage sources and defensive outcomes.

### Core Components

#### EnemyAnimationComponent
- Manages all enemy combat animations
- Automatically added to all enemies through EnemyBase
- Handles animation priorities and interruptions
- Blueprint-configurable montages

### Animation Types

#### 1. Hit Response Animations
- **Left Hit Response**: Plays when hit by left weapon (soul damage)
- **Right Hit Response**: Plays when hit by right weapon (physical damage)
- Differentiated by weapon source, not just damage type
- Automatically interrupts other animations

#### 2. Attack Animation
- Synchronized with AI attack state
- Plays during 1.5 second attack cast time
- Visual indicator of incoming damage

#### 3. Parry Response Animation
- Triggered when player achieves perfect defense
- Special stagger animation showing failed attack
- Longer duration for dramatic effect

### Setup Instructions

#### 1. Create Animation Montages
```
For each enemy skeletal mesh:
1. Create 4 animation montages:
   - LeftHitResponseMontage (soul damage reaction)
   - RightHitResponseMontage (physical damage reaction)
   - AttackMontage (attack wind-up and strike)
   - ParryResponseMontage (stagger from perfect parry)
```

#### 2. Configure Enemy Blueprints
```
1. Open enemy Blueprint (BP_StandardEnemy, etc.)
2. Select AnimationComponent in Components panel
3. In Details panel, assign all 4 montages
4. Compile and save
```

### Animation Guidelines
- **Hit Responses**: 0.3-0.5 seconds, directional stagger
- **Attack**: Match 1.5 second cast time, clear wind-up
- **Parry Response**: 1-2 seconds, dramatic recoil
- **Visual Effects**: Blue for soul damage, orange for physical

### Technical Features
- Weapon hand detection (left vs right)
- Automatic animation blending
- Priority system prevents animation conflicts
- Full Blueprint integration

---

## Current Project Status (2025-07-29)

### What's Implemented
- ✅ **Dual-Wielding Combat System**: Left katana (soul damage) and right katana (physical damage)
- ✅ **Animation System**: Simplified with 4 montages (attacks, interaction, wondering)
- ✅ **Enemy AI**: State machine with Idle, Chase, and Attack states
- ✅ **Stats Management**: Data-driven system for all gameplay values
- ✅ **Combat Visuals**: Damage numbers, enemy info panels, attack indicators
- ✅ **Physics Combat**: Gravity pull and knockback systems

### What's NOT Implemented
- ❌ **Right Katana Abilities**: E and R abilities are placeholders
- ❌ **Divine Anchor**: Removed but code references remain
- ❌ **Save System**: No persistence between sessions
- ❌ **Level Progression**: No chapter/level system
- ❌ **Combo Finishers**: Simplified animation system doesn't support complex combos

### Known Issues
- Animation timing can be inconsistent due to blueprint speed scaling
- Some deprecated weapon switching code remains
- Divine Anchor references need cleanup

## Future Development Plans

### Animation System (Next Priority)
- **Combo System Implementation**
  - C++ driven animation montage sequences
  - Light attack combos (LMB): 3-hit soul damage chain
  - Heavy attack combos (RMB): 2-hit physical damage chain
  - Mixed combos: Alternating between weapons
  - Combo timing windows and reset conditions
  
- **Animation Montages**
  - Unique attack animations for each katana
  - Transition animations between combos
  - Ability cast animations
  - Hit reaction animations
  - Defensive ability animations

- **Animation Blueprint Integration**
  - State machine for combat animations
  - Blend spaces for movement
  - Animation notifies for damage timing
  - Root motion support for special attacks

### Combat Enhancements
- **Right Katana Abilities**
  - E Ability: "Rending Strike" - Armor-piercing slash
  - R Ability: "Whirlwind" - 360° area attack
  
- **Combo Finishers**
  - Special animations for combo endings
  - Enhanced damage/effects on finishers
  - Unique visual effects per weapon

- **Combat Feel Improvements**
  - Hit pause/freeze frames
  - Camera shake on impacts
  - Improved particle effects
  - Sound effect integration

### Interactable System
- **Object Types**
  - Treasure chests
  - Upgrade stations
  - Lore tablets
  - Health shrines
  
- **Interaction Framework**
  - Base interactable class
  - Progress bars for timed interactions
  - Interrupt conditions
  - Multiplayer support considerations

### Weapon Upgrade System
- **Upgrade Paths**
  - Damage upgrades
  - Range extensions
  - Cooldown reductions
  - New combo branches
  
- **Visual Progression**
  - Weapon glow/effects based on upgrade level
  - Unique models for max-level weapons

### Performance Optimizations
- Animation LOD system
- Combo prediction for networking
- Effect pooling for particles
- Optimized collision detection

### Technical Debt
- Remove Divine Anchor completely
- Refactor old weapon switching code
- Clean up deprecated methods
- Update unit tests

---

*Last Updated: 2025-07-28*  
*Version: 2.0*