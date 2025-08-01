# Controller Support Guide

## Overview
TrinityFlow now has full controller support for all combat and UI interactions. The control scheme is designed to feel intuitive for dual-katana combat with separate controls for each weapon.

## Controller Button Mapping

### Combat Controls

#### Override Katana (Left Hand - Soul Damage)
- **L1**: Basic left attack
- **L2**: Q ability (Code Break / Data Breach)
- **Square**: Tab ability (Echoes of Data / Soul Link)

#### Physical Katana (Right Hand - Physical Damage)
- **R1**: Basic right attack
- **R2**: E ability (Duelist's Honor)
- **Circle**: R ability (Execution Strike)

#### Defensive Controls
- **X**: Global defensive ability (stance-based)
  - Soul Stance: Scripted Dodge
  - Power Stance: Order
  - Balanced Stance: Flow Guard (60% damage reduction for 1.5s)

### General Controls
- **Left Stick**: Movement
- **Right Stick**: Camera look
- **Triangle**: Interaction / Confirm in menus
- **Options**: Pause menu

### Shard Altar UI Controls
When interacting with a shard altar:
- **D-Pad Left/Right**: Switch between Soul/Power shard selection
- **D-Pad Up/Down**: Increase/Decrease shard activation amount
- **Triangle**: Confirm and activate selected shards
- **Circle**: Cancel and close UI

## Setup Instructions

### 1. Create Input Actions in Blueprint
In the Content Browser, create the following Input Action assets:
- `IA_Controller_L1`
- `IA_Controller_L2`
- `IA_Controller_Square`
- `IA_Controller_R1`
- `IA_Controller_R2`
- `IA_Controller_Circle`
- `IA_Controller_X`
- `IA_Controller_Triangle`
- `IA_Controller_DPad_Up`
- `IA_Controller_DPad_Down`
- `IA_Controller_DPad_Left`
- `IA_Controller_DPad_Right`

### 2. Configure Input Actions
For each Input Action:
1. Open the asset
2. Add a "Pressed" trigger
3. Add the corresponding gamepad button:
   - L1 → Gamepad_LeftShoulder
   - L2 → Gamepad_LeftTrigger
   - R1 → Gamepad_RightShoulder
   - R2 → Gamepad_RightTrigger
   - Square → Gamepad_FaceButton_Left
   - Circle → Gamepad_FaceButton_Right
   - X → Gamepad_FaceButton_Bottom
   - Triangle → Gamepad_FaceButton_Top
   - D-Pad → Gamepad_DPad_[Direction]

### 3. Assign Actions in Character Blueprint
1. Open `BP_TrinityFlowCharacter`
2. In Class Defaults → Input → Controller Input Actions:
   - Assign each created Input Action to its corresponding slot

### 4. Configure Input Mapping Context
1. Create or open your Input Mapping Context
2. Add mappings for both keyboard and controller inputs
3. Ensure the context is added to the player controller

## Implementation Details

### Global Defensive System
The X button triggers a stance-based defensive ability:
- Automatically selects the appropriate defense based on current stance
- Soul Stance uses left weapon's Scripted Dodge
- Power Stance uses right weapon's Order ability
- Balanced Stance activates Flow Guard (unique combined defense)

### UI Adaptation
The HUD automatically detects controller usage and updates prompts:
- "Press E" becomes "Press Triangle" when using controller
- Shard UI shows D-Pad controls instead of keyboard keys
- All prompts update dynamically based on input device

### Input Priority
- Controller inputs are bound alongside keyboard inputs
- Both input methods work simultaneously
- UI interactions properly filter input to prevent conflicts

## Testing Controller Support

1. **Basic Combat**:
   - Test L1/R1 for basic attacks
   - Verify L2/R2 trigger abilities correctly
   - Check Square/Circle for secondary abilities

2. **Defensive Abilities**:
   - Enter combat and press X
   - Verify correct defensive ability based on stance
   - Test Flow Guard in balanced stance

3. **Altar Interaction**:
   - Approach altar and press Triangle
   - Use D-Pad to navigate shard UI
   - Confirm activation with Triangle

4. **UI Prompts**:
   - Verify prompts change when using controller
   - Check that all UI elements show correct buttons

## Troubleshooting

### Controller Not Working
- Ensure Input Actions are created and assigned
- Check Input Mapping Context is active
- Verify Enhanced Input plugin is enabled

### Buttons Not Responding
- Check button assignments in Input Actions
- Ensure no input conflicts in mapping context
- Verify character blueprint has actions assigned

### UI Not Showing Controller Prompts
- The system detects controller by checking thumbstick input
- Move either thumbstick to trigger controller mode
- Prompts update automatically when controller is detected






