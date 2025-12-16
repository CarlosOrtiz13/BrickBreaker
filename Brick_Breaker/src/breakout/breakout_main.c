/*
 * breakout_main.c - Main game logic and coordination
 * 
 * This is the "conductor" of the game. It:
 * - Initializes the game state
 * - Defines the 4 levels
 * - Runs the main game loop
 * - Coordinates all the other systems
 * - Handles turn-based multiplayer
 * - Manages level progression
 * 
 * Think of this as the "main" function for the breakout game!
 * 
 * Author: CS Student
 * Date: December 2024
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

/* ============================================================================
 * GLOBAL GAME STATE
 * ============================================================================
 * This is THE game state. All other files access this through extern.
 */
game_state_t game;

/* ============================================================================
 * LEVEL DEFINITIONS
 * ============================================================================
 * We have 4 levels, each with unique brick patterns and difficulty.
 */
level_t levels[MAX_LEVELS] = {
    // LEVEL 1: CLASSIC - Full brick wall, easy
    {
        .pattern = {
            {1,1,1,1,1,1,1,1,1,1,1,1},  // Row 1: All 1-hit bricks
            {1,1,1,1,1,1,1,1,1,1,1,1},  // Row 2
            {1,1,1,1,1,1,1,1,1,1,1,1},  // Row 3
            {1,1,1,1,1,1,1,1,1,1,1,1},  // Row 4
            {1,1,1,1,1,1,1,1,1,1,1,1}   // Row 5
        },
        .colors = {4, 12, 14, 2, 1},  // Red, Light Red, Yellow, Green, Blue
        .ball_speed = 2,
        .name = "CLASSIC"
    },
    
    // LEVEL 2: CHECKERBOARD - Alternating pattern, harder bricks
    {
        .pattern = {
            {2,0,2,0,2,0,2,0,2,0,2,0},  // Checkerboard pattern
            {0,2,0,2,0,2,0,2,0,2,0,2},  // 2-hit bricks
            {2,0,2,0,2,0,2,0,2,0,2,0},
            {0,2,0,2,0,2,0,2,0,2,0,2},
            {2,0,2,0,2,0,2,0,2,0,2,0}
        },
        .colors = {12, 12, 14, 14, 2},
        .ball_speed = 3,  // Faster!
        .name = "CHECKERBOARD"
    },
    
    // LEVEL 3: PYRAMID - Triangle shape, mixed difficulty
    {
        .pattern = {
            {0,0,0,0,0,3,3,0,0,0,0,0},  // Top: 3-hit bricks
            {0,0,0,0,2,2,2,2,0,0,0,0},  // 2-hit bricks
            {0,0,0,2,2,2,2,2,2,0,0,0},
            {0,0,2,2,2,2,2,2,2,2,0,0},
            {0,2,2,2,2,2,2,2,2,2,2,0}   // Bottom: 2-hit bricks
        },
        .colors = {4, 12, 14, 2, 1},
        .ball_speed = 3,
        .name = "PYRAMID"
    },
    
    // LEVEL 4: BOSS - Full wall of tough bricks!
    {
        .pattern = {
            {3,3,3,3,3,3,3,3,3,3,3,3},  // All 3-hit bricks!
            {3,0,0,3,3,3,3,3,3,0,0,3},  // Some gaps for variety
            {3,3,3,3,3,3,3,3,3,3,3,3},
            {3,0,0,3,3,3,3,3,3,0,0,3},
            {3,3,3,3,3,3,3,3,3,3,3,3}
        },
        .colors = {4, 4, 12, 12, 14},
        .ball_speed = 4,  // Fastest!
        .name = "BOSS"
    }
};

/* ============================================================================
 * EXTERNAL FUNCTION DECLARATIONS
 * ============================================================================
 * These are implemented in other files but we need to call them here.
 */

// From breakout_physics.c
extern void init_bricks();
extern void init_balls();
extern void update_balls();
extern void update_bricks();
extern bool check_level_complete();

// From breakout_powerups.c
extern void update_powerups();

// From breakout_particles.c
extern void update_particles();

// From breakout_audio.c
extern void update_music();
extern void stop_sound();

// From breakout_graphics.c
extern void draw_rect(int x, int y, int width, int height, uint8_t color);
extern void draw_bricks();
extern void draw_balls();
extern void draw_paddle();
extern void draw_powerups();
extern void draw_lasers();
extern void draw_particles();

// From breakout_ui.c
extern void draw_hud();
extern void draw_level_start_screen();
extern void draw_turn_transition();
extern void draw_winner_screen();
extern void draw_countdown(int number);  // NEW: Countdown function

// Laser system (implemented below)
void shoot_laser(player_t* player);
void update_lasers();

