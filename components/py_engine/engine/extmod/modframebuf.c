/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"

#include "py/objstr.h"

#if MICROPY_PY_FRAMEBUF

#include "font_petme128_8x8.h"

typedef struct _mp_obj_framebuf_t {
    mp_obj_base_t base;
    mp_obj_t buf_obj; // need to store this to prevent GC from reclaiming buf
    void *buf;
    uint16_t width, height, stride;
    uint8_t format;
} mp_obj_framebuf_t;

typedef struct {
    char *font_path;
    FILE *fp;
    unsigned char font_height;
    unsigned char font_width;
    unsigned char is_cn;
} font_t;

// #if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_obj_type_t mp_type_framebuf;
// #endif

typedef void (*setpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, uint32_t);
typedef uint32_t (*getpixel_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int);
typedef void (*fill_rect_t)(const mp_obj_framebuf_t *, unsigned int, unsigned int, unsigned int, unsigned int, uint32_t);

typedef struct _mp_framebuf_p_t {
    setpixel_t setpixel;
    getpixel_t getpixel;
    fill_rect_t fill_rect;
} mp_framebuf_p_t;

// constants for formats
#define FRAMEBUF_MVLSB    (0)
#define FRAMEBUF_RGB565   (1)
#define FRAMEBUF_GS2_HMSB (5)
#define FRAMEBUF_GS4_HMSB (2)
#define FRAMEBUF_GS8      (6)
#define FRAMEBUF_MHLSB    (3)
#define FRAMEBUF_MHMSB    (4)

// constants for fonts
// 支持中英文 12-32
#define FONT_ASC8_8 (808)
#define FONT_ASC12_8 (1208)
// #define FONT_PATH_ASC12_8 "/data/font/ASC12_8"
#define FONT_ASC16_8 (1608)
// #define FONT_PATH_ASC16_8 "/data/font/ASC16_8"
#define FONT_ASC24_12 (2412)
// #define FONT_PATH_ASC24_12 "/data/font/ASC24_12"
#define FONT_ASC32_16 (3216)
// #define FONT_PATH_ASC32_16 "/data/font/ASC32_16"
// #define FONT_ASC48_24 (48)
// #define FONT_PATH_ASC48_24 "./font/ASC48_24"
#define FONT_HZK12 (1212)
// #define FONT_PATH_HZK12 "/data/font/HZK12"
#define FONT_HZK16 (1616)
// #define FONT_PATH_HZK16 "/data/font/HZK16"
#define FONT_HZK24 (2424)
// #define FONT_PATH_HZK24 "/data/font/HZK24"
#define FONT_HZK32 (3232)
// #define FONT_PATH_HZK32 "/data/font/HZK32"

static font_t ASC8_8 = {NULL, NULL, 8, 8, 0};
static font_t ASC12_8 = {NULL, NULL, 12, 8, 0};
static font_t ASC16_8 = {NULL, NULL, 16, 8, 0};
static font_t ASC24_12 = {NULL, NULL, 24, 12, 0};
static font_t ASC32_16 = {NULL, NULL, 32, 16, 0};

static font_t HZK12 = {NULL, NULL, 12, 12, 1};
static font_t HZK16 = {NULL, NULL, 16, 16, 1};
static font_t HZK24 = {NULL, NULL, 24, 24, 1};
static font_t HZK32 = {NULL, NULL, 32, 32, 1};

// Functions for MHLSB and MHMSB

STATIC void mono_horiz_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

STATIC uint32_t mono_horiz_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    size_t index = (x + y * fb->stride) >> 3;
    unsigned int offset = fb->format == FRAMEBUF_MHMSB ? x & 0x07 : 7 - (x & 0x07);
    return (((uint8_t *)fb->buf)[index] >> (offset)) & 0x01;
}

STATIC void mono_horiz_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    unsigned int reverse = fb->format == FRAMEBUF_MHMSB;
    unsigned int advance = fb->stride >> 3;
    while (w--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(x >> 3) + y * advance];
        unsigned int offset = reverse ?  x & 7 : 7 - (x & 7);
        for (unsigned int hh = h; hh; --hh) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            b += advance;
        }
        ++x;
    }
}

