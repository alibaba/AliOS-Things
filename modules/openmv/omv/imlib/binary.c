/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Binary image operations.
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_BINARY_OPS
void imlib_binary(image_t *out, image_t *img, list_t *thresholds, bool invert, bool zero, image_t *mask)
{
    image_t bmp;
    bmp.w = img->w;
    bmp.h = img->h;
    bmp.bpp = IMAGE_BPP_BINARY;
    bmp.data = fb_alloc0(image_size(&bmp), FB_ALLOC_NO_HINT);

    for (list_lnk_t *it = iterator_start_from_head(thresholds); it; it = iterator_next(it)) {
        color_thresholds_list_lnk_data_t lnk_data;
        iterator_get(thresholds, it, &lnk_data);
        switch(img->bpp) {
            case IMAGE_BPP_BINARY: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint32_t *old_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(old_row_ptr, x), &lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x), &lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        if (COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x), &lnk_data, invert)) {
                            IMAGE_SET_BINARY_PIXEL_FAST(bmp_row_ptr, x);
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            if (!zero) {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint32_t *old_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        int pixel = ((!mask) || image_get_mask_pixel(mask, x, y))
                            ? IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)
                            : IMAGE_GET_BINARY_PIXEL_FAST(old_row_ptr, x);
                        IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                    }
                }
            } else {
                for (int y = 0, yy = img->h; y < yy; y++) {
                    uint32_t *old_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                    for (int x = 0, xx = img->w; x < xx; x++) {
                        int pixel = IMAGE_GET_BINARY_PIXEL_FAST(old_row_ptr, x);
                        if (((!mask) || image_get_mask_pixel(mask, x, y))
                            && IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)) pixel = 0;
                        IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            if (out->bpp == IMAGE_BPP_BINARY) {
                if (!zero) {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = ((!mask) || image_get_mask_pixel(mask, x, y))
                                ? IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)
                                : COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x));
                            IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                } else {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x));
                            if (((!mask) || image_get_mask_pixel(mask, x, y))
                                && IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)) pixel = 0;
                            IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                }
            } else {
                if (!zero) {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint8_t *out_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = ((!mask) || image_get_mask_pixel(mask, x, y))
                                ? COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x))
                                : IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x);
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                } else {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint8_t *old_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint8_t *out_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row_ptr, x);
                            if (((!mask) || image_get_mask_pixel(mask, x, y))
                                && IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)) pixel = 0;
                            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            if (out->bpp == IMAGE_BPP_BINARY) {
                if (!zero) {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = ((!mask) || image_get_mask_pixel(mask, x, y))
                                ? IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)
                                : COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x));
                            IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                } else {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint32_t *out_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x));
                            if (((!mask) || image_get_mask_pixel(mask, x, y))
                                && IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)) pixel = 0;
                            IMAGE_PUT_BINARY_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                }
            } else {
                if (!zero) {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint16_t *out_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = ((!mask) || image_get_mask_pixel(mask, x, y))
                                ? COLOR_BINARY_TO_RGB565(IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x))
                                : IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x);
                            IMAGE_PUT_RGB565_PIXEL_FAST(out_row_ptr, x, pixel);
                        }
                    }
                } else {
                    for (int y = 0, yy = img->h; y < yy; y++) {
                        uint16_t *old_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                        uint16_t *out_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(out, y);
                        for (int x = 0, xx = img->w; x < xx; x++) {
                            int pixel = IMAGE_GET_RGB565_PIXEL_FAST(old_row_ptr, x);
                            if (((!mask) || image_get_mask_pixel(mask, x, y))
                                && IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x)) pixel = 0;
                            IMAGE_PUT_RGB565_PIXEL_FAST(out_row_ptr, x, pixel);
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

    fb_free();
}

void imlib_invert(image_t *img)
{
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            for (uint32_t *start = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, 0),
                 *end = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, img->h);
                 start < end; start++) {
                *start = ~*start;
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (uint8_t *start = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, 0),
                 *end = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, img->h);
                 start < end; start++) {
                *start = ~*start;
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (uint16_t *start = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, 0),
                 *end = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, img->h);
                 start < end; start++) {
                *start = ~*start;
            }
            break;
        }
        default: {
            break;
        }
    }
}

