# TrinityFlow Implementation Plan

## Overview
This document outlines the implementation plan for new features in TrinityFlow, focusing on the shard system, stance mechanics, new abilities, and enemy types to support 1v1 dueling-focused combat.

## Implementation Status

### ✅ Completed
- **Shard System Core** (2025-07-30)
  - ShardComponent for collection/activation tracking
  - ShardPickup actor with floating animation
  - ShardAltar actor with guardian support
  - Character integration with automatic stance updates
  
- **Stance System Foundation** (2025-07-30)
  - StanceComponent with damage modifiers
  - Power, Soul, and Balanced stance types
  - Integration with shard counts

### 🚧 In Progress
- UI integration for shard display
- Visual effects for collection/activation
- Additional puzzle types for altars

### 📋 Planned
- Physical Katana new abilities (E, R)
- Override Katana ability revisions
- New enemy types (Duelist, Void Knight, etc.)
- Balance adjustments based on playtesting

## 1. Shard System

### Core Mechanics
- **Shard Types**:
  - Soul Shards (Blue): Enhance Override Katana
  - Power Shards (Red): Enhance Physical Katana
  
- **Activation System**:
  - Shards must be taken to **Shard Altars** for activation
  - Each altar can activate 3-5 shards
  - Activation requires completing a mini-puzzle
  - 1-2 Shard Altars per chapter

### Shard Effects
- Shards provide stat bonuses based on quantity
- Shard count determines player stance
- No shard combinations - pure quantity-based system

## 2. Stance System

### Stance Types

#### Soul Stance
- **Condition**: Override shards > Physical shards
- **Benefits**: 
  - Soul damage +15%
  - Defense ability: Scripted Dodge (Shift)

#### Power Stance  
- **Condition**: Physical shards > Override shards
- **Benefits**:
  - Physical damage +15%
  - Defense ability: Order (Space)

#### Balanced Stance
- **Condition**: Equal shards (±1 difference)
- **Benefits**:
  - Both damage types +10%
  - Defense ability: Flow Guard (new)

### Flow Guard (Balanced Stance Defense)
- Reduces both soul and physical damage by 40%
- Perfect timing triggers dual-katana counter
- Combines dodge and armor-break properties
- 1.5 second window

## 3. Physical Katana Abilities

### E - Duelist's Honor
- Forces opponent into "Honor Duel" mode
- For 3 seconds, only perfectly timed attacks deal damage
- Successful hits grant stacking damage bonus
- 8 second cooldown
- Range: 400 units

### R - Execution Strike
- Instant kill on enemies below 30% HP
- Heavy damage + bleed on high HP enemies
- Cinematic finisher animation
- 12 second cooldown
- Range: 300 units

## 4. Override Katana Abilities

### Q - Data Breach (Code Break Revision)
- "Corrupts" enemy tags for 3 seconds
- Ghost → Can take physical damage
- Mechanical → Can take soul damage
- 6 second cooldown
- Range: 400 units

### Tab - Soul Link
- 1000 unit range environmental interaction
- Activates soul-powered mechanisms
- Marks enemies for echo damage
- 3 second cooldown

## 5. New Enemy Tags

### Confirmed Tags
- **Duelist**: Parry and riposte abilities, responds to honor duel mechanics
- **Void**: Boss tag, requires alternative damage sources
- **Mimic**: Copies player's last used ability, adaptive defenses
- **Berserk**: Damage and speed increase at low HP, CC resistant
- **Ethereal**: Periodic physical form shifts with different vulnerabilities

## 6. Shard Altar System

### Altar Types
- **Soul Altar**: Activates Override shards
- **Power Altar**: Activates Physical shards  
- **Balance Altar**: Activates both types with bonus

### Altar Features
- Surrounded by guardian enemies
- Mini-puzzle activation requirement
- Visual feedback for activation progress

## 7. Katana-Specific Interactions

### Override Katana Interactions
- Soul Reactors (power sources)
- Data Terminals (hidden paths)
- Energy Bridges (platform creation)

### Physical Katana Interactions
- Mechanical Levers (E to rotate)
- Heavy Doors (physical strength)
- Chain Mechanisms (cut to solve)

## 8. UI/UX Updates

### HUD Elements
- Active stance indicator
- Shard counters (active/inactive)
- Stance change animation
- Dynamic defense ability icon

### Visual Feedback
- Stance-based color coding
- Shard activation effects
- Ability cooldown indicators

## 9. Implementation Priority

### Phase 1: Core Systems
1. Stance system infrastructure in StateComponent
2. Shard counting and comparison logic
3. Stance-based stat modifications

### Phase 2: Shard Altars
1. Interactable altar objects
2. Activation UI and feedback
3. Mini-puzzle system

### Phase 3: Combat Abilities
1. Physical Katana abilities (E/R)
2. Override Katana ability revisions
3. Flow Guard implementation

### Phase 4: Enemy Systems
1. New tag implementation
2. Mimic AI copying system
3. Berserk rage mechanics
4. Ethereal form shifting

### Phase 5: Polish
1. UI/UX updates
2. Visual effects
3. Sound integration
4. Balance testing

## 10. Technical Considerations

### New Components Needed
- `UShardComponent`: Manages shard collection and activation
- `UStanceComponent`: Handles stance switching and effects

### Data Assets Required
- Shard altar configurations
- Stance stat modifiers
- New enemy stats with tags

### Animation Requirements
- Duelist's Honor stance animation
- Execution Strike finisher
- Flow Guard defensive pose
- Stance transition animations

## 11. Future Expansions

### Potential Additions
- Chapter-specific shard types
- Stance-specific combo chains
- Environmental hazards tied to stances
- Co-op stance synergies

### Balancing Considerations
- Shard drop rates
- Stance switching cooldowns
- Ability damage scaling
- Enemy health pools for 1v1 focus

---

*Last Updated: 2025-07-29*  
*Version: 1.0*