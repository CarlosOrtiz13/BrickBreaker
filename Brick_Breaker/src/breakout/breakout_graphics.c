/*
 * breakout_graphics.c - Rendering system
 * 
 * This file draws everything you see on screen:
 * - Bricks with 3D gradient effect
 * - Balls with motion trails
 * - Paddle with laser indicators
 * - Power-ups falling
 * - Laser beams
 * - Particle explosions
 * 
 * All drawing respects screen shake for impact effects!
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

// External references
extern game_state_t game;
extern level_t levels[MAX_LEVELS];

/* ============================================================================
 * HELPER DRAWING FUNCTIONS
 * ============================================================================
 */

/*
 * draw_rect - Draw a filled rectangle with screen shake
 * 
 * This is our basic drawing primitive. Everything builds on this!
 * Screen shake offset is automatically applied for cool effects.
 */
void draw_rect(int x, int y, int width, int height, uint8_t color)
{
    // Apply screen shake offset
    x += game.screen_shake_x;
    y += game.screen_shake_y;
    
    // Draw each pixel in the rectangle
    for (int dy = 0; dy < height; dy++)
    {
        for (int dx = 0; dx < width; dx++)
        {
            int px = x + dx;
            int py = y + dy;
            
            // Bounds checking (don't draw off-screen)
            if (px >= 0 && px < VGA_WIDTH && py >= 0 && py < VGA_HEIGHT)
            {
                vga_set_pixel(px, py, color);
            }
        }
    }
}

/*
 * draw_pixel - Draw a single pixel with screen shake
 */
void draw_pixel(int x, int y, uint8_t color)
{
    x += game.screen_shake_x;
    y += game.screen_shake_y;
    
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT)
    {
        vga_set_pixel(x, y, color);
    }
}

/* ============================================================================
 * BRICK RENDERING
 * ============================================================================
 */

/*
 * draw_bricks - Draw all bricks with 3D gradient effect
 * 
 * Each brick is drawn in 3 horizontal layers:
 * - Top layer: Lighter shade (highlight)
 * - Middle layer: Base color
 * - Bottom layer: Darker shade (shadow)
 * 
 * This creates a nice 3D look! Damaged bricks are darker.
 */
void draw_bricks()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            // Skip destroyed bricks
            if (game.bricks[row][col].health == 0)
            {
                continue;
            }
            
            // Calculate position (include shake offset if brick is shaking)
            int brick_x = col * (BRICK_WIDTH + 2) + 5 + game.bricks[row][col].shake_x;
            int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y + game.bricks[row][col].shake_y;
            
            // Get base color from level
            uint8_t base_color = levels[game.level].colors[row];
            
            // Damaged bricks look darker
            if (game.bricks[row][col].health == 1)
            {
                base_color = 8;  // Dark gray
            }
            
            // Calculate gradient colors for 3D effect
            uint8_t light_color = base_color;
            uint8_t dark_color = base_color;
            
            // Map base colors to lighter variants
            if (base_color == 4) light_color = 12;  // Red -> Light red
            if (base_color == 2) light_color = 10;  // Green -> Light green
            if (base_color == 1) light_color = 9;   // Blue -> Light blue
            
            // Map to darker variants
            if (base_color > 8)
            {
                dark_color = base_color - 4;
            }
            
            // Draw 3 horizontal layers for gradient
            draw_rect(brick_x, brick_y, BRICK_WIDTH, 3, light_color);      // Top (light)
            draw_rect(brick_x, brick_y + 3, BRICK_WIDTH, 4, base_color);   // Middle
            draw_rect(brick_x, brick_y + 7, BRICK_WIDTH, 3, dark_color);   // Bottom (dark)
            
            // Draw black border for definition
            for (int i = 0; i < BRICK_WIDTH; i++)
            {
                draw_pixel(brick_x + i, brick_y, 0);
                draw_pixel(brick_x + i, brick_y + BRICK_HEIGHT - 1, 0);
            }
            for (int i = 0; i < BRICK_HEIGHT; i++)
            {
                draw_pixel(brick_x, brick_y + i, 0);
                draw_pixel(brick_x + BRICK_WIDTH - 1, brick_y + i, 0);
            }
            
            // Draw health indicator dots (shows hits remaining)
            for (int h = 0; h < game.bricks[row][col].health && h < 3; h++)
            {
                int dot_x = brick_x + 3 + h * 4;
                int dot_y = brick_y + 2;
                
                // Draw 2x2 pixel dot (white)
                draw_pixel(dot_x, dot_y, 15);
                draw_pixel(dot_x + 1, dot_y, 15);
                draw_pixel(dot_x, dot_y + 1, 15);
                draw_pixel(dot_x + 1, dot_y + 1, 15);
            }
        }
    }
}

