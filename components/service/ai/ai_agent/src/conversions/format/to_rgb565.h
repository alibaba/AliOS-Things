/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _TO_RGB565_H_
#define _TO_RGB565_H_
#include <stdbool.h>
#include <stdint.h>

int rgb888torgb565(unsigned char* rgb888_buf, int rgb888_size, unsigned short *rgb565_buf, int rgb565_size);
int rgb565torgb888(unsigned short *rgb565_buf, int rgb565_size,unsigned char*rgb888_buf, int rgb888_size);
int rotate_right90(unsigned char * src, int srcW, int srcH, int channel);
int rotate_left90(unsigned char * src, int srcW, int srcH, int channel);
int rotate_down(unsigned char * src, int srcW, int srcH, int channel);
#endif // _TO_RGB565_H_

