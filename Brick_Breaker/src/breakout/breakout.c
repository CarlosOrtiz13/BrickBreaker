#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"
#include "io/io.h"

// Game state
static game_state_t game;

// Level definitions
static level_t levels[MAX_LEVELS] = {
    {
        .pattern = {
            {1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1,1,1,1,1}
        },
        .colors = {4, 12, 14, 2, 1},
        .ball_speed = 2,
        .name = "LEVEL 1"
    },
    {
        .pattern = {
            {2,0,2,0,2,0,2,0,2,0,2,0},
            {0,2,0,2,0,2,0,2,0,2,0,2},
            {2,0,2,0,2,0,2,0,2,0,2,0},
            {0,2,0,2,0,2,0,2,0,2,0,2},
            {2,0,2,0,2,0,2,0,2,0,2,0}
        },
        .colors = {12, 12, 14, 14, 2},
        .ball_speed = 3,
        .name = "LEVEL 2"
    },
    {
        .pattern = {
            {0,0,0,0,0,3,3,0,0,0,0,0},
            {0,0,0,0,2,2,2,2,0,0,0,0},
            {0,0,0,2,2,2,2,2,2,0,0,0},
            {0,0,2,2,2,2,2,2,2,2,0,0},
            {0,2,2,2,2,2,2,2,2,2,2,0}
        },
        .colors = {4, 12, 14, 2, 1},
        .ball_speed = 3,
        .name = "LEVEL 3"
    },
    {
        .pattern = {
            {3,3,3,3,3,3,3,3,3,3,3,3},
            {3,0,0,3,3,3,3,3,3,0,0,3},
            {3,3,3,3,3,3,3,3,3,3,3,3},
            {3,0,0,3,3,3,3,3,3,0,0,3},
            {3,3,3,3,3,3,3,3,3,3,3,3}
        },
        .colors = {4, 4, 12, 12, 14},
        .ball_speed = 4,
        .name = "BOSS"
    }
};

// ============================================================================
// SOUND SYSTEM
// ============================================================================

void play_sound(int frequency, int duration_ms)
{
    if (!game.sound_enabled || frequency == 0) return;
    
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(divisor & 0xFF));
    outb(0x42, (uint8_t)((divisor >> 8) & 0xFF));
    
    uint8_t tmp = insb(0x61);
    outb(0x61, tmp | 0x03);
}

void stop_sound()
{
    uint8_t tmp = insb(0x61);
    outb(0x61, tmp & 0xFC);
}

void update_music()
{
    if (!game.sound_enabled) return;
    
    static const int melody[] = {523, 587, 659, 698, 784, 698, 659, 587};
    static const int melody_len = 8;
    
    game.music_timer++;
    if (game.music_timer >= 30)
    {
        game.music_timer = 0;
        game.music_note = (game.music_note + 1) % melody_len;
        play_sound(melody[game.music_note], 100);
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static void vga_fill_rect(int x, int y, int width, int height, uint8_t color)
{
    x += game.screen_shake_x;
    y += game.screen_shake_y;
    
    for (int dy = 0; dy < height; dy++)
    {
        for (int dx = 0; dx < width; dx++)
        {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < VGA_WIDTH && py >= 0 && py < VGA_HEIGHT)
            {
                vga_set_pixel(px, py, color);
            }
        }
    }
}

static void vga_set_pixel_shake(int x, int y, uint8_t color)
{
    x += game.screen_shake_x;
    y += game.screen_shake_y;
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT)
    {
        vga_set_pixel(x, y, color);
    }
}

static bool check_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2);
}

static int random_range(int min, int max)
{
    static uint32_t seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return min + (seed % (max - min + 1));
}

// ============================================================================
// PARTICLE SYSTEM
// ============================================================================

static void spawn_particle(int x, int y, int dx, int dy, uint8_t color, int life)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!game.particles[i].active)
        {
            game.particles[i].x = x;
            game.particles[i].y = y;
            game.particles[i].dx = dx;
            game.particles[i].dy = dy;
            game.particles[i].color = color;
            game.particles[i].life = life;
            game.particles[i].active = true;
            return;
        }
    }
}

static void spawn_explosion(int x, int y, uint8_t color)
{
    for (int i = 0; i < 8; i++)
    {
        int dx = (i % 3) - 1;
        int dy = (i / 3) - 1;
        if (dx == 0 && dy == 0) dy = -1;
        spawn_particle(x, y, dx * 2, dy * 2, color, 15);
    }
    play_sound(200 + random_range(0, 100), 50);
}

