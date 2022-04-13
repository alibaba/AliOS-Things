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

/* This is a PNG image file loading framework */

#include "SDL_image.h"

/* We'll always have PNG save support */
#define SAVE_PNG

#if !(defined(__APPLE__) || defined(SDL_IMAGE_USE_WIC_BACKEND)) || defined(SDL_IMAGE_USE_COMMON_BACKEND)

#ifdef LOAD_PNG

#define USE_LIBPNG

/*=============================================================================
        File: SDL_png.c
     Purpose: A PNG loader and saver for the SDL library
    Revision:
  Created by: Philippe Lavoie          (2 November 1998)
              lavoie@zeus.genie.uottawa.ca
 Modified by:

 Copyright notice:
          Copyright (C) 1998 Philippe Lavoie

          This library is free software; you can redistribute it and/or
          modify it under the terms of the GNU Library General Public
          License as published by the Free Software Foundation; either
          version 2 of the License, or (at your option) any later version.

          This library is distributed in the hope that it will be useful,
          but WITHOUT ANY WARRANTY; without even the implied warranty of
          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
          Library General Public License for more details.

          You should have received a copy of the GNU Library General Public
          License along with this library; if not, write to the Free
          Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Comments: The load and save routine are basically the ones you can find
             in the example.c file from the libpng distribution.

  Changes:
    5/17/99 - Modified to use the new SDL data sources - Sam Lantinga

=============================================================================*/

#include "SDL_endian.h"

#ifdef macintosh
#define MACOS
#endif
#include <png.h>

/* Check for the older version of libpng */
#if (PNG_LIBPNG_VER_MAJOR == 1)
#if (PNG_LIBPNG_VER_MINOR < 5)
#define LIBPNG_VERSION_12
typedef png_bytep png_const_bytep;
typedef png_color *png_const_colorp;
#endif
#if (PNG_LIBPNG_VER_MINOR < 4)
typedef png_structp png_const_structp;
typedef png_infop png_const_infop;
#endif
#if (PNG_LIBPNG_VER_MINOR < 6)
typedef png_structp png_structrp;
typedef png_infop png_inforp;
typedef png_const_structp png_const_structrp;
typedef png_const_infop png_const_inforp;
/* noconst15: version < 1.6 doesn't have const, >= 1.6 adds it */
/* noconst16: version < 1.6 does have const, >= 1.6 removes it */
typedef png_structp png_noconst15_structrp;
typedef png_inforp png_noconst15_inforp;
typedef png_const_inforp png_noconst16_inforp;
#else
typedef png_const_structp png_noconst15_structrp;
typedef png_const_inforp png_noconst15_inforp;
typedef png_inforp png_noconst16_inforp;
#endif
#else
typedef png_const_structp png_noconst15_structrp;
typedef png_const_inforp png_noconst15_inforp;
typedef png_inforp png_noconst16_inforp;
#endif

