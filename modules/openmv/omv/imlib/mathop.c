/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image math operations.
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_MATH_OPS
void imlib_gamma_corr(image_t *img, float gamma, float contrast, float brightness)
{
    gamma = IM_DIV(1.0, gamma);
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            float pScale = COLOR_BINARY_MAX - COLOR_BINARY_MIN;
            float pDiv = 1 / pScale;
            int *p_lut = fb_alloc((COLOR_BINARY_MAX - COLOR_BINARY_MIN + 1) * sizeof(int), FB_ALLOC_NO_HINT);

            for (int i = COLOR_BINARY_MIN; i <= COLOR_BINARY_MAX; i++) {
                int p = ((fast_powf(i * pDiv, gamma) * contrast) + brightness) * pScale;
                p_lut[i] = IM_MIN(IM_MAX(p , COLOR_BINARY_MIN), COLOR_BINARY_MAX);
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, x);
                    int p = p_lut[dataPixel];
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, x, p);
                }
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            float pScale = COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN;
            float pDiv = 1 / pScale;
            int *p_lut = fb_alloc((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN + 1) * sizeof(int), FB_ALLOC_NO_HINT);

            for (int i = COLOR_GRAYSCALE_MIN; i <= COLOR_GRAYSCALE_MAX; i++) {
                int p = ((fast_powf(i * pDiv, gamma) * contrast) + brightness) * pScale;
                p_lut[i] = IM_MIN(IM_MAX(p , COLOR_GRAYSCALE_MIN), COLOR_GRAYSCALE_MAX);
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, x);
                    int p = p_lut[dataPixel];
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, x, p);
                }
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            float rScale = COLOR_R5_MAX - COLOR_R5_MIN;
            float gScale = COLOR_G6_MAX - COLOR_G6_MIN;
            float bScale = COLOR_B5_MAX - COLOR_B5_MIN;
            float rDiv = 1 / rScale;
            float gDiv = 1 / gScale;
            float bDiv = 1 / bScale;
            int *r_lut = fb_alloc((COLOR_R5_MAX - COLOR_R5_MIN + 1) * sizeof(int), FB_ALLOC_NO_HINT);
            int *g_lut = fb_alloc((COLOR_G6_MAX - COLOR_G6_MIN + 1) * sizeof(int), FB_ALLOC_NO_HINT);
            int *b_lut = fb_alloc((COLOR_B5_MAX - COLOR_B5_MIN + 1) * sizeof(int), FB_ALLOC_NO_HINT);

            for (int i = COLOR_R5_MIN; i <= COLOR_R5_MAX; i++) {
                int r = ((fast_powf(i * rDiv, gamma) * contrast) + brightness) * rScale;
                r_lut[i] = IM_MIN(IM_MAX(r , COLOR_R5_MIN), COLOR_R5_MAX);
            }

            for (int i = COLOR_G6_MIN; i <= COLOR_G6_MAX; i++) {
                int g = ((fast_powf(i * gDiv, gamma) * contrast) + brightness) * gScale;
                g_lut[i] = IM_MIN(IM_MAX(g , COLOR_G6_MIN), COLOR_G6_MAX);
            }

            for (int i = COLOR_B5_MIN; i <= COLOR_B5_MAX; i++) {
                int b = ((fast_powf(i * bDiv, gamma) * contrast) + brightness) * bScale;
                b_lut[i] = IM_MIN(IM_MAX(b , COLOR_B5_MIN), COLOR_B5_MAX);
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, x);
                    int r = r_lut[COLOR_RGB565_TO_R5(dataPixel)];
                    int g = g_lut[COLOR_RGB565_TO_G6(dataPixel)];
                    int b = b_lut[COLOR_RGB565_TO_B5(dataPixel)];
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }

            fb_free();
            fb_free();
            fb_free();
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_negate(image_t *img)
{
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                int x = 0, xx = img->w;
                uint32_t *s = data;
                for (; x < xx-31; x += 32) { // do it faster with bit access
                    s[0] = ~s[0]; // invert 32 bits (pixels) in one shot
                    s++;
                }
                for (; x < xx; x++) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, x);
                    int p = (COLOR_BINARY_MAX - COLOR_BINARY_MIN) - dataPixel;
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, x, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                int x = 0, xx = img->w;
                uint32_t a, b, *s = (uint32_t *)data;
                for (; x < xx-7; x+= 8) { // process a pair of 4 pixels at a time
                    a = s[0]; b = s[1]; // read 8 pixels
                    s[0] = ~a; s[1] = ~b;
                    s += 2;
                }
                for (; x < xx; x++) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, x);
                    int p = (COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) - dataPixel;
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, x, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, x);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, x, ~dataPixel);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

