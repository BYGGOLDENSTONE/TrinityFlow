# TrinityFlow

A third-person action combat game built with Unreal Engine 5.5, featuring a unique dual-damage system and physics-based combat.

## Game Overview

TrinityFlow is a fast-paced action game where players wield two distinct weapons - the Override Katana and Divine Anchor - each with unique abilities and playstyles. The game features a revolutionary dual-damage system where physical and soul damage interact differently with enemy defenses.

## Core Features

### Dual-Damage System
- **Physical Damage**: Reduced by defense points, blocked by shields
- **Soul Damage**: Bypasses all defenses with 2x damage multiplier

### Two Unique Weapons

#### Override Katana
- Fast, precise strikes with soul damage abilities
- **Echoes of Data**: Mark enemies to receive echo damage (75% of damage dealt to others)
- **Code Break**: Enhanced soul damage slash
- **Scripted Dodge**: Perfect timing resets cooldowns

#### Divine Anchor
- Heavy weapon with crowd control and physics-based abilities
- **Smash**: Area damage with knockback
- **Gravity Pull**: Physics-based enemy pulling with arc trajectory
- **Holy Gravity**: Large AoE that makes all enemies vulnerable
- **Order**: Counter that strips shields/armor

### Enemy Variety
- **Standard Enemy**: Basic melee attacker
- **Tank Enemy**: High health, armored, area damage
- **Shielded Tank**: Frontal damage immunity
- **Phase Enemy**: Ghost type, immune to physical damage
- **Shielded Tank Robot**: Mechanical, immune to soul damage

### Tag System
- **Shielded**: Blocks frontal physical damage
- **Armored**: Reduces physical damage by defense percentage
- **Ghost**: Immune to physical damage
- **Mechanical**: Immune to soul damage
- **HaveSoul**: Can receive soul damage

## Technical Features

### Stats Management System (NEW)
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
│       ├── Enemy/          # Enemy types
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

3. **Configure Stats System**
   - Create a Game Instance Blueprint
   - Set up Data Assets for characters and weapons
   - Configure the Stats Subsystem
   - See [STATS_SYSTEM_GUIDE.md](STATS_SYSTEM_GUIDE.md) for details

4. **Build and Run**
   - Build the project in Visual Studio
   - Run from Unreal Editor

## Controls

- **Movement**: WASD
- **Camera**: Mouse
- **Attack**: Left Mouse Button
- **Ability Q**: Q key
- **Ability E**: E key
- **Switch Weapon**: Tab
- **Defensive Ability**: Space (in combat)
- **Jump**: Space (out of combat)

## Documentation

- [Game Design Document](GDD.txt) - Original design specifications
- [Additions to GDD](ADDITIONS_TO_GDD.md) - Features added beyond original design
- [Stats System Guide](STATS_SYSTEM_GUIDE.md) - How to use the stats management system

## Recent Updates

- **Stats Management System**: All gameplay values now editable in editor
- **Data-Driven Tags**: Configure tags via data tables
- **Combat Visual Improvements**: Better damage feedback and enemy info display
- **Physics Fixes**: Improved gravity pull and knockback mechanics

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