static struct {
    int loaded;
    void *handle;
    png_infop (*png_create_info_struct) (png_noconst15_structrp png_ptr);
    png_structp (*png_create_read_struct) (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
    void (*png_destroy_read_struct) (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
    png_uint_32 (*png_get_IHDR) (png_noconst15_structrp png_ptr, png_noconst15_inforp info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_method, int *compression_method, int *filter_method);
    png_voidp (*png_get_io_ptr) (png_noconst15_structrp png_ptr);
    png_byte (*png_get_channels) (png_const_structrp png_ptr, png_const_inforp info_ptr);
    png_uint_32 (*png_get_PLTE) (png_const_structrp png_ptr, png_noconst16_inforp info_ptr, png_colorp *palette, int *num_palette);
    png_uint_32 (*png_get_tRNS) (png_const_structrp png_ptr, png_inforp info_ptr, png_bytep *trans, int *num_trans, png_color_16p *trans_values);
    png_uint_32 (*png_get_valid) (png_const_structrp png_ptr, png_const_inforp info_ptr, png_uint_32 flag);
    void (*png_read_image) (png_structrp png_ptr, png_bytepp image);
    void (*png_read_info) (png_structrp png_ptr, png_inforp info_ptr);
    void (*png_read_update_info) (png_structrp png_ptr, png_inforp info_ptr);
    void (*png_set_expand) (png_structrp png_ptr);
    void (*png_set_gray_to_rgb) (png_structrp png_ptr);
    void (*png_set_packing) (png_structrp png_ptr);
    void (*png_set_read_fn) (png_structrp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
    void (*png_set_strip_16) (png_structrp png_ptr);
    int (*png_set_interlace_handling) (png_structrp png_ptr);
    int (*png_sig_cmp) (png_const_bytep sig, png_size_t start, png_size_t num_to_check);
#ifdef PNG_SETJMP_SUPPORTED
#ifndef LIBPNG_VERSION_12
    jmp_buf* (*png_set_longjmp_fn) (png_structrp, png_longjmp_ptr, size_t);
#endif
#endif
#ifdef SAVE_PNG
    png_structp (*png_create_write_struct) (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
    void (*png_destroy_write_struct) (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
    void (*png_set_write_fn) (png_structrp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);
    void (*png_set_IHDR) (png_noconst15_structrp png_ptr, png_inforp info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_type, int compression_type, int filter_type);
    void (*png_write_info) (png_structrp png_ptr, png_noconst15_inforp info_ptr);
    void (*png_set_rows) (png_noconst15_structrp png_ptr, png_inforp info_ptr, png_bytepp row_pointers);
    void (*png_write_png) (png_structrp png_ptr, png_inforp info_ptr, int transforms, png_voidp params);
    void (*png_set_PLTE) (png_structrp png_ptr, png_inforp info_ptr, png_const_colorp palette, int num_palette);
#endif
} lib;

#ifdef LOAD_PNG_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    lib.FUNC = (SIG) SDL_LoadFunction(lib.handle, #FUNC); \
    if (lib.FUNC == NULL) { SDL_UnloadObject(lib.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    lib.FUNC = FUNC;
#endif

int IMG_InitPNG()
{
    if ( lib.loaded == 0 ) {
#ifdef LOAD_PNG_DYNAMIC
        lib.handle = SDL_LoadObject(LOAD_PNG_DYNAMIC);
        if ( lib.handle == NULL ) {
            return -1;
        }
#endif
        FUNCTION_LOADER(png_create_info_struct, png_infop (*) (png_noconst15_structrp png_ptr))
        FUNCTION_LOADER(png_create_read_struct, png_structp (*) (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn))
        FUNCTION_LOADER(png_destroy_read_struct, void (*) (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr))
        FUNCTION_LOADER(png_get_IHDR, png_uint_32 (*) (png_noconst15_structrp png_ptr, png_noconst15_inforp info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_method, int *compression_method, int *filter_method))
        FUNCTION_LOADER(png_get_io_ptr, png_voidp (*) (png_noconst15_structrp png_ptr))
        FUNCTION_LOADER(png_get_channels, png_byte (*) (png_const_structrp png_ptr, png_const_inforp info_ptr))
        FUNCTION_LOADER(png_get_PLTE, png_uint_32 (*) (png_const_structrp png_ptr, png_noconst16_inforp info_ptr, png_colorp *palette, int *num_palette))
        FUNCTION_LOADER(png_get_tRNS, png_uint_32 (*) (png_const_structrp png_ptr, png_inforp info_ptr, png_bytep *trans, int *num_trans, png_color_16p *trans_values))
        FUNCTION_LOADER(png_get_valid, png_uint_32 (*) (png_const_structrp png_ptr, png_const_inforp info_ptr, png_uint_32 flag))
        FUNCTION_LOADER(png_read_image, void (*) (png_structrp png_ptr, png_bytepp image))
        FUNCTION_LOADER(png_read_info, void (*) (png_structrp png_ptr, png_inforp info_ptr))
        FUNCTION_LOADER(png_read_update_info, void (*) (png_structrp png_ptr, png_inforp info_ptr))
        FUNCTION_LOADER(png_set_expand, void (*) (png_structrp png_ptr))
        FUNCTION_LOADER(png_set_gray_to_rgb, void (*) (png_structrp png_ptr))
        FUNCTION_LOADER(png_set_packing, void (*) (png_structrp png_ptr))
        FUNCTION_LOADER(png_set_read_fn, void (*) (png_structrp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn))
        FUNCTION_LOADER(png_set_strip_16, void (*) (png_structrp png_ptr))
        FUNCTION_LOADER(png_set_interlace_handling, int (*) (png_structrp png_ptr))
        FUNCTION_LOADER(png_sig_cmp, int (*) (png_const_bytep sig, png_size_t start, png_size_t num_to_check))
#ifdef PNG_SETJMP_SUPPORTED
#ifndef LIBPNG_VERSION_12
        FUNCTION_LOADER(png_set_longjmp_fn, jmp_buf* (*) (png_structrp, png_longjmp_ptr, size_t))
#endif
#endif
#ifdef SAVE_PNG
        FUNCTION_LOADER(png_create_write_struct, png_structp (*) (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn))
        FUNCTION_LOADER(png_destroy_write_struct, void (*) (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr))
        FUNCTION_LOADER(png_set_write_fn, void (*) (png_structrp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn))
        FUNCTION_LOADER(png_set_IHDR, void (*) (png_noconst15_structrp png_ptr, png_inforp info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_type, int compression_type, int filter_type))
        FUNCTION_LOADER(png_write_info, void (*) (png_structrp png_ptr, png_noconst15_inforp info_ptr))
        FUNCTION_LOADER(png_set_rows, void (*) (png_noconst15_structrp png_ptr, png_inforp info_ptr, png_bytepp row_pointers))
        FUNCTION_LOADER(png_write_png, void (*) (png_structrp png_ptr, png_inforp info_ptr, int transforms, png_voidp params))
        FUNCTION_LOADER(png_set_PLTE, void (*) (png_structrp png_ptr, png_inforp info_ptr, png_const_colorp palette, int num_palette))
#endif
    }
    ++lib.loaded;

    return 0;
}
void IMG_QuitPNG()
{
    if ( lib.loaded == 0 ) {
        return;
    }
    if ( lib.loaded == 1 ) {
#ifdef LOAD_PNG_DYNAMIC
        SDL_UnloadObject(lib.handle);
#endif
    }
    --lib.loaded;
}

/* See if an image is contained in a data source */
int IMG_isPNG(SDL_RWops *src)
{
    Sint64 start;
    int is_PNG;
    Uint8 magic[4];

    if ( !src ) {
        return 0;
    }

    start = SDL_RWtell(src);
    is_PNG = 0;
    if ( SDL_RWread(src, magic, 1, sizeof(magic)) == sizeof(magic) ) {
        if ( magic[0] == 0x89 &&
             magic[1] == 'P' &&
             magic[2] == 'N' &&
             magic[3] == 'G' ) {
            is_PNG = 1;
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_PNG);
}

/* Load a PNG type image from an SDL datasource */
static void png_read_data(png_structp ctx, png_bytep area, png_size_t size)
{
    SDL_RWops *src;

    src = (SDL_RWops *)lib.png_get_io_ptr(ctx);
    SDL_RWread(src, area, size, 1);
}
SDL_Surface *IMG_LoadPNG_RW(SDL_RWops *src)
{
    Sint64 start;
    const char *error;
    SDL_Surface *volatile surface;
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, num_channels;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    SDL_Palette *palette;
    png_bytep *volatile row_pointers;
    int row, i;
    int ckey = -1;
    png_color_16 *transv;

    if ( !src ) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }
    start = SDL_RWtell(src);

    if ( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0 ) {
        return NULL;
    }

    /* Initialize the data we will clean up when we're done */
    error = NULL;
    png_ptr = NULL; info_ptr = NULL; row_pointers = NULL; surface = NULL;

    /* Create the PNG loading context structure */
    png_ptr = lib.png_create_read_struct(PNG_LIBPNG_VER_STRING,
                      NULL,NULL,NULL);
    if (png_ptr == NULL){
        error = "Couldn't allocate memory for PNG file or incompatible PNG dll";
        goto done;
    }

     /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = lib.png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        error = "Couldn't create image information for PNG file";
        goto done;
    }

    /* Set error handling if you are using setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in png_create_read_struct() earlier.
     */

#ifdef PNG_SETJMP_SUPPORTED
#ifndef LIBPNG_VERSION_12
    if ( setjmp(*lib.png_set_longjmp_fn(png_ptr, longjmp, sizeof (jmp_buf))) )
#else
    if ( setjmp(png_ptr->jmpbuf) )
#endif
    {
        error = "Error reading the PNG file.";
        goto done;
    }
#endif
    /* Set up the input control */
    lib.png_set_read_fn(png_ptr, src, png_read_data);

    /* Read PNG header info */
    lib.png_read_info(png_ptr, info_ptr);
    lib.png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
            &color_type, &interlace_type, NULL, NULL);

    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    lib.png_set_strip_16(png_ptr);

    /* tell libpng to de-interlace (if the image is interlaced) */
    lib.png_set_interlace_handling(png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    lib.png_set_packing(png_ptr);

    /* scale greyscale values to the range 0..255 */
    if (color_type == PNG_COLOR_TYPE_GRAY)
        lib.png_set_expand(png_ptr);

    /* For images with a single "transparent colour", set colour key;
       if more than one index has transparency, or if partially transparent
       entries exist, use full alpha channel */
    if (lib.png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        int num_trans;
        Uint8 *trans;
        lib.png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &transv);
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            /* Check if all tRNS entries are opaque except one */
            int j, t = -1;
            for (j = 0; j < num_trans; j++) {
                if (trans[j] == 0) {
                    if (t >= 0) {
                        break;
                    }
                    t = j;
                } else if (trans[j] != 255) {
                    break;
                }
            }
            if (j == num_trans) {
                /* exactly one transparent index */
                ckey = t;
            } else {
                /* more than one transparent index, or translucency */
                lib.png_set_expand(png_ptr);
            }
        } else {
            ckey = 0; /* actual value will be set later */
        }
    }

    if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        lib.png_set_gray_to_rgb(png_ptr);

    lib.png_read_update_info(png_ptr, info_ptr);

    lib.png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
            &color_type, &interlace_type, NULL, NULL);

    /* Allocate the SDL surface to hold the image */
    Rmask = Gmask = Bmask = Amask = 0 ;
    num_channels = lib.png_get_channels(png_ptr, info_ptr);
    if ( num_channels >= 3 ) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        Rmask = 0x000000FF;
        Gmask = 0x0000FF00;
        Bmask = 0x00FF0000;
        Amask = (num_channels == 4) ? 0xFF000000 : 0;
#else
        int s = (num_channels == 4) ? 0 : 8;
        Rmask = 0xFF000000 >> s;
        Gmask = 0x00FF0000 >> s;
        Bmask = 0x0000FF00 >> s;
        Amask = 0x000000FF >> s;
#endif
    }
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
            bit_depth*num_channels, Rmask,Gmask,Bmask,Amask);
    if ( surface == NULL ) {
        error = SDL_GetError();
        goto done;
    }

    if (ckey != -1) {
        if (color_type != PNG_COLOR_TYPE_PALETTE) {
            /* FIXME: Should these be truncated or shifted down? */
            ckey = SDL_MapRGB(surface->format,
                         (Uint8)transv->red,
                         (Uint8)transv->green,
                         (Uint8)transv->blue);
        }
        SDL_SetColorKey(surface, SDL_TRUE, ckey);
    }

    /* Create the array of pointers to image data */
    row_pointers = (png_bytep*) SDL_malloc(sizeof(png_bytep)*height);
    if (!row_pointers) {
        error = "Out of memory";
        goto done;
    }
    for (row = 0; row < (int)height; row++) {
        row_pointers[row] = (png_bytep)
                (Uint8 *)surface->pixels + row*surface->pitch;
    }

    /* Read the entire image in one go */
    lib.png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired)
     * In some cases it can't read PNG's created by some popular programs (ACDSEE),
     * we do not want to process comments, so we omit png_read_end

