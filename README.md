BrickBreaker (Bare-Metal Breakout Game)
Overview

BrickBreaker is a Breakout-style game written entirely in C that runs directly on bare-metal x86 hardware, without any operating system or external libraries.
This project was developed as part of an operating systems / low-level systems coursework and demonstrates how a complete interactive application can be built using direct hardware access, interrupts, and real-time constraints.

Unlike traditional games that rely on an OS, this project runs inside a custom kernel, handling graphics, input, timing, memory, and audio manually.

Educational Focus

As a cybersecurity engineering undergraduate, this project emphasizes:

Low-level system design and hardware interaction

Interrupt-driven input handling (keyboard, timer)

Real-time execution guarantees (60 FPS)

Memory-safe static allocation (no malloc/free)

Deterministic game state management

Understanding the attack surface of bare-metal systems

The goal was not just to build a game, but to understand how software behaves at the lowest level of execution.

Core Features
Gameplay

Classic Breakout mechanics

Single-player or two-player turn-based mode

4 levels with increasing difficulty

Score tracking and automatic winner determination

Power-Ups (7 Types)

Multi-Ball (3 balls)

Expand Paddle

Shrink Paddle

Laser (brick shooting)

Slow Motion

Extra Life

Fast Ball

Visual System

VGA Mode 13h (320×240, 256 colors)

Particle system (up to 100 particles)

Ball motion trails (10-frame history)

Screen shake and brick damage feedback

Power-up glow and spark effects

Audio System

PC speaker sound synthesis

Background music (looped melody)

Sound effects for:

Brick destruction

Paddle collisions

Wall collisions

Power-up collection

Laser shots

Music can be toggled during gameplay

Technical Implementation
Hardware & Architecture

Pure x86 (i686) bare metal

No operating system

No standard libraries

Direct port I/O

Interrupt-driven execution

Key Systems

Timer: PIT-based frame timing (60 FPS)

Keyboard: IRQ1 PS/2 interrupt handler

Graphics: VGA Mode 13h framebuffer

Memory: Static allocation only

State Machine: Clean transitions between menus, turns, gameplay, and end screens

Controls
Gameplay

Left / Right Arrow or A / D – Move paddle

Left Ctrl – Fire laser (if power-up active)

P – Pause / Resume

M – Toggle music

Space – Restart after game over

ESC – Exit game and return to kernel

Game Modes
Single Player

3 lives

Complete all 4 levels

Maximize score

Two Player (Turn-Based)

Player 1 plays first

Player 2 plays the same levels

Highest score wins

Tie handling supported

Project Structure
src/
├── breakout/
│   ├── breakout.h      # Game constants and structures
│   └── breakout.c      # Main game logic (~1500 LOC)
├── vga/
│   └── vga.c           # VGA Mode 13h driver
├── keyboard/
│   └── keyboard.c      # PS/2 keyboard interrupt handler
├── timer/
│   └── timer.c         # PIT timer and frame timing
└── io/
    └── io.asm          # Low-level port I/O

System Requirements

x86 (i686) processor

VGA-compatible graphics

PS/2 keyboard

PC speaker (optional, for sound)

Build & Run Instructions
Prerequisites (Linux / Ubuntu)
sudo apt update
sudo apt install -y build-essential nasm qemu-system-x86

Cross-Compiler

You must have:

i686-elf-gcc

i686-elf-ld

(Used for freestanding kernel compilation.)

Build the Game
cd Brick_Breaker
chmod +x build.sh
./build.sh


This generates the bootable binary locally.

Run in QEMU
qemu-system-i386 -kernel bin/os.bin


The binary is intentionally not stored in GitHub and must be built locally.
