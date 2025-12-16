/*
 * breakout_menu.c - Title screen and menu system
 * 
 * Shows the "BRICK BREAKER ULTIMATE" title with animated falling bricks
 * and a menu where players can select game mode.
 * 
 * Author: CS Student
 * Date: December 2024
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout_menu.h"

// External function we need
extern void vga_fill_rect(int x, int y, int width, int height, uint8_t color);

// Menu state
static menu_state_t menu;

/* ============================================================================
 * SIMPLE PIXEL FONT FOR MENU TEXT
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
            vga_fill_rect(x + 2, y, 2, 12, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            vga_fill_rect(x, y, 2, 3, color);
            break;
            
        case '2':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x + 4, y, 2, 5, color);
            vga_fill_rect(x, y + 4, 6, 2, color);
            vga_fill_rect(x, y + 6, 2, 4, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case '3':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x + 4, y, 2, 12, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case '4':
            vga_fill_rect(x, y, 2, 6, color);
            vga_fill_rect(x + 4, y, 2, 12, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'A':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 4, y, 2, 12, color);
            vga_fill_rect(x, y + 6, 6, 2, color);
            break;
            
        case 'B':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y, 5, 2, color);
            vga_fill_rect(x, y + 5, 5, 2, color);
            vga_fill_rect(x, y + 10, 5, 2, color);
            vga_fill_rect(x + 4, y, 2, 5, color);
            vga_fill_rect(x + 4, y + 6, 2, 4, color);
            break;
            
        case 'C':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'D':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y, 5, 2, color);
            vga_fill_rect(x, y + 10, 5, 2, color);
            vga_fill_rect(x + 4, y + 2, 2, 8, color);
            break;
            
        case 'E':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x, y + 5, 5, 2, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'H':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 4, y, 2, 12, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'I':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x + 2, y, 2, 12, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'K':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 4, y, 2, 4, color);
            vga_fill_rect(x + 2, y + 5, 2, 2, color);
            vga_fill_rect(x + 4, y + 8, 2, 4, color);
            break;
            
        case 'L':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'M':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 5, y, 2, 12, color);
            vga_fill_rect(x + 2, y + 2, 1, 3, color);
            vga_fill_rect(x + 4, y + 2, 1, 3, color);
            break;
            
        case 'N':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 5, y, 2, 12, color);
            vga_fill_rect(x + 1, y + 3, 2, 2, color);
            vga_fill_rect(x + 2, y + 5, 2, 2, color);
            vga_fill_rect(x + 3, y + 7, 2, 2, color);
            break;
            
        case 'O':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x + 4, y, 2, 12, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'P':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x + 4, y, 2, 6, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            break;
            
        case 'R':
            vga_fill_rect(x, y, 2, 12, color);
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x + 4, y, 2, 6, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            vga_fill_rect(x + 4, y + 7, 2, 5, color);
            break;
            
        case 'S':
            vga_fill_rect(x, y, 6, 2, color);
            vga_fill_rect(x, y, 2, 6, color);
            vga_fill_rect(x, y + 5, 6, 2, color);
            vga_fill_rect(x + 4, y + 6, 2, 4, color);
            vga_fill_rect(x, y + 10, 6, 2, color);
            break;
            
        case 'T':
            vga_fill_rect(x, y, 8, 2, color);
            vga_fill_rect(x + 3, y, 2, 12, color);
            break;
            
        case 'V':
            vga_fill_rect(x, y, 2, 8, color);
            vga_fill_rect(x + 5, y, 2, 8, color);
            vga_fill_rect(x + 1, y + 8, 2, 2, color);
            vga_fill_rect(x + 4, y + 8, 2, 2, color);
            vga_fill_rect(x + 2, y + 10, 2, 2, color);
            break;
            
        case 'Y':
            vga_fill_rect(x, y, 2, 6, color);
            vga_fill_rect(x + 4, y, 2, 6, color);
            vga_fill_rect(x + 1, y + 5, 4, 2, color);
            vga_fill_rect(x + 2, y + 6, 2, 6, color);
            break;
            
        case ' ':
            // Space - do nothing
            break;
            
        default:
            // Unknown character - draw a small dot
            vga_fill_rect(x + 2, y + 5, 2, 2, color);
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
 * TITLE LOGO
 * ============================================================================
 */

