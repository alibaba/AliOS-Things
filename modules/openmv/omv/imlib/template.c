/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Template matching with NCC (Normalized Cross Correlation) using exhaustive and diamond search.
 *
 * References:
 * Briechle, Kai, and Uwe D. Hanebeck. "Template matching using fast normalized cross correlation." Aerospace
 * Lewis, J. P. "Fast normalized cross-correlation."
 * Zhu, Shan, and Kai-Kuang Ma. "A new diamond search algorithm for fast block-matching motion estimation."
 */
#include <stdio.h>
#include <float.h>
#include <limits.h>

#include "imlib.h"
#include "xalloc.h"

static void set_dsp(int cx, int cy, point_t *pts, bool sdsp, int step)
{
    if (sdsp) {
        // Small DSP
        //      4
        //    3 0 1
        //      2
        pts[0].x = cx;
        pts[0].y = cy;

        pts[1].x = cx + step/2;
        pts[1].y = cy;

        pts[2].x = cx;
        pts[2].y = cy + step/2;

        pts[3].x = cx - step/2;
        pts[3].y = cy;

        pts[4].x = cx;
        pts[4].y = cy - step/2;
    } else {
        // Large DSP
        //      7
        //    6   8
        //   5  0  1
        //    4   2
        //      3
        pts[0].x = cx;
        pts[0].y = cy;

        pts[1].x = cx + step;
        pts[1].y = cy;

        pts[2].x = cx + step/2;
        pts[2].y = cy + step/2;

        pts[3].x = cx;
        pts[3].y = cy + step;

        pts[4].x = cx - step/2;
        pts[4].y = cy + step/2;

        pts[5].x = cx - step;
        pts[5].y = cy;

        pts[6].x = cx - step/2;
        pts[6].y = cy - step/2;

        pts[7].x = cx;
        pts[7].y = cy - step;

        pts[8].x = cx + step/2;
        pts[8].y = cy - step/2;

    }
}

static float find_block_ncc(image_t *f, image_t *t, i_image_t *sum, int t_mean, uint32_t t_sumsq, int u, int v)
{
    int w = t->w;
    int h = t->h;

    int num = 0;
    uint32_t f_sumsq=0;

    if (u < 0) {
        u = 0;
    }

    if (v < 0) {
        v = 0;
    }

    if (u+w >= f->w) {
        w = f->w - u;
    }

    if (v+h >= f->h) {
        h = f->h - v;
    }

    // Find the mean of the current patch
    uint32_t f_sum = imlib_integral_lookup(sum, u, v, w, h);
    uint32_t f_mean = f_sum / (w*h);

    // Find the normalized sum of squares of the image
    for (int y=v; y<v+h; y++) {
        for (int x=u; x<u+w; x++) {
            int a = (int)f->data[y*f->w+x]-f_mean;
            int b = (int)t->data[(y-v)*t->w+(x-u)]-t_mean;
            num += a*b;
            f_sumsq += a*a;
        }
    }

    // Find the normalized cross-correlation
    return (num/(fast_sqrtf(f_sumsq) * fast_sqrtf(t_sumsq)));
}

