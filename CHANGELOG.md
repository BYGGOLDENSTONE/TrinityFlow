# Changelog

All notable changes to TrinityFlow are documented in this file.

## [Latest] - 2025-08-01

### Added
- **Complete Slate UI System**
  - Modern UI framework replacing Canvas-based rendering
  - Animated main menu with fade-in effects
  - Pause menu with proper input mode handling
  - Redesigned HUD with health bar, weapon panels, and stats display
  - Slate-based floating damage numbers with size scaling
  - Fully animated shard altar interface
  - Centralized UI Manager subsystem
  - Support for both Slate and Canvas rendering modes
  
- **UI Manager Architecture**
  - `UTrinityFlowUIManager` game instance subsystem
  - Centralized state management for all UI
  - Event-driven updates for better performance
  - Proper widget lifecycle management

- **Menu Navigation Features**
  - Full keyboard navigation (W/S keys or arrow keys)
  - E key or Enter for button activation
  - Mouse support with proper cursor management
  - Visual selection highlighting
  - Escape key support in pause menu

- **Game Flow Improvements**
  - Main menu appears on game start
  - Pause game functionality via Escape key
  - Full game restart when returning to main menu from pause
  - Proper input mode switching between UI and gameplay

### Changed
- **HUD System Enhancement**
  - Added `bUseSlateUI` flag to toggle between Canvas and Slate
  - HUD now forwards damage events to UI Manager when using Slate
  - Shard altar UI redirects to Slate implementation when enabled

- **Input System Updates**
  - Added PauseGameAction to character input mapping
  - Character now handles pause input directly
  - Input mode management moved to UI Manager

### Fixed
- **UI Cursor Issues**
  - Fixed mouse cursor not appearing in menus
  - Removed conflicting input mode settings in GameMode
  - Proper input mode transitions between states

- **Menu Transition Problems**
  - Fixed main menu not showing at game start
  - Fixed game restart functionality
  - Resolved level loading issues with single-level setup

## [Previous] - 2025-08-01

### Added
- **Full Controller Support**
  - Controller mappings can be added to existing Input Actions
  - Recommended mapping: L1/L2/Square for left katana, R1/R2/Circle for right katana
  - X button can trigger global defensive ability through RightDefensive action
  - Triangle can be added to E ability for interaction
  - D-Pad navigation for shard activation UI (hardcoded)
  
- **Global Defensive Ability System**
  - X button triggers stance-appropriate defense
  - Soul Stance: Activates Scripted Dodge
  - Power Stance: Activates Order
  - Balanced Stance: Activates Flow Guard (new ability)
  
- **Flow Guard Defensive Ability**
  - Exclusive to Balanced Stance
  - 60% damage reduction for 1.5 seconds
  - Combines benefits of dodge and armor mechanics
  
- **Dynamic UI Prompts**
  - Automatically detects controller usage
  - Updates all prompts to show appropriate buttons
  - Seamless switching between keyboard and controller

### Changed
- **Input System Enhancement**
  - Extended character input to support both keyboard and controller
  - Added input filtering for UI interactions
  - Improved input priority handling

## [Previous] - 2025-08-01 (Earlier)

### Added
- **Enemy Animation System**
  - Created `EnemyAnimationComponent` for managing all enemy combat animations
  - Separate hit response animations for left and right weapon attacks
  - Attack animation that plays when enemies initiate attacks
  - Parry response animation when player achieves perfect defense
  - Animation differentiation based on damage type (soul vs physical)
  
- **Enhanced Damage Information**
  - Extended `FDamageInfo` struct with `bIsLeftWeapon` flag
  - Weapons now identify themselves as left or right hand
  - Damage system passes weapon hand information for appropriate animations

### Changed
- **AI Attack State**
  - Integrated attack animations with AI state machine
  - Attack animations now play before damage is dealt
  
- **Defensive System Integration**
  - Perfect parries now trigger enemy parry response animation
  - Enhanced visual feedback for combat interactions

### Technical Details
- All enemy animations are managed through a modular component system
- Animations automatically interrupt each other when needed
- System works without animations but logs warnings for missing montages
- Full Blueprint integration for easy animation setup