static void update_particles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game.particles[i].active)
        {
            game.particles[i].x += game.particles[i].dx;
            game.particles[i].y += game.particles[i].dy;
            game.particles[i].dy += 1;
            game.particles[i].life--;
            
            if (game.particles[i].life <= 0)
            {
                game.particles[i].active = false;
            }
        }
    }
}

static void draw_particles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game.particles[i].active)
        {
            uint8_t color = game.particles[i].color;
            if (game.particles[i].life < 5) color = 8;
            
            vga_set_pixel_shake(game.particles[i].x, game.particles[i].y, color);
            vga_set_pixel_shake(game.particles[i].x + 1, game.particles[i].y, color);
        }
    }
}

// Continued in next part...

// ============================================================================
// POWER-UP SYSTEM
// ============================================================================

static void spawn_powerup(int x, int y)
{
    if (random_range(0, 100) > 30) return;
    
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (!game.powerups[i].active)
        {
            game.powerups[i].x = x;
            game.powerups[i].y = y;
            game.powerups[i].type = random_range(1, POWERUP_COUNT - 1);
            game.powerups[i].active = true;
            
            switch (game.powerups[i].type)
            {
                case POWERUP_MULTIBALL: game.powerups[i].color = 14; break;
                case POWERUP_EXPAND_PADDLE: game.powerups[i].color = 2; break;
                case POWERUP_SHRINK_PADDLE: game.powerups[i].color = 4; break;
                case POWERUP_LASER: game.powerups[i].color = 9; break;
                case POWERUP_SLOW_BALL: game.powerups[i].color = 11; break;
                case POWERUP_EXTRA_LIFE: game.powerups[i].color = 13; break;
                case POWERUP_FAST_BALL: game.powerups[i].color = 12; break;
                default: game.powerups[i].color = 15; break;
            }
            return;
        }
    }
}

static void update_powerups()
{
    player_t* player = &game.players[game.current_player];
    
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game.powerups[i].active)
        {
            game.powerups[i].y += POWERUP_FALL_SPEED;
            
            if (check_collision(game.powerups[i].x, game.powerups[i].y, POWERUP_SIZE, POWERUP_SIZE,
                              player->paddle_x, PADDLE_Y, player->paddle_width, PADDLE_HEIGHT))
            {
                switch (game.powerups[i].type)
                {
                    case POWERUP_MULTIBALL:
                        for (int b = 1; b < MAX_BALLS; b++)
                        {
                            if (!game.balls[b].active)
                            {
                                game.balls[b] = game.balls[0];
                                game.balls[b].active = true;
                                game.balls[b].dx = random_range(-3, 3);
                                if (game.balls[b].dx == 0) game.balls[b].dx = 2;
                            }
                        }
                        play_sound(800, 100);
                        break;
                        
                    case POWERUP_EXPAND_PADDLE:
                        player->paddle_width += 20;
                        if (player->paddle_width > 80) player->paddle_width = 80;
                        play_sound(600, 100);
                        break;
                        
                    case POWERUP_SHRINK_PADDLE:
                        player->paddle_width -= 10;
                        if (player->paddle_width < 20) player->paddle_width = 20;
                        play_sound(400, 100);
                        break;
                        
                    case POWERUP_LASER:
                        player->has_laser = true;
                        play_sound(1000, 100);
                        break;
                        
                    case POWERUP_SLOW_BALL:
                        game.ball_speed_multiplier = -1;
                        play_sound(300, 100);
                        break;
                        
                    case POWERUP_FAST_BALL:
                        game.ball_speed_multiplier = 1;
                        play_sound(900, 100);
                        break;
                        
                    case POWERUP_EXTRA_LIFE:
                        player->lives++;
                        play_sound(1200, 100);
                        break;
                        
                    default:
                        break;
                }
                
                game.powerups[i].active = false;
                spawn_explosion(game.powerups[i].x, game.powerups[i].y, game.powerups[i].color);
            }
            
            if (game.powerups[i].y > VGA_HEIGHT)
            {
                game.powerups[i].active = false;
            }
        }
    }
}

static void draw_powerups()
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        if (game.powerups[i].active)
        {
            vga_fill_rect(game.powerups[i].x, game.powerups[i].y, POWERUP_SIZE, POWERUP_SIZE, 
                         game.powerups[i].color);
            
            for (int j = 0; j < POWERUP_SIZE; j++)
            {
                vga_set_pixel_shake(game.powerups[i].x + j, game.powerups[i].y, 15);
                vga_set_pixel_shake(game.powerups[i].x + j, game.powerups[i].y + POWERUP_SIZE - 1, 15);
                vga_set_pixel_shake(game.powerups[i].x, game.powerups[i].y + j, 15);
                vga_set_pixel_shake(game.powerups[i].x + POWERUP_SIZE - 1, game.powerups[i].y + j, 15);
            }
        }
    }
}

