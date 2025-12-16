#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initialize PIT (Programmable Interval Timer) to 1000 Hz (1ms ticks)
void timer_init();

// Get milliseconds since boot
uint32_t timer_get_ticks();

// Busy-wait for specified milliseconds
void timer_wait(uint32_t ms);

#endif