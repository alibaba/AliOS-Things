/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image filtering functions.
 */
#include "fsort.h"
#include "imlib.h"

void imlib_histeq(image_t *img, image_t *mask)
{
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            int a = img->w * img->h;
            float s = (COLOR_BINARY_MAX - COLOR_BINARY_MIN) / ((float) a);
            uint32_t *hist = fb_alloc0((COLOR_BINARY_MAX - COLOR_BINARY_MIN + 1) * sizeof(uint32_t), FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    hist[IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x) - COLOR_BINARY_MIN] += 1;
                }
            }

            for (int i = 0, sum = 0, ii = COLOR_BINARY_MAX - COLOR_BINARY_MIN + 1; i < ii; i++) {
                sum += hist[i];
                hist[i] = sum;
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x,
                        fast_floorf((s * hist[pixel - COLOR_BINARY_MIN]) + COLOR_BINARY_MIN));
                }
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            int a = img->w * img->h;
            float s = (COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / ((float) a);
            uint32_t *hist = fb_alloc0((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN + 1) * sizeof(uint32_t), FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    hist[IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x) - COLOR_GRAYSCALE_MIN] += 1;
                }
            }

            for (int i = 0, sum = 0, ii = COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN + 1; i < ii; i++) {
                sum += hist[i];
                hist[i] = sum;
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x,
                        fast_floorf((s * hist[pixel - COLOR_GRAYSCALE_MIN]) + COLOR_GRAYSCALE_MIN));
                }
            }

            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            int a = img->w * img->h;
            float s = (COLOR_Y_MAX - COLOR_Y_MIN) / ((float) a);
            uint32_t *hist = fb_alloc0((COLOR_Y_MAX - COLOR_Y_MIN + 1) * sizeof(uint32_t), FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    hist[COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x)) - COLOR_Y_MIN] += 1;
                }
            }

            for (int i = 0, sum = 0, ii = COLOR_Y_MAX - COLOR_Y_MIN + 1; i < ii; i++) {
                sum += hist[i];
                hist[i] = sum;
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int r = COLOR_RGB565_TO_R8(pixel);
                    int g = COLOR_RGB565_TO_G8(pixel);
                    int b = COLOR_RGB565_TO_B8(pixel);
                    uint8_t y, u, v;
                    y = (uint8_t)(((r * 9770) + (g * 19182) + (b * 3736)) >> 15); // .299*r + .587*g + .114*b
                    u = (uint8_t)(((b << 14) - (r * 5529) - (g * 10855)) >> 15);  // -0.168736*r + -0.331264*g + 0.5*b
                    v = (uint8_t)(((r << 14) - (g * 13682) - (b * 2664)) >> 15);  // 0.5*r + -0.418688*g + -0.081312*b
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, imlib_yuv_to_rgb(fast_floorf(s * hist[y]), u,v));
                }
            }

            fb_free();
            break;
        }
        default: {
            break;
        }
    }
}

// ksize == 0 -> 1x1 kernel
// ksize == 1 -> 3x3 kernel
// ...
// ksize == n -> ((n*2)+1)x((n*2)+1) kernel
//
// To speed up this filter, we can help in two ways:
// 1) For the 'center portion' of the image area, we don't need to check
//   the x+y values against the boundary conditions on each pixel.
// 2) In that same region we can take advantage of the filter property being
//   the sum of all of the pixels by subtracting the last left edge values
//   and adding the new right edge values instead of re-calculating the sum
//   of every pixel. This will allow very large filters to be used without
//   much change in performance.
//
#ifdef IMLIB_ENABLE_MEAN
void imlib_mean_filter(image_t *img, const int ksize, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;

    int32_t over32_n = 65536 / (((ksize*2)+1)*((ksize*2)+1));

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                int pixel, acc = 0;
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y + j);
                            acc -= IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x - ksize - 1);
                            acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x + ksize);
                        }
                    } else {
                        acc = 0;

                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }

                    pixel = (int)((acc * over32_n)>>16);

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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
                int pixel, acc = 0;
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }
                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j<= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y+j);
                            acc -= IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x-ksize-1);
                            acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x+ksize); 
                        }
                    } else {
                        acc = 0;
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                                for (int k = -ksize; k <= ksize; k++) {
                                acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }

                    pixel = (int)((acc * over32_n)>>16);

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            int pixel, r, g, b, r_acc, g_acc, b_acc;
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));

                r_acc = g_acc = b_acc = 0;
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }
                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                       for (int j= -ksize; j<=ksize; j++) {
                           uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y+j);
                           // subtract last left-most pixel from the sums
                           pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x-ksize-1);
                           r_acc -= COLOR_RGB565_TO_R5(pixel);
                           g_acc -= COLOR_RGB565_TO_G6(pixel);
                           b_acc -= COLOR_RGB565_TO_B5(pixel);
                           // add new right edge pixel to the sums
                           pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x+ksize);
                           r_acc += COLOR_RGB565_TO_R5(pixel);
                           g_acc += COLOR_RGB565_TO_G6(pixel);
                           b_acc += COLOR_RGB565_TO_B5(pixel);
                        }
                    } else { // check bounds and do full sum calculations
                        r_acc = g_acc = b_acc = 0;
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                            IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                r_acc += COLOR_RGB565_TO_R5(pixel);
                                g_acc += COLOR_RGB565_TO_G6(pixel);
                                b_acc += COLOR_RGB565_TO_B5(pixel);
                            }
                        }
                    }
                    int pixel;
                    r = (int)((r_acc * over32_n)>>16);
                    g = (int)((g_acc * over32_n)>>16);
                    b = (int)((b_acc * over32_n)>>16);
                    pixel = COLOR_R5_G6_B5_TO_RGB565(r, g, b);
                     
                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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
