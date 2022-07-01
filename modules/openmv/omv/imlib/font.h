/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Font data.
 */
#ifndef __FONT_H__
#define __FONT_H__
#include <stdint.h>
typedef struct {
    int w;
    int h;
    uint8_t data[10];
} glyph_t;
extern const glyph_t font[95];
#endif // __FONT_H__
