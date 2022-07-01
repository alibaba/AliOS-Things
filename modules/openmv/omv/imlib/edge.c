/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Edge Detection.
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "imlib.h"
#include "fb_alloc.h"
#ifdef IMLIB_ENABLE_BINARY_OPS

typedef struct gvec {
    uint16_t t;
    uint16_t g;
} gvec_t;

void imlib_edge_simple(image_t *src, rectangle_t *roi, int low_thresh, int high_thresh)
{
    imlib_morph(src, 1, kernel_high_pass_3, 1.0f, 0.0f, false, 0, false, NULL);
    list_t thresholds;
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    color_thresholds_list_lnk_data_t lnk_data;
    lnk_data.LMin=low_thresh;
    lnk_data.LMax=high_thresh;
    list_push_back(&thresholds, &lnk_data);
    imlib_binary(src, src, &thresholds, false, false, NULL);
    list_free(&thresholds);
    imlib_erode(src, 1, 2, NULL);
}

void imlib_edge_canny(image_t *src, rectangle_t *roi, int low_thresh, int high_thresh)
{
    int w = src->w;

    gvec_t *gm = fb_alloc0(roi->w*roi->h*sizeof*gm, FB_ALLOC_NO_HINT);

    //1. Noise Reduction with a Gaussian filter
    imlib_sepconv3(src, kernel_gauss_3, 1.0f/16.0f, 0.0f);

    //2. Finding Image Gradients
    for (int gy=1, y=roi->y+1; y<roi->y+roi->h-1; y++, gy++) {
        for (int gx=1, x=roi->x+1; x<roi->x+roi->w-1; x++, gx++) {
            int vx=0, vy=0;
            // sobel kernel in the horizontal direction
            vx  = src->data [(y-1)*w+x-1]
                - src->data [(y-1)*w+x+1]
                + (src->data[(y+0)*w+x-1]<<1)
                - (src->data[(y+0)*w+x+1]<<1)
                + src->data [(y+1)*w+x-1]
                - src->data [(y+1)*w+x+1];

            // sobel kernel in the vertical direction
            vy  = src->data [(y-1)*w+x-1]
                + (src->data[(y-1)*w+x+0]<<1)
                + src->data [(y-1)*w+x+1]
                - src->data [(y+1)*w+x-1]
                - (src->data[(y+1)*w+x+0]<<1)
                - src->data [(y+1)*w+x+1];

            // Find magnitude
            int g = (int) fast_sqrtf(vx*vx + vy*vy);
            // Find the direction and round angle to 0, 45, 90 or 135
            int t = (int) fast_fabsf((atan2f(vy, vx)*180.0f/M_PI));
            if (t < 22) {
                t = 0;
            } else if (t < 67) {
                t = 45;
            } else if (t < 112) {
                t = 90;
            } else if (t < 160) {
                t = 135;
            } else if (t <= 180) {
                t = 0;
            }

            gm[gy*roi->w+gx].t = t;
            gm[gy*roi->w+gx].g = g;
        }
    }

    // 3. Hysteresis Thresholding
    // 4. Non-maximum Suppression and output
    for (int gy=0, y=roi->y; y<roi->y+roi->h; y++, gy++) {
        for (int gx=0, x=roi->x; x<roi->x+roi->w; x++, gx++) {
            int i = y*w+x;
            gvec_t *va=NULL, *vb=NULL, *vc = &gm[gy*roi->w+gx];

            // Clear the borders
            if (y == (roi->y) || y == (roi->y+roi->h-1) ||
                x == (roi->x) || x == (roi->x+roi->w-1)) {
                src->data[i] = 0;
                continue;
            }

            if (vc->g < low_thresh) {
                // Not an edge
                src->data[i] = 0;
                continue;
            // Check if strong or weak edge
            } else if (vc->g >= high_thresh ||
                       gm[(gy-1)*roi->w+(gx-1)].g >= high_thresh ||
                       gm[(gy-1)*roi->w+(gx+0)].g >= high_thresh ||
                       gm[(gy-1)*roi->w+(gx+1)].g >= high_thresh ||
                       gm[(gy+0)*roi->w+(gx-1)].g >= high_thresh ||
                       gm[(gy+0)*roi->w+(gx+1)].g >= high_thresh ||
                       gm[(gy+1)*roi->w+(gx-1)].g >= high_thresh ||
                       gm[(gy+1)*roi->w+(gx+0)].g >= high_thresh ||
                       gm[(gy+1)*roi->w+(gx+1)].g >= high_thresh) {
                vc->g = vc->g;
            } else { // Not an edge
                src->data[i] = 0;
                continue;
            }

            switch (vc->t) {
                case 0: {
                    va = &gm[(gy+0)*roi->w+(gx-1)];
                    vb = &gm[(gy+0)*roi->w+(gx+1)];
                    break;
                }

                case 45: {
                    va = &gm[(gy+1)*roi->w+(gx-1)];
                    vb = &gm[(gy-1)*roi->w+(gx+1)];
                    break;
                }

                case 90: {
                    va = &gm[(gy+1)*roi->w+(gx+0)];
                    vb = &gm[(gy-1)*roi->w+(gx+0)];
                    break;
                }

                case 135: {
                    va = &gm[(gy+1)*roi->w+(gx+1)];
                    vb = &gm[(gy-1)*roi->w+(gx-1)];
                    break;
                }
            }

            if (!(vc->g > va->g && vc->g > vb->g)) {
                src->data[i] = 0;
            } else {
                src->data[i] = 255;
            }
        }
    }

    fb_free();
}
#endif