// ============================================================================
// LASER SYSTEM
// ============================================================================

static void shoot_laser(player_t* player)
{
    if (!player->has_laser || player->laser_cooldown > 0) return;
    
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (!player->lasers[i].active)
        {
            player->lasers[i].x = player->paddle_x + player->paddle_width / 2;
            player->lasers[i].y = PADDLE_Y - 5;
            player->lasers[i].active = true;
            player->laser_cooldown = PADDLE_LASER_COOLDOWN;
            play_sound(1500, 30);
            return;
        }
    }
}

static void update_lasers()
{
    player_t* player = &game.players[game.current_player];
    
    if (player->laser_cooldown > 0) player->laser_cooldown--;
    
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (player->lasers[i].active)
        {
            player->lasers[i].y -= 5;
            
            for (int row = 0; row < BRICK_ROWS; row++)
            {
                for (int col = 0; col < BRICK_COLS; col++)
                {
                    if (game.bricks[row][col].health > 0)
                    {
                        int brick_x = col * (BRICK_WIDTH + 2) + 5;
                        int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y;
                        
                        if (check_collision(player->lasers[i].x, player->lasers[i].y, 2, 5,
                                          brick_x, brick_y, BRICK_WIDTH, BRICK_HEIGHT))
                        {
                            game.bricks[row][col].health--;
                            player->lasers[i].active = false;
                            
                            if (game.bricks[row][col].health == 0)
                            {
                                player->score += 10;
                                spawn_explosion(brick_x + BRICK_WIDTH/2, brick_y + BRICK_HEIGHT/2,
                                              levels[game.level].colors[row]);
                                spawn_powerup(brick_x, brick_y);
                            }
                            else
                            {
                                game.bricks[row][col].shake_timer = 5;
                            }
                            goto next_laser;
                        }
                    }
                }
            }
            
            next_laser:
            if (player->lasers[i].y < 0)
            {
                player->lasers[i].active = false;
            }
        }
    }
}

static void draw_lasers()
{
    player_t* player = &game.players[game.current_player];
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (player->lasers[i].active)
        {
            vga_fill_rect(player->lasers[i].x, player->lasers[i].y, 2, 5, 10);
            vga_fill_rect(player->lasers[i].x, player->lasers[i].y, 1, 5, 15);
        }
    }
}

// TO BE CONTINUED...

// ============================================================================
// BRICK SYSTEM
// ============================================================================

static void init_bricks()
{
    level_t* level = &levels[game.level];
    
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            game.bricks[row][col].health = level->pattern[row][col];
            game.bricks[row][col].shake_x = 0;
            game.bricks[row][col].shake_y = 0;
            game.bricks[row][col].shake_timer = 0;
        }
    }
}

static void update_bricks()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            if (game.bricks[row][col].shake_timer > 0)
            {
                game.bricks[row][col].shake_timer--;
                game.bricks[row][col].shake_x = random_range(-2, 2);
                game.bricks[row][col].shake_y = random_range(-1, 1);
            }
            else
            {
                game.bricks[row][col].shake_x = 0;
                game.bricks[row][col].shake_y = 0;
            }
        }
    }
}

static void draw_bricks()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            if (game.bricks[row][col].health > 0)
            {
                int brick_x = col * (BRICK_WIDTH + 2) + 5 + game.bricks[row][col].shake_x;
                int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y + game.bricks[row][col].shake_y;
                
                uint8_t color = levels[game.level].colors[row];
                if (game.bricks[row][col].health == 1) color = 8;
                
                vga_fill_rect(brick_x, brick_y, BRICK_WIDTH, BRICK_HEIGHT, color);
                
                for (int i = 0; i < BRICK_WIDTH; i++)
                {
                    vga_set_pixel_shake(brick_x + i, brick_y, 0);
                    vga_set_pixel_shake(brick_x + i, brick_y + BRICK_HEIGHT - 1, 0);
                }
                for (int i = 0; i < BRICK_HEIGHT; i++)
                {
                    vga_set_pixel_shake(brick_x, brick_y + i, 0);
                    vga_set_pixel_shake(brick_x + BRICK_WIDTH - 1, brick_y + i, 0);
                }
            }
        }
    }
}

static bool check_level_complete()
{
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            if (game.bricks[row][col].health > 0) return false;
        }
    }
    return true;
}

// ============================================================================
// BALL SYSTEM
// ============================================================================

