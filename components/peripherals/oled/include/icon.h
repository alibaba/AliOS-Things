#ifndef __ICON_H__
#define __ICON_H__

#include <stdio.h>

typedef struct
{
    uint8_t *p_icon_data;
    uint8_t width;
    uint8_t height;
    uint8_t *p_icon_mask;
} icon_t;

#endif