typedef struct imlib_replace_line_op_state {
    bool hmirror, vflip, transpose;
    image_t *mask;
} imlib_replace_line_op_state_t;

static void imlib_replace_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    bool hmirror = ((imlib_replace_line_op_state_t *) data)->hmirror;
    bool vflip = ((imlib_replace_line_op_state_t *) data)->vflip;
    bool transpose = ((imlib_replace_line_op_state_t *) data)->transpose;
    image_t *mask = ((imlib_replace_line_op_state_t *) data)->mask;

    image_t target;
    memcpy(&target, img, sizeof(image_t));

    if (transpose) {
        int w = target.w;
        int h = target.h;
        target.w = h;
        target.h = w;
    }

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), h_i);
                    IMAGE_PUT_BINARY_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), h_i);
                    IMAGE_PUT_GRAYSCALE_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), h_i);
                    IMAGE_PUT_RGB565_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_replace(image_t *img, const char *path, image_t *other, int scalar, bool hmirror, bool vflip, bool transpose, image_t *mask)
{
    bool in_place = img->data == other->data;
    image_t temp;

    if (in_place) {
        memcpy(&temp, other, sizeof(image_t));
        temp.data = fb_alloc(image_size(&temp), FB_ALLOC_NO_HINT);
        memcpy(temp.data, other->data, image_size(&temp));
        other = &temp;
    }

    imlib_replace_line_op_state_t state;
    state.hmirror = hmirror;
    state.vflip = vflip;
    state.mask = mask;
    state.transpose = transpose;
    imlib_image_operation(img, path, other, scalar, imlib_replace_line_op, &state);

    if (in_place) {
        fb_free();
    }

    if (transpose) {
        int w = img->w;
        int h = img->h;
        img->w = h;
        img->h = w;
    }
}

static void imlib_add_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = dataPixel | otherPixel; //dataPixel + otherPixel;
//                    p = IM_MIN(p, COLOR_BINARY_MAX);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = dataPixel + otherPixel;
                    p = IM_MIN(p, COLOR_GRAYSCALE_MAX);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = COLOR_RGB565_TO_R5(dataPixel) + COLOR_RGB565_TO_R5(otherPixel);
                    int g = COLOR_RGB565_TO_G6(dataPixel) + COLOR_RGB565_TO_G6(otherPixel);
                    int b = COLOR_RGB565_TO_B5(dataPixel) + COLOR_RGB565_TO_B5(otherPixel);
                    r = IM_MIN(r, COLOR_R5_MAX);
                    g = IM_MIN(g, COLOR_G6_MAX);
                    b = IM_MIN(b, COLOR_B5_MAX);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_add(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_add_line_op, mask);
}

typedef struct imlib_sub_line_op_state {
    bool reverse;
    image_t *mask;
} imlib_sub_line_op_state_t;

static void imlib_sub_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    bool reverse = ((imlib_sub_line_op_state_t *) data)->reverse;
    image_t *mask = ((imlib_sub_line_op_state_t *) data)->mask;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = reverse ? (otherPixel - dataPixel) : (dataPixel - otherPixel);
                    p = IM_MAX(p, COLOR_BINARY_MIN);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = reverse ? (otherPixel - dataPixel) : (dataPixel - otherPixel);
                    p = IM_MAX(p, COLOR_GRAYSCALE_MIN);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int dR = COLOR_RGB565_TO_R5(dataPixel);
                    int dG = COLOR_RGB565_TO_G6(dataPixel);
                    int dB = COLOR_RGB565_TO_B5(dataPixel);
                    int oR = COLOR_RGB565_TO_R5(otherPixel);
                    int oG = COLOR_RGB565_TO_G6(otherPixel);
                    int oB = COLOR_RGB565_TO_B5(otherPixel);
                    int r = reverse ? (oR - dR) : (dR - oR);
                    int g = reverse ? (oG - dG) : (dG - oG);
                    int b = reverse ? (oB - dB) : (dB - oB);
                    r = IM_MAX(r, COLOR_R5_MIN);
                    g = IM_MAX(g, COLOR_G6_MIN);
                    b = IM_MAX(b, COLOR_B5_MIN);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_sub(image_t *img, const char *path, image_t *other, int scalar, bool reverse, image_t *mask)
{
    imlib_sub_line_op_state_t state;
    state.reverse = reverse;
    state.mask = mask;
    imlib_image_operation(img, path, other, scalar, imlib_sub_line_op, &state);
}

