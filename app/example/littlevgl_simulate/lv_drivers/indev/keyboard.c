/**
 * @file sdl_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "keyboard.h"
#if USE_KEYBOARD

#include "lvgl/lv_core/lv_group.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t keycode_to_ascii(uint32_t sdl_key);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_key;
static lv_indev_state_t state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the keyboard
 */
void keyboard_init(void)
{
    /*Nothing to init*/
}

/**
 * Get the last pressed or released character from the PC's keyboard
 * @param data store the read data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool keyboard_read(lv_indev_data_t * data)
{
    data->state = state;

    if(state == LV_INDEV_STATE_REL) {
        data->key = 0;
    } else {
        data->key = keycode_to_ascii(last_key);
    }

    return false;
}


void keyboard_handler(SDL_Event *event)
{
    /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
    switch( event->type ){
        case SDL_KEYDOWN:
            last_key = event->key.keysym.sym;
            state = LV_INDEV_STATE_PR;
            break;
        case SDL_KEYUP:
            state = LV_INDEV_STATE_REL;
            break;
        default:
            break;

    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t keycode_to_ascii(uint32_t sdl_key)
{
    /*Remap some key to LV_GROUP_KEY_... to manage groups*/
    switch(sdl_key) {
        case SDLK_RIGHT:
        case SDLK_KP_PLUS:
            return LV_GROUP_KEY_RIGHT;

        case SDLK_LEFT:
        case SDLK_KP_MINUS:
            return LV_GROUP_KEY_LEFT;

        case SDLK_UP:
            return LV_GROUP_KEY_UP;

        case SDLK_DOWN:
            return LV_GROUP_KEY_DOWN;

        case SDLK_ESCAPE:
            return LV_GROUP_KEY_ESC;

        case SDLK_KP_ENTER:
            return LV_GROUP_KEY_ENTER;

        case '\r':
            return LV_GROUP_KEY_ENTER;

        default: return sdl_key;
    }
}
#endif