static void init_balls()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        game.balls[i].active = false;
    }
    
    game.balls[0].active = true;
    game.balls[0].x = VGA_WIDTH / 2;
    game.balls[0].y = VGA_HEIGHT / 2;
    game.balls[0].dx = BALL_SPEED;
    game.balls[0].dy = -BALL_SPEED;
    game.balls[0].trail_index = 0;
}

static void update_balls()
{
    bool any_active = false;
    player_t* player = &game.players[game.current_player];
    
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (!game.balls[i].active) continue;
        
        any_active = true;
        
        game.balls[i].trail_x[game.balls[i].trail_index] = game.balls[i].x;
        game.balls[i].trail_y[game.balls[i].trail_index] = game.balls[i].y;
        game.balls[i].trail_index = (game.balls[i].trail_index + 1) % 10;
        
        int speed_x = game.balls[i].dx;
        int speed_y = game.balls[i].dy;
        
        if (game.ball_speed_multiplier < 0)
        {
            static int slow_counter = 0;
            slow_counter++;
            if (slow_counter % 2 == 0) continue;
        }
        else if (game.ball_speed_multiplier > 0)
        {
            speed_x *= 2;
            speed_y *= 2;
        }
        
        game.balls[i].x += speed_x;
        game.balls[i].y += speed_y;
        
        if (game.balls[i].x <= 0 || game.balls[i].x >= VGA_WIDTH - BALL_SIZE)
        {
            game.balls[i].dx = -game.balls[i].dx;
            play_sound(400, 30);
        }
        
        if (game.balls[i].y <= 0)
        {
            game.balls[i].dy = -game.balls[i].dy;
            play_sound(400, 30);
        }
        
        if (game.balls[i].y >= VGA_HEIGHT)
        {
            game.balls[i].active = false;
            continue;
        }
        
        if (check_collision(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE,
                          player->paddle_x, PADDLE_Y, player->paddle_width, PADDLE_HEIGHT))
        {
            game.balls[i].dy = -abs(game.balls[i].dy);
            
            int paddle_center = player->paddle_x + player->paddle_width / 2;
            int ball_center = game.balls[i].x + BALL_SIZE / 2;
            int offset = ball_center - paddle_center;
            
            if (offset < -10) game.balls[i].dx = -BALL_SPEED;
            else if (offset > 10) game.balls[i].dx = BALL_SPEED;
            
            play_sound(600, 30);
        }
        
        for (int row = 0; row < BRICK_ROWS; row++)
        {
            for (int col = 0; col < BRICK_COLS; col++)
            {
                if (game.bricks[row][col].health > 0)
                {
                    int brick_x = col * (BRICK_WIDTH + 2) + 5;
                    int brick_y = row * (BRICK_HEIGHT + 2) + BRICK_START_Y;
                    
                    if (check_collision(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE,
                                      brick_x, brick_y, BRICK_WIDTH, BRICK_HEIGHT))
                    {
                        game.bricks[row][col].health--;
                        game.balls[i].dy = -game.balls[i].dy;
                        
                        if (game.bricks[row][col].health == 0)
                        {
                            player->score += 10;
                            spawn_explosion(brick_x + BRICK_WIDTH/2, brick_y + BRICK_HEIGHT/2,
                                          levels[game.level].colors[row]);
                            spawn_powerup(brick_x, brick_y);
                            game.screen_shake_timer = 3;
                        }
                        else
                        {
                            game.bricks[row][col].shake_timer = 5;
                            play_sound(300, 30);
                        }
                        
                        goto next_ball;
                    }
                }
            }
        }
        next_ball:;
    }
    
    if (!any_active)
    {
        player->lives--;
        if (player->lives > 0)
        {
            init_balls();
        }
        else
        {
            // TURN IS OVER!
            player->turn_complete = true;
        }
    }
}

static void draw_balls()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (game.balls[i].active)
        {
            for (int t = 0; t < 10; t++)
            {
                int alpha = t * 2;
                if (alpha > 8) alpha = 8;
                vga_set_pixel_shake(game.balls[i].trail_x[t], game.balls[i].trail_y[t], 8 + alpha);
            }
            
            vga_fill_rect(game.balls[i].x, game.balls[i].y, BALL_SIZE, BALL_SIZE, 15);
            vga_set_pixel_shake(game.balls[i].x + 1, game.balls[i].y + 1, 14);
        }
    }
}

// RENDERING CONTINUES...

// ============================================================================
// DIGIT FONT & HUD
// ============================================================================

static const uint8_t digit_font[10][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F},
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x1F},
    {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F},
    {0x1F, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x1F},
    {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01},
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F},
    {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F},
    {0x1F, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10},
    {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F},
    {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F},
};

