# BrickBreaker
A feature-rich Breakout clone written entirely in C for bare metal x86 architecture. This isn't just a game—it's a complete game engine running directly on hardware with no operating system.
🌟 Features
🎯 Core Gameplay

Turn-based multiplayer - Classic arcade-style gameplay where players take turns
4 unique levels with increasing difficulty and different brick patterns
Fair competition - Both players face the same levels for balanced gameplay
Score tracking with automatic winner determination

⚡ Advanced Game Mechanics

7 Power-ups with unique effects:

🟨 Multi-Ball (3 balls at once)
🟩 Expand Paddle (bigger paddle)
🟥 Shrink Paddle (smaller paddle)
🔵 Laser (shoot bricks!)
🔵 Slow Motion (bullet time effect)
🟪 Extra Life (+1 life)
🔴 Fast Ball (speed boost)



🎨 Visual Effects

Particle system - 100 simultaneous particles for explosions and effects
Ball motion trails - Smooth 10-frame trailing effect
Screen shake - Dynamic screen shake on big impacts
Brick shake - Damage feedback animation
Power-up sparkles - Visual feedback for power-up collection

🎵 Audio System

PC Speaker synthesis - Real-time audio generation using the PC speaker
Background music - 8-note melody playing continuously
Sound effects for:

Brick destruction (variable pitch based on type)
Paddle bounces
Wall collisions
Power-up collection
Laser shots


Toggle music on/off during gameplay

🎓 Technical Achievements

Pure bare metal - No OS, no libraries, direct hardware access
VGA Mode 13h - 320x240 @ 256 colors
60 FPS gameplay - Consistent frame timing using PIT timer
Keyboard interrupts - Real-time input handling via IRQ1
Custom memory management - Static allocation for embedded environment
State machine - Proper game state transitions and turn management

🎮 Controls
Gameplay

Arrow Keys or A/D - Move paddle left/right
Left Ctrl - Shoot laser (when power-up is active)
P - Pause/Unpause
M - Toggle music on/off
Space - Restart game (after game over)
ESC - Exit to kernel

📊 Game Modes
Single Player

Complete all 4 levels
3 lives to start
Maximize your score!

Two Player (Turn-Based)

Player 1 plays until losing all lives or clearing all levels
"PLAYER 2 TURN" transition screen appears
Player 2 plays with the same levels
Winner is determined by highest score
Tie handling for equal scores

🏗️ Technical Architecture
System Requirements

x86 (i686) processor
VGA-compatible graphics card
PS/2 keyboard
PC speaker (optional, for sound)

Components
src/
├── breakout/
│   ├── breakout.h          # Game structures and constants
│   └── breakout.c          # Main game implementation (~1500 lines)
├── vga/
│   └── vga.c               # VGA Mode 13h driver
├── keyboard/
│   └── keyboard.c          # PS/2 keyboard interrupt handler
├── timer/
│   └── timer.c             # PIT timer for frame timing
└── io/
    └── io.asm              # Low-level I/O operations
Key Systems
Particle Engine

100 simultaneous particles
Physics simulation with gravity
Lifespan management
Color fading effects

Power-up System

Random drop chance (30%)
8 different power-up types
Collision detection with paddle
Visual effects on collection

Laser System

Projectile physics
Brick collision detection
Cooldown management
Visual laser beam rendering

Audio Synthesis

Frequency generation via PIT
PC speaker control via port 0x61
Background music sequencer
Dynamic sound effects

📈 Scoring System
ActionPointsDestroy 1-hit brick10Destroy 2-hit brick20 (10 per hit)Destroy 3-hit brick30 (10 per hit)
Perfect Game: 1,920 points (all levels completed)
🎯 Level Design
Level 1: Classic

Full brick wall (60 bricks)
1 hit per brick
Ball speed: Normal
Max Score: 600 points

Level 2: Checkerboard

Alternating pattern (30 bricks)
2 hits per brick
Ball speed: Fast
Max Score: 300 points

Level 3: Pyramid

Pyramid formation (42 bricks)
Mixed 2-3 hit bricks
Ball speed: Fast
Max Score: 420 points

Level 4: Boss

Full wall (60 bricks)
3 hits per brick
Ball speed: Very fast
Max Score: 600 points

🔧 Building
Prerequisites
bash# Cross-compiler toolchain
i686-elf-gcc
i686-elf-ld

# Assembler
nasm

# Emulator (for testing)
qemu-system-i386
Compilation
bash# Add to your kernel's Makefile
./build/breakout/breakout.o: ./src/breakout/breakout.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@
Integration
c#include "breakout/breakout.h"

void kernel_main()
{
    // Initialize your kernel systems first
    // (GDT, IDT, timer, keyboard, VGA, etc.)
    
    // Initialize game
    breakout_init(2);  // 1 for single player, 2 for multiplayer
    
    // Run game loop
    breakout_run();
    
    // Game returns on ESC press
}
🎓 Educational Value
This project demonstrates:

Bare metal programming techniques
Hardware abstraction for VGA, keyboard, timer
Interrupt handling for responsive input
Game state management and turn-based logic
Real-time systems with consistent frame timing
Memory management without malloc/free
Audio synthesis using limited hardware
Visual effects programming
Collision detection algorithms
Game design principles

📚 Code Statistics

~1,500 lines of game code
0 external libraries (pure bare metal)
60 FPS target frame rate
100 particles maximum
7 power-up types
4 distinct levels
2 player support

🎨 Visual Showcase
Particle Effects

Explosion particles spray in 8 directions
Gravity simulation on particles
Color fading over lifespan
Sparkles on paddle hits

Screen Effects

Dynamic screen shake (2-pixel offset)
Brick shake on damage (random offset)
Ball motion trails (10-frame history)
Power-up glow effects

UI Elements

Player indicator (P1/P2) with color coding
Real-time score display
Lives display (heart shapes)
Level indicator
Laser charge indicators
Turn transition screens
Winner announcement screen

🏆 Achievements
Technical
✅ Complete game engine from scratch
✅ No operating system dependencies
✅ Real-time audio synthesis
✅ Particle physics system
✅ Turn-based multiplayer
✅ 60 FPS performance
Design
✅ 7 unique power-ups
✅ 4 distinct levels
✅ Fair competitive gameplay
✅ Professional visual polish
✅ Intuitive controls
✅ Clear win conditions
📝 License
MIT License - See LICENSE file for details
🤝 Contributing
This was created as an educational project for operating systems coursework. Feel free to fork and experiment!
👨‍💻 Authors
Created as part of a bare metal OS kernel project demonstrating:

Low-level hardware programming
Game development without OS support
Real-time systems design
Interrupt-driven architecture

🎯 Future Enhancements
Potential additions:

 Network multiplayer over serial port
 High score persistence to disk
 More levels with moving bricks
 Boss battles with special patterns
 Replay system
 Sound Blaster support for better audio

🌟 Showcase
This project demonstrates that even without an operating system, complex interactive applications with graphics, sound, and multiplayer gameplay are possible with proper hardware control and efficient programming.
Perfect for: OS development courses, embedded systems education, game engine architecture study, and low-level programming enthusiasts.
