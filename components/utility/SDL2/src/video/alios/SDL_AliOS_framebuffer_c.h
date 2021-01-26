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

#ifndef SDL_alios_framebuffer_c_h_
#define SDL_alios_framebuffer_c_h_

#include "../../SDL_internal.h"
#ifdef AOS_APP_GAME_DEMO
#include "hal_lcd.h"
#endif

extern int SDL_AliOS_CreateWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format, void ** pixels, int *pitch);
extern int SDL_AliOS_UpdateWindowFramebuffer(_THIS, SDL_Window * window, const SDL_Rect * rects, int numrects);
extern void SDL_AliOS_DestroyWindowFramebuffer(_THIS, SDL_Window * window);
#ifdef AOS_APP_GAME_DEMO
extern hal_lcd_t *get_hal_lcd(void);
#endif
#endif /* SDL_alios_framebuffer_c_h_ */

/* vi: set ts=4 sw=4 expandtab: */
