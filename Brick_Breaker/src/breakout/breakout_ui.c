/*
 * breakout_ui.c - User Interface and HUD
 * 
 * This file draws all the UI elements:
 * - HUD (score, lives, level, player indicator)
 * - Level start screens
 * - Player turn transition screens
 * - Winner/game over screen
 * - Text and number rendering
 * 
 * These screens use simple pixel art for text since we don't have a font library!
 * 
 * Author: CS Student
 * Date: December 2024
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

// External references
extern game_state_t game;
extern level_t levels[MAX_LEVELS];
extern void draw_rect(int x, int y, int width, int height, uint8_t color);
extern void draw_pixel(int x, int y, uint8_t color);

/* ============================================================================
 * SIMPLE PIXEL FONT FOR TEXT RENDERING
 * ============================================================================
 * Drawing letters pixel by pixel for a retro look!
 */

/*
 * draw_letter - Draw a single letter in pixel art style
 * 
 * This draws simple block letters. Not fancy, but readable!
 */
static void draw_letter(int x, int y, char letter, uint8_t color)
{
    // Make letter uppercase
    if (letter >= 'a' && letter <= 'z')
    {
        letter = letter - 32;
    }
    
    switch (letter)
    {
        case '1':
            draw_rect(x + 2, y, 2, 12, color);
            draw_rect(x, y + 10, 6, 2, color);
            draw_rect(x, y, 2, 3, color);
            break;
            
        case '2':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x + 4, y, 2, 5, color);
            draw_rect(x, y + 4, 6, 2, color);
            draw_rect(x, y + 6, 2, 4, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case '3':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x + 4, y, 2, 12, color);
            draw_rect(x, y + 5, 6, 2, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case '4':
            draw_rect(x, y, 2, 6, color);
            draw_rect(x + 4, y, 2, 12, color);
            draw_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'A':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 4, y, 2, 12, color);
            draw_rect(x, y + 6, 6, 2, color);
            break;
            
        case 'B':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y, 5, 2, color);
            draw_rect(x, y + 5, 5, 2, color);
            draw_rect(x, y + 10, 5, 2, color);
            draw_rect(x + 4, y, 2, 5, color);
            draw_rect(x + 4, y + 6, 2, 4, color);
            break;
            
        case 'C':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'D':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y, 5, 2, color);
            draw_rect(x, y + 10, 5, 2, color);
            draw_rect(x + 4, y + 2, 2, 8, color);
            break;
            
        case 'E':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y, 6, 2, color);
            draw_rect(x, y + 5, 5, 2, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'H':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 4, y, 2, 12, color);
            draw_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'I':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x + 2, y, 2, 12, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'K':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 4, y, 2, 4, color);
            draw_rect(x + 2, y + 5, 2, 2, color);
            draw_rect(x + 4, y + 8, 2, 4, color);
            break;
            
        case 'L':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'M':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 5, y, 2, 12, color);
            draw_rect(x + 2, y + 2, 1, 3, color);
            draw_rect(x + 4, y + 2, 1, 3, color);
            break;
            
        case 'N':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 5, y, 2, 12, color);
            draw_rect(x + 1, y + 3, 2, 2, color);
            draw_rect(x + 2, y + 5, 2, 2, color);
            draw_rect(x + 3, y + 7, 2, 2, color);
            break;
            
        case 'O':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x, y, 2, 12, color);
            draw_rect(x + 4, y, 2, 12, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'P':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y, 6, 2, color);
            draw_rect(x + 4, y, 2, 6, color);
            draw_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'R':
            draw_rect(x, y, 2, 12, color);
            draw_rect(x, y, 6, 2, color);
            draw_rect(x + 4, y, 2, 6, color);
            draw_rect(x, y + 5, 6, 2, color);
            draw_rect(x + 4, y + 7, 2, 5, color);
            break;
            
        case 'S':
            draw_rect(x, y, 6, 2, color);
            draw_rect(x, y, 2, 6, color);
            draw_rect(x, y + 5, 6, 2, color);
            draw_rect(x + 4, y + 6, 2, 4, color);
            draw_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'T':
            draw_rect(x, y, 8, 2, color);
            draw_rect(x + 3, y, 2, 12, color);
            break;
            
        case 'V':
            draw_rect(x, y, 2, 8, color);
            draw_rect(x + 5, y, 2, 8, color);
            draw_rect(x + 1, y + 8, 2, 2, color);
            draw_rect(x + 4, y + 8, 2, 2, color);
            draw_rect(x + 2, y + 10, 2, 2, color);
            break;
            
        case 'Y':
            draw_rect(x, y, 2, 6, color);
            draw_rect(x + 4, y, 2, 6, color);
            draw_rect(x + 1, y + 5, 4, 2, color);
            draw_rect(x + 2, y + 6, 2, 6, color);
            break;
            
        case ' ':
            // Space - do nothing
            break;
            
        default:
            // Unknown character - draw a small dot
            draw_rect(x + 2, y + 5, 2, 2, color);
            break;
    }
}

