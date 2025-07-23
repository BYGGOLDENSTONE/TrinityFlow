# Changelog

All notable changes to TrinityFlow are documented in this file.

## [Latest] - 2025-07-23

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