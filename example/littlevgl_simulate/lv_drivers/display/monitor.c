/**
 * @file monitor.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "monitor.h"
#if USE_MONITOR

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lvgl/lv_core/lv_vdb.h"

/*********************
 *      DEFINES
 *********************/
#define SDL_REFR_PERIOD	    50	/*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int sdl_refr(void * param);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
#if USE_MOUSE
void mouse_handler(SDL_Event *event);
#endif

#if USE_KEYBOARD
void keyboard_handler(SDL_Event *event);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t tft_fb[MONITOR_HOR_RES * MONITOR_VER_RES];
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_quit_qry = false;

int quit_filter (void *userdata, SDL_Event * event);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the monitor
 */
void monitor_init(void)
{
	SDL_CreateThread(sdl_refr, "sdl_refr", NULL);

	while(sdl_inited == false); /*Wait until 'sdl_refr' initializes the SDL*/
}


/**
 * Flush a buffer to the display. Calls 'lv_flush_ready()' when finished
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p array of colors to be flushed
 */
void monitor_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0 || y2 < 0 || x1 > MONITOR_HOR_RES - 1 || y1 > MONITOR_VER_RES - 1) {
        lv_flush_ready();
        return;
    }

    int32_t y;
#if LV_COLOR_DEPTH != 24
    int32_t x;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            tft_fb[y * MONITOR_HOR_RES + x] = lv_color_to24(*color_p);
            color_p++;
        }

    }
#else
    uint32_t w = x2 - x1 + 1;
    for(y = y1; y <= y2; y++) {
        memcpy(&tft_fb[y * MONITOR_HOR_RES + x1], color_p, w * sizeof(lv_color_t));

        color_p += w;
    }
#endif

    sdl_refr_qry = true;

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_flush_ready();
}


/**
 * Fill out the marked area with a color
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color fill color
 */
void monitor_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > MONITOR_HOR_RES - 1) return;
    if(y1 > MONITOR_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > MONITOR_HOR_RES - 1 ? MONITOR_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > MONITOR_VER_RES - 1 ? MONITOR_VER_RES - 1 : y2;

	int32_t x;
	int32_t y;
	uint32_t color24 = lv_color_to24(color);

	for(x = act_x1; x <= act_x2; x++) {
		for(y = act_y1; y <= act_y2; y++) {
			tft_fb[y * MONITOR_HOR_RES + x] = color24;
		}
	}

	sdl_refr_qry = true;
}

/**
 * Put a color map to the marked area
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 * @param color_p an array of colors
 */
void monitor_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
   /*Return if the area is out the screen*/
	if(x2 < 0) return;
	if(y2 < 0) return;
	if(x1 > MONITOR_HOR_RES - 1) return;
	if(y1 > MONITOR_VER_RES - 1) return;

	/*Truncate the area to the screen*/
	int32_t act_x1 = x1 < 0 ? 0 : x1;
	int32_t act_y1 = y1 < 0 ? 0 : y1;
	int32_t act_x2 = x2 > MONITOR_HOR_RES - 1 ? MONITOR_HOR_RES - 1 : x2;
	int32_t act_y2 = y2 > MONITOR_VER_RES - 1 ? MONITOR_VER_RES - 1 : y2;

	int32_t x;
	int32_t y;

	for(y = act_y1; y <= act_y2; y++) {
		for(x = act_x1; x <= act_x2; x++) {
			tft_fb[y * MONITOR_HOR_RES + x] = lv_color_to24(*color_p);
			color_p++;
		}

		color_p += x2 - act_x2;
	}

	sdl_refr_qry = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */
static int sdl_refr(void * param)
{
    (void)param;

	/*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

	window = SDL_CreateWindow("TFT Simulator",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		MONITOR_HOR_RES, MONITOR_VER_RES, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, MONITOR_HOR_RES, MONITOR_VER_RES);

	/*Initialize the frame buffer to gray (77 is an empirical value) */
	memset(tft_fb, 77, MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(uint32_t));
	SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
	sdl_refr_qry = true;
	sdl_inited = true;

	/*Run until quit event not arrives*/
	while(sdl_quit_qry == false) {

		/*Refresh handling*/
		if(sdl_refr_qry != false) {
            sdl_refr_qry = false;
            SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
		}

	    SDL_Event event;
	    while(SDL_PollEvent(&event)) {
#if USE_MOUSE != 0
            mouse_handler(&event);
#endif

#if USE_KEYBOARD
            keyboard_handler(&event);
#endif
	    }

		/*Sleep some time*/
		SDL_Delay(SDL_REFR_PERIOD);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	exit(0);

	return 0;
}

int quit_filter (void *userdata, SDL_Event * event)
{
    (void)userdata;

	if(event->type == SDL_QUIT) {
		sdl_quit_qry = true;
	}

	return 1;
}

#endif
