/*
 * breakout_powerups.c - Power-up system
 * 
 * This file handles the power-up system. When bricks are destroyed, there's
 * a 30% chance a power-up will drop. The player must catch it with their
 * paddle to get the effect.
 * 
 * Power-ups include:
 * - Multi-ball: 3 balls at once!
 * - Expand/Shrink paddle: Changes difficulty
 * - Laser: Shoot bricks
 * - Slow/Fast ball: Changes speed
 * - Extra life: One more chance
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

// External references
extern game_state_t game;
extern int random_range(int min, int max);
extern void spawn_explosion(int x, int y, uint8_t color);
extern void play_sound(int frequency, int duration_ms);

/*
 * spawn_powerup - Maybe create a power-up at given position
 * 
 * Called when a brick is destroyed. There's only a 30% chance of actually
 * spawning a power-up (otherwise the game would be too easy!).
 * 
 * Parameters:
 *   x, y - Position where brick was destroyed
 */
void spawn_powerup(int x, int y)
{
    // Only 30% chance to spawn - check random number 0-100
    if (random_range(0, 100) > 30)
    {
        return;  // No power-up this time
    }
    
    // Look for an empty power-up slot
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!game.powerups[i].active)
        {
            // Found a slot! Initialize the power-up
            game.powerups[i].x = x;
            game.powerups[i].y = y;
            
            // Choose random power-up type (skip POWERUP_NONE)
            game.powerups[i].type = random_range(1, POWERUP_COUNT - 1);
            game.powerups[i].active = true;
            
            // Set color based on power-up type (for visual identification)
            switch (game.powerups[i].type)
            {
                case POWERUP_MULTIBALL:
                    game.powerups[i].color = 14;  // Yellow
                    break;
                case POWERUP_EXPAND_PADDLE:
                    game.powerups[i].color = 2;   // Green (good!)
                    break;
                case POWERUP_SHRINK_PADDLE:
                    game.powerups[i].color = 4;   // Red (bad!)
                    break;
                case POWERUP_LASER:
                    game.powerups[i].color = 9;   // Light blue
                    break;
                case POWERUP_SLOW_BALL:
                    game.powerups[i].color = 11;  // Cyan
                    break;
                case POWERUP_EXTRA_LIFE:
                    game.powerups[i].color = 13;  // Pink
                    break;
                case POWERUP_FAST_BALL:
                    game.powerups[i].color = 12;  // Light red
                    break;
                default:
                    game.powerups[i].color = 15;  // White (fallback)
                    break;
            }
            
            return;  // Done!
        }
    }
    
    // All power-up slots full - oh well, no power-up this time
}

/*
 * check_collision - Simple rectangle collision detection
 * 
 * Checks if two rectangles overlap. Used to detect when the paddle
 * catches a power-up.
 * 
 * Returns:
 *   true if rectangles overlap, false otherwise
 */
static bool check_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2);
}

/*
 * update_powerups - Update all active power-ups
 * 
 * This makes power-ups fall down and checks if the player caught them.
 * Called every frame from the main game loop.
 */
void update_powerups()
{
    player_t* player = &game.players[game.current_player];
    
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        // Skip inactive power-ups
        if (!game.powerups[i].active)
        {
            continue;
        }
        
        // Make power-up fall
        game.powerups[i].y += POWERUP_FALL_SPEED;
        
        // Check if paddle caught the power-up
        if (check_collision(game.powerups[i].x, game.powerups[i].y, 
                          POWERUP_SIZE, POWERUP_SIZE,
                          player->paddle_x, PADDLE_Y, 
                          player->paddle_width, PADDLE_HEIGHT))
        {
            // Power-up caught! Apply the effect
            switch (game.powerups[i].type)
            {
                case POWERUP_MULTIBALL:
                    // Activate additional balls
                    for (int b = 1; b < MAX_BALLS; b++)
                    {
                        if (!game.balls[b].active)
                        {
                            // Copy first ball's state
                            game.balls[b] = game.balls[0];
                            game.balls[b].active = true;
                            
                            // Give it a random horizontal direction
                            game.balls[b].dx = random_range(-3, 3);
                            if (game.balls[b].dx == 0)
                            {
                                game.balls[b].dx = 2;  // Make sure it moves
                            }
                        }
                    }
                    play_sound(800, 100);  // High pitch for multi-ball
                    break;
                    
                case POWERUP_EXPAND_PADDLE:
                    // Make paddle bigger (easier to hit ball)
                    player->paddle_width += 20;
                    if (player->paddle_width > 80)
                    {
                        player->paddle_width = 80;  // Cap maximum size
                    }
                    play_sound(600, 100);
                    break;
                    
                case POWERUP_SHRINK_PADDLE:
                    // Make paddle smaller (harder to hit ball)
                    player->paddle_width -= 10;
                    if (player->paddle_width < 20)
                    {
                        player->paddle_width = 20;  // Minimum size
                    }
                    play_sound(400, 100);  // Lower pitch for bad power-up
                    break;
                    
                case POWERUP_LASER:
                    // Enable laser shooting
                    player->has_laser = true;
                    play_sound(1000, 100);
                    break;
                    
                case POWERUP_SLOW_BALL:
                    // Slow down the ball (makes game easier)
                    game.ball_speed_multiplier = -1;
                    play_sound(300, 100);
                    break;
                    
                case POWERUP_FAST_BALL:
                    // Speed up the ball (makes game harder)
                    game.ball_speed_multiplier = 1;
                    play_sound(900, 100);
                    break;
                    
                case POWERUP_EXTRA_LIFE:
                    // Give player an extra life!
                    player->lives++;
                    play_sound(1200, 100);  // High pitch for good power-up
                    break;
                    
                default:
                    break;
            }
            
            // Deactivate the power-up and show explosion effect
            game.powerups[i].active = false;
            spawn_explosion(game.powerups[i].x, game.powerups[i].y, 
                          game.powerups[i].color);
        }
        
        // Remove power-up if it fell off the bottom of the screen
        if (game.powerups[i].y > VGA_HEIGHT)
        {
            game.powerups[i].active = false;
        }
    }
}