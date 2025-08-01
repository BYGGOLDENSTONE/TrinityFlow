# TrinityFlow - Complete Game Design Document

## Game Overview
TrinityFlow is a fast-paced third-person action combat game featuring a revolutionary dual-damage system where players master two distinct combat styles through strategic weapon usage and dynamic stance management.

## Core Gameplay Loop
1. **Explore** environments to find shards and enemy encounters
2. **Combat** enemies using dual-katana system with stance management
3. **Collect** shards to enhance damage output
4. **Activate** shards at altars for permanent bonuses
5. **Progress** through increasingly challenging encounters

## Combat System

### Dual-Damage Mechanics
- **Soul Damage (Blue)**: Bypasses all defenses, 2x damage multiplier
- **Physical Damage (Orange)**: Affected by armor/shields, standard damage

### Weapon System
**Left Katana - Override (Soul)**
- Primary: Fast soul damage strikes (LMB)
- Q - Code Break: Enhanced soul slash
- Tab - Echoes of Data: Mark enemies (75% echo damage)
- Shift - Scripted Dodge: Perfect timing resets cooldowns

**Right Katana - Physical**
- Primary: Heavy physical strikes (RMB)
- E - TBD Physical ability
- R - TBD Physical ability
- Space - Order: Defensive counter

### Stance Flow System
Dynamic combat stances based on attack patterns:
- **Soul Stance (0-20%)**: +15% soul damage
- **Balanced Stance (40-60%)**: +10% all damage, requires alternating attacks
- **Power Stance (80-100%)**: +15% physical damage
- Consecutive attacks accelerate stance changes (up to 3x speed)
- Flow resets outside combat

### Defensive Mechanics
- **Timing Windows**: Perfect (no damage), Moderate (50% reduction), Failed (full damage)
- **Visual Feedback**: Timing bar above attacking enemy
- **Flexibility**: Can activate anytime, not restricted to windows
- **Stance-Based**: Different animations per stance

## Progression Systems

### Shard System
- **Collection**: Find Soul/Power shards in levels
- **Activation**: Use altars to convert inactive to active
- **Benefits**: 3% damage bonus per active shard
- **Strategy**: Balance soul vs physical damage bonuses

### Damage Calculation
```
Base Damage × (1 + Shard Bonus) × (1 + Stance Bonus) × Enemy Defense
```

## Enemy Design

### Base Types
1. **Standard**: Basic melee, balanced stats
2. **Tank**: High health, area attacks, slow
3. **Phase**: Ghost type, immune to physical
4. **Shielded**: Frontal damage immunity
5. **Mechanical**: Robot type, immune to soul

### Enemy Tags System
- **Armored**: Reduces physical damage by %
- **Ghost**: Physical immunity
- **Mechanical**: Soul immunity
- **Shielded**: Blocks frontal damage
- **HaveSoul**: Can receive soul damage
- **Duelist**: Parry/riposte abilities
- **Berserk**: Damage boost at low HP
- **Ethereal**: Phase shift mechanics

### AI Behavior
- **Detection**: Line-of-sight based
- **States**: Idle → Alert → Chase → Attack → Return
- **Combat**: 1.5s attack telegraph with defensive window
- **Navigation**: Full pathfinding with obstacle avoidance

## UI/UX Systems

### HUD Elements
- **Health Bar**: Bottom left with numerical display
- **Weapon Panels**: Center bottom showing abilities/cooldowns
- **Stance Flow Bar**: Above weapons, shows current position
- **Combat State**: Top center indicator
- **Damage Numbers**: Floating combat feedback (blue/orange)
- **Enemy Info**: Health bars above enemies

### Menu Systems
- **Main Menu**: Start, Settings, Quit
- **Pause Menu**: Resume, Settings, Main Menu, Quit
- **Altar Interface**: Shard selection with damage preview
- **Full keyboard/gamepad navigation**

## Visual Design
- **Color Coding**: Blue (Soul), Orange (Physical), Purple (Balanced)
- **Combat Effects**: Camera shake, directional bump on damage
- **Stance Indicators**: Zone highlighting, pulsing effects
- **Minimalist UI**: No unnecessary backgrounds

## Audio Design (Planned)
- **Dynamic Music**: Intensity based on combat state
- **Combat SFX**: Distinct sounds for soul/physical hits
- **Feedback**: Audio cues for perfect timing, stance changes

## Level Design Philosophy
- **Arena-Based**: Focus on combat encounters
- **Shard Placement**: Reward exploration
- **Altar Locations**: Strategic checkpoints
- **Enemy Variety**: Mixed types requiring stance adaptation

## Gameplay Objectives
1. **Primary**: Progress through all combat arenas
2. **Secondary**: Collect all shards for maximum power
3. **Mastery**: Perfect defensive timing, optimal stance management
4. **Challenge**: Defeat all enemy types efficiently

## Target Experience
- **Fast-Paced**: Quick decisions, fluid combat
- **Strategic Depth**: Stance management, enemy weaknesses
- **Skill Expression**: Timing, positioning, pattern recognition
- **Progression Feel**: Growing power through shards
- **Replayability**: Different build strategies, perfect runs