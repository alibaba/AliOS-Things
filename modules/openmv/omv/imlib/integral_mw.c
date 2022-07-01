/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * An integral image using a moving window.
 *
 * The high level steps are:
 *  1) Start with an array of pointers[n] where n = feature height.
 *  2) Compute the first n lines of the integral image.
 *  3) Do some processing with the integral image.
 *  4) Call integral_mw_image_shift(n)
 *
 *  This will shift the pointers by n and calculate n new lines, example:
 *  Assuming feature height is 4:
 *      mw_i_image[0] -> mem[0]
 *      mw_i_image[1] -> mem[1]
 *      mw_i_image[2] -> mem[2]
 *      mw_i_image[3] -> mem[3]
 *
 *  After shifting by 1 line, it looks like this:
 *      mw_i_image[0] -> mem[1]
 *      mw_i_image[1] -> mem[2]
 *      mw_i_image[2] -> mem[3]
 *      mw_i_image[3] -> mem[0]
 *  Line 3 will be computed as normal using line 2 which now
 *  points to the last integral image line computed initially.
 *
 *  After shifting by second line, it looks like this:
 *      mw_i_image[0] -> mem[2]
 *      mw_i_image[1] -> mem[3]
 *      mw_i_image[2] -> mem[0]
 *      mw_i_image[3] -> mem[1]
 *  Line 3 will be computed as usual using line 2 which now
 *  points to the last integral image line computed in the previous shift.
 *
 * Notes:
 *  The mw integral must Not be shifted more than image_height - feature_height, s_lines
 *  must be < feature_height-1 to keep at least one row for integral image calculations.
 *
 *  This only requires (image_width * (feature_height+1) * 4) bytes. Assuming a 24x24
 *  feature, the required memory is 320*25*4 (i.e. ~32KBs) instead of 320*240*4 (300KBs).
 *
 *  Functions without a suffix compute/shift summed images, _sq suffix compute/shift
 *  summed squared images, and _ss compute/shift both summed and squared in a single pass.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "imlib.h"
#include "fb_alloc.h"

// This macro swaps two pointers.
#define SWAP_PTRS(a, b)  \
    ({ __typeof__ (a) _t;\
       (_t) = ( a);  \
       ( a) = ( b);  \
       ( b) = (_t); })

void imlib_integral_mw_alloc(mw_image_t *sum, int w, int h)
{
    sum->w = w;
    sum->h = h;
    sum->y_offs = 0;
    sum->x_ratio = (1<<16)+1;
    sum->y_ratio = (1<<16)+1;
    sum->data = fb_alloc(h * sizeof(*sum->data), FB_ALLOC_NO_HINT);
    // swap is used when shifting the image pointers
    // to avoid overwriting the image rows in sum->data
    sum->swap = fb_alloc(h * sizeof(*sum->data), FB_ALLOC_NO_HINT);

    for (int i=0; i<h; i++) {
        sum->data[i] = fb_alloc(w * sizeof(**sum->data), FB_ALLOC_NO_HINT);
    }
}

void imlib_integral_mw_free(mw_image_t *sum)
{
    for (int i=0; i<sum->h; i++) {
        fb_free();  // Free h lines
    }
    fb_free();  // Free data
    fb_free();  // Free swap
}

void imlib_integral_mw_scale(rectangle_t *roi, mw_image_t *sum, int w, int h)
{
    // Set new width
    // Note: height doesn't change
    sum->w = w;
    // Reset y offset
    sum->y_offs = 0;
    // Set scaling ratios
    sum->x_ratio = (int)((roi->w<<16)/w)+1;
    sum->y_ratio = (int)((roi->h<<16)/h)+1;
}

void imlib_integral_mw(image_t *src, mw_image_t *sum)
{
    // Image pointers
    typeof(*sum->data) *sum_data = sum->data;

    // Compute the first row to avoid branching
    for (int sx, s=0, x=0; x<sum->w; x++) {
        // X offset
        sx = (x*sum->x_ratio)>>16;

        // Accumulate row data
        s += IM_TO_GS_PIXEL(src, sx, 0);
        sum_data[0][x] = s;
    }

    // Compute the remaining rows
    for (int sy, y=1; y<sum->h; y++) {
        // Y offset
        sy = (y*sum->y_ratio)>>16;

        // Sum the current row
        for (int sx, s=0, x=0; x<sum->w; x++) {
            // X offset
            sx = (x*sum->x_ratio)>>16;

            // Accumulate row data
            s += IM_TO_GS_PIXEL(src, sx, sy);
            sum_data[y][x] = s + sum_data[y-1][x];
        }
    }

    sum->y_offs = sum->h;
}

void imlib_integral_mw_sq(image_t *src, mw_image_t *sum)
{
    // Image pointers
    typeof(*sum->data) *sum_data = sum->data;

    // Compute the first row to avoid branching
    for (int sx, s=0, x=0; x<sum->w; x++) {
        // X offset
        sx = (x*sum->x_ratio)>>16;

        // Accumulate row data
        s += IM_TO_GS_PIXEL(src, sx, 0) * IM_TO_GS_PIXEL(src, sx, 0);
        sum_data[0][x] = s;
    }

    // Compute the remaining rows
    for (int sy, y=1; y<sum->h; y++) {
        // Y offset
        sy = (y*sum->y_ratio)>>16;

        // Sum the current row
        for (int sx, s=0, x=0; x<sum->w; x++) {
            // X offset
            sx = (x*sum->x_ratio)>>16;

            // Accumulate row data
            s += IM_TO_GS_PIXEL(src, sx, sy) * IM_TO_GS_PIXEL(src, sx, sy);
            sum_data[y][x] = s + sum_data[y-1][x];
        }
    }

    sum->y_offs = sum->h;
}