/*
 * draw_text - Draw a string of text
 */
static void draw_text(int x, int y, const char* text, uint8_t color)
{
    int cursor_x = x;
    
    for (int i = 0; text[i] != '\0'; i++)
    {
        draw_letter(cursor_x, y, text[i], color);
        cursor_x += 8;  // Move to next character position
    }
}

/* ============================================================================
 * DIGIT FONT SYSTEM
 * ============================================================================
 * We need to draw numbers for scores and lives. This is a simple 5x7 pixel
 * font for digits 0-9.
 */

/*
 * digit_font - 5x7 pixel representation of digits 0-9
 * 
 * Each digit is 7 rows tall. Each row is a byte where bits represent pixels.
 * Bit 4 = leftmost pixel, bit 0 = rightmost pixel.
 */
static const uint8_t digit_font[10][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F},  // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x1F},  // 1
    {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F},  // 2
    {0x1F, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x1F},  // 3
    {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01},  // 4
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F},  // 5
    {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F},  // 6
    {0x1F, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10},  // 7
    {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F},  // 8
    {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F},  // 9
};

/*
 * draw_digit - Draw a single digit
 * 
 * Parameters:
 *   x, y - Top-left position
 *   digit - Which digit (0-9)
 *   color - VGA color to use
 */
static void draw_digit(int x, int y, int digit, uint8_t color)
{
    if (digit < 0 || digit > 9)
    {
        return;  // Invalid digit
    }
    
    // Draw each row of the digit
    for (int row = 0; row < 7; row++)
    {
        uint8_t line = digit_font[digit][row];
        
        // Draw each pixel in the row
        for (int col = 0; col < 5; col++)
        {
            // Check if this bit is set
            if (line & (1 << (4 - col)))
            {
                vga_set_pixel(x + col, y + row, color);
            }
        }
    }
}

/*
 * draw_number - Draw a multi-digit number
 * 
 * Numbers are drawn right-to-left so they're right-aligned.
 * 
 * Parameters:
 *   x, y - Right edge position (numbers grow to the left)
 *   number - The number to draw
 *   color - VGA color
 */
static void draw_number(int x, int y, int number, uint8_t color)
{
    if (number < 0)
    {
        number = 0;  // Don't handle negative numbers
    }
    
    // Extract digits (up to 6 digits max)
    int digits[6];
    int num_digits = 0;
    
    // Handle zero specially
    if (number == 0)
    {
        digits[0] = 0;
        num_digits = 1;
    }
    else
    {
        // Extract digits in reverse order
        int temp = number;
        while (temp > 0 && num_digits < 6)
        {
            digits[num_digits++] = temp % 10;
            temp /= 10;
        }
    }
    
    // Draw digits from right to left
    for (int i = 0; i < num_digits; i++)
    {
        draw_digit(x - (i * 6), y, digits[i], color);
    }
}

/* ============================================================================
 * HUD (HEADS-UP DISPLAY)
 * ============================================================================
 */

/*
 * draw_hud - Draw the game HUD at the top of the screen
 * 
 * Shows:
 * - Current player indicator (P1 or P2)
 * - Score
 * - Lives (as heart shapes)
 * - Current level
 */
void draw_hud()
{
    player_t* player = &game.players[game.current_player];
    
    // Player indicator color (yellow for P1, cyan for P2)
    uint8_t player_color = (game.current_player == 0) ? 14 : 11;
    
    // Clear HUD area
    draw_rect(5, 5, 80, 12, 0);
    
    // Draw "P1" or "P2" text (simple pixel art)
    // P
    draw_rect(10, 7, 2, 7, player_color);
    draw_rect(10, 7, 4, 2, player_color);
    draw_rect(10, 10, 4, 2, player_color);
    
    // Draw player number
    if (game.current_player == 1)
    {
        // Draw "2"
        draw_rect(18, 7, 4, 2, player_color);
        draw_rect(18, 14, 4, 2, player_color);
        draw_rect(18, 7, 2, 7, player_color);
        draw_rect(20, 10, 2, 2, player_color);
    }
    else
    {
        // Draw "1"
        draw_rect(18, 7, 2, 7, player_color);
        draw_rect(18, 14, 4, 2, player_color);
    }
    
    // Draw score (right-aligned from position 75)
    draw_number(75, 7, player->score, player_color);
    
    // Draw lives as hearts at bottom of screen
    for (int i = 0; i < player->lives && i < 5; i++)
    {
        int heart_x = 10 + i * 12;
        int heart_y = VGA_HEIGHT - 10;
        
        // Simple heart shape (just a red blob)
        draw_rect(heart_x, heart_y, 4, 3, 4);
        draw_rect(heart_x + 5, heart_y, 4, 3, 4);
        draw_rect(heart_x + 1, heart_y + 1, 7, 5, 4);
    }
    
    // Draw current level indicator (top right)
    draw_number(VGA_WIDTH - 30, 7, game.level + 1, 11);
}

