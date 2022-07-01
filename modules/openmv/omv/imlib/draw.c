/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Basic drawing functions.
 */
#include "font.h"
#include "imlib.h"
#include "unaligned_memcpy.h"

#ifdef IMLIB_ENABLE_DMA2D
#include STM32_HAL_H
#include "dma.h"
#endif

void* imlib_compute_row_ptr(const image_t *img, int y) {
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            return IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
        }
        case IMAGE_BPP_GRAYSCALE: {
            return IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
        }
        case IMAGE_BPP_RGB565: {
            return IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
        }
        default: {
            // This shouldn't happen, at least we return a valid memory block
            return img->data;
        }
    }
}

inline int imlib_get_pixel_fast(int img_bpp, const void *row_ptr, int x)
{
    switch(img_bpp) {
        case IMAGE_BPP_BINARY: {
            return IMAGE_GET_BINARY_PIXEL_FAST((uint32_t*)row_ptr, x);
        }
        case IMAGE_BPP_GRAYSCALE: {
            return IMAGE_GET_GRAYSCALE_PIXEL_FAST((uint8_t*)row_ptr, x);
        }
        case IMAGE_BPP_RGB565: {
            return IMAGE_GET_RGB565_PIXEL_FAST((uint16_t*)row_ptr, x);
        }
        default: {
            return -1;
        }
    }
}


// Set pixel (handles boundary check and image type check).
void imlib_set_pixel(image_t *img, int x, int y, int p)
{
    if ((0 <= x) && (x < img->w) && (0 <= y) && (y < img->h)) {
        switch(img->bpp) {
            case IMAGE_BPP_BINARY: {
                IMAGE_PUT_BINARY_PIXEL(img, x, y, p);
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                IMAGE_PUT_GRAYSCALE_PIXEL(img, x, y, p);
                break;
            }
            case IMAGE_BPP_RGB565: {
                IMAGE_PUT_RGB565_PIXEL(img, x, y, p);
                break;
            }
            default: {
                break;
            }
        }
    }
}

// https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles
static void point_fill(image_t *img, int cx, int cy, int r0, int r1, int c)
{
    for (int y = r0; y <= r1; y++) {
        for (int x = r0; x <= r1; x++) {
            if (((x * x) + (y * y)) <= (r0 * r0)) {
                imlib_set_pixel(img, cx + x, cy + y, c);
            }
        }
    }
}

// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness)
{
    if (thickness > 0) {
        int thickness0 = (thickness - 0) / 2;
        int thickness1 = (thickness - 1) / 2;
        int dx = abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
        int dy = abs(y1 - y0), sy = (y0 < y1) ? 1 : -1;
        int err = ((dx > dy) ? dx : -dy) / 2;

        for (;;) {
            point_fill(img, x0, y0, -thickness0, thickness1, c);
            if ((x0 == x1) && (y0 == y1)) break;
            int e2 = err;
            if (e2 > -dx) { err -= dy; x0 += sx; }
            if (e2 <  dy) { err += dx; y0 += sy; }
        }
    }
}

static void xLine(image_t *img, int x1, int x2, int y, int c)
{
    while (x1 <= x2) imlib_set_pixel(img, x1++, y, c);
}

static void yLine(image_t *img, int x, int y1, int y2, int c)
{
    while (y1 <= y2) imlib_set_pixel(img, x, y1++, c);
}

void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill)
{
    if (fill) {

        for (int y = ry, yy = ry + rh; y < yy; y++) {
            for (int x = rx, xx = rx + rw; x < xx; x++) {
                imlib_set_pixel(img, x, y, c);
            }
        }

    } else if (thickness > 0) {
        int thickness0 = (thickness - 0) / 2;
        int thickness1 = (thickness - 1) / 2;

        for (int i = rx - thickness0, j = rx + rw + thickness1, k = ry + rh - 1; i < j; i++) {
            yLine(img, i, ry - thickness0, ry + thickness1, c);
            yLine(img, i, k - thickness0, k + thickness1, c);
        }

        for (int i = ry - thickness0, j = ry + rh + thickness1, k = rx + rw - 1; i < j; i++) {
            xLine(img, rx - thickness0, rx + thickness1, i, c);
            xLine(img, k - thickness0, k + thickness1, i, c);
        }
    }
}