typedef struct imlib_mul_line_op_state {
    bool invert;
    image_t *mask;
} imlib_mul_line_op_state_t;

static void imlib_mul_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    bool invert = ((imlib_mul_line_op_state_t *) data)->invert;
    image_t *mask = ((imlib_mul_line_op_state_t *) data)->mask;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            float pScale = COLOR_BINARY_MAX - COLOR_BINARY_MIN;
            float pDiv = 1 / pScale;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = invert ? (pScale - ((pScale - dataPixel) * (pScale - otherPixel) * pDiv))
                                   : (dataPixel * otherPixel * pDiv);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            float pScale = COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN;
            float pDiv = 1 / pScale;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = invert ? (pScale - ((pScale - dataPixel) * (pScale - otherPixel) * pDiv))
                                   : (dataPixel * otherPixel * pDiv);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            float rScale = COLOR_R5_MAX - COLOR_R5_MIN;
            float gScale = COLOR_G6_MAX - COLOR_G6_MIN;
            float bScale = COLOR_B5_MAX - COLOR_B5_MIN;
            float rDiv = 1 / rScale;
            float gDiv = 1 / gScale;
            float bDiv = 1 / bScale;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int dR = COLOR_RGB565_TO_R5(dataPixel);
                    int dG = COLOR_RGB565_TO_G6(dataPixel);
                    int dB = COLOR_RGB565_TO_B5(dataPixel);
                    int oR = COLOR_RGB565_TO_R5(otherPixel);
                    int oG = COLOR_RGB565_TO_G6(otherPixel);
                    int oB = COLOR_RGB565_TO_B5(otherPixel);
                    int r = invert ? (rScale - ((rScale - dR) * (rScale - oR) * rDiv))
                                   : (dR * oR * rDiv);
                    int g = invert ? (gScale - ((gScale - dG) * (gScale - oG) * gDiv))
                                   : (dG * oG * gDiv);
                    int b = invert ? (bScale - ((bScale - dB) * (bScale - oB) * bDiv))
                                   : (dB * oB * bDiv);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_mul(image_t *img, const char *path, image_t *other, int scalar, bool invert, image_t *mask)
{
    imlib_mul_line_op_state_t state;
    state.invert = invert;
    state.mask = mask;
    imlib_image_operation(img, path, other, scalar, imlib_mul_line_op, &state);
}

typedef struct imlib_div_line_op_state {
    bool invert, mod;
    image_t *mask;
} imlib_div_line_op_state_t;

static void imlib_div_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    bool invert = ((imlib_div_line_op_state_t *) data)->invert;
    bool mod = ((imlib_div_line_op_state_t *) data)->mod;
    image_t *mask = ((imlib_div_line_op_state_t *) data)->mask;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            int pScale = COLOR_BINARY_MAX - COLOR_BINARY_MIN;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = mod
                        ? IM_MOD((invert?otherPixel:dataPixel) * pScale, (invert?dataPixel:otherPixel))
                        : IM_DIV((invert?otherPixel:dataPixel) * pScale, (invert?dataPixel:otherPixel));
                    p = IM_MIN(p, COLOR_BINARY_MAX);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            int pScale = COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = mod
                        ? IM_MOD((invert?otherPixel:dataPixel) * pScale, (invert?dataPixel:otherPixel))
                        : IM_DIV((invert?otherPixel:dataPixel) * pScale, (invert?dataPixel:otherPixel));
                    p = IM_MIN(p, COLOR_GRAYSCALE_MAX);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            int rScale = COLOR_R5_MAX - COLOR_R5_MIN;
            int gScale = COLOR_G6_MAX - COLOR_G6_MIN;
            int bScale = COLOR_B5_MAX - COLOR_B5_MIN;
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int dR = COLOR_RGB565_TO_R5(dataPixel);
                    int dG = COLOR_RGB565_TO_G6(dataPixel);
                    int dB = COLOR_RGB565_TO_B5(dataPixel);
                    int oR = COLOR_RGB565_TO_R5(otherPixel);
                    int oG = COLOR_RGB565_TO_G6(otherPixel);
                    int oB = COLOR_RGB565_TO_B5(otherPixel);
                    int r = mod
                        ? IM_MOD((invert?oR:dR) * rScale, (invert?dR:oR))
                        : IM_DIV((invert?oR:dR) * rScale, (invert?dR:oR));
                    int g = mod
                        ? IM_MOD((invert?oG:dG) * gScale, (invert?dG:oG))
                        : IM_DIV((invert?oG:dG) * gScale, (invert?dG:oG));
                    int b = mod
                        ? IM_MOD((invert?oB:dB) * bScale, (invert?dB:oB))
                        : IM_DIV((invert?oB:dB) * bScale, (invert?dB:oB));
                    r = IM_MIN(r, COLOR_R5_MAX);
                    g = IM_MIN(g, COLOR_G6_MAX);
                    b = IM_MIN(b, COLOR_B5_MAX);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_div(image_t *img, const char *path, image_t *other, int scalar, bool invert, bool mod, image_t *mask)
{
    imlib_div_line_op_state_t state;
    state.invert = invert;
    state.mod = mod;
    state.mask = mask;
    imlib_image_operation(img, path, other, scalar, imlib_div_line_op, &state);
}

