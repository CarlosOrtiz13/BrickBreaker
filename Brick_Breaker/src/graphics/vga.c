#include "vga.h"
#include "io/io.h"
#include "memory/memory.h"

// ADDED: Pointer to VGA memory
static uint8_t* vga_memory = (uint8_t*)VGA_MEMORY;

void vga_init()
{
    // ADDED: We're already in text mode (mode 0x03) from boot
    // To get to mode 0x13, we need to use BIOS INT 0x10
    // But we're in protected mode, so we'll use a trick:
    // Write mode 13h setup directly
    
    // SIMPLE METHOD: Just assume mode 13h is set
    // (We'll do this properly in a moment)
    
    // For now, clear the screen
    vga_clear(0);
}

void vga_set_pixel(int x, int y, uint8_t color)
{
    // ADDED: Bounds check
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT)
        return;
    
    // ADDED: Calculate offset and write pixel
    vga_memory[y * VGA_WIDTH + x] = color;
}

void vga_clear(uint8_t color)
{
    // ADDED: Direct loop instead of memset (more reliable)
    uint8_t* vga = (uint8_t*)0xA0000;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        vga[i] = color;
    }
}

void vga_draw_frame(uint8_t* framebuffer)
{
    // ADDED: Direct loop instead of memcpy
    uint8_t* vga = (uint8_t*)0xA0000;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        vga[i] = framebuffer[i];
    }
}

void vga_set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    // ADDED: Set VGA palette entry (6-bit RGB)
    outb(0x3C8, index);
    outb(0x3C9, r >> 2);
    outb(0x3C9, g >> 2);
    outb(0x3C9, b >> 2);
}