// https://stackoverflow.com/questions/27755514/circle-with-thickness-drawing-algorithm
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill)
{
    if (fill) {
        point_fill(img, cx, cy, -r, r, c);
    } else if (thickness > 0) {
        int thickness0 = (thickness - 0) / 2;
        int thickness1 = (thickness - 1) / 2;

        int xo = r + thickness0;
        int xi = IM_MAX(r - thickness1, 0);
        int xi_tmp = xi;
        int y = 0;
        int erro = 1 - xo;
        int erri = 1 - xi;

        while(xo >= y) {
            xLine(img, cx + xi, cx + xo, cy + y,  c);
            yLine(img, cx + y,  cy + xi, cy + xo, c);
            xLine(img, cx - xo, cx - xi, cy + y,  c);
            yLine(img, cx - y,  cy + xi, cy + xo, c);
            xLine(img, cx - xo, cx - xi, cy - y,  c);
            yLine(img, cx - y,  cy - xo, cy - xi, c);
            xLine(img, cx + xi, cx + xo, cy - y,  c);
            yLine(img, cx + y,  cy - xo, cy - xi, c);

            y++;

            if (erro < 0) {
                erro += 2 * y + 1;
            } else {
                xo--;
                erro += 2 * (y - xo + 1);
            }

            if (y > xi_tmp) {
                xi = y;
            } else {
                if (erri < 0) {
                    erri += 2 * y + 1;
                } else {
                    xi--;
                    erri += 2 * (y - xi + 1);
                }
            }
        }
    }
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_pixel(image_t *img, int x0, int y0, int dx, int dy, float shear_dx, float shear_dy, int r0, int r1, int c)
{
    point_fill(img, x0 + dx, y0 + dy + fast_floorf((dx * shear_dy) / shear_dx), r0, r1, c);
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_line(image_t *img, int x0, int y0, int dx, int dy0, int dy1, float shear_dx, float shear_dy, int c)
{
    int y = y0 + fast_floorf((dx * shear_dy) / shear_dx);
    yLine(img, x0 + dx, y + dy0, y + dy1, c);
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_sheared_ellipse(image_t *img, int x0, int y0, int width, int height, bool filled, float shear_dx, float shear_dy, int c, int thickness)
{
    int thickness0 = (thickness - 0) / 2;
    int thickness1 = (thickness - 1) / 2;
    if (((thickness > 0) || filled) && (shear_dx != 0)) {
        int a_squared = width * width;
        int four_a_squared = a_squared * 4;
        int b_squared = height * height;
        int four_b_squared = b_squared * 4;

        int x = 0;
        int y = height;
        int sigma = (2 * b_squared) + (a_squared * (1 - (2 * height)));

        while ((b_squared * x) <= (a_squared * y)) {
            if (filled) {
                scratch_draw_line(img, x0, y0, x, -y, y, shear_dx, shear_dy, c);
                scratch_draw_line(img, x0, y0, -x, -y, y, shear_dx, shear_dy, c);
            } else {
                scratch_draw_pixel(img, x0, y0, x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
            }

            if (sigma >= 0) {
                sigma += four_a_squared * (1 - y);
                y -= 1;
            }

            sigma += b_squared * ((4 * x) + 6);
            x += 1;
        }

        x = width;
        y = 0;
        sigma = (2 * a_squared) + (b_squared * (1 - (2 * width)));

        while ((a_squared * y) <= (b_squared * x)) {
            if (filled) {
                scratch_draw_line(img, x0, y0, x, -y, y, shear_dx, shear_dy, c);
                scratch_draw_line(img, x0, y0, -x, -y, y, shear_dx, shear_dy, c);
            } else {
                scratch_draw_pixel(img, x0, y0, x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
            }

            if (sigma >= 0) {
                sigma += four_b_squared * (1 - x);
                x -= 1;
            }

            sigma += a_squared * ((4 * y) + 6);
            y += 1;
        }
    }
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_rotated_ellipse(image_t *img, int x, int y, int x_axis, int y_axis, int rotation, bool filled, int c, int thickness)
{
    if ((x_axis > 0) && (y_axis > 0)) {
        if ((x_axis == y_axis) || (rotation == 0)) {
            scratch_draw_sheared_ellipse(img, x, y, x_axis / 2, y_axis / 2, filled, 1, 0, c, thickness);
        } else if (rotation == 90) {
            scratch_draw_sheared_ellipse(img, x, y, y_axis / 2, x_axis / 2, filled, 1, 0, c, thickness);
        } else {

            // Avoid rotations above 90.
            if (rotation > 90) {
                rotation -= 90;
                int temp = x_axis;
                x_axis = y_axis;
                y_axis = temp;
            }

            // Avoid rotations above 45.
            if (rotation > 45) {
                rotation -= 90;
                int temp = x_axis;
                x_axis = y_axis;
                y_axis = temp;
            }

            float theta = fast_atanf(IM_DIV(y_axis, x_axis) * (-tanf(IM_DEG2RAD(rotation))));
            float shear_dx = (x_axis * cosf(theta) * cosf(IM_DEG2RAD(rotation))) - (y_axis * sinf(theta) * sinf(IM_DEG2RAD(rotation)));
            float shear_dy = (x_axis * cosf(theta) * sinf(IM_DEG2RAD(rotation))) + (y_axis * sinf(theta) * cosf(IM_DEG2RAD(rotation)));
            float shear_x_axis = fast_fabsf(shear_dx);
            float shear_y_axis = IM_DIV((y_axis * x_axis), shear_x_axis);
            scratch_draw_sheared_ellipse(img, x, y, fast_floorf(shear_x_axis / 2), fast_floorf(shear_y_axis / 2), filled, shear_dx, shear_dy, c, thickness);
        }
    }
}

void imlib_draw_ellipse(image_t *img, int cx, int cy, int rx, int ry, int rotation, int c, int thickness, bool fill)
{
    int r = rotation % 180;
    if (r < 0) r += 180;

    scratch_draw_rotated_ellipse(img, cx, cy, rx * 2, ry * 2, r, fill, c, thickness);
}

// char rotation == 0, 90, 180, 360, etc.
// string rotation == 0, 90, 180, 360, etc.
void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
                       int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_vflip)
{
    char_rotation %= 360;
    if (char_rotation < 0) char_rotation += 360;
    char_rotation = (char_rotation / 90) * 90;

    string_rotation %= 360;
    if (string_rotation < 0) string_rotation += 360;
    string_rotation = (string_rotation / 90) * 90;

    bool char_swap_w_h = (char_rotation == 90) || (char_rotation == 270);
    bool char_upsidedown = (char_rotation == 180) || (char_rotation == 270);

    if (string_hmirror) x_off -= fast_floorf(font[0].w * scale) - 1;
    if (string_vflip) y_off -= fast_floorf(font[0].h * scale) - 1;

    int org_x_off = x_off;
    int org_y_off = y_off;
    const int anchor = x_off;

    for(char ch, last = '\0'; (ch = *str); str++, last = ch) {

        if ((last == '\r') && (ch == '\n')) { // handle "\r\n" strings
            continue;
        }

        if ((ch == '\n') || (ch == '\r')) { // handle '\n' or '\r' strings
            x_off = anchor;
            y_off += (string_vflip ? -1 : +1) * (fast_floorf((char_swap_w_h ? font[0].w : font[0].h) * scale) + y_spacing); // newline height == space height
            continue;
        }

        if ((ch < ' ') || (ch > '~')) { // handle unknown characters
            continue;
        }

        const glyph_t *g = &font[ch - ' '];

        if (!mono_space) {
            // Find the first pixel set and offset to that.
            bool exit = false;

            if (!char_swap_w_h) {
                for (int x = 0, xx = g->w; x < xx; x++) {
                    for (int y = 0, yy = g->h; y < yy; y++) {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
                            (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
                            x_off += (string_hmirror ? +1 : -1) * fast_floorf(x * scale);
                            exit = true;
                            break;
                        }
                    }

                    if (exit) break;
                }
            } else {
                for (int y = g->h - 1; y >= 0; y--) {
                    for (int x = 0, xx = g->w; x < xx; x++) {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
                            (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
                            x_off += (string_hmirror ? +1 : -1) * fast_floorf((g->h - 1 - y) * scale);
                            exit = true;
                            break;
                        }
                    }

                    if (exit) break;
                }
            }
        }

        for (int y = 0, yy = fast_floorf(g->h * scale); y < yy; y++) {
            for (int x = 0, xx = fast_floorf(g->w * scale); x < xx; x++) {
                if (g->data[fast_floorf(y / scale)] & (1 << (g->w - 1 - fast_floorf(x / scale)))) {
                    int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x), y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
                    point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp, &y_tmp);
                    point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp, &y_tmp);
                    imlib_set_pixel(img, x_tmp, y_tmp, c);
                }
            }
        }

        if (mono_space) {
            x_off += (string_hmirror ? -1 : +1) * (fast_floorf((char_swap_w_h ? g->h : g->w) * scale) + x_spacing);
        } else {
            // Find the last pixel set and offset to that.
            bool exit = false;

            if (!char_swap_w_h) {
                for (int x = g->w - 1; x >= 0; x--) {
                    for (int y = g->h - 1; y >= 0; y--) {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
                            (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
                            x_off += (string_hmirror ? -1 : +1) * (fast_floorf((x + 2) * scale) + x_spacing);
                            exit = true;
                            break;
                        }
                    }

                    if (exit) break;
                }
            } else {
                for (int y = 0, yy = g->h; y < yy; y++) {
                    for (int x = g->w - 1; x >= 0; x--) {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
                            (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
                            x_off += (string_hmirror ? -1 : +1) * (fast_floorf(((g->h - 1 - y) + 2) * scale) + x_spacing);
                            exit = true;
                            break;
                        }
                    }

                    if (exit) break;
                }
            }

            if (!exit) x_off += (string_hmirror ? -1 : +1) * fast_floorf(scale * 3); // space char
        }
    }
}

void imlib_draw_row_setup(imlib_draw_row_data_t *data)
{
    image_t temp;
    temp.w = data->dst_img->w;
    temp.h = data->dst_img->h;
    temp.bpp = data->src_img_bpp;

    // Image Row Size should be the width of the destination image
    // but with the bpp of the source image.
    size_t image_row_size = image_size(&temp) / data->dst_img->h;

    data->toggle = 0;
    data->row_buffer[0] = fb_alloc(image_row_size, FB_ALLOC_NO_HINT);

#ifdef IMLIB_ENABLE_DMA2D
    data->dma2d_enabled = false;
    data->dma2d_initialized = false;

    void *dst_buff = data->dst_row_override ? data->dst_row_override : data->dst_img->data;

    if (data->dma2d_request && (data->dst_img->bpp == IMAGE_BPP_RGB565) && DMA_BUFFER(dst_buff) &&
        ((data->src_img_bpp == IMAGE_BPP_GRAYSCALE) ||
        ((data->src_img_bpp == IMAGE_BPP_RGB565) && (data->rgb_channel < 0) && (data->alpha != 256) && (!data->color_palette) && (!data->alpha_palette)))) {
        data->row_buffer[1] = fb_alloc(image_row_size, FB_ALLOC_NO_HINT);
        data->dma2d_enabled = true;
        data->dma2d_initialized = true;

        memset(&data->dma2d, 0, sizeof(data->dma2d));

        data->dma2d.Instance = DMA2D;
        data->dma2d.Init.Mode = DMA2D_M2M;
        if (data->dst_img->bpp != data->src_img_bpp) data->dma2d.Init.Mode = DMA2D_M2M_PFC;
        if ((data->alpha != 256) || data->alpha_palette) data->dma2d.Init.Mode = DMA2D_M2M_BLEND;
        data->dma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
        data->dma2d.Init.OutputOffset = 0;
        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
        data->dma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;
        data->dma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;
        #endif
        HAL_DMA2D_Init(&data->dma2d);

        data->dma2d.LayerCfg[0].InputOffset = 0;
        data->dma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
        data->dma2d.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
        data->dma2d.LayerCfg[0].InputAlpha = data->black_background ? 0x00 : 0xff;
        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
        data->dma2d.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;
        data->dma2d.LayerCfg[0].RedBlueSwap = DMA2D_RB_REGULAR;
        #endif
        #if defined(MCU_SERIES_H7)
        data->dma2d.LayerCfg[0].ChromaSubSampling = DMA2D_NO_CSS;
        #endif
        HAL_DMA2D_ConfigLayer(&data->dma2d, 0);

        switch (data->src_img_bpp) {
            case IMAGE_BPP_GRAYSCALE: {
                data->dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_L8;
                data->dma2d.LayerCfg[1].AlphaMode = DMA2D_COMBINE_ALPHA;
                uint32_t *clut = fb_alloc(256 * sizeof(uint32_t), FB_ALLOC_NO_HINT);

                if (!data->alpha_palette) {
                    if (!data->color_palette) {
                        for (int i = 0; i < 256; i++) {
                            clut[i] = (0xff << 24) | COLOR_Y_TO_RGB888(i);
                        }
                    } else {
                        for (int i = 0; i < 256; i++) {
                            int pixel = data->color_palette[i];
                            clut[i] = (0xff << 24) | (COLOR_RGB565_TO_R8(pixel) << 16) | (COLOR_RGB565_TO_G8(pixel) << 8) | COLOR_RGB565_TO_B8(pixel);
                        }
                    }
                } else {
                    if (!data->color_palette) {
                        for (int i = 0; i < 256; i++) {
                            clut[i] = (data->alpha_palette[i] << 24) | COLOR_Y_TO_RGB888(i);
                        }
                    } else {
                        for (int i = 0; i < 256; i++) {
                            int pixel = data->color_palette[i];
                            clut[i] = (data->alpha_palette[i] << 24) | (COLOR_RGB565_TO_R8(pixel) << 16) | (COLOR_RGB565_TO_G8(pixel) << 8) | COLOR_RGB565_TO_B8(pixel);
                        }
                    }
                }

                DMA2D_CLUTCfgTypeDef cfg;
                cfg.pCLUT = clut;
                cfg.CLUTColorMode = DMA2D_CCM_ARGB8888;
                cfg.Size = 255;
                #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                SCB_CleanDCache_by_Addr(clut, 256 * sizeof(uint32_t));
                #endif
                HAL_DMA2D_CLUTLoad(&data->dma2d, cfg, 1);
                HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
                break;
            }
            case IMAGE_BPP_RGB565: {
                data->dma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
                data->dma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
                break;
            }
            default: {
                break;
            }
        }

        data->dma2d.LayerCfg[1].InputOffset = 0;
        data->dma2d.LayerCfg[1].InputAlpha = fast_roundf((data->alpha * 255) / 256.f);
        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
        data->dma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
        data->dma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
        #endif
        #if defined(MCU_SERIES_H7)
        data->dma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
        #endif
        HAL_DMA2D_ConfigLayer(&data->dma2d, 1);
    } else {
        data->row_buffer[1] = data->row_buffer[0];
    }
#else
    data->row_buffer[1] = data->row_buffer[0];
#endif

    int alpha = data->alpha, max = 256;

    if (data->dst_img->bpp == IMAGE_BPP_RGB565) {
        alpha >>= 3; // 5-bit alpha for RGB565
        max = 32;
    }

    // Set smuad_alpha and smuad_alpha_palette even if we don't use them with DMA2D as we may have
    // to fallback to using them if the draw_image calls imlib_draw_row_put_row_buffer().
    data->smuad_alpha = data->black_background ? alpha : ((alpha << 16) | (max - alpha));

    if (data->alpha_palette) {
        data->smuad_alpha_palette = fb_alloc(256 * sizeof(uint32_t), FB_ALLOC_NO_HINT);

        for (int i = 0, a = alpha; i < 256; i++) {
            int new_alpha = fast_roundf((a * data->alpha_palette[i]) / 255.f);
            data->smuad_alpha_palette[i] = data->black_background ? new_alpha : ((new_alpha << 16) | (max - new_alpha));
        }
    } else {
        data->smuad_alpha_palette = NULL;
    }
}

void imlib_draw_row_teardown(imlib_draw_row_data_t *data)
{
    if (data->smuad_alpha_palette) fb_free();
#ifdef IMLIB_ENABLE_DMA2D
    if (data->dma2d_initialized) {
        if (!data->callback) HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
        HAL_DMA2D_DeInit(&data->dma2d);
        if (data->src_img_bpp == IMAGE_BPP_GRAYSCALE) fb_free(); // clut...
        fb_free(); // data->row_buffer[1]
    }
#endif
    fb_free(); // data->row_buffer[0]
}

#ifdef IMLIB_ENABLE_DMA2D
void imlib_draw_row_deinit_all()
{
    DMA2D_HandleTypeDef dma2d = {};
    dma2d.Instance = DMA2D;
    HAL_DMA2D_DeInit(&dma2d);
}
#endif

void *imlib_draw_row_get_row_buffer(imlib_draw_row_data_t *data)
{
    void *result = data->row_buffer[data->toggle];
    data->toggle = !data->toggle;
    return result;
}

void imlib_draw_row_put_row_buffer(imlib_draw_row_data_t *data, void *row_buffer)
{
    data->row_buffer[data->toggle] = row_buffer;
    data->toggle = !data->toggle;
#ifdef IMLIB_ENABLE_DMA2D
    if (data->dma2d_enabled && (!DMA_BUFFER(row_buffer))) {
        data->dma2d_enabled = false;
    }
#endif
}

// Draws (x_end - x_start) pixels.
// src width must be equal to dst width.
void imlib_draw_row(int x_start, int x_end, int y_row, imlib_draw_row_data_t *data)
{
    #define BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha) \
    ({ \
        __typeof__ (src_pixel) _src_pixel = (src_pixel); \
        __typeof__ (dst_pixel) _dst_pixel = (dst_pixel); \
        __typeof__ (smuad_alpha) _smuad_alpha = (smuad_alpha); \
        const long mask_r = 0x7c007c00, mask_g = 0x07e007e0, mask_b = 0x001f001f; \
        uint32_t rgb = (_src_pixel << 16) | _dst_pixel; \
        long rb = ((rgb >> 1) & mask_r) | (rgb & mask_b); \
        long g = rgb & mask_g; \
        int rb_out = __SMUAD(_smuad_alpha, rb) >> 5; \
        int g_out = __SMUAD(_smuad_alpha, g) >> 5; \
        ((rb_out << 1) & 0xf800) | (g_out & 0x07e0) | (rb_out & 0x001f); \
    })

    #define BLEND_RGB566_0(src_pixel, smuad_alpha) \
    ({ \
        __typeof__ (src_pixel) _src_pixel = (src_pixel); \
        __typeof__ (smuad_alpha) _smuad_alpha = (smuad_alpha); \
        int rb_out = ((_src_pixel & 0xf81f) * _smuad_alpha) >> 5; \
        int g_out = ((_src_pixel & 0x7e0) * _smuad_alpha) >> 5; \
        (rb_out & 0xf81f) | (g_out & 0x7e0); \
    })

    #define COLOR_GRAYSCALE_BINARY_MIN_LSL16 (COLOR_GRAYSCALE_BINARY_MIN << 16)
    #define COLOR_GRAYSCALE_BINARY_MAX_LSL16 (COLOR_GRAYSCALE_BINARY_MAX << 16)

    switch (data->dst_img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *dst32 = data->dst_row_override ? ((uint32_t *) data->dst_row_override) : IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(data->dst_img, y_row);
            switch (data->src_img_bpp) {
                case IMAGE_BPP_BINARY: {
                    uint32_t *src32 = (uint32_t *) data->row_buffer[!data->toggle];
                    if (data->smuad_alpha_palette) {
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = (pixel ? COLOR_GRAYSCALE_BINARY_MAX_LSL16 : COLOR_GRAYSCALE_BINARY_MIN_LSL16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = pixel ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN;
                                    pixel = ((smuad_alpha * smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            uint32_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0) << 16;
                            pal255 = COLOR_RGB565_TO_Y(pal255) << 16;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = (pixel ? pal255 : pal0) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = pixel ? pal255 : pal0;
                                    pixel = ((smuad_alpha * smuad_pixel) >> 24) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            for (int x = x_start; x < x_end; x++) {
                                int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0) > 127;
                            pal255 = COLOR_RGB565_TO_Y(pal255) > 127;
                            switch ((pal0 << 1) | (pal255 << 0)) {
                                case 0: {
                                    for (int x = x_start; x < x_end; x++) {
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, 0);
                                    }
                                    break;
                                }
                                case 1: {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                    break;
                                }
                                case 2: {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = !IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                    break;
                                }
                                case 3: {
                                    for (int x = x_start; x < x_end; x++) {
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, 1);
                                    }
                                    break;
                                }
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_GRAYSCALE_BINARY_MAX_LSL16 : COLOR_GRAYSCALE_BINARY_MIN_LSL16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    int pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN;
                                    int pixel = ((smuad_alpha * smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint32_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0) << 16;
                            pal255 = COLOR_RGB565_TO_Y(pal255) << 16;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    int pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                                    int pixel = ((smuad_alpha * smuad_pixel) >> 24) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        }
                        // More desirable results are produced by alpha blending with 8-bits.
                        // if (!data->color_palette) {
                        //     for (int x = x_start; x < x_end; x++) {
                        //         int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) & IMAGE_GET_BINARY_PIXEL_FAST(dst32, x);
                        //         IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                        //     }
                        // } else {
                        //     const uint16_t *color_palette = data->color_palette;
                        //     uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                        //     pal0 = COLOR_RGB565_TO_Y(pal0) > 127;
                        //     pal255 = COLOR_RGB565_TO_Y(pal255) > 127;
                        //     switch ((pal0 << 1) | (pal255 << 0)) {
                        //         case 0: {
                        //             for (int x = x_start; x < x_end; x++) {
                        //                 IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, 0);
                        //             }
                        //             break;
                        //         }
                        //         case 1: {
                        //             for (int x = x_start; x < x_end; x++) {
                        //                 int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) & IMAGE_GET_BINARY_PIXEL_FAST(dst32, x);
                        //                 IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                        //             }
                        //             break;
                        //         }
                        //         case 2: {
                        //             for (int x = x_start; x < x_end; x++) {
                        //                 int pixel = !(IMAGE_GET_BINARY_PIXEL_FAST(src32, x) | IMAGE_GET_BINARY_PIXEL_FAST(dst32, x));
                        //                 IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                        //             }
                        //             break;
                        //         }
                        //         case 3: {
                        //             for (int x = x_start; x < x_end; x++) {
                        //                 IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, 1);
                        //             }
                        //             break;
                        //         }
                        //     }
                        // }
                    }
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    uint8_t *src8 = ((uint8_t *) data->row_buffer[!data->toggle]) + x_start;
                    if (data->smuad_alpha_palette) {
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    pixel = color_palette[pixel];
                                    pixel = COLOR_RGB565_TO_Y(pixel);
                                    long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    pixel = color_palette[pixel];
                                    pixel = COLOR_RGB565_TO_Y(pixel);
                                    pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            for (int x = x_start; x < x_end; x++) {
                                int pixel = *src8++ > 127;
                                IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            for (int x = x_start; x < x_end; x++) {
                                int pixel = color_palette[*src8++];
                                pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (*src8++ << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    int pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = ((smuad_alpha * (*src8++)) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = color_palette[*src8++];
                                    pixel = COLOR_RGB565_TO_Y(pixel);
                                    long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                    pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = color_palette[*src8++];
                                    pixel = COLOR_RGB565_TO_Y(pixel);
                                    pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    uint16_t *src16 = ((uint16_t *) data->row_buffer[!data->toggle]) + x_start;
                    if (data->rgb_channel < 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                    pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_R8(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                    pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_R8(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_R8(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 1) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_G8(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                    pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_G8(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_G8(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 2) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        long smuad_pixel = (pixel << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel];
                                        pixel = ((smuad_alpha * pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_B8(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                    pixel = COLOR_RGB565_TO_Y(pixel) > 127;
                                    IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_B8(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_B8(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | (IMAGE_GET_BINARY_PIXEL_FAST(dst32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN);
                                        pixel = (__SMUAD(smuad_alpha, smuad_pixel) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                        pixel = ((smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8) > 127;
                                        IMAGE_PUT_BINARY_PIXEL_FAST(dst32, x, pixel);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *dst8 = (data->dst_row_override ? ((uint8_t *) data->dst_row_override) : IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(data->dst_img, y_row)) + x_start;
            switch (data->src_img_bpp) {
                case IMAGE_BPP_BINARY: {
                    uint32_t *src32 = (uint32_t *) data->row_buffer[!data->toggle];
                    if (data->smuad_alpha_palette) {
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = (pixel ? COLOR_GRAYSCALE_BINARY_MAX_LSL16 : COLOR_GRAYSCALE_BINARY_MIN_LSL16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = pixel ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN;
                                    *dst8++ = (smuad_alpha * smuad_pixel) >> 8;
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            uint32_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0) << 16;
                            pal255 = COLOR_RGB565_TO_Y(pal255) << 16;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = (pixel ? pal255 : pal0) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    long smuad_pixel = pixel ? pal255 : pal0;
                                    *dst8++ = (smuad_alpha * smuad_pixel) >> 24;
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            for (int x = x_start; x < x_end; x++) {
                                *dst8++ = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN;
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0);
                            pal255 = COLOR_RGB565_TO_Y(pal255);
                            for (int x = x_start; x < x_end; x++) {
                                *dst8++ = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_GRAYSCALE_BINARY_MAX_LSL16 : COLOR_GRAYSCALE_BINARY_MIN_LSL16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_GRAYSCALE_BINARY_MAX : COLOR_GRAYSCALE_BINARY_MIN;
                                    *dst8++ = (smuad_alpha * smuad_pixel) >> 8;
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint32_t pal0 = color_palette[0], pal255 = color_palette[255];
                            pal0 = COLOR_RGB565_TO_Y(pal0) << 16;
                            pal255 = COLOR_RGB565_TO_Y(pal255) << 16;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                                    *dst8++ = (smuad_alpha * smuad_pixel) >> 24;
                                }
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    uint8_t *src8 = ((uint8_t *) data->row_buffer[!data->toggle]) + x_start;
                    if (data->smuad_alpha_palette) {
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    long smuad_pixel = (pixel << 16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    *dst8++ = (smuad_alpha * pixel) >> 8;
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    pixel = color_palette[pixel];
                                    long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[pixel];
                                    pixel = color_palette[pixel];
                                    *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            unaligned_memcpy(dst8 , src8, (x_end - x_start) * sizeof(uint8_t));
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            for (int x = x_start; x < x_end; x++) {
                                int pixel = color_palette[*src8++];
                                *dst8++ = COLOR_RGB565_TO_Y(pixel);
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    long smuad_pixel = (*src8++ << 16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    *dst8++ = (smuad_alpha * (*src8++)) >> 8;
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = color_palette[*src8++];
                                    long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                    *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = color_palette[*src8++];
                                    *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                }
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    uint16_t *src16 = ((uint16_t *) data->row_buffer[!data->toggle]) + x_start;
                    if (data->rgb_channel < 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        long smuad_pixel = (pixel_y << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * pixel_y) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_Y(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst8++ = COLOR_RGB565_TO_Y(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                    *dst8++ = COLOR_RGB565_TO_Y(pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        long smuad_pixel = (pixel_y << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * pixel_y) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_R8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst8++ = COLOR_RGB565_TO_R8(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                    *dst8++ = COLOR_RGB565_TO_Y(pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_R8(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_R8(pixel)) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 1) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        long smuad_pixel = (pixel_y << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * pixel_y) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_G8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst8++ = COLOR_RGB565_TO_G8(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                    *dst8++ = COLOR_RGB565_TO_Y(pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_G8(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_G8(pixel)) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 2) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        long smuad_pixel = (pixel_y << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * pixel_y) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        int pixel_y = COLOR_RGB565_TO_B8(pixel);
                                        long smuad_alpha = smuad_alpha_palette[pixel_y];
                                        pixel = color_palette[pixel_y];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst8++ = COLOR_RGB565_TO_B8(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                    *dst8++ = COLOR_RGB565_TO_Y(pixel);
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        long smuad_pixel = (COLOR_RGB565_TO_B8(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_B8(pixel)) >> 8;
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                        long smuad_pixel = (COLOR_RGB565_TO_Y(pixel) << 16) | *dst8;
                                        *dst8++ = __SMUAD(smuad_alpha, smuad_pixel) >> 8;
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int pixel = *src16++;
                                        pixel = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                        *dst8++ = (smuad_alpha * COLOR_RGB565_TO_Y(pixel)) >> 8;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *dst16 = (data->dst_row_override ? ((uint16_t *) data->dst_row_override) : IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(data->dst_img, y_row)) + x_start;
            switch (data->src_img_bpp) {
                case IMAGE_BPP_BINARY: {
                    uint32_t *src32 = (uint32_t *) data->row_buffer[!data->toggle];
                    if (data->smuad_alpha_palette) {
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    int src_pixel = pixel ? COLOR_RGB565_BINARY_MAX : COLOR_RGB565_BINARY_MIN;
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    int src_pixel = pixel ? COLOR_RGB565_BINARY_MAX : COLOR_RGB565_BINARY_MIN;
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint32_t alpha_pal0 = smuad_alpha_palette[0], alpha_pal255 = smuad_alpha_palette[255];
                            uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    int src_pixel = pixel ? pal255 : pal0;
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x);
                                    long smuad_alpha = pixel ? alpha_pal255 : alpha_pal0;
                                    int src_pixel = pixel ? pal255 : pal0;
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            for (int x = x_start; x < x_end; x++) {
                                *dst16++ = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_RGB565_BINARY_MAX : COLOR_RGB565_BINARY_MIN;
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                            for (int x = x_start; x < x_end; x++) {
                                *dst16++ = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_RGB565_BINARY_MAX : COLOR_RGB565_BINARY_MIN;
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? COLOR_RGB565_BINARY_MAX : COLOR_RGB565_BINARY_MIN;
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            uint16_t pal0 = color_palette[0], pal255 = color_palette[255];
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = IMAGE_GET_BINARY_PIXEL_FAST(src32, x) ? pal255 : pal0;
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    uint8_t *src8 = ((uint8_t *) data->row_buffer[!data->toggle]) + x_start;
#ifdef IMLIB_ENABLE_DMA2D
                    if (data->dma2d_enabled) {
                        if (!data->callback) HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
                        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                        SCB_CleanDCache_by_Addr((uint32_t *) src8, (x_end - x_start) * sizeof(uint8_t));
                        SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dst16, (x_end - x_start) * sizeof(uint16_t));
                        #endif
                        HAL_DMA2D_BlendingStart(&data->dma2d, (uint32_t) src8, (uint32_t) dst16, (uint32_t) dst16, x_end - x_start, 1);
                        if (data->callback) HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
                    } else if (data->smuad_alpha_palette) {
#else
                    if (data->smuad_alpha_palette) {
#endif
                        const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[src_pixel];
                                    src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[src_pixel];
                                    src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[src_pixel];
                                    src_pixel = color_palette[src_pixel];
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    long smuad_alpha = smuad_alpha_palette[src_pixel];
                                    src_pixel = color_palette[src_pixel];
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        }
                    } else if (data->alpha == 256) {
                        if (!data->color_palette) {
                            for (int x = x_start; x < x_end; x++) {
                                int pixel = *src8++;
                                *dst16++ = COLOR_Y_TO_RGB565(pixel);
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            for (int x = x_start; x < x_end; x++) {
                                *dst16++ = color_palette[*src8++];
                            }
                        }
                    } else {
                        long smuad_alpha = data->smuad_alpha;
                        if (!data->color_palette) {
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = *src8++;
                                    src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        } else {
                            const uint16_t *color_palette = data->color_palette;
                            if (!data->black_background) {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = color_palette[*src8++];
                                    int dst_pixel = *dst16;
                                    *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                }
                            } else {
                                for (int x = x_start; x < x_end; x++) {
                                    int src_pixel = color_palette[*src8++];
                                    *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                }
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    uint16_t *src16 = ((uint16_t *) data->row_buffer[!data->toggle]) + x_start;
                    if (data->rgb_channel < 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        long smuad_alpha = smuad_alpha_palette[COLOR_RGB565_TO_Y(src_pixel)];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        long smuad_alpha = smuad_alpha_palette[COLOR_RGB565_TO_Y(src_pixel)];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_Y(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_Y(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                unaligned_memcpy(dst16, src16, (x_end - x_start) * sizeof(uint16_t));
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst16++ = color_palette[COLOR_RGB565_TO_Y(pixel)];
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
#ifdef IMLIB_ENABLE_DMA2D
                                if (data->dma2d_enabled) {
                                    if (!data->callback) HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
                                    #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                    SCB_CleanDCache_by_Addr((uint32_t *) src16, (x_end - x_start) * sizeof(uint16_t));
                                    SCB_CleanInvalidateDCache_by_Addr((uint32_t *) dst16, (x_end - x_start) * sizeof(uint16_t));
                                    #endif
                                    HAL_DMA2D_BlendingStart(&data->dma2d, (uint32_t) src16, (uint32_t) dst16, (uint32_t) dst16, x_end - x_start, 1);
                                    if (data->callback) HAL_DMA2D_PollForTransfer(&data->dma2d, 1000);
                                } else if (!data->black_background) {
#else
                                if (!data->black_background) {
#endif
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_Y(src_pixel)];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_Y(src_pixel)];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 0) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_R8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_R8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_R8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_R8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_R8(pixel);
                                    *dst16++ = COLOR_Y_TO_RGB565(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst16++ = color_palette[COLOR_RGB565_TO_R8(pixel)];
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_R8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_R8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_R8(src_pixel)];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_R8(src_pixel)];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 1) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_G8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_G8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_G8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_G8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_G8(pixel);
                                    *dst16++ = COLOR_Y_TO_RGB565(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst16++ = color_palette[COLOR_RGB565_TO_G8(pixel)];
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_G8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_G8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_G8(src_pixel)];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_G8(src_pixel)];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        }
                    } else if (data->rgb_channel == 2) {
                        if (data->smuad_alpha_palette) {
                            const uint32_t *smuad_alpha_palette = data->smuad_alpha_palette;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_B8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_B8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel_y);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_B8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        int src_pixel_y = COLOR_RGB565_TO_B8(src_pixel);
                                        long smuad_alpha = smuad_alpha_palette[src_pixel_y];
                                        src_pixel = color_palette[src_pixel_y];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        } else if (data->alpha == 256) {
                            if (!data->color_palette) {
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    pixel = COLOR_RGB565_TO_B8(pixel);
                                    *dst16++ = COLOR_Y_TO_RGB565(pixel);
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                for (int x = x_start; x < x_end; x++) {
                                    int pixel = *src16++;
                                    *dst16++ = color_palette[COLOR_RGB565_TO_B8(pixel)];
                                }
                            }
                        } else {
                            long smuad_alpha = data->smuad_alpha;
                            if (!data->color_palette) {
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_B8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = COLOR_RGB565_TO_B8(src_pixel);
                                        src_pixel = COLOR_Y_TO_RGB565(src_pixel);
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            } else {
                                const uint16_t *color_palette = data->color_palette;
                                if (!data->black_background) {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_B8(src_pixel)];
                                        int dst_pixel = *dst16;
                                        *dst16++ = BLEND_RGB566(src_pixel, dst_pixel, smuad_alpha);
                                    }
                                } else {
                                    for (int x = x_start; x < x_end; x++) {
                                        int src_pixel = *src16++;
                                        src_pixel = color_palette[COLOR_RGB565_TO_B8(src_pixel)];
                                        *dst16++ = BLEND_RGB566_0(src_pixel, smuad_alpha);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        // Only bayer copying/cropping is supported.
        case IMAGE_BPP_BAYER: {
            uint8_t *dst8 = (data->dst_row_override
                ? ((uint8_t *) data->dst_row_override)
                : IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(data->dst_img, y_row)) + x_start;
            uint8_t *src8 = ((uint8_t *) data->row_buffer[!data->toggle]) + x_start;
            unaligned_memcpy(dst8, src8, (x_end - x_start) * sizeof(uint8_t));
            break;
        }
        default: {
            break;
        }
    }

    if (data->callback) {
        ((imlib_draw_row_callback_t) data->callback)(x_start, x_end, y_row, data);
    }

    #undef COLOR_GRAYSCALE_BINARY_MIN_LSL16
    #undef COLOR_GRAYSCALE_BINARY_MAX_LSL16
    #undef BLEND_RGB566_0
    #undef BLEND_RGB566
}

// False == Image is black, True == rect valid
bool imlib_draw_image_rectangle(image_t *dst_img, image_t *src_img, int dst_x_start, int dst_y_start, float x_scale, float y_scale, rectangle_t *roi,
                                int alpha, const uint8_t *alpha_palette, image_hint_t hint,
                                int *x0, int *x1, int *y0, int *y1)
{
    if (!alpha) {
        return false;
    }

    if (alpha_palette) {
        int i = 0;
        while ((i < 256) && (!alpha_palette[i])) i++;
        if (i == 256) { // zero alpha palette
            return false;
        }
    }

    int src_width_scaled = fast_floorf(fast_fabsf(x_scale) * (roi ? roi->w : src_img->w));
    int src_height_scaled = fast_floorf(fast_fabsf(y_scale) * (roi ? roi->h : src_img->h));

    // Center src if hint is set.
    if (hint & IMAGE_HINT_CENTER) {
        dst_x_start -= src_width_scaled / 2;
        dst_y_start -= src_height_scaled / 2;
    }

    // Clamp start x to image bounds.
    int src_x_start = 0;
    if (dst_x_start < 0) {
        src_x_start -= dst_x_start; // this is an add because dst_x_start is negative
        dst_x_start = 0;
    }

    if (dst_x_start >= dst_img->w) {
        return false;
    }

    int src_x_dst_width = src_width_scaled - src_x_start;

    if (src_x_dst_width <= 0) {
        return false;
    }

    // Clamp start y to image bounds.
    int src_y_start = 0;
    if (dst_y_start < 0) {
        src_y_start -= dst_y_start; // this is an add because dst_y_start is negative
        dst_y_start = 0;
    }

    if (dst_y_start >= dst_img->h) {
        return false;
    }

    int src_y_dst_height = src_height_scaled - src_y_start;

    if (src_y_dst_height <= 0) {
        return false;
    }

    // Clamp end x to image bounds.
    int dst_x_end = dst_x_start + src_x_dst_width;
    if (dst_x_end > dst_img->w) dst_x_end = dst_img->w;

    // Clamp end y to image bounds.
    int dst_y_end = dst_y_start + src_y_dst_height;
    if (dst_y_end > dst_img->h) dst_y_end = dst_img->h;

    *x0 = dst_x_start;
    *x1 = dst_x_end;
    *y0 = dst_y_start;
    *y1 = dst_y_end;

    return true;
}

void imlib_draw_image(image_t *dst_img, image_t *src_img, int dst_x_start, int dst_y_start, float x_scale, float y_scale, rectangle_t *roi,
                      int rgb_channel, int alpha, const uint16_t *color_palette, const uint8_t *alpha_palette, image_hint_t hint,
                      imlib_draw_row_callback_t callback, void *dst_row_override)
{
    int dst_delta_x = 1; // positive direction
    if (x_scale < 0.f) { // flip X
        dst_delta_x = -1;
        x_scale = -x_scale;
    }

    int dst_delta_y = 1; // positive direction
    if (y_scale < 0.f) { // flip Y
        dst_delta_y = -1;
        y_scale = -y_scale;
    }

    int src_img_w = roi ? roi->w : src_img->w, w_limit = src_img_w - 1, w_limit_m_1 = w_limit - 1;
    int src_img_h = roi ? roi->h : src_img->h, h_limit = src_img_h - 1, h_limit_m_1 = h_limit - 1;

    int src_width_scaled = fast_floorf(x_scale * src_img_w);
    int src_height_scaled = fast_floorf(y_scale * src_img_h);

    // Nothing to draw
    if ((src_width_scaled < 1) || (src_height_scaled < 1)) return;

    // If alpha is 0 then nothing changes.
    if (alpha == 0) return;

    if (alpha_palette) {
        int i = 0;
        while ((i < 256) && (!alpha_palette[i])) i++;
        if (i == 256) return; // zero alpha palette
    }

    // Center src if hint is set.
    if (hint & IMAGE_HINT_CENTER) {
        dst_x_start -= src_width_scaled / 2;
        dst_y_start -= src_height_scaled / 2;
    }

    // Clamp start x to image bounds.
    int src_x_start = 0;
    if (dst_x_start < 0) {
        src_x_start -= dst_x_start; // this is an add becasue dst_x_start is negative
        dst_x_start = 0;
    }

    if (dst_x_start >= dst_img->w) return;
    int src_x_dst_width = src_width_scaled - src_x_start;
    if (src_x_dst_width <= 0) return;

    // Clamp start y to image bounds.
    int src_y_start = 0;
    if (dst_y_start < 0) {
        src_y_start -= dst_y_start; // this is an add becasue dst_y_start is negative
        dst_y_start = 0;
    }

    if (dst_y_start >= dst_img->h) return;
    int src_y_dst_height = src_height_scaled - src_y_start;
    if (src_y_dst_height <= 0) return;

    // Clamp end x to image bounds.
    int dst_x_end = dst_x_start + src_x_dst_width;
    if (dst_x_end > dst_img->w) dst_x_end = dst_img->w;

    // Clamp end y to image bounds.
    int dst_y_end = dst_y_start + src_y_dst_height;
    if (dst_y_end > dst_img->h) dst_y_end = dst_img->h;

    if (dst_delta_x < 0) {
        // Since we are drawing backwards we have to slide our drawing offset forward by an amount
        // limited by the size of the drawing area left. E.g. when we hit the right edge we have
        // advance the offset to prevent the image from sliding.
        int allowed_offset_width = src_width_scaled - (dst_x_end - dst_x_start);
        src_x_start = IM_MIN(dst_x_start, allowed_offset_width);
    }

    // Apply roi offset
    if (roi) src_x_start += fast_floorf(roi->x * x_scale);

    if (dst_delta_y < 0) {
        // Since we are drawing backwards we have to slide our drawing offset forward by an amount
        // limited by the size of the drawing area left. E.g. when we hit the bottom edge we have
        // advance the offset to prevent the image from sliding.
        int allowed_offset_height = src_height_scaled - (dst_y_end - dst_y_start);
        src_y_start = IM_MIN(dst_y_start, allowed_offset_height);
    }

    // Apply roi offset
    if (roi) src_y_start += fast_floorf(roi->y * y_scale);

    // For all of the scaling algorithms (nearest neighbor, bilinear, bicubic, and area)
    // we use a 32-bit fraction instead of a floating point value for iteration. Below,
    // we calculate an increment which fits in 32-bits. We can then add this value
    // successively as we loop over the destination pixels and then shift this sum
    // right by 16 to get the corresponding source pixel. If we want the fractional
    // position we just have to look at the bottom 16-bits.
    //
    // top 16-bits = whole part, bottom 16-bits = fractional part.

    int dst_x_reset = (dst_delta_x < 0) ? (dst_x_end - 1) : dst_x_start;
    long src_x_frac = fast_floorf(65536.0f / x_scale), src_x_frac_size = (src_x_frac + 0xFFFF) >> 16;
    long src_x_accum_reset = fast_floorf((src_x_start << 16) / x_scale);

    int dst_y_reset = (dst_delta_y < 0) ? (dst_y_end - 1) : dst_y_start;
    long src_y_frac = fast_floorf(65536.0f / y_scale), src_y_frac_size = (src_y_frac + 0xFFFF) >> 16;
    long src_y_accum_reset = fast_floorf((src_y_start << 16) / y_scale);

    // Nearest Neighbor
    if ((src_x_frac == 65536) && (src_y_frac == 65536)) hint &= ~(IMAGE_HINT_AREA | IMAGE_HINT_BICUBIC | IMAGE_HINT_BILINEAR);

    // Nearest Neighbor
    if ((hint & IMAGE_HINT_AREA) && (x_scale >= 1.f) && (y_scale >= 1.f)) hint &= ~(IMAGE_HINT_AREA | IMAGE_HINT_BICUBIC | IMAGE_HINT_BILINEAR);

    // Cannot interpolate.
    if ((src_img_w <= 3) || (src_img_h <= 3)) {
        if (hint & IMAGE_HINT_BICUBIC) hint |= IMAGE_HINT_BILINEAR;
        hint &= ~IMAGE_HINT_BICUBIC;
    }

    // Cannot interpolate.
    if ((src_img_w <= 1) || (src_img_h <= 1)) hint &= ~(IMAGE_HINT_AREA | IMAGE_HINT_BILINEAR);

    // Bicbuic and bilinear both shift the image right by (0.5, 0.5) so we have to undo that.
    if (hint & (IMAGE_HINT_BICUBIC | IMAGE_HINT_BILINEAR)) {
        src_x_accum_reset -= 0x8000;
        src_y_accum_reset -= 0x8000;
    }

    bool is_bayer = src_img->bpp == IMAGE_BPP_BAYER;
    bool is_color = is_bayer || (src_img->bpp == IMAGE_BPP_RGB565);

    // rgb_channel extracted / color_palette applied image
    image_t new_src_img;

    if (((hint & IMAGE_HINT_EXTRACT_RGB_CHANNEL_FIRST) && is_color && (rgb_channel != -1))
    || ((hint & IMAGE_HINT_APPLY_COLOR_PALETTE_FIRST) && color_palette)) {
        new_src_img.w = src_img_w; // same width as source image
        new_src_img.h = src_img_h; // same height as source image
        new_src_img.bpp = color_palette ? IMAGE_BPP_RGB565 : IMAGE_BPP_GRAYSCALE;
        new_src_img.data = fb_alloc(image_size(&new_src_img), FB_ALLOC_NO_HINT);
        imlib_draw_image(&new_src_img, src_img, 0, 0, 1.f, 1.f, NULL, rgb_channel, 256, color_palette, NULL, 0, NULL, NULL);
        src_img = &new_src_img;
        rgb_channel = -1;
        color_palette = NULL;
    }

    // Best bpp to convert bayer image to.
    int is_bayer_bpp = (rgb_channel != -1) ? IMAGE_BPP_RGB565 : (color_palette ? IMAGE_BPP_GRAYSCALE : dst_img->bpp);

    bool no_scaling_nearest_neighbor = (dst_delta_x == 1)
            && (dst_x_start == 0) && (src_x_start == 0)
            && (src_x_frac == 65536) && (src_y_frac == 65536);

    // If we are scaling just make a deep copy.
    bool is_scaling = (hint & (IMAGE_HINT_AREA | IMAGE_HINT_BICUBIC | IMAGE_HINT_BILINEAR))
            || (!no_scaling_nearest_neighbor);

    // Otherwise, we only have to do a deep copy if the image is growing.
    size_t src_img_row_bytes = image_size(src_img) / src_img->h;
    size_t dst_img_row_bytes = image_size(dst_img) / dst_img->h;

    // Do we need to convert the image?
    bool is_bayer_color_conversion = is_bayer && (dst_img->bpp != IMAGE_BPP_BAYER);

    // Force a deep copy if we cannot use the image in-place.
    bool need_deep_copy = (dst_img->data == src_img->data)
            && (is_scaling || (src_img_row_bytes < dst_img_row_bytes) || is_bayer_color_conversion);

    // Force a deep copy if we are scaling.
    bool is_bayer_scaling = is_bayer_color_conversion && is_scaling;

    // Make a deep copy of the source iamge.
    if (need_deep_copy || is_bayer_scaling) {
        new_src_img.w = src_img->w; // same width as source image
        new_src_img.h = src_img->h; // same height as source image

        if (is_bayer) {
            new_src_img.bpp = is_bayer_bpp;
            size_t size = image_size(&new_src_img);
            new_src_img.data = fb_alloc(image_size(&new_src_img), FB_ALLOC_NO_HINT);

            switch (new_src_img.bpp) {
                case IMAGE_BPP_BINARY: {
                    imlib_debayer_image_to_binary(&new_src_img, src_img);
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    imlib_debayer_image_to_grayscale(&new_src_img, src_img);
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    imlib_debayer_image_to_rgb565(&new_src_img, src_img);
                    break;
                }
                case IMAGE_BPP_BAYER: {
                    memcpy(new_src_img.data, src_img->data, size);
                    break;
                }
                default : {
                    break;
                }
            }
        } else {
            new_src_img.bpp = src_img->bpp;
            size_t size = image_size(&new_src_img);
            new_src_img.data = fb_alloc(size, FB_ALLOC_NO_HINT);
            memcpy(new_src_img.data, src_img->data, size);
        }

        src_img = &new_src_img;
    }

    imlib_draw_row_data_t imlib_draw_row_data;
    imlib_draw_row_data.dst_img = dst_img;
    imlib_draw_row_data.src_img_bpp = is_bayer ? is_bayer_bpp : src_img->bpp;
    imlib_draw_row_data.rgb_channel = rgb_channel;
    imlib_draw_row_data.alpha = alpha;
    imlib_draw_row_data.color_palette = color_palette;
    imlib_draw_row_data.alpha_palette = alpha_palette;
    imlib_draw_row_data.black_background = hint & IMAGE_HINT_BLACK_BACKGROUND;
    imlib_draw_row_data.callback = callback;
    imlib_draw_row_data.dst_row_override = dst_row_override;
    #ifdef IMLIB_ENABLE_DMA2D
    imlib_draw_row_data.dma2d_request = (alpha != 256) || alpha_palette ||
        (hint & (IMAGE_HINT_AREA | IMAGE_HINT_BICUBIC | IMAGE_HINT_BILINEAR));
    #endif

    imlib_draw_row_setup(&imlib_draw_row_data);

    // Y loop iteration variables
    int dst_y = dst_y_reset;
    long src_y_accum = src_y_accum_reset;
    int next_src_y_index = src_y_accum >> 16;
    int y = dst_y_start;
    bool y_not_done = y < dst_y_end;

    if (hint & IMAGE_HINT_AREA) {
        // The area scaling algorithm runs in fast mode if the image is being scaled down by
        // 1, 2, 3, 4, 5, etc. or slow mode if it's a fractional scale.
        //
        // In fast mode area scaling is just the sum of the specified area. No weighting of pixels
        // is required to get the job done.
        //
        // In slow mode we need to weight pixels that lie on the edges of the area scale rectangle.
        // This prevents making the inner loop of the algorithm tight.
        //
        if ((!(src_x_frac & 0xFFFF)) && (!(src_y_frac & 0xFFFF))) { // fast
            switch (src_img->bpp) {
                case IMAGE_BPP_BINARY: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index;
                        int src_y_index_end = src_y_index + src_y_frac_size;
                        if (src_y_index_end > src_img_h) src_y_index_end = src_img_h;
                        int height = src_y_index_end - src_y_index;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_end = src_x_index + src_x_frac_size;
                            if (src_x_index_end > src_img_w) src_x_index_end = src_img_w;
                            int width = src_x_index_end - src_x_index;

                            uint32_t area = width * height;
                            uint32_t acc = 0;

                            for (int i = src_y_index; i < src_y_index_end; i++) {
                                uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, i);
                                for (int j = src_x_index; j < src_x_index_end; j++) {
                                    acc += IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, j);
                                }
                            }

                            int pixel = (acc + (area >> 1)) / area;

                            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index;
                        int src_y_index_end = src_y_index + src_y_frac_size;
                        if (src_y_index_end > src_img_h) src_y_index_end = src_img_h;
                        int height = src_y_index_end - src_y_index;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_end = src_x_index + src_x_frac_size;
                            if (src_x_index_end > src_img_w) src_x_index_end = src_img_w;
                            int width = src_x_index_end - src_x_index;

                            uint32_t area = width * height;
                            uint32_t acc = 0;

                            if (width < 4) {
                                for (int i = src_y_index; i < src_y_index_end; i++) {
                                    uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, i) + src_x_index;
                                    int n = width;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                    uint16_t *src_row_ptr16 = (uint16_t *) src_row_ptr;

                                    for (; n > 1; n -= 2) {
                                        uint16_t pixels = *src_row_ptr16++;
                                        acc = __USADA8(pixels, 0, acc);
                                    }

                                    src_row_ptr = (uint8_t *) src_row_ptr16;
#endif
                                    for (; n > 0; n -= 1)  {
                                        acc += *src_row_ptr++;
                                    }
                                }
                            } else {
                                for (int i = src_y_index; i < src_y_index_end; i++) {
                                    uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, i) + src_x_index;
                                    int n = width;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                    uint32_t *src_row_ptr32 = (uint32_t *) src_row_ptr;

                                    for (; n > 3; n -= 4) {
                                        uint32_t pixels = *src_row_ptr32++;
                                        acc = __USADA8(pixels, 0, acc);
                                    }

                                    src_row_ptr = (uint8_t *) src_row_ptr32;
#endif
                                    for (; n > 0; n -= 1)  {
                                        acc += *src_row_ptr++;
                                    }
                                }
                            }

                            int pixel = (acc + (area >> 1)) / area;

                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index;
                        int src_y_index_end = src_y_index + src_y_frac_size;
                        if (src_y_index_end > src_img_h) src_y_index_end = src_img_h;
                        int height = src_y_index_end - src_y_index;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_end = src_x_index + src_x_frac_size;
                            if (src_x_index_end > src_img_w) src_x_index_end = src_img_w;
                            int width = src_x_index_end - src_x_index;

                            uint32_t area = width * height;
                            uint32_t r_acc = 0, g_acc = 0, b_acc = 0;

                            for (int i = src_y_index; i < src_y_index_end; i++) {
                                uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, i) + src_x_index;
                                int n = width;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                uint32_t *src_row_ptr32 = (uint32_t *) src_row_ptr;

                                for (; n > 1; n -= 2) {
                                    uint32_t pixels = *src_row_ptr32++;

                                    long r = (pixels >> 11) & 0x1F001F;
                                    r_acc = __USADA8(r, 0, r_acc);

                                    long g = (pixels >> 5) & 0x3F003F;
                                    g_acc = __USADA8(g, 0, g_acc);

                                    long b = pixels & 0x1F001F;
                                    b_acc = __USADA8(b, 0, b_acc);
                                }

                                src_row_ptr = (uint16_t *) src_row_ptr32;
#endif
                                for (; n > 0; n -= 1)  {
                                    int pixel = *src_row_ptr++;
                                    r_acc += COLOR_RGB565_TO_R5(pixel);
                                    g_acc += COLOR_RGB565_TO_G6(pixel);
                                    b_acc += COLOR_RGB565_TO_B5(pixel);
                                }
                            }

                            r_acc = (r_acc + (area >> 1)) / area;
                            g_acc = (g_acc + (area >> 1)) / area;
                            b_acc = (b_acc + (area >> 1)) / area;

                            int pixel = COLOR_R5_G6_B5_TO_RGB565(r_acc, g_acc, b_acc);

                            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                default: {
                    break;
                }
            }
        } else { // slow
            switch (src_img->bpp) {
                case IMAGE_BPP_BINARY: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index, src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_end = src_y_index + src_y_frac_size - 1; // inclusive end

                        int t_y_weight = 256 - ((src_y_accum >> 8) & 0xFF);
                        int b_y_weight = ((src_y_accum + src_y_frac) >> 8) & 0xFF;
                        // Since src_y_index_end is inclusive this should be 256 when there's perfect overlap.
                        if (!b_y_weight) b_y_weight = 256;

                        // Handle end being off the edge.
                        if (src_y_index_end > h_limit) {
                            src_y_index_end = h_limit;
                            // Either we don't need end of we chopped off the last part.
                            if (src_y_index_end == src_y_index) b_y_weight = 0;
                            else b_y_weight = 256; // max out if we chopped off
                        }

                        int y_height_m_2 = src_y_index_end - src_y_index - 1;

                        uint32_t *t_src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index);
                        uint32_t *b_src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_end);

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index, src_x_index_p_1 = src_x_index + 1;
                            int src_x_index_end = src_x_index + src_x_frac_size - 1; // inclusive end

                            int l_x_weight = 256 - ((src_x_accum >> 8) & 0xFF);
                            int r_x_weight = ((src_x_accum + src_x_frac) >> 8) & 0xFF;
                            // Since src_x_index_end is inclusive this should be 256 when there's perfect overlap.
                            if (!r_x_weight) r_x_weight = 256;

                            // Handle end being off the edge.
                            if (src_x_index_end > w_limit) {
                                src_x_index_end = w_limit;
                                // Either we don't need end of we chopped off the last part.
                                if (src_x_index_end == src_x_index) r_x_weight = 0;
                                else r_x_weight = 256; // max out if we chopped off
                            }

                            int x_width_m_2 = src_x_index_end - src_x_index - 1;

                            int t_l_weight = t_y_weight * l_x_weight;
                            int t_r_weight = t_y_weight * r_x_weight;
                            int b_l_weight = b_y_weight * l_x_weight;
                            int b_r_weight = b_y_weight * r_x_weight;

                            uint32_t area = t_l_weight + t_r_weight + b_l_weight + b_r_weight;
                            uint32_t acc = 0;

                            // sum corners

                            acc += IMAGE_GET_BINARY_PIXEL_FAST(t_src_row_ptr, src_x_index) * t_l_weight;
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(t_src_row_ptr, src_x_index_end) * t_r_weight;
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(b_src_row_ptr, src_x_index) * b_l_weight;
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(b_src_row_ptr, src_x_index_end) * b_r_weight;

                            area = (area + 255) >> 8;
                            acc = (acc + 128) >> 8;

                            if (x_width_m_2 > 0) { // sum top/bot
                                area += x_width_m_2 * (t_y_weight + b_y_weight);
                                for (int i = src_x_index_p_1; i < src_x_index_end; i++) {
                                    acc += IMAGE_GET_BINARY_PIXEL_FAST(t_src_row_ptr, i) * t_y_weight;
                                    acc += IMAGE_GET_BINARY_PIXEL_FAST(b_src_row_ptr, i) * b_y_weight;
                                }
                            }

                            if (y_height_m_2 > 0) { // sum left/right
                                area += y_height_m_2 * (l_x_weight + r_x_weight);
                                for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                    uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, i);
                                    acc += IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, src_x_index) * l_x_weight;
                                    acc += IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, src_x_index_end) * r_x_weight;
                                }
                            }

                            area = (area + 255) >> 8;
                            acc = (acc + 128) >> 8;

                            if ((x_width_m_2 > 0) && (y_height_m_2 > 0)) { // sum middle
                                area += x_width_m_2 * y_height_m_2;
                                for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                    uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, i);
                                    for (int j = src_x_index_p_1; j < src_x_index_end; j++) {
                                        acc += IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, j);
                                    }
                                }
                            }

                            int pixel = (acc + (area >> 1)) / area;

                            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index, src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_end = src_y_index + src_y_frac_size - 1; // inclusive end

                        int t_y_weight = 256 - ((src_y_accum >> 8) & 0xFF);
                        int b_y_weight = ((src_y_accum + src_y_frac) >> 8) & 0xFF;
                        // Since src_y_index_end is inclusive this should be 256 when there's perfect overlap.
                        if (!b_y_weight) b_y_weight = 256;

                        // Handle end being off the edge.
                        if (src_y_index_end > h_limit) {
                            src_y_index_end = h_limit;
                            // Either we don't need end of we chopped off the last part.
                            if (src_y_index_end == src_y_index) b_y_weight = 0;
                            else b_y_weight = 256; // max out if we chopped off
                        }

                        int y_height_m_2 = src_y_index_end - src_y_index - 1;

                        uint8_t *t_src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index);
                        uint8_t *b_src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_end);

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index, src_x_index_p_1 = src_x_index + 1;
                            int src_x_index_end = src_x_index + src_x_frac_size - 1; // inclusive end

                            int l_x_weight = 256 - ((src_x_accum >> 8) & 0xFF);
                            int r_x_weight = ((src_x_accum + src_x_frac) >> 8) & 0xFF;
                            // Since src_x_index_end is inclusive this should be 256 when there's perfect overlap.
                            if (!r_x_weight) r_x_weight = 256;

                            // Handle end being off the edge.
                            if (src_x_index_end > w_limit) {
                                src_x_index_end = w_limit;
                                // Either we don't need end of we chopped off the last part.
                                if (src_x_index_end == src_x_index) r_x_weight = 0;
                                else r_x_weight = 256; // max out if we chopped off
                            }

                            int x_width_m_2 = src_x_index_end - src_x_index - 1;

                            int t_l_weight = t_y_weight * l_x_weight;
                            int t_r_weight = t_y_weight * r_x_weight;
                            int b_l_weight = b_y_weight * l_x_weight;
                            int b_r_weight = b_y_weight * r_x_weight;

                            uint32_t area = t_l_weight + t_r_weight + b_l_weight + b_r_weight;
                            uint32_t acc = 0;

                            // sum corners

                            acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(t_src_row_ptr, src_x_index) * t_l_weight;
                            acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(t_src_row_ptr, src_x_index_end) * t_r_weight;
                            acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(b_src_row_ptr, src_x_index) * b_l_weight;
                            acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(b_src_row_ptr, src_x_index_end) * b_r_weight;

                            area = (area + 255) >> 8;
                            acc = (acc + 128) >> 8;

                            if (x_width_m_2 > 0) { // sum top/bot
                                area += x_width_m_2 * (t_y_weight + b_y_weight);
                                uint8_t *t_src_row_ptr_tmp = t_src_row_ptr + src_x_index_p_1;
                                uint8_t *b_src_row_ptr_tmp = b_src_row_ptr + src_x_index_p_1;
                                for (int i = src_x_index_p_1; i < src_x_index_end; i++) {
                                    acc += *t_src_row_ptr_tmp++ * t_y_weight;
                                    acc += *b_src_row_ptr_tmp++ * b_y_weight;
                                }
                            }

                            if (y_height_m_2 > 0) { // sum left/right
                                area += y_height_m_2 * (l_x_weight + r_x_weight);
                                for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                    uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, i);
                                    acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, src_x_index) * l_x_weight;
                                    acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, src_x_index_end) * r_x_weight;
                                }
                            }

                            area = (area + 255) >> 8;
                            acc = (acc + 128) >> 8;

                            if ((x_width_m_2 > 0) && (y_height_m_2 > 0)) { // sum middle
                                area += x_width_m_2 * y_height_m_2;
                                if (x_width_m_2 < 4) {
                                    for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                        uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, i) + src_x_index_p_1;
                                        int n = x_width_m_2;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                        uint16_t *src_row_ptr16 = (uint16_t *) src_row_ptr;

                                        for (; n > 1; n -= 2) {
                                            uint16_t pixels = *src_row_ptr16++;
                                            acc = __USADA8(pixels, 0, acc);
                                        }

                                        src_row_ptr = (uint8_t *) src_row_ptr16;
#endif
                                        for (; n > 0; n -= 1)  {
                                            acc += *src_row_ptr++;
                                        }
                                    }
                                } else {
                                    for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                        uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, i) + src_x_index_p_1;
                                        int n = x_width_m_2;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                        uint32_t *src_row_ptr32 = (uint32_t *) src_row_ptr;

                                        for (; n > 4; n -= 4) {
                                            uint32_t pixels = *src_row_ptr32++;
                                            acc = __USADA8(pixels, 0, acc);
                                        }

                                        src_row_ptr = (uint8_t *) src_row_ptr32;
#endif
                                        for (; n > 0; n -= 1)  {
                                            acc += *src_row_ptr++;
                                        }
                                    }
                                }
                            }

                            int pixel = (acc + (area >> 1)) / area;

                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    while (y_not_done) {
                        int src_y_index = next_src_y_index, src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_end = src_y_index + src_y_frac_size - 1; // inclusive end

                        int t_y_weight = 128 - ((src_y_accum >> 9) & 0x7F);
                        int b_y_weight = ((src_y_accum + src_y_frac) >> 9) & 0x7F;
                        // Since src_y_index_end is inclusive this should be 128 when there's perfect overlap.
                        if (!b_y_weight) b_y_weight = 128;

                        // Handle end being off the edge.
                        if (src_y_index_end > h_limit) {
                            src_y_index_end = h_limit;
                            // Either we don't need end of we chopped off the last part.
                            if (src_y_index_end == src_y_index) b_y_weight = 0;
                            else b_y_weight = 128; // max out if we chopped off
                        }

                        int y_height_m_2 = src_y_index_end - src_y_index - 1;
                        long smlad_y_weight = (t_y_weight << 16) | b_y_weight;

                        uint16_t *t_src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index);
                        uint16_t *b_src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_end);

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index, src_x_index_p_1 = src_x_index + 1;
                            int src_x_index_end = src_x_index + src_x_frac_size - 1; // inclusive end

                            int l_x_weight = 128 - ((src_x_accum >> 9) & 0x7F);
                            int r_x_weight = ((src_x_accum + src_x_frac) >> 9) & 0x7F;
                            // Since src_x_index_end is inclusive this should be 128 when there's perfect overlap.
                            if (!r_x_weight) r_x_weight = 128;

                            // Handle end being off the edge.
                            if (src_x_index_end > w_limit) {
                                src_x_index_end = w_limit;
                                // Either we don't need end of we chopped off the last part.
                                if (src_x_index_end == src_x_index) r_x_weight = 0;
                                else r_x_weight = 128; // max out if we chopped off
                            }

                            int x_width_m_2 = src_x_index_end - src_x_index - 1;
                            long smlad_x_weight = (l_x_weight << 16) | r_x_weight;

                            long t_smlad_x_weight = smlad_x_weight * t_y_weight;
                            long b_smlad_x_weight = smlad_x_weight * b_y_weight;
                            long t_b_smlad_x_weight_sum = __QADD16(t_smlad_x_weight, b_smlad_x_weight);

                            uint32_t area = __SMUAD(t_b_smlad_x_weight_sum, 0x10001);
                            uint32_t r_acc = 0, g_acc = 0, b_acc = 0;

                            // sum corners

                            int t_l_pixel = IMAGE_GET_RGB565_PIXEL_FAST(t_src_row_ptr, src_x_index);
                            int t_r_pixel = IMAGE_GET_RGB565_PIXEL_FAST(t_src_row_ptr, src_x_index_end);
                            int t_pixels = (t_l_pixel << 16) | t_r_pixel;

                            long t_r = (t_pixels >> 11) & 0x1F001F;
                            r_acc = __SMLAD(t_r, t_smlad_x_weight, r_acc);

                            long t_g = (t_pixels >> 5) & 0x3F003F;
                            g_acc = __SMLAD(t_g, t_smlad_x_weight, g_acc);

                            long t_b = t_pixels & 0x1F001F;
                            b_acc = __SMLAD(t_b, t_smlad_x_weight, b_acc);

                            int b_l_pixel = IMAGE_GET_RGB565_PIXEL_FAST(b_src_row_ptr, src_x_index);
                            int b_r_pixel = IMAGE_GET_RGB565_PIXEL_FAST(b_src_row_ptr, src_x_index_end);
                            int b_pixels = (b_l_pixel << 16) | b_r_pixel;

                            long b_r = (b_pixels >> 11) & 0x1F001F;
                            r_acc = __SMLAD(b_r, b_smlad_x_weight, r_acc);

                            long b_g = (b_pixels >> 5) & 0x3F003F;
                            g_acc = __SMLAD(b_g, b_smlad_x_weight, g_acc);

                            long b_b = b_pixels & 0x1F001F;
                            b_acc = __SMLAD(b_b, b_smlad_x_weight, b_acc);

                            area = (area + 127) >> 7;
                            r_acc = (r_acc + 64) >> 7;
                            g_acc = (g_acc + 64) >> 7;
                            b_acc = (b_acc + 64) >> 7;

                            if (x_width_m_2 > 0) { // sum top/bot
                                area += x_width_m_2 * (t_y_weight + b_y_weight);
                                uint16_t *t_src_row_ptr_tmp = t_src_row_ptr + src_x_index_p_1;
                                uint16_t *b_src_row_ptr_tmp = b_src_row_ptr + src_x_index_p_1;
                                for (int i = src_x_index_p_1; i < src_x_index_end; i++) {
                                    int t_y_pixel = *t_src_row_ptr_tmp++;
                                    int b_y_pixel = *b_src_row_ptr_tmp++;
                                    int pixels = (t_y_pixel << 16) | b_y_pixel;

                                    long r = (pixels >> 11) & 0x1F001F;
                                    r_acc = __SMLAD(r, smlad_y_weight, r_acc);

                                    long g = (pixels >> 5) & 0x3F003F;
                                    g_acc = __SMLAD(g, smlad_y_weight, g_acc);

                                    long b = pixels & 0x1F001F;
                                    b_acc = __SMLAD(b, smlad_y_weight, b_acc);
                                }
                            }

                            if (y_height_m_2 > 0) { // sum left/right
                                area += y_height_m_2 * (l_x_weight + r_x_weight);
                                for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                    uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, i);
                                    int l_x_pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, src_x_index);
                                    int r_x_pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, src_x_index_end);
                                    int pixels = (l_x_pixel << 16) | r_x_pixel;

                                    long r = (pixels >> 11) & 0x1F001F;
                                    r_acc = __SMLAD(r, smlad_x_weight, r_acc);

                                    long g = (pixels >> 5) & 0x3F003F;
                                    g_acc = __SMLAD(g, smlad_x_weight, g_acc);

                                    long b = pixels & 0x1F001F;
                                    b_acc = __SMLAD(b, smlad_x_weight, b_acc);
                                }
                            }

                            area = (area + 127) >> 7;
                            r_acc = (r_acc + 64) >> 7;
                            g_acc = (g_acc + 64) >> 7;
                            b_acc = (b_acc + 64) >> 7;

                            if ((x_width_m_2 > 0) && (y_height_m_2 > 0)) { // sum middle
                                area += x_width_m_2 * y_height_m_2;
                                for (int i = src_y_index_p_1; i < src_y_index_end; i++) {
                                    uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, i) + src_x_index_p_1;
                                    int n = x_width_m_2;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                                    uint32_t *src_row_ptr32 = (uint32_t *) src_row_ptr;

                                    for (; n > 1; n -= 2) {
                                        uint32_t pixels = *src_row_ptr32++;

                                        long r = (pixels >> 11) & 0x1F001F;
                                        r_acc = __USADA8(r, 0, r_acc);

                                        long g = (pixels >> 5) & 0x3F003F;
                                        g_acc = __USADA8(g, 0, g_acc);

                                        long b = pixels & 0x1F001F;
                                        b_acc = __USADA8(b, 0, b_acc);
                                    }

                                    src_row_ptr = (uint16_t *) src_row_ptr32;
#endif
                                    for (; n > 0; n -= 1)  {
                                        int pixel = *src_row_ptr++;
                                        r_acc += COLOR_RGB565_TO_R5(pixel);
                                        g_acc += COLOR_RGB565_TO_G6(pixel);
                                        b_acc += COLOR_RGB565_TO_B5(pixel);
                                    }
                                }
                            }

                            r_acc = (r_acc + (area >> 1)) / area;
                            g_acc = (g_acc + (area >> 1)) / area;
                            b_acc = (b_acc + (area >> 1)) / area;

                            int pixel = COLOR_R5_G6_B5_TO_RGB565(r_acc, g_acc, b_acc);

                            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                default: {
                    break;
                }
            }
        }
    } else if (hint & IMAGE_HINT_BICUBIC) {
        // Implements the traditional bicubic interpolation algorithm which uses
        // a 4x4 filter block with the current pixel centered at (1,1) (C below).
        // However, instead of floating point math, it uses integer (fixed point).
        // The Cortex-M4/M7 has a hardware floating point unit, so doing FP math
        // doesn't take any extra time, but it does take extra time to convert
        // the integer pixels to floating point and back to integers again.
        // So this allows it to execute more quickly in pure integer math.
        //
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
        // | x | C | x | x |
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
        // | x | x | x | x |
        // +---+---+---+---+
        //
        switch (src_img->bpp) {
            case IMAGE_BPP_BINARY: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint32_t *src_row_ptr_0, *src_row_ptr_1, *src_row_ptr_2, *src_row_ptr_3;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = src_row_ptr_2 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_3 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 1);
                    } else if (src_y_index == 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_2 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 1);
                        src_row_ptr_3 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 2);
                    } else if (src_y_index == h_limit_m_1) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, h_limit_m_1);
                        src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, h_limit);
                    } else if (src_y_index >= h_limit) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 4 neighboring rows
                        int src_y_index_m_1 = src_y_index - 1;
                        int src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_p_2 = src_y_index + 2;
                        src_row_ptr_0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_2 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                        src_row_ptr_3 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_p_2);
                    }

                    do { // Cache the results of getting the source rows
                        // 15-bit fraction to fit a square of it in 32-bits
                        // pre-calculate the ^1, ^2, and ^3 of the fraction
                        int dy = ((src_y_accum >> 1) & 0x7FFF);
                        int dy2 = (dy * dy) >> 15;
                        int dy3 = (dy2 * dy) >> 15;
                        long smuad_dy_dy2 = (dy << 16) | dy2;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_m_1 = src_x_index - 1;
                            int src_x_index_p_1 = src_x_index + 1;
                            int src_x_index_p_2 = src_x_index + 2;
                            int pixel_x_offests[4];

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = pixel_x_offests[2] = 0;
                                pixel_x_offests[3] = 1;
                            } else if (src_x_index == 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = 0;
                                pixel_x_offests[2] = 1;
                                pixel_x_offests[3] = 2;
                            } else if (src_x_index == w_limit_m_1) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = w_limit_m_1;
                                pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else if (src_x_index >= w_limit) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else { // get 4 neighboring rows
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = src_x_index;
                                pixel_x_offests[2] = src_x_index_p_1;
                                pixel_x_offests[3] = src_x_index_p_2;
                            }

                            int d[4];

                            for (int z = 0; z < 4; z++) { // bicubic x step (-1 to +2)
                                int pixel_0 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr_0, pixel_x_offests[z]) * 0xFF; // more res
                                int pixel_1 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr_1, pixel_x_offests[z]) * 0xFF; // more res
                                int pixel_2 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr_2, pixel_x_offests[z]) * 0xFF; // more res
                                int pixel_3 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr_3, pixel_x_offests[z]) * 0xFF; // more res

                                int a0 = pixel_2 - pixel_0;
                                int a1 = (pixel_0 << 1) + (pixel_2 << 2) - (5 * pixel_1) - pixel_3;
                                int a2 = (3 * (pixel_1 - pixel_2)) + pixel_3 - pixel_0;
                                long smuad_a0_a1 = __PKHBT(a1, a0, 16);
                                int pixel_1_avg = (pixel_1 << 16) | 0x8000;

                                d[z] = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1, (dy3 * a2) + pixel_1_avg)) >> 16;
                            } // for z

                            int d0 = d[0], d1 = d[1], d2 = d[2], d3 = d[3];
                            int a0 = d2 - d0;
                            int a1 = (d0 << 1) + (d2 << 2) - (5 * d1) - d3;
                            int a2 = (3 * (d1 - d2)) + d3 - d0;
                            long smuad_a0_a1 = __PKHBT(a1, a0, 16);
                            int d1_avg = (d1 << 16) | 0x8000;

                            do { // Cache the results of getting the source pixels
                                // 15-bit fraction to fit a square of it in 32-bits
                                // pre-calculate the ^1, ^2, and ^3 of the fraction
                                int dx = ((src_x_accum >> 1) & 0x7FFF);
                                int dx2 = (dx * dx) >> 15;
                                int dx3 = (dx2 * dx) >> 15;
                                long smuad_dx_dx2 = (dx << 16) | dx2;

                                int pixel = __SMLAD(smuad_dx_dx2, smuad_a0_a1, (dx3 * a2) + d1_avg);

                                // clamp output
                                pixel = __USAT_ASR(pixel, 1, 23);

                                IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint8_t *src_row_ptr_0, *src_row_ptr_1, *src_row_ptr_2, *src_row_ptr_3;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = src_row_ptr_2 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_3 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 1);
                    } else if (src_y_index == 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_2 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 1);
                        src_row_ptr_3 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 2);
                    } else if (src_y_index == h_limit_m_1) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, h_limit_m_1);
                        src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, h_limit);
                    } else if (src_y_index >= h_limit) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 4 neighboring rows
                        int src_y_index_m_1 = src_y_index - 1;
                        int src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_p_2 = src_y_index + 2;
                        src_row_ptr_0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_2 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                        src_row_ptr_3 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_p_2);
                    }

                    do { // Cache the results of getting the source rows
                        // 15-bit fraction to fit a square of it in 32-bits
                        // pre-calculate the ^1, ^2, and ^3 of the fraction
                        int dy = ((src_y_accum >> 1) & 0x7FFF);
                        int dy2 = (dy * dy) >> 15;
                        int dy3 = (dy2 * dy) >> 15;
                        long smuad_dy_dy2 = (dy << 16) | dy2;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_m_1 = src_x_index - 1;
// Concept code showing off how to do 4 operations in parallel. Not useful however because of overflows
// in the 8-bit accumulators - the final image looks bad. Might be workable for lower bit-depth images.
#if 0
                            int pixel_row_0, pixel_row_1, pixel_row_2, pixel_row_3;
                            // Column 0 = Bits[7:0]
                            // Column 1 = Bits[15:8]
                            // Column 2 = Bits[23:16]
                            // Column 3 = Bits[31:24]

                            if (src_x_index < 0) {
                                pixel_row_0 = ((*src_row_ptr_0) * 0x010101) | ((*(src_row_ptr_0 + 1)) << 24);
                                pixel_row_1 = ((*src_row_ptr_1) * 0x010101) | ((*(src_row_ptr_1 + 1)) << 24);
                                pixel_row_2 = ((*src_row_ptr_2) * 0x010101) | ((*(src_row_ptr_2 + 1)) << 24);
                                pixel_row_3 = ((*src_row_ptr_3) * 0x010101) | ((*(src_row_ptr_3 + 1)) << 24);
                            } else if (src_x_index == 0) {
                                pixel_row_0 = ((*src_row_ptr_0) * 0x0101) | ((*((uint16_t *) (src_row_ptr_0 + 1))) << 16);
                                pixel_row_1 = ((*src_row_ptr_1) * 0x0101) | ((*((uint16_t *) (src_row_ptr_1 + 1))) << 16);
                                pixel_row_2 = ((*src_row_ptr_2) * 0x0101) | ((*((uint16_t *) (src_row_ptr_2 + 1))) << 16);
                                pixel_row_3 = ((*src_row_ptr_3) * 0x0101) | ((*((uint16_t *) (src_row_ptr_3 + 1))) << 16);
                            } else if (src_x_index == w_limit_m_1) {
                                pixel_row_0 = (*((uint16_t *) (src_row_ptr_0 + src_x_index_m_1))) | ((*(src_row_ptr_0 + w_limit)) * 0x01010000);
                                pixel_row_1 = (*((uint16_t *) (src_row_ptr_1 + src_x_index_m_1))) | ((*(src_row_ptr_1 + w_limit)) * 0x01010000);
                                pixel_row_2 = (*((uint16_t *) (src_row_ptr_2 + src_x_index_m_1))) | ((*(src_row_ptr_2 + w_limit)) * 0x01010000);
                                pixel_row_3 = (*((uint16_t *) (src_row_ptr_3 + src_x_index_m_1))) | ((*(src_row_ptr_3 + w_limit)) * 0x01010000);
                            } else if (src_x_index >= w_limit) {
                                pixel_row_0 = (*(src_row_ptr_0 + src_x_index_m_1)) | ((*(src_row_ptr_0 + w_limit)) * 0x01010100);
                                pixel_row_1 = (*(src_row_ptr_1 + src_x_index_m_1)) | ((*(src_row_ptr_1 + w_limit)) * 0x01010100);
                                pixel_row_2 = (*(src_row_ptr_2 + src_x_index_m_1)) | ((*(src_row_ptr_2 + w_limit)) * 0x01010100);
                                pixel_row_3 = (*(src_row_ptr_3 + src_x_index_m_1)) | ((*(src_row_ptr_3 + w_limit)) * 0x01010100);
                            } else { // get 4 neighboring rows
                                pixel_row_0 = *((uint32_t *) (src_row_ptr_0 + src_x_index_m_1));
                                pixel_row_1 = *((uint32_t *) (src_row_ptr_1 + src_x_index_m_1));
                                pixel_row_2 = *((uint32_t *) (src_row_ptr_2 + src_x_index_m_1));
                                pixel_row_3 = *((uint32_t *) (src_row_ptr_3 + src_x_index_m_1));
                            }

                            // Need 8-bit signed (0x7F max).
                            pixel_row_0 = __UHADD8(pixel_row_0, 0);
                            pixel_row_1 = __UHADD8(pixel_row_1, 0);
                            pixel_row_2 = __UHADD8(pixel_row_2, 0);
                            pixel_row_3 = __UHADD8(pixel_row_3, 0);

                            // Need 1/3 gaurd bits.
                            pixel_row_0 = __UHADD8(pixel_row_0, 0);
                            pixel_row_1 = __UHADD8(pixel_row_1, 0);
                            pixel_row_2 = __UHADD8(pixel_row_2, 0);
                            pixel_row_3 = __UHADD8(pixel_row_3, 0);

                            // Need 2/3 gaurd bits.
                            pixel_row_0 = __UHADD8(pixel_row_0, 0);
                            pixel_row_1 = __UHADD8(pixel_row_1, 0);
                            pixel_row_2 = __UHADD8(pixel_row_2, 0);
                            pixel_row_3 = __UHADD8(pixel_row_3, 0);

                            // Need 3/3 gaurd bits.
                            pixel_row_0 = __UHADD8(pixel_row_0, 0);
                            pixel_row_1 = __UHADD8(pixel_row_1, 0);
                            pixel_row_2 = __UHADD8(pixel_row_2, 0);
                            pixel_row_3 = __UHADD8(pixel_row_3, 0);

                            long temp0 = __QADD8(pixel_row_2, pixel_row_2);
                            long temp1 = __QADD8(pixel_row_1, pixel_row_1);
                            long temp2 = __QSUB8(pixel_row_1, pixel_row_2);

                            long a0_col = __QSUB8(pixel_row_2, pixel_row_0);
                            long a1_col = __QSUB8(__QSUB8(__QADD8(__QADD8(pixel_row_0, pixel_row_0), __QADD8(temp0, temp0)), __QADD8(__QADD8(temp1, temp1), pixel_row_1)), pixel_row_3);
                            long a2_col = __QSUB8(__QADD8(__QADD8(__QADD8(temp2, temp2), temp2), pixel_row_3), pixel_row_0);

                            long a0_col_2_0 = __SXTB16(a0_col);
                            long a1_col_2_0 = __SXTB16(a1_col);
                            long a2_col_2_0 = __SXTB16(a2_col);

                            long smuad_a0_a1_0 = __PKHBT(a1_col_2_0, a0_col_2_0, 16);
                            long pixel_1_avg_0 = ((pixel_row_1 & 0xff) << 16) | 0x8000;
                            int d0 = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1_0, __SMLAD(dy3, a2_col_2_0, pixel_1_avg_0))) >> 16;

                            long smuad_a0_a1_2 = __PKHTB(a0_col_2_0, a1_col_2_0, 16);
                            long pixel_1_avg_2 = (pixel_row_1 & 0xff0000) | 0x8000;
                            int d2 = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1_2, __SMLADX(dy3, a2_col_2_0, pixel_1_avg_2))) >> 16;

                            long a0_col_3_1 = __SXTB16_RORn(a0_col, 8);
                            long a1_col_3_1 = __SXTB16_RORn(a1_col, 8);
                            long a2_col_3_1 = __SXTB16_RORn(a2_col, 8);

                            long smuad_a0_a1_1 = __PKHBT(a1_col_3_1, a0_col_3_1, 16);
                            long pixel_1_avg_1 = ((pixel_row_1 << 8) & 0xff0000) | 0x8000;
                            int d1 = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1_1, __SMLAD(dy3, a2_col_3_1, pixel_1_avg_1))) >> 16;

                            long smuad_a0_a1_3 = __PKHTB(a0_col_3_1, a1_col_3_1, 16);
                            long pixel_1_avg_3 = ((pixel_row_1 >> 8) & 0xff0000) | 0x8000;
                            int d3 = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1_3, __SMLADX(dy3, a2_col_3_1, pixel_1_avg_3))) >> 16;