// Functions for MVLSB format

STATIC void mvlsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    size_t index = (y >> 3) * fb->stride + x;
    uint8_t offset = y & 0x07;
    ((uint8_t *)fb->buf)[index] = (((uint8_t *)fb->buf)[index] & ~(0x01 << offset)) | ((col != 0) << offset);
}

STATIC uint32_t mvlsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return (((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x] >> (y & 0x07)) & 0x01;
}

STATIC void mvlsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    while (h--) {
        uint8_t *b = &((uint8_t *)fb->buf)[(y >> 3) * fb->stride + x];
        uint8_t offset = y & 0x07;
        for (unsigned int ww = w; ww; --ww) {
            *b = (*b & ~(0x01 << offset)) | ((col != 0) << offset);
            ++b;
        }
        ++y;
    }
}

// Functions for RGB565 format

STATIC void rgb565_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    ((uint16_t *)fb->buf)[x + y * fb->stride] = col;
}

STATIC uint32_t rgb565_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint16_t *)fb->buf)[x + y * fb->stride];
}

STATIC void rgb565_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint16_t *b = &((uint16_t *)fb->buf)[x + y * fb->stride];
    while (h--) {
        for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
}

// Functions for GS2_HMSB format

STATIC void gs2_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    uint8_t mask = 0x3 << shift;
    uint8_t color = (col & 0x3) << shift;
    *pixel = color | (*pixel & (~mask));
}

STATIC uint32_t gs2_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    uint8_t pixel = ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 2];
    uint8_t shift = (x & 0x3) << 1;
    return (pixel >> shift) & 0x3;
}

STATIC void gs2_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    for (unsigned int xx = x; xx < x + w; xx++) {
        for (unsigned int yy = y; yy < y + h; yy++) {
            gs2_hmsb_setpixel(fb, xx, yy, col);
        }
    }
}

// Functions for GS4_HMSB format

STATIC void gs4_hmsb_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];

    if (x % 2) {
        *pixel = ((uint8_t)col & 0x0f) | (*pixel & 0xf0);
    } else {
        *pixel = ((uint8_t)col << 4) | (*pixel & 0x0f);
    }
}

STATIC uint32_t gs4_hmsb_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    if (x % 2) {
        return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] & 0x0f;
    }

    return ((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1] >> 4;
}

STATIC void gs4_hmsb_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    col &= 0x0f;
    uint8_t *pixel_pair = &((uint8_t *)fb->buf)[(x + y * fb->stride) >> 1];
    uint8_t col_shifted_left = col << 4;
    uint8_t col_pixel_pair = col_shifted_left | col;
    unsigned int pixel_count_till_next_line = (fb->stride - w) >> 1;
    bool odd_x = (x % 2 == 1);

    while (h--) {
        unsigned int ww = w;

        if (odd_x && ww > 0) {
            *pixel_pair = (*pixel_pair & 0xf0) | col;
            pixel_pair++;
            ww--;
        }

        memset(pixel_pair, col_pixel_pair, ww >> 1);
        pixel_pair += ww >> 1;

        if (ww % 2) {
            *pixel_pair = col_shifted_left | (*pixel_pair & 0x0f);
            if (!odd_x) {
                pixel_pair++;
            }
        }

        pixel_pair += pixel_count_till_next_line;
    }
}

// Functions for GS8 format

STATIC void gs8_setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    *pixel = col & 0xff;
}

STATIC uint32_t gs8_getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return ((uint8_t *)fb->buf)[(x + y * fb->stride)];
}

STATIC void gs8_fill_rect(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t col) {
    uint8_t *pixel = &((uint8_t *)fb->buf)[(x + y * fb->stride)];
    while (h--) {
        memset(pixel, col, w);
        pixel += fb->stride;
    }
}

