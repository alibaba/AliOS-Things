/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * HoG.
 * See Histograms of Oriented Gradients (Navneet Dalal and Bill Triggs)
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "imlib.h"
#include "fb_alloc.h"
#include "xalloc.h"

#ifdef IMLIB_ENABLE_HOG
#define N_BINS      (9)
typedef struct bin {
    int d;
    int m;
} bin_t;

int bin_array_comp(const void *obj0, const void *obj1)
{
    const bin_t *b0 = obj0;
    const bin_t *b1 = obj1;
    if (b0->m < b1->m)
        return -1;
    if (b0->m > b1->m)
        return 1;

    return 0;
}

void imlib_find_hog(image_t *src, rectangle_t *roi, int cell_size)
{
    int s = src->w;
    int w = roi->x+roi->w-1;
    int h = roi->y+roi->h-1;

    int block_size = cell_size * 2;
    int x_cells = (roi->w/cell_size);
    int y_cells = (roi->h/cell_size);

    // TODO: Assert row->w/h >= cell_size *2;
    float *hog = fb_alloc0(x_cells * y_cells * N_BINS * sizeof*hog, FB_ALLOC_NO_HINT);

    //2. Finding Image Gradients
    for (int y=roi->y, hog_index=0; y<h; y+=block_size) {
        for (int x=roi->x; x<w; x+=block_size) {
            float k = 0.0f;
            for (int cy=0; cy<block_size; cy++) {
                for (int cx=0; cx<block_size; cx++) {
                    if ((y+cy) > 0 && (y+cy) < h && (x+cx) > 0 && (x+cx) < w) {
                        // Find horizontal/vertical direction
                        int vx = src->data[(y+cy+0)*s+(x+cx+1)] - src->data[(y+cy-0)*s+(x+cx-1)];
                        int vy = src->data[(y+cy+1)*s+(x+cx+0)] - src->data[(y+cy-1)*s+(x+cx-0)];
                        // Find magnitude
                        float m = fast_sqrtf(vx*vx + vy*vy);
                        if(((int) m) > 1) {
                            k += m*m;
                            // Find and quantize gradient degree
                            // TODO atan2f is swapped for visualization
                            int t = ((int) fast_fabsf((atan2f(vx, vy)*180.0f/M_PI))) / 20;
                            t = (t == 9)? 0 : t;

                            // hog[((cy/cell_size) * x_cells + (cx/cell_size)) * N_BINS + t] += m;
                            hog[hog_index + (((cy/8)*2+(cx/8)) * N_BINS) + t] += m;
                        }
                    }
                }
            }

            // Normalize the last block
            k = sqrtf(k);
            for (int i=hog_index; i<(hog_index+(N_BINS*4)); i++) {
                    hog[i] = hog[i]/k;
            }

            hog_index += (N_BINS*4);
        }
    }

    memset(src->pixels, 0, src->w*src->h);

    array_t *gds;
    bin_t bins[9];
    array_alloc(&gds, NULL);

    for (int i=0; i<N_BINS; i++) {
        array_push_back(gds, &bins[i]);
    }

    int l = cell_size/2;
    // Note cells are not ordered histograms of 4 cells
    for (int by=0, hog_index=0; by<y_cells; by+=2) {
        for (int bx=0; bx<x_cells; bx+=2) {
            for (int y=0; y<2; y++) {
                for (int x=0; x<2; x++) {
                    // Sort and draw bins
                    for (int i=hog_index; i<hog_index+N_BINS; i++) {
                        int m = (int)(hog[i]*255);
                        if (m > 255) {
                            m = 255;
                        } else if (m < 0) {
                            m = 0;
                        }
                        bin_t *bin = array_at(gds, (i%N_BINS));
                        bin->m = m;
                        bin->d = ((i%N_BINS)*20);
                    }

                    array_sort(gds, bin_array_comp);

                    int x1 = (x+bx) * cell_size + l;
                    int y1 = (y+by) * cell_size + l;
                    for (int i=0; i<N_BINS; i++) {
                        bin_t *bin = array_at(gds, i);
                        int x2 = l * cos_table[bin->d];
                        int y2 = l * sin_table[bin->d];
                        imlib_draw_line(src, (x1 - x2), (y1 + y2), (x1 + x2), (y1 - y2), bin->m, 1);
                    }

                    hog_index += N_BINS;
                }
            }
        }
    }

    xfree(gds);
    fb_free();
}
#endif // IMLIB_ENABLE_HOG
