/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Point functions.
 */
#include "imlib.h"
#include "xalloc.h"

point_t *point_alloc(int16_t x, int16_t y)
{
    point_t *p = xalloc(sizeof(point_t));
    p->x = x;
    p->y = y;
    return p;
}

bool point_equal(point_t *p1, point_t *p2)
{
    return ((p1->x==p2->x)&&(p1->y==p2->y));
}

float point_distance(point_t *p1, point_t *p2)
{
    float sum=0.0f;
    sum += (p1->x - p2->x) * (p1->x - p2->x);
    sum += (p1->y - p2->y) * (p1->y - p2->y);
    return fast_sqrtf(sum);
}