float imlib_template_match_ds(image_t *f, image_t *t, rectangle_t *r)
{
    point_t pts[9];

    // Integral images
    i_image_t sum;
    imlib_integral_image_alloc(&sum, f->w, f->h);
    imlib_integral_image(f, &sum);

    // Normalized sum of squares of the template
    int t_mean = 0;
    uint32_t t_sumsq=0;
    imlib_image_mean(t, &t_mean, &t_mean, &t_mean);
    for (int i=0; i < (t->w*t->h); i++) {
        int c = (int)t->data[i]-t_mean;
        t_sumsq += c*c;
    }

    int px = 0;
    int py = 0;

    // Initial center point
    int cx = f->w/2 - t->w/2;
    int cy = f->h/2 - t->h/2;

    // Max cross-correlation
    float max_xc=-FLT_MAX;

    // Start with the Large Diamond Search Pattern (LDSP) 9 points.
    bool sdsp = false;

    // Step size == template width
    int step = t->w;

    while (step > 0) {
        // Set the Diamond Search Pattern (DSP).
        set_dsp(cx, cy, pts, sdsp, step);

        // Set the number of search blocks (5 or 9 for SDSP and LDSP respectively).
        int num_pts = (sdsp == true)? 5: 9;

        // Find the block with the highest NCC
        for (int i=0; i<num_pts; i++) {
            if (pts[i].x >= f->w || pts[i].y >= f->h) {
                continue;
            }
            float blk_xc = find_block_ncc(f, t, &sum, t_mean, t_sumsq, pts[i].x, pts[i].y);
            if (blk_xc > max_xc) {
                px = pts[i].x;
                py = pts[i].y;
                max_xc = blk_xc;
            }
        }

        // If the highest correlation is found at the center block and search is using
        // LDSP then the highest correlation is found, if not then switch search to SDSP.
        if (px == cx && py == cy) {
            // Note instead of switching to the smaller pattern, the step size can be reduced
            // each time the highest correlation is found at the center, and break on step == 0.
            // This makes DS much more accurate, but slower.
            step --;
        }

        // Set the new search center to the block with highest correlation
        cx = px;
        cy = py;
    }

    r->x = cx;
    r->y = cy;
    r->w = t->w;
    r->h = t->h;

    if (cx < 0) r->x = 0;
    if (cy < 0) r->y = 0;

    if (cx+t->w > f->w) {
        r->w = f->w - cx;
    }

    if (cy+t->h > f->h) {
        r->h = f->h - cy;
    }

    imlib_integral_image_free(&sum);

    //printf("max xc: %f\n", (double) max_xc);
    return max_xc;
}

/* The NCC can be optimized using integral images and rectangular basis functions.
 * See Kai Briechle's paper "Template Matching using Fast Normalized Cross Correlation".
 *
 * NOTE: only the denominator is optimized.
 *
 */
float imlib_template_match_ex(image_t *f, image_t *t, rectangle_t *roi, int step, rectangle_t *r)
{
    int den_b=0;
    float corr=0.0f;

    // Integral images
    i_image_t sum;
    i_image_t sumsq;

    imlib_integral_image_alloc(&sum, f->w, f->h);
    imlib_integral_image_alloc(&sumsq, f->w, f->h);

    imlib_integral_image(f, &sum);
    imlib_integral_image_sq(f, &sumsq);

    // Normalized sum of squares of the template
    int t_mean = 0;
    imlib_image_mean(t, &t_mean, &t_mean, &t_mean);

    for (int i=0; i < (t->w*t->h); i++) {
        int c = (int)t->data[i]-t_mean;
        den_b += c*c;
    }

    for (int v=roi->y; v<=(roi->y+roi->h-t->h); v+=step) {
    for (int u=roi->x; u<=(roi->x+roi->w-t->w); u+=step) {
        int num = 0;
        // The mean of the current patch
        uint32_t f_sum = imlib_integral_lookup(&sum, u, v, t->w, t->h);
        uint32_t f_sumsq = imlib_integral_lookup(&sumsq, u, v, t->w, t->h);
        uint32_t f_mean = f_sum / (float) (t->w*t->h);

        // Normalized sum of squares of the image
        for (int y=v; y<(v+t->h); y++) {
            for (int x=u; x<(u+t->w); x++) {
                int a = (int)f->data[y*f->w+x]-f_mean;
                int b = (int)t->data[(y-v)*t->w+(x-u)]-t_mean;
                num += a*b;
            }
        }

        uint32_t den_a = f_sumsq - f_sum * (f_sum / (float) (t->w * t->h));

        // Find normalized cross-correlation
        float c = num/(fast_sqrtf(den_a) * fast_sqrtf(den_b));

        if (c > corr) {
            corr = c;
            r->x = u;
            r->y = v;
            r->w = t->w;
            r->h = t->h;
        }
    }
    }

    imlib_integral_image_free(&sum);
    imlib_integral_image_free(&sumsq);
    return corr;
}