static void draw_title_logo()
{
    // Draw the big colorful "BRICK BREAKER ULTIMATE" title
    // (This is the part that's working - the colored blocks)
    
    int title_y = 30;
    int title_x = 40;
    
    // "BRICK" - First word (already working from original code)
    vga_fill_rect(title_x, title_y, 20, 5, 4);
    vga_fill_rect(title_x, title_y + 10, 20, 5, 4);
    vga_fill_rect(title_x, title_y + 20, 20, 5, 4);
    vga_fill_rect(title_x, title_y, 5, 25, 4);
    vga_fill_rect(title_x + 15, title_y, 5, 10, 4);
    vga_fill_rect(title_x + 15, title_y + 15, 5, 10, 4);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 20, 5, 12);
    vga_fill_rect(title_x, title_y + 10, 15, 5, 12);
    vga_fill_rect(title_x, title_y, 5, 25, 12);
    vga_fill_rect(title_x + 15, title_y, 5, 10, 12);
    vga_fill_rect(title_x + 15, title_y + 15, 5, 10, 12);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 15, 5, 14);
    vga_fill_rect(title_x, title_y + 20, 15, 5, 14);
    vga_fill_rect(title_x + 5, title_y, 5, 25, 14);
    
    title_x += 20;
    vga_fill_rect(title_x, title_y, 20, 5, 2);
    vga_fill_rect(title_x, title_y + 20, 20, 5, 2);
    vga_fill_rect(title_x, title_y, 5, 25, 2);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 5, 25, 1);
    vga_fill_rect(title_x + 10, title_y, 5, 10, 1);
    vga_fill_rect(title_x + 5, title_y + 10, 10, 5, 1);
    vga_fill_rect(title_x + 10, title_y + 15, 5, 10, 1);
    
    // "BREAKER" - Second line
    title_y = 65;
    title_x = 55;
    
    vga_fill_rect(title_x, title_y, 20, 5, 13);
    vga_fill_rect(title_x, title_y + 10, 20, 5, 13);
    vga_fill_rect(title_x, title_y + 20, 20, 5, 13);
    vga_fill_rect(title_x, title_y, 5, 25, 13);
    vga_fill_rect(title_x + 15, title_y, 5, 10, 13);
    vga_fill_rect(title_x + 15, title_y + 15, 5, 10, 13);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 20, 5, 12);
    vga_fill_rect(title_x, title_y + 10, 15, 5, 12);
    vga_fill_rect(title_x, title_y, 5, 25, 12);
    vga_fill_rect(title_x + 15, title_y, 5, 10, 12);
    vga_fill_rect(title_x + 15, title_y + 15, 5, 10, 12);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 20, 5, 14);
    vga_fill_rect(title_x, title_y + 10, 15, 5, 14);
    vga_fill_rect(title_x, title_y + 20, 20, 5, 14);
    vga_fill_rect(title_x, title_y, 5, 25, 14);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 20, 5, 2);
    vga_fill_rect(title_x, title_y + 10, 20, 5, 2);
    vga_fill_rect(title_x, title_y, 5, 25, 2);
    vga_fill_rect(title_x + 15, title_y, 5, 25, 2);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 5, 25, 11);
    vga_fill_rect(title_x + 10, title_y, 5, 10, 11);
    vga_fill_rect(title_x + 5, title_y + 10, 10, 5, 11);
    vga_fill_rect(title_x + 10, title_y + 15, 5, 10, 11);
    
    title_x += 20;
    vga_fill_rect(title_x, title_y, 20, 5, 9);
    vga_fill_rect(title_x, title_y + 10, 15, 5, 9);
    vga_fill_rect(title_x, title_y + 20, 20, 5, 9);
    vga_fill_rect(title_x, title_y, 5, 25, 9);
    
    title_x += 25;
    vga_fill_rect(title_x, title_y, 20, 5, 4);
    vga_fill_rect(title_x, title_y + 10, 15, 5, 4);
    vga_fill_rect(title_x, title_y, 5, 25, 4);
    vga_fill_rect(title_x + 15, title_y, 5, 10, 4);
    vga_fill_rect(title_x + 15, title_y + 15, 5, 10, 4);
    
    // "ULTIMATE" subtitle
    title_y = 100;
    title_x = 110;
    
    for (int i = 0; i < 8; i++)
    {
        vga_fill_rect(title_x + i * 8, title_y, 6, 8, 15);
        vga_fill_rect(title_x + i * 8 + 1, title_y + 1, 4, 6, 0);
    }
}

/* ============================================================================
 * ANIMATED FALLING BRICKS
 * ============================================================================
 */