#else
                            int src_x_index_p_1 = src_x_index + 1;
                            int src_x_index_p_2 = src_x_index + 2;
                            int pixel_x_offests[4];

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = pixel_x_offests[2] = 0;
                                pixel_x_offests[3] = 1;
                            } else if (src_x_index == 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = 0;
                                pixel_x_offests[2] = 1;
                                pixel_x_offests[3] = 2;
                            } else if (src_x_index == w_limit_m_1) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = w_limit_m_1;
                                pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else if (src_x_index >= w_limit) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else { // get 4 neighboring rows
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = src_x_index;
                                pixel_x_offests[2] = src_x_index_p_1;
                                pixel_x_offests[3] = src_x_index_p_2;
                            }

                            int d[4];

                            for (int z = 0; z < 4; z++) { // bicubic x step (-1 to +2)
                                int pixel_0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr_0, pixel_x_offests[z]);
                                int pixel_1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr_1, pixel_x_offests[z]);
                                int pixel_2 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr_2, pixel_x_offests[z]);
                                int pixel_3 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr_3, pixel_x_offests[z]);

                                int a0 = pixel_2 - pixel_0;
                                int a1 = (pixel_0 << 1) + (pixel_2 << 2) - (5 * pixel_1) - pixel_3;
                                int a2 = (3 * (pixel_1 - pixel_2)) + pixel_3 - pixel_0;
                                long smuad_a0_a1 = __PKHBT(a1, a0, 16);
                                int pixel_1_avg = (pixel_1 << 16) | 0x8000;

                                d[z] = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_a0_a1, (dy3 * a2) + pixel_1_avg)) >> 16;
                            } // for z

                            int d0 = d[0], d1 = d[1], d2 = d[2], d3 = d[3];
