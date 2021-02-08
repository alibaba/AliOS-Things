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

/* This is a JPEG image file loading framework */

#include <stdio.h>
#include <setjmp.h>

#include "SDL_image.h"

#if !(defined(__APPLE__) || defined(SDL_IMAGE_USE_WIC_BACKEND)) || defined(SDL_IMAGE_USE_COMMON_BACKEND)

#ifdef LOAD_JPG

#define USE_JPEGLIB

#include <jpeglib.h>

#ifdef JPEG_TRUE  /* MinGW version of jpeg-8.x renamed TRUE to JPEG_TRUE etc. */
    typedef JPEG_boolean boolean;
    #define TRUE JPEG_TRUE
    #define FALSE JPEG_FALSE
#endif

/* Define this for fast loading and not as good image quality */
#define FAST_JPEG

/* Define this for quicker (but less perfect) JPEG identification */
#define FAST_IS_JPEG

static struct {
    int loaded;
    void *handle;
    void (*jpeg_calc_output_dimensions) (j_decompress_ptr cinfo);
    void (*jpeg_CreateDecompress) (j_decompress_ptr cinfo, int version, size_t structsize);
    void (*jpeg_destroy_decompress) (j_decompress_ptr cinfo);
    boolean (*jpeg_finish_decompress) (j_decompress_ptr cinfo);
    int (*jpeg_read_header) (j_decompress_ptr cinfo, boolean require_image);
    JDIMENSION (*jpeg_read_scanlines) (j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
    boolean (*jpeg_resync_to_restart) (j_decompress_ptr cinfo, int desired);
    boolean (*jpeg_start_decompress) (j_decompress_ptr cinfo);
    void (*jpeg_CreateCompress) (j_compress_ptr cinfo, int version, size_t structsize);
    void (*jpeg_start_compress) (j_compress_ptr cinfo, boolean write_all_tables);
    void (*jpeg_set_quality) (j_compress_ptr cinfo, int quality, boolean force_baseline);
    void (*jpeg_set_defaults) (j_compress_ptr cinfo);
    JDIMENSION (*jpeg_write_scanlines) (j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);
    void (*jpeg_finish_compress) (j_compress_ptr cinfo);
    void (*jpeg_destroy_compress) (j_compress_ptr cinfo);
    struct jpeg_error_mgr * (*jpeg_std_error) (struct jpeg_error_mgr * err);
} lib;

#ifdef LOAD_JPG_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    lib.FUNC = (SIG) SDL_LoadFunction(lib.handle, #FUNC); \
    if (lib.FUNC == NULL) { SDL_UnloadObject(lib.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    lib.FUNC = FUNC;
#endif

int IMG_InitJPG()
{
    if ( lib.loaded == 0 ) {
#ifdef LOAD_JPG_DYNAMIC
        lib.handle = SDL_LoadObject(LOAD_JPG_DYNAMIC);
        if ( lib.handle == NULL ) {
            return -1;
        }
#endif
        FUNCTION_LOADER(jpeg_calc_output_dimensions, void (*) (j_decompress_ptr cinfo))
        FUNCTION_LOADER(jpeg_CreateDecompress, void (*) (j_decompress_ptr cinfo, int version, size_t structsize))
        FUNCTION_LOADER(jpeg_destroy_decompress, void (*) (j_decompress_ptr cinfo))
        FUNCTION_LOADER(jpeg_finish_decompress, boolean (*) (j_decompress_ptr cinfo))
        FUNCTION_LOADER(jpeg_read_header, int (*) (j_decompress_ptr cinfo, boolean require_image))
        FUNCTION_LOADER(jpeg_read_scanlines, JDIMENSION (*) (j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines))
        FUNCTION_LOADER(jpeg_resync_to_restart, boolean (*) (j_decompress_ptr cinfo, int desired))
        FUNCTION_LOADER(jpeg_start_decompress, boolean (*) (j_decompress_ptr cinfo))
        FUNCTION_LOADER(jpeg_CreateCompress, void (*) (j_compress_ptr cinfo, int version, size_t structsize))
        FUNCTION_LOADER(jpeg_start_compress, void (*) (j_compress_ptr cinfo, boolean write_all_tables))
        FUNCTION_LOADER(jpeg_set_quality, void (*) (j_compress_ptr cinfo, int quality, boolean force_baseline))
        FUNCTION_LOADER(jpeg_set_defaults, void (*) (j_compress_ptr cinfo))
        FUNCTION_LOADER(jpeg_write_scanlines, JDIMENSION (*) (j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines))
        FUNCTION_LOADER(jpeg_finish_compress, void (*) (j_compress_ptr cinfo))
        FUNCTION_LOADER(jpeg_destroy_compress, void (*) (j_compress_ptr cinfo))
        FUNCTION_LOADER(jpeg_std_error, struct jpeg_error_mgr * (*) (struct jpeg_error_mgr * err))
    }
    ++lib.loaded;

    return 0;
}
void IMG_QuitJPG()
{
    if ( lib.loaded == 0 ) {
        return;
    }
    if ( lib.loaded == 1 ) {
#ifdef LOAD_JPG_DYNAMIC
        SDL_UnloadObject(lib.handle);
#endif
    }
    --lib.loaded;
}

/* See if an image is contained in a data source */
int IMG_isJPG(SDL_RWops *src)
{
    Sint64 start;
    int is_JPG;
    int in_scan;
    Uint8 magic[4];

    /* This detection code is by Steaphan Greene <stea@cs.binghamton.edu> */
    /* Blame me, not Sam, if this doesn't work right. */
    /* And don't forget to report the problem to the the sdl list too! */

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_JPG = 0;
    in_scan = 0;
    if ( SDL_RWread(src, magic, 2, 1) ) {
        if ( (magic[0] == 0xFF) && (magic[1] == 0xD8) ) {
            is_JPG = 1;
            while (is_JPG == 1) {
                if(SDL_RWread(src, magic, 1, 2) != 2) {
                    is_JPG = 0;
                } else if( (magic[0] != 0xFF) && (in_scan == 0) ) {
                    is_JPG = 0;
                } else if( (magic[0] != 0xFF) || (magic[1] == 0xFF) ) {
                    /* Extra padding in JPEG (legal) */
                    /* or this is data and we are scanning */
                    SDL_RWseek(src, -1, RW_SEEK_CUR);
                } else if(magic[1] == 0xD9) {
                    /* Got to end of good JPEG */
                    break;
                } else if( (in_scan == 1) && (magic[1] == 0x00) ) {
                    /* This is an encoded 0xFF within the data */
                } else if( (magic[1] >= 0xD0) && (magic[1] < 0xD9) ) {
                    /* These have nothing else */
                } else if(SDL_RWread(src, magic+2, 1, 2) != 2) {
                    is_JPG = 0;
                } else {
                    /* Yes, it's big-endian */
                    Sint64 innerStart;
                    Uint32 size;
                    Sint64 end;
                    innerStart = SDL_RWtell(src);
                    size = (magic[2] << 8) + magic[3];
                    end = SDL_RWseek(src, size-2, RW_SEEK_CUR);
                    if ( end != innerStart + size - 2 ) is_JPG = 0;
                    if ( magic[1] == 0xDA ) {
                        /* Now comes the actual JPEG meat */
#ifdef  FAST_IS_JPEG
                        /* Ok, I'm convinced.  It is a JPEG. */
                        break;
#else
                        /* I'm not convinced.  Prove it! */
                        in_scan = 1;
#endif
                    }
                }
            }
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_JPG);
}

#define INPUT_BUFFER_SIZE   4096
typedef struct {
    struct jpeg_source_mgr pub;

    SDL_RWops *ctx;
    Uint8 buffer[INPUT_BUFFER_SIZE];
} my_source_mgr;

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */
static void init_source (j_decompress_ptr cinfo)
{
    /* We don't actually need to do anything */
    return;
}

/*
 * Fill the input buffer --- called whenever buffer is emptied.
 */
static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
    my_source_mgr * src = (my_source_mgr *) cinfo->src;
    int nbytes;

    nbytes = (int)SDL_RWread(src->ctx, src->buffer, 1, INPUT_BUFFER_SIZE);
    if (nbytes <= 0) {
        /* Insert a fake EOI marker */
        src->buffer[0] = (Uint8) 0xFF;
        src->buffer[1] = (Uint8) JPEG_EOI;
        nbytes = 2;
    }
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;

    return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    my_source_mgr * src = (my_source_mgr *) cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->pub.bytes_in_buffer) {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) src->pub.fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never
             * return FALSE, so suspension need not be handled.
             */
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.
 */
static void term_source (j_decompress_ptr cinfo)
{
    /* We don't actually need to do anything */
    return;
}

/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */
static void jpeg_SDL_RW_src (j_decompress_ptr cinfo, SDL_RWops *ctx)
{
  my_source_mgr *src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) { /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                  sizeof(my_source_mgr));
    src = (my_source_mgr *) cinfo->src;
  }

  src = (my_source_mgr *) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = lib.jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->ctx = ctx;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

struct my_error_mgr {
    struct jpeg_error_mgr errmgr;
    jmp_buf escape;
};

static void my_error_exit(j_common_ptr cinfo)
{
    struct my_error_mgr *err = (struct my_error_mgr *)cinfo->err;
    longjmp(err->escape, 1);
}

static void output_no_message(j_common_ptr cinfo)
{
    /* do nothing */
}

/* Load a JPEG type image from an SDL datasource */
SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src)
{
    Sint64 start;
    struct jpeg_decompress_struct cinfo;
    JSAMPROW rowptr[1];
    SDL_Surface *volatile surface = NULL;
    struct my_error_mgr jerr;

    if ( !src ) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }
    start = SDL_RWtell(src);

