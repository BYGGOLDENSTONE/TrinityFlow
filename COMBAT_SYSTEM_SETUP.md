# Combat System Setup Guide

## Overview
This guide covers the complete setup for the TrinityFlow combat system, including weapons, animations, and input configuration.

## Prerequisites
- Character Blueprint created (BP_TrinityFlowCharacter)
- Enhanced Input system configured
- Character skeletal mesh imported

## Step 1: Input Action Setup

### 1.1 Create Input Actions
In Content Browser, create these Input Action assets:
- `IA_Attack` - Left Mouse Button (LMB)
- `IA_RightAttack` - Right Mouse Button (RMB)
- `IA_AbilityQ` - Q key
- `IA_AbilityE` - E key
- `IA_AbilityTab` - Tab key
- `IA_AbilityR` - R key
- `IA_LeftDefensive` - Shift key
- `IA_RightDefensive` - Space key

### 1.2 Assign in Character Blueprint
1. Open BP_TrinityFlowCharacter
2. In Class Defaults → Input:
   - Attack Action: `IA_Attack`
   - Right Attack Action: `IA_RightAttack`
   - (Assign all other actions)

## Step 2: Weapon Setup

### 2.1 Create Weapon Blueprints
1. **Left Hand - Override Katana**:
   - Blueprint Class → Parent: `OverrideKatana`
   - Name: `BP_OverrideKatana`
   - Set mesh (cylinder for prototype)
   - Scale: (0.05, 0.05, 0.5)
   - Material: Blue/Purple for soul damage

2. **Right Hand - Physical Katana**:
   - Blueprint Class → Parent: `PhysicalKatana`
   - Name: `BP_PhysicalKatana`
   - Set mesh (cylinder for prototype)
   - Scale: (0.05, 0.05, 0.5)
   - Material: Red/Metallic for physical damage

### 2.2 Configure Weapon Sockets
1. Open character skeletal mesh
2. Add sockets to hand bones:
   - `hand_l` bone → Add Socket → Name: `WeaponSocket_L`
   - `hand_r` bone → Add Socket → Name: `WeaponSocket_R`
3. Position sockets in palm center

### 2.3 Assign Weapons in Character
1. Open BP_TrinityFlowCharacter
2. Class Defaults → Weapons:
   - Left Katana Class: `BP_OverrideKatana`
   - Right Katana Class: `BP_PhysicalKatana`

## Step 3: Weapon Stats Configuration (REQUIRED)

### 3.1 Create Stats Data Assets
1. **Override Katana Stats**:
   - Right-click → Miscellaneous → Data Asset
   - Class: `TrinityFlowKatanaStats`
   - Name: `DA_OverrideKatanaStats`
   - Configure:
     - Weapon ID: "OverrideKatana"
     - Basic Damage Type: Soul

2. **Physical Katana Stats**:
   - Right-click → Miscellaneous → Data Asset
   - Class: `TrinityFlowPhysicalKatanaStats`
   - Name: `DA_PhysicalKatanaStats`
   - Configure:
     - Weapon ID: "PhysicalKatana"
     - Basic Damage Type: Physical

### 3.2 Configure Game Instance
1. Create/Open `BP_TrinityFlowGameInstance`
2. Class Defaults → Stats Configuration:
   - Default Left Katana Stats: `DA_OverrideKatanaStats`
   - Default Right Katana Stats: `DA_PhysicalKatanaStats`
3. Project Settings → Maps & Modes → Game Instance Class: `BP_TrinityFlowGameInstance`

## Step 4: Animation Setup (Optional)

### 4.1 Animation Montages
In AnimationComponent of Character Blueprint, assign:
- **Right Slash**: Right hand attack animation
- **Left Slash**: Left hand attack animation
- **Interaction**: E key interaction
- **Wondering**: Idle animation (plays after 5 seconds)

### 4.2 Without Animations
The system works without animations:
- Attacks use 0.5 second default duration
- Weapons still spawn and deal damage
- Console shows debug messages

## Step 5: Testing

### 5.1 Play in Editor
- Press Play (NOT Simulate - it will crash)
- Check console for:
  - "Spawned and attached LeftKatana to socket: WeaponSocket_L"
  - "Spawned and attached RightKatana to socket: WeaponSocket_R"

### 5.2 Test Combat
- LMB: Left katana soul damage attack
- RMB: Right katana physical damage attack
- Q: Code Break ability
- E: Interaction/Ability
- Shift: Scripted Dodge
- Space: Order defense

## Troubleshooting

### "No weapon stats found"
- Ensure Game Instance is set in Project Settings
- Verify stats data assets are created and assigned
- Check weapon IDs match exactly

### Weapons not appearing
- Check weapon Blueprint classes are assigned
- Verify sockets exist on skeletal mesh
- Look for spawn errors in console

### Attacks not working
- Verify Enhanced Input is configured
- Check Input Actions are assigned
- Ensure weapon stats are loaded

## Important Notes
- Do NOT use Simulate mode in Blueprint viewport
- Weapons only spawn during gameplay (PIE)
- Stats system requires Game Instance configuration
- Socket names are customizable in Character Blueprint