static void draw_digit(int x, int y, int digit, uint8_t color)
{
    if (digit < 0 || digit > 9) return;
    
    for (int row = 0; row < 7; row++)
    {
        uint8_t line = digit_font[digit][row];
        for (int col = 0; col < 5; col++)
        {
            if (line & (1 << (4 - col)))
            {
                vga_set_pixel_shake(x + col, y + row, color);
            }
        }
    }
}

static void draw_number(int x, int y, int number, uint8_t color)
{
    if (number < 0) number = 0;
    
    int digits[6];
    int num_digits = 0;
    
    if (number == 0)
    {
        digits[0] = 0;
        num_digits = 1;
    }
    else
    {
        int temp = number;
        while (temp > 0 && num_digits < 6)
        {
            digits[num_digits++] = temp % 10;
            temp /= 10;
        }
    }
    
    for (int i = 0; i < num_digits; i++)
    {
        draw_digit(x - (i * 6), y, digits[i], color);
    }
}

static void draw_hud()
{
    player_t* player = &game.players[game.current_player];
    
    // Current player indicator
    uint8_t player_color = (game.current_player == 0) ? 14 : 11;  // Yellow or Cyan
    vga_fill_rect(5, 5, 80, 12, 0);
    
    // "P1" or "P2"
    vga_fill_rect(10, 7, 2, 7, player_color);
    vga_fill_rect(10, 7, 4, 2, player_color);
    vga_fill_rect(10, 10, 4, 2, player_color);
    
    if (game.current_player == 1)
    {
        vga_fill_rect(18, 7, 4, 2, player_color);
        vga_fill_rect(18, 14, 4, 2, player_color);
        vga_fill_rect(18, 7, 2, 7, player_color);
        vga_fill_rect(20, 10, 2, 2, player_color);
    }
    
    // Score
    draw_number(75, 7, player->score, player_color);
    
    // Lives
    for (int i = 0; i < player->lives && i < 5; i++)
    {
        vga_fill_rect(10 + i * 12, VGA_HEIGHT - 10, 4, 3, 4);
        vga_fill_rect(15 + i * 12, VGA_HEIGHT - 10, 4, 3, 4);
        vga_fill_rect(11 + i * 12, VGA_HEIGHT - 9, 7, 5, 4);
    }
    
    // Level
    draw_number(VGA_WIDTH - 30, 7, game.level + 1, 11);
}

static void draw_paddle()
{
    player_t* player = &game.players[game.current_player];
    uint8_t color = (game.current_player == 0) ? 15 : 11;
    
    vga_fill_rect(player->paddle_x, PADDLE_Y, player->paddle_width, PADDLE_HEIGHT, color);
    
    if (player->has_laser)
    {
        vga_fill_rect(player->paddle_x + 2, PADDLE_Y - 3, 3, 2, 10);
        vga_fill_rect(player->paddle_x + player->paddle_width - 5, PADDLE_Y - 3, 3, 2, 10);
    }
}

// ============================================================================
// TURN TRANSITION & WINNER SCREEN
// ============================================================================

