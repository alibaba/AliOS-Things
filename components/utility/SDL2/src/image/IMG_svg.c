/*
  SDL_image:  An example image loading library for use with SDL
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

/* This is an SVG image file loading framework, based on Nano SVG:
 * https://github.com/memononen/nanosvg
 */

#include "SDL_image.h"

#ifdef LOAD_SVG

/* Replace C runtime functions with SDL C runtime functions for building on Windows */
#define acosf   SDL_acosf
#define atan2f  SDL_atan2f
#define cosf    SDL_cosf
#define ceilf   SDL_ceilf
#define fabs    SDL_fabs
#define fabsf   SDL_fabsf
#define floorf  SDL_floorf
#define fmodf   SDL_fmodf
#define free    SDL_free
#define malloc  SDL_malloc
#undef memcpy
#define memcpy  SDL_memcpy
#undef memset
#define memset  SDL_memset
#define pow     SDL_pow
#define qsort   SDL_qsort
#define realloc SDL_realloc
#define sinf    SDL_sinf
#define sqrt    SDL_sqrt
#define sqrtf   SDL_sqrtf
#define sscanf  SDL_sscanf
#undef strchr
#define strchr  SDL_strchr
#undef strcmp
#define strcmp  SDL_strcmp
#undef strncmp
#define strncmp SDL_strncmp
#undef strncpy
#define strncpy SDL_strlcpy
#define strlen  SDL_strlen
#define strstr  SDL_strstr
#define strtol  SDL_strtol
#define strtoll SDL_strtoll
#define tanf    SDL_tanf
#ifndef FLT_MAX
#define FLT_MAX     3.402823466e+38F
#endif
#undef HAVE_STDIO_H

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

/* See if an image is contained in a data source */
int IMG_isSVG(SDL_RWops *src)
{
    Sint64 start;
    int is_SVG;
    char magic[4096];
    size_t magic_len;

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_SVG = 0;
    magic_len = SDL_RWread(src, magic, 1, sizeof(magic) - 1);
    magic[magic_len] = '\0';
    if ( SDL_strstr(magic, "<svg") ) {
        is_SVG = 1;
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_SVG);
}

/* Load a SVG type image from an SDL datasource */
SDL_Surface *IMG_LoadSVG_RW(SDL_RWops *src)
{
    char *data;
    struct NSVGimage *image;
    struct NSVGrasterizer *rasterizer;
    SDL_Surface *surface = NULL;
    float scale = 1.0f;

    data = (char *)SDL_LoadFile_RW(src, NULL, SDL_FALSE);
    if ( !data ) {
        return NULL;
    }

    /* For now just use default units of pixels at 96 DPI */
    image = nsvgParse(data, "px", 96.0f);
    SDL_free(data);
    if ( !image ) {
        IMG_SetError("Couldn't parse SVG image");
        return NULL;
    }

    rasterizer = nsvgCreateRasterizer();
    if ( !rasterizer ) {
        IMG_SetError("Couldn't create SVG rasterizer");
        nsvgDelete( image );
        return NULL;
    }

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                   (int)(image->width * scale),
                                   (int)(image->height * scale),
                                   32,
                                   0x000000FF,
                                   0x0000FF00,
                                   0x00FF0000,
                                   0xFF000000);
    if ( !surface ) {
        nsvgDeleteRasterizer( rasterizer );
        nsvgDelete( image );
        return NULL;
    }

    nsvgRasterize(rasterizer, image, 0.0f, 0.0f, scale, (unsigned char *)surface->pixels, surface->w, surface->h, surface->pitch);
    nsvgDeleteRasterizer( rasterizer );
    nsvgDelete( image );

    return surface;
}

#else

/* See if an image is contained in a data source */
int IMG_isSVG(SDL_RWops *src)
{
    return(0);
}

/* Load a SVG type image from an SDL datasource */
SDL_Surface *IMG_LoadSVG_RW(SDL_RWops *src)
{
    return(NULL);
}

#endif /* LOAD_SVG */