#endif
                            int a0 = d2 - d0;
                            int a1 = (d0 << 1) + (d2 << 2) - (5 * d1) - d3;
                            int a2 = (3 * (d1 - d2)) + d3 - d0;
                            long smuad_a0_a1 = __PKHBT(a1, a0, 16);
                            int d1_avg = (d1 << 16) | 0x8000;

                            do { // Cache the results of getting the source pixels
                                // 15-bit fraction to fit a square of it in 32-bits
                                // pre-calculate the ^1, ^2, and ^3 of the fraction
                                int dx = ((src_x_accum >> 1) & 0x7FFF);
                                int dx2 = (dx * dx) >> 15;
                                int dx3 = (dx2 * dx) >> 15;
                                long smuad_dx_dx2 = (dx << 16) | dx2;

                                int pixel = __SMLAD(smuad_dx_dx2, smuad_a0_a1, (dx3 * a2) + d1_avg);

                                // clamp output
                                pixel = __USAT_ASR(pixel, 8, 16);

                                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_RGB565: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint16_t *src_row_ptr_0, *src_row_ptr_1, *src_row_ptr_2, *src_row_ptr_3;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = src_row_ptr_2 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_3 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 1);
                    } else if (src_y_index == 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 0);
                        src_row_ptr_2 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 1);
                        src_row_ptr_3 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 2);
                    } else if (src_y_index == h_limit_m_1) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, h_limit_m_1);
                        src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, h_limit);
                    } else if (src_y_index >= h_limit) {
                        int src_y_index_m_1 = src_y_index - 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = src_row_ptr_2 = src_row_ptr_3 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 4 neighboring rows
                        int src_y_index_m_1 = src_y_index - 1;
                        int src_y_index_p_1 = src_y_index + 1;
                        int src_y_index_p_2 = src_y_index + 2;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_m_1);
                        src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_2 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                        src_row_ptr_3 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_p_2);
                    }

                    do { // Cache the results of getting the source rows
                        // 15-bit fraction to fit a square of it in 32-bits
                        // pre-calculate the ^1, ^2, and ^3 of the fraction
                        int dy = ((src_y_accum >> 1) & 0x7FFF);
                        int dy2 = (dy * dy) >> 15;
                        int dy3 = (dy2 * dy) >> 15;
                        long smuad_dy_dy2 = (dy << 16) | dy2;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int src_x_index_m_1 = src_x_index - 1;
                            int src_x_index_p_1 = src_x_index + 1;
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
                            uint32_t pixel_row_0[2], pixel_row_1[2], pixel_row_2[2], pixel_row_3[2];
                            // Column 0 = Bits[15:0]
                            // Column 1 = Bits[31:16]

                            if (src_x_index < 0) {
                                pixel_row_0[0] = (*src_row_ptr_0) * 0x10001;
                                pixel_row_0[1] = __PKHBT(pixel_row_0[0], *(src_row_ptr_0 + 1), 16);
                                pixel_row_1[0] = (*src_row_ptr_1) * 0x10001;
                                pixel_row_1[1] = __PKHBT(pixel_row_1[0], *(src_row_ptr_1 + 1), 16);
                                pixel_row_2[0] = (*src_row_ptr_2) * 0x10001;
                                pixel_row_2[1] = __PKHBT(pixel_row_2[0], *(src_row_ptr_2 + 1), 16);
                                pixel_row_3[0] = (*src_row_ptr_3) * 0x10001;
                                pixel_row_3[1] = __PKHBT(pixel_row_3[0], *(src_row_ptr_3 + 1), 16);
                            } else if (src_x_index == 0) {
                                pixel_row_0[0] = (*src_row_ptr_0) * 0x10001;
                                pixel_row_0[1] = *((uint32_t *) (src_row_ptr_0 + 1));
                                pixel_row_1[0] = (*src_row_ptr_1) * 0x10001;
                                pixel_row_1[1] = *((uint32_t *) (src_row_ptr_1 + 1));
                                pixel_row_2[0] = (*src_row_ptr_2) * 0x10001;
                                pixel_row_2[1] = *((uint32_t *) (src_row_ptr_2 + 1));
                                pixel_row_3[0] = (*src_row_ptr_3) * 0x10001;
                                pixel_row_3[1] = *((uint32_t *) (src_row_ptr_3 + 1));
                            } else if (src_x_index == w_limit_m_1) {
                                pixel_row_0[0] = *((uint32_t *) (src_row_ptr_0 + src_x_index_m_1));
                                pixel_row_0[1] = (*(src_row_ptr_0 + w_limit)) * 0x10001;
                                pixel_row_1[0] = *((uint32_t *) (src_row_ptr_1 + src_x_index_m_1));
                                pixel_row_1[1] = (*(src_row_ptr_1 + w_limit)) * 0x10001;
                                pixel_row_2[0] = *((uint32_t *) (src_row_ptr_2 + src_x_index_m_1));
                                pixel_row_2[1] = (*(src_row_ptr_2 + w_limit)) * 0x10001;
                                pixel_row_3[0] = *((uint32_t *) (src_row_ptr_3 + src_x_index_m_1));
                                pixel_row_3[1] = (*(src_row_ptr_3 + w_limit)) * 0x10001;
                            } else if (src_x_index >= w_limit) {
                                pixel_row_0[0] = *((uint32_t *) (src_row_ptr_0 + src_x_index_m_1));
                                pixel_row_0[1] = (pixel_row_0[0] >> 16) * 0x10001;
                                pixel_row_1[0] = *((uint32_t *) (src_row_ptr_1 + src_x_index_m_1));
                                pixel_row_1[1] = (pixel_row_1[0] >> 16) * 0x10001;
                                pixel_row_2[0] = *((uint32_t *) (src_row_ptr_2 + src_x_index_m_1));
                                pixel_row_2[1] = (pixel_row_2[0] >> 16) * 0x10001;
                                pixel_row_3[0] = *((uint32_t *) (src_row_ptr_3 + src_x_index_m_1));
                                pixel_row_3[1] = (pixel_row_3[0] >> 16) * 0x10001;
                            } else { // get 4 neighboring rows
                                pixel_row_0[0] = *((uint32_t *) (src_row_ptr_0 + src_x_index_m_1));
                                pixel_row_0[1] = *((uint32_t *) (src_row_ptr_0 + src_x_index_p_1));
                                pixel_row_1[0] = *((uint32_t *) (src_row_ptr_1 + src_x_index_m_1));
                                pixel_row_1[1] = *((uint32_t *) (src_row_ptr_1 + src_x_index_p_1));
                                pixel_row_2[0] = *((uint32_t *) (src_row_ptr_2 + src_x_index_m_1));
                                pixel_row_2[1] = *((uint32_t *) (src_row_ptr_2 + src_x_index_p_1));
                                pixel_row_3[0] = *((uint32_t *) (src_row_ptr_3 + src_x_index_m_1));
                                pixel_row_3[1] = *((uint32_t *) (src_row_ptr_3 + src_x_index_p_1));
                            }

                            int r_d[4], g_d[4], b_d[4];

                            for (int z = 0; z < 2; z++) { // dual bicubic x step (-1 to +2)

                                long r_pixel_row_0 = (pixel_row_0[z] >> 11) & 0x1f001f;
                                long r_pixel_row_1 = (pixel_row_1[z] >> 11) & 0x1f001f;
                                long r_pixel_row_2 = (pixel_row_2[z] >> 11) & 0x1f001f;
                                long r_pixel_row_3 = (pixel_row_3[z] >> 11) & 0x1f001f;

                                uint32_t r_a0_col = __QSUB16(r_pixel_row_2, r_pixel_row_0);
                                uint32_t r_a1_col = __QSUB16(__QSUB16(__QADD16(r_pixel_row_0 << 1, r_pixel_row_2 << 2), r_pixel_row_1 * 5), r_pixel_row_3);
                                uint32_t r_a2_col = __QSUB16(__QADD16(__QSUB16(r_pixel_row_1 * 3, r_pixel_row_2 * 3), r_pixel_row_3), r_pixel_row_0);

                                long r_smuad_a0_a1_0 = __PKHBT(r_a1_col, r_a0_col, 16);
                                long r_pixel_1_avg_0 =  (r_pixel_row_1 << 16) | 0x8000;
                                r_d[z*2] = ((int32_t) __SMLAD(smuad_dy_dy2, r_smuad_a0_a1_0, __SMLAD(dy3, r_a2_col, r_pixel_1_avg_0))) >> 16;

                                long r_smuad_a0_a1_1 = __PKHTB(r_a0_col, r_a1_col, 16);
                                long r_pixel_1_avg_1 = __PKHTB(r_pixel_row_1, 0x8000, 0);
                                r_d[(z*2)+1] = ((int32_t) __SMLAD(smuad_dy_dy2, r_smuad_a0_a1_1, __SMLADX(dy3, r_a2_col, r_pixel_1_avg_1))) >> 16;

                                long g_pixel_row_0 = (pixel_row_0[z] >> 5) & 0x3f003f;
                                long g_pixel_row_1 = (pixel_row_1[z] >> 5) & 0x3f003f;
                                long g_pixel_row_2 = (pixel_row_2[z] >> 5) & 0x3f003f;
                                long g_pixel_row_3 = (pixel_row_3[z] >> 5) & 0x3f003f;

                                uint32_t g_a0_col = __QSUB16(g_pixel_row_2, g_pixel_row_0);
                                uint32_t g_a1_col = __QSUB16(__QSUB16(__QADD16(g_pixel_row_0 << 1, g_pixel_row_2 << 2), g_pixel_row_1 * 5), g_pixel_row_3);
                                uint32_t g_a2_col = __QSUB16(__QADD16(__QSUB16(g_pixel_row_1 * 3, g_pixel_row_2 * 3), g_pixel_row_3), g_pixel_row_0);

                                long g_smuad_a0_a1_0 = __PKHBT(g_a1_col, g_a0_col, 16);
                                long g_pixel_1_avg_0 =  (g_pixel_row_1 << 16) | 0x8000;
                                g_d[z*2] = ((int32_t) __SMLAD(smuad_dy_dy2, g_smuad_a0_a1_0, __SMLAD(dy3, g_a2_col, g_pixel_1_avg_0))) >> 16;

                                long g_smuad_a0_a1_1 = __PKHTB(g_a0_col, g_a1_col, 16);
                                long g_pixel_1_avg_1 = __PKHTB(g_pixel_row_1, 0x8000, 0);
                                g_d[(z*2)+1] = ((int32_t) __SMLAD(smuad_dy_dy2, g_smuad_a0_a1_1, __SMLADX(dy3, g_a2_col, g_pixel_1_avg_1))) >> 16;

                                long b_pixel_row_0 = pixel_row_0[z] & 0x1f001f;
                                long b_pixel_row_1 = pixel_row_1[z] & 0x1f001f;
                                long b_pixel_row_2 = pixel_row_2[z] & 0x1f001f;
                                long b_pixel_row_3 = pixel_row_3[z] & 0x1f001f;

                                uint32_t b_a0_col = __QSUB16(b_pixel_row_2, b_pixel_row_0);
                                uint32_t b_a1_col = __QSUB16(__QSUB16(__QADD16(b_pixel_row_0 << 1, b_pixel_row_2 << 2), b_pixel_row_1 * 5), b_pixel_row_3);
                                uint32_t b_a2_col = __QSUB16(__QADD16(__QSUB16(b_pixel_row_1 * 3, b_pixel_row_2 * 3), b_pixel_row_3), b_pixel_row_0);

                                long b_smuad_a0_a1_0 = __PKHBT(b_a1_col, b_a0_col, 16);
                                long b_pixel_1_avg_0 =  (b_pixel_row_1 << 16) | 0x8000;
                                b_d[z*2] = ((int32_t) __SMLAD(smuad_dy_dy2, b_smuad_a0_a1_0, __SMLAD(dy3, b_a2_col, b_pixel_1_avg_0))) >> 16;

                                long b_smuad_a0_a1_1 = __PKHTB(b_a0_col, b_a1_col, 16);
                                long b_pixel_1_avg_1 = __PKHTB(b_pixel_row_1, 0x8000, 0);
                                b_d[(z*2)+1] = ((int32_t) __SMLAD(smuad_dy_dy2, b_smuad_a0_a1_1, __SMLADX(dy3, b_a2_col, b_pixel_1_avg_1))) >> 16;
                            } // for z
#else
                            int src_x_index_p_2 = src_x_index + 2;
                            int pixel_x_offests[4];

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = pixel_x_offests[2] = 0;
                                pixel_x_offests[3] = 1;
                            } else if (src_x_index == 0) {
                                pixel_x_offests[0] = pixel_x_offests[1] = 0;
                                pixel_x_offests[2] = 1;
                                pixel_x_offests[3] = 2;
                            } else if (src_x_index == w_limit_m_1) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = w_limit_m_1;
                                pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else if (src_x_index >= w_limit) {
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = pixel_x_offests[2] = pixel_x_offests[3] = w_limit;
                            } else { // get 4 neighboring rows
                                pixel_x_offests[0] = src_x_index_m_1;
                                pixel_x_offests[1] = src_x_index;
                                pixel_x_offests[2] = src_x_index_p_1;
                                pixel_x_offests[3] = src_x_index_p_2;
                            }

                            int r_d[4], g_d[4], b_d[4];

                            for (int z = 0; z < 4; z++) { // bicubic x step (-1 to +2)
                                int pixel_0 = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr_0, pixel_x_offests[z]);
                                int pixel_1 = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr_1, pixel_x_offests[z]);
                                int pixel_2 = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr_2, pixel_x_offests[z]);
                                int pixel_3 = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr_3, pixel_x_offests[z]);

                                int r0 = pixel_0 >> 11;
                                int r1 = pixel_1 >> 11;
                                int r2 = pixel_2 >> 11;
                                int r3 = pixel_3 >> 11;

                                int r_a0 = r2 - r0;
                                int r_a1 = (r0 << 1) + (r2 << 2) - (5 * r1) - r3;
                                int r_a2 = (3 * (r1 - r2)) + r3 - r0;
                                long smuad_r_a0_r_a1 = __PKHBT(r_a1, r_a0, 16);
                                int r1_avg = (r1 << 16) | 0x8000;

                                r_d[z] = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_r_a0_r_a1, (dy3 * r_a2) + r1_avg)) >> 16;

                                int g0 = (pixel_0 >> 5) & 0x3F;
                                int g1 = (pixel_1 >> 5) & 0x3F;
                                int g2 = (pixel_2 >> 5) & 0x3F;
                                int g3 = (pixel_3 >> 5) & 0x3F;

                                int g_a0 = g2 - g0;
                                int g_a1 = (g0 << 1) + (g2 << 2) - (5 * g1) - g3;
                                int g_a2 = (3 * (g1 - g2)) + g3 - g0;
                                long smuad_g_a0_g_a1 = __PKHBT(g_a1, g_a0, 16);
                                int g1_avg = (g1 << 16) | 0x8000;

                                g_d[z] = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_g_a0_g_a1, (dy3 * g_a2) + g1_avg)) >> 16;

                                int b0 = pixel_0 & 0x1F;
                                int b1 = pixel_1 & 0x1F;
                                int b2 = pixel_2 & 0x1F;
                                int b3 = pixel_3 & 0x1F;

                                int b_a0 = b2 - b0;
                                int b_a1 = (b0 << 1) + (b2 << 2) - (5 * b1) - b3;
                                int b_a2 = (3 * (b1 - b2)) + b3 - b0;
                                long smuad_b_a0_b_a1 = __PKHBT(b_a1, b_a0, 16);
                                int b1_avg = (b1 << 16) | 0x8000;

                                b_d[z] = ((int32_t) __SMLAD(smuad_dy_dy2, smuad_b_a0_b_a1, (dy3 * b_a2) + b1_avg)) >> 16;
                            } // for z
