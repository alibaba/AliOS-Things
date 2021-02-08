/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ROTATE_H_
#define _ROTATE_H_
#include <stdbool.h>
#include <stdint.h>

int rotate_right90(unsigned char * src, int srcW, int srcH, int channel);
int rotate_left90(unsigned char * src, int srcW, int srcH, int channel);
int rotate_down(unsigned char * src, int srcW, int srcH, int channel);
#endif // _ROTATE_H_