## [Previous] - 2025-07-31

### Added
- **Unified Defensive System**
  - All defensive abilities now use Space key (stance-dependent)
  - Configurable timing windows in Blueprint
  - Perfect timing (0-0.3s): No damage taken with parry animation
  - Moderate timing (0.3-1.0s): 50% damage reduction with block animation
  - Failed defense (>1.0s): Full damage with hit response animation
  - Visual feedback displays defense result above attacker

- **Defensive Animation Montages**
  - Hit Response montage for taking damage
  - Left/Right Parry montages for perfect defense
  - Left/Right Block montages for moderate defense
  - Animations play based on current stance
  - Balanced stance temporarily uses Power animations

- **Shard Damage Bonus System**
  - Each active shard grants 3% damage increase
  - Stance bonus: Doubles shard bonus when in matching stance
  - Example: 3 soul shards = 9% soul damage (18% in Soul stance)
  - Real-time damage bonus display in HUD
  - Clear breakdown shows shard bonus + stance bonus

- **UI Color Theming**
  - Blue theme (#0080FF) for soul/left elements
  - Orange theme (#FF8000) for physical/right elements
  - Consistent color scheme throughout all UI
  - Damage numbers color-coded by type
  - HUD elements use appropriate theme colors

### Changed
- **Defensive Input System**
  - Removed separate Shift key binding for left defense
  - Space key now handles all defensive abilities
  - Defense type determined by current stance
  - Cleaner, more intuitive control scheme

- **Damage Calculation**
  - Shard bonuses now apply to weapon damage
  - Damage numbers accurately reflect all bonuses
  - Defense timing affects damage reduction

### Fixed
- **Altar Activation Crash**
  - Removed direct input checking from HUD
  - Character's AbilityE now handles shard activation UI
  - Resolved Enhanced Input System conflict
  - ProcessShardActivation made public for proper access

- **Compilation Errors**
  - Added getter methods for protected timing windows
  - Fixed TakeDamage signature with FDamageInfo struct
  - Proper damage direction calculation

### Documentation
- Updated README with defensive system changes
- Added color theming to Game Design Document
- Documented shard damage bonus mechanics
- Updated control scheme documentation

## [Previous] - 2025-07-31

### Added
- **C++ Weapon Spawning System**
  - Automatic weapon spawning in BeginPlay
  - Socket attachment support with customizable names
  - Blueprint-assignable weapon classes
  - Safety checks for editor modes (prevents Simulate crashes)
  - Debug logging for troubleshooting

- **Comprehensive Documentation**
  - Created COMBAT_SYSTEM_SETUP.md - Complete combat setup guide
  - Includes weapon, animation, input, and stats configuration
  - Step-by-step troubleshooting section
  - Consolidated weapon and animation guides

### Changed
- **Socket System**
  - Default socket names changed to WeaponSocket_L/R
  - Clear separation from hardcoded weapon bones
  - Supports flexible weapon swapping system

### Fixed
- **Editor Crash in Simulate Mode**
  - Added null checks for GameInstance
  - Skip weapon spawning in non-game worlds
  - Weapons only spawn during actual gameplay

### Documentation
- Deleted outdated files:
  - NEXT_SESSION_NOTES.md (already implemented)
  - ANIMATION_SETUP.md (merged into combat guide)
  - WEAPON_BLUEPRINT_SETUP.md (merged into combat guide)
- Updated README with current documentation structure

## [Previous] - 2025-07-31

### Added
- **Interactive Shard Activation UI**
  - Full-screen HUD panel for selective shard activation
  - Players can choose exactly how many shards of each type to activate
  - Real-time stance preview showing resulting stance before activation
  - Keyboard-only controls with visual indicators
  - Input isolation - movement and combat disabled during UI interaction

### Changed
- **Shard Altar System**
  - Altars now accept both Soul and Power shards in single interaction
  - Removed separate altar types - all altars are universal
  - Added `StartSelectiveActivation()` method for specific shard counts
  - Simplified altar interaction from auto-activation to UI-based selection
  - Restructured component hierarchy - InteractionZone is now root for proper scaling

- **HUD System Updates**
  - Added player info section showing stance and shard counts
  - Moved health bar higher to avoid UI overlap
  - Altar interaction now opens selection UI instead of progress bar
  - Input handling through character class filtering instead of input modes

- **Controls**
  - E key: Open altar UI (when near altar) or activate shards (when UI open)
  - A/D: Switch between Soul/Power shard selection
  - W/S: Increase/Decrease shard amount
  - Q: Cancel and close UI
  - All movement and combat blocked during altar interaction

### Fixed
- **Input System**
  - Fixed input blocking issues with FInputModeUIOnly
  - Implemented custom input filtering in character class
  - Movement and camera controls properly disabled during UI interaction
  - UI-specific inputs work without affecting gameplay

- **Altar Scaling**
  - Fixed hardcoded interaction zone size
  - Component hierarchy now matches shard pickup structure
  - Actor scale now properly affects both mesh and interaction zone
  - Added editable InteractionZoneSize property for fine-tuning

### Technical Details
- All UI implemented in C++ (no Blueprint widgets required)
- Selective activation allows strategic stance management
- Players can save shards for future use at other altars
- Consistent component structure between pickups and altars

## [Previous] - 2025-07-30

### Added
- **Shard System Implementation**
  - Created `UShardComponent` for managing shard collection and activation
  - Tracks active/inactive shard counts for Soul and Power types
  - Methods for collecting shards and activating them at altars
  - Events for UI updates: OnShardCollected, OnShardsActivated
  
  - Created `AShardPickup` actor for collectible shards in the world
  - Floating/rotating animation with configurable parameters
  - Sphere collision for player pickup detection
  - Optional respawn system for testing
  - Blueprint event for collection effects
  
  - Created `AShardAltar` base actor for shard activation locations
  - Configurable altar types (Soul/Power) with max activation limits
  - Guardian enemy system - altars can require defeating enemies first
  - Three puzzle types: None (instant), HoldToActivate, TimingChallenge, PatternMatch
  - Interaction zone with overlap detection
  - Blueprint events for activation states
  
  - Integrated ShardComponent with TrinityFlowCharacter
  - Automatic stance updates when shard counts change
  - Ready for UI integration

## [Previous] - 2025-07-30

### Added
- **Stance System Foundation**
  - Created `UStanceComponent` for managing player stances
  - Added `EStanceType` enum: Soul, Power, Balanced
  - Added `EShardType` enum: Soul, Power
  - Implemented damage modifiers based on stance (15% for matching, 10% for balanced)
  - Default stance set to Power for testing

- **Extended Enemy Tag System**
  - Added 5 new enemy tags to support advanced combat behaviors:
    - Duelist: Parry and riposte abilities
    - Void: Boss tag requiring alternative damage
    - Mimic: Copies player abilities
    - Berserk: Rage mode at low HP
    - Ethereal: Phase shifting vulnerabilities
  - Created `ECharacterTagExtended` to handle additional tags

- **Weapon Attachment System**
  - Added configurable weapon Blueprint class references to Character
  - Implemented proper socket-based weapon attachment
  - Added `WeaponMesh` component to `WeaponBase` for visual representation
  - Fixed katana visibility issues

### Fixed
- **PhysicalKatana Defensive Ability Crash**
  - Added null checks for PhysicalKatanaStats
  - Implemented fallback values when stats not loaded
  - Fixed Order ability timing window calculations

- **Weapon Collision Issues**
  - Disabled all collision on weapon meshes
  - Fixed character teleporting/glitching during animations
  - Weapons no longer interfere with character physics

- **Blueprint Integration**
  - Character now spawns Blueprint weapon classes instead of base C++ classes
  - Socket names are now configurable in Character Blueprint
  - Proper mesh component hierarchy for weapon visuals

### Changed
- **Build System**
  - Fixed ECharacterTag enum to use uint8 (split into two enums for Blueprint compatibility)

### Technical Details
- StanceComponent provides `GetDamageModifier()` for combat calculations
- Weapon spawning now uses `TSubclassOf<>` for Blueprint class references
- Socket attachment uses configurable FName properties

## [2025-07-29]

### Removed
- **Chapter System**: Removed entire chapter/level streaming system due to editor crashes
  - Deleted all Chapter-related classes (ChapterManager, ChapterData, ChapterVolumes, LevelFactory)
  - Removed Quest system classes
  - Removed editor module dependencies
  - Cleaned up documentation and build files
  - Decision: Focus on core combat mechanics rather than complex level management

## [2025-07-29] - Earlier

### Changed
- **Simplified Animation System**
  - Removed complex combo system and input buffering
  - Reduced to 4 essential montages: Right Slash, Left Slash, Interaction, Wondering
  - Direct attack execution without buffering delays
  - All animations now play in-place without root motion

- **New Wondering Animation System**
  - Added wondering animation that plays after 5 seconds of idle in non-combat state
  - Resets on any player input or movement
  - Replaces idle animation after timeout

- **Attack Animation Timing**
  - Fixed attack animations ending early (6-7 frames instead of 20)
  - Now uses actual montage length instead of weapon duration
  - Added configurable Attack Animation Speed Scale property
  - Removed tick-based attack completion in favor of animation callbacks

### Fixed
- **Animation Playback Issues**
  - Fixed montages being cut short due to animation blueprint speed scaling
  - Added compensation for montage rate scales
  - Prevented weapon timer conflicts from interrupting animations
  - Improved montage completion detection

### Added
- **Debug Logging**
  - Extensive animation debugging information
  - Montage progress tracking during playback
  - Slot name and section count logging
  - AnimNotify detection for troubleshooting

### Technical Details
- Removed all root motion code as animations are now in-place
- Simplified AnimationComponent to ~300 lines from ~500
- Attack timing now driven by animation length (0.2s for 20 frames at 60fps)
- Configurable speed scale to compensate for blueprint animation speed issues

## [2025-07-28] - Update 2

### Added
- **Animation Component System**
  - New `AnimationComponent` for centralized animation management
  - All montages organized in one component with Blueprint categories
  - Configurable timing parameters exposed to editor
  
- **New Combo Animation System**
  - Replaced freeze-frame with dynamic wait animations
  - Attack flow: First attack → Wait animation → Second attack or idle
  - Spam combo detection for rapid button presses
  - Movement input cancels combo chains
  - State-based attack management (None, FirstAttack, WaitingForCombo, SecondAttack, ComboAttack)
  
- **Montage Structure**
  - Right Attack: `RightSlash1`, `RightSlash1Wait`, `RightSlash2`, `RightSlashCombo`
  - Left Attack: `LeftSlash1`, `LeftSlash1Wait`, `LeftSlash2`, `LeftSlashCombo`
  - General: `NonCombatIdleMontage`, `CombatIdleMontage`, `InteractionMontage`
  - Two idle animations: combat and non-combat states

- **Timing Configuration**
  - Spam Combo Threshold (default 0.3s) - Time window for rapid attacks
  - Attack Montage Play Rate - Speed multiplier for attacks
  - Combo Window Duration - Time to input next attack

### Changed
- **Animation System Overhaul**
  - Removed freeze frame system in favor of wait animations
  - Consolidated all montage properties from Character to AnimationComponent
  - Updated attack system to use state machine instead of index-based combos
  - Improved interaction montage handling with proper locking
  
- **Combat Flow**
  - More fluid transitions between animations
  - Natural combo chains with visual feedback
  - Better attack coordination between dual katanas

### Fixed
- Animation interruption issues during attacks
- Interaction montage early termination
- Combat state transition animations
- Idle animation looping in different states

## [2025-07-28] - Dual-Wielding Implementation

### Added
- **Dual-Wielding Katana System**
  - Complete overhaul from single weapon to dual katana system
  - Left Katana (OverrideKatana): Soul damage weapon
  - Right Katana (PhysicalKatana): Physical damage weapon
  - Attack coordination system preventing simultaneous basic attacks
  - Separate defensive abilities for each weapon

- **New Input System**
  - LMB: Left katana attack (soul damage)
  - RMB: Right katana attack (physical damage)
  - Q: Code Break (left katana ability)
  - Tab: Echoes of Data (left katana ability)
  - E: Right katana ability / Interaction (non-combat)
  - R: Right katana ability 2
  - Shift: Scripted Dodge (left katana defensive)
  - Space: Order (right katana defensive) / Jump (non-combat)

- **PhysicalKatana Class**
  - New weapon class for physical damage
  - Order defensive ability from Divine Anchor
  - Counter-attack system that strips shields or reduces armor
  - Placeholder abilities for E and R keys

- **Enhanced Stats System**
  - PhysicalKatanaStats data asset class
  - Updated game instance for dual katana stats
  - Separate upgrade paths for each weapon

- **Interaction System**
  - Added interaction animation montage support
  - E key triggers interaction in non-combat state
  - Foundation for future interactable objects

### Changed
- **OverrideKatana Updates**
  - Now deals soul damage instead of physical
  - Abilities remapped: Q for Code Break, Tab for Echoes of Data
  - Removed left/right attack alternation
  
- **Character System**
  - Removed weapon switching mechanic
  - Added attack coordination with bIsAttacking flag
  - Dual weapon attachment (hand_l and hand_r sockets)
  
- **HUD Improvements**
  - Updated to display dual katana information
  - Separate cooldown tracking for each weapon
  - Repositioned health bar to bottom left corner
  - Fixed overlap between health bar and ability info

### Fixed
- Compilation errors with stats subsystem
- HUD color references (Cyan/Orange)
- HealthComponent defense point access
- Missing includes for new weapon classes

### Deprecated
- Divine Anchor weapon system (to be removed)
- Single weapon switching system
- IsKatanaActive() and GetCurrentWeapon() methods

## [Previous] - 2025-07-27

### Added
- **Dual Attack System for Katana**
  - Left Mouse Button (LMB) and Right Mouse Button (RMB) input support
  - Separate attack methods for left and right attacks
  - Animation montage support for different attack animations
  - Attack alternation system in OverrideKatana

- **Attack Delay System**
  - 0.6-second delay between attack input and damage application
  - Allows for proper wind-up animations
  - Visual debug indicators (orange during wind-up, red on hit)
  - Separate timers for concurrent attacks

### Changed
- **Player Character Input**
  - Added RightAttackAction input binding
  - Added RightAttack() function to handle RMB attacks
  - Moved animation montage properties from weapon to character
  - Animation handling now centralized in character blueprint

- **Weapon Base System**
  - Added ExecuteBasicAttack for delayed damage application
  - Added attack timer handles and pending target tracking
  - Updated attack flow to support animation timing

### Technical Details
- Animation montages are now managed in TrinityFlowCharacter blueprint
- OverrideKatana tracks attack type with bIsLeftAttack flag
- Both LMB and RMB currently call BasicAttack but are ready for differentiation
- Attack delay configurable through BasicAttackDamageDelay property

## [Previous] - 2025-07-26

### Removed
- **C++ Animation System**
  - Removed PlayerAnimationComponent class entirely
  - Removed all animation montage references from C++ code
  - Removed AnimationCore module dependency
  - Simplified attack system to focus on gameplay logic
  
### Changed
- **Character Attack System**
  - Attack() method now directly calls weapon attack without animation coupling
  - Cleaner separation between gameplay and visual representation
  - Preparation for new animation system implementation

### Technical Details
- Deleted files:
  - `Source/TrinityFlow/Public/Player/PlayerAnimationComponent.h`
  - `Source/TrinityFlow/Private/Player/PlayerAnimationComponent.cpp`
- Updated TrinityFlowCharacter to remove animation component references
- Updated build configuration to remove AnimationCore dependency

## [Previous] - 2025-07-24

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
  - Uses CrowdFollowingComponent for better group navigation

### Changed
- **Enemy Base Class Refactor**
  - Changed `AEnemyBase` from `APawn` to `ACharacter` for better movement support
  - Now uses built-in CharacterMovementComponent instead of custom movement
  - Leverages Unreal's character features for improved AI navigation
  - Better integration with movement systems
  
- **Movement System Improvements**
  - Removed custom FloatingPawnMovement in favor of CharacterMovement
  - Movement speed now properly updates from stats data
  - Better pathfinding with proper ground detection and obstacle avoidance
  

### Updated
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