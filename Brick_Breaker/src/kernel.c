#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "string/string.h"
#include "fs/file.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "gdt/gdt.h"
#include "config.h"
#include "status.h"
#include "timer/timer.h"  // ADDED: Timer functions
#include "keyboard/keyboard.h"  // ADDED
#include "graphics/vga.h" // ADDED
#include "breakout/breakout.h"
#include "breakout/breakout_menu.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}
void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }   
}

void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}


static struct paging_4gb_chunk* kernel_chunk = 0;

void panic(const char* msg)
{
    print(msg);
    while(1) {}
}

struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
};

// ADDED: Helper function to print numbers
void print_number(uint32_t num)
{
    if (num == 0)
    {
        terminal_writechar('0', 15);
        return;
    }
    
    char buf[32];
    int i = 0;
    
    while (num > 0)
    {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Print in reverse
    for (int j = i - 1; j >= 0; j--)
    {
        terminal_writechar(buf[j], 15);
    }
}

#define WAD_LBA   8948

// IMPORTANT: set this to the real file size
// Run: stat -c%s src/doomgeneric/Doom_UserFiles/doom1.wad
#define WAD_BYTES 4196020

void* wad_data = 0;
unsigned int wad_size_rounded = 0;

static void load_wad_to_ram(void)
{
    // Color 14 = yellow: starting WAD load
    vga_clear(14);

    size_t wad_sectors = (WAD_BYTES + 511) / 512;
    wad_size_rounded = wad_sectors * 512;

    wad_data = kmalloc(wad_size_rounded);
    if (!wad_data)
    {
        vga_clear(4); // red = alloc fail
        while (1) {}
    }

    struct disk_stream* stream = diskstreamer_new(0);
    if (!stream)
    {
        vga_clear(5); // magenta = streamer fail
        while (1) {}
    }

    diskstreamer_seek(stream, WAD_LBA * 512);

    int res = diskstreamer_read(stream, wad_data, (int)wad_size_rounded);
    diskstreamer_close(stream);

    if (res < 0)
    {
        vga_clear(1); // blue = read fail
        while (1) {}
    }

    char* p = (char*)wad_data;
    if (!(p[0]=='I' && p[1]=='W' && p[2]=='A' && p[3]=='D'))
    {
        vga_clear(6); // brown = bad header
        while (1) {}
    }

    // Green = WAD OK
    vga_clear(2);
}


void kernel_main()
{
    // DON'T initialize terminal - we're in graphics mode now!
    // terminal_initialize();  // REMOVE THIS
    
    // Setup GDT (no visual output)
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, PEACHOS_TOTAL_GDT_SEGMENTS);
    gdt_load(gdt_real, sizeof(gdt_real));
    
    // Initialize heap
    kheap_init();
    
    // Initialize filesystems
    fs_init();
    
    // Initialize IDT
    idt_init();
    
    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    enable_paging();
    
    // Initialize timer
    timer_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Enable interrupts
    enable_interrupts();
    
    // ADDED: Initialize VGA (already in mode 13h from boot)
    vga_init();

    int num_players = menu_run();

    if (num_players == 0){
        vga_clear(0);

        while(1) {}
    }

    breakout_init(num_players);

    breakout_run();

    // ------------------------------------------------------------------------
    // 4. GAME ENDED
    // ------------------------------------------------------------------------
    // User pressed ESC, game loop exited
    vga_clear(0);
    
    while(1) {}
    
}