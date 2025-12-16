/*
 * breakout_particles.c - Particle system for visual effects
 * 
 * This file creates explosion effects when bricks are destroyed. When a brick
 * breaks, we spawn 8 particles that spray outward in different directions.
 * Each particle has physics (velocity and gravity) and a lifetime.
 * 
 * The particle system makes the game feel more dynamic and satisfying!
 */

#include "keyboard/keyboard.h"
#include "graphics/vga.h"
#include "timer/timer.h"
#include "breakout.h"

// External references
extern game_state_t game;
extern void play_sound(int frequency, int duration_ms);

/*
 * random_range - Generate a pseudo-random number
 * 
 * We need randomness for particle directions and power-up spawning.
 * This is a simple Linear Congruential Generator (LCG) - good enough
 * for game effects!
 * 
 * Parameters:
 *   min - Minimum value (inclusive)
 *   max - Maximum value (inclusive)
 * 
 * Returns:
 *   A pseudo-random number between min and max
 */
int random_range(int min, int max)
{
    // Static variable persists between function calls
    static uint32_t seed = 12345;
    
    // LCG formula: seed = (a * seed + c) mod m
    // These constants are from Numerical Recipes
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    
    // Map to our desired range
    return min + (seed % (max - min + 1));
}

/*
 * spawn_particle - Create a single particle
 * 
 * This adds a particle to the particle array if there's room.
 * Particles are stored in a fixed-size array, so we look for an
 * inactive slot to use.
 * 
 * Parameters:
 *   x, y - Starting position
 *   dx, dy - Initial velocity
 *   color - VGA color index
 *   life - How many frames this particle should live
 */
void spawn_particle(int x, int y, int dx, int dy, uint8_t color, int life)
{
    // Search for an inactive particle slot
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!game.particles[i].active)
        {
            // Found a free slot! Initialize it
            game.particles[i].x = x;
            game.particles[i].y = y;
            game.particles[i].dx = dx;
            game.particles[i].dy = dy;
            game.particles[i].color = color;
            game.particles[i].life = life;
            game.particles[i].active = true;
            return;  // Done!
        }
    }
    
    // If we get here, all particle slots are full
    // That's okay, we just won't spawn this particle
}

/*
 * spawn_explosion - Create an explosion effect
 * 
 * This spawns 8 particles in different directions to create a burst effect.
 * Called when a brick is destroyed.
 * 
 * Parameters:
 *   x, y - Center of explosion (usually center of brick)
 *   color - Color for the particles (usually the brick's color)
 */
void spawn_explosion(int x, int y, uint8_t color)
{
    // Create 8 particles in a circular pattern
    // We use a 3x3 grid to determine directions
    for (int i = 0; i < 8; i++)
    {
        // Calculate direction based on position in 3x3 grid
        int dx = (i % 3) - 1;  // -1, 0, or 1
        int dy = (i / 3) - 1;  // -1, 0, or 1
        
        // Skip the center (0,0) - we want 8 directions, not 9
        if (dx == 0 && dy == 0)
        {
            dy = -1;  // Make it go up instead
        }
        
        // Spawn particle with velocity in this direction
        // Multiply by 2 to make particles move faster
        spawn_particle(x, y, dx * 2, dy * 2, color, 15);
    }
    
    // Play explosion sound
    // Randomize pitch slightly for variation
    play_sound(200 + random_range(0, 100), 50);
}

/*
 * update_particles - Update all active particles
 * 
 * This is called every frame. It updates particle positions using their
 * velocity, applies gravity, and decrements their lifetime. When a particle's
 * life reaches 0, it's deactivated.
 */
void update_particles()
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        // Skip inactive particles
        if (!game.particles[i].active)
        {
            continue;
        }
        
        // Apply velocity to position
        game.particles[i].x += game.particles[i].dx;
        game.particles[i].y += game.particles[i].dy;
        
        // Apply gravity (pulls particles down)
        // This makes the explosion look more natural
        game.particles[i].dy += 1;
        
        // Decrease lifetime
        game.particles[i].life--;
        
        // Check if particle died
        if (game.particles[i].life <= 0)
        {
            game.particles[i].active = false;
        }
    }
}