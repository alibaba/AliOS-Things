/*
 * NOTE: This code is mostly auto-generated.
 * See https://www.edwardrosten.com/work/fast.html
 *
 * Copyright (c) 2006, 2008, 2009, 2010 Edward Rosten All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 	*Redistributions of source code must retain the above copyright
 * 	 notice, this list of conditions and the following disclaimer.
 * 	*Redistributions in binary form must reproduce the above copyright
 * 	 notice, this list of conditions and the following disclaimer in the
 * 	 documentation and/or other materials provided with the distribution.
 * 	*Neither the name of the University of Cambridge nor the names of 
 * 	 its contributors may be used to endorse or promote products derived 
 * 	 from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "imlib.h"
#include "xalloc.h"
#include "fb_alloc.h"
#include "gc.h"

#ifdef IMLIB_ENABLE_FAST

#define MAX_ROW         (480)
#define MAX_CORNERS     (2000)
#define Compare(X, Y) ((X)>=(Y))

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t score;
} corner_t;

static int pixel[16];
static corner_t *fast9_detect(image_t *image, rectangle_t *roi, int *n_corners, int b);
static void fast9_score(image_t *image, corner_t *corners, int num_corners, int b);
static void nonmax_suppression(corner_t *corners, int num_corners, array_t *keypoints);

static kp_t *alloc_keypoint(uint16_t x, uint16_t y, uint16_t score)
{
    // Note must set keypoint descriptor to zeros
    kp_t *kpt = xalloc0(sizeof*kpt);
    kpt->x = x;
    kpt->y = y;
    kpt->score = score;
    return kpt;
}

static void make_offsets(int pixel[], int row_stride)
{
        pixel[0] = 0 + row_stride * 3;
        pixel[1] = 1 + row_stride * 3;
        pixel[2] = 2 + row_stride * 2;
        pixel[3] = 3 + row_stride * 1;
        pixel[4] = 3 + row_stride * 0;
        pixel[5] = 3 + row_stride * -1;
        pixel[6] = 2 + row_stride * -2;
        pixel[7] = 1 + row_stride * -3;
        pixel[8] = 0 + row_stride * -3;
        pixel[9] = -1 + row_stride * -3;
        pixel[10] = -2 + row_stride * -2;
        pixel[11] = -3 + row_stride * -1;
        pixel[12] = -3 + row_stride * 0;
        pixel[13] = -3 + row_stride * 1;
        pixel[14] = -2 + row_stride * 2;
        pixel[15] = -1 + row_stride * 3;
}

void fast_detect(image_t *image, array_t *keypoints, int threshold, rectangle_t *roi)
{
    int num_corners=0;
    make_offsets(pixel, image->w);

    // Find corners
    corner_t *corners = fast9_detect(image, roi, &num_corners, threshold);
    if (num_corners) {
        // Score corners
        fast9_score(image, corners, num_corners, threshold);
        // Non-max suppression
        nonmax_suppression(corners, num_corners, keypoints);
    }

    // Free corners;
    fb_free();
}

static void nonmax_suppression(corner_t *corners, int num_corners, array_t *keypoints)
{
    gc_info_t info;

	int last_row;
	int16_t row_start[MAX_ROW+1];
	const int sz = num_corners;

	/* Point above points (roughly) to the pixel above
       the one of interest, if there is a feature there.*/
	int point_above = 0;
	int point_below = 0;

	/* Find where each row begins (the corners are output in raster scan order).
       A beginning of -1 signifies that there are no corners on that row. */
	last_row  = corners[sz-1].y;

	for(int i=0; i<last_row+1; i++) {
		row_start[i] = -1;
    }

    for (int i=0, prev_row=-1; i<sz; i++) {
        corner_t *c = &corners[i];
        if (c->y != prev_row) {
            row_start[c->y] = i;
            prev_row = c->y;
        }
    }

    for(int i=0; i<sz; i++) {
        corner_t pos = corners[i];
        uint16_t score = pos.score;

        /*Check left */
        if (i > 0) {
            if (corners[i-1].x == pos.x-1 && corners[i-1].y == pos.y && Compare(corners[i-1].score, score)) {
                goto nonmax;
            }
        }

        /*Check right*/
        if (i < (sz - 1)) {
            if (corners[i+1].x == pos.x+1 && corners[i+1].y == pos.y && Compare(corners[i+1].score, score)) {
                goto nonmax;
            }
        }

        /*Check above (if there is a valid row above)*/
        if (pos.y != 0 && row_start[pos.y - 1] != -1)  {
            /*Make sure that current point_above is one row above.*/
            if(corners[point_above].y < pos.y - 1)
                point_above = row_start[pos.y-1];

            /*Make point_above point to the first of the pixels above the current point, if it exists.*/
            for (; corners[point_above].y < pos.y && corners[point_above].x < pos.x - 1; point_above++) {

            }

            for (int j=point_above; corners[j].y < pos.y && corners[j].x <= pos.x + 1; j++) {
                int x = corners[j].x;
                if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && Compare(corners[j].score, score))
                    goto nonmax;
            }
        }

        /*Check below (if there is anything below)*/
        if (pos.y != last_row && row_start[pos.y + 1] != -1 && point_below < sz) /*Nothing below*/ {
            if (corners[point_below].y < pos.y + 1)
                point_below = row_start[pos.y+1];

            /* Make point below point to one of the pixels belowthe current point, if it exists.*/
            for (; point_below < sz && corners[point_below].y == pos.y+1 && corners[point_below].x < pos.x - 1; point_below++) {
            }

            for (int j=point_below; j < sz && corners[j].y == pos.y+1 && corners[j].x <= pos.x + 1; j++) {
                int x = corners[j].x;
                if( (x == pos.x - 1 || x ==pos.x || x == pos.x+1) && Compare(corners[j].score, score))
                    goto nonmax;
            }
        }

        gc_info(&info);
        #define MIN_MEM (10*1024)
        // Allocate keypoints until we're almost out of memory
        if (info.free < MIN_MEM) {
            // Try collecting memory
            gc_collect();
            // If it didn't work break
            gc_info(&info);
            if (info.free < MIN_MEM) {
                break;
            }
        }

        #undef MIN_MEM
        array_push_back(keypoints, alloc_keypoint(pos.x, pos.y, pos.score));
        nonmax:
        ;
    }
}

