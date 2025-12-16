#include "timer.h"
#include "io/io.h"
#include "idt/idt.h"

// Global tick counter (incremented by IRQ0 handler)
// ADDED: volatile tells compiler this can change at any time (from interrupt)
static volatile uint32_t g_timer_ticks = 0;

// PIT operates at 1.193182 MHz base frequency
#define PIT_FREQUENCY 1193182
#define TARGET_FREQUENCY 1000  // 1000 Hz = 1 tick per millisecond

// ADDED: This function is called by the IRQ0 assembly wrapper
void timer_handler()
{
    g_timer_ticks++;
    // ADDED: Send End-Of-Interrupt signal to PIC
    outb(0x20, 0x20);
}

void timer_init()
{
    // ADDED: Calculate divisor for desired frequency
    uint32_t divisor = PIT_FREQUENCY / TARGET_FREQUENCY;
    
    // ADDED: Send command byte to PIT (Channel 0, Mode 3 - square wave)
    outb(0x43, 0x36);
    
    // ADDED: Send divisor (low byte, then high byte)
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    
    // Timer will now fire IRQ0 at 1000 Hz
}

uint32_t timer_get_ticks()
{
    return g_timer_ticks;
}

void timer_wait(uint32_t ms)
{
    uint32_t start = g_timer_ticks;
    // ADDED: Busy-wait loop until enough ticks have passed
    while ((g_timer_ticks - start) < ms)
    {
        // Just wait
    }
}