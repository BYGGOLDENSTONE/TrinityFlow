# Shard System Setup Guide

## Overview
The shard system allows players to collect shards and selectively activate them at altars to change their stance, affecting damage modifiers. Players can choose exactly how many shards of each type to activate, giving them strategic control over their character build.

## Blueprint Setup

### 1. Create Shard Pickup Blueprints

1. Create new Blueprint class:
   - Parent: `AShardPickup`
   - Name: `BP_ShardPickup_Soul`
   
2. In the Blueprint:
   - Set `ShardType` to `Soul`
   - Set up mesh (any small glowing object works)
   - Configure visual settings:
     - Float Amplitude: 20.0
     - Float Speed: 2.0
   - Optional: Enable respawns for testing (10s respawn time)
   
3. Duplicate for Power shards:
   - Name: `BP_ShardPickup_Power`
   - Set `ShardType` to `Power`
   - Use different colored mesh/material

### 2. Create Shard Altar Blueprint

1. Create new Blueprint class:
   - Parent: `AShardAltar`
   - Name: `BP_ShardAltar_Universal`
   
2. In the Blueprint:
   - Set `bAcceptsBothTypes` to `true` (default)
   - Configure altar settings:
     - Max Shards Per Activation: 99 (no practical limit)
     - Min Shards To Activate: 1
     - Puzzle Type: `HoldToActivate` (2 seconds)
   - Set up altar mesh (pedestal, shrine, etc.)
   - Adjust mesh position relative to interaction zone if needed
   
3. Component Structure:
   - InteractionZone (Root) - Scales with actor
   - AltarMesh (Child) - Scales proportionally
   
4. Scaling:
   - Use actor scale to resize entire altar
   - Both mesh and interaction zone scale together
   - Fine-tune with InteractionZoneSize property if needed

### 3. Place in Level

1. Place several shard pickups around the level (mix of Soul and Power)
2. Place universal altars at strategic locations
3. Optional: Add guardian enemies to altar's `GuardianEnemies` array

## Testing the System

### Console Commands (create these in Blueprint if needed)
```
// Give player shards
GiveShards Soul 5
GiveShards Power 3

// Check current stance
ShowStance

// Check shard counts
ShowShards
```

### Expected Behavior

1. **Collecting Shards**:
   - Walk over shard pickup
   - Shard disappears with effects
   - Inactive shard count increases

2. **Activating at Altar (Interactive UI)**:
   - Approach altar with inactive shards
   - Press E to open shard activation UI
   - UI shows full-screen panel with:
     - Current active/inactive shard counts
     - Two sections for Soul and Power shards
     - Selected section highlighted in yellow
   - Controls:
     - A/D: Switch between Soul/Power shard selection
     - W/S: Increase/Decrease activation amount
     - E: Confirm and activate selected shards
     - Q: Cancel and close UI
   - Player movement and combat are disabled while UI is open
   - After activation, shards move from inactive to active
   - Stance updates based on new active shard totals

3. **Stance Changes**:
   - Power > Soul shards = Power stance (+15% physical damage)
   - Soul > Power shards = Soul stance (+15% soul damage)
   - Equal shards = Balanced stance (+10% both damages)

## UI Integration

### HUD Display
The C++ HUD system displays:
- Player info section shows:
  - Current stance and damage bonuses
  - Active/Inactive shard counts for both types
- Interactive altar UI provides full shard management

### System Events

1. **ShardComponent Events**:
   - `OnShardCollected(ShardType, NewInactiveCount)`
   - `OnShardsActivated(ShardType, ActivatedCount, NewActiveCount)`

2. **StanceComponent Events**:
   - `OnStanceChanged(NewStance)`

3. **Altar Events**:
   - `OnAltarInteractionStarted(Interactor)`
   - `OnAltarInteractionEnded()`
   - `OnAltarActivated(SoulShardsActivated, PowerShardsActivated, Activator)`

## Implementation Notes

### Key Features
1. **Selective Activation**: Players choose exactly how many shards to activate
2. **Strategic Stance Control**: Save shards for later to maintain desired stance
3. **Universal Altars**: One altar type accepts both shard types
4. **Input Isolation**: Movement/combat disabled during UI interaction
5. **Visual Feedback**: Real-time stance preview before activation

### Technical Details
- All UI implemented in C++ (TrinityFlowHUD)
- No Blueprint widgets required
- Input handled through character class filtering
- Supports keyboard-only interaction (no mouse needed)

## Next Steps

1. Add visual effects:
   - Shard collection particles
   - Altar activation effects
   - Stance change effects

2. Balance considerations:
   - Shard distribution in levels
   - Altar placement strategy
   - Stance bonus values

3. Polish:
   - Sound effects for UI interactions
   - Animation for shard activation
   - Visual feedback for stance changes