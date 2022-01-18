/**
 * @file mouse.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "SDL_mouse.h"
#if USE_MOUSE != 0

/*********************
 *      DEFINES
 *********************/
#ifndef MONITOR_ZOOM
#define MONITOR_ZOOM    1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool left_button_down = false;
static int16_t first_x = 0, last_x = 0;
static int16_t first_y = 0, last_y = 0;
static bool mouse_was_read = false;
static int16_t cached_clicks = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
void mouse_read(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /* Replay cached clicks on its original coordinates */
    if (cached_clicks > 0) {
        cached_clicks--;
        data->point.x = first_x;
        data->point.y = first_y;
        data->state = (cached_clicks&1)==1 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

        data->continue_reading = true;
        return;
    }

    /* Store the collected data */
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = left_button_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    mouse_was_read = true;

    data->continue_reading = false;
    return;
}

/**
 * It will be called from the main SDL thread
 */
void mouse_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = false;
                if (!mouse_was_read) cached_clicks+=2;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / MONITOR_ZOOM;
                last_y = event->motion.y / MONITOR_ZOOM;
                if (mouse_was_read) {
                    first_x = last_x;
                    first_y = last_y;
                }
                mouse_was_read = false;
            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / MONITOR_ZOOM;
            last_y = event->motion.y / MONITOR_ZOOM;

            break;
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