#endif // IMLIB_ENABLE_MEAN

#ifdef IMLIB_ENABLE_MEDIAN
static uint8_t hist_median(uint8_t *data, int len, const int cutoff)
{
int i;
#if defined(ARM_MATH_CM7) || defined(ARM_MATH_CM4)
uint32_t oldsum=0, sum32 = 0;

    for (i=0; i<len; i+=4) { // work 4 at time with SIMD
        sum32 = __USADA8(*(uint32_t *)&data[i], 0, sum32);
        if (sum32 >= cutoff) { // within this group
            while (oldsum < cutoff && i < len)
                oldsum += data[i++];
            break;
        } // if we're at the last 4 values
        oldsum = sum32;
    } // for each group of 4 elements
#else // generic C version
int sum = 0;
    for (i=0; i<len && sum < cutoff; i++) {
        sum += data[i];
    }
#endif
    return i-1;
} /* hist_median() */

void imlib_median_filter(image_t *img, const int ksize, float percentile, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;

    const int n = ((ksize*2)+1)*((ksize*2)+1);
    const int median_cutoff = fast_floorf(percentile * (float)n);

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            int sum = 0;

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }
                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y <img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y+j);
                            sum -= IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x-ksize-1);
                            sum += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr, x+ksize);
                        }
                    } else {
                        sum = 0;
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                sum += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }
                    int pixel = (sum >= median_cutoff);

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            uint8_t *data = fb_alloc(64, FB_ALLOC_NO_HINT);
            uint8_t pixel;
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        // update histogram edges
                        for (int j=-ksize; j<= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y+j);
                            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x-ksize-1);
                            data[pixel >> 2]--; // remove old pixels
                            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x+ksize);
                            data[pixel >> 2]++; // add new pixels                            
                        } // for j
                    } else { // slow way
                        memset(data, 0, 64);
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            data[pixel >> 2]++;
                            }
                        }
                    }

                    pixel = hist_median(data, 64, median_cutoff); // find the median
                    pixel <<= 2; // scale it back up
                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            uint8_t *r_data = fb_alloc(32, FB_ALLOC_NO_HINT);
            uint8_t *g_data = fb_alloc(64, FB_ALLOC_NO_HINT);
            uint8_t *b_data = fb_alloc(32, FB_ALLOC_NO_HINT);
            uint8_t r, g, b;
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }


                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y+j);
                            int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x-ksize-1);
                            r_data[COLOR_RGB565_TO_R5(pixel)]--; // remove left pixel
                            g_data[COLOR_RGB565_TO_G6(pixel)]--;
                            b_data[COLOR_RGB565_TO_B5(pixel)]--;
                            pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x+ksize);
                            r_data[COLOR_RGB565_TO_R5(pixel)]++; // add right pixel
                            g_data[COLOR_RGB565_TO_G6(pixel)]++;
                            b_data[COLOR_RGB565_TO_B5(pixel)]++;
                        }
                    } else { // need to check bounds
                        memset(r_data, 0, 32); memset(g_data, 0, 64); memset(b_data, 0, 32);
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                r_data[COLOR_RGB565_TO_R5(pixel)]++;
                                g_data[COLOR_RGB565_TO_G6(pixel)]++;
                                b_data[COLOR_RGB565_TO_B5(pixel)]++;
                            }
                        }
                    }

                    r = hist_median(r_data, 32, median_cutoff);
                    g = hist_median(g_data, 64, median_cutoff);
                    b = hist_median(b_data, 32, median_cutoff);

                    int pixel = COLOR_R5_G6_B5_TO_RGB565(r, g, b);

                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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
