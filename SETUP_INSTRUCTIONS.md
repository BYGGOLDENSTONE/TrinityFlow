# TrinityFlow Setup Instructions

## Project Setup

1. **Compile the Project**
   - Open TrinityFlow.uproject in Unreal Engine 5.5
   - When prompted to compile, click "Yes"
   - Wait for compilation to complete

2. **Create a Test Map**
   - In the Content Browser, create a new level (File > New Level > Basic)
   - Save it as "TestArena" in the Maps folder
   - Add a large floor (e.g., a scaled cube or plane) for the arena
   - Add some walls around the edges if desired

3. **Set Up Player Start**
   - Place a Player Start actor in the level (from Place Actors panel)
   - Position it in the center of your arena

4. **Place Enemy Spawners**
   - In the Place Actors panel, search for "Enemy Spawner"
   - Place several spawners around the arena
   - In the Details panel for each spawner:
     - Set "Enemy Type To Spawn" to different enemy types
     - Set "Auto Spawn" to true
     - Adjust "Spawn Delay" as needed

5. **Configure Game Mode**
   - Go to Edit > Project Settings > Maps & Modes
   - Set Default GameMode to "TrinityFlowGameMode"
   - Set Default Map to your TestArena map

## Input Configuration

The game uses Enhanced Input. Default controls:
- **WASD** - Movement
- **Mouse** - Look around
- **Space** - Jump (non-combat) / Defensive ability (combat)
- **Left Mouse Button** - Basic attack
- **Q** - Weapon ability 1
- **E** - Weapon ability 2
- **Tab** - Switch weapon

If input isn't working, you may need to:
1. Create Input Actions in Content Browser
2. Create an Input Mapping Context
3. Assign them to the character blueprint

## Testing the Game

1. **Play in Editor**
   - Click the Play button in the toolbar
   - You should spawn as the player character

2. **Combat Testing**
   - Approach enemies to trigger combat mode
   - The HUD will show "IN COMBAT" when enemies see you
   - Test weapon switching with Tab
   - Try different abilities on different enemy types

3. **HUD Features**
   - Player health bar at bottom center
   - Crosshair dot in center
   - Target info panel on right (when aiming at enemy)
   - Player info panel on left
   - Weapon and cooldown info at bottom
   - Nearby enemies list

## Key Gameplay Mechanics

### Damage Calculation
- **Physical**: `attackpoint * (100 - defencepoint)%`
- **Soul**: `attackpoint * 2` (bypasses all defenses)

### Enemy Types
- **Standard**: Basic enemy with soul damage vulnerability
- **Tank**: Armored enemy with area attacks
- **Shielded Tank**: Frontal shield blocks physical damage
- **Phase**: Ghost enemy immune to physical damage
- **Shielded Tank Robot**: Mechanical enemy immune to soul damage

### Weapon Abilities
**Override Katana:**
- Q: Echoes of Data - Marks enemy for echo damage
- E: Code Break - Soul damage slash

**Divine Anchor:**
- Q: Gravity Pull - Pulls enemy and makes vulnerable
- E: Holy Gravity - Area damage and vulnerability

### Defensive Abilities
- **Scripted Dodge** (Katana): Time the dodge for damage reduction
  - 0-0.75s: Moderate (half damage)
  - 0.75-1.5s: Perfect (no damage, resets Code Break)
- **Order** (Anchor): Counter attack
  - Perfect timing strips shield or reduces armor

## Troubleshooting

1. **Compilation Errors**
   - Regenerate project files (right-click .uproject > Generate Visual Studio project files)
   - Clean and rebuild the project

2. **Missing Assets**
   - The mannequin mesh is referenced but uses default assets
   - You can assign any skeletal mesh to the character

3. **Input Not Working**
   - Check that Enhanced Input plugin is enabled
   - Verify input actions are created and assigned

4. **Enemies Not Spawning**
   - Check spawner settings in level
   - Ensure spawn location is not blocked

## Notes
- All visual feedback uses debug lines/shapes for prototyping
- No animations are implemented - attacks show as growing trace lines
- The system is designed for quick testing and iteration