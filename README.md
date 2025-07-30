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

## Technical Features

### Animation System (Updated)
- **Simplified Animation Component**: Direct attack execution without complex buffering
- **4 Essential Montages**: Right Slash, Left Slash, Interaction, Wondering
- **Wondering System**: Automatic look-around animation after 5 seconds idle
- **Animation-Driven Timing**: Attack duration matches actual animation length
- **Configurable Speed Scale**: Compensate for blueprint animation speed issues

### Stats Management System
- Centralized configuration for all gameplay values
- Edit stats directly in Unreal Editor without recompiling
- Data Asset-based system for characters and weapons
- Runtime tweaking and reloading support

### Enhanced Combat Visuals
- Floating damage numbers (red for direct, blue for echo)
- Enemy info panels above heads showing health, stats, and tags
- Color-coded attack timing windows for defensive abilities
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

3. **Configure Weapon System**
   - Create Blueprint classes: BP_OverrideKatana and BP_PhysicalKatana
   - Add weapon meshes to the WeaponMesh component
   - In Character Blueprint, set weapon class references
   - Configure socket names (default: "weapon_l" and "weapon_r")

4. **Configure Stats System**
   - Create a Game Instance Blueprint
   - Set up Data Assets for characters and weapons
   - Configure the Stats Subsystem
   - See [STATS_SYSTEM_GUIDE.md](STATS_SYSTEM_GUIDE.md) for details

5. **Build and Run**
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
- **Left Defensive**: Shift (Scripted Dodge)
- **Right Defensive**: Space (Order / Jump out of combat)

## Documentation

- [Game Design Document](GDD.md) - Complete game design specifications
- [Changelog](CHANGELOG.md) - Detailed update history

## Recent Updates

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