    lib.png_read_end(png_ptr, info_ptr);
    */

    /* Load the palette, if any */
    palette = surface->format->palette;
    if ( palette ) {
        int png_num_palette;
        png_colorp png_palette;
        lib.png_get_PLTE(png_ptr, info_ptr, &png_palette, &png_num_palette);
        if (color_type == PNG_COLOR_TYPE_GRAY) {
            palette->ncolors = 256;
            for (i = 0; i < 256; i++) {
                palette->colors[i].r = (Uint8)i;
                palette->colors[i].g = (Uint8)i;
                palette->colors[i].b = (Uint8)i;
            }
        } else if (png_num_palette > 0 ) {
            palette->ncolors = png_num_palette;
            for ( i=0; i<png_num_palette; ++i ) {
                palette->colors[i].b = png_palette[i].blue;
                palette->colors[i].g = png_palette[i].green;
                palette->colors[i].r = png_palette[i].red;
            }
        }
    }

done:   /* Clean up and return */
    if ( png_ptr ) {
        lib.png_destroy_read_struct(&png_ptr,
                                info_ptr ? &info_ptr : (png_infopp)0,
                                (png_infopp)0);
    }
    if ( row_pointers ) {
        SDL_free(row_pointers);
    }
    if ( error ) {
        SDL_RWseek(src, start, RW_SEEK_SET);
        if ( surface ) {
            SDL_FreeSurface(surface);
            surface = NULL;
        }
        IMG_SetError("%s", error);
    }
    return(surface);
}

