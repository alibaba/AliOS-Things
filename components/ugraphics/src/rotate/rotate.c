
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "to_rgb565.h"
#include <stdbool.h>

// 顺时针90度
int rotate_right90(unsigned char *src, int srcW, int srcH, int channel)
{
    unsigned char *tempSrc = NULL;
    int mSize = srcW * srcH * sizeof(char) * channel;
    int i = 0;
    int j = 0;
    int k = 0;
    int desW = 0;
    int desH = 0;

    desW = srcH;
    desH = srcW;

    tempSrc = (unsigned char *)malloc(sizeof(char) * srcW * srcH * channel);
    memcpy(tempSrc, src, mSize);

    for (i = 0; i < desH; i++) {
        for (j = 0; j < desW; j++) {
            for (k = 0; k < channel; k++)
                src[(i * desW + j) * channel + k] =
                     tempSrc[((srcH - 1 - j) * srcW + i) * channel + k];

        }
    }

    free(tempSrc);
    return 0;
}

// 逆时针90度
int rotate_left90(unsigned char *src, int srcW, int srcH, int channel)
{
    unsigned char *tempSrc = NULL;
    int mSize = srcW * srcH * sizeof(char) * channel;
    int i = 0;
    int j = 0;
    int k = 0;
    int desW = 0;
    int desH = 0;

    desW = srcH;
    desH = srcW;

    tempSrc = (unsigned char *)malloc(sizeof(char) * srcW * srcH * channel);
    memcpy(tempSrc, src, mSize);

    for (i = 0; i < desH; i++) {
        for (j = 0; j < desW; j++) {
            for (k = 0; k < channel; k++)
                src[(i * desW + j) * channel + k] =
                     tempSrc[(j * srcW + i) * channel + k];

        }
    }

    free(tempSrc);
    return 0;
}

// 旋转180度
int rotate_down(unsigned char *src, int srcW, int srcH, int channel)
{
    unsigned char *tempSrc = NULL;
    int mSize = srcW * srcH * sizeof(char) * channel;
    int i = 0;
    int j = 0;
    int k = 0;
    int desW = 0;
    int desH = 0;

    desW = srcW;
    desH = srcH;

    tempSrc = (unsigned char *)malloc(sizeof(char) * srcW * srcH * channel);
    memcpy(tempSrc, src, mSize);

    for (i = 0; i < desH; i++) {
        for (j = 0; j < desW; j++) {
            for (k = 0; k < channel; k++)
                src[(i * desW + j) * channel + k] =
                     tempSrc[((srcH - 1 - i) * srcW +
                                 srcW - 1 - j) * channel + k];

        }
    }

    free(tempSrc);
    return 0;
}

