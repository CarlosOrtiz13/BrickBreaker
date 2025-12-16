/*
 * breakout_audio.c - Sound system using PC speaker
 * 
 * This file handles all sound effects and background music. The PC speaker
 * is a very simple device - we can only play one frequency at a time, but
 * by changing frequencies quickly we can make melodies and sound effects.
 * 
 * How it works:
 * - The Programmable Interval Timer (PIT) generates square waves
 * - Port 0x61 controls whether the speaker is enabled
 * - We calculate a "divisor" from the frequency we want
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"
#include "io/io.h"  // For inb/outb functions

// External reference to game state (defined in breakout_main.c)
extern game_state_t game;

/*
 * play_sound - Play a specific frequency through the PC speaker
 * 
 * This uses the PIT (Programmable Interval Timer) to generate a square wave
 * at the desired frequency. The PIT has a base frequency of 1193180 Hz, so
 * we divide that by our desired frequency to get the right divisor.
 * 
 * Parameters:
 *   frequency - The frequency in Hz (e.g., 440 = A note)
 *   duration_ms - How long to play (not currently used, sound plays until stopped)
 */
void play_sound(int frequency, int duration_ms)
{
    // Don't play if sound is disabled or frequency is 0
    if (!game.sound_enabled || frequency == 0)
    {
        return;
    }
    
    // Calculate the PIT divisor for this frequency
    // PIT base frequency is 1193180 Hz
    uint32_t divisor = 1193180 / frequency;
    
    // Configure PIT channel 2 (the speaker channel)
    // Command byte: 10110110 = channel 2, access mode lobyte/hibyte, square wave
    outb(0x43, 0xB6);
    
    // Send the divisor (low byte then high byte)
    outb(0x42, (uint8_t)(divisor & 0xFF));
    outb(0x42, (uint8_t)((divisor >> 8) & 0xFF));
    
    // Enable the speaker
    // Port 0x61 controls the speaker - we need to set bits 0 and 1
    uint8_t tmp = insb(0x61);
    outb(0x61, tmp | 0x03);  // Set bits 0 and 1 to enable
}

/*
 * stop_sound - Turn off the PC speaker
 * 
 * This disables the speaker by clearing the control bits in port 0x61.
 */
void stop_sound()
{
    // Disable speaker by clearing bits 0 and 1 of port 0x61
    uint8_t tmp = insb(0x61);
    outb(0x61, tmp & 0xFC);  // Clear bits 0 and 1
}

/*
 * update_music - Play background music
 * 
 * This plays a simple 8-note melody that loops. It's called every frame
 * from the main game loop. We change notes every 30 frames (about 0.5 seconds).
 */
void update_music()
{
    // Don't play if sound is disabled
    if (!game.sound_enabled)
    {
        return;
    }
    
    // 8-note melody (C, D, E, F, G, F, E, D)
    // These are frequencies in Hz
    static const int melody[] = {
        523,  // C5
        587,  // D5
        659,  // E5
        698,  // F5
        784,  // G5
        698,  // F5
        659,  // E5
        587   // D5
    };
    static const int melody_length = 8;
    
    // Increment music timer every frame
    game.music_timer++;
    
    // Change note every 30 frames (about 0.5 seconds at 60 FPS)
    if (game.music_timer >= 30)
    {
        game.music_timer = 0;  // Reset timer
        
        // Move to next note (wrap around to 0 after last note)
        game.music_note = (game.music_note + 1) % melody_length;
        
        // Play the new note
        play_sound(melody[game.music_note], 100);
    }
}