/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_ALIOS

/* Dummy SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "AliOS" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_AliOS_video.h"
#include "SDL_AliOS_events_c.h"
#include "SDL_AliOS_framebuffer_c.h"

#define ALIOSVID_DRIVER_NAME "alios"

/* Initialization/Query functions */
static int AliOS_VideoInit(_THIS);
static int AliOS_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode);
static void AliOS_VideoQuit(_THIS);

/* AliOS driver bootstrap functions */

static int
AliOS_Available(void)
{
#ifdef __ALIOS__
    return (1);
#else
    const char *envr = SDL_getenv("SDL_VIDEODRIVER");
    if ((envr) && (SDL_strcmp(envr, ALIOSVID_DRIVER_NAME) == 0)) {
        return (1);
    }

    return (0);
#endif
}

static void
AliOS_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_free(device);
}

static SDL_VideoDevice *
AliOS_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return (0);
    }
    device->is_dummy = SDL_TRUE;

    /* Set the function pointers */
    device->VideoInit = AliOS_VideoInit;
    device->VideoQuit = AliOS_VideoQuit;
    device->SetDisplayMode = AliOS_SetDisplayMode;
    device->PumpEvents = AliOS_PumpEvents;
    device->CreateWindowFramebuffer = SDL_AliOS_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = SDL_AliOS_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = SDL_AliOS_DestroyWindowFramebuffer;

    device->free = AliOS_DeleteDevice;

    return device;
}

VideoBootStrap AliOS_bootstrap = {
    ALIOSVID_DRIVER_NAME, "SDL alios video driver",
    AliOS_Available, AliOS_CreateDevice
};


int
AliOS_VideoInit(_THIS)
{
    SDL_DisplayMode mode;

    /* Use a fake 32-bpp desktop mode */
#ifdef AOS_COMP_LCD
#ifdef AOS_APP_GAME_DEMO
    hal_lcd_t *hal_lcd = get_hal_lcd();
    uint8_t *frame;
    mode.format = SDL_PIXELFORMAT_RGB565;
    mode.w = 240;
    mode.h = 320;
    frame = malloc(mode.w*mode.h*2);
#else
    hal_lcd_t *hal_lcd = get_hal_lcd();
    uint8_t *frame;
    mode.format = SDL_PIXELFORMAT_BGR565;
    mode.w = 320;
    mode.h = 240;
    frame = malloc(mode.w*mode.h*2);
#endif
#endif
#ifdef AOS_COMP_UDISPLAY
    mode.format = SDL_PIXELFORMAT_BGR565;
    mode.w = 320;
    mode.h = 240;
#endif
    mode.refresh_rate = 0;
    mode.driverdata = NULL;
    if (SDL_AddBasicVideoDisplay(&mode) < 0) {
        return -1;
    }

    SDL_zero(mode);
    SDL_AddDisplayMode(&_this->displays[0], &mode);

#ifdef SDL_INPUT_LINUXEV
    SDL_EVDEV_Init();
#endif

#ifdef AOS_COMP_LCD
#ifdef AOS_APP_GAME_DEMO
    hal_lcd->lcd_init();
    hal_lcd->lcd_rotation_set(HAL_LCD_ROTATE_0);
    memset(frame, 0xffff, mode.w*mode.h*2);
    hal_lcd->lcd_frame_draw(frame);
    free(frame);
#else
    hal_lcd->lcd_init();
    hal_lcd->lcd_rotation_set(HAL_LCD_ROTATE_90);
    memset(frame, 0xffff, mode.w*mode.h*2);
    hal_lcd->lcd_frame_draw(frame);
    free(frame);
#endif
#endif
    //AliOS_InitMouse(_this);

#ifdef AOS_COMP_UDISPLAY
    udisplay_init();
#endif
    /* We're done! */
    return 0;
}

static int
AliOS_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
    return 0;
}

void
AliOS_VideoQuit(_THIS)
{
}

#endif /* SDL_VIDEO_DRIVER_ALIOS */

/* vi: set ts=4 sw=4 expandtab: */
