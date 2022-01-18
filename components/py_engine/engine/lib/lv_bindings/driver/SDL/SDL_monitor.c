/**
 * @file monitor.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "SDL_monitor.h"
#if USE_MONITOR

#ifndef MONITOR_SDL_INCLUDE_PATH
#define MONITOR_SDL_INCLUDE_PATH <SDL2/SDL.h>
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include MONITOR_SDL_INCLUDE_PATH
#include "SDL_mouse.h"

/*********************
 *      DEFINES
 *********************/

#ifndef MONITOR_ZOOM
#define MONITOR_ZOOM        1
#endif

#if defined(__EMSCRIPTEN__)
#  define MONITOR_EMSCRIPTEN
#endif

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t* tft_fb;
static int monitor_w;
static int monitor_h;
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_quit_qry = false;

static int quit_filter(void * userdata, SDL_Event * event);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool monitor_active(void)
{
    return sdl_inited && !sdl_quit_qry;
}

/**
 * Flush a buffer to the display. Calls 'lv_flush_ready()' when finished
 */
void monitor_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen or the monitor is not active*/
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > MONITOR_HOR_RES - 1 || area->y1 > MONITOR_VER_RES - 1
       || !monitor_active()) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    int32_t y;
#if LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32    /*32 is valid but support 24 for backward compatibility too*/
    int32_t x;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            tft_fb[y * MONITOR_HOR_RES + x] = lv_color_to32(*color_p);
            color_p++;
        }

    }
#else
    uint32_t w = area->x2 - area->x1 + 1;
    for(y = area->y1; y <= area->y2; y++) {
        memcpy(&tft_fb[y * MONITOR_HOR_RES + area->x1], color_p, w * sizeof(lv_color_t));

        color_p += w;
    }
#endif

    sdl_refr_qry = true;

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);
}

/**
 * Handle "quit" event
 */
static int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if(event->type == SDL_QUIT) {
        sdl_quit_qry = true;
    }

    return 1;
}

/**
 * Initialize the monitor
 */
void monitor_init(int w, int h)
{
    sdl_refr_qry = false;
    sdl_quit_qry = false;

    monitor_w = w;
    monitor_h = h;
    tft_fb = (uint32_t*)malloc(MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(uint32_t));

    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetEventFilter(quit_filter, NULL);

    window = SDL_CreateWindow("TFT Simulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              MONITOR_HOR_RES * MONITOR_ZOOM, MONITOR_VER_RES * MONITOR_ZOOM, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

#if MONITOR_VIRTUAL_MACHINE  || defined(MONITOR_EMSCRIPTEN)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
#else
    renderer = SDL_CreateRenderer(window, -1, 0);
#endif
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, MONITOR_HOR_RES, MONITOR_VER_RES);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    memset(tft_fb, 77, MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(uint32_t));
    SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
    sdl_refr_qry = true;
    sdl_inited = true;
}

/**
 * Deinit the monitor and close SDL
 */
void monitor_deinit(void)
{
    sdl_quit_qry = true;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(tft_fb);
}

/**
 * This is SDL main loop. It draws the screen and handle mouse events.
 * It should be called periodically, from the same thread Micropython is running
 * This is done by schduling it using mp_sched_schedule
 */
void monitor_sdl_refr_core(void)
{
     if(sdl_refr_qry != false) {
        sdl_refr_qry = false;
        SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        /*Test: Draw a background to test transparent screens (LV_COLOR_SCREEN_TRANSP)*/
//        SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
//        SDL_Rect r;
//        r.x = 0; r.y = 0; r.w = MONITOR_HOR_RES; r.w = MONITOR_VER_RES;
//        SDL_RenderDrawRect(renderer, &r);

        /*Update the renderer with the texture containing the rendered image*/
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    SDL_Event event;
    while(monitor_active() && SDL_PollEvent(&event)) {
#if USE_MOUSE != 0
        mouse_handler(&event);
#endif

#if USE_MOUSEWHEEL != 0
        mousewheel_handler(&event);
#endif

#if USE_KEYBOARD
        keyboard_handler(&event);
#endif
        if((&event)->type == SDL_WINDOWEVENT) {
            switch((&event)->window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    break;
                default:
                    break;
            }
        }
    }

    if (sdl_quit_qry) {
        monitor_deinit();
    }
}

#endif /*USE_MONITOR*/