STATIC mp_framebuf_p_t formats[] = {
    [FRAMEBUF_MVLSB] = {mvlsb_setpixel, mvlsb_getpixel, mvlsb_fill_rect},
    [FRAMEBUF_RGB565] = {rgb565_setpixel, rgb565_getpixel, rgb565_fill_rect},
    [FRAMEBUF_GS2_HMSB] = {gs2_hmsb_setpixel, gs2_hmsb_getpixel, gs2_hmsb_fill_rect},
    [FRAMEBUF_GS4_HMSB] = {gs4_hmsb_setpixel, gs4_hmsb_getpixel, gs4_hmsb_fill_rect},
    [FRAMEBUF_GS8] = {gs8_setpixel, gs8_getpixel, gs8_fill_rect},
    [FRAMEBUF_MHLSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
    [FRAMEBUF_MHMSB] = {mono_horiz_setpixel, mono_horiz_getpixel, mono_horiz_fill_rect},
};

static inline void setpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y, uint32_t col) {
    formats[fb->format].setpixel(fb, x, y, col);
}

static inline uint32_t getpixel(const mp_obj_framebuf_t *fb, unsigned int x, unsigned int y) {
    return formats[fb->format].getpixel(fb, x, y);
}

STATIC void fill_rect(const mp_obj_framebuf_t *fb, int x, int y, int w, int h, uint32_t col) {
    if (h < 1 || w < 1 || x + w <= 0 || y + h <= 0 || y >= fb->height || x >= fb->width) {
        // No operation needed.
        return;
    }

    // clip to the framebuffer
    int xend = MIN(fb->width, x + w);
    int yend = MIN(fb->height, y + h);
    x = MAX(x, 0);
    y = MAX(y, 0);

    formats[fb->format].fill_rect(fb, x, y, xend - x, yend - y, col);
}

STATIC mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 4, 5, false);

    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = type;
    o->buf_obj = args[0];

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->format = mp_obj_get_int(args[3]);
    if (n_args >= 5) {
        o->stride = mp_obj_get_int(args[4]);
    } else {
        o->stride = o->width;
    }

    switch (o->format) {
        case FRAMEBUF_MVLSB:
        case FRAMEBUF_RGB565:
            break;
        case FRAMEBUF_MHLSB:
        case FRAMEBUF_MHMSB:
            o->stride = (o->stride + 7) & ~7;
            break;
        case FRAMEBUF_GS2_HMSB:
            o->stride = (o->stride + 3) & ~3;
            break;
        case FRAMEBUF_GS4_HMSB:
            o->stride = (o->stride + 1) & ~1;
            break;
        case FRAMEBUF_GS8:
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_int_t framebuf_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    bufinfo->buf = self->buf;
    bufinfo->len = self->stride * self->height * (self->format == FRAMEBUF_RGB565 ? 2 : 1);
    bufinfo->typecode = 'B'; // view framebuf as bytes
    return 0;
}

STATIC mp_obj_t framebuf_fill(mp_obj_t self_in, mp_obj_t col_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t col = mp_obj_get_int(col_in);
    formats[self->format].fill_rect(self, 0, 0, self->width, self->height, col);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf_fill_obj, framebuf_fill);