#endif // IMLIB_ENABLE_MEDIAN

#ifdef IMLIB_ENABLE_MODE
static uint8_t find_mode(uint8_t *bins, int len)
{
int i, j;
uint8_t mode = 0, mcount = 0;
    for(i=0; i<len; i+=4) {
        if (*(uint32_t *)&bins[i] == 0)
            continue; // skip empty bins quickly
        for (j=i; j<i+4; j++) {
            if (bins[j] > mcount) {
                mcount = bins[j];
                mode = j;
            }
        }
    }
    return mode;
} /* find_mode() */

void imlib_mode_filter(image_t *img, const int ksize, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;
    const uint8_t n2 = (((ksize*2)+1)*((ksize*2)+1))/2;
    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            int bins = 0;

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }
                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,y+j);
                                bins -= IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x-ksize-1);
                                bins += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x+ksize);
                        }
                    } else {
                        bins = 0;
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));
                            for (int k = -ksize; k <= ksize; k++) {
                                bins += IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }

                    uint8_t pixel = (bins > n2);

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            fb_free();
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            buf.data = fb_alloc(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            uint8_t *bins = fb_alloc((COLOR_GRAYSCALE_MAX-COLOR_GRAYSCALE_MIN+1), FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));
                uint8_t pixel = 0, mode = 0;
                int mcount = -1;
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }
                    if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t m, *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,y+j);
                            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x-ksize-1);
                            m = --bins[pixel];
                            if (pixel == mode) {
                                if (m < n2)
                                    mcount = 256; // need to search later
                                else
                                    mcount = m; // we're still the mode
                            }
                            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x+ksize);
                            m = ++bins[pixel];
                            if (m > mcount) {
                                mcount = m;
                                mode = pixel;
                            }
                        }
                        if (mcount == 256) { // need to find max
                            mode = find_mode(bins, 256);
                            mcount = bins[mode];
                        }
                    } else { // slow way
                        mcount = -1;
                        memset(bins, 0, (COLOR_GRAYSCALE_MAX-COLOR_GRAYSCALE_MIN+1));
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                bins[pixel]++;

                                if (bins[pixel] > mcount) {
                                    mcount = bins[pixel];
                                    mode = pixel;
                                }
                            }
                        }
                    }
                    pixel = mode;

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            uint8_t *r_bins = fb_alloc((COLOR_R5_MAX-COLOR_R5_MIN+1), FB_ALLOC_NO_HINT);
            uint8_t *g_bins = fb_alloc((COLOR_G6_MAX-COLOR_G6_MIN+1), FB_ALLOC_NO_HINT);
            uint8_t *b_bins = fb_alloc((COLOR_B5_MAX-COLOR_B5_MIN+1), FB_ALLOC_NO_HINT);
            int r_pixel, g_pixel, b_pixel;

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));
                int r_mcount=0, g_mcount=0, b_mcount=0;
                int pixel, r_mode, g_mode, b_mode;
                r_mode = g_mode = b_mode = 0;
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                if (!mask && x > ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                    for (int j = -ksize; j <= ksize; j++) {
                        uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y + j);
                        pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x - ksize -1);
                        r_pixel = COLOR_RGB565_TO_R5(pixel);
                        g_pixel = COLOR_RGB565_TO_G6(pixel);
                        b_pixel = COLOR_RGB565_TO_B5(pixel);
                        r_bins[r_pixel]--;
                        g_bins[g_pixel]--;
                        b_bins[b_pixel]--;
                        if (r_pixel == r_mode) {
                            if (r_bins[r_pixel] < n2)
                                r_mcount = 256; // need to search later
                            else
                                r_mcount = r_bins[r_pixel]; // we're still the mode
                        }
                        if (g_pixel == g_mode) {
                            if (g_bins[g_pixel] < n2)
                                g_mcount = 256; // need to search later
                            else
                                g_mcount = g_bins[g_pixel]; // we're still the mode
                        }
                        if (b_pixel == b_mode) {
                            if (b_bins[b_pixel] < n2)
                                b_mcount = 256; // need to search later
                            else
                                b_mcount = b_bins[b_pixel]; // we're still the mode
                        }

                        pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr, x + ksize);
                        r_pixel = COLOR_RGB565_TO_R5(pixel);
                        g_pixel = COLOR_RGB565_TO_G6(pixel);
                        b_pixel = COLOR_RGB565_TO_B5(pixel);
                        r_bins[r_pixel]++;
                        g_bins[g_pixel]++;
                        b_bins[b_pixel]++;
                        if (r_bins[r_pixel] > r_mcount) {
                            r_mcount = r_bins[r_pixel];
                            r_mode = r_pixel;
                        }

                        if (g_bins[g_pixel] > g_mcount) {
                            g_mcount = g_bins[g_pixel];
                            g_mode = g_pixel;
                        }

                        if (b_bins[b_pixel] > b_mcount) {
                            b_mcount = b_bins[b_pixel];
                            b_mode = b_pixel;
                        }
                    } // for j
                    if (r_mcount == 256) { // need to find max
                        r_mode = find_mode(r_bins, 32);
                        r_mcount = r_bins[r_mode];
                    }
                    if (g_mcount == 256) { // need to find max
                        g_mode = find_mode(g_bins, 64);
                        g_mcount = g_bins[g_mode];
                    }
                    if (b_mcount == 256) { // need to find max
                        b_mode = find_mode(b_bins, 32);
                        b_mcount = r_bins[b_mode];
                    }
                } else { // slower way 
                    memset(r_bins, 0, (COLOR_R5_MAX-COLOR_R5_MIN+1));
                    memset(g_bins, 0, (COLOR_G6_MAX-COLOR_G6_MIN+1));
                    memset(b_bins, 0, (COLOR_B5_MAX-COLOR_B5_MIN+1));
                    r_mcount = g_mcount = b_mcount = 0;
                    for (int j = -ksize; j <= ksize; j++) {
                        uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                            IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                        for (int k = -ksize; k <= ksize; k++) {
                            pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            r_pixel = COLOR_RGB565_TO_R5(pixel);
                            g_pixel = COLOR_RGB565_TO_G6(pixel);
                            b_pixel = COLOR_RGB565_TO_B5(pixel);
                            r_bins[r_pixel]++;
                            g_bins[g_pixel]++;
                            b_bins[b_pixel]++;

                            if (r_bins[r_pixel] > r_mcount) {
                                r_mcount = r_bins[r_pixel];
                                r_mode = r_pixel;
                            }

                            if (g_bins[g_pixel] > g_mcount) {
                                g_mcount = g_bins[g_pixel];
                                g_mode = g_pixel;
                            }

                            if (b_bins[b_pixel] > b_mcount) {
                                b_mcount = b_bins[b_pixel];
                                b_mode = b_pixel;
                            }
                        } // for k
                    } // for j
                    } // slow/fast way
                    pixel = COLOR_R5_G6_B5_TO_RGB565(r_mode, g_mode, b_mode);
                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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