static void draw_animated_bricks()
{
    static int brick_y[10] = {0, -20, -40, -60, -80, -100, -120, -140, -160, -180};
    static uint8_t brick_colors[10] = {4, 12, 14, 2, 1, 13, 9, 11, 10, 6};
    
    for (int i = 0; i < 10; i++)
    {
        brick_y[i] += 2;
        if (brick_y[i] > 200) brick_y[i] = -20;
        
        int x = 20 + (i % 5) * 60;
        int y = brick_y[i];
        
        if (y >= 0 && y < 180)
        {
            vga_fill_rect(x, y, 25, 10, brick_colors[i]);
            
            for (int j = 0; j < 25; j++)
            {
                vga_set_pixel(x + j, y, 0);
                vga_set_pixel(x + j, y + 9, 0);
            }
            for (int j = 0; j < 10; j++)
            {
                vga_set_pixel(x, y + j, 0);
                vga_set_pixel(x + 24, y + j, 0);
            }
        }
    }
}

/* ============================================================================
 * MENU OPTIONS
 * ============================================================================
 */

static void draw_menu_options()
{
    int menu_y = 135;
    int menu_x = 100;
    
    // Draw each menu option
    for (int i = 0; i < MENU_COUNT; i++)
    {
        uint8_t color = (i == menu.selected) ? 15 : 8;
        uint8_t bg_color = (i == menu.selected) ? 14 : 0;
        
        // Draw selection highlight
        if (i == menu.selected)
        {
            vga_fill_rect(menu_x - 5, menu_y + i * 15 - 2, 140, 12, bg_color);
            vga_fill_rect(menu_x - 3, menu_y + i * 15, 136, 8, 0);
        }
        
        // Draw menu text
        int text_x = menu_x;
        int text_y = menu_y + i * 15;
        
        switch (i)
        {
            case MENU_SINGLE_PLAYER:
                draw_text(text_x, text_y, "1 PLAYER", color);
                break;
                
            case MENU_TWO_PLAYER:
                draw_text(text_x, text_y, "2 PLAYERS", color);
                break;
                
            case MENU_OPTIONS:  // FIXED: was MENU_SETTINGS
                draw_text(text_x, text_y, "SETTINGS", color);
                break;
                
            case MENU_EXIT:
                draw_text(text_x, text_y, "EXIT", color);
                break;
        }
        
        // Draw arrow for selected item
        if (i == menu.selected)
        {
            int arrow_offset = (menu.animation_frame / 10) % 3;
            int arrow_x = menu_x - 12 - arrow_offset;
            int arrow_y = menu_y + i * 15 + 2;
            
            vga_fill_rect(arrow_x, arrow_y, 3, 5, 14);
            vga_fill_rect(arrow_x + 3, arrow_y + 1, 2, 3, 14);
            vga_fill_rect(arrow_x + 5, arrow_y + 2, 1, 1, 14);
        }
    }
    
    // Draw footer
    int footer_y = 185;
    vga_fill_rect(50, footer_y, 220, 1, 8);
    
    // "PRESS ENTER" message (blinking)
    int info_y = footer_y + 5;
    uint8_t info_color = ((menu.animation_frame / 30) % 2 == 0) ? 15 : 8;
    
    draw_text(70, info_y, "PRESS ENTER TO START", info_color);
}

/* ============================================================================
 * MAIN MENU FUNCTIONS
 * ============================================================================
 */

void menu_show()
{
    vga_clear(0);
    draw_animated_bricks();
    draw_title_logo();
    draw_menu_options();
}

int menu_run()
{
    menu.selected = MENU_SINGLE_PLAYER;
    menu.in_menu = true;
    menu.animation_frame = 0;
    
    uint32_t last_update = timer_get_ticks();
    
    while (menu.in_menu)
    {
        // Handle input
        key_event_t event;
        while (keyboard_get_event(&event))
        {
            if (!event.pressed) continue;
            
            // Navigation
            if (event.scancode == 0x48 || event.scancode == 0x11)  // Up or W
            {
                if (menu.selected > 0)
                    menu.selected--;
                else
                    menu.selected = MENU_COUNT - 1;
            }
            else if (event.scancode == 0x50 || event.scancode == 0x1F)  // Down or S
            {
                menu.selected = (menu.selected + 1) % MENU_COUNT;
            }
            else if (event.scancode == 0x1C)  // Enter
            {
                switch (menu.selected)
                {
                    case MENU_SINGLE_PLAYER:
                        return 1;
                    case MENU_TWO_PLAYER:
                        return 2;
                    case MENU_OPTIONS:  // FIXED: was MENU_SETTINGS
                        // Not implemented yet
                        break;
                    case MENU_EXIT:
                        return 0;
                }
            }
            else if (event.scancode == 0x01)  // ESC
            {
                return 0;
            }
        }
        
        // Update animation
        uint32_t current_ticks = timer_get_ticks();
        if (current_ticks - last_update >= 16)
        {
            last_update = current_ticks;
            menu.animation_frame++;
            menu_show();
        }
    }
    
    return 0;
}