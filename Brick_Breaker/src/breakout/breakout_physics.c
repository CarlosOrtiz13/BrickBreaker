/*
 * breakout_physics.c - Game physics and collision detection
 * 
 * This is the "brain" of the game! It handles:
 * - Ball movement and bouncing
 * - Collision detection (ball vs. bricks, paddle, walls)
 * - Brick damage and destruction
 * - Level completion checking
 * 
 * The physics runs at 60 FPS, so smooth movement!
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

// External references
extern game_state_t game;
extern void spawn_particle(int x, int y, int dx, int dy, uint8_t color, int life);
extern void spawn_explosion(int x, int y, uint8_t color);
extern void spawn_powerup(int x, int y);
extern void play_sound(int frequency, int duration_ms);
extern int random_range(int min, int max);

// Level definitions (in breakout_main.c)
extern level_t levels[MAX_LEVELS];

/*
 * check_collision - Rectangle collision detection
 * 
 * This is a classic Axis-Aligned Bounding Box (AABB) collision test.
 * Two rectangles collide if they overlap on both X and Y axes.
 * 
 * Returns:
 *   true if rectangles overlap, false otherwise
 */
bool check_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 > x2 &&  // Right edge of rect1 past left edge of rect2
            x1 < x2 + w2 &&  // Left edge of rect1 before right edge of rect2
            y1 + h1 > y2 &&  // Bottom edge of rect1 past top edge of rect2
            y1 < y2 + h2);   // Top edge of rect1 before bottom edge of rect2
}

/*
 * init_bricks - Set up bricks for current level
 * 
 * Reads the level pattern and creates bricks accordingly.
 * Some positions might be empty (0), others have bricks with 1-3 health.
 */
void init_bricks()
{
    level_t* level = &levels[game.level];
    
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            // Copy health from level pattern
            game.bricks[row][col].health = level->pattern[row][col];
            
            // Reset animation state
            game.bricks[row][col].shake_x = 0;
            game.bricks[row][col].shake_y = 0;
            game.bricks[row][col].shake_timer = 0;
        }
    }
}

/*
 * update_bricks - Update brick animations
 * 
 * When a brick is hit, it shakes for a few frames to give visual feedback.
 * This function updates those shake animations.
 */
void update_bricks()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            // If brick is shaking, apply random offset
            if (game.bricks[row][col].shake_timer > 0)
            {
                game.bricks[row][col].shake_timer--;
                
                // Random offset for shake effect
                game.bricks[row][col].shake_x = random_range(-2, 2);
                game.bricks[row][col].shake_y = random_range(-1, 1);
            }
            else
            {
                // No shake, return to normal position
                game.bricks[row][col].shake_x = 0;
                game.bricks[row][col].shake_y = 0;
            }
        }
    }
}

/*
 * check_level_complete - Are all bricks destroyed?
 * 
 * Checks if any bricks remain. If not, level is complete!
 * 
 * Returns:
 *   true if all bricks destroyed, false otherwise
 */
bool check_level_complete()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            if (game.bricks[row][col].health > 0)
            {
                return false;  // Found a brick, not complete
            }
        }
    }
    return true;  // No bricks remaining!
}

/*
 * init_balls - Reset balls (usually after losing a life)
 * 
 * Deactivates all balls and spawns a new one in the center.
 */
void init_balls()
{
    // Deactivate all balls
    for (int i = 0; i < MAX_BALLS; i++)
    {
        game.balls[i].active = false;
    }
    
    // Activate first ball in center of screen
    game.balls[0].active = true;
    game.balls[0].x = VGA_WIDTH / 2;
    game.balls[0].y = VGA_HEIGHT / 2;
    game.balls[0].dx = BALL_SPEED;
    game.balls[0].dy = -BALL_SPEED;  // Start going upward
    game.balls[0].trail_index = 0;
}

/*
 * update_balls - Update all ball physics
 * 
 * This is the heart of the game! It handles:
 * - Ball movement
 * - Wall collisions
 * - Paddle collisions (with "english" - spin based on hit location)
 * - Brick collisions
 * - Losing a life when ball falls off bottom
 * 
 * Called every frame.
 */