#endif // IMLIB_ENABLE_MODE

#ifdef IMLIB_ENABLE_MIDPOINT
void imlib_midpoint_filter(image_t *img, const int ksize, float bias, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;
    uint8_t *u8BiasTable;
    float max_bias = bias, min_bias = 1.0f - bias;

    u8BiasTable = fb_alloc(256, FB_ALLOC_NO_HINT);
    for (int i=0; i<256; i++) {
        u8BiasTable[i] = (uint8_t)fast_floorf((float)i * bias);
    }

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int min = COLOR_BINARY_MAX, max = COLOR_BINARY_MIN;

                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x+k);
                                min &= pixel;
                                max |= pixel;
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                min &= pixel;
                                max |= pixel;
                            }
                        }
                    }

                    int pixel = fast_floorf((min*min_bias)+(max*max_bias));

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int min = COLOR_GRAYSCALE_MAX, max = COLOR_GRAYSCALE_MIN;
                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x+k);
                                if (pixel < min) min = pixel;
                                else if (pixel > max) max = pixel;
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                if (pixel < min) min = pixel;
                                else if (pixel > max) max = pixel;
                            }
                        }
                    }

                    int pixel = min + u8BiasTable[max-min];

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int r_min = COLOR_R5_MAX, r_max = COLOR_R5_MIN;
                    int g_min = COLOR_G6_MAX, g_max = COLOR_G6_MIN;
                    int b_min = COLOR_B5_MAX, b_max = COLOR_B5_MIN;
                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,x+k);
                                int r_pixel = COLOR_RGB565_TO_R5(pixel);
                                int g_pixel = COLOR_RGB565_TO_G6(pixel);
                                int b_pixel = COLOR_RGB565_TO_B5(pixel);
                                if (r_pixel < r_min) r_min = r_pixel;
                                else if (r_pixel > r_max) r_max = r_pixel;
                                if (g_pixel < g_min) g_min = g_pixel;
                                else if (g_pixel > g_max) g_max = g_pixel;
                                if (b_pixel < b_min) b_min = b_pixel;
                                else if (b_pixel > b_max) b_max = b_pixel;
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                int r_pixel = COLOR_RGB565_TO_R5(pixel);
                                int g_pixel = COLOR_RGB565_TO_G6(pixel);
                                int b_pixel = COLOR_RGB565_TO_B5(pixel);
                                if (r_pixel < r_min) r_min = r_pixel;
                                else if (r_pixel > r_max) r_max = r_pixel;
                                if (g_pixel < g_min) g_min = g_pixel;
                                else if (g_pixel > g_max) g_max = g_pixel;
                                if (b_pixel < b_min) b_min = b_pixel;
                                else if (b_pixel > b_max) b_max = b_pixel;
                            }
                        }
                    }

                    r_min += u8BiasTable[r_max-r_min];
                    g_min += u8BiasTable[g_max-g_min];
                    b_min += u8BiasTable[b_max-b_min];
                    int pixel = COLOR_R5_G6_B5_TO_RGB565(r_min, g_min, b_min);

                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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
    fb_free();
}
#endif // IMLIB_ENABLE_MIDPOINT