static int fast9_corner_score(const byte* p, int bstart)
{    
    int bmin = bstart;
    int bmax = 255;
    int b = (bmax + bmin)/2;
    
    /*Compute the score using binary search*/
	for(;;)
    {
		int cb = *p + b;
		int c_b= *p - b;


        if( p[pixel[0]] > cb)
         if( p[pixel[1]] > cb)
          if( p[pixel[2]] > cb)
           if( p[pixel[3]] > cb)
            if( p[pixel[4]] > cb)
             if( p[pixel[5]] > cb)
              if( p[pixel[6]] > cb)
               if( p[pixel[7]] > cb)
                if( p[pixel[8]] > cb)
                 goto is_a_corner;
                else
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
               else if( p[pixel[7]] < c_b)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[pixel[14]] < c_b)
                 if( p[pixel[8]] < c_b)
                  if( p[pixel[9]] < c_b)
                   if( p[pixel[10]] < c_b)
                    if( p[pixel[11]] < c_b)
                     if( p[pixel[12]] < c_b)
                      if( p[pixel[13]] < c_b)
                       if( p[pixel[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else if( p[pixel[6]] < c_b)
               if( p[pixel[15]] > cb)
                if( p[pixel[13]] > cb)
                 if( p[pixel[14]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[pixel[13]] < c_b)
                 if( p[pixel[7]] < c_b)
                  if( p[pixel[8]] < c_b)
                   if( p[pixel[9]] < c_b)
                    if( p[pixel[10]] < c_b)
                     if( p[pixel[11]] < c_b)
                      if( p[pixel[12]] < c_b)
                       if( p[pixel[14]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[pixel[7]] < c_b)
                 if( p[pixel[8]] < c_b)
                  if( p[pixel[9]] < c_b)
                   if( p[pixel[10]] < c_b)
                    if( p[pixel[11]] < c_b)
                     if( p[pixel[12]] < c_b)
                      if( p[pixel[13]] < c_b)
                       if( p[pixel[14]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[pixel[13]] < c_b)
                if( p[pixel[7]] < c_b)
                 if( p[pixel[8]] < c_b)
                  if( p[pixel[9]] < c_b)
                   if( p[pixel[10]] < c_b)
                    if( p[pixel[11]] < c_b)
                     if( p[pixel[12]] < c_b)
                      if( p[pixel[14]] < c_b)
                       if( p[pixel[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else if( p[pixel[5]] < c_b)
              if( p[pixel[14]] > cb)
               if( p[pixel[12]] > cb)
                if( p[pixel[13]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       if( p[pixel[11]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[pixel[12]] < c_b)
                if( p[pixel[6]] < c_b)
                 if( p[pixel[7]] < c_b)
                  if( p[pixel[8]] < c_b)
                   if( p[pixel[9]] < c_b)
                    if( p[pixel[10]] < c_b)
                     if( p[pixel[11]] < c_b)
                      if( p[pixel[13]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[14]] < c_b)
               if( p[pixel[7]] < c_b)
                if( p[pixel[8]] < c_b)
                 if( p[pixel[9]] < c_b)
                  if( p[pixel[10]] < c_b)
                   if( p[pixel[11]] < c_b)
                    if( p[pixel[12]] < c_b)
                     if( p[pixel[13]] < c_b)
                      if( p[pixel[6]] < c_b)
                       goto is_a_corner;
                      else
                       if( p[pixel[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               if( p[pixel[6]] < c_b)
                if( p[pixel[7]] < c_b)
                 if( p[pixel[8]] < c_b)
                  if( p[pixel[9]] < c_b)
                   if( p[pixel[10]] < c_b)
                    if( p[pixel[11]] < c_b)
                     if( p[pixel[12]] < c_b)
                      if( p[pixel[13]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       if( p[pixel[11]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[12]] < c_b)
               if( p[pixel[7]] < c_b)
                if( p[pixel[8]] < c_b)
                 if( p[pixel[9]] < c_b)
                  if( p[pixel[10]] < c_b)
                   if( p[pixel[11]] < c_b)
                    if( p[pixel[13]] < c_b)
                     if( p[pixel[14]] < c_b)
                      if( p[pixel[6]] < c_b)
                       goto is_a_corner;
                      else
                       if( p[pixel[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else if( p[pixel[4]] < c_b)
             if( p[pixel[13]] > cb)
              if( p[pixel[11]] > cb)
               if( p[pixel[12]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[11]] < c_b)
               if( p[pixel[5]] < c_b)
                if( p[pixel[6]] < c_b)
                 if( p[pixel[7]] < c_b)
                  if( p[pixel[8]] < c_b)
                   if( p[pixel[9]] < c_b)
                    if( p[pixel[10]] < c_b)
                     if( p[pixel[12]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[pixel[13]] < c_b)
              if( p[pixel[7]] < c_b)
               if( p[pixel[8]] < c_b)
                if( p[pixel[9]] < c_b)
                 if( p[pixel[10]] < c_b)
                  if( p[pixel[11]] < c_b)
                   if( p[pixel[12]] < c_b)
                    if( p[pixel[6]] < c_b)
                     if( p[pixel[5]] < c_b)
                      goto is_a_corner;
                     else
                      if( p[pixel[14]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[pixel[14]] < c_b)
                      if( p[pixel[15]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              if( p[pixel[5]] < c_b)
               if( p[pixel[6]] < c_b)
                if( p[pixel[7]] < c_b)
                 if( p[pixel[8]] < c_b)
                  if( p[pixel[9]] < c_b)
                   if( p[pixel[10]] < c_b)
                    if( p[pixel[11]] < c_b)
                     if( p[pixel[12]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      if( p[pixel[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[pixel[11]] < c_b)
              if( p[pixel[7]] < c_b)
               if( p[pixel[8]] < c_b)
                if( p[pixel[9]] < c_b)
                 if( p[pixel[10]] < c_b)
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    if( p[pixel[6]] < c_b)
                     if( p[pixel[5]] < c_b)
                      goto is_a_corner;
                     else
                      if( p[pixel[14]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[pixel[14]] < c_b)
                      if( p[pixel[15]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else if( p[pixel[3]] < c_b)
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[pixel[10]] < c_b)
             if( p[pixel[7]] < c_b)
              if( p[pixel[8]] < c_b)
               if( p[pixel[9]] < c_b)
                if( p[pixel[11]] < c_b)
                 if( p[pixel[6]] < c_b)
                  if( p[pixel[5]] < c_b)
                   if( p[pixel[4]] < c_b)
                    goto is_a_corner;
                   else
                    if( p[pixel[12]] < c_b)
                     if( p[pixel[13]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[12]] < c_b)
                    if( p[pixel[13]] < c_b)
                     if( p[pixel[14]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    if( p[pixel[14]] < c_b)
                     if( p[pixel[15]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     if( p[pixel[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[pixel[10]] < c_b)
             if( p[pixel[7]] < c_b)
              if( p[pixel[8]] < c_b)
               if( p[pixel[9]] < c_b)
                if( p[pixel[11]] < c_b)
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[5]] < c_b)
                    if( p[pixel[4]] < c_b)
                     goto is_a_corner;
                    else
                     if( p[pixel[13]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    if( p[pixel[13]] < c_b)
                     if( p[pixel[14]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[13]] < c_b)
                    if( p[pixel[14]] < c_b)
                     if( p[pixel[15]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
          else if( p[pixel[2]] < c_b)
           if( p[pixel[9]] > cb)
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[pixel[9]] < c_b)
            if( p[pixel[7]] < c_b)
             if( p[pixel[8]] < c_b)
              if( p[pixel[10]] < c_b)
               if( p[pixel[6]] < c_b)
                if( p[pixel[5]] < c_b)
                 if( p[pixel[4]] < c_b)
                  if( p[pixel[3]] < c_b)
                   goto is_a_corner;
                  else
                   if( p[pixel[11]] < c_b)
                    if( p[pixel[12]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[11]] < c_b)
                   if( p[pixel[12]] < c_b)
                    if( p[pixel[13]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[11]] < c_b)
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    if( p[pixel[14]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[11]] < c_b)
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[13]] < c_b)
                   if( p[pixel[14]] < c_b)
                    if( p[pixel[15]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           if( p[pixel[9]] > cb)
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    if( p[pixel[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[pixel[9]] < c_b)
            if( p[pixel[7]] < c_b)
             if( p[pixel[8]] < c_b)
              if( p[pixel[10]] < c_b)
               if( p[pixel[11]] < c_b)
                if( p[pixel[6]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[4]] < c_b)
                   if( p[pixel[3]] < c_b)
                    goto is_a_corner;
                   else
                    if( p[pixel[12]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[12]] < c_b)
                    if( p[pixel[13]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    if( p[pixel[14]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[13]] < c_b)
                   if( p[pixel[14]] < c_b)
                    if( p[pixel[15]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
         else if( p[pixel[1]] < c_b)
          if( p[pixel[8]] > cb)
           if( p[pixel[9]] > cb)
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[2]] > cb)
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[pixel[8]] < c_b)
           if( p[pixel[7]] < c_b)
            if( p[pixel[9]] < c_b)
             if( p[pixel[6]] < c_b)
              if( p[pixel[5]] < c_b)
               if( p[pixel[4]] < c_b)
                if( p[pixel[3]] < c_b)
                 if( p[pixel[2]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[10]] < c_b)
                   if( p[pixel[11]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[10]] < c_b)
                  if( p[pixel[11]] < c_b)
                   if( p[pixel[12]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[10]] < c_b)
                 if( p[pixel[11]] < c_b)
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[10]] < c_b)
                if( p[pixel[11]] < c_b)
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[13]] < c_b)
                   if( p[pixel[14]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[10]] < c_b)
               if( p[pixel[11]] < c_b)
                if( p[pixel[12]] < c_b)
                 if( p[pixel[13]] < c_b)
                  if( p[pixel[14]] < c_b)
                   if( p[pixel[15]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else
          if( p[pixel[8]] > cb)
           if( p[pixel[9]] > cb)
            if( p[pixel[10]] > cb)
             if( p[pixel[11]] > cb)
              if( p[pixel[12]] > cb)
               if( p[pixel[13]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[2]] > cb)
               if( p[pixel[3]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[pixel[8]] < c_b)
           if( p[pixel[7]] < c_b)
            if( p[pixel[9]] < c_b)
             if( p[pixel[10]] < c_b)
              if( p[pixel[6]] < c_b)
               if( p[pixel[5]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[3]] < c_b)
                  if( p[pixel[2]] < c_b)
                   goto is_a_corner;
                  else
                   if( p[pixel[11]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[11]] < c_b)
                   if( p[pixel[12]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[11]] < c_b)
                  if( p[pixel[12]] < c_b)
                   if( p[pixel[13]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[11]] < c_b)
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[13]] < c_b)
                   if( p[pixel[14]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[11]] < c_b)
                if( p[pixel[12]] < c_b)
                 if( p[pixel[13]] < c_b)
                  if( p[pixel[14]] < c_b)
                   if( p[pixel[15]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
        else if( p[pixel[0]] < c_b)
         if( p[pixel[1]] > cb)
          if( p[pixel[8]] > cb)
           if( p[pixel[7]] > cb)
            if( p[pixel[9]] > cb)
             if( p[pixel[6]] > cb)
              if( p[pixel[5]] > cb)
               if( p[pixel[4]] > cb)
                if( p[pixel[3]] > cb)
                 if( p[pixel[2]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[10]] > cb)
                   if( p[pixel[11]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[10]] > cb)
                  if( p[pixel[11]] > cb)
                   if( p[pixel[12]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[10]] > cb)
                 if( p[pixel[11]] > cb)
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[10]] > cb)
                if( p[pixel[11]] > cb)
                 if( p[pixel[12]] > cb)
                  if( p[pixel[13]] > cb)
                   if( p[pixel[14]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[10]] > cb)
               if( p[pixel[11]] > cb)
                if( p[pixel[12]] > cb)
                 if( p[pixel[13]] > cb)
                  if( p[pixel[14]] > cb)
                   if( p[pixel[15]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[pixel[8]] < c_b)
           if( p[pixel[9]] < c_b)
            if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[2]] < c_b)
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else if( p[pixel[1]] < c_b)
          if( p[pixel[2]] > cb)
           if( p[pixel[9]] > cb)
            if( p[pixel[7]] > cb)
             if( p[pixel[8]] > cb)
              if( p[pixel[10]] > cb)
               if( p[pixel[6]] > cb)
                if( p[pixel[5]] > cb)
                 if( p[pixel[4]] > cb)
                  if( p[pixel[3]] > cb)
                   goto is_a_corner;
                  else
                   if( p[pixel[11]] > cb)
                    if( p[pixel[12]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[11]] > cb)
                   if( p[pixel[12]] > cb)
                    if( p[pixel[13]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[11]] > cb)
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    if( p[pixel[14]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[11]] > cb)
                 if( p[pixel[12]] > cb)
                  if( p[pixel[13]] > cb)
                   if( p[pixel[14]] > cb)
                    if( p[pixel[15]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[pixel[9]] < c_b)
            if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[pixel[2]] < c_b)
           if( p[pixel[3]] > cb)
            if( p[pixel[10]] > cb)
             if( p[pixel[7]] > cb)
              if( p[pixel[8]] > cb)
               if( p[pixel[9]] > cb)
                if( p[pixel[11]] > cb)
                 if( p[pixel[6]] > cb)
                  if( p[pixel[5]] > cb)
                   if( p[pixel[4]] > cb)
                    goto is_a_corner;
                   else
                    if( p[pixel[12]] > cb)
                     if( p[pixel[13]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[12]] > cb)
                    if( p[pixel[13]] > cb)
                     if( p[pixel[14]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    if( p[pixel[14]] > cb)
                     if( p[pixel[15]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[pixel[3]] < c_b)
            if( p[pixel[4]] > cb)
             if( p[pixel[13]] > cb)
              if( p[pixel[7]] > cb)
               if( p[pixel[8]] > cb)
                if( p[pixel[9]] > cb)
                 if( p[pixel[10]] > cb)
                  if( p[pixel[11]] > cb)
                   if( p[pixel[12]] > cb)
                    if( p[pixel[6]] > cb)
                     if( p[pixel[5]] > cb)
                      goto is_a_corner;
                     else
                      if( p[pixel[14]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[pixel[14]] > cb)
                      if( p[pixel[15]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[pixel[13]] < c_b)
              if( p[pixel[11]] > cb)
               if( p[pixel[5]] > cb)
                if( p[pixel[6]] > cb)
                 if( p[pixel[7]] > cb)
                  if( p[pixel[8]] > cb)
                   if( p[pixel[9]] > cb)
                    if( p[pixel[10]] > cb)
                     if( p[pixel[12]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[11]] < c_b)
               if( p[pixel[12]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              if( p[pixel[5]] > cb)
               if( p[pixel[6]] > cb)
                if( p[pixel[7]] > cb)
                 if( p[pixel[8]] > cb)
                  if( p[pixel[9]] > cb)
                   if( p[pixel[10]] > cb)
                    if( p[pixel[11]] > cb)
                     if( p[pixel[12]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else if( p[pixel[4]] < c_b)
             if( p[pixel[5]] > cb)
              if( p[pixel[14]] > cb)
               if( p[pixel[7]] > cb)
                if( p[pixel[8]] > cb)
                 if( p[pixel[9]] > cb)
                  if( p[pixel[10]] > cb)
                   if( p[pixel[11]] > cb)
                    if( p[pixel[12]] > cb)
                     if( p[pixel[13]] > cb)
                      if( p[pixel[6]] > cb)
                       goto is_a_corner;
                      else
                       if( p[pixel[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[14]] < c_b)
               if( p[pixel[12]] > cb)
                if( p[pixel[6]] > cb)
                 if( p[pixel[7]] > cb)
                  if( p[pixel[8]] > cb)
                   if( p[pixel[9]] > cb)
                    if( p[pixel[10]] > cb)
                     if( p[pixel[11]] > cb)
                      if( p[pixel[13]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[pixel[12]] < c_b)
                if( p[pixel[13]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       if( p[pixel[11]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               if( p[pixel[6]] > cb)
                if( p[pixel[7]] > cb)
                 if( p[pixel[8]] > cb)
                  if( p[pixel[9]] > cb)
                   if( p[pixel[10]] > cb)
                    if( p[pixel[11]] > cb)
                     if( p[pixel[12]] > cb)
                      if( p[pixel[13]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else if( p[pixel[5]] < c_b)
              if( p[pixel[6]] > cb)
               if( p[pixel[15]] < c_b)
                if( p[pixel[13]] > cb)
                 if( p[pixel[7]] > cb)
                  if( p[pixel[8]] > cb)
                   if( p[pixel[9]] > cb)
                    if( p[pixel[10]] > cb)
                     if( p[pixel[11]] > cb)
                      if( p[pixel[12]] > cb)
                       if( p[pixel[14]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[pixel[13]] < c_b)
                 if( p[pixel[14]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[pixel[7]] > cb)
                 if( p[pixel[8]] > cb)
                  if( p[pixel[9]] > cb)
                   if( p[pixel[10]] > cb)
                    if( p[pixel[11]] > cb)
                     if( p[pixel[12]] > cb)
                      if( p[pixel[13]] > cb)
                       if( p[pixel[14]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else if( p[pixel[6]] < c_b)
               if( p[pixel[7]] > cb)
                if( p[pixel[14]] > cb)
                 if( p[pixel[8]] > cb)
                  if( p[pixel[9]] > cb)
                   if( p[pixel[10]] > cb)
                    if( p[pixel[11]] > cb)
                     if( p[pixel[12]] > cb)
                      if( p[pixel[13]] > cb)
                       if( p[pixel[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[pixel[7]] < c_b)
                if( p[pixel[8]] < c_b)
                 goto is_a_corner;
                else
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[13]] > cb)
                if( p[pixel[7]] > cb)
                 if( p[pixel[8]] > cb)
                  if( p[pixel[9]] > cb)
                   if( p[pixel[10]] > cb)
                    if( p[pixel[11]] > cb)
                     if( p[pixel[12]] > cb)
                      if( p[pixel[14]] > cb)
                       if( p[pixel[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[12]] > cb)
               if( p[pixel[7]] > cb)
                if( p[pixel[8]] > cb)
                 if( p[pixel[9]] > cb)
                  if( p[pixel[10]] > cb)
                   if( p[pixel[11]] > cb)
                    if( p[pixel[13]] > cb)
                     if( p[pixel[14]] > cb)
                      if( p[pixel[6]] > cb)
                       goto is_a_corner;
                      else
                       if( p[pixel[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       if( p[pixel[11]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[pixel[11]] > cb)
              if( p[pixel[7]] > cb)
               if( p[pixel[8]] > cb)
                if( p[pixel[9]] > cb)
                 if( p[pixel[10]] > cb)
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    if( p[pixel[6]] > cb)
                     if( p[pixel[5]] > cb)
                      goto is_a_corner;
                     else
                      if( p[pixel[14]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[pixel[14]] > cb)
                      if( p[pixel[15]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      if( p[pixel[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            if( p[pixel[10]] > cb)
             if( p[pixel[7]] > cb)
              if( p[pixel[8]] > cb)
               if( p[pixel[9]] > cb)
                if( p[pixel[11]] > cb)
                 if( p[pixel[12]] > cb)
                  if( p[pixel[6]] > cb)
                   if( p[pixel[5]] > cb)
                    if( p[pixel[4]] > cb)
                     goto is_a_corner;
                    else
                     if( p[pixel[13]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    if( p[pixel[13]] > cb)
                     if( p[pixel[14]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[13]] > cb)
                    if( p[pixel[14]] > cb)
                     if( p[pixel[15]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     if( p[pixel[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
          else
           if( p[pixel[9]] > cb)
            if( p[pixel[7]] > cb)
             if( p[pixel[8]] > cb)
              if( p[pixel[10]] > cb)
               if( p[pixel[11]] > cb)
                if( p[pixel[6]] > cb)
                 if( p[pixel[5]] > cb)
                  if( p[pixel[4]] > cb)
                   if( p[pixel[3]] > cb)
                    goto is_a_corner;
                   else
                    if( p[pixel[12]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[pixel[12]] > cb)
                    if( p[pixel[13]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    if( p[pixel[14]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[12]] > cb)
                  if( p[pixel[13]] > cb)
                   if( p[pixel[14]] > cb)
                    if( p[pixel[15]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[pixel[9]] < c_b)
            if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    if( p[pixel[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
         else
          if( p[pixel[8]] > cb)
           if( p[pixel[7]] > cb)
            if( p[pixel[9]] > cb)
             if( p[pixel[10]] > cb)
              if( p[pixel[6]] > cb)
               if( p[pixel[5]] > cb)
                if( p[pixel[4]] > cb)
                 if( p[pixel[3]] > cb)
                  if( p[pixel[2]] > cb)
                   goto is_a_corner;
                  else
                   if( p[pixel[11]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[pixel[11]] > cb)
                   if( p[pixel[12]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[11]] > cb)
                  if( p[pixel[12]] > cb)
                   if( p[pixel[13]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[11]] > cb)
                 if( p[pixel[12]] > cb)
                  if( p[pixel[13]] > cb)
                   if( p[pixel[14]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[11]] > cb)
                if( p[pixel[12]] > cb)
                 if( p[pixel[13]] > cb)
                  if( p[pixel[14]] > cb)
                   if( p[pixel[15]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[pixel[8]] < c_b)
           if( p[pixel[9]] < c_b)
            if( p[pixel[10]] < c_b)
             if( p[pixel[11]] < c_b)
              if( p[pixel[12]] < c_b)
               if( p[pixel[13]] < c_b)
                if( p[pixel[14]] < c_b)
                 if( p[pixel[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[2]] < c_b)
               if( p[pixel[3]] < c_b)
                if( p[pixel[4]] < c_b)
                 if( p[pixel[5]] < c_b)
                  if( p[pixel[6]] < c_b)
                   if( p[pixel[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
        else
         if( p[pixel[7]] > cb)
          if( p[pixel[8]] > cb)
           if( p[pixel[9]] > cb)
            if( p[pixel[6]] > cb)
             if( p[pixel[5]] > cb)
              if( p[pixel[4]] > cb)
               if( p[pixel[3]] > cb)
                if( p[pixel[2]] > cb)
                 if( p[pixel[1]] > cb)
                  goto is_a_corner;
                 else
                  if( p[pixel[10]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[10]] > cb)
                  if( p[pixel[11]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[10]] > cb)
                 if( p[pixel[11]] > cb)
                  if( p[pixel[12]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[10]] > cb)
                if( p[pixel[11]] > cb)
                 if( p[pixel[12]] > cb)
                  if( p[pixel[13]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[10]] > cb)
               if( p[pixel[11]] > cb)
                if( p[pixel[12]] > cb)
                 if( p[pixel[13]] > cb)
                  if( p[pixel[14]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[pixel[10]] > cb)
              if( p[pixel[11]] > cb)
               if( p[pixel[12]] > cb)
                if( p[pixel[13]] > cb)
                 if( p[pixel[14]] > cb)
                  if( p[pixel[15]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else if( p[pixel[7]] < c_b)
          if( p[pixel[8]] < c_b)
           if( p[pixel[9]] < c_b)
            if( p[pixel[6]] < c_b)
             if( p[pixel[5]] < c_b)
              if( p[pixel[4]] < c_b)
               if( p[pixel[3]] < c_b)
                if( p[pixel[2]] < c_b)
                 if( p[pixel[1]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[pixel[10]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[pixel[10]] < c_b)
                  if( p[pixel[11]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[pixel[10]] < c_b)
                 if( p[pixel[11]] < c_b)
                  if( p[pixel[12]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[pixel[10]] < c_b)
                if( p[pixel[11]] < c_b)
                 if( p[pixel[12]] < c_b)
                  if( p[pixel[13]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[pixel[10]] < c_b)
               if( p[pixel[11]] < c_b)
                if( p[pixel[12]] < c_b)
                 if( p[pixel[13]] < c_b)
                  if( p[pixel[14]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[pixel[10]] < c_b)
              if( p[pixel[11]] < c_b)
               if( p[pixel[12]] < c_b)
                if( p[pixel[13]] < c_b)
                 if( p[pixel[14]] < c_b)
                  if( p[pixel[15]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else
          goto is_not_a_corner;

		is_a_corner:
			bmin=b;
			goto end_if;

		is_not_a_corner:
			bmax=b;
			goto end_if;

		end_if:

		if(bmin == bmax - 1 || bmin == bmax)
			return bmin;
		b = (bmin + bmax) / 2;
    }
}

static void fast9_score(image_t *image, corner_t *corners, int num_corners, int b)
{
    for (int i=0; i<num_corners; i++) {
        corner_t *c = &corners[i];
        c->score = fast9_corner_score(image->pixels + c->y*image->w + c->x, b);
    }
}

static corner_t *fast9_detect(image_t *image, rectangle_t *roi, int *n_corners, int b)
{
    int num_corners = 0;
    // Try to alloc MAX_CORNERS or the actual max corners we can alloc.
    int max_corners = IM_MIN(MAX_CORNERS, (fb_avail() / sizeof(corner_t)));
    corner_t *corners = (corner_t*) fb_alloc(max_corners * sizeof(corner_t), FB_ALLOC_NO_HINT);

    for(int y=roi->y+3; y<roi->y+roi->h-3; y++) {
        for(int x=roi->x+3; x<roi->x+roi->w-3; x++) {
            const uint8_t *p = image->pixels+(y * image->w + x);
			int cb = *p + b;
			int c_b= *p - b;
            if(p[pixel[0]] > cb)
             if(p[pixel[1]] > cb)
              if(p[pixel[2]] > cb)
               if(p[pixel[3]] > cb)
                if(p[pixel[4]] > cb)
                 if(p[pixel[5]] > cb)
                  if(p[pixel[6]] > cb)
                   if(p[pixel[7]] > cb)
                    if(p[pixel[8]] > cb)
                     {}
                    else
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      continue;
                   else if(p[pixel[7]] < c_b)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      continue;
                    else if(p[pixel[14]] < c_b)
                     if(p[pixel[8]] < c_b)
                      if(p[pixel[9]] < c_b)
                       if(p[pixel[10]] < c_b)
                        if(p[pixel[11]] < c_b)
                         if(p[pixel[12]] < c_b)
                          if(p[pixel[13]] < c_b)
                           if(p[pixel[15]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      continue;
                    else
                     continue;
                  else if(p[pixel[6]] < c_b)
                   if(p[pixel[15]] > cb)
                    if(p[pixel[13]] > cb)
                     if(p[pixel[14]] > cb)
                      {}
                     else
                      continue;
                    else if(p[pixel[13]] < c_b)
                     if(p[pixel[7]] < c_b)
                      if(p[pixel[8]] < c_b)
                       if(p[pixel[9]] < c_b)
                        if(p[pixel[10]] < c_b)
                         if(p[pixel[11]] < c_b)
                          if(p[pixel[12]] < c_b)
                           if(p[pixel[14]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    if(p[pixel[7]] < c_b)
                     if(p[pixel[8]] < c_b)
                      if(p[pixel[9]] < c_b)
                       if(p[pixel[10]] < c_b)
                        if(p[pixel[11]] < c_b)
                         if(p[pixel[12]] < c_b)
                          if(p[pixel[13]] < c_b)
                           if(p[pixel[14]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      continue;
                    else
                     continue;
                   else if(p[pixel[13]] < c_b)
                    if(p[pixel[7]] < c_b)
                     if(p[pixel[8]] < c_b)
                      if(p[pixel[9]] < c_b)
                       if(p[pixel[10]] < c_b)
                        if(p[pixel[11]] < c_b)
                         if(p[pixel[12]] < c_b)
                          if(p[pixel[14]] < c_b)
                           if(p[pixel[15]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else if(p[pixel[5]] < c_b)
                  if(p[pixel[14]] > cb)
                   if(p[pixel[12]] > cb)
                    if(p[pixel[13]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           if(p[pixel[11]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else if(p[pixel[12]] < c_b)
                    if(p[pixel[6]] < c_b)
                     if(p[pixel[7]] < c_b)
                      if(p[pixel[8]] < c_b)
                       if(p[pixel[9]] < c_b)
                        if(p[pixel[10]] < c_b)
                         if(p[pixel[11]] < c_b)
                          if(p[pixel[13]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else if(p[pixel[14]] < c_b)
                   if(p[pixel[7]] < c_b)
                    if(p[pixel[8]] < c_b)
                     if(p[pixel[9]] < c_b)
                      if(p[pixel[10]] < c_b)
                       if(p[pixel[11]] < c_b)
                        if(p[pixel[12]] < c_b)
                         if(p[pixel[13]] < c_b)
                          if(p[pixel[6]] < c_b)
                           {}
                          else
                           if(p[pixel[15]] < c_b)
                            {}
                           else
                            continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   if(p[pixel[6]] < c_b)
                    if(p[pixel[7]] < c_b)
                     if(p[pixel[8]] < c_b)
                      if(p[pixel[9]] < c_b)
                       if(p[pixel[10]] < c_b)
                        if(p[pixel[11]] < c_b)
                         if(p[pixel[12]] < c_b)
                          if(p[pixel[13]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           if(p[pixel[11]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else
                    continue;
                  else if(p[pixel[12]] < c_b)
                   if(p[pixel[7]] < c_b)
                    if(p[pixel[8]] < c_b)
                     if(p[pixel[9]] < c_b)
                      if(p[pixel[10]] < c_b)
                       if(p[pixel[11]] < c_b)
                        if(p[pixel[13]] < c_b)
                         if(p[pixel[14]] < c_b)
                          if(p[pixel[6]] < c_b)
                           {}
                          else
                           if(p[pixel[15]] < c_b)
                            {}
                           else
                            continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else if(p[pixel[4]] < c_b)
                 if(p[pixel[13]] > cb)
                  if(p[pixel[11]] > cb)
                   if(p[pixel[12]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else if(p[pixel[11]] < c_b)
                   if(p[pixel[5]] < c_b)
                    if(p[pixel[6]] < c_b)
                     if(p[pixel[7]] < c_b)
                      if(p[pixel[8]] < c_b)
                       if(p[pixel[9]] < c_b)
                        if(p[pixel[10]] < c_b)
                         if(p[pixel[12]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else if(p[pixel[13]] < c_b)
                  if(p[pixel[7]] < c_b)
                   if(p[pixel[8]] < c_b)
                    if(p[pixel[9]] < c_b)
                     if(p[pixel[10]] < c_b)
                      if(p[pixel[11]] < c_b)
                       if(p[pixel[12]] < c_b)
                        if(p[pixel[6]] < c_b)
                         if(p[pixel[5]] < c_b)
                          {}
                         else
                          if(p[pixel[14]] < c_b)
                           {}
                          else
                           continue;
                        else
                         if(p[pixel[14]] < c_b)
                          if(p[pixel[15]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  if(p[pixel[5]] < c_b)
                   if(p[pixel[6]] < c_b)
                    if(p[pixel[7]] < c_b)
                     if(p[pixel[8]] < c_b)
                      if(p[pixel[9]] < c_b)
                       if(p[pixel[10]] < c_b)
                        if(p[pixel[11]] < c_b)
                         if(p[pixel[12]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          if(p[pixel[10]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else
                   continue;
                 else if(p[pixel[11]] < c_b)
                  if(p[pixel[7]] < c_b)
                   if(p[pixel[8]] < c_b)
                    if(p[pixel[9]] < c_b)
                     if(p[pixel[10]] < c_b)
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        if(p[pixel[6]] < c_b)
                         if(p[pixel[5]] < c_b)
                          {}
                         else
                          if(p[pixel[14]] < c_b)
                           {}
                          else
                           continue;
                        else
                         if(p[pixel[14]] < c_b)
                          if(p[pixel[15]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
               else if(p[pixel[3]] < c_b)
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else if(p[pixel[10]] < c_b)
                 if(p[pixel[7]] < c_b)
                  if(p[pixel[8]] < c_b)
                   if(p[pixel[9]] < c_b)
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[6]] < c_b)
                      if(p[pixel[5]] < c_b)
                       if(p[pixel[4]] < c_b)
                        {}
                       else
                        if(p[pixel[12]] < c_b)
                         if(p[pixel[13]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                      else
                       if(p[pixel[12]] < c_b)
                        if(p[pixel[13]] < c_b)
                         if(p[pixel[14]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        if(p[pixel[14]] < c_b)
                         if(p[pixel[15]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         if(p[pixel[9]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else if(p[pixel[10]] < c_b)
                 if(p[pixel[7]] < c_b)
                  if(p[pixel[8]] < c_b)
                   if(p[pixel[9]] < c_b)
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[5]] < c_b)
                        if(p[pixel[4]] < c_b)
                         {}
                        else
                         if(p[pixel[13]] < c_b)
                          {}
                         else
                          continue;
                       else
                        if(p[pixel[13]] < c_b)
                         if(p[pixel[14]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                      else
                       if(p[pixel[13]] < c_b)
                        if(p[pixel[14]] < c_b)
                         if(p[pixel[15]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
              else if(p[pixel[2]] < c_b)
               if(p[pixel[9]] > cb)
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else if(p[pixel[9]] < c_b)
                if(p[pixel[7]] < c_b)
                 if(p[pixel[8]] < c_b)
                  if(p[pixel[10]] < c_b)
                   if(p[pixel[6]] < c_b)
                    if(p[pixel[5]] < c_b)
                     if(p[pixel[4]] < c_b)
                      if(p[pixel[3]] < c_b)
                       {}
                      else
                       if(p[pixel[11]] < c_b)
                        if(p[pixel[12]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[11]] < c_b)
                       if(p[pixel[12]] < c_b)
                        if(p[pixel[13]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[11]] < c_b)
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        if(p[pixel[14]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[13]] < c_b)
                       if(p[pixel[14]] < c_b)
                        if(p[pixel[15]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
              else
               if(p[pixel[9]] > cb)
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        if(p[pixel[8]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else if(p[pixel[9]] < c_b)
                if(p[pixel[7]] < c_b)
                 if(p[pixel[8]] < c_b)
                  if(p[pixel[10]] < c_b)
                   if(p[pixel[11]] < c_b)
                    if(p[pixel[6]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[4]] < c_b)
                       if(p[pixel[3]] < c_b)
                        {}
                       else
                        if(p[pixel[12]] < c_b)
                         {}
                        else
                         continue;
                      else
                       if(p[pixel[12]] < c_b)
                        if(p[pixel[13]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        if(p[pixel[14]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[13]] < c_b)
                       if(p[pixel[14]] < c_b)
                        if(p[pixel[15]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
             else if(p[pixel[1]] < c_b)
              if(p[pixel[8]] > cb)
               if(p[pixel[9]] > cb)
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[2]] > cb)
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else if(p[pixel[8]] < c_b)
               if(p[pixel[7]] < c_b)
                if(p[pixel[9]] < c_b)
                 if(p[pixel[6]] < c_b)
                  if(p[pixel[5]] < c_b)
                   if(p[pixel[4]] < c_b)
                    if(p[pixel[3]] < c_b)
                     if(p[pixel[2]] < c_b)
                      {}
                     else
                      if(p[pixel[10]] < c_b)
                       if(p[pixel[11]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[10]] < c_b)
                      if(p[pixel[11]] < c_b)
                       if(p[pixel[12]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[10]] < c_b)
                     if(p[pixel[11]] < c_b)
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[10]] < c_b)
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[13]] < c_b)
                       if(p[pixel[14]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[10]] < c_b)
                   if(p[pixel[11]] < c_b)
                    if(p[pixel[12]] < c_b)
                     if(p[pixel[13]] < c_b)
                      if(p[pixel[14]] < c_b)
                       if(p[pixel[15]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else
               continue;
             else
              if(p[pixel[8]] > cb)
               if(p[pixel[9]] > cb)
                if(p[pixel[10]] > cb)
                 if(p[pixel[11]] > cb)
                  if(p[pixel[12]] > cb)
                   if(p[pixel[13]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[15]] > cb)
                      {}
                     else
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[2]] > cb)
                   if(p[pixel[3]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[7]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else if(p[pixel[8]] < c_b)
               if(p[pixel[7]] < c_b)
                if(p[pixel[9]] < c_b)
                 if(p[pixel[10]] < c_b)
                  if(p[pixel[6]] < c_b)
                   if(p[pixel[5]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[3]] < c_b)
                      if(p[pixel[2]] < c_b)
                       {}
                      else
                       if(p[pixel[11]] < c_b)
                        {}
                       else
                        continue;
                     else
                      if(p[pixel[11]] < c_b)
                       if(p[pixel[12]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[11]] < c_b)
                      if(p[pixel[12]] < c_b)
                       if(p[pixel[13]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[13]] < c_b)
                       if(p[pixel[14]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[11]] < c_b)
                    if(p[pixel[12]] < c_b)
                     if(p[pixel[13]] < c_b)
                      if(p[pixel[14]] < c_b)
                       if(p[pixel[15]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
              else
               continue;
            else if(p[pixel[0]] < c_b)
             if(p[pixel[1]] > cb)
              if(p[pixel[8]] > cb)
               if(p[pixel[7]] > cb)
                if(p[pixel[9]] > cb)
                 if(p[pixel[6]] > cb)
                  if(p[pixel[5]] > cb)
                   if(p[pixel[4]] > cb)
                    if(p[pixel[3]] > cb)
                     if(p[pixel[2]] > cb)
                      {}
                     else
                      if(p[pixel[10]] > cb)
                       if(p[pixel[11]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[10]] > cb)
                      if(p[pixel[11]] > cb)
                       if(p[pixel[12]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[10]] > cb)
                     if(p[pixel[11]] > cb)
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[10]] > cb)
                    if(p[pixel[11]] > cb)
                     if(p[pixel[12]] > cb)
                      if(p[pixel[13]] > cb)
                       if(p[pixel[14]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[10]] > cb)
                   if(p[pixel[11]] > cb)
                    if(p[pixel[12]] > cb)
                     if(p[pixel[13]] > cb)
                      if(p[pixel[14]] > cb)
                       if(p[pixel[15]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else if(p[pixel[8]] < c_b)
               if(p[pixel[9]] < c_b)
                if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[2]] < c_b)
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else
               continue;
             else if(p[pixel[1]] < c_b)
              if(p[pixel[2]] > cb)
               if(p[pixel[9]] > cb)
                if(p[pixel[7]] > cb)
                 if(p[pixel[8]] > cb)
                  if(p[pixel[10]] > cb)
                   if(p[pixel[6]] > cb)
                    if(p[pixel[5]] > cb)
                     if(p[pixel[4]] > cb)
                      if(p[pixel[3]] > cb)
                       {}
                      else
                       if(p[pixel[11]] > cb)
                        if(p[pixel[12]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[11]] > cb)
                       if(p[pixel[12]] > cb)
                        if(p[pixel[13]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[11]] > cb)
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        if(p[pixel[14]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[11]] > cb)
                     if(p[pixel[12]] > cb)
                      if(p[pixel[13]] > cb)
                       if(p[pixel[14]] > cb)
                        if(p[pixel[15]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else if(p[pixel[9]] < c_b)
                if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
              else if(p[pixel[2]] < c_b)
               if(p[pixel[3]] > cb)
                if(p[pixel[10]] > cb)
                 if(p[pixel[7]] > cb)
                  if(p[pixel[8]] > cb)
                   if(p[pixel[9]] > cb)
                    if(p[pixel[11]] > cb)
                     if(p[pixel[6]] > cb)
                      if(p[pixel[5]] > cb)
                       if(p[pixel[4]] > cb)
                        {}
                       else
                        if(p[pixel[12]] > cb)
                         if(p[pixel[13]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                      else
                       if(p[pixel[12]] > cb)
                        if(p[pixel[13]] > cb)
                         if(p[pixel[14]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        if(p[pixel[14]] > cb)
                         if(p[pixel[15]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else if(p[pixel[3]] < c_b)
                if(p[pixel[4]] > cb)
                 if(p[pixel[13]] > cb)
                  if(p[pixel[7]] > cb)
                   if(p[pixel[8]] > cb)
                    if(p[pixel[9]] > cb)
                     if(p[pixel[10]] > cb)
                      if(p[pixel[11]] > cb)
                       if(p[pixel[12]] > cb)
                        if(p[pixel[6]] > cb)
                         if(p[pixel[5]] > cb)
                          {}
                         else
                          if(p[pixel[14]] > cb)
                           {}
                          else
                           continue;
                        else
                         if(p[pixel[14]] > cb)
                          if(p[pixel[15]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else if(p[pixel[13]] < c_b)
                  if(p[pixel[11]] > cb)
                   if(p[pixel[5]] > cb)
                    if(p[pixel[6]] > cb)
                     if(p[pixel[7]] > cb)
                      if(p[pixel[8]] > cb)
                       if(p[pixel[9]] > cb)
                        if(p[pixel[10]] > cb)
                         if(p[pixel[12]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else if(p[pixel[11]] < c_b)
                   if(p[pixel[12]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  if(p[pixel[5]] > cb)
                   if(p[pixel[6]] > cb)
                    if(p[pixel[7]] > cb)
                     if(p[pixel[8]] > cb)
                      if(p[pixel[9]] > cb)
                       if(p[pixel[10]] > cb)
                        if(p[pixel[11]] > cb)
                         if(p[pixel[12]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else if(p[pixel[4]] < c_b)
                 if(p[pixel[5]] > cb)
                  if(p[pixel[14]] > cb)
                   if(p[pixel[7]] > cb)
                    if(p[pixel[8]] > cb)
                     if(p[pixel[9]] > cb)
                      if(p[pixel[10]] > cb)
                       if(p[pixel[11]] > cb)
                        if(p[pixel[12]] > cb)
                         if(p[pixel[13]] > cb)
                          if(p[pixel[6]] > cb)
                           {}
                          else
                           if(p[pixel[15]] > cb)
                            {}
                           else
                            continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else if(p[pixel[14]] < c_b)
                   if(p[pixel[12]] > cb)
                    if(p[pixel[6]] > cb)
                     if(p[pixel[7]] > cb)
                      if(p[pixel[8]] > cb)
                       if(p[pixel[9]] > cb)
                        if(p[pixel[10]] > cb)
                         if(p[pixel[11]] > cb)
                          if(p[pixel[13]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else if(p[pixel[12]] < c_b)
                    if(p[pixel[13]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           if(p[pixel[11]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   if(p[pixel[6]] > cb)
                    if(p[pixel[7]] > cb)
                     if(p[pixel[8]] > cb)
                      if(p[pixel[9]] > cb)
                       if(p[pixel[10]] > cb)
                        if(p[pixel[11]] > cb)
                         if(p[pixel[12]] > cb)
                          if(p[pixel[13]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else if(p[pixel[5]] < c_b)
                  if(p[pixel[6]] > cb)
                   if(p[pixel[15]] < c_b)
                    if(p[pixel[13]] > cb)
                     if(p[pixel[7]] > cb)
                      if(p[pixel[8]] > cb)
                       if(p[pixel[9]] > cb)
                        if(p[pixel[10]] > cb)
                         if(p[pixel[11]] > cb)
                          if(p[pixel[12]] > cb)
                           if(p[pixel[14]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else if(p[pixel[13]] < c_b)
                     if(p[pixel[14]] < c_b)
                      {}
                     else
                      continue;
                    else
                     continue;
                   else
                    if(p[pixel[7]] > cb)
                     if(p[pixel[8]] > cb)
                      if(p[pixel[9]] > cb)
                       if(p[pixel[10]] > cb)
                        if(p[pixel[11]] > cb)
                         if(p[pixel[12]] > cb)
                          if(p[pixel[13]] > cb)
                           if(p[pixel[14]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else if(p[pixel[6]] < c_b)
                   if(p[pixel[7]] > cb)
                    if(p[pixel[14]] > cb)
                     if(p[pixel[8]] > cb)
                      if(p[pixel[9]] > cb)
                       if(p[pixel[10]] > cb)
                        if(p[pixel[11]] > cb)
                         if(p[pixel[12]] > cb)
                          if(p[pixel[13]] > cb)
                           if(p[pixel[15]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      continue;
                    else
                     continue;
                   else if(p[pixel[7]] < c_b)
                    if(p[pixel[8]] < c_b)
                     {}
                    else
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      continue;
                   else
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[13]] > cb)
                    if(p[pixel[7]] > cb)
                     if(p[pixel[8]] > cb)
                      if(p[pixel[9]] > cb)
                       if(p[pixel[10]] > cb)
                        if(p[pixel[11]] > cb)
                         if(p[pixel[12]] > cb)
                          if(p[pixel[14]] > cb)
                           if(p[pixel[15]] > cb)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[12]] > cb)
                   if(p[pixel[7]] > cb)
                    if(p[pixel[8]] > cb)
                     if(p[pixel[9]] > cb)
                      if(p[pixel[10]] > cb)
                       if(p[pixel[11]] > cb)
                        if(p[pixel[13]] > cb)
                         if(p[pixel[14]] > cb)
                          if(p[pixel[6]] > cb)
                           {}
                          else
                           if(p[pixel[15]] > cb)
                            {}
                           else
                            continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           if(p[pixel[11]] < c_b)
                            {}
                           else
                            continue;
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 if(p[pixel[11]] > cb)
                  if(p[pixel[7]] > cb)
                   if(p[pixel[8]] > cb)
                    if(p[pixel[9]] > cb)
                     if(p[pixel[10]] > cb)
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        if(p[pixel[6]] > cb)
                         if(p[pixel[5]] > cb)
                          {}
                         else
                          if(p[pixel[14]] > cb)
                           {}
                          else
                           continue;
                        else
                         if(p[pixel[14]] > cb)
                          if(p[pixel[15]] > cb)
                           {}
                          else
                           continue;
                         else
                          continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          if(p[pixel[10]] < c_b)
                           {}
                          else
                           continue;
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
               else
                if(p[pixel[10]] > cb)
                 if(p[pixel[7]] > cb)
                  if(p[pixel[8]] > cb)
                   if(p[pixel[9]] > cb)
                    if(p[pixel[11]] > cb)
                     if(p[pixel[12]] > cb)
                      if(p[pixel[6]] > cb)
                       if(p[pixel[5]] > cb)
                        if(p[pixel[4]] > cb)
                         {}
                        else
                         if(p[pixel[13]] > cb)
                          {}
                         else
                          continue;
                       else
                        if(p[pixel[13]] > cb)
                         if(p[pixel[14]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                      else
                       if(p[pixel[13]] > cb)
                        if(p[pixel[14]] > cb)
                         if(p[pixel[15]] > cb)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         if(p[pixel[9]] < c_b)
                          {}
                         else
                          continue;
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
              else
               if(p[pixel[9]] > cb)
                if(p[pixel[7]] > cb)
                 if(p[pixel[8]] > cb)
                  if(p[pixel[10]] > cb)
                   if(p[pixel[11]] > cb)
                    if(p[pixel[6]] > cb)
                     if(p[pixel[5]] > cb)
                      if(p[pixel[4]] > cb)
                       if(p[pixel[3]] > cb)
                        {}
                       else
                        if(p[pixel[12]] > cb)
                         {}
                        else
                         continue;
                      else
                       if(p[pixel[12]] > cb)
                        if(p[pixel[13]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                     else
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        if(p[pixel[14]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[12]] > cb)
                      if(p[pixel[13]] > cb)
                       if(p[pixel[14]] > cb)
                        if(p[pixel[15]] > cb)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
                else
                 continue;
               else if(p[pixel[9]] < c_b)
                if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        if(p[pixel[8]] < c_b)
                         {}
                        else
                         continue;
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
             else
              if(p[pixel[8]] > cb)
               if(p[pixel[7]] > cb)
                if(p[pixel[9]] > cb)
                 if(p[pixel[10]] > cb)
                  if(p[pixel[6]] > cb)
                   if(p[pixel[5]] > cb)
                    if(p[pixel[4]] > cb)
                     if(p[pixel[3]] > cb)
                      if(p[pixel[2]] > cb)
                       {}
                      else
                       if(p[pixel[11]] > cb)
                        {}
                       else
                        continue;
                     else
                      if(p[pixel[11]] > cb)
                       if(p[pixel[12]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[11]] > cb)
                      if(p[pixel[12]] > cb)
                       if(p[pixel[13]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[11]] > cb)
                     if(p[pixel[12]] > cb)
                      if(p[pixel[13]] > cb)
                       if(p[pixel[14]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[11]] > cb)
                    if(p[pixel[12]] > cb)
                     if(p[pixel[13]] > cb)
                      if(p[pixel[14]] > cb)
                       if(p[pixel[15]] > cb)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  continue;
                else
                 continue;
               else
                continue;
              else if(p[pixel[8]] < c_b)
               if(p[pixel[9]] < c_b)
                if(p[pixel[10]] < c_b)
                 if(p[pixel[11]] < c_b)
                  if(p[pixel[12]] < c_b)
                   if(p[pixel[13]] < c_b)
                    if(p[pixel[14]] < c_b)
                     if(p[pixel[15]] < c_b)
                      {}
                     else
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                    else
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[2]] < c_b)
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[4]] < c_b)
                     if(p[pixel[5]] < c_b)
                      if(p[pixel[6]] < c_b)
                       if(p[pixel[7]] < c_b)
                        {}
                       else
                        continue;
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 continue;
               else
                continue;
              else
               continue;
            else
             if(p[pixel[7]] > cb)
              if(p[pixel[8]] > cb)
               if(p[pixel[9]] > cb)
                if(p[pixel[6]] > cb)
                 if(p[pixel[5]] > cb)
                  if(p[pixel[4]] > cb)
                   if(p[pixel[3]] > cb)
                    if(p[pixel[2]] > cb)
                     if(p[pixel[1]] > cb)
                      {}
                     else
                      if(p[pixel[10]] > cb)
                       {}
                      else
                       continue;
                    else
                     if(p[pixel[10]] > cb)
                      if(p[pixel[11]] > cb)
                       {}
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[10]] > cb)
                     if(p[pixel[11]] > cb)
                      if(p[pixel[12]] > cb)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[10]] > cb)
                    if(p[pixel[11]] > cb)
                     if(p[pixel[12]] > cb)
                      if(p[pixel[13]] > cb)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[10]] > cb)
                   if(p[pixel[11]] > cb)
                    if(p[pixel[12]] > cb)
                     if(p[pixel[13]] > cb)
                      if(p[pixel[14]] > cb)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 if(p[pixel[10]] > cb)
                  if(p[pixel[11]] > cb)
                   if(p[pixel[12]] > cb)
                    if(p[pixel[13]] > cb)
                     if(p[pixel[14]] > cb)
                      if(p[pixel[15]] > cb)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
               else
                continue;
              else
               continue;
             else if(p[pixel[7]] < c_b)
              if(p[pixel[8]] < c_b)
               if(p[pixel[9]] < c_b)
                if(p[pixel[6]] < c_b)
                 if(p[pixel[5]] < c_b)
                  if(p[pixel[4]] < c_b)
                   if(p[pixel[3]] < c_b)
                    if(p[pixel[2]] < c_b)
                     if(p[pixel[1]] < c_b)
                      {}
                     else
                      if(p[pixel[10]] < c_b)
                       {}
                      else
                       continue;
                    else
                     if(p[pixel[10]] < c_b)
                      if(p[pixel[11]] < c_b)
                       {}
                      else
                       continue;
                     else
                      continue;
                   else
                    if(p[pixel[10]] < c_b)
                     if(p[pixel[11]] < c_b)
                      if(p[pixel[12]] < c_b)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                  else
                   if(p[pixel[10]] < c_b)
                    if(p[pixel[11]] < c_b)
                     if(p[pixel[12]] < c_b)
                      if(p[pixel[13]] < c_b)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                 else
                  if(p[pixel[10]] < c_b)
                   if(p[pixel[11]] < c_b)
                    if(p[pixel[12]] < c_b)
                     if(p[pixel[13]] < c_b)
                      if(p[pixel[14]] < c_b)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                else
                 if(p[pixel[10]] < c_b)
                  if(p[pixel[11]] < c_b)
                   if(p[pixel[12]] < c_b)
                    if(p[pixel[13]] < c_b)
                     if(p[pixel[14]] < c_b)
                      if(p[pixel[15]] < c_b)
                       {}
                      else
                       continue;
                     else
                      continue;
                    else
                     continue;
                   else
                    continue;
                  else
                   continue;
                 else
                  continue;
               else
                continue;
              else
               continue;
             else
              continue;

            // Add corner
            corners[num_corners].x = x;
            corners[num_corners].y = y;

            if (++num_corners == max_corners) {
                goto done;
            }
		}
    }

done:
    *n_corners = num_corners;
    return corners;
}
#endif //IMLIB_ENABLE_FAST