#else

int IMG_InitPNG()
{
    IMG_SetError("PNG images are not supported");
    return(-1);
}

void IMG_QuitPNG()
{
}

/* See if an image is contained in a data source */
int IMG_isPNG(SDL_RWops *src)
{
    return(0);
}

/* Load a PNG type image from an SDL datasource */
SDL_Surface *IMG_LoadPNG_RW(SDL_RWops *src)
{
    return(NULL);
}

#endif /* LOAD_PNG */

#endif /* !defined(__APPLE__) || defined(SDL_IMAGE_USE_COMMON_BACKEND) */

#ifdef SAVE_PNG

int IMG_SavePNG(SDL_Surface *surface, const char *file)
{
    SDL_RWops *dst = SDL_RWFromFile(file, "wb");
    if (dst) {
        return IMG_SavePNG_RW(surface, dst, 1);
    } else {
        return -1;
    }
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
static const Uint32 png_format = SDL_PIXELFORMAT_ABGR8888;
#else
static const Uint32 png_format = SDL_PIXELFORMAT_RGBA8888;
#endif

#ifdef USE_LIBPNG

static void png_write_data(png_structp png_ptr, png_bytep src, png_size_t size)
{
    SDL_RWops *dst = (SDL_RWops *)lib.png_get_io_ptr(png_ptr);
    SDL_RWwrite(dst, src, size, 1);
}

static void png_flush_data(png_structp png_ptr)
{
}

static int IMG_SavePNG_RW_libpng(SDL_Surface *surface, SDL_RWops *dst, int freedst)
{
    if (dst) {
        png_structp png_ptr;
        png_infop info_ptr;
        png_colorp color_ptr = NULL;
        SDL_Surface *source = surface;
        SDL_Palette *palette;
        int png_color_type = PNG_COLOR_TYPE_RGB_ALPHA;

        png_ptr = lib.png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr == NULL) {
            SDL_SetError("Couldn't allocate memory for PNG file or incompatible PNG dll");
            return -1;
        }

        info_ptr = lib.png_create_info_struct(png_ptr);
        if (info_ptr == NULL) {
            lib.png_destroy_write_struct(&png_ptr, NULL);
            SDL_SetError("Couldn't create image information for PNG file");
            return -1;
        }
#ifdef PNG_SETJMP_SUPPORTED
#ifndef LIBPNG_VERSION_12
        if (setjmp(*lib.png_set_longjmp_fn(png_ptr, longjmp, sizeof (jmp_buf))))
#else
        if (setjmp(png_ptr->jmpbuf))
#endif
#endif
        {
            lib.png_destroy_write_struct(&png_ptr, &info_ptr);
            SDL_SetError("Error writing the PNG file.");
            return -1;
        }

        palette = surface->format->palette;
        if (palette) {
            const int ncolors = palette->ncolors;
            int i;

            color_ptr = SDL_malloc(sizeof(png_colorp) * ncolors);
            if (color_ptr == NULL)
            {
                lib.png_destroy_write_struct(&png_ptr, &info_ptr);
                SDL_SetError("Couldn't create palette for PNG file");
                return -1;
            }
            for (i = 0; i < ncolors; i++) {
                color_ptr[i].red = palette->colors[i].r;
                color_ptr[i].green = palette->colors[i].g;
                color_ptr[i].blue = palette->colors[i].b;
            }
            lib.png_set_PLTE(png_ptr, info_ptr, color_ptr, ncolors);
            png_color_type = PNG_COLOR_TYPE_PALETTE;
        }
        else if (surface->format->format != png_format) {
            source = SDL_ConvertSurfaceFormat(surface, png_format, 0);
        }

        lib.png_set_write_fn(png_ptr, dst, png_write_data, png_flush_data);

        lib.png_set_IHDR(png_ptr, info_ptr, surface->w, surface->h,
                         8, png_color_type, PNG_INTERLACE_NONE,
                         PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        if (source) {
            png_bytep *row_pointers;
            int row;

            row_pointers = (png_bytep *) SDL_malloc(sizeof(png_bytep) * source->h);
            if (!row_pointers) {
                lib.png_destroy_write_struct(&png_ptr, &info_ptr);
                SDL_SetError("Out of memory");
                return -1;
            }
            for (row = 0; row < (int)source->h; row++) {
                row_pointers[row] = (png_bytep) (Uint8 *) source->pixels + row * source->pitch;
            }

            lib.png_set_rows(png_ptr, info_ptr, row_pointers);
            lib.png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

            SDL_free(row_pointers);
            if (source != surface) {
                SDL_FreeSurface(source);
            }
        }
        lib.png_destroy_write_struct(&png_ptr, &info_ptr);
        if (color_ptr) {
            SDL_free(color_ptr);
        }
        if (freedst) {
            SDL_RWclose(dst);
        }
    } else {
        SDL_SetError("Passed NULL dst");
        return -1;
    }
    return 0;
}

#endif /* USE_LIBPNG */

/* Replace C runtime functions with SDL C runtime functions for building on Windows */
#define MINIZ_NO_STDIO
#define MINIZ_NO_TIME
#define MINIZ_SDL_MALLOC
#define MZ_ASSERT(x) SDL_assert(x)
#undef memcpy
#define memcpy  SDL_memcpy
#undef memset
#define memset  SDL_memset
#define strlen  SDL_strlen

#include "miniz.h"

static int IMG_SavePNG_RW_miniz(SDL_Surface *surface, SDL_RWops *dst, int freedst)
{
    int result = -1;

    if (dst) {
        size_t size = 0;
        void *png = NULL;

        if (surface->format->format == png_format) {
            png = tdefl_write_image_to_png_file_in_memory(surface->pixels, surface->w, surface->h, surface->format->BytesPerPixel, surface->pitch, &size);
        } else {
            SDL_Surface *cvt = SDL_ConvertSurfaceFormat(surface, png_format, 0);
            if (cvt) {
                png = tdefl_write_image_to_png_file_in_memory(cvt->pixels, cvt->w, cvt->h, cvt->format->BytesPerPixel, cvt->pitch, &size);
                SDL_FreeSurface(cvt);
            }
        }
        if (png) {
            if (SDL_RWwrite(dst, png, size, 1)) {
                result = 0;
            }
            SDL_free(png);
        } else {
            SDL_SetError("Failed to convert and save image");
        }
        if (freedst) {
            SDL_RWclose(dst);
        }
    } else {
        SDL_SetError("Passed NULL dst");
    }
    return result;
}

int IMG_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst)
{
    static int (*rw_func)(SDL_Surface *surface, SDL_RWops *dst, int freedst);

    if (!rw_func)
    {
#ifdef USE_LIBPNG
        if (IMG_Init(IMG_INIT_PNG)) {
            rw_func = IMG_SavePNG_RW_libpng;
        } else
#endif
            rw_func = IMG_SavePNG_RW_miniz;
    }

    return rw_func(surface, dst, freedst);
}

#endif /* SAVE_PNG */