static void imlib_min_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = IM_MIN(dataPixel, otherPixel);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = IM_MIN(dataPixel, otherPixel);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = IM_MIN(COLOR_RGB565_TO_R5(dataPixel), COLOR_RGB565_TO_R5(otherPixel));
                    int g = IM_MIN(COLOR_RGB565_TO_G6(dataPixel), COLOR_RGB565_TO_G6(otherPixel));
                    int b = IM_MIN(COLOR_RGB565_TO_B5(dataPixel), COLOR_RGB565_TO_B5(otherPixel));
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_min(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_min_line_op, mask);
}

static void imlib_max_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = IM_MAX(dataPixel, otherPixel);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = IM_MAX(dataPixel, otherPixel);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = IM_MAX(COLOR_RGB565_TO_R5(dataPixel), COLOR_RGB565_TO_R5(otherPixel));
                    int g = IM_MAX(COLOR_RGB565_TO_G6(dataPixel), COLOR_RGB565_TO_G6(otherPixel));
                    int b = IM_MAX(COLOR_RGB565_TO_B5(dataPixel), COLOR_RGB565_TO_B5(otherPixel));
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_max(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_max_line_op, mask);
}

static void imlib_difference_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = dataPixel ^ otherPixel; // abs(dataPixel - otherPixel);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = abs(dataPixel - otherPixel);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = abs(COLOR_RGB565_TO_R5(dataPixel) - COLOR_RGB565_TO_R5(otherPixel));
                    int g = abs(COLOR_RGB565_TO_G6(dataPixel) - COLOR_RGB565_TO_G6(otherPixel));
                    int b = abs(COLOR_RGB565_TO_B5(dataPixel) - COLOR_RGB565_TO_B5(otherPixel));
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_difference(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar,imlib_difference_line_op,  mask);
}

typedef struct imlib_blend_line_op_state {
    float alpha;
    image_t *mask;
} imlib_blend_line_op_t;

static void imlib_blend_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    float alpha = ((imlib_blend_line_op_t *) data)->alpha, beta = 1 - alpha;
    image_t *mask = ((imlib_blend_line_op_t *) data)->mask;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = (dataPixel * alpha) + (otherPixel * beta);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = (dataPixel * alpha) + (otherPixel * beta);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = (COLOR_RGB565_TO_R5(dataPixel) * alpha) + (COLOR_RGB565_TO_R5(otherPixel) * beta);
                    int g = (COLOR_RGB565_TO_G6(dataPixel) * alpha) + (COLOR_RGB565_TO_G6(otherPixel) * beta);
                    int b = (COLOR_RGB565_TO_B5(dataPixel) * alpha) + (COLOR_RGB565_TO_B5(otherPixel) * beta);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_blend(image_t *img, const char *path, image_t *other, int scalar, float alpha, image_t *mask)
{
    imlib_blend_line_op_t state;
    state.alpha = alpha;
    state.mask = mask;
    imlib_image_operation(img, path, other, scalar, imlib_blend_line_op, &state);
}
#endif //IMLIB_ENABLE_MATH_OPS