void imlib_integral_mw_shift(image_t *src, mw_image_t *sum, int n)
{
    // Shift integral image rows by n lines
    for (int y=0; y<sum->h; y++) {
        sum->swap[y] = sum->data[(y+n) % sum->h];
    }

    // Swap the data and swap pointers
    SWAP_PTRS(sum->data, sum->swap);

    // Pointer to the current sum data
    typeof(*sum->data) *sum_data = sum->data;

    // Compute the last n lines
    for (int sy, y=(sum->h - n); y<sum->h; y++, sum->y_offs++) {
        // Y offset
        sy = (sum->y_offs*sum->y_ratio)>>16;

        // Sum the current row
        for (int sx, s=0, x=0; x<sum->w; x++) {
            // X offset
            sx = (x*sum->x_ratio)>>16;

            // Accumulate row data
            s += IM_TO_GS_PIXEL(src, sx, sy);
            sum_data[y][x] = s + sum_data[y-1][x];
        }
    }
}

void imlib_integral_mw_shift_sq(image_t *src, mw_image_t *sum, int n)
{
    // Shift integral image rows by n lines
    for (int y=0; y<sum->h; y++) {
        sum->swap[y] = sum->data[(y+n) % sum->h];
    }

    // Swap data and swap pointers
    SWAP_PTRS(sum->data, sum->swap);

    // Pointer to the current sum data
    typeof(*sum->data) *sum_data = sum->data;

    // Compute the last n lines
    for (int sy, y=(sum->h - n); y<sum->h; y++, sum->y_offs++) {
        // The y offset is set to the last line + 1
        sy = (sum->y_offs*sum->y_ratio)>>16;

        // Sum the current row
        for (int sx, s=0, x=0; x<sum->w; x++) {
            // X offset
            sx = (x*sum->x_ratio)>>16;

            // Accumulate row data
            s += IM_TO_GS_PIXEL(src, sx, sy) * IM_TO_GS_PIXEL(src, sx, sy);
            sum_data[y][x] = (s + sum_data[y-1][x]);
        }
    }
}

void imlib_integral_mw_ss(image_t *src, mw_image_t *sum, mw_image_t *ssq, rectangle_t *roi)
{
    // Image data pointers
    typeof(*sum->data) *sum_data = sum->data;
    typeof(*sum->data) *ssq_data = ssq->data;

    // Compute the first row to avoid branching
    for (int sx, s=0, sq=0, x=0; x<sum->w; x++) {
        // X offset
        sx = roi->x+((x*sum->x_ratio)>>16);

        // Accumulate row data
        s  += IM_TO_GS_PIXEL(src, sx, roi->y);
        sq += IM_TO_GS_PIXEL(src, sx, roi->y) * IM_TO_GS_PIXEL(src, sx, roi->y);

        sum_data[0][x] = s;
        ssq_data[0][x] = sq;
    }

    // Compute the last n lines
    for (int sy, y=1; y<sum->h; y++) {
        // Y offset
        sy = roi->y+((y*sum->y_ratio)>>16);

        // Sum the current row
        for (int sx, s=0, sq=0, x=0; x<sum->w; x++) {
            // X offset
            sx = roi->x+((x*sum->x_ratio)>>16);

            // Accumulate row data
            s  += IM_TO_GS_PIXEL(src, sx, sy);
            sq += IM_TO_GS_PIXEL(src, sx, sy) * IM_TO_GS_PIXEL(src, sx, sy);

            sum_data[y][x] = s + sum_data[y-1][x];
            ssq_data[y][x] = sq + ssq_data[y-1][x];
        }
    }

    sum->y_offs = sum->h;
    ssq->y_offs = sum->h;
}

void imlib_integral_mw_shift_ss(image_t *src, mw_image_t *sum, mw_image_t *ssq, rectangle_t *roi, int n)
{
    // Shift integral image rows by n lines
    for (int y=0; y<sum->h; y++) {
        sum->swap[y] = sum->data[(y+n) % sum->h];
        ssq->swap[y] = ssq->data[(y+n) % ssq->h];
    }

    // Swap the data and swap pointers
    SWAP_PTRS(sum->data, sum->swap);
    SWAP_PTRS(ssq->data, ssq->swap);

    // Pointer to the current sum and ssq data
    typeof(*sum->data) *sum_data = sum->data;
    typeof(*ssq->data) *ssq_data = ssq->data;

    // Compute the last n lines
    for (int sy, y=(sum->h - n); y<sum->h; y++, sum->y_offs++, ssq->y_offs++) {
        // The y offset is set to the last line + 1
        sy = roi->y+((sum->y_offs*sum->y_ratio)>>16);

        // Sum of the current row
        for (int sx, s=0, sq=0, x=0; x<sum->w; x++) {
            // X offset
            sx = roi->x+((x*sum->x_ratio)>>16);

            // Accumulate row data
            s  += IM_TO_GS_PIXEL(src, sx, sy);
            sq += IM_TO_GS_PIXEL(src, sx, sy) * IM_TO_GS_PIXEL(src, sx, sy);

            sum_data[y][x] = s  + sum_data[y-1][x];
            ssq_data[y][x] = sq + ssq_data[y-1][x];
        }
    }
}

long imlib_integral_mw_lookup(mw_image_t *sum, int x, int y, int w, int h)
{
#define PIXEL_AT(x,y)\
    (sum->data[(y)][x])
    return PIXEL_AT(w+x, h+y) + PIXEL_AT(x, y) - PIXEL_AT(w+x, y) - PIXEL_AT(x, h+y);
#undef  PIXEL_AT
}
