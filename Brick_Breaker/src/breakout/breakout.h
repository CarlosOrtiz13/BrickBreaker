#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <stdint.h>
#include <stdbool.h>

// VGA constants
#ifndef VGA_WIDTH
#define VGA_WIDTH 320
#endif

#ifndef VGA_HEIGHT
#define VGA_HEIGHT 200
#endif

// Game constants
#define MAX_PLAYERS 2
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 8
#define PADDLE_Y (VGA_HEIGHT - 20)
#define PADDLE_SPEED 5
#define PADDLE_LASER_COOLDOWN 10

#define BALL_SIZE 4
#define BALL_SPEED 2
#define MAX_BALLS 5

#define BRICK_WIDTH 25
#define BRICK_HEIGHT 10
#define BRICK_ROWS 5
#define BRICK_COLS 12
#define BRICK_START_Y 30

#define MAX_POWERUPS 10
#define POWERUP_SIZE 8
#define POWERUP_FALL_SPEED 2

#define MAX_PARTICLES 100
#define MAX_LASERS 10

#define MAX_LEVELS 4

// Power-up types
typedef enum {
    POWERUP_NONE = 0,
    POWERUP_MULTIBALL,
    POWERUP_EXPAND_PADDLE,
    POWERUP_SHRINK_PADDLE,
    POWERUP_LASER,
    POWERUP_SLOW_BALL,
    POWERUP_EXTRA_LIFE,
    POWERUP_FAST_BALL,
    POWERUP_COUNT
} powerup_type_t;

// Forward declarations for complex types
typedef struct powerup_t powerup_t;
typedef struct ball_t ball_t;
typedef struct particle_t particle_t;
typedef struct laser_t laser_t;
typedef struct brick_t brick_t;
typedef struct level_t level_t;

// Power-up structure
struct powerup_t {
    int x, y;
    powerup_type_t type;
    bool active;
    uint8_t color;
};

// Ball structure
struct ball_t {
    int x, y;
    int dx, dy;
    bool active;
    uint8_t trail_x[10];
    uint8_t trail_y[10];
    int trail_index;
};

// Particle structure
struct particle_t {
    int x, y;
    int dx, dy;
    uint8_t color;
    int life;
    bool active;
};

// Laser structure
struct laser_t {
    int x, y;
    bool active;
};

// Brick structure
struct brick_t {
    uint8_t health;
    int shake_x, shake_y;
    int shake_timer;
};

// Player structure
typedef struct {
    char name[20];
    int paddle_x;
    int paddle_width;
    int lives;
    int score;
    bool has_laser;
    int laser_cooldown;
    laser_t lasers[MAX_LASERS];
    bool turn_complete;
} player_t;

// Game state structure
typedef struct {
    // Players
    player_t players[MAX_PLAYERS];
    int num_players;
    int current_player;  // Whose turn it is (0 or 1)
    
    // Balls
    ball_t balls[MAX_BALLS];
    int ball_speed_multiplier;
    
    // Bricks
    brick_t bricks[BRICK_ROWS][BRICK_COLS];
    
    // Power-ups
    powerup_t powerups[MAX_POWERUPS];
    
    // Particles
    particle_t particles[MAX_PARTICLES];
    
    // Game state
    int level;
    bool all_players_done;
    bool paused;
    int screen_shake_timer;
    int screen_shake_x, screen_shake_y;
    
    // Sound
    bool sound_enabled;
    int music_note;
    int music_timer;
} game_state_t;

// Level patterns
struct level_t {
    uint8_t pattern[BRICK_ROWS][BRICK_COLS];
    uint8_t colors[BRICK_ROWS];
    int ball_speed;
    char* name;
};

// Public functions
void breakout_init(int num_players);
void breakout_run();

// Sound function
void play_sound(int frequency, int duration_ms);

#endif // BREAKOUT_H