/* ============================================================================
 * LASER SYSTEM
 * ============================================================================
 * Players can shoot lasers upward when they have the laser power-up.
 */

/*
 * shoot_laser - Fire a laser from the paddle
 * 
 * Creates a new laser projectile if the player has the laser power-up
 * and the cooldown has expired.
 */
void shoot_laser(player_t* player)
{
    // Check if player can shoot
    if (!player->has_laser || player->laser_cooldown > 0)
    {
        return;
    }
    
    // Find an inactive laser slot
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (!player->lasers[i].active)
        {
            // Fire laser from center of paddle
            player->lasers[i].x = player->paddle_x + player->paddle_width / 2;
            player->lasers[i].y = PADDLE_Y - 5;
            player->lasers[i].active = true;
            
            // Start cooldown
            player->laser_cooldown = PADDLE_LASER_COOLDOWN;
            
            // Play sound
            play_sound(1500, 30);
            
            return;
        }
    }
}

/*
 * update_lasers - Move lasers and check brick collisions
 * 
 * Lasers move upward and destroy bricks they hit.
 */
void update_lasers()
{
    player_t* player = &game.players[game.current_player];
    
    // Decrease cooldown
    if (player->laser_cooldown > 0)
    {
        player->laser_cooldown--;
    }
    
    // Update each laser
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (!player->lasers[i].active)
        {
            continue;
        }
        
        // Move laser upward
        player->lasers[i].y -= 5;
        
        // Check collision with bricks
        for (int row = 0; row < BRICK_ROWS; row++)
        {
            for (int col = 0; col < BRICK_COLS; col++)
            {
                if (game.bricks[row][col].health == 0)
                {
                    continue;
                }
                
                int brick_x = col * (BRICK_WIDTH + 2) + 5;
                int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y;
                
                // Simple collision check
                if (player->lasers[i].x >= brick_x && 
                    player->lasers[i].x < brick_x + BRICK_WIDTH &&
                    player->lasers[i].y >= brick_y && 
                    player->lasers[i].y < brick_y + BRICK_HEIGHT)
                {
                    // Hit a brick!
                    game.bricks[row][col].health--;
                    player->lasers[i].active = false;
                    
                    // Check if brick destroyed
                    if (game.bricks[row][col].health == 0)
                    {
                        // Award points
                        player->score += 10;
                        
                        // External functions we need
                        extern void spawn_explosion(int x, int y, uint8_t color);
                        extern void spawn_powerup(int x, int y);
                        
                        // Visual feedback
                        spawn_explosion(brick_x + BRICK_WIDTH/2, brick_y + BRICK_HEIGHT/2,
                                      levels[game.level].colors[row]);
                        spawn_powerup(brick_x, brick_y);
                    }
                    else
                    {
                        // Just damaged
                        game.bricks[row][col].shake_timer = 5;
                    }
                    
                    goto next_laser;
                }
            }
        }
        
        next_laser:
        
        // Remove laser if it went off top of screen
        if (player->lasers[i].y < 0)
        {
            player->lasers[i].active = false;
        }
    }
}

/* ============================================================================
 * INPUT HANDLING
 * ============================================================================
 */

/*
 * handle_input - Process keyboard input
 * 
 * Handles paddle movement, laser shooting, pause, etc.
 */
static void handle_input(key_event_t* event)
{
    if (!event->pressed)
    {
        return;  // Only respond to key presses, not releases
    }
    
    // If game over, only allow restart
    if (game.all_players_done)
    {
        if (event->scancode == 0x39)  // Space bar
        {
            breakout_init(game.num_players);
        }
        return;
    }
    
    player_t* player = &game.players[game.current_player];
    
    // Paddle movement - Left arrow or A key
    if (event->scancode == 0x4B || event->scancode == 0x1E)
    {
        player->paddle_x -= PADDLE_SPEED * 2;
        if (player->paddle_x < 0)
        {
            player->paddle_x = 0;
        }
    }
    
    // Paddle movement - Right arrow or D key
    if (event->scancode == 0x4D || event->scancode == 0x20)
    {
        player->paddle_x += PADDLE_SPEED * 2;
        if (player->paddle_x > VGA_WIDTH - player->paddle_width)
        {
            player->paddle_x = VGA_WIDTH - player->paddle_width;
        }
    }
    
    // Shoot laser - Left Ctrl
    if (event->scancode == 0x1D)
    {
        shoot_laser(player);
    }
    
    // Pause - P key
    if (event->scancode == 0x19)
    {
        game.paused = !game.paused;
    }
    
    // Toggle music - M key
    if (event->scancode == 0x32)
    {
        game.sound_enabled = !game.sound_enabled;
        if (!game.sound_enabled)
        {
            stop_sound();
        }
    }
}

