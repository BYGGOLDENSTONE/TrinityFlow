# Shard System Setup Guide

## Overview
The shard system allows players to collect shards and activate them at altars to change their stance, affecting damage modifiers.

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

### 2. Create Shard Altar Blueprints

1. Create new Blueprint class:
   - Parent: `AShardAltar`
   - Name: `BP_ShardAltar_Soul`
   
2. In the Blueprint:
   - Set `AltarType` to `Soul`
   - Configure altar settings:
     - Max Shards Per Activation: 5
     - Min Shards To Activate: 1
     - Puzzle Type: `HoldToActivate` (3 seconds)
   - Set up altar mesh (pedestal, shrine, etc.)
   
3. Duplicate for Power altars:
   - Name: `BP_ShardAltar_Power`
   - Set `AltarType` to `Power`

### 3. Place in Level

1. Place several shard pickups around the level
2. Place at least one altar of each type
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

2. **Activating at Altar**:
   - Approach altar with shards
   - Hold interact button for 3 seconds
   - Shards move from inactive to active
   - Stance updates automatically

3. **Stance Changes**:
   - Power > Soul shards = Power stance (+15% physical damage)
   - Soul > Power shards = Soul stance (+15% soul damage)
   - Equal shards = Balanced stance (+10% both damages)

## UI Integration Points

The system fires these events for UI updates:

1. **ShardComponent Events**:
   - `OnShardCollected(ShardType, NewInactiveCount)`
   - `OnShardsActivated(ShardType, ActivatedCount, NewActiveCount)`

2. **StanceComponent Events**:
   - `OnStanceChanged(NewStance)`

3. **Altar Events**:
   - `OnAltarInteractionStarted(Interactor)`
   - `OnAltarInteractionEnded()`
   - `OnAltarActivated(ShardType, ShardsActivated, Activator)`

## Next Steps

1. Create UI widgets to display:
   - Active/Inactive shard counts
   - Current stance indicator
   - Altar interaction progress

2. Add visual effects:
   - Shard collection particles
   - Altar activation effects
   - Stance change effects

3. Implement remaining puzzle types:
   - Timing challenges
   - Pattern matching mini-games

4. Balance shard distribution in levels