/* ============================================================================
 * LEVEL START SCREEN
 * ============================================================================
 */

/*
 * draw_countdown - Show 3-2-1-GO countdown
 * 
 * Displays a large countdown before gameplay starts.
 * Makes the number pulse for effect!
 */
void draw_countdown(int number)
{
    vga_clear(0);
    
    // Determine what to show
    char* text = "GO!";
    uint8_t color = 10;  // Green for GO
    
    if (number == 3)
    {
        text = "3";
        color = 4;  // Red
    }
    else if (number == 2)
    {
        text = "2";
        color = 14;  // Yellow
    }
    else if (number == 1)
    {
        text = "1";
        color = 10;  // Green
    }
    
    // Draw big centered number/text
    int center_x = VGA_WIDTH / 2;
    int center_y = VGA_HEIGHT / 2;
    
    if (number >= 1 && number <= 3)
    {
        // Draw large digit (40x60 pixels)
        int digit = number;
        int x = center_x - 20;
        int y = center_y - 30;
        
        // Draw the digit based on which number
        switch (digit)
        {
            case 3:
                // Draw "3"
                draw_rect(x, y, 40, 8, color);           // Top bar
                draw_rect(x + 32, y, 8, 28, color);      // Right side top
                draw_rect(x, y + 26, 40, 8, color);      // Middle bar
                draw_rect(x + 32, y + 32, 8, 28, color); // Right side bottom
                draw_rect(x, y + 52, 40, 8, color);      // Bottom bar
                break;
                
            case 2:
                // Draw "2"
                draw_rect(x, y, 40, 8, color);           // Top bar
                draw_rect(x + 32, y, 8, 28, color);      // Right side top
                draw_rect(x, y + 26, 40, 8, color);      // Middle bar
                draw_rect(x, y + 32, 8, 28, color);      // Left side bottom
                draw_rect(x, y + 52, 40, 8, color);      // Bottom bar
                break;
                
            case 1:
                // Draw "1"
                draw_rect(x, y, 20, 8, color);           // Top angle
                draw_rect(x + 16, y, 8, 60, color);      // Vertical bar
                draw_rect(x, y + 52, 40, 8, color);      // Bottom bar
                break;
        }
        
        // Draw glowing border effect
        for (int i = 0; i < 3; i++)
        {
            // Top
            for (int dx = -5 - i; dx < 45 + i; dx++)
                draw_pixel(x + dx, y - 5 - i, color - 4);
            // Bottom
            for (int dx = -5 - i; dx < 45 + i; dx++)
                draw_pixel(x + dx, y + 65 + i, color - 4);
            // Left
            for (int dy = -5 - i; dy < 65 + i; dy++)
                draw_pixel(x - 5 - i, y + dy, color - 4);
            // Right
            for (int dy = -5 - i; dy < 65 + i; dy++)
                draw_pixel(x + 45 + i, y + dy, color - 4);
        }
    }
    else
    {
        // Draw "GO!" text (bigger and clearer)
        int x = center_x - 40;
        int y = center_y - 25;
        
        // G (larger)
        draw_rect(x, y, 25, 8, color);          // Top
        draw_rect(x, y, 8, 50, color);          // Left side
        draw_rect(x, y + 42, 25, 8, color);     // Bottom
        draw_rect(x + 17, y + 25, 8, 25, color);// Right side bottom
        draw_rect(x + 12, y + 25, 13, 8, color);// Inner horizontal
        
        x += 35;
        
        // O (larger)
        draw_rect(x, y, 25, 8, color);          // Top
        draw_rect(x, y, 8, 50, color);          // Left side
        draw_rect(x + 17, y, 8, 50, color);     // Right side
        draw_rect(x, y + 42, 25, 8, color);     // Bottom
        
        x += 35;
        
        // ! (larger)
        draw_rect(x + 6, y, 8, 35, color);      // Vertical
        draw_rect(x + 6, y + 40, 8, 10, color); // Dot
        
        // Draw border around GO!
        int border_x = center_x - 50;
        int border_y = center_y - 35;
        int border_w = 100;
        int border_h = 70;
        
        // Thick white border
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < border_w; j++)
            {
                draw_pixel(border_x + j, border_y + i, 15);
                draw_pixel(border_x + j, border_y + border_h - 1 - i, 15);
            }
            for (int j = 0; j < border_h; j++)
            {
                draw_pixel(border_x + i, border_y + j, 15);
                draw_pixel(border_x + border_w - 1 - i, border_y + j, 15);
            }
        }
        
        // Pulse effect rings
        for (int i = 1; i <= 3; i++)
        {
            int offset = i * 10;
            uint8_t ring_color = 10 - (i * 2);
            
            // Top
            for (int j = 0; j < border_w + offset * 2; j++)
                draw_pixel(border_x - offset + j, border_y - offset, ring_color);
            // Bottom
            for (int j = 0; j < border_w + offset * 2; j++)
                draw_pixel(border_x - offset + j, border_y + border_h + offset - 1, ring_color);
            // Left
            for (int j = 0; j < border_h + offset * 2; j++)
                draw_pixel(border_x - offset, border_y - offset + j, ring_color);
            // Right
            for (int j = 0; j < border_h + offset * 2; j++)
                draw_pixel(border_x + border_w + offset - 1, border_y - offset + j, ring_color);
        }
    }
}

