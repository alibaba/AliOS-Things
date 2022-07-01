/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * QR-code recognition library.
 */
#include "imlib.h"
#ifdef IMLIB_ENABLE_QRCODES
////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "quirc.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

struct quirc;

/* Obtain the library version string. */
const char *quirc_version(void);

/* Construct a new QR-code recognizer. This function will return NULL
 * if sufficient memory could not be allocated.
 */
struct quirc *quirc_new(void);

/* Destroy a QR-code recognizer. */
void quirc_destroy(struct quirc *q);

/* Resize the QR-code recognizer. The size of an image must be
 * specified before codes can be analyzed.
 *
 * This function returns 0 on success, or -1 if sufficient memory could
 * not be allocated.
 */
int quirc_resize(struct quirc *q, int w, int h);

/* These functions are used to process images for QR-code recognition.
 * quirc_begin() must first be called to obtain access to a buffer into
 * which the input image should be placed. Optionally, the current
 * width and height may be returned.
 *
 * After filling the buffer, quirc_end() should be called to process
 * the image for QR-code recognition. The locations and content of each
 * code may be obtained using accessor functions described below.
 */
uint8_t *quirc_begin(struct quirc *q, int *w, int *h);
void quirc_end(struct quirc *q);

/* This structure describes a location in the input image buffer. */
struct quirc_point {
    int x;
    int y;
};

/* This enum describes the various decoder errors which may occur. */
typedef enum {
    QUIRC_SUCCESS = 0,
    QUIRC_ERROR_INVALID_GRID_SIZE,
    QUIRC_ERROR_INVALID_VERSION,
    QUIRC_ERROR_FORMAT_ECC,
    QUIRC_ERROR_DATA_ECC,
    QUIRC_ERROR_UNKNOWN_DATA_TYPE,
    QUIRC_ERROR_DATA_OVERFLOW,
    QUIRC_ERROR_DATA_UNDERFLOW
} quirc_decode_error_t;

/* Return a string error message for an error code. */
const char *quirc_strerror(quirc_decode_error_t err);

/* Limits on the maximum size of QR-codes and their content. */
#define QUIRC_MAX_BITMAP    3917
#define QUIRC_MAX_PAYLOAD   8896

/* QR-code ECC types. */
#define QUIRC_ECC_LEVEL_M   0
#define QUIRC_ECC_LEVEL_L   1
#define QUIRC_ECC_LEVEL_H   2
#define QUIRC_ECC_LEVEL_Q   3

/* QR-code data types. */
#define QUIRC_DATA_TYPE_NUMERIC 1
#define QUIRC_DATA_TYPE_ALPHA   2
#define QUIRC_DATA_TYPE_BYTE    4
#define QUIRC_DATA_TYPE_KANJI   8

/* Common character encodings */
#define QUIRC_ECI_ISO_8859_1    1
#define QUIRC_ECI_IBM437        2
#define QUIRC_ECI_ISO_8859_2    4
#define QUIRC_ECI_ISO_8859_3    5
#define QUIRC_ECI_ISO_8859_4    6
#define QUIRC_ECI_ISO_8859_5    7
#define QUIRC_ECI_ISO_8859_6    8
#define QUIRC_ECI_ISO_8859_7    9
#define QUIRC_ECI_ISO_8859_8    10
#define QUIRC_ECI_ISO_8859_9    11
#define QUIRC_ECI_WINDOWS_874   13
#define QUIRC_ECI_ISO_8859_13   15
#define QUIRC_ECI_ISO_8859_15   17
#define QUIRC_ECI_SHIFT_JIS     20
#define QUIRC_ECI_UTF_8         26

/* This structure is used to return information about detected QR codes
 * in the input image.
 */
struct quirc_code {
    /* The four corners of the QR-code, from top left, clockwise */
    struct quirc_point  corners[4];

    /* The number of cells across in the QR-code. The cell bitmap
     * is a bitmask giving the actual values of cells. If the cell
     * at (x, y) is black, then the following bit is set:
     *
     *      cell_bitmap[i >> 3] & (1 << (i & 7))
     *
     * where i = (y * size) + x.
     */
    int                 size;
    uint8_t             cell_bitmap[QUIRC_MAX_BITMAP];
};

/* This structure holds the decoded QR-code data */
struct quirc_data {
    /* Various parameters of the QR-code. These can mostly be
     * ignored if you only care about the data.
     */
    int         version;
    int         ecc_level;
    int         mask;

    /* This field is the highest-valued data type found in the QR
     * code.
     */
    int         data_type;

    /* Data payload. For the Kanji datatype, payload is encoded as
     * Shift-JIS. For all other datatypes, payload is ASCII text.
     */
    uint8_t     payload[QUIRC_MAX_PAYLOAD];
    int         payload_len;

    /* ECI assignment number */
    uint32_t    eci;
};

/* Return the number of QR-codes identified in the last processed
 * image.
 */
int quirc_count(const struct quirc *q);

/* Extract the QR-code specified by the given index. */
void quirc_extract(const struct quirc *q, int index,
                   struct quirc_code *code);

/* Decode a QR-code, returning the payload data. */
quirc_decode_error_t quirc_decode(const struct quirc_code *code,
                                  struct quirc_data *data);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "quirc_internal.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define QUIRC_PIXEL_WHITE   0
#define QUIRC_PIXEL_BLACK   1
#define QUIRC_PIXEL_REGION  2

#ifndef QUIRC_MAX_REGIONS
#define QUIRC_MAX_REGIONS   254
#endif

#define QUIRC_MAX_CAPSTONES 32
#define QUIRC_MAX_GRIDS     8

#define QUIRC_PERSPECTIVE_PARAMS    8

#if QUIRC_MAX_REGIONS < UINT8_MAX
typedef uint8_t quirc_pixel_t;
#elif QUIRC_MAX_REGIONS < UINT16_MAX
typedef uint16_t quirc_pixel_t;
#else
#error "QUIRC_MAX_REGIONS > 65534 is not supported"
#endif

struct quirc_region {
    struct quirc_point  seed;
    int                 count;
    int                 capstone;
};

struct quirc_capstone {
    int                 ring;
    int                 stone;

    struct quirc_point  corners[4];
    struct quirc_point  center;
    float               c[QUIRC_PERSPECTIVE_PARAMS];

    int                 qr_grid;
};

struct quirc_grid {
    /* Capstone indices */
    int                 caps[3];

    /* Alignment pattern region and corner */
    int                 align_region;
    struct quirc_point  align;

    /* Timing pattern endpoints */
    struct quirc_point  tpep[3];
    int                 hscan;
    int                 vscan;

    /* Grid size and perspective transform */
    int                 grid_size;
    float               c[QUIRC_PERSPECTIVE_PARAMS];
};

struct quirc {
    uint8_t                 *image;
    quirc_pixel_t           *pixels;
    int                     w;
    int                     h;

    int                     num_regions;
    struct quirc_region     regions[QUIRC_MAX_REGIONS];

    int                     num_capstones;
    struct quirc_capstone   capstones[QUIRC_MAX_CAPSTONES];

    int                     num_grids;
    struct quirc_grid       grids[QUIRC_MAX_GRIDS];
};

/************************************************************************
 * QR-code version information database
 */

#define QUIRC_MAX_VERSION   40
#define QUIRC_MAX_ALIGNMENT 7

struct quirc_rs_params {
    uint8_t bs; /* Small block size */
    uint8_t dw; /* Small data words */
    uint8_t ns; /* Number of small blocks */
};

