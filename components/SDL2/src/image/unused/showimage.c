/*
  showimage:  A test application for the SDL image loading library.
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"


/* Draw a Gimpish background pattern to show transparency in the image */
static void draw_background(SDL_Renderer *renderer, int w, int h)
{
    SDL_Color col[2] = {
        { 0x66, 0x66, 0x66, 0xff },
        { 0x99, 0x99, 0x99, 0xff },
    };
    int i, x, y;
    SDL_Rect rect;

    rect.w = 8;
    rect.h = 8;
    for (y = 0; y < h; y += rect.h) {
        for (x = 0; x < w; x += rect.w) {
            /* use an 8x8 checkerboard pattern */
            i = (((x ^ y) >> 3) & 1);
            SDL_SetRenderDrawColor(renderer, col[i].r, col[i].g, col[i].b, col[i].a);

            rect.x = x;
            rect.y = y;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

int main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 flags;
    int i, w, h, done;
    SDL_Event event;
    const char *saveFile = NULL;

    /* Check command line usage */
    if ( ! argv[1] ) {
        SDL_Log("Usage: %s [-fullscreen] [-save file.png] <image_file> ...\n", argv[0]);
        return(1);
    }

    flags = SDL_WINDOW_HIDDEN;
    for ( i=1; argv[i]; ++i ) {
        if ( SDL_strcmp(argv[i], "-fullscreen") == 0 ) {
            SDL_ShowCursor(0);
            flags |= SDL_WINDOW_FULLSCREEN;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
        return(2);
    }

    if (SDL_CreateWindowAndRenderer(0, 0, flags, &window, &renderer) < 0) {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
        return(2);
    }

    for ( i=1; argv[i]; ++i ) {
        if ( SDL_strcmp(argv[i], "-fullscreen") == 0 ) {
            continue;
        }

        if ( SDL_strcmp(argv[i], "-save") == 0 && argv[i+1] ) {
            ++i;
            saveFile = argv[i];
            continue;
        }

        /* Open the image file */
        texture = IMG_LoadTexture(renderer, argv[i]);
        if (!texture) {
            SDL_Log("Couldn't load %s: %s\n", argv[i], SDL_GetError());
            continue;
        }
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        /* Save the image file, if desired */
        if ( saveFile ) {
            SDL_Surface *surface = IMG_Load(argv[i]);
            if (surface) {
                int result;
                const char *ext = SDL_strrchr(saveFile, '.');
                if ( ext && SDL_strcasecmp(ext, ".bmp") == 0 ) {
                    result = SDL_SaveBMP(surface, saveFile);
                } else if ( ext && SDL_strcasecmp(ext, ".jpg") == 0 ) {
                    result = IMG_SaveJPG(surface, saveFile, 90);
                } else {
                    result = IMG_SavePNG(surface, saveFile);
                }
                if ( result < 0 ) {
                    SDL_Log("Couldn't save %s: %s\n", saveFile, SDL_GetError());
                }
            } else {
                SDL_Log("Couldn't load %s: %s\n", argv[i], SDL_GetError());
            }
        }

        /* Show the window */
        SDL_SetWindowTitle(window, argv[i]);
        SDL_SetWindowSize(window, w, h);
        SDL_ShowWindow(window);

        done = 0;
        while ( ! done ) {
            while ( SDL_PollEvent(&event) ) {
                switch (event.type) {
                    case SDL_KEYUP:
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:
                                if ( i > 1 ) {
                                    i -= 2;
                                    done = 1;
                                }
                                break;
                            case SDLK_RIGHT:
                                if ( argv[i+1] ) {
                                    done = 1;
                                }
                                break;
                            case SDLK_ESCAPE:
                            case SDLK_q:
                                argv[i+1] = NULL;
                            /* Drop through to done */
                            case SDLK_SPACE:
                            case SDLK_TAB:
                            done = 1;
                            break;
                            default:
                            break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        done = 1;
                        break;
                    case SDL_QUIT:
                        argv[i+1] = NULL;
                        done = 1;
                        break;
                    default:
                        break;
                }
            }
            /* Draw a background pattern in case the image has transparency */
            draw_background(renderer, w, h);

            /* Display the image */
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_Delay(100);
        }
        SDL_DestroyTexture(texture);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    /* We're done! */
    SDL_Quit();
    return(0);
}