/*
 * draw_level_start_screen - Show level info before starting
 * 
 * Displays:
 * - Level number
 * - Level name
 * - Preview of brick pattern
 * 
 * This gives players a moment to prepare!
 */
void draw_level_start_screen()
{
    vga_clear(0);  // Clear screen to black
    
    int box_x = VGA_WIDTH / 2 - 80;
    int box_y = VGA_HEIGHT / 2 - 40;
    int box_w = 160;
    int box_h = 80;
    
    // Level-specific colors for the box
    uint8_t level_colors[4] = {14, 12, 2, 4};  // Yellow, Light Red, Green, Red
    uint8_t bg_color = level_colors[game.level];
    
    // Draw colored box
    draw_rect(box_x, box_y, box_w, box_h, bg_color);
    draw_rect(box_x + 3, box_y + 3, box_w - 6, box_h - 6, 0);
    
    // Draw white border for emphasis
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
    int text_x = box_x + 20;
    
    // Draw "LEVEL" using our text function
    draw_text(text_x, text_y, "LEVEL", 15);
    
    // Draw level number (bigger, next to LEVEL text)
    char level_num[2];
    level_num[0] = '1' + game.level;
    level_num[1] = '\0';
    
    // Draw big level number
    int num_x = text_x + 50;
    int num_y = text_y - 2;
    
    // Draw the digit 2x size
    for (int scale = 0; scale < 2; scale++)
    {
        for (int sy = 0; sy < 2; sy++)
        {
            draw_text(num_x + scale, num_y + sy, level_num, bg_color);
        }
    }
    
    // Draw level name below (use the actual level name from data)
    text_y += 20;
    text_x = box_x + box_w / 2 - 30;
    
    // Get level name and draw it
    char* level_name = levels[game.level].name;
    draw_text(text_x, text_y, level_name, 15);
    
    // Draw mini preview of brick pattern
    text_y += 18;
    int preview_x = box_x + 20;
    int preview_y = text_y;
    
    // Show first 3 rows and 10 columns
    for (int row = 0; row < BRICK_ROWS && row < 3; row++)
    {
        for (int col = 0; col < BRICK_COLS && col < 10; col++)
        {
            if (levels[game.level].pattern[row][col] > 0)
            {
                int px = preview_x + col * 12;
                int py = preview_y + row * 7;
                
                // Draw mini brick (10x5 pixels)
                uint8_t brick_color = levels[game.level].colors[row];
                draw_rect(px, py, 10, 5, brick_color);
                
                // Border
                vga_set_pixel(px, py, 0);
                vga_set_pixel(px + 9, py, 0);
                vga_set_pixel(px, py + 4, 0);
                vga_set_pixel(px + 9, py + 4, 0);
            }
        }
    }
}

/* ============================================================================
 * TURN TRANSITION SCREEN
 * ============================================================================
 */