/* ============================================================================
 * GAME INITIALIZATION
 * ============================================================================
 */

/*
 * breakout_init - Initialize the game
 * 
 * Sets up the initial state for a new game.
 * 
 * Parameters:
 *   num_players - 1 for single player, 2 for turn-based multiplayer
 */
void breakout_init(int num_players)
{
    // Set number of players (cap at MAX_PLAYERS)
    game.num_players = (num_players <= MAX_PLAYERS) ? num_players : 1;
    game.current_player = 0;  // Start with player 1
    game.level = 0;  // Start at level 1
    game.all_players_done = false;
    game.paused = false;
    game.sound_enabled = true;
    game.ball_speed_multiplier = 0;  // Normal speed
    game.music_note = 0;
    game.music_timer = 0;
    game.screen_shake_timer = 0;
    game.screen_shake_x = 0;
    game.screen_shake_y = 0;
    
    // Initialize both players
    for (int p = 0; p < MAX_PLAYERS; p++)
    {
        game.players[p].lives = 3;
        game.players[p].score = 0;
        game.players[p].paddle_width = PADDLE_WIDTH;
        game.players[p].paddle_x = VGA_WIDTH / 2 - PADDLE_WIDTH / 2;
        game.players[p].has_laser = false;
        game.players[p].laser_cooldown = 0;
        game.players[p].turn_complete = false;
        
        // Deactivate all lasers
        for (int i = 0; i < MAX_LASERS; i++)
        {
            game.players[p].lasers[i].active = false;
        }
    }
    
    // Initialize game objects
    init_bricks();
    init_balls();
    
    // Deactivate all power-ups
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        game.powerups[i].active = false;
    }
    
    // Deactivate all particles
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        game.particles[i].active = false;
    }
}

/* ============================================================================
 * MAIN GAME LOOP
 * ============================================================================
 */

/*
 * breakout_run - The main game loop
 * 
 * This is where the game actually runs! It:
 * 1. Handles input
 * 2. Updates game state (physics, collisions, etc.)
 * 3. Renders everything
 * 4. Manages transitions between states
 * 
 * Runs at 60 FPS (updates every 16ms)
 */
