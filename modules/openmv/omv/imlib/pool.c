/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image pooling.
 *
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_MIDPOINT_POOLING
void imlib_midpoint_pool(image_t *img_i, image_t *img_o, int x_div, int y_div, const int bias)
{
    int min_bias = (256-bias);
    int max_bias = bias;
    switch(img_i->bpp)
    {
        case IMAGE_BPP_BINARY:
        {
            for (int y = 0, yy = img_i->h / y_div, yyy = (img_i->h % y_div) / 2; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int min = COLOR_BINARY_MAX, max = COLOR_BINARY_MIN;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            int pixel = IMAGE_GET_BINARY_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            min = IM_MIN(min, pixel);
                            max = IM_MAX(max, pixel);
                        }
                    }
                    IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x,
                        ((min*min_bias)+(max*max_bias))>>8);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE:
        {
            for (int y = 0, yy = img_i->h, yyy = (img_i->h % y_div) / 2 / y_div; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int min = COLOR_GRAYSCALE_MAX, max = COLOR_GRAYSCALE_MIN;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            int pixel = IMAGE_GET_GRAYSCALE_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            min = IM_MIN(min, pixel);
                            max = IM_MAX(max, pixel);
                        }
                    }
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x,
                        ((min*min_bias)+(max*max_bias))>>8);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565:
        {
            for (int y = 0, yy = img_i->h / y_div, yyy = (img_i->h % y_div) / 2; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int r_min = COLOR_R5_MAX, r_max = COLOR_R5_MIN;
                    int g_min = COLOR_G6_MAX, g_max = COLOR_G6_MIN;
                    int b_min = COLOR_B5_MAX, b_max = COLOR_B5_MIN;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            const uint16_t pixel = IM_GET_RGB565_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            int r = COLOR_RGB565_TO_R5(pixel);
                            int g = COLOR_RGB565_TO_G6(pixel);
                            int b = COLOR_RGB565_TO_B5(pixel);
                            r_min = IM_MIN(r_min, r);
                            r_max = IM_MAX(r_max, r);
                            g_min = IM_MIN(g_min, g);
                            g_max = IM_MAX(g_max, g);
                            b_min = IM_MIN(b_min, b);
                            b_max = IM_MAX(b_max, b);
                        }
                    }
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x,
                        COLOR_R5_G6_B5_TO_RGB565(((r_min*min_bias)+(r_max*max_bias))>>8,
                                                 ((g_min*min_bias)+(g_max*max_bias))>>8,
                                                 ((b_min*min_bias)+(b_max*max_bias))>>8));
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
#endif // IMLIB_ENABLE_MIDPOINT_POOLING

#ifdef IMLIB_ENABLE_MEAN_POOLING
void imlib_mean_pool(image_t *img_i, image_t *img_o, int x_div, int y_div)
{
    int n = x_div * y_div;
    switch(img_i->bpp)
    {
        case IMAGE_BPP_BINARY:
        {
            for (int y = 0, yy = img_i->h / y_div, yyy = (img_i->h % y_div) / 2; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int acc = 0;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            int pixel = IMAGE_GET_BINARY_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            acc += pixel;
                        }
                    }
                    IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, acc / n);
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE:
        {
            for (int y = 0, yy = img_i->h / y_div, yyy = (img_i->h % y_div) / 2; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int acc = 0;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            int pixel = IMAGE_GET_GRAYSCALE_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            acc += pixel;
                        }
                    }
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, acc / n);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565:
        {
            for (int y = 0, yy = img_i->h / y_div, yyy = (img_i->h % y_div) / 2; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img_o, y);
                for (int x = 0, xx = img_i->w / x_div, xxx = (img_i->w % x_div) / 2; x < xx; x++) {
                    int r_acc = 0;
                    int g_acc = 0;
                    int b_acc = 0;
                    for (int i = 0; i < y_div; i++) {
                        for (int j = 0; j < x_div; j++) {
                            int pixel = IMAGE_GET_RGB565_PIXEL(img_i, xxx + (x * x_div) + j, yyy + (y * y_div) + i);
                            r_acc += COLOR_RGB565_TO_R5(pixel);
                            g_acc += COLOR_RGB565_TO_G6(pixel);
                            b_acc += COLOR_RGB565_TO_B5(pixel);
                        }
                    }
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, COLOR_R5_G6_B5_TO_RGB565(r_acc / n, g_acc / n, b_acc / n));
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
#endif // IMLIB_ENABLE_MEAN_POOLING