static void draw_turn_transition()
{
    vga_clear(0);
    
    int text_x = VGA_WIDTH / 2;
    int text_y = VGA_HEIGHT / 2 - 20;
    
    // "PLAYER X TURN"
    uint8_t color = (game.current_player == 0) ? 14 : 11;
    
    // Draw solid box
    vga_fill_rect(text_x - 60, text_y - 10, 120, 50, color);
    vga_fill_rect(text_x - 57, text_y - 7, 114, 44, 0);
    
    // Draw "PLAYER" text (simple pixel art)
    int px = text_x - 35;
    int py = text_y;
    
    // P
    vga_fill_rect(px, py, 2, 10, 15);
    vga_fill_rect(px, py, 5, 2, 15);
    vga_fill_rect(px + 4, py, 2, 5, 15);
    vga_fill_rect(px, py + 4, 5, 2, 15);
    
    px += 8;
    // L
    vga_fill_rect(px, py, 2, 10, 15);
    vga_fill_rect(px, py + 8, 5, 2, 15);
    
    px += 8;
    // A
    vga_fill_rect(px, py, 5, 2, 15);
    vga_fill_rect(px, py, 2, 10, 15);
    vga_fill_rect(px + 3, py, 2, 10, 15);
    vga_fill_rect(px, py + 5, 5, 2, 15);
    
    px += 7;
    // Y
    vga_fill_rect(px, py, 2, 5, 15);
    vga_fill_rect(px + 3, py, 2, 5, 15);
    vga_fill_rect(px + 1, py + 4, 3, 1, 15);
    vga_fill_rect(px + 2, py + 5, 1, 5, 15);
    
    px += 7;
    // E
    vga_fill_rect(px, py, 2, 10, 15);
    vga_fill_rect(px, py, 5, 2, 15);
    vga_fill_rect(px, py + 4, 4, 2, 15);
    vga_fill_rect(px, py + 8, 5, 2, 15);
    
    px += 7;
    // R
    vga_fill_rect(px, py, 2, 10, 15);
    vga_fill_rect(px, py, 5, 2, 15);
    vga_fill_rect(px + 4, py, 2, 5, 15);
    vga_fill_rect(px, py + 4, 5, 2, 15);
    vga_fill_rect(px + 4, py + 6, 2, 4, 15);
    
    // Draw player number (BIG)
    py += 15;
    px = text_x - 5;
    draw_number(px, py, game.current_player + 1, color);
    
    // Make number bigger (draw it 2x2)
    for (int dy = 0; dy < 7; dy++)
    {
        for (int dx = 0; dx < 5; dx++)
        {
            uint8_t digit = game.current_player + 1;
            if (digit >= 0 && digit <= 9)
            {
                uint8_t line = digit_font[digit][dy];
                if (line & (1 << (4 - dx)))
                {
                    vga_set_pixel(px + dx * 2, py + dy * 2, color);
                    vga_set_pixel(px + dx * 2 + 1, py + dy * 2, color);
                    vga_set_pixel(px + dx * 2, py + dy * 2 + 1, color);
                    vga_set_pixel(px + dx * 2 + 1, py + dy * 2 + 1, color);
                }
            }
        }
    }
}