// http://www.fmwconcepts.com/imagemagick/digital_image_filtering.pdf

void imlib_morph(image_t *img, const int ksize, const int *krn, const float m, const int b, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;
    const int32_t m_int = (int32_t)(65536.0 * m); // m is 1/kernel_weight

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int acc = 0, ptr = 0;

                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                acc += krn[ptr++] * IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,x+k);
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += krn[ptr++] * IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }
                    int32_t tmp = (acc * m_int) >> 16;
                    int pixel = tmp + b;
                    if (pixel < 0) pixel = 0;
                    else if (pixel > 1) pixel = 1;

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int32_t acc = 0, ptr = 0;

                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                acc += krn[ptr++] * IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,x+k);
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                acc += krn[ptr++] * IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            }
                        }
                    }
                    int32_t tmp = (acc * m_int)>>16;
                    int pixel = tmp + b;
                    if (pixel > COLOR_GRAYSCALE_MAX) pixel = COLOR_GRAYSCALE_MAX;
                    else if (pixel < 0) pixel = 0;

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int32_t tmp, r_acc = 0, g_acc = 0, b_acc = 0, ptr = 0;

                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,x+k);
                                r_acc += krn[ptr] * COLOR_RGB565_TO_R5(pixel);
                                g_acc += krn[ptr] * COLOR_RGB565_TO_G6(pixel);
                                b_acc += krn[ptr++] * COLOR_RGB565_TO_B5(pixel);
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                r_acc += krn[ptr] * COLOR_RGB565_TO_R5(pixel);
                                g_acc += krn[ptr] * COLOR_RGB565_TO_G6(pixel);
                                b_acc += krn[ptr++] * COLOR_RGB565_TO_B5(pixel);
                            }
                        }
                    }
                    tmp = (r_acc * m_int) >> 16;
                    r_acc = tmp + b;
                    if (r_acc > COLOR_R5_MAX) r_acc = COLOR_R5_MAX;
                    else if (r_acc < 0) r_acc = 0;
                    tmp = (g_acc * m_int) >> 16;
                    g_acc = tmp + b;
                    if (g_acc > COLOR_G6_MAX) g_acc = COLOR_G6_MAX;
                    else if (g_acc < 0) g_acc = 0;
                    tmp = (b_acc * m_int) >> 16;
                    b_acc = tmp + b;
                    if (b_acc > COLOR_B5_MAX) b_acc = COLOR_B5_MAX;
                    else if (b_acc < 0) b_acc = 0;
                    
                    int pixel = COLOR_R5_G6_B5_TO_RGB565(r_acc, g_acc, b_acc);

                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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

