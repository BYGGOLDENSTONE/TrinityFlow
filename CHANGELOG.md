# Changelog

All notable changes to TrinityFlow are documented in this file.

## [Latest] - 2025-07-24

### Added
- **Enemy AI State Machine System**
  - Pure C++ implementation of state-based AI
  - Three core states: Idle, Chase, and Attack
  - Smooth transitions between states based on player detection and range
  - Integration with Unreal's navigation system for pathfinding
  
- **AI States Implementation**
  - `UAIState` base class with Enter/Update/Exit pattern
  - `UAIState_Idle`: Enemy patrols or waits, detects players within sight range
  - `UAIState_Chase`: Enemy pursues player using navigation mesh pathfinding
  - `UAIState_Attack`: Enemy attacks when player is within attack range
  
- **Enemy AI Controller**
  - `AEnemyAIController` handles movement requests and pathfinding
  - Automatic navigation mesh integration
  - Support for FloatingPawnMovement
  
- **Movement System**
  - Added FloatingPawnMovement component to all enemies
  - Configurable movement speed in enemy stats
  - Proper collision settings for navigation
  
- **Animation Support**
  - Added `GetCurrentSpeed()` and `IsMoving()` blueprint functions
  - Velocity-based animation system ready for implementation
  - Movement component properly updates velocity for animation blueprints

### Changed
- **Enemy Base Class Updates**
  - Integrated AI state machine component
  - Added movement component and AI controller support
  - Removed hardcoded combat behavior in favor of state machine
  - Made `bHasSeenPlayer` accessible for state management
  
- **Build Configuration**
  - Added AIModule and NavigationSystem dependencies
  - Proper module configuration for AI features

### Fixed
- **Combat State Manager Integration**
  - Enemies now properly trigger combat state when detecting player
  - State transitions correctly notify combat manager
  
- **Movement Issues**
  - Fixed FloatingPawnMovement not processing input
  - Removed plane constraints that caused positioning issues
  - Proper movement component initialization and activation

### Technical Details
- **State Machine Architecture**
  - Component-based design for easy integration
  - Blueprint-compatible state classes
  - Configurable state transitions through Blueprint
  
- **Navigation Integration**
  - Full support for Unreal's navigation mesh
  - Pathfinding with obstacle avoidance
  - Dynamic path updates during chase

## [Previous] - 2025-07-23 (Update 2)

### Fixed
- **Tag System in Editor**
  - Fixed bitmask enum display issues causing wrong tags to appear
  - Updated enum definition to use proper bit shift notation
  - Added correct enum metadata for editor compatibility
  - Tags now properly save and load from data assets

- **Player Stats Loading**
  - Player character now loads stats from the subsystem
  - Added fallback to default values if stats not found
  - Consistent with enemy stat loading pattern

### Added
- Debug logging for tag values to help diagnose issues
- Detailed setup instructions in GDD for stats system

### Changed
- Updated documentation with complete setup steps
- Improved error handling for missing stats

## [Previous Update] - 2025-07-23

### Added
- **Stats Management System**
  - Character stats data assets for all characters and enemies
  - Weapon stats data assets for Override Katana and Divine Anchor
  - Centralized stats subsystem (Game Instance Subsystem)
  - Runtime stat reloading capability
  - Editor-friendly configuration without code recompilation

- **Data-Driven Tag System**
  - Tag data table structure with effects
  - Support for damage immunity, reduction, and modifiers
  - Tag validation (conflicts and requirements)
  - Tag manager utilities

- **Documentation**
  - Comprehensive README.md
  - Stats System Guide
  - Updated Additions to GDD

### Changed
- **Combat Visuals**
  - Damage numbers: Yellow → Red for direct damage
  - Echo damage: Purple → Blue
  - Increased damage number size (1.5x-3.5x scaling)

- **Divine Anchor Abilities**
  - Gravity Pull: Now uses configurable force multiplier and duration
  - Smash: Physics-based knockback instead of teleportation
  - All ability parameters now data-driven

### Fixed
- **Physics System**
  - Enemies no longer get stuck in air when spamming abilities
  - Added proper ground detection for all physics effects
  - 3-second timeout for physics effects
  - Automatic cleanup for floating enemies
  - Improved gravity simulation

- **Build Errors**
  - Fixed FString::Join compilation error with FName arrays

## [Previous] - 2025-07-22

### Added
- Floating enemy information panels
- Attack timing indicators
- Echo damage system improvements
- Defensive ability windows

### Changed
- Camera to shoulder view
- UI elements removed backgrounds

### Fixed
- Enemy attack timing
- Echo damage calculations
- Component initialization order

## [Initial] - 2025-07-21

### Added
- Complete TrinityFlow combat system
- Dual-damage mechanics (Physical/Soul)
- Two weapons with unique abilities
- Five enemy types
- Tag-based resistance system