# TrinityFlow

A third-person action combat game built with Unreal Engine 5.5, featuring a unique dual-damage system and physics-based combat.

## Game Overview

TrinityFlow is a fast-paced action game where players wield two distinct weapons - the Override Katana and Divine Anchor - each with unique abilities and playstyles. The game features a revolutionary dual-damage system where physical and soul damage interact differently with enemy defenses.

## Core Features

### Dual-Damage System
- **Physical Damage**: Reduced by defense points, blocked by shields
- **Soul Damage**: Bypasses all defenses with 2x damage multiplier

### Dual-Wielding Combat System

#### Left Katana (Soul Damage)
- Fast, precise strikes that bypass all defenses
- **Code Break (Q)**: Enhanced soul damage slash
- **Echoes of Data (Tab)**: Mark enemies to receive echo damage (75% of damage dealt to others)
- **Scripted Dodge (Shift)**: Perfect timing resets cooldowns

#### Right Katana (Physical Damage)
- Traditional physical attacks affected by armor and shields
- **Ability 1 (E)**: TBD - Physical damage ability
- **Ability 2 (R)**: TBD - Physical damage ability
- **Order (Space)**: Defensive counter from Divine Anchor heritage

### Enemy Variety
- **Standard Enemy**: Basic melee attacker
- **Tank Enemy**: High health, armored, area damage
- **Shielded Tank**: Frontal damage immunity
- **Phase Enemy**: Ghost type, immune to physical damage
- **Shielded Tank Robot**: Mechanical, immune to soul damage

### Enemy AI System
- **State Machine Architecture**: Pure C++ implementation with modular state design
- **Intelligent Behavior**: Line-of-sight detection, chase with pathfinding, melee attacks
- **Character-Based Movement**: Enemies use ACharacter for proper navigation and physics
- **Navigation Integration**: Full NavMesh pathfinding with CharacterMovementComponent
- **Configurable Parameters**: Sight range, attack range, movement speed via data assets

### Enemy Animation System
- **Hit Response Differentiation**: Separate animations for left (soul) and right (physical) weapon hits
- **Attack Animations**: Synchronized with AI attack timing (1.5 second cast)
- **Parry Response**: Special animation when player perfectly parries enemy attacks
- **Modular Component**: EnemyAnimationComponent manages all combat animations
- **Blueprint Integration**: Easy montage assignment through enemy Blueprints

### Tag System
- **Shielded**: Blocks frontal physical damage
- **Armored**: Reduces physical damage by defense percentage
- **Ghost**: Immune to physical damage
- **Mechanical**: Immune to soul damage
- **HaveSoul**: Can receive soul damage
- **Duelist**: Parry and riposte abilities (NEW)
- **Void**: Boss tag, requires alternative damage sources (NEW)
- **Mimic**: Copies player's last used ability (NEW)
- **Berserk**: Damage and speed boost at low HP (NEW)
- **Ethereal**: Phase shifts with vulnerability changes (NEW)

### Shard System (NEW)
- **Collectible Shards**: Soul and Power shards scattered throughout levels
- **Shard Activation**: Altars convert inactive shards to active ones
- **Dynamic Stances**: Active shard balance determines combat stance
  - Power Stance: More Power shards (doubles physical shard bonus)
  - Soul Stance: More Soul shards (doubles soul shard bonus)
  - Balanced Stance: Equal shards (no stance bonus)
- **Damage Bonuses**: Each active shard grants 3% damage bonus
  - Stance bonus: Doubles the shard bonus when in matching stance
  - Clear UI display shows shard and stance bonuses separately
- **Guardian Challenges**: Some altars require defeating guardians
- **Activation Puzzles**: Hold-to-activate, timing, and pattern matching

## Technical Features

### Animation System (Updated)
- **Comprehensive Animation Component**: Manages all character animations
- **Combat Montages**: 
  - Attack: Right Slash, Left Slash
  - Defense: Hit Response, Left/Right Parry (perfect), Left/Right Block (moderate)
  - Utility: Interaction, Wondering
