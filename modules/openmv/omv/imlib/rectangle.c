/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Rectangle functions.
 */
#include "imlib.h"
#include "array.h"
#include "xalloc.h"

rectangle_t *rectangle_alloc(int16_t x, int16_t y, int16_t w, int16_t h)
{
    rectangle_t *r = xalloc(sizeof(rectangle_t));
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    return r;
}

bool rectangle_equal(rectangle_t *r1, rectangle_t *r2)
{
    return ((r1->x==r2->x)&&(r1->y==r2->y)&&(r1->w==r2->w)&&(r1->h==r2->h));
}

bool rectangle_intersects(rectangle_t *r1, rectangle_t *r2)
{
    return  ((r1->x < (r2->x+r2->w)) &&
             (r1->y < (r2->y+r2->h)) &&
             ((r1->x+r1->w) > r2->x) &&
             ((r1->y+r1->h) > r2->y));
}

// Determine subimg even if it is going off the edge of the main image.
bool rectangle_subimg(image_t *img, rectangle_t *r, rectangle_t *r_out)
{
    rectangle_t r_img;
    r_img.x = 0;
    r_img.y = 0;
    r_img.w = img->w;
    r_img.h = img->h;
    bool result = rectangle_intersects(&r_img, r);
    if (result) {
        int r_img_x2 = r_img.x + r_img.w;
        int r_img_y2 = r_img.y + r_img.h;
        int r_x2 = r->x + r->w;
        int r_y2 = r->y + r->h;
        r_out->x = IM_MAX(r_img.x, r->x);
        r_out->y = IM_MAX(r_img.y, r->y);
        r_out->w = IM_MIN(r_img_x2, r_x2) - r_out->x;
        r_out->h = IM_MIN(r_img_y2, r_y2) - r_out->y;
    }
    return result;
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_add(rectangle_t *r1, rectangle_t *r2)
{
    r1->x += r2->x;
    r1->y += r2->y;
    r1->w += r2->w;
    r1->h += r2->h;
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_div(rectangle_t *r, int c)
{
    r->x /= c;
    r->y /= c;
    r->w /= c;
    r->h /= c;
}

array_t *rectangle_merge(array_t *rectangles)
{
    array_t *objects; array_alloc(&objects, xfree);
    array_t *overlap; array_alloc(&overlap, xfree);
    /* merge overlaping detections */
    while (array_length(rectangles)) {
        /* check for overlaping detections */
        rectangle_t *rect = (rectangle_t *) array_take(rectangles, 0);
        for (int j=0; j<array_length(rectangles); j++) { // do not cache bound
            if (rectangle_intersects(rect, (rectangle_t *) array_at(rectangles, j))) {
                array_push_back(overlap, array_take(rectangles, j--));
            }
        }
        /* add the overlaping detections */
        int count = array_length(overlap);
        for (int i=0; i<count; i++) {
            rectangle_t *overlap_rect = (rectangle_t *) array_pop_back(overlap);
            rectangle_add(rect, overlap_rect);
            xfree(overlap_rect);
        }
        /* average the overlaping detections */
        rectangle_div(rect, count + 1);
        array_push_back(objects, rect);
    }
    array_free(rectangles);
    array_free(overlap);
    return objects;
}

// Expands a bounding box with a point.
// After adding all points sub x from w and y from h.
void rectangle_expand(rectangle_t *r, int x, int y)
{
    if (x < r->x) {
        r->x = x;
    }
    if (y < r->y) {
        r->y = y;
    }
    if (x > r->w) {
        r->w = x;
    }
    if (y > r->h) {
        r->h = y;
    }
}
