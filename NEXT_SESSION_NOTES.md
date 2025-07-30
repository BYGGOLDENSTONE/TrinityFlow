# Next Session Notes - Shard System Implementation

## Overview
Implement the shard collection and activation system as outlined in the IMPLEMENTATION_PLAN.md.

## Tasks for Next Session:

### 1. ShardComponent (New Component)
Create `UShardComponent` to manage shard collection and activation:
- Track soul and power shard counts (active/inactive)
- Methods:
  - `CollectShard(EShardType Type)` - Add to inactive count
  - `ActivateShards(EShardType Type, int32 Count)` - Move from inactive to active
  - `GetActiveShardCount(EShardType Type)` - For stance calculation
  - `GetInactiveShardCount(EShardType Type)` - For UI display
- Events:
  - `OnShardCollected` - Broadcast when shard picked up
  - `OnShardsActivated` - Broadcast when shards are activated at altar

### 2. Shard Pickup Actor
Create `AShardPickup` actor class:
- Mesh component for visual representation
- Sphere collision for pickup detection
- `EShardType` property to determine shard type
- Float/rotate animation in Blueprint
- Particle effects on collection
- Respawn timer option for testing

### 3. Shard Altar System
Create `AShardAltar` base actor:
- Properties:
  - `EShardType AltarType` (Soul/Power/Balance)
  - `int32 MaxShardsPerActivation` (3-5)
  - `TArray<AActor*> GuardianEnemies` - Enemies that guard the altar
- Interaction system:
  - Detect player proximity
  - Check for inactive shards
  - Open activation UI
- Mini-puzzle options:
  - Simple timing-based challenge
  - Pattern matching
  - Hold-to-activate with enemy waves

### 4. Integration with Stance System
- Add ShardComponent to TrinityFlowCharacter
- On shard count change:
  - Call `StanceComponent->UpdateStanceFromShards(SoulShards, PowerShards)`
  - This will automatically update the stance based on shard counts
- The stance change will trigger damage modifier updates

### 5. UI Requirements
- HUD updates:
  - Active shard counter (Soul: X, Power: Y)
  - Inactive shard counter
  - Current stance indicator
- Altar interaction UI:
  - Show available shards
  - Activation progress
  - Puzzle interface

### 6. Blueprint Setup Notes
After implementation:
1. Create BP_ShardPickup_Soul and BP_ShardPickup_Power
2. Create BP_ShardAltar with variations for each type
3. Place test shards and altars in the level
4. Configure guardian enemies around altars

## Implementation Order:
1. ShardComponent first (core system)
2. ShardPickup actor (collectibles)
3. Basic altar without puzzle (test activation)
4. UI elements
5. Puzzle system
6. Visual polish

## Testing Scenarios:
- Collect multiple shards of each type
- Activate shards at altar
- Verify stance changes based on active shard counts
- Test guardian enemy spawning at altars
- Verify UI updates correctly

## Notes from Current Session:
- StanceComponent already has `UpdateStanceFromShards()` method ready
- Power stance is default and working (+15% physical damage)
- Character already has all necessary components set up
- Tag system is ready for guardian enemies (can use new tags like Duelist)

## Reference:
See IMPLEMENTATION_PLAN.md for full shard system design details.