/* ============================================================================
 * BALL RENDERING
 * ============================================================================
 */

/*
 * draw_balls - Draw all active balls with motion trails
 * 
 * Each ball has a fading trail showing where it's been.
 * The trail is stored as a circular buffer of the last 10 positions.
 */
void draw_balls()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (!game.balls[i].active)
        {
            continue;
        }
        
        // Draw trail (oldest positions are darker)
        for (int t = 0; t < 10; t++)
        {
            // Calculate fade color (older = darker)
            int alpha = t * 2;
            if (alpha > 8) alpha = 8;
            uint8_t trail_color = 8 + alpha;  // Dark gray to light gray gradient
            
            draw_pixel(game.balls[i].trail_x[t], game.balls[i].trail_y[t], trail_color);
        }
        
        // Draw main ball (white with yellow highlight)
        draw_rect(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE, 15);
        
        // Add highlight pixel for 3D look
        draw_pixel(game.balls[i].x + 1, game.balls[i].y + 1, 14);
    }
}

/* ============================================================================
 * PADDLE RENDERING
 * ============================================================================
 */

/*
 * draw_paddle - Draw the current player's paddle
 * 
 * Color changes based on which player's turn it is.
 * Shows laser indicators if laser power-up is active.
 */
void draw_paddle()
{
    player_t* player = &game.players[game.current_player];
    
    // Different color per player
    uint8_t color = (game.current_player == 0) ? 15 : 11;  // White or cyan
    
    // Draw paddle rectangle
    draw_rect(player->paddle_x, PADDLE_Y, player->paddle_width, PADDLE_HEIGHT, color);
    
    // Draw laser indicators if laser power-up is active
    if (player->has_laser)
    {
        // Small green rectangles on sides of paddle
        draw_rect(player->paddle_x + 2, PADDLE_Y - 3, 3, 2, 10);
        draw_rect(player->paddle_x + player->paddle_width - 5, PADDLE_Y - 3, 3, 2, 10);
    }
}

/* ============================================================================
 * POWER-UP RENDERING
 * ============================================================================
 */

/*
 * draw_powerups - Draw falling power-ups
 * 
 * Each power-up has a unique color so players can identify them.
 */
void draw_powerups()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!game.powerups[i].active)
        {
            continue;
        }
        
        // Draw power-up box
        draw_rect(game.powerups[i].x, game.powerups[i].y, 
                 POWERUP_SIZE, POWERUP_SIZE, game.powerups[i].color);
        
        // Draw white border
        for (int j = 0; j < POWERUP_SIZE; j++)
        {
            draw_pixel(game.powerups[i].x + j, game.powerups[i].y, 15);
            draw_pixel(game.powerups[i].x + j, game.powerups[i].y + POWERUP_SIZE - 1, 15);
            draw_pixel(game.powerups[i].x, game.powerups[i].y + j, 15);
            draw_pixel(game.powerups[i].x + POWERUP_SIZE - 1, game.powerups[i].y + j, 15);
        }
        
        // Draw simple icon (black cross in center)
        int cx = game.powerups[i].x + POWERUP_SIZE / 2;
        int cy = game.powerups[i].y + POWERUP_SIZE / 2;
        draw_pixel(cx, cy, 0);
        draw_pixel(cx - 1, cy, 0);
        draw_pixel(cx + 1, cy, 0);
        draw_pixel(cx, cy - 1, 0);
        draw_pixel(cx, cy + 1, 0);
    }
}

/* ============================================================================
 * LASER RENDERING
 * ============================================================================
 */

/*
 * draw_lasers - Draw laser beams
 * 
 * Lasers are green with a white core for a beam effect.
 */
void draw_lasers()
{
    player_t* player = &game.players[game.current_player];
    
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (!player->lasers[i].active)
        {
            continue;
        }
        
        // Draw laser beam (2 pixels wide, 5 pixels tall)
        draw_rect(player->lasers[i].x, player->lasers[i].y, 2, 5, 10);  // Light green
        draw_rect(player->lasers[i].x, player->lasers[i].y, 1, 5, 15);  // White core
    }
}

/* ============================================================================
 * PARTICLE RENDERING
 * ============================================================================
 */

/*
 * draw_particles - Draw explosion particles
 * 
 * Particles fade to gray as they die.
 */
void draw_particles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!game.particles[i].active)
        {
            continue;
        }
        
        // Fade color when particle is dying
        uint8_t color = game.particles[i].color;
        if (game.particles[i].life < 5)
        {
            color = 8;  // Fade to dark gray
        }
        
        // Draw particle (2 pixels for visibility)
        draw_pixel(game.particles[i].x, game.particles[i].y, color);
        draw_pixel(game.particles[i].x + 1, game.particles[i].y, color);
    }
}