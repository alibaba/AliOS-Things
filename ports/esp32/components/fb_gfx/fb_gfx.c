// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "stdint.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "fb_gfx.h"

typedef struct
{                            // Data stored PER GLYPH
    uint16_t bitmapOffset;   // Pointer into GFXfont->bitmap
    uint8_t width, height;   // Bitmap dimensions in pixels
    uint8_t xAdvance;        // Distance to advance cursor (x axis)
    int8_t xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct
{                        // Data stored for FONT AS A WHOLE:
    uint8_t *bitmap;     // Glyph bitmaps, concatenated
    GFXglyph *glyph;     // Glyph array
    uint8_t first, last; // ASCII extents
    uint8_t yAdvance;    // Newline distance (y axis)
    uint8_t yOffset;     // Y offset of the font zero line (y axis)
} GFXfont;

#include "FreeMonoBold12pt7b.h" //14x24
#define gfxFont ((GFXfont *)(&FreeMonoBold12pt7b))

void fb_gfx_fillRect(camera_fb_t *fb, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    int bytes_per_pixel = 0;
    switch (fb->format)
    {
    case PIXFORMAT_GRAYSCALE:
        bytes_per_pixel = 1;
        break;
    case PIXFORMAT_RGB565:
        bytes_per_pixel = 2;
        break;
    case PIXFORMAT_RGB888:
        bytes_per_pixel = 3;
    default:
        break;
    }
    int32_t line_step = (fb->width - w) * 3;
    uint8_t *data = fb->buf + ((x + (y * fb->width)) * bytes_per_pixel);
    uint8_t c0 = color >> 16;
    uint8_t c1 = color >> 8;
    uint8_t c2 = color;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            switch (bytes_per_pixel)
            {
            case 1:
                data[0] = c2;
                data++;
                break;
            case 2:
                data[0] = c1;
                data[1] = c2;
                data += 2;
                break;
            case 3:
                data[0] = c0;
                data[1] = c1;
                data[2] = c2;
                data += 3;
            default:
                break;
            }
        }
        data += line_step;
    }
}

void fb_gfx_drawFastHLine(camera_fb_t *fb, int32_t x, int32_t y, int32_t w, uint32_t color)
{
    fb_gfx_fillRect(fb, x, y, w, 1, color);
}

void fb_gfx_drawFastVLine(camera_fb_t *fb, int32_t x, int32_t y, int32_t h, uint32_t color)
{
    fb_gfx_fillRect(fb, x, y, 1, h, color);
}

uint8_t fb_gfx_putc(camera_fb_t *fb, int32_t x, int32_t y, uint32_t color, unsigned char c)
{
    uint16_t line_width;
    uint8_t xa = 0, bit = 0, bits = 0, xx, yy;
    uint8_t *bitmap;
    GFXglyph *glyph;

    if ((c < 32) || (c < gfxFont->first) || (c > gfxFont->last))
    {
        return xa;
    }

    c -= gfxFont->first;

    glyph = &(gfxFont->glyph[c]);
    bitmap = gfxFont->bitmap + glyph->bitmapOffset;

    xa = glyph->xAdvance;
    x += glyph->xOffset;
    y += glyph->yOffset;
    y += gfxFont->yOffset;
    line_width = 0;

    for (yy = 0; yy < glyph->height; yy++)
    {
        for (xx = 0; xx < glyph->width; xx++)
        {
            if (bit == 0)
            {
                bits = *bitmap++;
                bit = 0x80;
            }
            if (bits & bit)
            {
                line_width++;
            }
            else if (line_width)
            {
                fb_gfx_drawFastHLine(fb, x + xx - line_width, y + yy, line_width, color);
                line_width = 0;
            }
            bit >>= 1;
        }
        if (line_width)
        {
            fb_gfx_drawFastHLine(fb, x + xx - line_width, y + yy, line_width, color);
            line_width = 0;
        }
    }
    return xa;
}

uint32_t fb_gfx_print(camera_fb_t *fb, int x, int y, uint32_t color, const char *str)
{
    uint32_t l = 0;
    int xc = x, yc = y, lc = fb->width - gfxFont->glyph[0].xAdvance;
    uint8_t fh = gfxFont->yAdvance;
    char c = *str++;
    while (c)
    {
        if (c != '\r')
        {
            if (c == '\n')
            {
                yc += fh;
                xc = x;
            }
            else
            {
                if (xc > lc)
                {
                    yc += fh;
                    xc = x;
                }
                xc += fb_gfx_putc(fb, xc, yc, color, c);
            }
        }
        l++;
        c = *str++;
    }
    return l;
}

uint32_t fb_gfx_printf(camera_fb_t *fb, int32_t x, int32_t y, uint32_t color, const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            return 0;
        }
    }
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    fb_gfx_print(fb, x, y, color, temp);
    if (len > 64)
    {
        free(temp);
    }
    return len;
}