static void draw_winner_screen()
{
    vga_clear(0);
    
    // Determine winner
    int winner = 0;
    if (game.num_players == 2)
    {
        if (game.players[1].score > game.players[0].score)
        {
            winner = 1;
        }
        else if (game.players[0].score == game.players[1].score)
        {
            winner = -1;  // Tie
        }
    }
    
    int box_x = VGA_WIDTH / 2 - 80;
    int box_y = VGA_HEIGHT / 2 - 50;
    int box_w = 160;
    int box_h = 100;
    
    // Winner color
    uint8_t bg_color = (winner == 0) ? 14 : (winner == 1) ? 11 : 2;
    
    // Draw solid box
    vga_fill_rect(box_x, box_y, box_w, box_h, bg_color);
    vga_fill_rect(box_x + 3, box_y + 3, box_w - 6, box_h - 6, 0);
    
    // Border (thicker)
    for (int i = 0; i < box_w; i++)
    {
        vga_set_pixel(box_x + i, box_y, 15);
        vga_set_pixel(box_x + i, box_y + 1, 15);
        vga_set_pixel(box_x + i, box_y + box_h - 1, 15);
        vga_set_pixel(box_x + i, box_y + box_h - 2, 15);
    }
    for (int i = 0; i < box_h; i++)
    {
        vga_set_pixel(box_x, box_y + i, 15);
        vga_set_pixel(box_x + 1, box_y + i, 15);
        vga_set_pixel(box_x + box_w - 1, box_y + i, 15);
        vga_set_pixel(box_x + box_w - 2, box_y + i, 15);
    }
    
    int text_y = box_y + 15;
    int text_x = box_x + box_w / 2;
    
    // "WINNER!" or "TIE!"
    int wx = text_x - 30;
    int wy = text_y;
    
    if (winner >= 0)
    {
        // W
        vga_fill_rect(wx, wy, 2, 10, 15);
        vga_fill_rect(wx + 3, wy + 7, 2, 3, 15);
        vga_fill_rect(wx + 6, wy, 2, 10, 15);
        vga_fill_rect(wx + 1, wy + 8, 1, 2, 15);
        vga_fill_rect(wx + 4, wy + 8, 1, 2, 15);
        vga_fill_rect(wx + 7, wy + 8, 1, 2, 15);
        
        wx += 10;
        // I
        vga_fill_rect(wx, wy, 2, 10, 15);
        
        wx += 4;
        // N
        vga_fill_rect(wx, wy, 2, 10, 15);
        vga_fill_rect(wx + 5, wy, 2, 10, 15);
        vga_fill_rect(wx + 2, wy + 2, 1, 2, 15);
        vga_fill_rect(wx + 3, wy + 4, 1, 2, 15);
        vga_fill_rect(wx + 4, wy + 6, 1, 2, 15);
        
        wx += 9;
        // !
        vga_fill_rect(wx, wy, 2, 7, 15);
        vga_fill_rect(wx, wy + 9, 2, 1, 15);
        
        // Player number (big)
        text_y += 15;
        draw_number(text_x, text_y, winner + 1, bg_color);
    }
    else
    {
        // "TIE!"
        // T
        vga_fill_rect(wx, wy, 8, 2, 15);
        vga_fill_rect(wx + 3, wy, 2, 10, 15);
        
        wx += 10;
        // I
        vga_fill_rect(wx, wy, 2, 10, 15);
        
        wx += 4;
        // E
        vga_fill_rect(wx, wy, 2, 10, 15);
        vga_fill_rect(wx, wy, 5, 2, 15);
        vga_fill_rect(wx, wy + 4, 4, 2, 15);
        vga_fill_rect(wx, wy + 8, 5, 2, 15);
        
        wx += 7;
        // !
        vga_fill_rect(wx, wy, 2, 7, 15);
        vga_fill_rect(wx, wy + 9, 2, 1, 15);
    }
    
    // Show both scores
    text_y += 25;
    
    // P1 Score (solid)
    vga_fill_rect(text_x - 50, text_y - 2, 35, 12, 14);
    vga_fill_rect(text_x - 48, text_y, 31, 8, 0);
    draw_number(text_x - 35, text_y + 1, 1, 14);
    draw_number(text_x - 20, text_y + 1, game.players[0].score, 14);
    
    if (game.num_players == 2)
    {
        // P2 Score (solid)
        vga_fill_rect(text_x + 15, text_y - 2, 35, 12, 11);
        vga_fill_rect(text_x + 17, text_y, 31, 8, 0);
        draw_number(text_x + 30, text_y + 1, 2, 11);
        draw_number(text_x + 45, text_y + 1, game.players[1].score, 11);
    }
    
    // "PRESS SPACE"
    text_y += 20;
    int sx = text_x - 35;
    int sy = text_y;
    
    // S
    vga_fill_rect(sx, sy, 4, 1, 8);
    vga_fill_rect(sx, sy, 1, 3, 8);
    vga_fill_rect(sx, sy + 2, 4, 1, 8);
    vga_fill_rect(sx + 3, sy + 2, 1, 3, 8);
    vga_fill_rect(sx, sy + 4, 4, 1, 8);
    sx += 6;
    
    // P
    vga_fill_rect(sx, sy, 1, 5, 8);
    vga_fill_rect(sx, sy, 3, 1, 8);
    vga_fill_rect(sx + 2, sy, 1, 3, 8);
    vga_fill_rect(sx, sy + 2, 3, 1, 8);
    sx += 5;
    
    // A
    vga_fill_rect(sx, sy, 3, 1, 8);
    vga_fill_rect(sx, sy, 1, 5, 8);
    vga_fill_rect(sx + 2, sy, 1, 5, 8);
    vga_fill_rect(sx, sy + 2, 3, 1, 8);
    sx += 5;
    
    // C
    vga_fill_rect(sx, sy, 3, 1, 8);
    vga_fill_rect(sx, sy, 1, 5, 8);
    vga_fill_rect(sx, sy + 4, 3, 1, 8);
    sx += 5;
    
    // E
    vga_fill_rect(sx, sy, 1, 5, 8);
    vga_fill_rect(sx, sy, 3, 1, 8);
    vga_fill_rect(sx, sy + 2, 2, 1, 8);
    vga_fill_rect(sx, sy + 4, 3, 1, 8);
}

// ============================================================================
// GAME INITIALIZATION
// ============================================================================

void breakout_init(int num_players)
{
    game.num_players = (num_players <= MAX_PLAYERS) ? num_players : 1;
    game.current_player = 0;
    game.level = 0;
    game.all_players_done = false;
    game.paused = false;
    game.sound_enabled = true;
    game.ball_speed_multiplier = 0;
    game.music_note = 0;
    game.music_timer = 0;
    game.screen_shake_timer = 0;
    game.screen_shake_x = 0;
    game.screen_shake_y = 0;
    
    for (int p = 0; p < MAX_PLAYERS; p++)
    {
        game.players[p].lives = 3;
        game.players[p].score = 0;
        game.players[p].paddle_width = PADDLE_WIDTH;
        game.players[p].paddle_x = VGA_WIDTH / 2 - PADDLE_WIDTH / 2;
        game.players[p].has_laser = false;
        game.players[p].laser_cooldown = 0;
        game.players[p].turn_complete = false;
        
        for (int i = 0; i < MAX_LASERS; i++)
        {
            game.players[p].lasers[i].active = false;
        }
    }
    
    init_bricks();
    init_balls();
    
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        game.powerups[i].active = false;
    }
    
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        game.particles[i].active = false;
    }
}