    if ( (IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == 0 ) {
        return NULL;
    }

    /* Create a decompression structure and load the JPEG header */
    cinfo.err = lib.jpeg_std_error(&jerr.errmgr);
    jerr.errmgr.error_exit = my_error_exit;
    jerr.errmgr.output_message = output_no_message;
    if(setjmp(jerr.escape)) {
        /* If we get here, libjpeg found an error */
        lib.jpeg_destroy_decompress(&cinfo);
        if ( surface != NULL ) {
            SDL_FreeSurface(surface);
        }
        SDL_RWseek(src, start, RW_SEEK_SET);
        IMG_SetError("JPEG loading error");
        return NULL;
    }

    lib.jpeg_create_decompress(&cinfo);
    jpeg_SDL_RW_src(&cinfo, src);
    lib.jpeg_read_header(&cinfo, TRUE);

    if(cinfo.num_components == 4) {
        /* Set 32-bit Raw output */
        cinfo.out_color_space = JCS_CMYK;
        cinfo.quantize_colors = FALSE;
        lib.jpeg_calc_output_dimensions(&cinfo);

        /* Allocate an output surface to hold the image */
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                cinfo.output_width, cinfo.output_height, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                           0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
#else
                           0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF);
#endif
    } else {
        /* Set 24-bit RGB output */
        cinfo.out_color_space = JCS_RGB;
        cinfo.quantize_colors = FALSE;
#ifdef FAST_JPEG
        cinfo.scale_num   = 1;
        cinfo.scale_denom = 1;
        cinfo.dct_method = JDCT_FASTEST;
        cinfo.do_fancy_upsampling = FALSE;
#endif
        lib.jpeg_calc_output_dimensions(&cinfo);

        /* Allocate an output surface to hold the image */
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                cinfo.output_width, cinfo.output_height, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                           0x0000FF, 0x00FF00, 0xFF0000,
#else
                           0xFF0000, 0x00FF00, 0x0000FF,
#endif
                           0);
    }

    if ( surface == NULL ) {
        lib.jpeg_destroy_decompress(&cinfo);
        SDL_RWseek(src, start, RW_SEEK_SET);
        IMG_SetError("Out of memory");
        return NULL;
    }

    /* Decompress the image */
    lib.jpeg_start_decompress(&cinfo);
    while ( cinfo.output_scanline < cinfo.output_height ) {
        rowptr[0] = (JSAMPROW)(Uint8 *)surface->pixels +
                            cinfo.output_scanline * surface->pitch;
        lib.jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
    }
    lib.jpeg_finish_decompress(&cinfo);
    lib.jpeg_destroy_decompress(&cinfo);

    return(surface);
}

