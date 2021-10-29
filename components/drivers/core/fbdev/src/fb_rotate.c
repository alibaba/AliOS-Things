/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

// #include <fb.h>
#include <fb_rotate.h>


/*rotate 90 degrees clockwise*/
void rotate_cw(const uint8_t *in, uint8_t *out, uint32_t w, uint32_t h)
{
    int32_t i, ix, iy, res; /* Default is RGBA8888 */

    if (in && out) {
        for (iy = 0; iy < h; iy++) {
            for (ix = 0; ix < w; ix++)
                ((uint32_t *)out)[h - 1 - iy + h * ix] = ((uint32_t *)in)[ix + iy * w];
        }
    }
    return;
}