// ============================================================================
// INPUT HANDLING
// ============================================================================

static void handle_input(key_event_t* event)
{
    if (!event->pressed) return;
    
    // If game is over, only allow restart or exit
    if (game.all_players_done)
    {
        if (event->scancode == 0x39)  // Space to restart
        {
            breakout_init(game.num_players);
        }
        return;  // Don't process other inputs when game over
    }
    
    player_t* player = &game.players[game.current_player];
    
    if (event->scancode == 0x4B || event->scancode == 0x1E)  // Left / A
    {
        player->paddle_x -= PADDLE_SPEED * 2;
        if (player->paddle_x < 0) player->paddle_x = 0;
    }
    if (event->scancode == 0x4D || event->scancode == 0x20)  // Right / D
    {
        player->paddle_x += PADDLE_SPEED * 2;
        if (player->paddle_x > VGA_WIDTH - player->paddle_width)
            player->paddle_x = VGA_WIDTH - player->paddle_width;
    }
    if (event->scancode == 0x1D)  // Ctrl - shoot laser
    {
        shoot_laser(player);
    }
    
    if (event->scancode == 0x19)  // P - pause
    {
        game.paused = !game.paused;
    }
    
    if (event->scancode == 0x32)  // M - music toggle
    {
        game.sound_enabled = !game.sound_enabled;
        if (!game.sound_enabled) stop_sound();
    }
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

void breakout_run()
{
    uint32_t last_update = timer_get_ticks();
    uint32_t sound_timer = 0;
    bool showing_transition = false;
    uint32_t transition_start = 0;
    bool transition_drawn = false;
    bool winner_drawn = false;
    
    while (1)
    {
        key_event_t event;
        while (keyboard_get_event(&event))
        {
            if (event.pressed && event.scancode == 0x01)  // ESC
            {
                stop_sound();
                return;
            }
            
            // Check for restart (Space when game over)
            if (event.pressed && event.scancode == 0x39 && game.all_players_done)
            {
                breakout_init(game.num_players);
                winner_drawn = false;  // Reset flags
                transition_drawn = false;
                showing_transition = false;
                continue;
            }
            
            handle_input(&event);
        }
        
        uint32_t current_ticks = timer_get_ticks();
        
        // Check for turn switch
        if (game.players[game.current_player].turn_complete && !showing_transition)
        {
            if (game.current_player < game.num_players - 1)
            {
                // Next player's turn
                game.current_player++;
                showing_transition = true;
                transition_start = current_ticks;
                transition_drawn = false;  // Need to redraw
                init_bricks();  // Reset bricks for next player
                init_balls();
                game.ball_speed_multiplier = 0;
                
                // Reset powerups
                for (int i = 0; i < MAX_POWERUPS; i++)
                {
                    game.powerups[i].active = false;
                }
            }
            else
            {
                // All players done!
                game.all_players_done = true;
                winner_drawn = false;  // Need to draw winner screen
            }
        }
        
        // Show transition screen (DRAW ONCE!)
        if (showing_transition)
        {
            if (!transition_drawn)
            {
                draw_turn_transition();
                transition_drawn = true;
            }
            
            if (current_ticks - transition_start >= 2000)  // 2 seconds
            {
                showing_transition = false;
            }
            continue;
        }
        
        // Show winner screen (DRAW ONCE!)
        if (game.all_players_done)
        {
            if (!winner_drawn)
            {
                draw_winner_screen();
                winner_drawn = true;
            }
            continue;  // Don't redraw!
        }
        
        // Update at 60 FPS (ONLY DURING GAMEPLAY)
        if (current_ticks - last_update >= 16)
        {
            last_update = current_ticks;
            
            if (!game.paused)
            {
                update_balls();
                update_bricks();
                update_powerups();
                update_particles();
                update_lasers();
                
                // Check level complete
                if (check_level_complete())
                {
                    game.level++;
                    if (game.level >= MAX_LEVELS)
                    {
                        game.players[game.current_player].turn_complete = true;
                    }
                    else
                    {
                        init_bricks();
                        init_balls();
                    }
                }
                
                // Screen shake
                if (game.screen_shake_timer > 0)
                {
                    game.screen_shake_timer--;
                    game.screen_shake_x = random_range(-2, 2);
                    game.screen_shake_y = random_range(-2, 2);
                }
                else
                {
                    game.screen_shake_x = 0;
                    game.screen_shake_y = 0;
                }
                
                // Music
                sound_timer++;
                if (sound_timer >= 5)
                {
                    sound_timer = 0;
                    update_music();
                }
            }
            
            // Render (ONLY during active gameplay)
            vga_clear(0);
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