void breakout_run()
{
    uint32_t last_update = timer_get_ticks();
    uint32_t sound_timer = 0;
    
    // State machine flags
    bool showing_transition = false;
    uint32_t transition_start = 0;
    bool transition_drawn = false;
    
    bool winner_drawn = false;
    
    bool showing_level_start = true;
    bool level_start_drawn = false;
    uint32_t level_start_time = 0;  // NEW: Track when level screen started
    
    // NEW: Countdown state
    bool showing_countdown = false;
    int countdown_number = 3;
    uint32_t countdown_start = 0;
    bool countdown_drawn = false;
    
    // Initialize level start timer
    level_start_time = timer_get_ticks();
    
    // Main game loop - runs forever until ESC pressed
    while (1)
    {
        // ====================================================================
        // INPUT HANDLING
        // ====================================================================
        key_event_t event;
        while (keyboard_get_event(&event))
        {
            // ESC key - exit game
            if (event.pressed && event.scancode == 0x01)
            {
                stop_sound();
                return;
            }
            
            // Skip level start screen with any key
            if (event.pressed && showing_level_start)
            {
                showing_level_start = false;
                continue;
            }
            
            // Restart game after game over
            if (event.pressed && event.scancode == 0x39 && game.all_players_done)
            {
                breakout_init(game.num_players);
                winner_drawn = false;
                transition_drawn = false;
                showing_transition = false;
                showing_level_start = true;
                level_start_drawn = false;
                // Reset countdown
                showing_countdown = false;
                countdown_number = 3;
                continue;
            }
            
            handle_input(&event);
        }
        
        uint32_t current_ticks = timer_get_ticks();
        
        // ====================================================================
        // LEVEL START SCREEN
        // ====================================================================
        if (showing_level_start)
        {
            if (!level_start_drawn)
            {
                draw_level_start_screen();
                level_start_drawn = true;
                level_start_time = current_ticks;  // Start timer when screen is drawn
            }
            
            // Auto-advance after 3 seconds OR on any keypress
            if (current_ticks - level_start_time >= 3000)
            {
                showing_level_start = false;
                // Start countdown after level screen
                showing_countdown = true;
                countdown_number = 3;
                countdown_start = current_ticks;
                countdown_drawn = false;
            }
            continue;
        }
        
        // ====================================================================
        // COUNTDOWN (3-2-1-GO!)
        // ====================================================================
        if (showing_countdown)
        {
            // Redraw every second
            uint32_t elapsed = current_ticks - countdown_start;
            uint32_t seconds = elapsed / 1000;
            
            if (seconds == 0 && countdown_number == 3)
            {
                if (!countdown_drawn)
                {
                    draw_countdown(3);
                    play_sound(800, 100);
                    countdown_drawn = true;
                }
            }
            else if (seconds == 1 && countdown_number == 3)
            {
                countdown_number = 2;
                countdown_drawn = false;
            }
            else if (seconds == 1 && countdown_number == 2)
            {
                if (!countdown_drawn)
                {
                    draw_countdown(2);
                    play_sound(900, 100);
                    countdown_drawn = true;
                }
            }
            else if (seconds == 2 && countdown_number == 2)
            {
                countdown_number = 1;
                countdown_drawn = false;
            }
            else if (seconds == 2 && countdown_number == 1)
            {
                if (!countdown_drawn)
                {
                    draw_countdown(1);
                    play_sound(1000, 100);
                    countdown_drawn = true;
                }
            }
            else if (seconds == 3 && countdown_number == 1)
            {
                countdown_number = 0;
                countdown_drawn = false;
            }
            else if (seconds == 3 && countdown_number == 0)
            {
                if (!countdown_drawn)
                {
                    draw_countdown(0);  // GO!
                    play_sound(1200, 200);
                    countdown_drawn = true;
                }
            }
            else if (seconds >= 4)
            {
                // Countdown finished, start game!
                showing_countdown = false;
                last_update = current_ticks;
            }
            continue;
        }
        
        // ====================================================================
        // TURN SWITCHING (2-PLAYER MODE)
        // ====================================================================
        if (game.players[game.current_player].turn_complete && !showing_transition)
        {
            if (game.current_player < game.num_players - 1)
            {
                // Next player's turn
                game.current_player++;
                showing_transition = true;
                transition_start = current_ticks;
                transition_drawn = false;
                
                // Reset game state for next player
                init_bricks();
                init_balls();
                game.ball_speed_multiplier = 0;
                game.level = 0;  // Start from level 1
                
                // Clear power-ups
                for (int i = 0; i < MAX_POWERUPS; i++)
                {
                    game.powerups[i].active = false;
                }
            }
            else
            {
                // All players finished!
                game.all_players_done = true;
                winner_drawn = false;
            }
        }
        
        // ====================================================================
        // TURN TRANSITION SCREEN
        // ====================================================================
        if (showing_transition)
        {
            if (!transition_drawn)
            {
                draw_turn_transition();
                transition_drawn = true;
            }
            
            // Show for 2 seconds
            if (current_ticks - transition_start >= 2000)
            {
                showing_transition = false;
                showing_level_start = true;  // Show level screen for new player
                level_start_drawn = false;
                // Reset countdown for new player
                showing_countdown = false;
                countdown_number = 3;
            }
            continue;
        }
        
        // ====================================================================
        // WINNER SCREEN
        // ====================================================================
        if (game.all_players_done)
        {
            if (!winner_drawn)
            {
                draw_winner_screen();
                winner_drawn = true;
            }
            continue;  // Don't update game, just wait for restart
        }
        
        // ====================================================================
        // GAME UPDATE (60 FPS)
        // ====================================================================
        if (current_ticks - last_update >= 16)  // 16ms = ~60 FPS
        {
            last_update = current_ticks;
            
            if (!game.paused)
            {
                // Update all game systems
                update_balls();
                update_bricks();
                update_powerups();
                update_particles();
                update_lasers();
                
                // Check if level complete
                if (check_level_complete())
                {
                    game.level++;
                    
                    if (game.level >= MAX_LEVELS)
                    {
                        // Player beat all levels!
                        game.players[game.current_player].turn_complete = true;
                    }
                    else
                    {
                        // Next level
                        init_bricks();
                        init_balls();
                        showing_level_start = true;
                        level_start_drawn = false;
                        // Reset countdown for next level
                        showing_countdown = false;
                        countdown_number = 3;
                    }
                }
                
                // Update screen shake
                if (game.screen_shake_timer > 0)
                {
                    game.screen_shake_timer--;
                    
                    // Random shake offset
                    extern int random_range(int min, int max);
                    game.screen_shake_x = random_range(-2, 2);
                    game.screen_shake_y = random_range(-2, 2);
                }
                else
                {
                    game.screen_shake_x = 0;
                    game.screen_shake_y = 0;
                }
                
                // Update music
                sound_timer++;
                if (sound_timer >= 5)
                {
                    sound_timer = 0;
                    update_music();
                }
            }
            
            // ================================================================
            // RENDER EVERYTHING
            // ================================================================
            vga_clear(0);  // Clear screen to black
            draw_bricks();
            draw_paddle();
            draw_balls();
            draw_powerups();
            draw_lasers();
            draw_particles();
            draw_hud();
        }
    }
}