/*
 * breakout.h - Main header file for Brick Breaker Ultimate
 * 
 * This file contains all the structure definitions, constants, and function
 * prototypes for the game. Think of this as the "blueprint" that all other
 * files reference.
 */

#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * SCREEN CONSTANTS
 * ============================================================================
 * VGA Mode 13h gives us 320x200 resolution with 256 colors
 */
#ifndef VGA_WIDTH
#define VGA_WIDTH 320
#endif

#ifndef VGA_HEIGHT
#define VGA_HEIGHT 200
#endif

/* ============================================================================
 * GAME CONSTANTS
 * ============================================================================
 * These define the basic limits and sizes for the game
 */

// Maximum number of players (single player or 2-player turn-based)
#define MAX_PLAYERS 2

// Ball system - we support multi-ball power-up
#define MAX_BALLS 5
#define BALL_SIZE 4
#define BALL_SPEED 2

// Paddle settings
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8
#define PADDLE_Y (VGA_HEIGHT - 20)  // Position near bottom of screen
#define PADDLE_SPEED 5
#define PADDLE_LASER_COOLDOWN 10    // Frames between laser shots

// Brick grid layout
#define BRICK_WIDTH 25
#define BRICK_HEIGHT 10
#define BRICK_ROWS 5
#define BRICK_COLS 12
#define BRICK_START_Y 30  // Top margin before bricks start

// Power-up system
#define MAX_POWERUPS 10
#define POWERUP_SIZE 8
#define POWERUP_FALL_SPEED 2

// Visual effects
#define MAX_PARTICLES 100  // For explosion effects
#define MAX_LASERS 10      // Laser beams per player

// Game has 4 levels
#define MAX_LEVELS 4

/* ============================================================================
 * POWER-UP TYPES
 * ============================================================================
 * Different power-ups that can drop from destroyed bricks
 */
typedef enum {
    POWERUP_NONE = 0,
    POWERUP_MULTIBALL,      // Spawns 3 balls at once
    POWERUP_EXPAND_PADDLE,  // Makes paddle bigger (easier)
    POWERUP_SHRINK_PADDLE,  // Makes paddle smaller (harder!)
    POWERUP_LASER,          // Lets you shoot bricks
    POWERUP_SLOW_BALL,      // Slows ball down (easier)
    POWERUP_EXTRA_LIFE,     // Gives an extra life
    POWERUP_FAST_BALL,      // Speeds up ball (harder!)
    POWERUP_COUNT           // Total number of power-up types
} powerup_type_t;

/* ============================================================================
 * STRUCTURE DEFINITIONS
 * ============================================================================
 * These structures hold the state of each game object
 */

/**
 * ball_t - Represents a single ball in play
 * 
 * The game can have up to MAX_BALLS active at once (multi-ball power-up).
 * Each ball tracks its position, velocity, and motion trail.
 */
typedef struct {
    int x, y;              // Current position on screen
    int dx, dy;            // Velocity (direction and speed)
    bool active;           // Is this ball currently in play?
    
    // Motion trail effect - stores last 10 positions
    uint8_t trail_x[10];
    uint8_t trail_y[10];
    int trail_index;       // Current position in circular buffer
} ball_t;

/**
 * brick_t - Represents a single brick in the grid
 * 
 * Each brick can take multiple hits before breaking. When hit, it shakes
 * for visual feedback.
 */
typedef struct {
    uint8_t health;        // 0 = destroyed, 1-3 = hits remaining
    int shake_x, shake_y;  // Offset for shake animation
    int shake_timer;       // How many frames left to shake
} brick_t;

/**
 * powerup_t - A falling power-up
 * 
 * Power-ups drop from destroyed bricks and fall down. If the paddle catches
 * them, the player gets the power-up effect.
 */
typedef struct {
    int x, y;              // Current position
    powerup_type_t type;   // Which power-up is this?
    bool active;           // Is it currently falling?
    uint8_t color;         // Display color (different per type)
} powerup_t;

/**
 * particle_t - A single particle for explosion effects
 * 
 * When bricks break, we spawn 8 particles that spray outward with physics.
 * Each particle has a lifetime and fades out.
 */
typedef struct {
    int x, y;              // Position
    int dx, dy;            // Velocity
    uint8_t color;         // Color
    int life;              // Frames remaining (particle dies when this hits 0)
    bool active;           // Is this particle alive?
} particle_t;

/**
 * laser_t - A laser projectile
 * 
 * When the player has the laser power-up, they can shoot these upward
 * to destroy bricks.
 */
typedef struct {
    int x, y;              // Position
    bool active;           // Is this laser flying?
} laser_t;

/**
 * player_t - One player's complete state
 * 
 * In 2-player mode, we have two of these and players take turns.
 */
typedef struct {
    char name[20];         // Player name (could be used for high scores)
    
    // Paddle state
    int paddle_x;          // Horizontal position
    int paddle_width;      // Current width (changes with power-ups)
    
    // Game state
    int lives;             // Remaining lives
    int score;             // Current score
    
    // Laser power-up state
    bool has_laser;        // Can the player shoot?
    int laser_cooldown;    // Frames until can shoot again
    laser_t lasers[MAX_LASERS];
    
    // Turn-based multiplayer
    bool turn_complete;    // Has this player finished their turn?
} player_t;

/**
 * level_t - Defines a single level's layout and difficulty
 * 
 * Each level has a unique brick pattern and settings.
 */
typedef struct {
    uint8_t pattern[BRICK_ROWS][BRICK_COLS];  // 0 = no brick, 1-3 = brick health
    uint8_t colors[BRICK_ROWS];                // Color for each row
    int ball_speed;                             // How fast the ball moves
    char* name;                                 // Level name for display
} level_t;

/**
 * game_state_t - The complete game state
 * 
 * This is THE main structure that holds everything about the current game.
 * All the game files access this to update and render the game.
 */
typedef struct {
    // Players (1 or 2)
    player_t players[MAX_PLAYERS];
    int num_players;
    int current_player;    // Whose turn? (0 or 1)
    
    // Active game objects
    ball_t balls[MAX_BALLS];
    brick_t bricks[BRICK_ROWS][BRICK_COLS];
    powerup_t powerups[MAX_POWERUPS];
    particle_t particles[MAX_PARTICLES];
    
    // Game progression
    int level;             // Current level (0-3)
    bool all_players_done; // Have both players finished?
    bool paused;           // Is game paused?
    
    // Power-up effects
    int ball_speed_multiplier;  // -1 = slow, 0 = normal, 1 = fast
    
    // Visual effects
    int screen_shake_timer;
    int screen_shake_x, screen_shake_y;
    
    // Audio state
    bool sound_enabled;
    int music_note;        // Current note in melody
    int music_timer;       // Timer for background music
} game_state_t;

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================
 * These are the main functions that other files can call
 */

// Main game functions (in breakout_main.c)
void breakout_init(int num_players);
void breakout_run();

// Helper functions that various files will need
void play_sound(int frequency, int duration_ms);

#endif // BREAKOUT_H