static void imlib_b_and_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] &= ((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             & IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] &= ((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             & IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] &= ((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             & IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_and(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_b_and_line_op, mask);
}

static void imlib_b_nand_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] &= ~((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             & ~IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] &= ~((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             & ~IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] &= ~((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             & ~IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_nand(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_b_nand_line_op, mask);
}

static void imlib_b_or_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] |= ((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             | IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] |= ((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             | IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] |= ((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             | IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_or(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_b_or_line_op, mask);
}

static void imlib_b_nor_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] |= ~((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             | ~IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] |= ~((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             | ~IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] |= ~((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             | ~IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_nor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar,imlib_b_nor_line_op,  mask);
}

static void imlib_b_xor_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             ^ IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             ^ IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             ^ IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_xor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_b_xor_line_op, mask);
}

static void imlib_b_xnor_line_op(image_t *img, int line, void *other, void *data, bool vflipped)
{
    image_t *mask = (image_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_BINARY_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ~((uint32_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(data, i,
                            (IMAGE_GET_BINARY_PIXEL_FAST(data, i)
                             ^ ~IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_GRAYSCALE_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ~((uint8_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i,
                            (IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i)
                             ^ ~IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i)));
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);

            if(!mask) {
                for (int i = 0, j = IMAGE_RGB565_LINE_LEN(img); i < j; i++) {
                    data[i] ^= ~((uint16_t *) other)[i];
                }
            } else {
                for (int i = 0, j = img->w; i < j; i++) {
                    if (image_get_mask_pixel(mask, i, line)) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(data, i,
                            (IMAGE_GET_RGB565_PIXEL_FAST(data, i)
                             ^ ~IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i)));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_b_xnor(image_t *img, const char *path, image_t *other, int scalar, image_t *mask)
{
    imlib_image_operation(img, path, other, scalar, imlib_b_xnor_line_op, mask);
}

static void imlib_erode_dilate(image_t *img, int ksize, int threshold, int e_or_d, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0, xx = img->w; x < xx; x++) {
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }
                    if (x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) { // faster
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,y+j);
                            // subtract old left column and add new right column
                            acc -= IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x-ksize-1);
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x+ksize);
                        }
                    } else { // slower (checks boundaries per pixel)
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) IMAGE_CLEAR_BINARY_PIXEL_FAST(buf_row_ptr, x);
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) IMAGE_SET_BINARY_PIXEL_FAST(buf_row_ptr, x);
                    }
                }

                if (y >= ksize) { // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_BINARY_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0), yy = img->h; y < yy; y++) {
                memcpy(IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_BINARY_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            buf.data = fb_alloc(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0, xx = img->w; x < xx; x++) {
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }

                    if (x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) { // faster
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,y+j);
                            // subtract old left edge and add new right edge to sum
                            acc -= (IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x-ksize-1) & 1); // values have already been thresholded to 0x00 or 0xFF
                            acc += (IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x+ksize) & 1);
                        } // for j
                    } else { // slower way which checks boundaries per pixel
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += ((IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)))) >> 7);
                            }  // for k
                        } // for j
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x,
                                                                            COLOR_GRAYSCALE_BINARY_MIN);
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x,
                                                                            COLOR_GRAYSCALE_BINARY_MAX);
                    }
                }

                if (y >= ksize) { // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0), yy = img->h; y < yy; y++) {
                memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));
                int acc = 0;

                for (int x = 0, xx = img->w; x < xx; x++) {
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);

                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        continue; // Short circuit.
                    }

                    if (x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) { // faster
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,y+j);
                            // subtract old left column and add new right column
                            acc -= IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,x-ksize-1) & 1; // already 0 or FFFF
                            acc += IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,x+ksize) & 1; // (pre-thresholded)
                        }
                    } else { // need to check boundary conditions for each pixel
                        acc = e_or_d ? 0 : -1; // Don't count center pixel...
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += (IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)))) & 1; // already 0 or FFFF
                            }
                        }
                    }

                    if (!e_or_d) {
                        // Preserve original pixel value... or clear it.
                        if (acc < threshold) IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x,
                                                                         COLOR_RGB565_BINARY_MIN);
                    } else {
                        // Preserve original pixel value... or set it.
                        if (acc > threshold) IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x,
                                                                         COLOR_RGB565_BINARY_MAX);
                    }
                }

                if (y >= ksize) { // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_RGB565_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = IM_MAX(img->h - ksize, 0), yy = img->h; y < yy; y++) {
                memcpy(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_RGB565_LINE_LEN_BYTES(img));
            }

            fb_free();
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_erode(image_t *img, int ksize, int threshold, image_t *mask)
{
    // Threshold should be equal to (((ksize*2)+1)*((ksize*2)+1))-1
    // for normal operation. E.g. for ksize==3 -> threshold==8
    // Basically you're adjusting the number of data that
    // must be set in the kernel (besides the center) for the output to be 1.
    // Erode normally requires all data to be 1.
    imlib_erode_dilate(img, ksize, threshold, 0, mask);
}

void imlib_dilate(image_t *img, int ksize, int threshold, image_t *mask)
{
    // Threshold should be equal to 0
    // for normal operation. E.g. for ksize==3 -> threshold==0
    // Basically you're adjusting the number of data that
    // must be set in the kernel (besides the center) for the output to be 1.
    // Dilate normally requires one pixel to be 1.
    imlib_erode_dilate(img, ksize, threshold, 1, mask);
}

void imlib_open(image_t *img, int ksize, int threshold, image_t *mask)
{
    imlib_erode(img, ksize, (((ksize*2)+1)*((ksize*2)+1))-1 - threshold, mask);
    imlib_dilate(img, ksize, 0 + threshold, mask);
}

void imlib_close(image_t *img, int ksize, int threshold, image_t *mask)
{
    imlib_dilate(img, ksize, 0 + threshold, mask);
    imlib_erode(img, ksize, (((ksize*2)+1)*((ksize*2)+1))-1 - threshold, mask);
}

void imlib_top_hat(image_t *img, int ksize, int threshold, image_t *mask)
{
    image_t temp;
    temp.w = img->w;
    temp.h = img->h;
    temp.bpp = img->bpp;
    temp.data = fb_alloc(image_size(img), FB_ALLOC_NO_HINT);
    memcpy(temp.data, img->data, image_size(img));
    imlib_open(&temp, ksize, threshold, mask);
    imlib_difference(img, NULL, &temp, 0, mask);
    fb_free();
}

void imlib_black_hat(image_t *img, int ksize, int threshold, image_t *mask)
{
    image_t temp;
    temp.w = img->w;
    temp.h = img->h;
    temp.bpp = img->bpp;
    temp.data = fb_alloc(image_size(img), FB_ALLOC_NO_HINT);
    memcpy(temp.data, img->data, image_size(img));
    imlib_close(&temp, ksize, threshold, mask);
    imlib_difference(img, NULL, &temp, 0, mask);
    fb_free();
}
#endif