#define OUTPUT_BUFFER_SIZE   4096
typedef struct {
    struct jpeg_destination_mgr pub;

    SDL_RWops *ctx;
    Uint8 buffer[OUTPUT_BUFFER_SIZE];
} my_destination_mgr;

static void init_destination(j_compress_ptr cinfo)
{
    /* We don't actually need to do anything */
    return;
}

static boolean empty_output_buffer(j_compress_ptr cinfo)
{
    my_destination_mgr * dest = (my_destination_mgr *)cinfo->dest;

    /* In typical applications, it should write out the *entire* buffer */
    SDL_RWwrite(dest->ctx, dest->buffer, 1, OUTPUT_BUFFER_SIZE);
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;

    return TRUE;
}

static void term_destination(j_compress_ptr cinfo)
{
    my_destination_mgr * dest = (my_destination_mgr *)cinfo->dest;

    /*  In most applications, this must flush any data remaining in the buffer */
    SDL_RWwrite(dest->ctx, dest->buffer, 1, OUTPUT_BUFFER_SIZE - dest->pub.free_in_buffer);
}

static void jpeg_SDL_RW_dest(j_compress_ptr cinfo, SDL_RWops *ctx)
{
    my_destination_mgr *dest;

    if (cinfo->dest == NULL) {
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_PERMANENT,
            sizeof(my_destination_mgr));
        dest = (my_destination_mgr *)cinfo->dest;
    }

    dest = (my_destination_mgr *)cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
    dest->ctx = ctx;
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUFFER_SIZE;
}