struct quirc_version_info {
    uint16_t                data_bytes;
    uint8_t                 apat[QUIRC_MAX_ALIGNMENT];
    struct quirc_rs_params  ecc[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "version_db.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

const struct quirc_version_info quirc_version_db[QUIRC_MAX_VERSION + 1] = {
    {0},
    { /* Version 1 */
        .data_bytes = 26,
        .apat = {0},
        .ecc = {
            {.bs = 26, .dw = 16, .ns = 1},
            {.bs = 26, .dw = 19, .ns = 1},
            {.bs = 26, .dw = 9, .ns = 1},
            {.bs = 26, .dw = 13, .ns = 1}
        }
    },
    { /* Version 2 */
        .data_bytes = 44,
        .apat = {6, 18, 0},
        .ecc = {
            {.bs = 44, .dw = 28, .ns = 1},
            {.bs = 44, .dw = 34, .ns = 1},
            {.bs = 44, .dw = 16, .ns = 1},
            {.bs = 44, .dw = 22, .ns = 1}
        }
    },
    { /* Version 3 */
        .data_bytes = 70,
        .apat = {6, 22, 0},
        .ecc = {
            {.bs = 70, .dw = 44, .ns = 1},
            {.bs = 70, .dw = 55, .ns = 1},
            {.bs = 35, .dw = 13, .ns = 2},
            {.bs = 35, .dw = 17, .ns = 2}
        }
    },
    { /* Version 4 */
        .data_bytes = 100,
        .apat = {6, 26, 0},
        .ecc = {
            {.bs = 50, .dw = 32, .ns = 2},
            {.bs = 100, .dw = 80, .ns = 1},
            {.bs = 25, .dw = 9, .ns = 4},
            {.bs = 50, .dw = 24, .ns = 2}
        }
    },
    { /* Version 5 */
        .data_bytes = 134,
        .apat = {6, 30, 0},
        .ecc = {
            {.bs = 67, .dw = 43, .ns = 2},
            {.bs = 134, .dw = 108, .ns = 1},
            {.bs = 33, .dw = 11, .ns = 2},
            {.bs = 33, .dw = 15, .ns = 2}
        }
    },
    { /* Version 6 */
        .data_bytes = 172,
        .apat = {6, 34, 0},
        .ecc = {
            {.bs = 43, .dw = 27, .ns = 4},
            {.bs = 86, .dw = 68, .ns = 2},
            {.bs = 43, .dw = 15, .ns = 4},
            {.bs = 43, .dw = 19, .ns = 4}
        }
    },
    { /* Version 7 */
        .data_bytes = 196,
        .apat = {6, 22, 38, 0},
        .ecc = {
            {.bs = 49, .dw = 31, .ns = 4},
            {.bs = 98, .dw = 78, .ns = 2},
            {.bs = 39, .dw = 13, .ns = 4},
            {.bs = 32, .dw = 14, .ns = 2}
        }
    },
    { /* Version 8 */
        .data_bytes = 242,
        .apat = {6, 24, 42, 0},
        .ecc = {
            {.bs = 60, .dw = 38, .ns = 2},
            {.bs = 121, .dw = 97, .ns = 2},
            {.bs = 40, .dw = 14, .ns = 4},
            {.bs = 40, .dw = 18, .ns = 4}
        }
    },
    { /* Version 9 */
        .data_bytes = 292,
        .apat = {6, 26, 46, 0},
        .ecc = {
            {.bs = 58, .dw = 36, .ns = 3},
            {.bs = 146, .dw = 116, .ns = 2},
            {.bs = 36, .dw = 12, .ns = 4},
            {.bs = 36, .dw = 16, .ns = 4}
        }
    },
    { /* Version 10 */
        .data_bytes = 346,
        .apat = {6, 28, 50, 0},
        .ecc = {
            {.bs = 69, .dw = 43, .ns = 4},
            {.bs = 86, .dw = 68, .ns = 2},
            {.bs = 43, .dw = 15, .ns = 6},
            {.bs = 43, .dw = 19, .ns = 6}
        }
    },
    { /* Version 11 */
        .data_bytes = 404,
        .apat = {6, 30, 54, 0},
        .ecc = {
            {.bs = 80, .dw = 50, .ns = 1},
            {.bs = 101, .dw = 81, .ns = 4},
            {.bs = 36, .dw = 12, .ns = 3},
            {.bs = 50, .dw = 22, .ns = 4}
        }
    },
    { /* Version 12 */
        .data_bytes = 466,
        .apat = {6, 32, 58, 0},
        .ecc = {
            {.bs = 58, .dw = 36, .ns = 6},
            {.bs = 116, .dw = 92, .ns = 2},
            {.bs = 42, .dw = 14, .ns = 7},
            {.bs = 46, .dw = 20, .ns = 4}
        }
    },
    { /* Version 13 */
        .data_bytes = 532,
        .apat = {6, 34, 62, 0},
        .ecc = {
            {.bs = 59, .dw = 37, .ns = 8},
            {.bs = 133, .dw = 107, .ns = 4},
            {.bs = 33, .dw = 11, .ns = 12},
            {.bs = 44, .dw = 20, .ns = 8}
        }
    },
    { /* Version 14 */
        .data_bytes = 581,
        .apat = {6, 26, 46, 66, 0},
        .ecc = {
            {.bs = 64, .dw = 40, .ns = 4},
            {.bs = 145, .dw = 115, .ns = 3},
            {.bs = 36, .dw = 12, .ns = 11},
            {.bs = 36, .dw = 16, .ns = 11}
        }
    },
    { /* Version 15 */
        .data_bytes = 655,
        .apat = {6, 26, 48, 70, 0},
        .ecc = {
            {.bs = 65, .dw = 41, .ns = 5},
            {.bs = 109, .dw = 87, .ns = 5},
            {.bs = 36, .dw = 12, .ns = 11},
            {.bs = 54, .dw = 24, .ns = 5}
        }
    },
    { /* Version 16 */
        .data_bytes = 733,
        .apat = {6, 26, 50, 74, 0},
        .ecc = {
            {.bs = 73, .dw = 45, .ns = 7},
            {.bs = 122, .dw = 98, .ns = 5},
            {.bs = 45, .dw = 15, .ns = 3},
            {.bs = 43, .dw = 19, .ns = 15}
        }
    },
    { /* Version 17 */
        .data_bytes = 815,
        .apat = {6, 30, 54, 78, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 10},
            {.bs = 135, .dw = 107, .ns = 1},
            {.bs = 42, .dw = 14, .ns = 2},
            {.bs = 50, .dw = 22, .ns = 1}
        }
    },
    { /* Version 18 */
        .data_bytes = 901,
        .apat = {6, 30, 56, 82, 0},
        .ecc = {
            {.bs = 69, .dw = 43, .ns = 9},
            {.bs = 150, .dw = 120, .ns = 5},
            {.bs = 42, .dw = 14, .ns = 2},
            {.bs = 50, .dw = 22, .ns = 17}
        }
    },
    { /* Version 19 */
        .data_bytes = 991,
        .apat = {6, 30, 58, 86, 0},
        .ecc = {
            {.bs = 70, .dw = 44, .ns = 3},
            {.bs = 141, .dw = 113, .ns = 3},
            {.bs = 39, .dw = 13, .ns = 9},
            {.bs = 47, .dw = 21, .ns = 17}
        }
    },
    { /* Version 20 */
        .data_bytes = 1085,
        .apat = {6, 34, 62, 90, 0},
        .ecc = {
            {.bs = 67, .dw = 41, .ns = 3},
            {.bs = 135, .dw = 107, .ns = 3},
            {.bs = 43, .dw = 15, .ns = 15},
            {.bs = 54, .dw = 24, .ns = 15}
        }
    },
    { /* Version 21 */
        .data_bytes = 1156,
        .apat = {6, 28, 50, 72, 92, 0},
        .ecc = {
            {.bs = 68, .dw = 42, .ns = 17},
            {.bs = 144, .dw = 116, .ns = 4},
            {.bs = 46, .dw = 16, .ns = 19},
            {.bs = 50, .dw = 22, .ns = 17}
        }
    },
    { /* Version 22 */
        .data_bytes = 1258,
        .apat = {6, 26, 50, 74, 98, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 17},
            {.bs = 139, .dw = 111, .ns = 2},
            {.bs = 37, .dw = 13, .ns = 34},
            {.bs = 54, .dw = 24, .ns = 7}
        }
    },
    { /* Version 23 */
        .data_bytes = 1364,
        .apat = {6, 30, 54, 78, 102, 0},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 4},
            {.bs = 151, .dw = 121, .ns = 4},
            {.bs = 45, .dw = 15, .ns = 16},
            {.bs = 54, .dw = 24, .ns = 11}
        }
    },
    { /* Version 24 */
        .data_bytes = 1474,
        .apat = {6, 28, 54, 80, 106, 0},
        .ecc = {
            {.bs = 73, .dw = 45, .ns = 6},
            {.bs = 147, .dw = 117, .ns = 6},
            {.bs = 46, .dw = 16, .ns = 30},
            {.bs = 54, .dw = 24, .ns = 11}
        }
    },
    { /* Version 25 */
        .data_bytes = 1588,
        .apat = {6, 32, 58, 84, 110, 0},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 8},
            {.bs = 132, .dw = 106, .ns = 8},
            {.bs = 45, .dw = 15, .ns = 22},
            {.bs = 54, .dw = 24, .ns = 7}
        }
    },
    { /* Version 26 */
        .data_bytes = 1706,
        .apat = {6, 30, 58, 86, 114, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 19},
            {.bs = 142, .dw = 114, .ns = 10},
            {.bs = 46, .dw = 16, .ns = 33},
            {.bs = 50, .dw = 22, .ns = 28}
        }
    },
    { /* Version 27 */
        .data_bytes = 1828,
        .apat = {6, 34, 62, 90, 118, 0},
        .ecc = {
            {.bs = 73, .dw = 45, .ns = 22},
            {.bs = 152, .dw = 122, .ns = 8},
            {.bs = 45, .dw = 15, .ns = 12},
            {.bs = 53, .dw = 23, .ns = 8}
        }
    },
    { /* Version 28 */
        .data_bytes = 1921,
        .apat = {6, 26, 50, 74, 98, 122, 0},
        .ecc = {
            {.bs = 73, .dw = 45, .ns = 3},
            {.bs = 147, .dw = 117, .ns = 3},
            {.bs = 45, .dw = 15, .ns = 11},
            {.bs = 54, .dw = 24, .ns = 4}
        }
    },
    { /* Version 29 */
        .data_bytes = 2051,
        .apat = {6, 30, 54, 78, 102, 126, 0},
        .ecc = {
            {.bs = 73, .dw = 45, .ns = 21},
            {.bs = 146, .dw = 116, .ns = 7},
            {.bs = 45, .dw = 15, .ns = 19},
            {.bs = 53, .dw = 23, .ns = 1}
        }
    },
    { /* Version 30 */
        .data_bytes = 2185,
        .apat = {6, 26, 52, 78, 104, 130, 0},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 19},
            {.bs = 145, .dw = 115, .ns = 5},
            {.bs = 45, .dw = 15, .ns = 23},
            {.bs = 54, .dw = 24, .ns = 15}
        }
    },
    { /* Version 31 */
        .data_bytes = 2323,
        .apat = {6, 30, 56, 82, 108, 134, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 2},
            {.bs = 145, .dw = 115, .ns = 13},
            {.bs = 45, .dw = 15, .ns = 23},
            {.bs = 54, .dw = 24, .ns = 42}
        }
    },
    { /* Version 32 */
        .data_bytes = 2465,
        .apat = {6, 34, 60, 86, 112, 138, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 10},
            {.bs = 145, .dw = 115, .ns = 17},
            {.bs = 45, .dw = 15, .ns = 19},
            {.bs = 54, .dw = 24, .ns = 10}
        }
    },
    { /* Version 33 */
        .data_bytes = 2611,
        .apat = {6, 30, 58, 86, 114, 142, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 14},
            {.bs = 145, .dw = 115, .ns = 17},
            {.bs = 45, .dw = 15, .ns = 11},
            {.bs = 54, .dw = 24, .ns = 29}
        }
    },
    { /* Version 34 */
        .data_bytes = 2761,
        .apat = {6, 34, 62, 90, 118, 146, 0},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 14},
            {.bs = 145, .dw = 115, .ns = 13},
            {.bs = 46, .dw = 16, .ns = 59},
            {.bs = 54, .dw = 24, .ns = 44}
        }
    },
    { /* Version 35 */
        .data_bytes = 2876,
        .apat = {6, 30, 54, 78, 102, 126, 150},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 12},
            {.bs = 151, .dw = 121, .ns = 12},
            {.bs = 45, .dw = 15, .ns = 22},
            {.bs = 54, .dw = 24, .ns = 39}
        }
    },
    { /* Version 36 */
        .data_bytes = 3034,
        .apat = {6, 24, 50, 76, 102, 128, 154},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 6},
            {.bs = 151, .dw = 121, .ns = 6},
            {.bs = 45, .dw = 15, .ns = 2},
            {.bs = 54, .dw = 24, .ns = 46}
        }
    },
    { /* Version 37 */
        .data_bytes = 3196,
        .apat = {6, 28, 54, 80, 106, 132, 158},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 29},
            {.bs = 152, .dw = 122, .ns = 17},
            {.bs = 45, .dw = 15, .ns = 24},
            {.bs = 54, .dw = 24, .ns = 49}
        }
    },
    { /* Version 38 */
        .data_bytes = 3362,
        .apat = {6, 32, 58, 84, 110, 136, 162},
        .ecc = {
            {.bs = 74, .dw = 46, .ns = 13},
            {.bs = 152, .dw = 122, .ns = 4},
            {.bs = 45, .dw = 15, .ns = 42},
            {.bs = 54, .dw = 24, .ns = 48}
        }
    },
    { /* Version 39 */
        .data_bytes = 3532,
        .apat = {6, 26, 54, 82, 110, 138, 166},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 40},
            {.bs = 147, .dw = 117, .ns = 20},
            {.bs = 45, .dw = 15, .ns = 10},
            {.bs = 54, .dw = 24, .ns = 43}
        }
    },
    { /* Version 40 */
        .data_bytes = 3706,
        .apat = {6, 30, 58, 86, 114, 142, 170},
        .ecc = {
            {.bs = 75, .dw = 47, .ns = 18},
            {.bs = 148, .dw = 118, .ns = 19},
            {.bs = 45, .dw = 15, .ns = 20},
            {.bs = 54, .dw = 24, .ns = 34}
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "indentify.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc - QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/************************************************************************
 * Linear algebra routines
 */

static int line_intersect(const struct quirc_point *p0,
                          const struct quirc_point *p1,
                          const struct quirc_point *q0,
                          const struct quirc_point *q1,
                          struct quirc_point *r)
{
    /* (a, b) is perpendicular to line p */
    int a = -(p1->y - p0->y);
    int b = p1->x - p0->x;

    /* (c, d) is perpendicular to line q */
    int c = -(q1->y - q0->y);
    int d = q1->x - q0->x;

    /* e and f are dot products of the respective vectors with p and q */
    int e = a * p1->x + b * p1->y;
    int f = c * q1->x + d * q1->y;

    /* Now we need to solve:
     *     [a b] [rx]   [e]
     *     [c d] [ry] = [f]
     *
     * We do this by inverting the matrix and applying it to (e, f):
     *       [ d -b] [e]   [rx]
     * 1/det [-c  a] [f] = [ry]
     */
    int det = (a * d) - (b * c);

    if (!det)
        return 0;

    r->x = (d * e - b * f) / det;
    r->y = (-c * e + a * f) / det;

    return 1;
}

static void perspective_setup(float *c,
                              const struct quirc_point *rect,
                              float w, float h)
{
    float x0 = rect[0].x;
    float y0 = rect[0].y;
    float x1 = rect[1].x;
    float y1 = rect[1].y;
    float x2 = rect[2].x;
    float y2 = rect[2].y;
    float x3 = rect[3].x;
    float y3 = rect[3].y;

    float wden = w * (x2*y3 - x3*y2 + (x3-x2)*y1 + x1*(y2-y3));
    float hden = h * (x2*y3 + x1*(y2-y3) - x3*y2 + (x3-x2)*y1);

    c[0] = (x1*(x2*y3-x3*y2) + x0*(-x2*y3+x3*y2+(x2-x3)*y1) +
        x1*(x3-x2)*y0) / wden;
    c[1] = -(x0*(x2*y3+x1*(y2-y3)-x2*y1) - x1*x3*y2 + x2*x3*y1
         + (x1*x3-x2*x3)*y0) / hden;
    c[2] = x0;
    c[3] = (y0*(x1*(y3-y2)-x2*y3+x3*y2) + y1*(x2*y3-x3*y2) +
        x0*y1*(y2-y3)) / wden;
    c[4] = (x0*(y1*y3-y2*y3) + x1*y2*y3 - x2*y1*y3 +
        y0*(x3*y2-x1*y2+(x2-x3)*y1)) / hden;
    c[5] = y0;
    c[6] = (x1*(y3-y2) + x0*(y2-y3) + (x2-x3)*y1 + (x3-x2)*y0) / wden;
    c[7] = (-x2*y3 + x1*y3 + x3*y2 + x0*(y1-y2) - x3*y1 + (x2-x1)*y0) /
        hden;
}

static void perspective_map(const float *c,
                            float u, float v, struct quirc_point *ret)
{
    float den = c[6]*u + c[7]*v + 1.0;
    float x = (c[0]*u + c[1]*v + c[2]) / den;
    float y = (c[3]*u + c[4]*v + c[5]) / den;

    ret->x = fast_roundf(x);
    ret->y = fast_roundf(y);
}

static void perspective_unmap(const float *c,
                              const struct quirc_point *in,
                              float *u, float *v)
{
    float x = in->x;
    float y = in->y;
    float den = -c[0]*c[7]*y + c[1]*c[6]*y + (c[3]*c[7]-c[4]*c[6])*x +
        c[0]*c[4] - c[1]*c[3];

    *u = -(c[1]*(y-c[5]) - c[2]*c[7]*y + (c[5]*c[7]-c[4])*x + c[2]*c[4]) /
        den;
    *v = (c[0]*(y-c[5]) - c[2]*c[6]*y + (c[5]*c[6]-c[3])*x + c[2]*c[3]) /
        den;
}

/************************************************************************
 * Span-based floodfill routine
 */

typedef void (*span_func_t)(void *user_data, int y, int left, int right);

typedef struct xylf
{
    int16_t x, y, l, r;
}
xylf_t;

static void lifo_enqueue_fast(lifo_t *ptr, void *data)
{
// we know the structure size is 8 bytes, so don't waste time calling memcpy
    uint32_t *d = (uint32_t *)(ptr->data + (ptr->len * ptr->data_len));
    uint32_t *s = (uint32_t *)data;
//    memcpy(ptr->data + (ptr->len * ptr->data_len), data, ptr->data_len);
    d[0] = s[0]; d[1] = s[1]; // copy 8 bytes
    ptr->len += 1;
}

static void lifo_dequeue_fast(lifo_t *ptr, void *data)
{
    // we know the structure size is 8 bytes, so don't waste time calling memcpy
    uint32_t *s = (uint32_t *)(ptr->data + ((ptr->len-1) * ptr->data_len));
    uint32_t *d = (uint32_t *)data;
//    if (data) {
//        memcpy(data, ptr->data + ((ptr->len - 1) * ptr->data_len), ptr->data_len);
//    }
    d[0] = s[0]; d[1] = s[1]; // copy 8 bytes
    ptr->len -= 1;
}

static void flood_fill_seed(struct quirc *q, int x, int y, int from, int to,
                            span_func_t func, void *user_data,
                            int depth)
{
    (void) depth; // unused
    uint8_t from8 = from, to8=to;

    lifo_t lifo;
    size_t lifo_len;
    lifo_alloc_all(&lifo, &lifo_len, sizeof(xylf_t));

    for(;;) {
        int left = x;
        int right = x;
        int i;
        quirc_pixel_t *row = q->pixels + y * q->w;

        while (left > 0 && row[left - 1] == from8)
            left--;

        while (right < q->w - 1 && row[right + 1] == from8)
            right++;

        /* Fill the extent */
        for (i = left; i <= right; i++)
            row[i] = to8;

        if (func)
            func(user_data, y, left, right);

        for(;;) {
            if (/*lifo_size(&lifo)*/ lifo.len < lifo_len) {
                /* Seed new flood-fills */
                if (y > 0) {
                    row = q->pixels + (y - 1) * q->w;

                    bool recurse = false;
                    for (i = left; i <= right; i++)
                        if (row[i] == from8) {
                            xylf_t context;
                            context.x = x;
                            context.y = y;
                            context.l = left;
                            context.r = right;
                            lifo_enqueue_fast(&lifo, &context);
                            x = i;
                            y = y - 1;
                            recurse = true;
                            break;
                        }
                    if (recurse)
                        break;
                }

                if (y < q->h - 1) {
                    row = q->pixels + (y + 1) * q->w;

                    bool recurse = false;
                    for (i = left; i <= right; i++)
                        if (row[i] == from8) {
                            xylf_t context;
                            context.x = x;
                            context.y = y;
                            context.l = left;
                            context.r = right;
                            lifo_enqueue_fast(&lifo, &context);
                            x = i;
                            y = y + 1;
                            recurse = true;
                            break;
                        }
                    if (recurse)
                        break;
                }
            }

            if (!lifo.len /*lifo_size(&lifo)*/) {
                lifo_free(&lifo);
                return;
            }

            xylf_t context;
            lifo_dequeue_fast(&lifo, &context);
            x = context.x;
            y = context.y;
            left = context.l;
            right = context.r;
        }
    }
}

/************************************************************************
 * Adaptive thresholding
 */

#define THRESHOLD_S_MIN 1
#define THRESHOLD_S_DEN 8
#define THRESHOLD_T     5

static void threshold(struct quirc *q)
{
    int x, y;
    int avg_w = 0;
    int avg_u = 0;
    int threshold_s = q->w / THRESHOLD_S_DEN;
    int fracmul, fracmul2;
    quirc_pixel_t *row = q->pixels;
    int width = q->w;

    /*
     * Ensure a sane, non-zero value for threshold_s.
     *
     * threshold_s can be zero if the image width is small. We need to avoid
     * SIGFPE as it will be used as divisor.
     */
    if (threshold_s < THRESHOLD_S_MIN)
        threshold_s = THRESHOLD_S_MIN;

    fracmul = (32768 * (threshold_s - 1)) / threshold_s; // to use multipy instead of divide (not too many bits or we'll overflow)
    // to get the effect used below (a fraction of threshold_s-1/threshold_s
    // The second constant is to reduce the averaged values to compare with the current pixel
    fracmul2 = (0x100000 * (100 - THRESHOLD_T)) / (200 * threshold_s); // use as many bits as possible without overflowing

    for (y = 0; y < q->h; y++) {
        int row_average[q->w];

        memset(row_average, 0, sizeof(row_average));

        for (x = 0; x < width; x++) {
            int w, u;

            if (y & 1) {
                w = x;
                u = width - 1 - x;
            } else {
                w = width - 1 - x;
                u = x;
            }

//            avg_w = (avg_w * (threshold_s - 1)) / threshold_s + row[w];
//            avg_u = (avg_u * (threshold_s - 1)) / threshold_s + row[u];
            // The original mul/div operation sought to reduce the average value by a small fraction (e.g. 1/79)
            // This mul/shift approximation achieves the same goal with only a small percentage difference
            avg_w = ((avg_w * fracmul) >> 15) + row[w];
            avg_u = ((avg_u * fracmul) >> 15) + row[u];

            row_average[w] += avg_w;
            row_average[u] += avg_u;
        }

        for (x = 0; x < width; x++) {
            //            if (row[x] < row_average[x] * (100 - THRESHOLD_T) / (200 * threshold_s))
            if (row[x] < ((row_average[x] * fracmul2) >> 20))
                row[x] = QUIRC_PIXEL_BLACK;
            else
                row[x] = QUIRC_PIXEL_WHITE;
        }

        row += width;
    }
} /* threshold() */

static void area_count(void *user_data, int y, int left, int right)
{
    ((struct quirc_region *)user_data)->count += right - left + 1;
}

static int region_code(struct quirc *q, int x, int y)
{
    int pixel;
    struct quirc_region *box;
    int region;

    if (x < 0 || y < 0 || x >= q->w || y >= q->h)
        return -1;

    pixel = q->pixels[y * q->w + x];

    if (pixel >= QUIRC_PIXEL_REGION)
        return pixel;

    if (pixel == QUIRC_PIXEL_WHITE)
        return -1;

    if (q->num_regions >= QUIRC_MAX_REGIONS)
        return -1;

    region = q->num_regions;
    box = &q->regions[q->num_regions++];

    memset(box, 0, sizeof(*box));

    box->seed.x = x;
    box->seed.y = y;
    box->capstone = -1;

    flood_fill_seed(q, x, y, pixel, region, area_count, box, 0);

    return region;
}

struct polygon_score_data {
    struct quirc_point  ref;

    int                 scores[4];
    struct quirc_point  *corners;
};

static void find_one_corner(void *user_data, int y, int left, int right)
{
    struct polygon_score_data *psd =
        (struct polygon_score_data *)user_data;
    int xs[2] = {left, right};
    int dy = y - psd->ref.y;
    int i;

    for (i = 0; i < 2; i++) {
        int dx = xs[i] - psd->ref.x;
        int d = dx * dx + dy * dy;

        if (d > psd->scores[0]) {
            psd->scores[0] = d;
            psd->corners[0].x = xs[i];
            psd->corners[0].y = y;
        }
    }
}

static void find_other_corners(void *user_data, int y, int left, int right)
{
    struct polygon_score_data *psd =
        (struct polygon_score_data *)user_data;
    int xs[2] = {left, right};
    int i;

    for (i = 0; i < 2; i++) {
        int up = xs[i] * psd->ref.x + y * psd->ref.y;
        int right = xs[i] * -psd->ref.y + y * psd->ref.x;
        int scores[4] = {up, right, -up, -right};
        int j;

        for (j = 0; j < 4; j++) {
            if (scores[j] > psd->scores[j]) {
                psd->scores[j] = scores[j];
                psd->corners[j].x = xs[i];
                psd->corners[j].y = y;
            }
        }
    }
}

static void find_region_corners(struct quirc *q,
                                int rcode, const struct quirc_point *ref,
                                struct quirc_point *corners)
{
    struct quirc_region *region = &q->regions[rcode];
    struct polygon_score_data psd;
    int i;

    memset(&psd, 0, sizeof(psd));
    psd.corners = corners;

    memcpy(&psd.ref, ref, sizeof(psd.ref));
    psd.scores[0] = -1;
    flood_fill_seed(q, region->seed.x, region->seed.y,
                    rcode, QUIRC_PIXEL_BLACK,
                    find_one_corner, &psd, 0);

    psd.ref.x = psd.corners[0].x - psd.ref.x;
    psd.ref.y = psd.corners[0].y - psd.ref.y;

    for (i = 0; i < 4; i++)
        memcpy(&psd.corners[i], &region->seed,
               sizeof(psd.corners[i]));

    i = region->seed.x * psd.ref.x + region->seed.y * psd.ref.y;
    psd.scores[0] = i;
    psd.scores[2] = -i;
    i = region->seed.x * -psd.ref.y + region->seed.y * psd.ref.x;
    psd.scores[1] = i;
    psd.scores[3] = -i;

    flood_fill_seed(q, region->seed.x, region->seed.y,
                    QUIRC_PIXEL_BLACK, rcode,
                    find_other_corners, &psd, 0);
}

static void record_capstone(struct quirc *q, int ring, int stone)
{
    struct quirc_region *stone_reg = &q->regions[stone];
    struct quirc_region *ring_reg = &q->regions[ring];
    struct quirc_capstone *capstone;
    int cs_index;

    if (q->num_capstones >= QUIRC_MAX_CAPSTONES)
        return;

    cs_index = q->num_capstones;
    capstone = &q->capstones[q->num_capstones++];

    memset(capstone, 0, sizeof(*capstone));

    capstone->qr_grid = -1;
    capstone->ring = ring;
    capstone->stone = stone;
    stone_reg->capstone = cs_index;
    ring_reg->capstone = cs_index;

    /* Find the corners of the ring */
    find_region_corners(q, ring, &stone_reg->seed, capstone->corners);

    /* Set up the perspective transform and find the center */
    perspective_setup(capstone->c, capstone->corners, 7.0, 7.0);
    perspective_map(capstone->c, 3.5, 3.5, &capstone->center);
}

static void test_capstone(struct quirc *q, int x, int y, int *pb)
{
    int ring_right, ring_left, stone;
    ring_right = region_code(q, x - pb[4], y);
    ring_left = region_code(q, x - pb[4] - pb[3] -
                    pb[2] - pb[1] - pb[0],
                    y);
    struct quirc_region *stone_reg;
    struct quirc_region *ring_reg;
    int ratio;

    if (ring_left < 0 || ring_right < 0)// || stone < 0)
        return;

    /* Left and ring of ring should be connected */
    if (ring_left != ring_right) // <-- most of the time, it exits here
        return;

    stone = region_code(q, x - pb[4] - pb[3] - pb[2], y);
    if (stone < 0)
       return;

    /* Ring should be disconnected from stone */
    if (ring_left == stone)
        return;

    stone_reg = &q->regions[stone];
    ring_reg = &q->regions[ring_left];

    /* Already detected */
    if (stone_reg->capstone >= 0 || ring_reg->capstone >= 0)
        return;

    /* Ratio should ideally be 37.5 */
    ratio = stone_reg->count * 100 / ring_reg->count;
    if (ratio < 10 || ratio > 70)
        return;

    record_capstone(q, ring_left, stone);
}

static void finder_scan(struct quirc *q, int y)
{
    quirc_pixel_t *row = q->pixels + y * q->w;
    int x;
    uint8_t color, last_color;
    int run_length = 1;
    int run_count = 0;
    int pb[5];

    memset(pb, 0, sizeof(pb));
    last_color = row[0];
    for (x = 1; x < q->w; x++) {
        color = row[x];

        if (/* x && */ color != last_color) {
            memmove(pb, pb + 1, sizeof(pb[0]) * 4);
            pb[4] = run_length;
            run_length = 0;
            run_count++;

            if (!color && run_count >= 5) {
                static int check[5] = {1, 1, 3, 1, 1};
                int avg, err;
                int i;
                int ok = 1;

                avg = (pb[0] + pb[1] + pb[3] + pb[4]) / 4;
                err = avg * 3 / 4;

                for (i = 0; i < 5; i++)
                    if (pb[i] < check[i] * avg - err ||
                        pb[i] > check[i] * avg + err)
                        ok = 0;

                if (ok)
                    test_capstone(q, x, y, pb);
            }
        }

        run_length++;
        last_color = color;
    }
}

static void find_alignment_pattern(struct quirc *q, int index)
{
    struct quirc_grid *qr = &q->grids[index];
    struct quirc_capstone *c0 = &q->capstones[qr->caps[0]];
    struct quirc_capstone *c2 = &q->capstones[qr->caps[2]];
    struct quirc_point a;
    struct quirc_point b;
    struct quirc_point c;
    int size_estimate;
    int step_size = 1;
    int dir = 0;
    float u, v;

    /* Grab our previous estimate of the alignment pattern corner */
    memcpy(&b, &qr->align, sizeof(b));

    /* Guess another two corners of the alignment pattern so that we
     * can estimate its size.
     */
    perspective_unmap(c0->c, &b, &u, &v);
    perspective_map(c0->c, u, v + 1.0, &a);
    perspective_unmap(c2->c, &b, &u, &v);
    perspective_map(c2->c, u + 1.0, v, &c);

    size_estimate = abs((a.x - b.x) * -(c.y - b.y) +
                    (a.y - b.y) * (c.x - b.x));

    /* Spiral outwards from the estimate point until we find something
     * roughly the right size. Don't look too far from the estimate
     * point.
     */
    while (step_size * step_size < size_estimate * 100) {
        static const int dx_map[] = {1, 0, -1, 0};
        static const int dy_map[] = {0, -1, 0, 1};
        int i;

        for (i = 0; i < step_size; i++) {
            int code = region_code(q, b.x, b.y);

            if (code >= 0) {
                struct quirc_region *reg = &q->regions[code];

                if (reg->count >= size_estimate / 2 &&
                    reg->count <= size_estimate * 2) {
                    qr->align_region = code;
                    return;
                }
            }

            b.x += dx_map[dir];
            b.y += dy_map[dir];
        }

        dir = (dir + 1) % 4;
        if (!(dir & 1))
            step_size++;
    }
}

static void find_leftmost_to_line(void *user_data, int y, int left, int right)
{
    struct polygon_score_data *psd =
        (struct polygon_score_data *)user_data;
    int xs[2] = {left, right};
    int i;

    for (i = 0; i < 2; i++) {
        int d = -psd->ref.y * xs[i] + psd->ref.x * y;

        if (d < psd->scores[0]) {
            psd->scores[0] = d;
            psd->corners[0].x = xs[i];
            psd->corners[0].y = y;
        }
    }
}

/* Do a Bresenham scan from one point to another and count the number
 * of black/white transitions.
 */
static int timing_scan(const struct quirc *q,
               const struct quirc_point *p0,
               const struct quirc_point *p1)
{
    int n = p1->x - p0->x;
    int d = p1->y - p0->y;
    int x = p0->x;
    int y = p0->y;
    int *dom, *nondom;
    int dom_step;
    int nondom_step;
    int a = 0;
    int i;
    int run_length = 0;
    int count = 0;

    if (p0->x < 0 || p0->y < 0 || p0->x >= q->w || p0->y >= q->h)
        return -1;
    if (p1->x < 0 || p1->y < 0 || p1->x >= q->w || p1->y >= q->h)
        return -1;

    if (abs(n) > abs(d)) {
        int swap = n;

        n = d;
        d = swap;

        dom = &x;
        nondom = &y;
    } else {
        dom = &y;
        nondom = &x;
    }

    if (n < 0) {
        n = -n;
        nondom_step = -1;
    } else {
        nondom_step = 1;
    }

    if (d < 0) {
        d = -d;
        dom_step = -1;
    } else {
        dom_step = 1;
    }

    x = p0->x;
    y = p0->y;
    for (i = 0; i <= d; i++) {
        int pixel;

        if (y < 0 || y >= q->h || x < 0 || x >= q->w)
            break;

        pixel = q->pixels[y * q->w + x];

        if (pixel) {
            if (run_length >= 2)
                count++;
            run_length = 0;
        } else {
            run_length++;
        }

        a += n;
        *dom += dom_step;
        if (a >= d) {
            *nondom += nondom_step;
            a -= d;
        }
    }

    return count;
}

/* Try the measure the timing pattern for a given QR code. This does
 * not require the global perspective to have been set up, but it
 * does require that the capstone corners have been set to their
 * canonical rotation.
 *
 * For each capstone, we find a point in the middle of the ring band
 * which is nearest the centre of the code. Using these points, we do
 * a horizontal and a vertical timing scan.
 */
static int measure_timing_pattern(struct quirc *q, int index)
{
    struct quirc_grid *qr = &q->grids[index];
    int i;
    int scan;
    int ver;
    int size;

    for (i = 0; i < 3; i++) {
        static const float us[] = {6.5, 6.5, 0.5};
        static const float vs[] = {0.5, 6.5, 6.5};
        struct quirc_capstone *cap = &q->capstones[qr->caps[i]];

        perspective_map(cap->c, us[i], vs[i], &qr->tpep[i]);
    }

    qr->hscan = timing_scan(q, &qr->tpep[1], &qr->tpep[2]);
    qr->vscan = timing_scan(q, &qr->tpep[1], &qr->tpep[0]);

    scan = qr->hscan;
    if (qr->vscan > scan)
        scan = qr->vscan;

    /* If neither scan worked, we can't go any further. */
    if (scan < 0)
        return -1;

    /* Choose the nearest allowable grid size */
    size = scan * 2 + 13;
    ver = (size - 15) / 4;
    qr->grid_size = ver * 4 + 17;

    return 0;
}

/* Read a cell from a grid using the currently set perspective
 * transform. Returns +/- 1 for black/white, 0 for cells which are
 * out of image bounds.
 */
static int read_cell(const struct quirc *q, int index, int x, int y)
{
    const struct quirc_grid *qr = &q->grids[index];
    struct quirc_point p;

    perspective_map(qr->c, x + 0.5, y + 0.5, &p);
    if (p.y < 0 || p.y >= q->h || p.x < 0 || p.x >= q->w)
        return 0;

    return q->pixels[p.y * q->w + p.x] ? 1 : -1;
}

static int fitness_cell(const struct quirc *q, int index, int x, int y)
{
    const struct quirc_grid *qr = &q->grids[index];
    int score = 0;
    int u, v;

    for (v = 0; v < 3; v++)
        for (u = 0; u < 3; u++) {
            static const float offsets[] = {0.3, 0.5, 0.7};
            struct quirc_point p;

            perspective_map(qr->c, x + offsets[u],
                           y + offsets[v], &p);
            if (p.y < 0 || p.y >= q->h || p.x < 0 || p.x >= q->w)
                continue;

            if (q->pixels[p.y * q->w + p.x])
                score++;
            else
                score--;
        }

    return score;
}

static int fitness_ring(const struct quirc *q, int index, int cx, int cy,
                        int radius)
{
    int i;
    int score = 0;

    for (i = 0; i < radius * 2; i++) {
        score += fitness_cell(q, index, cx - radius + i, cy - radius);
        score += fitness_cell(q, index, cx - radius, cy + radius - i);
        score += fitness_cell(q, index, cx + radius, cy - radius + i);
        score += fitness_cell(q, index, cx + radius - i, cy + radius);
    }

    return score;
}

static int fitness_apat(const struct quirc *q, int index, int cx, int cy)
{
    return fitness_cell(q, index, cx, cy) -
        fitness_ring(q, index, cx, cy, 1) +
        fitness_ring(q, index, cx, cy, 2);
}

static int fitness_capstone(const struct quirc *q, int index, int x, int y)
{
    x += 3;
    y += 3;

    return fitness_cell(q, index, x, y) +
        fitness_ring(q, index, x, y, 1) -
        fitness_ring(q, index, x, y, 2) +
        fitness_ring(q, index, x, y, 3);
}

/* Compute a fitness score for the currently configured perspective
 * transform, using the features we expect to find by scanning the
 * grid.
 */
static int fitness_all(const struct quirc *q, int index)
{
    const struct quirc_grid *qr = &q->grids[index];
    int version = (qr->grid_size - 17) / 4;
    const struct quirc_version_info *info = &quirc_version_db[version];
    int score = 0;
    int i, j;
    int ap_count;

    /* Check the timing pattern */
    for (i = 0; i < qr->grid_size - 14; i++) {
        int expect = (i & 1) ? 1 : -1;

        score += fitness_cell(q, index, i + 7, 6) * expect;
        score += fitness_cell(q, index, 6, i + 7) * expect;
    }

    /* Check capstones */
    score += fitness_capstone(q, index, 0, 0);
    score += fitness_capstone(q, index, qr->grid_size - 7, 0);
    score += fitness_capstone(q, index, 0, qr->grid_size - 7);

    if (version < 0 || version > QUIRC_MAX_VERSION)
        return score;

    /* Check alignment patterns */
    ap_count = 0;
    while ((ap_count < QUIRC_MAX_ALIGNMENT) && info->apat[ap_count])
        ap_count++;

    for (i = 1; i + 1 < ap_count; i++) {
        score += fitness_apat(q, index, 6, info->apat[i]);
        score += fitness_apat(q, index, info->apat[i], 6);
    }

    for (i = 1; i < ap_count; i++)
        for (j = 1; j < ap_count; j++)
            score += fitness_apat(q, index,
                    info->apat[i], info->apat[j]);

    return score;
}

static void jiggle_perspective(struct quirc *q, int index)
{
    struct quirc_grid *qr = &q->grids[index];
    int best = fitness_all(q, index);
    int pass;
    float adjustments[8];
    int i;

    for (i = 0; i < 8; i++)
        adjustments[i] = qr->c[i] * 0.02;

    for (pass = 0; pass < 5; pass++) {
        for (i = 0; i < 16; i++) {
            int j = i >> 1;
            int test;
            float old = qr->c[j];
            float step = adjustments[j];
            float new;

            if (i & 1)
                new = old + step;
            else
                new = old - step;

            qr->c[j] = new;
            test = fitness_all(q, index);

            if (test > best)
                best = test;
            else
                qr->c[j] = old;
        }

        for (i = 0; i < 8; i++)
            adjustments[i] *= 0.5;
    }
}

/* Once the capstones are in place and an alignment point has been
 * chosen, we call this function to set up a grid-reading perspective
 * transform.
 */
static void setup_qr_perspective(struct quirc *q, int index)
{
    struct quirc_grid *qr = &q->grids[index];
    struct quirc_point rect[4];

    /* Set up the perspective map for reading the grid */
    memcpy(&rect[0], &q->capstones[qr->caps[1]].corners[0],
           sizeof(rect[0]));
    memcpy(&rect[1], &q->capstones[qr->caps[2]].corners[0],
           sizeof(rect[0]));
    memcpy(&rect[2], &qr->align, sizeof(rect[0]));
    memcpy(&rect[3], &q->capstones[qr->caps[0]].corners[0],
           sizeof(rect[0]));
    perspective_setup(qr->c, rect, qr->grid_size - 7, qr->grid_size - 7);

    jiggle_perspective(q, index);
}

/* Rotate the capstone with so that corner 0 is the leftmost with respect
 * to the given reference line.
 */
static void rotate_capstone(struct quirc_capstone *cap,
                            const struct quirc_point *h0,
                            const struct quirc_point *hd)
{
    struct quirc_point copy[4];
    int j;
    int best = 0;
    int best_score = 0;

    for (j = 0; j < 4; j++) {
        struct quirc_point *p = &cap->corners[j];
        int score = (p->x - h0->x) * -hd->y +
            (p->y - h0->y) * hd->x;

        if (!j || score < best_score) {
            best = j;
            best_score = score;
        }
    }

    /* Rotate the capstone */
    for (j = 0; j < 4; j++)
        memcpy(&copy[j], &cap->corners[(j + best) % 4],
               sizeof(copy[j]));
    memcpy(cap->corners, copy, sizeof(cap->corners));
    perspective_setup(cap->c, cap->corners, 7.0, 7.0);
}

static void record_qr_grid(struct quirc *q, int a, int b, int c)
{
    struct quirc_point h0, hd;
    int i;
    int qr_index;
    struct quirc_grid *qr;

    if (q->num_grids >= QUIRC_MAX_GRIDS)
        return;

    /* Construct the hypotenuse line from A to C. B should be to
     * the left of this line.
     */
    memcpy(&h0, &q->capstones[a].center, sizeof(h0));
    hd.x = q->capstones[c].center.x - q->capstones[a].center.x;
    hd.y = q->capstones[c].center.y - q->capstones[a].center.y;

    /* Make sure A-B-C is clockwise */
    if ((q->capstones[b].center.x - h0.x) * -hd.y +
        (q->capstones[b].center.y - h0.y) * hd.x > 0) {
        int swap = a;

        a = c;
        c = swap;
        hd.x = -hd.x;
        hd.y = -hd.y;
    }

    /* Record the grid and its components */
    qr_index = q->num_grids;
    qr = &q->grids[q->num_grids++];

    memset(qr, 0, sizeof(*qr));
    qr->caps[0] = a;
    qr->caps[1] = b;
    qr->caps[2] = c;
    qr->align_region = -1;

    /* Rotate each capstone so that corner 0 is top-left with respect
     * to the grid.
     */
    for (i = 0; i < 3; i++) {
        struct quirc_capstone *cap = &q->capstones[qr->caps[i]];

        rotate_capstone(cap, &h0, &hd);
        cap->qr_grid = qr_index;
    }

    /* Check the timing pattern. This doesn't require a perspective
     * transform.
     */
    if (measure_timing_pattern(q, qr_index) < 0)
        goto fail;

    /* Make an estimate based for the alignment pattern based on extending
     * lines from capstones A and C.
     */
    if (!line_intersect(&q->capstones[a].corners[0],
                &q->capstones[a].corners[1],
                &q->capstones[c].corners[0],
                &q->capstones[c].corners[3],
                &qr->align))
        goto fail;

    /* On V2+ grids, we should use the alignment pattern. */
    if (qr->grid_size > 21) {
        /* Try to find the actual location of the alignment pattern. */
        find_alignment_pattern(q, qr_index);

        /* Find the point of the alignment pattern closest to the
         * top-left of the QR grid.
         */
        if (qr->align_region >= 0) {
            struct polygon_score_data psd;
            struct quirc_region *reg =
                &q->regions[qr->align_region];

            /* Start from some point inside the alignment pattern */
            memcpy(&qr->align, &reg->seed, sizeof(qr->align));

            memcpy(&psd.ref, &hd, sizeof(psd.ref));
            psd.corners = &qr->align;
            psd.scores[0] = -hd.y * qr->align.x +
                hd.x * qr->align.y;

            flood_fill_seed(q, reg->seed.x, reg->seed.y,
                    qr->align_region, QUIRC_PIXEL_BLACK,
                    NULL, NULL, 0);
            flood_fill_seed(q, reg->seed.x, reg->seed.y,
                    QUIRC_PIXEL_BLACK, qr->align_region,
                    find_leftmost_to_line, &psd, 0);
        }
    }

    setup_qr_perspective(q, qr_index);
    return;

fail:
    /* We've been unable to complete setup for this grid. Undo what we've
     * recorded and pretend it never happened.
     */
    for (i = 0; i < 3; i++)
        q->capstones[qr->caps[i]].qr_grid = -1;
    q->num_grids--;
}

struct neighbour {
    int     index;
    float   distance;
};

struct neighbour_list {
    struct neighbour    n[QUIRC_MAX_CAPSTONES];
    int                 count;
};

static void test_neighbours(struct quirc *q, int i,
                            const struct neighbour_list *hlist,
                            const struct neighbour_list *vlist)
{
    int j, k;
    float best_score = 0.0;
    int best_h = -1, best_v = -1;

    /* Test each possible grouping */
    for (j = 0; j < hlist->count; j++)
        for (k = 0; k < vlist->count; k++) {
            const struct neighbour *hn = &hlist->n[j];
            const struct neighbour *vn = &vlist->n[k];
            float score = fast_fabsf(1.0 - hn->distance / vn->distance);

            if (score > 2.5)
                continue;

            if (best_h < 0 || score < best_score) {
                best_h = hn->index;
                best_v = vn->index;
                best_score = score;
            }
        }

    if (best_h < 0 || best_v < 0)
        return;

    record_qr_grid(q, best_h, i, best_v);
}

static void test_grouping(struct quirc *q, int i)
{
    struct quirc_capstone *c1 = &q->capstones[i];
    int j;
    struct neighbour_list hlist;
    struct neighbour_list vlist;

    if (c1->qr_grid >= 0)
        return;

    hlist.count = 0;
    vlist.count = 0;

    /* Look for potential neighbours by examining the relative gradients
     * from this capstone to others.
     */
    for (j = 0; j < q->num_capstones; j++) {
        struct quirc_capstone *c2 = &q->capstones[j];
        float u, v;

        if (i == j || c2->qr_grid >= 0)
            continue;

        perspective_unmap(c1->c, &c2->center, &u, &v);

        u = fast_fabsf(u - 3.5);
        v = fast_fabsf(v - 3.5);

        if (u < 0.2 * v) {
            struct neighbour *n = &hlist.n[hlist.count++];

            n->index = j;
            n->distance = v;
        }

        if (v < 0.2 * u) {
            struct neighbour *n = &vlist.n[vlist.count++];

            n->index = j;
            n->distance = u;
        }
    }

    if (!(hlist.count && vlist.count))
        return;

    test_neighbours(q, i, &hlist, &vlist);
}

static void pixels_setup(struct quirc *q)
{
    if (sizeof(*q->image) == sizeof(*q->pixels)) {
        q->pixels = (quirc_pixel_t *)q->image;
    } else {
        int x, y;
        for (y = 0; y < q->h; y++) {
            for (x = 0; x < q->w; x++) {
                q->pixels[y * q->w + x] = q->image[y * q->w + x];
            }
        }
    }
}

uint8_t *quirc_begin(struct quirc *q, int *w, int *h)
{
    q->num_regions = QUIRC_PIXEL_REGION;
    q->num_capstones = 0;
    q->num_grids = 0;

    if (w)
        *w = q->w;
    if (h)
        *h = q->h;

    return q->image;
}

void quirc_end(struct quirc *q)
{
    int i;

    pixels_setup(q);
    threshold(q);

    for (i = 0; i < q->h; i++)
        finder_scan(q, i);

    for (i = 0; i < q->num_capstones; i++)
        test_grouping(q, i);
}

void quirc_extract(const struct quirc *q, int index,
                   struct quirc_code *code)
{
    const struct quirc_grid *qr = &q->grids[index];
    int y;
    int i = 0;

    if (index < 0 || index > q->num_grids)
        return;

    memset(code, 0, sizeof(*code));

    perspective_map(qr->c, 0.0, 0.0, &code->corners[0]);
    perspective_map(qr->c, qr->grid_size, 0.0, &code->corners[1]);
    perspective_map(qr->c, qr->grid_size, qr->grid_size,
            &code->corners[2]);
    perspective_map(qr->c, 0.0, qr->grid_size, &code->corners[3]);

    code->size = qr->grid_size;

    for (y = 0; y < qr->grid_size; y++) {
        int x;

        for (x = 0; x < qr->grid_size; x++) {
            if (read_cell(q, index, x, y) > 0)
                code->cell_bitmap[i >> 3] |= (1 << (i & 7));

            i++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "decode.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define MAX_POLY    64

/************************************************************************
 * Galois fields
 */

struct galois_field {
    int             p;
    const uint8_t   *log;
    const uint8_t   *exp;
};

static const uint8_t gf16_exp[16] = {
    0x01, 0x02, 0x04, 0x08, 0x03, 0x06, 0x0c, 0x0b,
    0x05, 0x0a, 0x07, 0x0e, 0x0f, 0x0d, 0x09, 0x01
};

static const uint8_t gf16_log[16] = {
    0x00, 0x0f, 0x01, 0x04, 0x02, 0x08, 0x05, 0x0a,
    0x03, 0x0e, 0x09, 0x07, 0x06, 0x0d, 0x0b, 0x0c
};

static const struct galois_field gf16 = {
    .p = 15,
    .log = gf16_log,
    .exp = gf16_exp
};

static const uint8_t gf256_exp[256] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26,
    0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9,
    0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,
    0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35,
    0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23,
    0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0,
    0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1,
    0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc,
    0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0,
    0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f,
    0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2,
    0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88,
    0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce,
    0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93,
    0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc,
    0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9,
    0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54,
    0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa,
    0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73,
    0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e,
    0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff,
    0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4,
    0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41,
    0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e,
    0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6,
    0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef,
    0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09,
    0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5,
    0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16,
    0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83,
    0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01
};

static const uint8_t gf256_log[256] = {
    0x00, 0xff, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6,
    0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b,
    0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81,
    0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71,
    0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21,
    0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45,
    0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9,
    0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6,
    0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd,
    0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88,
    0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd,
    0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40,
    0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e,
    0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d,
    0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b,
    0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57,
    0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d,
    0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18,
    0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c,
    0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e,
    0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd,
    0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61,
    0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e,
    0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2,
    0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76,
    0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6,
    0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa,
    0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a,
    0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51,
    0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7,
    0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8,
    0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf
};

const static struct galois_field gf256 = {
    .p = 255,
    .log = gf256_log,
    .exp = gf256_exp
};

/************************************************************************
 * Polynomial operations
 */

static void poly_add(uint8_t *dst, const uint8_t *src, uint8_t c,
                     int shift, const struct galois_field *gf)
{
    int i;
    int log_c = gf->log[c];

    if (!c)
        return;

    for (i = 0; i < MAX_POLY; i++) {
        int p = i + shift;
        uint8_t v = src[i];

        if (p < 0 || p >= MAX_POLY)
            continue;
        if (!v)
            continue;

        dst[p] ^= gf->exp[(gf->log[v] + log_c) % gf->p];
    }
}

static uint8_t poly_eval(const uint8_t *s, uint8_t x,
                         const struct galois_field *gf)
{
    int i;
    uint8_t sum = 0;
    uint8_t log_x = gf->log[x];

    if (!x)
        return s[0];

    for (i = 0; i < MAX_POLY; i++) {
        uint8_t c = s[i];

        if (!c)
            continue;

        sum ^= gf->exp[(gf->log[c] + log_x * i) % gf->p];
    }

    return sum;
}

/************************************************************************
 * Berlekamp-Massey algorithm for finding error locator polynomials.
 */

static void berlekamp_massey(const uint8_t *s, int N,
                             const struct galois_field *gf,
                             uint8_t *sigma)
{
    uint8_t C[MAX_POLY];
    uint8_t B[MAX_POLY];
    int L = 0;
    int m = 1;
    uint8_t b = 1;
    int n;

    memset(B, 0, sizeof(B));
    memset(C, 0, sizeof(C));
    B[0] = 1;
    C[0] = 1;

    for (n = 0; n < N; n++) {
        uint8_t d = s[n];
        uint8_t mult;
        int i;

        for (i = 1; i <= L; i++) {
            if (!(C[i] && s[n - i]))
                continue;

            d ^= gf->exp[(gf->log[C[i]] +
                 gf->log[s[n - i]]) %
                 gf->p];
        }

        mult = gf->exp[(gf->p - gf->log[b] + gf->log[d]) % gf->p];

        if (!d) {
            m++;
        } else if (L * 2 <= n) {
            uint8_t T[MAX_POLY];

            memcpy(T, C, sizeof(T));
            poly_add(C, B, mult, m, gf);
            memcpy(B, T, sizeof(B));
            L = n + 1 - L;
            b = d;
            m = 1;
        } else {
            poly_add(C, B, mult, m, gf);
            m++;
        }
    }

    memcpy(sigma, C, MAX_POLY);
}

/************************************************************************
 * Code stream error correction
 *
 * Generator polynomial for GF(2^8) is x^8 + x^4 + x^3 + x^2 + 1
 */

static int block_syndromes(const uint8_t *data, int bs, int npar, uint8_t *s)
{
    int nonzero = 0;
    int i;

    memset(s, 0, MAX_POLY);

    for (i = 0; i < npar; i++) {
        int j;

        for (j = 0; j < bs; j++) {
            uint8_t c = data[bs - j - 1];

            if (!c)
                continue;

            s[i] ^= gf256_exp[((int)gf256_log[c] +
                    i * j) % 255];
        }

        if (s[i])
            nonzero = 1;
    }

    return nonzero;
}

static void eloc_poly(uint8_t *omega,
              const uint8_t *s, const uint8_t *sigma,
              int npar)
{
    int i;

    memset(omega, 0, MAX_POLY);

    for (i = 0; i < npar; i++) {
        const uint8_t a = sigma[i];
        const uint8_t log_a = gf256_log[a];
        int j;

        if (!a)
            continue;

        for (j = 0; j + 1 < MAX_POLY; j++) {
            const uint8_t b = s[j + 1];

            if (i + j >= npar)
                break;

            if (!b)
                continue;

            omega[i + j] ^=
                gf256_exp[(log_a + gf256_log[b]) % 255];
        }
    }
}

static quirc_decode_error_t correct_block(uint8_t *data,
                      const struct quirc_rs_params *ecc)
{
    int npar = ecc->bs - ecc->dw;
    uint8_t s[MAX_POLY];
    uint8_t sigma[MAX_POLY];
    uint8_t sigma_deriv[MAX_POLY];
    uint8_t omega[MAX_POLY];
    int i;

    /* Compute syndrome vector */
    if (!block_syndromes(data, ecc->bs, npar, s))
        return QUIRC_SUCCESS;

    berlekamp_massey(s, npar, &gf256, sigma);

    /* Compute derivative of sigma */
    memset(sigma_deriv, 0, MAX_POLY);
    for (i = 0; i + 1 < MAX_POLY; i += 2)
        sigma_deriv[i] = sigma[i + 1];

    /* Compute error evaluator polynomial */
    eloc_poly(omega, s, sigma, npar - 1);

    /* Find error locations and magnitudes */
    for (i = 0; i < ecc->bs; i++) {
        uint8_t xinv = gf256_exp[255 - i];

        if (!poly_eval(sigma, xinv, &gf256)) {
            uint8_t sd_x = poly_eval(sigma_deriv, xinv, &gf256);
            uint8_t omega_x = poly_eval(omega, xinv, &gf256);
            uint8_t error = gf256_exp[(255 - gf256_log[sd_x] +
                            gf256_log[omega_x]) % 255];

            data[ecc->bs - i - 1] ^= error;
        }
    }

    if (block_syndromes(data, ecc->bs, npar, s))
        return QUIRC_ERROR_DATA_ECC;

    return QUIRC_SUCCESS;
}

/************************************************************************
 * Format value error correction
 *
 * Generator polynomial for GF(2^4) is x^4 + x + 1
 */

#define FORMAT_MAX_ERROR    3
#define FORMAT_SYNDROMES    (FORMAT_MAX_ERROR * 2)
#define FORMAT_BITS         15

static int format_syndromes(uint16_t u, uint8_t *s)
{
    int i;
    int nonzero = 0;

    memset(s, 0, MAX_POLY);

    for (i = 0; i < FORMAT_SYNDROMES; i++) {
        int j;

        s[i] = 0;
        for (j = 0; j < FORMAT_BITS; j++)
            if (u & (1 << j))
                s[i] ^= gf16_exp[((i + 1) * j) % 15];

        if (s[i])
            nonzero = 1;
    }

    return nonzero;
}

static quirc_decode_error_t correct_format(uint16_t *f_ret)
{
    uint16_t u = *f_ret;
    int i;
    uint8_t s[MAX_POLY];
    uint8_t sigma[MAX_POLY];

    /* Evaluate U (received codeword) at each of alpha_1 .. alpha_6
     * to get S_1 .. S_6 (but we index them from 0).
     */
    if (!format_syndromes(u, s))
        return QUIRC_SUCCESS;

    berlekamp_massey(s, FORMAT_SYNDROMES, &gf16, sigma);

    /* Now, find the roots of the polynomial */
    for (i = 0; i < 15; i++)
        if (!poly_eval(sigma, gf16_exp[15 - i], &gf16))
            u ^= (1 << i);

    if (format_syndromes(u, s))
        return QUIRC_ERROR_FORMAT_ECC;

    *f_ret = u;
    return QUIRC_SUCCESS;
}

/************************************************************************
 * Decoder algorithm
 */

struct datastream {
    uint8_t raw[QUIRC_MAX_PAYLOAD];
    int     data_bits;
    int     ptr;

    uint8_t data[QUIRC_MAX_PAYLOAD];
};

static inline int grid_bit(const struct quirc_code *code, int x, int y)
{
    int p = y * code->size + x;

    return (code->cell_bitmap[p >> 3] >> (p & 7)) & 1;
}

static quirc_decode_error_t read_format(const struct quirc_code *code,
                                        struct quirc_data *data, int which)
{
    int i;
    uint16_t format = 0;
    uint16_t fdata;
    quirc_decode_error_t err;

    if (which) {
        for (i = 0; i < 7; i++)
            format = (format << 1) |
                grid_bit(code, 8, code->size - 1 - i);
        for (i = 0; i < 8; i++)
            format = (format << 1) |
                grid_bit(code, code->size - 8 + i, 8);
    } else {
        static const int xs[15] = {
            8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 4, 3, 2, 1, 0
        };
        static const int ys[15] = {
            0, 1, 2, 3, 4, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8
        };

        for (i = 14; i >= 0; i--)
            format = (format << 1) | grid_bit(code, xs[i], ys[i]);
    }

    format ^= 0x5412;

    err = correct_format(&format);
    if (err)
        return err;

    fdata = format >> 10;
    data->ecc_level = fdata >> 3;
    data->mask = fdata & 7;

    return QUIRC_SUCCESS;
}

static int mask_bit(int mask, int i, int j)
{
    switch (mask) {
    case 0: return !((i + j) % 2);
    case 1: return !(i % 2);
    case 2: return !(j % 3);
    case 3: return !((i + j) % 3);
    case 4: return !(((i / 2) + (j / 3)) % 2);
    case 5: return !((i * j) % 2 + (i * j) % 3);
    case 6: return !(((i * j) % 2 + (i * j) % 3) % 2);
    case 7: return !(((i * j) % 3 + (i + j) % 2) % 2);
    }

    return 0;
}

static int reserved_cell(int version, int i, int j)
{
    const struct quirc_version_info *ver = &quirc_version_db[version];
    int size = version * 4 + 17;
    int ai = -1, aj = -1, a;

    /* Finder + format: top left */
    if (i < 9 && j < 9)
        return 1;

    /* Finder + format: bottom left */
    if (i + 8 >= size && j < 9)
        return 1;

    /* Finder + format: top right */
    if (i < 9 && j + 8 >= size)
        return 1;

    /* Exclude timing patterns */
    if (i == 6 || j == 6)
        return 1;

    /* Exclude version info, if it exists. Version info sits adjacent to
     * the top-right and bottom-left finders in three rows, bounded by
     * the timing pattern.
     */
    if (version >= 7) {
        if (i < 6 && j + 11 >= size)
            return 1;
        if (i + 11 >= size && j < 6)
            return 1;
    }

    /* Exclude alignment patterns */
    for (a = 0; a < QUIRC_MAX_ALIGNMENT && ver->apat[a]; a++) {
        int p = ver->apat[a];

        if (abs(p - i) < 3)
            ai = a;
        if (abs(p - j) < 3)
            aj = a;
    }

    if (ai >= 0 && aj >= 0) {
        a--;
        if (ai > 0 && ai < a)
            return 1;
        if (aj > 0 && aj < a)
            return 1;
        if (aj == a && ai == a)
            return 1;
    }

    return 0;
}

static void read_bit(const struct quirc_code *code,
                     struct quirc_data *data,
                     struct datastream *ds, int i, int j)
{
    int bitpos = ds->data_bits & 7;
    int bytepos = ds->data_bits >> 3;
    int v = grid_bit(code, j, i);

    if (mask_bit(data->mask, i, j))
        v ^= 1;

    if (v)
        ds->raw[bytepos] |= (0x80 >> bitpos);

    ds->data_bits++;
}

static void read_data(const struct quirc_code *code,
                      struct quirc_data *data,
                      struct datastream *ds)
{
    int y = code->size - 1;
    int x = code->size - 1;
    int dir = -1;

    while (x > 0) {
        if (x == 6)
            x--;

        if (!reserved_cell(data->version, y, x))
            read_bit(code, data, ds, y, x);

        if (!reserved_cell(data->version, y, x - 1))
            read_bit(code, data, ds, y, x - 1);

        y += dir;
        if (y < 0 || y >= code->size) {
            dir = -dir;
            x -= 2;
            y += dir;
        }
    }
}

static quirc_decode_error_t codestream_ecc(struct quirc_data *data,
                                           struct datastream *ds)
{
    const struct quirc_version_info *ver =
        &quirc_version_db[data->version];
    const struct quirc_rs_params *sb_ecc = &ver->ecc[data->ecc_level];
    struct quirc_rs_params lb_ecc;
    const int lb_count =
        (ver->data_bytes - sb_ecc->bs * sb_ecc->ns) / (sb_ecc->bs + 1);
    const int bc = lb_count + sb_ecc->ns;
    const int ecc_offset = sb_ecc->dw * bc + lb_count;
    int dst_offset = 0;
    int i;

    memcpy(&lb_ecc, sb_ecc, sizeof(lb_ecc));
    lb_ecc.dw++;
    lb_ecc.bs++;

    for (i = 0; i < bc; i++) {
        uint8_t *dst = ds->data + dst_offset;
        const struct quirc_rs_params *ecc =
            (i < sb_ecc->ns) ? sb_ecc : &lb_ecc;
        const int num_ec = ecc->bs - ecc->dw;
        quirc_decode_error_t err;
        int j;

        for (j = 0; j < ecc->dw; j++)
            dst[j] = ds->raw[j * bc + i];
        for (j = 0; j < num_ec; j++)
            dst[ecc->dw + j] = ds->raw[ecc_offset + j * bc + i];

        err = correct_block(dst, ecc);
        if (err)
            return err;

        dst_offset += ecc->dw;
    }

    ds->data_bits = dst_offset * 8;

    return QUIRC_SUCCESS;
}

static inline int bits_remaining(const struct datastream *ds)
{
    return ds->data_bits - ds->ptr;
}

static int take_bits(struct datastream *ds, int len)
{
    int ret = 0;

    while (len && (ds->ptr < ds->data_bits)) {
        uint8_t b = ds->data[ds->ptr >> 3];
        int bitpos = ds->ptr & 7;

        ret <<= 1;
        if ((b << bitpos) & 0x80)
            ret |= 1;

        ds->ptr++;
        len--;
    }

    return ret;
}

static int numeric_tuple(struct quirc_data *data,
                         struct datastream *ds,
                         int bits, int digits)
{
    int tuple;
    int i;

    if (bits_remaining(ds) < bits)
        return -1;

    tuple = take_bits(ds, bits);

    for (i = digits - 1; i >= 0; i--) {
        data->payload[data->payload_len + i] = tuple % 10 + '0';
        tuple /= 10;
    }

    data->payload_len += digits;
    return 0;
}

static quirc_decode_error_t decode_numeric(struct quirc_data *data,
                                           struct datastream *ds)
{
    int bits = 14;
    int count;

    if (data->version < 10)
        bits = 10;
    else if (data->version < 27)
        bits = 12;

    count = take_bits(ds, bits);
    if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
        return QUIRC_ERROR_DATA_OVERFLOW;

    while (count >= 3) {
        if (numeric_tuple(data, ds, 10, 3) < 0)
            return QUIRC_ERROR_DATA_UNDERFLOW;
        count -= 3;
    }

    if (count >= 2) {
        if (numeric_tuple(data, ds, 7, 2) < 0)
            return QUIRC_ERROR_DATA_UNDERFLOW;
        count -= 2;
    }

    if (count) {
        if (numeric_tuple(data, ds, 4, 1) < 0)
            return QUIRC_ERROR_DATA_UNDERFLOW;
        count--;
    }

    return QUIRC_SUCCESS;
}

static int alpha_tuple(struct quirc_data *data,
                       struct datastream *ds,
                       int bits, int digits)
{
    int tuple;
    int i;

    if (bits_remaining(ds) < bits)
        return -1;

    tuple = take_bits(ds, bits);

    for (i = 0; i < digits; i++) {
        static const char *alpha_map =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

        data->payload[data->payload_len + digits - i - 1] =
            alpha_map[tuple % 45];
        tuple /= 45;
    }

    data->payload_len += digits;
    return 0;
}

static quirc_decode_error_t decode_alpha(struct quirc_data *data,
                                         struct datastream *ds)
{
    int bits = 13;
    int count;

    if (data->version < 10)
        bits = 9;
    else if (data->version < 27)
        bits = 11;

    count = take_bits(ds, bits);
    if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
        return QUIRC_ERROR_DATA_OVERFLOW;

    while (count >= 2) {
        if (alpha_tuple(data, ds, 11, 2) < 0)
            return QUIRC_ERROR_DATA_UNDERFLOW;
        count -= 2;
    }

    if (count) {
        if (alpha_tuple(data, ds, 6, 1) < 0)
            return QUIRC_ERROR_DATA_UNDERFLOW;
        count--;
    }

    return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_byte(struct quirc_data *data,
                                        struct datastream *ds)
{
    int bits = 16;
    int count;
    int i;

    if (data->version < 10)
        bits = 8;

    count = take_bits(ds, bits);
    if (data->payload_len + count + 1 > QUIRC_MAX_PAYLOAD)
        return QUIRC_ERROR_DATA_OVERFLOW;
    if (bits_remaining(ds) < count * 8)
        return QUIRC_ERROR_DATA_UNDERFLOW;

    for (i = 0; i < count; i++)
        data->payload[data->payload_len++] = take_bits(ds, 8);

    return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_kanji(struct quirc_data *data,
                                         struct datastream *ds)
{
    int bits = 12;
    int count;
    int i;

    if (data->version < 10)
        bits = 8;
    else if (data->version < 27)
        bits = 10;

    count = take_bits(ds, bits);
    if (data->payload_len + count * 2 + 1 > QUIRC_MAX_PAYLOAD)
        return QUIRC_ERROR_DATA_OVERFLOW;
    if (bits_remaining(ds) < count * 13)
        return QUIRC_ERROR_DATA_UNDERFLOW;

    for (i = 0; i < count; i++) {
        int d = take_bits(ds, 13);
        int msB = d / 0xc0;
        int lsB = d % 0xc0;
        int intermediate = (msB << 8) | lsB;
        uint16_t sjw;

        if (intermediate + 0x8140 <= 0x9ffc) {
            /* bytes are in the range 0x8140 to 0x9FFC */
            sjw = intermediate + 0x8140;
        } else {
            /* bytes are in the range 0xE040 to 0xEBBF */
            sjw = intermediate + 0xc140;
        }

        data->payload[data->payload_len++] = sjw >> 8;
        data->payload[data->payload_len++] = sjw & 0xff;
    }

    return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_eci(struct quirc_data *data,
                                       struct datastream *ds)
{
    if (bits_remaining(ds) < 8)
        return QUIRC_ERROR_DATA_UNDERFLOW;

    data->eci = take_bits(ds, 8);

    if ((data->eci & 0xc0) == 0x80) {
        if (bits_remaining(ds) < 8)
            return QUIRC_ERROR_DATA_UNDERFLOW;

        data->eci = (data->eci << 8) | take_bits(ds, 8);
    } else if ((data->eci & 0xe0) == 0xc0) {
        if (bits_remaining(ds) < 16)
            return QUIRC_ERROR_DATA_UNDERFLOW;

        data->eci = (data->eci << 16) | take_bits(ds, 16);
    }

    return QUIRC_SUCCESS;
}

static quirc_decode_error_t decode_payload(struct quirc_data *data,
                                           struct datastream *ds)
{
    while (bits_remaining(ds) >= 4) {
        quirc_decode_error_t err = QUIRC_SUCCESS;
        int type = take_bits(ds, 4);

        switch (type) {
        case QUIRC_DATA_TYPE_NUMERIC:
            err = decode_numeric(data, ds);
            break;

        case QUIRC_DATA_TYPE_ALPHA:
            err = decode_alpha(data, ds);
            break;

        case QUIRC_DATA_TYPE_BYTE:
            err = decode_byte(data, ds);
            break;

        case QUIRC_DATA_TYPE_KANJI:
            err = decode_kanji(data, ds);
            break;

        case 7:
            err = decode_eci(data, ds);
            break;

        default:
            goto done;
        }

        if (err)
            return err;

        if (!(type & (type - 1)) && (type > data->data_type))
            data->data_type = type;
    }

done:

    /* Add nul terminator to all payloads */
    if (data->payload_len >= sizeof(data->payload))
        data->payload_len--;
    data->payload[data->payload_len] = 0;

    return QUIRC_SUCCESS;
}

quirc_decode_error_t quirc_decode(const struct quirc_code *code,
                                  struct quirc_data *data)
{
    quirc_decode_error_t err;
    struct datastream *ds = fb_alloc(sizeof(struct datastream), FB_ALLOC_NO_HINT);

    if ((code->size - 17) % 4)
        { fb_free(); return QUIRC_ERROR_INVALID_GRID_SIZE; }

    memset(data, 0, sizeof(*data));
    memset(ds, 0, sizeof(*ds));

    data->version = (code->size - 17) / 4;

    if (data->version < 1 ||
        data->version > QUIRC_MAX_VERSION)
        { fb_free(); return QUIRC_ERROR_INVALID_VERSION; }

    /* Read format information -- try both locations */
    err = read_format(code, data, 0);
    if (err)
        err = read_format(code, data, 1);
    if (err)
        { fb_free(); return err; }

    read_data(code, data, ds);
    err = codestream_ecc(data, ds);
    if (err)
        { fb_free(); return err; }

    err = decode_payload(data, ds);
    if (err)
        { fb_free(); return err; }

    fb_free(); return QUIRC_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "quirc.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

const char *quirc_version(void)
{
    return "1.0";
}

struct quirc *quirc_new(void)
{
    struct quirc *q = fb_alloc(sizeof(*q), FB_ALLOC_NO_HINT);

    if (!q)
        return NULL;

    memset(q, 0, sizeof(*q));
    return q;
}

void quirc_destroy(struct quirc *q)
{
    if (q->image)
        if (q->image) fb_free();
    if (sizeof(*q->image) != sizeof(*q->pixels))
        if (q->pixels) fb_free();

    if (q) fb_free();
}

int quirc_resize(struct quirc *q, int w, int h)
{
    if (q->image) fb_free();
    uint8_t *new_image = fb_alloc(w * h, FB_ALLOC_NO_HINT);

    if (!new_image)
        return -1;

    if (sizeof(*q->image) != sizeof(*q->pixels)) {
        size_t new_size = w * h * sizeof(quirc_pixel_t);
        if (q->pixels) fb_free();
        quirc_pixel_t *new_pixels = fb_alloc(new_size, FB_ALLOC_NO_HINT);
        if (!new_pixels) {
            fb_free();
            return -1;
        }
        q->pixels = new_pixels;
    }

    q->image = new_image;
    q->w = w;
    q->h = h;

    return 0;
}

int quirc_count(const struct quirc *q)
{
    return q->num_grids;
}

static const char *const error_table[] = {
    [QUIRC_SUCCESS] = "Success",
    [QUIRC_ERROR_INVALID_GRID_SIZE] = "Invalid grid size",
    [QUIRC_ERROR_INVALID_VERSION] = "Invalid version",
    [QUIRC_ERROR_FORMAT_ECC] = "Format data ECC failure",
    [QUIRC_ERROR_DATA_ECC] = "ECC failure",
    [QUIRC_ERROR_UNKNOWN_DATA_TYPE] = "Unknown data type",
    [QUIRC_ERROR_DATA_OVERFLOW] = "Data overflow",
    [QUIRC_ERROR_DATA_UNDERFLOW] = "Data underflow"
};

const char *quirc_strerror(quirc_decode_error_t err)
{
    if (err >= 0 && err < sizeof(error_table) / sizeof(error_table[0]))
        return error_table[err];

    return "Unknown error";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void imlib_find_qrcodes(list_t *out, image_t *ptr, rectangle_t *roi)
{
    struct quirc *controller = quirc_new();
    quirc_resize(controller, roi->w, roi->h);
    uint8_t *grayscale_image = quirc_begin(controller, NULL, NULL);

    switch(ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                memcpy(grayscale_image, &row_ptr[roi->x], roi->w);
                grayscale_image += roi->w;
//                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
//                    *(grayscale_image++) = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
//                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        default: {
            memset(grayscale_image, 0, roi->w * roi->h);
            break;
        }
    }

    quirc_end(controller);
    list_init(out, sizeof(find_qrcodes_list_lnk_data_t));

    for (int i = 0, j = quirc_count(controller); i < j; i++) {
        struct quirc_code *code = fb_alloc(sizeof(struct quirc_code), FB_ALLOC_NO_HINT);
        struct quirc_data *data = fb_alloc(sizeof(struct quirc_data), FB_ALLOC_NO_HINT);
        quirc_extract(controller, i, code);

        if(quirc_decode(code, data) == QUIRC_SUCCESS) {
            find_qrcodes_list_lnk_data_t lnk_data;
            rectangle_init(&(lnk_data.rect), code->corners[0].x + roi->x, code->corners[0].y + roi->y, 0, 0);

            for (size_t k = 1, l = (sizeof(code->corners) / sizeof(code->corners[0])); k < l; k++) {
                rectangle_t temp;
                rectangle_init(&temp, code->corners[k].x + roi->x, code->corners[k].y + roi->y, 0, 0);
                rectangle_united(&(lnk_data.rect), &temp);
            }

            // Add corners...
            lnk_data.corners[0].x = fast_roundf(code->corners[0].x) + roi->x; // top-left
            lnk_data.corners[0].y = fast_roundf(code->corners[0].y) + roi->y; // top-left
            lnk_data.corners[1].x = fast_roundf(code->corners[1].x) + roi->x; // top-right
            lnk_data.corners[1].y = fast_roundf(code->corners[1].y) + roi->y; // top-right
            lnk_data.corners[2].x = fast_roundf(code->corners[2].x) + roi->x; // bottom-right
            lnk_data.corners[2].y = fast_roundf(code->corners[2].y) + roi->y; // bottom-right
            lnk_data.corners[3].x = fast_roundf(code->corners[3].x) + roi->x; // bottom-left
            lnk_data.corners[3].y = fast_roundf(code->corners[3].y) + roi->y; // bottom-left

            // Payload is already null terminated.
            lnk_data.payload_len = data->payload_len;
            lnk_data.payload = xalloc(data->payload_len);
            memcpy(lnk_data.payload, data->payload, data->payload_len);

            lnk_data.version = data->version;
            lnk_data.ecc_level = data->ecc_level;
            lnk_data.mask = data->mask;
            lnk_data.data_type = data->data_type;
            lnk_data.eci = data->eci;

            list_push_back(out, &lnk_data);
        }

        fb_free();
        fb_free();
    }

    quirc_destroy(controller);
}
#endif //IMLIB_ENABLE_QRCODES