STATIC mp_obj_t framebuf_fill_rect(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t height = mp_obj_get_int(args[4]);
    mp_int_t col = mp_obj_get_int(args[5]);

    fill_rect(self, x, y, width, height, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_fill_rect_obj, 6, 6, framebuf_fill_rect);

STATIC mp_obj_t framebuf_pixel(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    if (0 <= x && x < self->width && 0 <= y && y < self->height) {
        if (n_args == 3) {
            // get
            return MP_OBJ_NEW_SMALL_INT(getpixel(self, x, y));
        } else {
            // set
            setpixel(self, x, y, mp_obj_get_int(args[3]));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_pixel_obj, 3, 4, framebuf_pixel);

STATIC mp_obj_t framebuf_hline(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t col = mp_obj_get_int(args[4]);

    fill_rect(self, x, y, w, 1, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_hline_obj, 5, 5, framebuf_hline);

STATIC mp_obj_t framebuf_vline(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t h = mp_obj_get_int(args[3]);
    mp_int_t col = mp_obj_get_int(args[4]);

    fill_rect(self, x, y, 1, h, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_vline_obj, 5, 5, framebuf_vline);

STATIC mp_obj_t framebuf_rect(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t w = mp_obj_get_int(args[3]);
    mp_int_t h = mp_obj_get_int(args[4]);
    mp_int_t col = mp_obj_get_int(args[5]);

    fill_rect(self, x, y, w, 1, col);
    fill_rect(self, x, y + h - 1, w, 1, col);
    fill_rect(self, x, y, 1, h, col);
    fill_rect(self, x + w - 1, y, 1, h, col);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_rect_obj, 6, 6, framebuf_rect);

STATIC mp_obj_t framebuf_line(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x1 = mp_obj_get_int(args[1]);
    mp_int_t y1 = mp_obj_get_int(args[2]);
    mp_int_t x2 = mp_obj_get_int(args[3]);
    mp_int_t y2 = mp_obj_get_int(args[4]);
    mp_int_t col = mp_obj_get_int(args[5]);

    mp_int_t dx = x2 - x1;
    mp_int_t sx;
    if (dx > 0) {
        sx = 1;
    } else {
        dx = -dx;
        sx = -1;
    }

    mp_int_t dy = y2 - y1;
    mp_int_t sy;
    if (dy > 0) {
        sy = 1;
    } else {
        dy = -dy;
        sy = -1;
    }

    bool steep;
    if (dy > dx) {
        mp_int_t temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
        temp = sx;
        sx = sy;
        sy = temp;
        steep = true;
    } else {
        steep = false;
    }

    mp_int_t e = 2 * dy - dx;
    for (mp_int_t i = 0; i < dx; ++i) {
        if (steep) {
            if (0 <= y1 && y1 < self->width && 0 <= x1 && x1 < self->height) {
                setpixel(self, y1, x1, col);
            }
        } else {
            if (0 <= x1 && x1 < self->width && 0 <= y1 && y1 < self->height) {
                setpixel(self, x1, y1, col);
            }
        }
        while (e >= 0) {
            y1 += sy;
            e -= 2 * dx;
        }
        x1 += sx;
        e += 2 * dy;
    }

    if (0 <= x2 && x2 < self->width && 0 <= y2 && y2 < self->height) {
        setpixel(self, x2, y2, col);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_line_obj, 6, 6, framebuf_line);

STATIC mp_obj_t framebuf_blit(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t source_in = mp_obj_cast_to_native_base(args[1], MP_OBJ_FROM_PTR(&mp_type_framebuf));
    if (source_in == MP_OBJ_NULL) {
        mp_raise_TypeError(NULL);
    }
    mp_obj_framebuf_t *source = MP_OBJ_TO_PTR(source_in);

    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t key = -1;
    if (n_args > 4) {
        key = mp_obj_get_int(args[4]);
    }

    if (
        (x >= self->width) ||
        (y >= self->height) ||
        (-x >= source->width) ||
        (-y >= source->height)
        ) {
        // Out of bounds, no-op.
        return mp_const_none;
    }

    // Clip.
    int x0 = MAX(0, x);
    int y0 = MAX(0, y);
    int x1 = MAX(0, -x);
    int y1 = MAX(0, -y);
    int x0end = MIN(self->width, x + source->width);
    int y0end = MIN(self->height, y + source->height);

    for (; y0 < y0end; ++y0) {
        int cx1 = x1;
        for (int cx0 = x0; cx0 < x0end; ++cx0) {
            uint32_t col = getpixel(source, cx1, y1);
            if (col != (uint32_t)key) {
                setpixel(self, cx0, y0, col);
            }
            ++cx1;
        }
        ++y1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(framebuf_blit_obj, 4, 5, framebuf_blit);

STATIC mp_obj_t framebuf_scroll(mp_obj_t self_in, mp_obj_t xstep_in, mp_obj_t ystep_in) {
    mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t xstep = mp_obj_get_int(xstep_in);
    mp_int_t ystep = mp_obj_get_int(ystep_in);
    int sx, y, xend, yend, dx, dy;
    if (xstep < 0) {
        sx = 0;
        xend = self->width + xstep;
        dx = 1;
    } else {
        sx = self->width - 1;
        xend = xstep - 1;
        dx = -1;
    }
    if (ystep < 0) {
        y = 0;
        yend = self->height + ystep;
        dy = 1;
    } else {
        y = self->height - 1;
        yend = ystep - 1;
        dy = -1;
    }
    for (; y != yend; y += dy) {
        for (int x = sx; x != xend; x += dx) {
            setpixel(self, x, y, getpixel(self, x - xstep, y - ystep));
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(framebuf_scroll_obj, framebuf_scroll);

STATIC unsigned char hz_get_mat(font_t *font, char *chr, unsigned char *mat)
{
    unsigned char font_width;
    if (font == &HZK12) {
        font_width = 16;
    } else {
        font_width = font->font_width;
    }

    unsigned char size = font->font_height;
    unsigned char offset_step = (font->font_height * font_width / 8);
    unsigned long offset = 0;

    int t1 = (int) (*(chr) & 0xff);
    int t2 = (int) (*(chr + 1) & 0xff);

    if (t1 > 0xa0) {
        if (size == 40 || size == 48) {
            offset = ((t1 - 0xa1 - 0x0f) * 94 + (t2 - 0xa1)) * offset_step;
        } else if (size == 12 || size == 16 || size == 24 || size == 32) {
            offset = ((t1 - 0xa1) * 94 + (t2 - 0xa1)) * offset_step;
        }
    } else {
        offset = (t1 + 156 - 1) * offset_step;
    }

    if (font->fp == NULL) {
        font->fp = fopen(font->font_path , "rb");
        if (font->fp == NULL) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("No font file %s"), font->font_path);
            memset(mat, 0, offset_step);
            return 0;
        }
    }

    fseek(font->fp, offset, SEEK_SET);
    fread(mat, offset_step, 1, font->fp);

    return 1;
}

STATIC void hz_print_mat(mp_obj_framebuf_t *self, mp_int_t x, mp_int_t y, mp_int_t col, font_t* font, unsigned char * mat, mp_int_t zoom)
{
    char key[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    if (font->font_height == 40 || font->font_height == 48) {
        for (int i = 0; i < font->font_height; i++) {
            for (int j = 0; j < font->font_width; j++) {
                int index = i * font->font_width + j;
                int flag = mat[index / 8] & key[index % 8];
                if (flag > 0) {
                    for (int r = 0; r < zoom; r++) {
                        setpixel(self, x + j * zoom + r, y + i * zoom, col);
                        for (int r = 0; r < zoom; r++) {
                            setpixel(self, x + j * zoom, y + i * zoom + r, col);
                        }
                    }
                }
            }
        }
    } else if (font->font_height == 16 || font->font_height == 24 || font->font_height == 32) {
        for (int i = 0; i < font->font_height; i++) {
            for (int j = 0; j < font->font_width; j++) {
                int index = j * font->font_width + i;
                int flag = mat[index / 8] & key[index % 8];
                if (flag > 0) {
                    for (int r = 0; r < zoom; r++) {
                        setpixel(self, x + j * zoom + r, y + i * zoom, col);
                        for (int r = 0; r < zoom; r++) {
                            setpixel(self, x + j * zoom, y + i * zoom + r, col);
                        }
                    }
                }
            }
        }
    } else if (font->font_height == 12) {
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 12; j++) {
                int index = j * 16 + i;
                int flag = mat[index / 8] & key[index % 8];
                if (flag > 0) {
                    for (int r = 0; r < zoom; r++) {
                        setpixel(self, x + j * zoom + r, y + i * zoom, col);
                        for (int r = 0; r < zoom; r++) {
                            setpixel(self, x + j * zoom, y + i * zoom + r, col);
                        }
                    }
                }
            }
        }
    }
}

STATIC unsigned char asc_get_mat(font_t *font, char *chr, unsigned char *mat)
{
    int ascii = (int) (*chr);
    if (ascii > 127 || ascii < 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("Input char is invaild"));
        return;
    }

    unsigned char offset_step = (font->font_width * font->font_height / 8);
    long offset = (ascii - 32) *  offset_step;

    if (font->fp == NULL) {
        font->fp = fopen(font->font_path , "rb");
        if (font->fp == NULL) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("error no font file %s"), font->font_path);
            memset(mat, 0, offset_step);
            return 0;
        }
    }

    fseek(font->fp, offset, SEEK_SET);
    fread(mat, offset_step, 1, font->fp);

    return 1;
}

STATIC void asc_print_mat(mp_obj_framebuf_t *self, mp_int_t x, mp_int_t y, mp_int_t col, font_t* font, unsigned char * mat, mp_int_t zoom)
{
    char key[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    if (font->font_height == 12 || font->font_height == 48) {
        for (int i = 0; i < font->font_height; i++) {
            for (int j = 0; j < font->font_width; j++) {
                int index = i * font->font_width + j;
                int flag = mat[index / 8] & key[index % 8];
                if (flag > 0) {
                    for (int r = 0; r < zoom; r++) {
                        setpixel(self, x + j * zoom + r, y + i * zoom, col);
                        for (int r = 0; r < zoom; r++) {
                            setpixel(self, x + j * zoom, y + i * zoom + r, col);
                        }
                    }
                }
            }
        }
        } else {
        for (int i = 0; i < font->font_height; i++) {
            for (int j = 0; j < font->font_width; j++) {
                    int index = j * font->font_height + i;
                int flag = mat[index / 8] & key[index % 8];
                if (flag > 0) {
                    for (int r = 0; r < zoom; r++) {
                        setpixel(self, x + j * zoom + r, y + i * zoom, col);
                        for (int r = 0; r < zoom; r++) {
                            setpixel(self, x + j * zoom, y + i * zoom + r, col);
                        }
                    }
                }
            }
        }
    }
}

STATIC mp_obj_t framebuf_text_helper(mp_obj_framebuf_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    // extract arguments
    enum { ARG_s, ARG_x, ARG_y, ARG_c, ARG_size, ARG_zoom, ARG_space, ARG_font };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_s, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_c, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
        { MP_QSTR_zoom, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_space, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_font, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t x0 = args[ARG_x].u_int;
    mp_int_t y0 = args[ARG_y].u_int;
    mp_int_t font = args[ARG_font].u_int;
    mp_int_t size = args[ARG_size].u_int;
    mp_int_t col = args[ARG_c].u_int;
    mp_int_t space = args[ARG_space].u_int;
    mp_int_t zoom = args[ARG_zoom].u_int;

    mp_check_self(mp_obj_is_str_or_bytes(args[ARG_s].u_obj));
    GET_STR_DATA_LEN(args[ARG_s].u_obj, str, str_len);

    if (str_len == 0)
        return mp_const_none;

    font_t *cn_font = NULL;
    font_t *en_font = NULL;

    if (font != -1) {
        switch (font) {
        case FONT_ASC8_8:
            cn_font = NULL;
            en_font = NULL;
            break;

        case FONT_ASC12_8:
            cn_font = NULL;
            en_font = &ASC12_8;
            break;
        case FONT_ASC16_8:
            cn_font = NULL;
            en_font = &ASC16_8;
            break;
        case FONT_ASC24_12:
            cn_font = NULL;
            en_font = &ASC24_12;
            break;
        case FONT_ASC32_16:
            cn_font = NULL;
            en_font = &ASC32_16;
            break;

        case FONT_HZK12:
            cn_font = &HZK12;
            en_font = NULL;
            break;
        case FONT_HZK16:
            cn_font = &HZK16;
            en_font = NULL;
            break;
        case FONT_HZK24:
            cn_font = &HZK24;
            en_font = NULL;
            break;
        case FONT_HZK32:
            cn_font = &HZK32;
            en_font = NULL;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("Wrong font"));
            break;
        }
    }

    if (size != -1) {
        switch (size) {
        case 8:
            cn_font = NULL;
            en_font = NULL;
            break;
        case 12:
            cn_font = &HZK12;
            en_font = &ASC12_8;
            break;
        case 16:
            cn_font = &HZK16;
            en_font = &ASC16_8;
            break;
        case 24:
            cn_font = &HZK24;
            en_font = &ASC24_12;
            break;
        case 32:
            cn_font = &HZK32;
            en_font = &ASC32_16;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("Wrong size. Only support 12 16 24 32"));
            break;
        }
    }

    if (font == -1 && size == -1) {
        cn_font = &HZK12;
        en_font = &ASC8_8;
    }

    int x = x0;

    // loop over chars
    for (int i = 0; *(str + i); ++i) {
        int chr = *(uint8_t *)(str + i);
        if (chr > 127) {
            // cn
            if (cn_font == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("Wrong chr. no cn font selected."));
                // continue;
            }

            if (cn_font->font_path == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("Wrong chr. no cn font file not set. See set_font_path."));
            }

            unsigned char *chr_data;

            if (cn_font == &HZK12) {
                chr_data = (unsigned char *)malloc(16 * cn_font->font_height / 8);
            } else {
                chr_data = (unsigned char *)malloc(cn_font->font_width * cn_font->font_height / 8);
            }

            if (hz_get_mat(cn_font, str + i, chr_data))
                hz_print_mat(self, x, y0, col, cn_font, chr_data, zoom);
            ++i;
            x += cn_font->font_width * zoom + space;
            free(chr_data);
        }
        if (chr >= 32 && chr <= 127) {
            // en
            if (en_font == &ASC8_8) {
                // get char data
                const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
                for (int j = 0; j < 8; j++, x++) {
                    if (0 <= x && x < self->width) {
                        uint vline_data = chr_data[j];
                        for (int y = y0; vline_data; vline_data >>= 1, y++) {
                            if (vline_data & 1) {
                                if (0 <= y && y < self->height) {
                                    setpixel(self, x, y, col);
                                }
                            }
                        }
                    }
                }
                x += space;
            } else {
                if (en_font == NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("Wrong chr. no en font selected"));
                    // continue;
                }

                if (en_font->font_path == NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("Wrong chr. no en font file not set. See set_font_path."));
                }

                unsigned char *chr_data = (unsigned char *)malloc(cn_font->font_width * cn_font->font_height / 8);
                if (asc_get_mat(en_font, str + i, chr_data))
                    asc_print_mat(self, x, y0, col, en_font, chr_data, zoom);
                x += en_font->font_width * zoom + space;
                free(chr_data);
            }
        }
    }

    if (cn_font && cn_font->fp) {
        fclose(cn_font->fp);
        cn_font->fp = NULL;
    }
    if (en_font && en_font->fp) {
        fclose(en_font->fp);
        en_font->fp = NULL;
    }

    return mp_const_none;
}

STATIC mp_obj_t framebuf_text(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    framebuf_text_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(framebuf_text_obj, 1, framebuf_text);


font_t *get_font_obj(int font_tag)
{
    switch (font_tag) {
    case FONT_ASC8_8:
        return &ASC8_8;
        break;
    case FONT_ASC12_8:
        return &ASC12_8;
        break;
    case FONT_ASC16_8:
        return &ASC16_8;
        break;
    case FONT_ASC24_12:
        return &ASC24_12;
        break;
    case FONT_ASC32_16:
        return &ASC32_16;
        break;
    case FONT_HZK12:
        return &HZK12;
        break;
    case FONT_HZK16:
        return &HZK16;
        break;
    case FONT_HZK24:
        return &HZK24;
        break;
    case FONT_HZK32:
        return &HZK32;
        break;
    default:
        return NULL;
        break;
    }
}

STATIC mp_obj_t framebuf_set_font_path(const mp_obj_t arg_font_tag, const mp_obj_t arg_font_path)
{
    mp_int_t font_tag = mp_obj_get_int(arg_font_tag);
    const char *font_path = mp_obj_str_get_str(arg_font_path);

    font_t *font = get_font_obj(font_tag);
    if (font == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid font"));
    }

    if (font->font_path != NULL) {
        free(font->font_path);
        font->font_path = NULL;
    }

    font->font_path = malloc(strnlen(font_path, 128) + 1);
    memcpy(font->font_path, font_path, strnlen(font_path, 128) + 1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(framebuf_set_font_path_obj, framebuf_set_font_path);

// #if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t framebuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&framebuf_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&framebuf_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&framebuf_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&framebuf_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&framebuf_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&framebuf_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&framebuf_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&framebuf_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_scroll), MP_ROM_PTR(&framebuf_scroll_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&framebuf_text_obj) },
};
STATIC MP_DEFINE_CONST_DICT(framebuf_locals_dict, framebuf_locals_dict_table);

STATIC const mp_obj_type_t mp_type_framebuf = {
    { &mp_type_type },
    .name = MP_QSTR_FrameBuffer,
    .make_new = framebuf_make_new,
    .buffer_p = { .get_buffer = framebuf_get_buffer },
    .locals_dict = (mp_obj_dict_t *)&framebuf_locals_dict,
};
// #endif

// this factory function is provided for backwards compatibility with old FrameBuffer1 class
STATIC mp_obj_t legacy_framebuffer1(size_t n_args, const mp_obj_t *args) {
    mp_obj_framebuf_t *o = m_new_obj(mp_obj_framebuf_t);
    o->base.type = &mp_type_framebuf;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    o->buf = bufinfo.buf;

    o->width = mp_obj_get_int(args[1]);
    o->height = mp_obj_get_int(args[2]);
    o->format = FRAMEBUF_MVLSB;
    if (n_args >= 4) {
        o->stride = mp_obj_get_int(args[3]);
    } else {
        o->stride = o->width;
    }

    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(legacy_framebuffer1_obj, 3, 4, legacy_framebuffer1);

// #if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t framebuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer), MP_ROM_PTR(&mp_type_framebuf) },
    { MP_ROM_QSTR(MP_QSTR_FrameBuffer1), MP_ROM_PTR(&legacy_framebuffer1_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_font_path), MP_ROM_PTR(&framebuf_set_font_path_obj) },
    { MP_ROM_QSTR(MP_QSTR_MVLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_VLSB), MP_ROM_INT(FRAMEBUF_MVLSB) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(FRAMEBUF_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_GS2_HMSB), MP_ROM_INT(FRAMEBUF_GS2_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS4_HMSB), MP_ROM_INT(FRAMEBUF_GS4_HMSB) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(FRAMEBUF_GS8) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HLSB), MP_ROM_INT(FRAMEBUF_MHLSB) },
    { MP_ROM_QSTR(MP_QSTR_MONO_HMSB), MP_ROM_INT(FRAMEBUF_MHMSB) },
    { MP_ROM_QSTR(MP_QSTR_FONT_ASC8_8), MP_ROM_INT(FONT_ASC8_8) },
    { MP_ROM_QSTR(MP_QSTR_FONT_ASC12_8), MP_ROM_INT(FONT_ASC12_8) },
    { MP_ROM_QSTR(MP_QSTR_FONT_ASC16_8), MP_ROM_INT(FONT_ASC16_8) },
    { MP_ROM_QSTR(MP_QSTR_FONT_ASC24_12), MP_ROM_INT(FONT_ASC24_12) },
    { MP_ROM_QSTR(MP_QSTR_FONT_ASC32_16), MP_ROM_INT(FONT_ASC32_16) },
    { MP_ROM_QSTR(MP_QSTR_FONT_HZK12), MP_ROM_INT(FONT_HZK12) },
    { MP_ROM_QSTR(MP_QSTR_FONT_HZK16), MP_ROM_INT(FONT_HZK16) },
    { MP_ROM_QSTR(MP_QSTR_FONT_HZK24), MP_ROM_INT(FONT_HZK24) },
    { MP_ROM_QSTR(MP_QSTR_FONT_HZK32), MP_ROM_INT(FONT_HZK32) },
};

STATIC MP_DEFINE_CONST_DICT(framebuf_module_globals, framebuf_module_globals_table);

const mp_obj_module_t mp_module_framebuf = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&framebuf_module_globals,
};
// #endif

#endif // MICROPY_PY_FRAMEBUF