static int IMG_SaveJPG_RW_jpeglib(SDL_Surface *surface, SDL_RWops *dst, int freedst, int quality)
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    static const Uint32 jpg_format = SDL_PIXELFORMAT_RGB24;
#else
    static const Uint32 jpg_format = SDL_PIXELFORMAT_BGR24;
#endif
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPROW row_pointer[1];
    SDL_Surface* jpeg_surface = surface;
    int result = -1;

    if (!dst) {
        SDL_SetError("Passed NULL dst");
        goto done;
    }

    if (!IMG_Init(IMG_INIT_JPG)) {
        goto done;
    }

    /* Convert surface to format we can save */
    if (surface->format->format != jpg_format) {
        jpeg_surface = SDL_ConvertSurfaceFormat(surface, jpg_format, 0);
        if (!jpeg_surface) {
            goto done;
        }
    }

    /* Create a decompression structure and load the JPEG header */
    cinfo.err = lib.jpeg_std_error(&jerr.errmgr);
    jerr.errmgr.error_exit = my_error_exit;
    jerr.errmgr.output_message = output_no_message;

    lib.jpeg_create_compress(&cinfo);
    jpeg_SDL_RW_dest(&cinfo, dst);

    cinfo.image_width = jpeg_surface->w;
    cinfo.image_height = jpeg_surface->h;
    cinfo.in_color_space = JCS_RGB;
    cinfo.input_components = 3;

    lib.jpeg_set_defaults(&cinfo);
    lib.jpeg_set_quality(&cinfo, quality, TRUE);
    lib.jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        int offset = cinfo.next_scanline * jpeg_surface->pitch;
        row_pointer[0] = ((Uint8*)jpeg_surface->pixels) + offset;
        lib.jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    lib.jpeg_finish_compress(&cinfo);
    lib.jpeg_destroy_compress(&cinfo);

    if (jpeg_surface != surface) {
        SDL_FreeSurface(jpeg_surface);
    }

    result = 0;

done:
    if (freedst) {
        SDL_RWclose(dst);
    }
    return result;
}

#else

int IMG_InitJPG()
{
    IMG_SetError("JPEG images are not supported");
    return(-1);
}

void IMG_QuitJPG()
{
}

/* See if an image is contained in a data source */
int IMG_isJPG(SDL_RWops *src)
{
    return(0);
}

/* Load a JPEG type image from an SDL datasource */
SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src)
{
    return(NULL);
}

#endif /* LOAD_JPG */

#endif /* !defined(__APPLE__) || defined(SDL_IMAGE_USE_COMMON_BACKEND) */

/* We'll always have JPG save support */
#define SAVE_JPG

#ifdef SAVE_JPG

int IMG_SaveJPG(SDL_Surface *surface, const char *file, int quality)
{
    SDL_RWops *dst = SDL_RWFromFile(file, "wb");
    if (dst) {
        return IMG_SaveJPG_RW(surface, dst, 1, quality);
    } else {
        return -1;
    }
}

int IMG_SaveJPG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst, int quality)
{
#ifdef USE_JPEGLIB
    return IMG_SaveJPG_RW_jpeglib(surface, dst, freedst, quality);
#else
    return IMG_SetError("SDL_image not built with jpeglib, saving not supported");
#endif
}

#endif /* SAVE_JPG */