void update_balls()
{
    bool any_active = false;  // Track if any ball is still in play
    player_t* player = &game.players[game.current_player];
    
    for (int i = 0; i < MAX_BALLS; i++)
    {
        // Skip inactive balls
        if (!game.balls[i].active)
        {
            continue;
        }
        
        any_active = true;
        
        // Update motion trail (circular buffer of last 10 positions)
        game.balls[i].trail_x[game.balls[i].trail_index] = game.balls[i].x;
        game.balls[i].trail_y[game.balls[i].trail_index] = game.balls[i].y;
        game.balls[i].trail_index = (game.balls[i].trail_index + 1) % 10;
        
        // Calculate speed (can be modified by power-ups)
        int speed_x = game.balls[i].dx;
        int speed_y = game.balls[i].dy;
        
        // Handle slow-motion power-up (skip every other frame)
        if (game.ball_speed_multiplier < 0)
        {
            static int slow_counter = 0;
            slow_counter++;
            if (slow_counter % 2 == 0)
            {
                continue;  // Skip this frame
            }
        }
        // Handle fast power-up (double speed)
        else if (game.ball_speed_multiplier > 0)
        {
            speed_x *= 2;
            speed_y *= 2;
        }
        
        // Move ball
        game.balls[i].x += speed_x;
        game.balls[i].y += speed_y;
        
        // ====================================================================
        // COLLISION: Left and right walls
        // ====================================================================
        if (game.balls[i].x <= 0 || game.balls[i].x >= VGA_WIDTH - BALL_SIZE)
        {
            game.balls[i].dx = -game.balls[i].dx;  // Reverse horizontal direction
            play_sound(400, 30);
        }
        
        // ====================================================================
        // COLLISION: Top wall
        // ====================================================================
        if (game.balls[i].y <= 0)
        {
            game.balls[i].dy = -game.balls[i].dy;  // Reverse vertical direction
            play_sound(400, 30);
        }
        
        // ====================================================================
        // COLLISION: Bottom (ball fell off = lose life)
        // ====================================================================
        if (game.balls[i].y >= VGA_HEIGHT)
        {
            game.balls[i].active = false;
            continue;  // Don't process rest of this ball
        }
        
        // ====================================================================
        // COLLISION: Paddle
        // ====================================================================
        if (check_collision(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE,
                          player->paddle_x, PADDLE_Y, player->paddle_width, PADDLE_HEIGHT))
        {
            // Bounce ball upward (always make dy negative)
            game.balls[i].dy = -abs(game.balls[i].dy);
            
            // Add "english" - change horizontal direction based on where ball hit
            // This gives player more control!
            int paddle_center = player->paddle_x + player->paddle_width / 2;
            int ball_center = game.balls[i].x + BALL_SIZE / 2;
            int offset = ball_center - paddle_center;
            
            if (offset < -10)
            {
                game.balls[i].dx = -BALL_SPEED;  // Hit left side = go left
            }
            else if (offset > 10)
            {
                game.balls[i].dx = BALL_SPEED;   // Hit right side = go right
            }
            // If hit center, keep current direction
            
            play_sound(600, 30);
            
            // Spawn sparkle particles on paddle hit
            spawn_particle(game.balls[i].x, game.balls[i].y, 0, -2, 15, 10);
            spawn_particle(game.balls[i].x, game.balls[i].y, 1, -2, 14, 10);
            spawn_particle(game.balls[i].x, game.balls[i].y, -1, -2, 14, 10);
        }
        
        // ====================================================================
        // COLLISION: Bricks
        // ====================================================================
        for (int row = 0; row < BRICK_ROWS; row++)
        {
            for (int col = 0; col < BRICK_COLS; col++)
            {
                // Skip destroyed bricks
                if (game.bricks[row][col].health == 0)
                {
                    continue;
                }
                
                // Calculate brick position
                int brick_x = col * (BRICK_WIDTH + 2) + 5;
                int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y;
                
                // Check collision
                if (check_collision(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE,
                                  brick_x, brick_y, BRICK_WIDTH, BRICK_HEIGHT))
                {
                    // Hit a brick! Damage it
                    game.bricks[row][col].health--;
                    
                    // Bounce ball
                    game.balls[i].dy = -game.balls[i].dy;
                    
                    // Check if brick was destroyed
                    if (game.bricks[row][col].health == 0)
                    {
                        // Award points
                        player->score += 10;
                        
                        // Visual feedback
                        spawn_explosion(brick_x + BRICK_WIDTH/2, brick_y + BRICK_HEIGHT/2,
                                      levels[game.level].colors[row]);
                        
                        // Maybe spawn a power-up
                        spawn_powerup(brick_x, brick_y);
                        
                        // Screen shake for impact feel
                        game.screen_shake_timer = 3;
                    }
                    else
                    {
                        // Brick damaged but not destroyed - make it shake
                        game.bricks[row][col].shake_timer = 5;
                        play_sound(300, 30);
                    }
                    
                    // Only process one brick collision per frame
                    // (prevents ball from hitting multiple bricks at once)
                    goto next_ball;
                }
            }
        }
        
        next_ball:;  // Label for breaking out of nested loops
    }
    
    // ========================================================================
    // Check if all balls are gone (player loses a life)
    // ========================================================================
    if (!any_active)
    {
        player->lives--;
        
        // If player still has lives, spawn a new ball
        if (player->lives > 0)
        {
            init_balls();
        }
        else
        {
            // No lives left - turn is over!
            player->turn_complete = true;
        }
    }
}