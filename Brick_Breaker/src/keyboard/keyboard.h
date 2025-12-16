#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// ADDED: Get Doom key from keyboard buffer
// Returns 1 if key event available, 0 otherwise
// *pressed = 1 for press, 0 for release
// *doom_key = Doom key code
int keyboard_get_doom_key(int* pressed, unsigned char* doom_key);

// ADDED: Key event structure
typedef struct {
    uint8_t scancode;   // Raw scancode from keyboard
    char ascii;         // ASCII character (0 if non-printable)
    bool pressed;       // true = pressed, false = released
} key_event_t;

// ADDED: Initialize keyboard (clears buffer)
void keyboard_init();

// ADDED: Get next key event (returns false if no event available)
bool keyboard_get_event(key_event_t* event);

#endif