/*
 * draw_turn_transition - Show "PLAYER X TURN" screen
 * 
 * This appears between player turns in 2-player mode.
 */
void draw_turn_transition()
{
    vga_clear(0);
    
    int text_x = VGA_WIDTH / 2 - 60;
    int text_y = VGA_HEIGHT / 2 - 20;
    
    // Color depends on which player's turn
    uint8_t color = (game.current_player == 0) ? 14 : 11;
    
    // Draw box
    draw_rect(text_x - 10, text_y - 10, 140, 50, color);
    draw_rect(text_x - 7, text_y - 7, 134, 44, 0);
    
    // Draw "PLAYER" text
    draw_text(text_x, text_y, "PLAYER", 15);
    
    // Draw player number
    char player_num[2];
    player_num[0] = '1' + game.current_player;
    player_num[1] = '\0';
    
    draw_text(text_x + 56, text_y, player_num, color);
    
    // Draw "TURN" text below
    draw_text(text_x + 20, text_y + 18, "TURN", 15);
}

/* ============================================================================
 * WINNER SCREEN
 * ============================================================================
 */

/*
 * draw_winner_screen - Show final scores and winner
 * 
 * Displays both player scores and declares a winner (or tie).
 */
void draw_winner_screen()
{
    // If your game loop calls this every frame, this prevents flicker.
    static int drawn_once = 0;
    if (drawn_once) return;
    drawn_once = 1;

    // Decide if this is actually GAME OVER (single-player losing)
    int is_game_over = 0;
    if (game.num_players == 1)
    {
        if (game.players[0].lives <= 0)
            is_game_over = 1;
    }

    vga_clear(0);

    // Determine winner (only meaningful for 2 players and not game over)
    int winner = 0;
    if (!is_game_over && game.num_players == 2)
    {
        if (game.players[1].score > game.players[0].score) winner = 1;
        else if (game.players[0].score == game.players[1].score) winner = -1; // tie
    }

    int box_x = VGA_WIDTH / 2 - 80;
    int box_y = VGA_HEIGHT / 2 - 50;
    int box_w = 160;
    int box_h = 100;

    // Use calmer colors to improve readability (no big yellow fill)
    uint8_t accent = 15; // white border by default
    if (is_game_over) accent = 4;                 // red accent
    else if (winner == 0) accent = 14;            // P1 accent
    else if (winner == 1) accent = 11;            // P2 accent
    else accent = 2;                              // tie accent (green)

    // Draw a black box with colored border (prevents bright flashing fill)
    draw_rect(box_x, box_y, box_w, box_h, 0);

    // Thick border
    for (int i = 0; i < box_w; i++)
    {
        vga_set_pixel(box_x + i, box_y, accent);
        vga_set_pixel(box_x + i, box_y + 1, accent);
        vga_set_pixel(box_x + i, box_y + box_h - 1, accent);
        vga_set_pixel(box_x + i, box_y + box_h - 2, accent);
    }
    for (int i = 0; i < box_h; i++)
    {
        vga_set_pixel(box_x, box_y + i, accent);
        vga_set_pixel(box_x + 1, box_y + i, accent);
        vga_set_pixel(box_x + box_w - 1, box_y + i, accent);
        vga_set_pixel(box_x + box_w - 2, box_y + i, accent);
    }

    int text_y = box_y + 15;
    int text_x = box_x + 25;

    if (is_game_over)
    {
        draw_text(text_x + 15, text_y, "GAME", 15);
        draw_text(text_x + 15, text_y + 18, "OVER", 15);
    }
    else if (winner >= 0)
    {
        draw_text(text_x + 15, text_y, "WINNER", 15);

        text_y += 20;
        draw_text(text_x + 20, text_y, "PLAYER", 15);

        char winner_num[2];
        winner_num[0] = '1' + winner;
        winner_num[1] = '\0';

        // Use white (not bg color) so it's always readable
        draw_text(text_x + 72, text_y, winner_num, 15);
    }
    else
    {
        draw_text(text_x + 55, text_y, "TIE", 15);
    }

    // Show scores
    text_y += 30;
    text_x = box_x + 20;

    draw_text(text_x, text_y, "P1", 14);
    draw_number(text_x + 50, text_y, game.players[0].score, 14);

    if (game.num_players == 2)
    {
        text_x = box_x + 90;
        draw_text(text_x, text_y, "P2", 11);
        draw_number(text_x + 50, text_y, game.players[1].score, 11);
    }

    // PRESS SPACE
    text_y += 20;
    draw_text(box_x + 25, text_y, "PRESS SPACE", 8);
}
