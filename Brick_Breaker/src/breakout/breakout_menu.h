#ifndef BREAKOUT_MENU_H
#define BREAKOUT_MENU_H

#include <stdint.h>
#include <stdbool.h>

// Menu options
typedef enum {
    MENU_SINGLE_PLAYER = 0,
    MENU_TWO_PLAYER = 1,
    MENU_OPTIONS = 2,
    MENU_EXIT = 3,
    MENU_COUNT = 4
} menu_option_t;

// Menu state
typedef struct {
    menu_option_t selected;
    bool in_menu;
    int animation_frame;
} menu_state_t;

// Public functions
void menu_show();  // Returns selected option (1 or 2 players, or 0 for exit)
int menu_run();    // Run the menu, return number of players (or 0 to exit)

#endif // BREAKOUT_MENU_H