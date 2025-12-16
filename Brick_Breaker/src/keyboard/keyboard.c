#include "keyboard.h"
#include "io/io.h"
#include "doomkeys.h"  // ADDED

// ADDED: Circular buffer for keyboard events
#define KEYBOARD_BUFFER_SIZE 16

static key_event_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_read_pos = 0;
static int buffer_write_pos = 0;

// ADDED: Scancode to ASCII table (US keyboard layout)
static const char scancode_to_ascii[] = {
    0,    0,   '1',  '2',  '3',  '4',  '5',  '6',   // 0x00-0x07
    '7',  '8', '9',  '0',  '-',  '=',  '\b', '\t',  // 0x08-0x0F
    'q',  'w', 'e',  'r',  't',  'y',  'u',  'i',   // 0x10-0x17
    'o',  'p', '[',  ']',  '\n', 0,    'a',  's',   // 0x18-0x1F
    'd',  'f', 'g',  'h',  'j',  'k',  'l',  ';',   // 0x20-0x27
    '\'', '`', 0,    '\\', 'z',  'x',  'c',  'v',   // 0x28-0x2F
    'b',  'n', 'm',  ',',  '.',  '/',  0,    '*',   // 0x30-0x37
    0,    ' '                                        // 0x38-0x39
};

// ADDED: This is called by IRQ1 handler in idt.c
void keyboard_handler()
{
    // ADDED: Read scancode from keyboard controller
    uint8_t scancode = insb(0x60);
    
    // ADDED: Check if this is a key release (bit 7 set)
    bool pressed = !(scancode & 0x80);
    if (!pressed)
    {
        scancode &= 0x7F;  // Remove release bit
    }
    
    // ADDED: Convert scancode to ASCII
    char ascii = 0;
    if (scancode < sizeof(scancode_to_ascii))
    {
        ascii = scancode_to_ascii[scancode];
    }
    
    // ADDED: Add to circular buffer
    int next_write = (buffer_write_pos + 1) % KEYBOARD_BUFFER_SIZE;
    if (next_write != buffer_read_pos)  // Check if buffer is not full
    {
        keyboard_buffer[buffer_write_pos].scancode = scancode;
        keyboard_buffer[buffer_write_pos].ascii = ascii;
        keyboard_buffer[buffer_write_pos].pressed = pressed;
        buffer_write_pos = next_write;
    }
    
    // ADDED: Send EOI to PIC
    outb(0x20, 0x20);
}

void keyboard_init()
{
    // ADDED: Clear buffer
    buffer_read_pos = 0;
    buffer_write_pos = 0;
}

bool keyboard_get_event(key_event_t* event)
{
    // ADDED: Check if buffer has events
    if (buffer_read_pos == buffer_write_pos)
    {
        return false;  // Buffer empty
    }
    
    // ADDED: Read from buffer
    *event = keyboard_buffer[buffer_read_pos];
    buffer_read_pos = (buffer_read_pos + 1) % KEYBOARD_BUFFER_SIZE;
    
    return true;
}

// ADDED: Map scancode to Doom key code
static unsigned char scancode_to_doom_key(uint8_t scancode)
{
    switch (scancode)
    {
        // Arrow keys
        case 0x48: return KEY_UPARROW;     // Up arrow
        case 0x4B: return KEY_LEFTARROW;   // Left arrow
        case 0x4D: return KEY_RIGHTARROW;  // Right arrow
        case 0x50: return KEY_DOWNARROW;   // Down arrow
        
        // WASD (alternative movement)
        case 0x11: return KEY_UPARROW;     // W = forward
        case 0x1E: return KEY_STRAFE_L;    // A = strafe left
        case 0x1F: return KEY_DOWNARROW;   // S = backward
        case 0x20: return KEY_STRAFE_R;    // D = strafe right
        
        // Action keys
        case 0x1D: return KEY_FIRE;        // Left Ctrl = fire
        case 0x39: return KEY_USE;         // Space = use/open
        case 0x01: return KEY_ESCAPE;      // ESC
        case 0x1C: return KEY_ENTER;       // Enter
        
        // Function keys
        case 0x3B: return KEY_F1;
        case 0x3C: return KEY_F2;
        case 0x3D: return KEY_F3;
        case 0x3E: return KEY_F4;
        case 0x3F: return KEY_F5;
        case 0x40: return KEY_F6;
        case 0x41: return KEY_F7;
        case 0x42: return KEY_F8;
        case 0x43: return KEY_F9;
        case 0x44: return KEY_F10;
        
        // Other useful keys
        case 0x0F: return KEY_TAB;         // Tab (automap)
        case 0x0E: return KEY_BACKSPACE;
        case 0x0C: return KEY_MINUS;       // - (zoom out)
        case 0x0D: return KEY_EQUALS;      // = (zoom in)
        
        // Numbers for weapon selection
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        
        default: return 0;  // Unmapped key
    }
}

// ADDED: Get Doom key event (for DG_GetKey)
int keyboard_get_doom_key(int* pressed, unsigned char* doom_key)
{
    key_event_t event;
    
    // ADDED: Get next keyboard event
    if (!keyboard_get_event(&event))
    {
        return 0;  // No event available
    }
    
    // ADDED: Map scancode to Doom key
    unsigned char dk = scancode_to_doom_key(event.scancode);
    if (dk == 0)
    {
        return 0;  // Key not mapped to Doom
    }
    
    // ADDED: Return Doom key event
    *pressed = event.pressed ? 1 : 0;
    *doom_key = dk;
    
    return 1;  // Event available
}