#ifdef IMLIB_ENABLE_BILATERAL
static float gaussian(float x, float sigma)
{
    return fast_expf((x * x) / (-2.0f * sigma * sigma)) / (fabsf(sigma) * 2.506628f); // sqrt(2 * PI)
}

static float distance(int x, int y)
{
    return fast_sqrtf((x * x) + (y * y));
}

void imlib_bilateral_filter(image_t *img, const int ksize, float color_sigma, float space_sigma, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            buf.data = fb_alloc(IMAGE_BINARY_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            float *gi_lut_ptr = fb_alloc((COLOR_BINARY_MAX - COLOR_BINARY_MIN + 1) * sizeof(float) *2, FB_ALLOC_NO_HINT);
            float *gi_lut = &gi_lut_ptr[1];
            float max_color = IM_DIV(1.0f, COLOR_BINARY_MAX - COLOR_BINARY_MIN);
            for (int i = COLOR_BINARY_MIN; i <= COLOR_BINARY_MAX; i++) {
                gi_lut[-i] = gi_lut[i] = gaussian(i * max_color, color_sigma);
            }

            int n = (ksize * 2) + 1;
            float *gs_lut = fb_alloc(n * n * sizeof(float), FB_ALLOC_NO_HINT);

            float max_space = IM_DIV(1.0f, distance(ksize, ksize));
            for (int y = -ksize; y <= ksize; y++) {
                for (int x = -ksize; x <= ksize; x++) {
                    gs_lut[(n * (y + ksize)) + (x + ksize)] = gaussian(distance(x, y) * max_space, space_sigma);
                }
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *buf_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int this_pixel = IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x);
                    float i_acc = 0, w_acc = 0;
                    int ptr = 0;
                    for (int j = -ksize; j <= ksize; j++) {
                        uint32_t *k_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img,
                            IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                        for (int k = -ksize; k <= ksize; k++) {
                            int pixel = IMAGE_GET_BINARY_PIXEL_FAST(k_row_ptr,
                                IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                            float w = gi_lut[(this_pixel - pixel)] * gs_lut[ptr++];
                            i_acc += pixel * w;
                            w_acc += w;
                        }
                    }

                    int pixel = fast_floorf(IM_MIN(IM_DIV(i_acc, w_acc), COLOR_BINARY_MAX));

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_BINARY_MAX;
                        } else {
                            pixel = COLOR_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_BINARY_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            fb_free();
            fb_free();
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            buf.data = fb_alloc(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            float *gi_lut_ptr = fb_alloc((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN + 1) * sizeof(float) * 2, FB_ALLOC_NO_HINT);
            float *gi_lut = &gi_lut_ptr[256]; // point to the middle
            float max_color = IM_DIV(1.0f, COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN);
            for (int i = COLOR_GRAYSCALE_MIN; i <= COLOR_GRAYSCALE_MAX; i++) {
                gi_lut[-i] = gi_lut[i] = gaussian(i * max_color, color_sigma);
            }

            int n = (ksize * 2) + 1;
            float *gs_lut = fb_alloc(n * n * sizeof(float), FB_ALLOC_NO_HINT);

            float max_space = IM_DIV(1.0f, distance(ksize, ksize));
            for (int y = -ksize; y <= ksize; y++) {
                for (int x = -ksize; x <= ksize; x++) {
                    gs_lut[(n * (y + ksize)) + (x + ksize)] = gaussian(distance(x, y) * max_space, space_sigma);
                }
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int this_pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    float i_acc = 0, w_acc = 0;
                    int ptr = 0;
                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr, x+k);
                                float w = gi_lut[this_pixel - pixel] * gs_lut[ptr++];
                                i_acc += pixel * w;
                                w_acc += w;
                            }
                        }
                    } else {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                float w = gi_lut[(this_pixel - pixel)] * gs_lut[ptr++];
                                i_acc += pixel * w;
                                w_acc += w;
                            }
                        }
                    }

                    int pixel = fast_floorf(IM_MIN(IM_DIV(i_acc, w_acc), COLOR_GRAYSCALE_MAX));

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, pixel);
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
            fb_free();
            fb_free();
            break;
        }
        case IMAGE_BPP_RGB565: {
            buf.data = fb_alloc(IMAGE_RGB565_LINE_LEN_BYTES(img) * brows, FB_ALLOC_NO_HINT);
            float *rb_gi_ptr = fb_alloc((COLOR_R5_MAX - COLOR_R5_MIN + 1) * sizeof(float) *2, FB_ALLOC_NO_HINT);
            float *g_gi_ptr = fb_alloc((COLOR_G6_MAX - COLOR_G6_MIN + 1) * sizeof(float) *2, FB_ALLOC_NO_HINT);
            float *rb_gi_lut = &rb_gi_ptr[32]; // center
            float *g_gi_lut = &g_gi_ptr[64];

            float r_max_color = IM_DIV(1.0f, COLOR_R5_MAX - COLOR_R5_MIN);
            for (int i = COLOR_R5_MIN; i <= COLOR_R5_MAX; i++) {
                rb_gi_lut[-i] = rb_gi_lut[i] = gaussian(i * r_max_color, color_sigma);
            }

            float g_max_color = IM_DIV(1.0f, COLOR_G6_MAX - COLOR_G6_MIN);
            for (int i = COLOR_G6_MIN; i <= COLOR_G6_MAX; i++) {
                g_gi_lut[-i] = g_gi_lut[i] = gaussian(i * g_max_color, color_sigma);
            }

            int n = (ksize * 2) + 1;
            float *gs_lut = fb_alloc(n * n * sizeof(float), FB_ALLOC_NO_HINT);

            float max_space = IM_DIV(1.0f, distance(ksize, ksize));
            for (int y = -ksize; y <= ksize; y++) {
                for (int x = -ksize; x <= ksize; x++) {
                    gs_lut[(n * (y + ksize)) + (x + ksize)] = gaussian(distance(x, y) * max_space, space_sigma);
                }
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint16_t *buf_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int this_pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    int r_this_pixel = COLOR_RGB565_TO_R5(this_pixel);
                    int g_this_pixel = COLOR_RGB565_TO_G6(this_pixel);
                    int b_this_pixel = COLOR_RGB565_TO_B5(this_pixel);
                    float r_i_acc = 0, r_w_acc = 0;
                    float g_i_acc = 0, g_w_acc = 0;
                    float b_i_acc = 0, b_w_acc = 0;
                    int ptr = 0;
                    if (x >= ksize && x < img->w-ksize && y >= ksize && y < img->h-ksize) {
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,y+j);
                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,x+k);
                                int r_pixel = COLOR_RGB565_TO_R5(pixel);
                                int g_pixel = COLOR_RGB565_TO_G6(pixel);
                                int b_pixel = COLOR_RGB565_TO_B5(pixel);
                                float gs = gs_lut[ptr++];
                                float r_w = rb_gi_lut[(r_this_pixel - r_pixel)] * gs;
                                float g_w = g_gi_lut[(g_this_pixel - g_pixel)] * gs;
                                float b_w = rb_gi_lut[(b_this_pixel - b_pixel)] * gs;
                                r_i_acc += r_pixel * r_w;
                                r_w_acc += r_w;
                                g_i_acc += g_pixel * g_w;
                                g_w_acc += g_w;
                                b_i_acc += b_pixel * b_w;
                                b_w_acc += b_w;
                            }
                        }
                    } else { // check boundary conditions
                        for (int j = -ksize; j <= ksize; j++) {
                            uint16_t *k_row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img,
                                IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                            for (int k = -ksize; k <= ksize; k++) {
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(k_row_ptr,
                                    IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                                int r_pixel = COLOR_RGB565_TO_R5(pixel);
                                int g_pixel = COLOR_RGB565_TO_G6(pixel);
                                int b_pixel = COLOR_RGB565_TO_B5(pixel);
                                float gs = gs_lut[ptr++];
                                float r_w = rb_gi_lut[(r_this_pixel - r_pixel)] * gs;
                                float g_w = g_gi_lut[(g_this_pixel - g_pixel)] * gs;
                                float b_w = rb_gi_lut[(b_this_pixel - b_pixel)] * gs;
                                r_i_acc += r_pixel * r_w;
                                r_w_acc += r_w;
                                g_i_acc += g_pixel * g_w;
                                g_w_acc += g_w;
                                b_i_acc += b_pixel * b_w;
                                b_w_acc += b_w;
                            }
                        }
                    }

                    int pixel = COLOR_R5_G6_B5_TO_RGB565(fast_floorf(IM_MIN(IM_DIV(r_i_acc, r_w_acc), COLOR_R5_MAX)),
                                                         fast_floorf(IM_MIN(IM_DIV(g_i_acc, g_w_acc), COLOR_G6_MAX)),
                                                         fast_floorf(IM_MIN(IM_DIV(b_i_acc, b_w_acc), COLOR_B5_MAX)));

                    if (threshold) {
                        if (((COLOR_RGB565_TO_Y(pixel) - offset) < COLOR_RGB565_TO_Y(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x))) ^ invert) {
                            pixel = COLOR_RGB565_BINARY_MAX;
                        } else {
                            pixel = COLOR_RGB565_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_RGB565_PIXEL_FAST(buf_row_ptr, x, pixel);
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
#endif // IMLIB_ENABLE_BILATERAL

#ifdef IMLIB_ENABLE_CARTOON
typedef struct imlib_cartoon_filter_mean_state {
    int r_acc, g_acc, b_acc, pixels;
} imlib_cartoon_filter_mean_state_t;

static void imlib_cartoon_filter_mean(image_t *img, int line, int l, int r, void *data)
{
    imlib_cartoon_filter_mean_state_t *state = (imlib_cartoon_filter_mean_state_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                state->g_acc += IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, i);
                state->pixels += 1;
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                state->g_acc += IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, i);
                state->pixels += 1;
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, i);
                state->r_acc += COLOR_RGB565_TO_R5(pixel);
                state->g_acc += COLOR_RGB565_TO_G6(pixel);
                state->b_acc += COLOR_RGB565_TO_B5(pixel);
                state->pixels += 1;
            }
            break;
        }
        default: {
            break;
        }
    }
}