#endif
                            int r_d0 = r_d[0], r_d1 = r_d[1], r_d2 = r_d[2], r_d3 = r_d[3];
                            int r_a0 = r_d2 - r_d0;
                            int r_a1 = (r_d0 << 1) + (r_d2 << 2) - (5 * r_d1) - r_d3;
                            int r_a2 = (3 * (r_d1 - r_d2)) + r_d3 - r_d0;
                            long smuad_r_a0_r_a1 = __PKHBT(r_a1, r_a0, 16);
                            int r_d1_avg = (r_d1 << 16) | 0x8000;

                            int g_d0 = g_d[0], g_d1 = g_d[1], g_d2 = g_d[2], g_d3 = g_d[3];
                            int g_a0 = g_d2 - g_d0;
                            int g_a1 = (g_d0 << 1) + (g_d2 << 2) - (5 * g_d1) - g_d3;
                            int g_a2 = (3 * (g_d1 - g_d2)) + g_d3 - g_d0;
                            long smuad_g_a0_g_a1 = __PKHBT(g_a1, g_a0, 16);
                            int g_d1_avg = (g_d1 << 16) | 0x8000;

                            int b_d0 = b_d[0], b_d1 = b_d[1], b_d2 = b_d[2], b_d3 = b_d[3];
                            int b_a0 = b_d2 - b_d0;
                            int b_a1 = (b_d0 << 1) + (b_d2 << 2) - (5 * b_d1) - b_d3;
                            int b_a2 = (3 * (b_d1 - b_d2)) + b_d3 - b_d0;
                            long smuad_b_a0_b_a1 = __PKHBT(b_a1, b_a0, 16);
                            int b_d1_avg = (b_d1 << 16) | 0x8000;

                            do { // Cache the results of getting the source pixels
                                // 15-bit fraction to fit a square of it in 32-bits
                                // pre-calculate the ^1, ^2, and ^3 of the fraction
                                int dx = ((src_x_accum >> 1) & 0x7FFF);
                                int dx2 = (dx * dx) >> 15;
                                int dx3 = (dx2 * dx) >> 15;
                                long smuad_dx_dx2 = (dx << 16) | dx2;

                                long r_pixel = __SMLAD(smuad_dx_dx2, smuad_r_a0_r_a1, (dx3 * r_a2) + r_d1_avg);

                                // clamp output
                                r_pixel = __USAT_ASR(r_pixel, 5, 16);

                                long g_pixel = __SMLAD(smuad_dx_dx2, smuad_g_a0_g_a1, (dx3 * g_a2) + g_d1_avg);

                                // clamp output
                                g_pixel = __USAT_ASR(g_pixel, 6, 16);

                                long b_pixel = __SMLAD(smuad_dx_dx2, smuad_b_a0_b_a1, (dx3 * b_a2) + b_d1_avg);

                                // clamp output
                                b_pixel = __USAT_ASR(b_pixel, 5, 16);

                                int pixel = COLOR_R5_G6_B5_TO_RGB565(r_pixel, g_pixel, b_pixel);

                                IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            default: {
                break;
            }
        }
    } else if (hint & IMAGE_HINT_BILINEAR) {
        // Implements the traditional bilinear interpolation algorithm which uses
        // a 2x2 filter block with the current pixel centered at (0,0) (C below).
        // However, instead of floating point math, it uses integer (fixed point).
        // The Cortex-M4/M7 has a hardware floating point unit, so doing FP math
        // doesn't take any extra time, but it does take extra time to convert
        // the integer pixels to floating point and back to integers again.
        // So this allows it to execute more quickly in pure integer math.
        //
        // +---+---+
        // | C | x |
        // +---+---+
        // | x | x |
        // +---+---+
        //
        switch (src_img->bpp) {
            case IMAGE_BPP_BINARY: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint32_t *src_row_ptr_0, *src_row_ptr_1;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, 0);
                    } else if (src_y_index >= h_limit) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 2 neighboring rows
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                    }

                    do { // Cache the results of getting the source rows
                        uint32_t * src_row_ptr = ((src_y_accum >> 15) & 0x1) ? src_row_ptr_1 : src_row_ptr_0;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel_0, pixel_1;

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_0 = pixel_1 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, 0);
                            } else if (src_x_index >= w_limit) {
                                pixel_0 = pixel_1 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, w_limit);
                            } else { // get 4 neighboring pixels
                                int src_x_index_p_1 = src_x_index + 1;
                                pixel_0 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, src_x_index);
                                pixel_1 = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, src_x_index_p_1);
                            }

                            do { // Cache the results of getting the source pixels
                                int pixel = ((src_x_accum >> 15) & 0x1) ? pixel_1 : pixel_0;

                                IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint8_t *src_row_ptr_0, *src_row_ptr_1;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, 0);
                    } else if (src_y_index >= h_limit) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 2 neighboring rows
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                    }

                    do { // Cache the results of getting the source rows
                        // used to mix pixels vertically
                        long smuad_y = (src_y_accum >> 8) & 0xff;
                        smuad_y |= (256 - smuad_y) << 16;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel_00, pixel_10, pixel_01, pixel_11;

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_00 = pixel_10 = src_row_ptr_0[0];
                                pixel_01 = pixel_11 = src_row_ptr_1[0];
                            } else if (src_x_index >= w_limit) {
                                pixel_00 = pixel_10 = src_row_ptr_0[w_limit];
                                pixel_01 = pixel_11 = src_row_ptr_1[w_limit];
                            } else { // get 4 neighboring pixels
                                int src_x_index_p_1 = src_x_index + 1;
                                pixel_00 = src_row_ptr_0[src_x_index]; pixel_10 = src_row_ptr_0[src_x_index_p_1];
                                pixel_01 = src_row_ptr_1[src_x_index]; pixel_11 = src_row_ptr_1[src_x_index_p_1];
                            }

                            long vertical_avg_0 = (pixel_00 << 16) | pixel_01;
                            int pixel_l = __SMLAD(smuad_y, vertical_avg_0, 128) >> 8; // vertically average

                            long vertical_avg_1 = (pixel_10 << 16) | pixel_11;
                            int pixel_r = __SMLAD(smuad_y, vertical_avg_1, 128) >> 8; // vertically average

                            long horizontal_avg = (pixel_l << 16) | pixel_r;

                            do { // Cache the results of getting the source pixels
                                // used to mix pixels horizontally
                                long smuad_x = (src_x_accum >> 8) & 0xff;
                                smuad_x |= (256 - smuad_x) << 16;

                                int pixel = __SMLAD(smuad_x, horizontal_avg, 128) >> 8; // horizontally average

                                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_RGB565: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint16_t *src_row_ptr_0, *src_row_ptr_1;

                    // keep row pointers in bounds
                    if (src_y_index < 0) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, 0);
                    } else if (src_y_index >= h_limit) {
                        src_row_ptr_0 = src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, h_limit);
                    } else { // get 2 neighboring rows
                        int src_y_index_p_1 = src_y_index + 1;
                        src_row_ptr_0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index);
                        src_row_ptr_1 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index_p_1);
                    }

                    do { // Cache the results of getting the source rows
                        // used to mix pixels vertically
                        long smuad_y = (src_y_accum >> 11) & 0x1f;
                        smuad_y |= (32 - smuad_y) << 16;

                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel_00, pixel_10, pixel_01, pixel_11;

                            // keep pixels in bounds
                            if (src_x_index < 0) {
                                pixel_00 = pixel_10 = src_row_ptr_0[0];
                                pixel_01 = pixel_11 = src_row_ptr_1[0];
                            } else if (src_x_index >= w_limit) {
                                pixel_00 = pixel_10 = src_row_ptr_0[w_limit];
                                pixel_01 = pixel_11 = src_row_ptr_1[w_limit];
                            } else { // get 4 neighboring pixels
                                int src_x_index_p_1 = src_x_index + 1;
                                pixel_00 = src_row_ptr_0[src_x_index]; pixel_10 = src_row_ptr_0[src_x_index_p_1];
                                pixel_01 = src_row_ptr_1[src_x_index]; pixel_11 = src_row_ptr_1[src_x_index_p_1];
                            }

                            const long mask_r = 0x7c007c00, mask_g = 0x07e007e0, mask_b = 0x001f001f;
                            const long avg_rb = 0x4010, avg_g = 0x200;

                            uint32_t rgb_l = (pixel_00 << 16) | pixel_01;
                            long rb_l = ((rgb_l >> 1) & mask_r) | (rgb_l & mask_b);
                            long g_l = rgb_l & mask_g;
                            int rb_out_l = (__SMLAD(smuad_y, rb_l, avg_rb) >> 5) & 0x7c1f;
                            int g_out_l = (__SMLAD(smuad_y, g_l, avg_g) >> 5) & 0x07e0;

                            uint32_t rgb_r = (pixel_10 << 16) | pixel_11;
                            long rb_r = ((rgb_r >> 1) & mask_r) | (rgb_r & mask_b);
                            long g_r = rgb_r & mask_g;
                            int rb_out_r = (__SMLAD(smuad_y, rb_r, avg_rb) >> 5) & 0x7c1f;
                            int g_out_r = (__SMLAD(smuad_y, g_r, avg_g) >> 5) & 0x07e0;

                            long rb = (rb_out_l << 16) | rb_out_r;
                            long g = (g_out_l << 16) | g_out_r;

                            do { // Cache the results of getting the source pixels
                                // used to mix pixels horizontally
                                long smuad_x = (src_x_accum >> 11) & 0x1f;
                                smuad_x |= (32 - smuad_x) << 16;

                                int rb_out = __SMLAD(smuad_x, rb, avg_rb) >> 5;
                                int g_out = __SMLAD(smuad_x, g, avg_g) >> 5;
                                int pixel = ((rb_out << 1) & 0xf800) | (g_out & 0x07e0) | (rb_out & 0x001f);

                                IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            default: {
                break;
            }
        }
    } else if (no_scaling_nearest_neighbor) { // copy
        if (dst_img->data == src_img->data) { // In-Place
            switch (src_img->bpp) {
                case IMAGE_BPP_BINARY: {
                    while (y_not_done) {
                        uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, next_src_x_index);
                            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_GRAYSCALE:
                // Re-use grayscale for bayer.
                case IMAGE_BPP_BAYER: {
                    while (y_not_done) {
                        uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, next_src_x_index);
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    while (y_not_done) {
                        uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, next_src_x_index);
                            IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                            // Increment offsets
                            dst_x += dst_delta_x;
                            src_x_accum += src_x_frac;
                            next_src_x_index = src_x_accum >> 16;
                            x_not_done = ++x < dst_x_end;
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                default: {
                    break;
                }
            }
        } else { // Out-of-Place
            switch (src_img->bpp) {
                case IMAGE_BPP_BINARY: {
                    while (y_not_done) {
                        uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        imlib_draw_row_put_row_buffer(&imlib_draw_row_data, src_row_ptr);
                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    while (y_not_done) {
                        uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        imlib_draw_row_put_row_buffer(&imlib_draw_row_data, src_row_ptr);
                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    while (y_not_done) {
                        uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, next_src_y_index);
                        imlib_draw_row_put_row_buffer(&imlib_draw_row_data, src_row_ptr);
                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                case IMAGE_BPP_BAYER: {
                    while (y_not_done) {
                        switch (is_bayer_bpp) {
                            case IMAGE_BPP_BINARY: {
                                uint32_t *dst_row_ptr = imlib_draw_row_get_row_buffer(&imlib_draw_row_data);
                                imlib_debayer_line_to_binary(dst_x_start, dst_x_end, next_src_y_index,
                                                             dst_row_ptr, src_img);
                                break;
                            }
                            case IMAGE_BPP_GRAYSCALE: {
                                uint8_t *dst_row_ptr = imlib_draw_row_get_row_buffer(&imlib_draw_row_data);
                                imlib_debayer_line_to_grayscale(dst_x_start, dst_x_end, next_src_y_index,
                                                                dst_row_ptr, src_img);
                                break;
                            }
                            case IMAGE_BPP_RGB565: {
                                uint16_t *dst_row_ptr = imlib_draw_row_get_row_buffer(&imlib_draw_row_data);
                                imlib_debayer_line_to_rgb565(dst_x_start, dst_x_end, next_src_y_index,
                                                             dst_row_ptr, src_img);
                                break;
                            }
                            case IMAGE_BPP_BAYER: { // Bayer images have the same shape as grayscale.
                                uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, next_src_y_index);
                                imlib_draw_row_put_row_buffer(&imlib_draw_row_data, src_row_ptr);
                                break;
                            }
                            default : {
                                break;
                            }
                        }

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } // while y
                    break;
                }
                default: {
                    break;
                }
            }
        }
    } else { // nearest neighbor
        switch (src_img->bpp) {
            case IMAGE_BPP_BINARY: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint32_t *src_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(src_img, src_y_index);

                    do { // Cache the results of getting the source row
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint32_t *dst_row_ptr = (uint32_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel = IMAGE_GET_BINARY_PIXEL_FAST(src_row_ptr, src_x_index);

                            do { // Cache the results of getting the source pixel
                                IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_GRAYSCALE:
            // Re-use grayscale for bayer.
            case IMAGE_BPP_BAYER: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint8_t *src_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, src_y_index);

                    do { // Cache the results of getting the source row
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint8_t *dst_row_ptr = (uint8_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(src_row_ptr, src_x_index);

                            do { // Cache the results of getting the source pixel
                                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            case IMAGE_BPP_RGB565: {
                while (y_not_done) {
                    int src_y_index = next_src_y_index;
                    uint16_t *src_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, src_y_index);

                    do { // Cache the results of getting the source row
                        // Must be called per loop to get the address of the temp buffer to blend with
                        uint16_t *dst_row_ptr = (uint16_t *) imlib_draw_row_get_row_buffer(&imlib_draw_row_data);

                        // X loop iteration variables
                        int dst_x = dst_x_reset;
                        long src_x_accum = src_x_accum_reset;
                        int next_src_x_index = src_x_accum >> 16;
                        int x = dst_x_start;
                        bool x_not_done = x < dst_x_end;

                        while (x_not_done) {
                            int src_x_index = next_src_x_index;
                            int pixel = IMAGE_GET_RGB565_PIXEL_FAST(src_row_ptr, src_x_index);

                            do { // Cache the results of getting the source pixel
                                IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, dst_x, pixel);

                                // Increment offsets
                                dst_x += dst_delta_x;
                                src_x_accum += src_x_frac;
                                next_src_x_index = src_x_accum >> 16;
                                x_not_done = ++x < dst_x_end;
                            } while (x_not_done && (src_x_index == next_src_x_index));
                        } // while x

                        imlib_draw_row(dst_x_start, dst_x_end, dst_y, &imlib_draw_row_data);

                        // Increment offsets
                        dst_y += dst_delta_y;
                        src_y_accum += src_y_frac;
                        next_src_y_index = src_y_accum >> 16;
                        y_not_done = ++y < dst_y_end;
                    } while (y_not_done && (src_y_index == next_src_y_index));
                } // while y
                break;
            }
            default: {
                break;
            }
        }
    }

    imlib_draw_row_teardown(&imlib_draw_row_data);
    if (&new_src_img == src_img) fb_free();
}

#ifdef IMLIB_ENABLE_FLOOD_FILL
void imlib_flood_fill(image_t *img, int x, int y,
                      float seed_threshold, float floating_threshold,
                      int c, bool invert, bool clear_background, image_t *mask)
{
    if ((0 <= x) && (x < img->w) && (0 <= y) && (y < img->h)) {
        image_t out;
        out.w = img->w;
        out.h = img->h;
        out.bpp = IMAGE_BPP_BINARY;
        out.data = fb_alloc0(image_size(&out), FB_ALLOC_NO_HINT);

        if (mask) {
            for (int y = 0, yy = out.h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                for (int x = 0, xx = out.w; x < xx; x++) {
                    if (image_get_mask_pixel(mask, x, y)) IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x);
                }
            }
        }

        int color_seed_threshold = 0;
        int color_floating_threshold = 0;

        switch(img->bpp) {
            case IMAGE_BPP_BINARY: {
                color_seed_threshold = fast_floorf(seed_threshold * COLOR_BINARY_MAX);
                color_floating_threshold = fast_floorf(floating_threshold * COLOR_BINARY_MAX);
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                color_seed_threshold = fast_floorf(seed_threshold * COLOR_GRAYSCALE_MAX);
                color_floating_threshold = fast_floorf(floating_threshold * COLOR_GRAYSCALE_MAX);
                break;
            }
            case IMAGE_BPP_RGB565: {
                color_seed_threshold = COLOR_R5_G6_B5_TO_RGB565(fast_floorf(seed_threshold * COLOR_R5_MAX),
                                                                fast_floorf(seed_threshold * COLOR_G6_MAX),
                                                                fast_floorf(seed_threshold * COLOR_B5_MAX));
                color_floating_threshold = COLOR_R5_G6_B5_TO_RGB565(fast_floorf(floating_threshold * COLOR_R5_MAX),
                                                                    fast_floorf(floating_threshold * COLOR_G6_MAX),
                                                                    fast_floorf(floating_threshold * COLOR_B5_MAX));
                break;
            }
            default: {
                break;
            }
        }

        imlib_flood_fill_int(&out, img, x, y, color_seed_threshold, color_floating_threshold, NULL, NULL);

        switch(img->bpp) {
            case IMAGE_BPP_BINARY: {
                for (int y = 0, yy = out.h; y < yy; y++) {
                    uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                    for (int x = 0, xx = out.w; x < xx; x++) {
                        if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, c);
                        } else if (clear_background) {
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, 0);
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                for (int y = 0, yy = out.h; y < yy; y++) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                    for (int x = 0, xx = out.w; x < xx; x++) {
                        if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, c);
                        } else if (clear_background) {
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, 0);
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                for (int y = 0, yy = out.h; y < yy; y++) {
                    uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&out, y);
                    for (int x = 0, xx = out.w; x < xx; x++) {
                        if (IMAGE_GET_BINARY_PIXEL_FAST(out_row_ptr, x) ^ invert) {
                            IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, c);
                        } else if (clear_background) {
                            IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, 0);
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }

        fb_free();
    }
}
#endif // IMLIB_ENABLE_FLOOD_FILL
