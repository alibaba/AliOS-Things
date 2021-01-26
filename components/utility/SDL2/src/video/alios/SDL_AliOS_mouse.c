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
/*We don't use it anymore since our driver doesn't not support cursor*/

#include "SDL_assert.h"

#include "SDL_AliOS_video.h"
#include "SDL_AliOS_mouse.h"
#include "SDL_touch.h"
#include "../SDL_sysvideo.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/default_cursor.h"

static SDL_Cursor *AliOS_CreateDefaultCursor(void);
static SDL_Cursor *AliOS_CreateCursor(SDL_Surface * surface,
                                         int hot_x, int hot_y);
static int AliOS_ShowCursor(SDL_Cursor * cursor);
static void AliOS_FreeCursor(SDL_Cursor * cursor);
static void AliOS_WarpMouse(SDL_Window * window, int x, int y);

static SDL_Cursor *
AliOS_CreateDefaultCursor(void)
{
    return SDL_CreateCursor(default_cdata, default_cmask, DEFAULT_CWIDTH, DEFAULT_CHEIGHT, DEFAULT_CHOTX, DEFAULT_CHOTY);
}

/* Create a cursor from a surface */
static SDL_Cursor *
AliOS_CreateCursor(SDL_Surface * surface, int hot_x, int hot_y)
{
    AliOS_CursorData *curdata;
    SDL_Cursor *cursor;
    int pitch, i;

    cursor = SDL_malloc(sizeof(SDL_Cursor));
    curdata = SDL_malloc(sizeof(AliOS_CursorData));

    curdata->hotx = hot_x;
    curdata->hoty = hot_y;
    curdata->surface = surface;
    cursor->driverdata = curdata;

    printf("AliOS_CreateCursor done\n");
    return cursor;
}

static SDL_Cursor *
AliOS_CreateSystemCursor(SDL_Surface * surface, int hot_x, int hot_y)
{
    return NULL;
}

/* Show the specified cursor, or hide if cursor is NULL */
static int
AliOS_ShowCursor(SDL_Cursor * cursor)
{
    SDL_Mouse *mouse;
    SDL_VideoDisplay *display;
    SDL_Window *window;
    SDL_Surface *surface;

    if (cursor) {
        surface = ((AliOS_CursorData *)cursor->driverdata)->surface;

        window = SDL_GetFocusWindow();
        if (!window)
            return -1;
        else {
            SDL_Surface *w_surface;
            w_surface = SDL_GetWindowSurface(window);
            SDL_BlitSurface(surface, NULL, w_surface, NULL);
            SDL_UpdateWindowSurface(window);
            printf("show cursor done\n");
        }
        printf("ShowCursor done\n");
    }

    return 0;
}

/* Free a window manager cursor */
static void
AliOS_FreeCursor(SDL_Cursor * cursor)
{

}

/* Warp the mouse to (x,y) */
static void
AliOS_WarpMouse(SDL_Window * window, int x, int y)
{
    printf("AliOS_WarpMouse done\n");
    return;
}

/* move cursor to (x,y) */
static void
AliOS_MoveCursor(SDL_Cursor *cursor)
{
    printf("AliOS_WarpMouse done\n");
    return;
}

void
AliOS_InitMouse(_THIS)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->CreateCursor = NULL;
    mouse->CreateSystemCursor = NULL;
    mouse->ShowCursor = NULL;
    mouse->WarpMouse = NULL;
    mouse->FreeCursor = NULL;
    mouse->MoveCursor = NULL;

    SDL_SetDefaultCursor(AliOS_CreateDefaultCursor());
    //SDL_AddTouch(1, SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE, "alios_touch");
    printf("AliOS_InitMouse done\n");
}

void
AliOS_QuitMouse(_THIS)
{
    return;
}

#endif /* SDL_VIDEO_DRIVER_ALIOS */

/* vi: set ts=4 sw=4 expandtab: */