- **Defensive System**: 
  - Space key for all defensive abilities (stance-dependent)
  - Perfect timing: No damage taken
  - Moderate timing: 50% damage reduction
  - Failed timing: Full damage
  - Visual feedback shows defense result above enemy
- **Wondering System**: Automatic look-around animation after 5 seconds idle
- **Configurable Timing Windows**: Adjustable perfect/moderate defense windows

### Stats Management System
- Centralized configuration for all gameplay values
- Edit stats directly in Unreal Editor without recompiling
- Data Asset-based system for characters and weapons
- Runtime tweaking and reloading support

### Enhanced Combat Visuals
- **Color-Themed UI**: 
  - Blue theme for soul/left-related elements
  - Orange theme for physical/right-related elements
- **Damage Numbers**: Blue for soul damage, orange for physical damage
- **Defense Feedback**: "PERFECT DEFENSE!", "BLOCKED!", or "FAILED!" text above enemies
- **Enemy Info Panels**: Health, stats, and tags displayed above enemies
- **Shard Bonus Display**: Real-time damage bonus breakdown in HUD
- No UI backgrounds for cleaner visuals

### Physics-Based Combat
- Realistic gravity and knockback systems
- Ground detection prevents floating enemies
- Smooth arc trajectories for pulling abilities
- Momentum-based movement

## Project Structure

```
TrinityFlow/
├── Source/
│   └── TrinityFlow/
│       ├── Core/           # Core systems (health, combat, stats)
│       ├── Combat/         # Weapon base classes
│       ├── Player/         # Player character and weapons
│       ├── Enemy/          # Enemy types (now Character-based)
│       ├── AI/             # AI state machine and controllers
│       ├── HUD/            # UI and HUD
│       └── Data/           # Stats and configuration
├── Content/
│   ├── blueprints/         # Blueprint implementations
│   ├── Characters/         # Character assets
│   └── StarterContent/     # Basic assets
├── Config/                 # Project configuration
└── Documentation/          # Game design documents
```