typedef struct imlib_cartoon_filter_fill_state {
    int mean;
} imlib_cartoon_filter_fill_state_t;

static void imlib_cartoon_filter_fill(image_t *img, int line, int l, int r, void *data)
{
    imlib_cartoon_filter_fill_state_t *state = (imlib_cartoon_filter_fill_state_t *) data;

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, i, state->mean);
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, i, state->mean);
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = l; i <= r; i++) {
                IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, i, state->mean);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_cartoon_filter(image_t *img, float seed_threshold, float floating_threshold, image_t *mask)
{
    image_t mean_image, fill_image;

    mean_image.w = img->w;
    mean_image.h = img->h;
    mean_image.bpp = IMAGE_BPP_BINARY;
    mean_image.data = fb_alloc0(image_size(&mean_image), FB_ALLOC_NO_HINT);

    fill_image.w = img->w;
    fill_image.h = img->h;
    fill_image.bpp = IMAGE_BPP_BINARY;
    fill_image.data = fb_alloc0(image_size(&fill_image), FB_ALLOC_NO_HINT);

    if (mask) {
        for (int y = 0, yy = fill_image.h; y < yy; y++) {
            uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&fill_image, y);
            for (int x = 0, xx = fill_image.w; x < xx; x++) {
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

    for (int y = 0, yy = img->h; y < yy; y++) {
        uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&mean_image, y);
        for (int x = 0, xx = img->w; x < xx; x++) {
            if (!IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)) {

                imlib_cartoon_filter_mean_state_t mean_state;
                memset(&mean_state, 0, sizeof(imlib_cartoon_filter_mean_state_t));
                imlib_flood_fill_int(&mean_image, img, x, y, color_seed_threshold, color_floating_threshold,
                                     imlib_cartoon_filter_mean, &mean_state);

                imlib_cartoon_filter_fill_state_t fill_state;
                memset(&fill_state, 0, sizeof(imlib_cartoon_filter_fill_state_t));

                switch(img->bpp) {
                    case IMAGE_BPP_BINARY: {
                        fill_state.mean = mean_state.g_acc / mean_state.pixels;
                        break;
                    }
                    case IMAGE_BPP_GRAYSCALE: {
                        fill_state.mean = mean_state.g_acc / mean_state.pixels;
                        break;
                    }
                    case IMAGE_BPP_RGB565: {
                        fill_state.mean = COLOR_R5_G6_B5_TO_RGB565(mean_state.r_acc / mean_state.pixels,
                                                                   mean_state.g_acc / mean_state.pixels,
                                                                   mean_state.b_acc / mean_state.pixels);
                        break;
                    }
                    default: {
                        break;
                    }
                }

                imlib_flood_fill_int(&fill_image, img, x, y, color_seed_threshold, color_floating_threshold,
                                     imlib_cartoon_filter_fill, &fill_state);
            }
        }
    }

    fb_free();
    fb_free();
}
#endif // IMLIB_ENABLE_CARTOON
