#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stdbool.h>

// ADDED: VGA Mode 13h constants
#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define VGA_MEMORY 0xA0000

// ADDED: Initialize VGA Mode 13h (320x200, 256 colors)
void vga_init();

// ADDED: Set a single pixel
void vga_set_pixel(int x, int y, uint8_t color);

// ADDED: Clear screen to a color
void vga_clear(uint8_t color);

// ADDED: Draw entire framebuffer (for DoomGeneric)
void vga_draw_frame(uint8_t* framebuffer);

// ADDED: Set VGA palette entry
void vga_set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

#endif