## Setup Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/BYGGOLDENSTONE/TrinityFlow.git
   ```

2. **Open in Unreal Engine 5.5**
   - Right-click on `TrinityFlow.uproject`
   - Select "Generate Visual Studio project files"
   - Open the generated `.sln` file

3. **Configure Combat System**
   - See [Combat System Setup](COMBAT_SYSTEM_SETUP.md) for complete setup guide
   - Create weapon Blueprints and stats data assets
   - Configure Game Instance with weapon stats
   - Set up input actions and sockets

4. **Build and Run**
   - Build the project in Visual Studio
   - Run from Unreal Editor

## Controls

- **Movement**: WASD
- **Camera**: Mouse
- **Left Katana Attack**: Left Mouse Button (Soul damage)
- **Right Katana Attack**: Right Mouse Button (Physical damage)
- **Ability Q**: Q key (Code Break - Left katana)
- **Ability E**: E key (Right katana ability / Interaction)
- **Ability Tab**: Tab key (Echoes of Data - Left katana)
- **Ability R**: R key (Right katana ability)
- **Defensive Ability**: Space (Stance-dependent defense / Jump out of combat)
  - Soul Stance: Soul-themed defense
  - Power Stance: Physical-themed defense
  - Balanced Stance: Power stance animations (temporary)
- **Pause Menu**: Escape key
- **Menu Navigation**: W/S keys or Up/Down arrows
- **Menu Select**: E key or Enter

## Documentation

- [Game Design Document](GDD.md) - Complete game design specifications
- [Combat System Setup](COMBAT_SYSTEM_SETUP.md) - Complete guide for setting up combat, weapons, and animations
- [Shard System Setup](SHARD_SYSTEM_SETUP.md) - Guide for shard collection and altar activation system
- [Implementation Plan](IMPLEMENTATION_PLAN.md) - Detailed technical roadmap
- [Changelog](CHANGELOG.md) - Detailed update history

## Recent Updates

### Slate UI System Implementation (2025-08-01)
- **Complete UI System Overhaul**: Migrated from Canvas to Slate framework
  - Created centralized UI Manager subsystem for state management
  - Implemented main menu with fade-in animations
  - Added pause menu with resume/settings/quit options
  - Created in-game HUD with health bar, weapon panels, and player stats
  - Built animated shard altar interaction UI
  - Added floating damage numbers using Slate
- **Menu Navigation System**:
  - Full keyboard support (W/S for navigation, E for activation)
  - Mouse support with proper input mode switching
  - Smooth transitions between UI states
- **Game Flow Improvements**:
  - Main menu appears on game start
  - Pause menu accessible via Escape key
  - Full game restart when returning to main menu
  - Proper input mode management (UI vs Game)
- **Visual Enhancements**:
  - Consistent styling across all UI elements
  - Keyboard selection highlighting
  - Animation support throughout

### Defensive System & UI Overhaul (2025-07-31)
- **Unified Defensive System**: All defensive abilities now use Space key
  - Stance determines which defensive animation plays
  - Perfect timing window: No damage taken
  - Moderate timing window: 50% damage reduction
  - Failed defense: Full damage taken
- **Visual Feedback System**: 
  - Defense results displayed as floating text
  - Color-coded damage numbers (blue for soul, orange for physical)
- **Shard Damage Bonus System**:
  - 3% damage bonus per active shard
  - Stance doubles matching shard bonuses
  - Real-time bonus display in HUD
- **UI Color Theming**: 
  - Consistent blue/orange color scheme throughout
  - Clear visual distinction between damage types
- **Altar Activation Fix**: Resolved input conflict crash

### Shard System Implementation (2025-07-30)
- **ShardComponent**: Manages shard collection and activation
  - Tracks active/inactive shard counts
  - Automatic stance updates based on active shards
- **ShardPickup Actor**: Collectible shards with floating animation
  - Configurable respawn system for testing
  - Blueprint events for visual effects
- **ShardAltar Actor**: Shard activation locations
  - Guardian enemy support
  - Multiple puzzle types (instant, hold-to-activate, pattern matching)
  - Progress tracking and UI events
- **Character Integration**: ShardComponent integrated with player character

### Stance System Foundation (2025-07-30)
- **StanceComponent**: New component for managing player combat stances
  - Power Stance: +15% physical damage
  - Soul Stance: +15% soul damage (future)
  - Balanced Stance: +10% to both damage types (future)
- **Extended Tag System**: Added Duelist, Void, Mimic, Berserk, and Ethereal tags
- **Weapon Blueprint Integration**: Fixed katana visibility and attachment issues
- **Collision Fixes**: Resolved character physics glitches from weapon collisions

### Animation & Combat System Overhaul
- **Dual-Wielding System**: Implemented left (soul damage) and right (physical damage) katana system
- **New Combo System**: Dynamic attack chains with wait animations
  - Normal combo: Attack → Wait animation → Second attack
  - Spam combo: Rapid attacks trigger special combo animation
  - Movement cancels combo chains
- **AnimationComponent**: Centralized animation management
  - All montages organized in one component
  - Configurable timing for combos
  - Smooth transitions between states
- **State-Based Attack Flow**: Proper attack state management prevents animation conflicts

### Previous Updates
- **Simplified Animation System**: Direct attack execution with 4 essential montages
- **Enemy AI System**: Pure C++ state machine implementation
- **Stats Management System**: All gameplay values now editable in editor
- **Dual-Wielding Implementation**: Complete katana system overhaul
- **Combat Visual Improvements**: Better damage feedback and enemy info display
- **Physics Fixes**: Improved gravity pull and knockback mechanics

## Current Status

The project is a functional combat prototype focusing on the unique dual-damage system. Key features are implemented and working, but some planned features (right katana abilities, save system) are not yet developed. The codebase prioritizes clean, maintainable C++ over complex systems.

## Development

Built with:
- Unreal Engine 5.5
- C++ (primary implementation)
- Component-based architecture
- Minimal Blueprint usage

## Contributing

This is a personal project, but feel free to fork and experiment!

## License

This project is for educational and portfolio purposes.