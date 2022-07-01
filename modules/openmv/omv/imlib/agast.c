/*
 * NOTE: This code is mostly auto-generated.
 * See: http://archive.www6.in.tum.de/www6/Main/ResearchAgast.html
 *
 * Copyright (C) 2010 Elmar Mair
 * The program you can download here is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the
 * GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.
 */
#include <stdio.h>
#include <stdint.h>
#include "imlib.h"
#include "xalloc.h"
#include "fb_alloc.h"
#include "py/gc.h"

#define MAX_ROW         (480)
#define MAX_CORNERS     (2000)
#define Compare(X, Y) ((X)>=(Y))

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t score;
} corner_t;

static int s_width=-1;
static int_fast16_t s_offset0;
static int_fast16_t s_offset1;
static int_fast16_t s_offset2;
static int_fast16_t s_offset3;
static int_fast16_t s_offset4;
static int_fast16_t s_offset5;
static int_fast16_t s_offset6;
static int_fast16_t s_offset7;

static corner_t *agast58_detect(image_t *img, int b, int* num_corners, rectangle_t *roi);
static int agast58_score(const unsigned char* p, int bstart);
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

static void init5_8_pattern(int image_width)
{
	if(image_width==s_width)
		return;

	s_width=image_width;

	s_offset0=(-1)+(0)*s_width;
	s_offset1=(-1)+(-1)*s_width;
	s_offset2=(0)+(-1)*s_width;
	s_offset3=(1)+(-1)*s_width;
	s_offset4=(1)+(0)*s_width;
	s_offset5=(1)+(1)*s_width;
	s_offset6=(0)+(1)*s_width;
	s_offset7=(-1)+(1)*s_width;
}

void agast_detect(image_t *image, array_t *keypoints, int threshold, rectangle_t *roi)
{
    int num_corners=0;
	init5_8_pattern(image->w);

    // Find corners
    corner_t *corners = agast58_detect(image, threshold, &num_corners, roi);
    if (num_corners) {
        // Score corners
        for(int i=0; i<num_corners; i++) {
            corners[i].score = agast58_score(image->pixels + (corners[i].y*image->w + corners[i].x), threshold);
        }
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

static corner_t *agast58_detect(image_t *img, int b, int* num_corners, rectangle_t *roi)
{
	int total=0;
	register int x, y;
	register int xsizeB=(roi->x+roi->w) - 2;
	register int ysizeB=(roi->y+roi->h) - 1;
	register int_fast16_t offset0, offset1, offset2, offset3, offset4, offset5, offset6, offset7;
	register int width;

	offset0=s_offset0;
	offset1=s_offset1;
	offset2=s_offset2;
	offset3=s_offset3;
	offset4=s_offset4;
	offset5=s_offset5;
	offset6=s_offset6;
	offset7=s_offset7;
	width=s_width;

    // Try to alloc MAX_CORNERS or the actual max corners we can alloc.
    int max_corners = IM_MIN(MAX_CORNERS, (fb_avail() / sizeof(corner_t)));
    corner_t *corners = (corner_t*) fb_alloc(max_corners * sizeof(corner_t), FB_ALLOC_NO_HINT);

	for(y=roi->y+1; y < ysizeB; y++)
	{										
		x=roi->x;
		while(1)							
		{									
homogeneous:
{
			x++;			
			if(x>xsizeB)	
				break;
			else
			{
				register const unsigned char* const p = img->pixels + y*width + x;
				register const int cb = *p + b;
				register const int c_b = *p - b;
				if(p[offset0] > cb)
				  if(p[offset2] > cb)
					if(p[offset3] > cb)
					  if(p[offset5] > cb)
						if(p[offset1] > cb)
						  if(p[offset4] > cb)
							goto success_structured;
						  else
							if(p[offset7] > cb)
							  goto success_structured;
							else
							  goto homogeneous;
						else
						  if(p[offset4] > cb)
							if(p[offset6] > cb)
							  goto success_structured;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						if(p[offset1] > cb)
						  if(p[offset4] > cb)
							goto success_homogeneous;
						  else
							if(p[offset7] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
					else
					  if(p[offset7] > cb)
						if(p[offset6] > cb)
						  if(p[offset5] > cb)
							if(p[offset1] > cb)
							  goto success_structured;
							else
							  if(p[offset4] > cb)
								goto success_structured;
							  else
								goto homogeneous;
						  else
							if(p[offset1] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
					  else
						if(p[offset5] < c_b)
						  if(p[offset3] < c_b)
							if(p[offset7] < c_b)
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto structured;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto homogeneous;
				  else
					if(p[offset5] > cb)
					  if(p[offset7] > cb)
						if(p[offset6] > cb)
						  if(p[offset1] > cb)
							goto success_homogeneous;
						  else
							if(p[offset4] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
					  else
						goto homogeneous;
					else
					  if(p[offset5] < c_b)
						if(p[offset3] < c_b)
						  if(p[offset2] < c_b)
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_structured;
							  else
								goto homogeneous;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto homogeneous;
							  else
								goto homogeneous;
						  else
							if(p[offset7] < c_b)
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
					  else
						goto homogeneous;
				else if(p[offset0] < c_b)
				  if(p[offset2] < c_b)
					if(p[offset7] > cb)
					  if(p[offset3] < c_b)
						if(p[offset5] < c_b)
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_structured;
							else
							  goto structured;
						  else
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_structured;
							  else
								goto structured;
							else
							  goto homogeneous;
						else
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_structured;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						if(p[offset5] > cb)
						  if(p[offset3] > cb)
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto structured;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto homogeneous;
					else
					  if(p[offset7] < c_b)
						if(p[offset3] < c_b)
						  if(p[offset5] < c_b)
							if(p[offset1] < c_b)
							  goto success_structured;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto structured;
							  else
								goto homogeneous;
						  else
							if(p[offset1] < c_b)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						else
						  if(p[offset6] < c_b)
							if(p[offset5] < c_b)
							  if(p[offset1] < c_b)
								goto success_structured;
							  else
								if(p[offset4] < c_b)
								  goto success_structured;
								else
								  goto homogeneous;
							else
							  if(p[offset1] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
						  else
							goto homogeneous;
					  else
						if(p[offset3] < c_b)
						  if(p[offset5] < c_b)
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_structured;
							  else
								goto homogeneous;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto homogeneous;
							  else
								goto homogeneous;
						  else
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
				  else
					if(p[offset5] > cb)
					  if(p[offset3] > cb)
						if(p[offset2] > cb)
						  if(p[offset1] > cb)
							if(p[offset4] > cb)
							  goto success_structured;
							else
							  goto homogeneous;
						  else
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  if(p[offset7] > cb)
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						goto homogeneous;
					else
					  if(p[offset5] < c_b)
						if(p[offset7] < c_b)
						  if(p[offset6] < c_b)
							if(p[offset1] < c_b)
							  goto success_homogeneous;
							else
							  if(p[offset4] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto homogeneous;
					  else
						goto homogeneous;
				else
				  if(p[offset3] > cb)
					if(p[offset5] > cb)
					  if(p[offset2] > cb)
						if(p[offset1] > cb)
						  if(p[offset4] > cb)
							goto success_homogeneous;
						  else
							goto homogeneous;
						else
						  if(p[offset4] > cb)
							if(p[offset6] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						if(p[offset7] > cb)
						  if(p[offset4] > cb)
							if(p[offset6] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto homogeneous;
					else
					  goto homogeneous;
				  else
					if(p[offset3] < c_b)
					  if(p[offset5] < c_b)
						if(p[offset2] < c_b)
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  if(p[offset7] < c_b)
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						goto homogeneous;
					else
					  goto homogeneous;
			}
}
structured:
{
			x++;			
			if(x>xsizeB)	
				break;
			else
			{
				register const unsigned char* const p = img->pixels + y*width + x;
				register const int cb = *p + b;
				register const int c_b = *p - b;
				if(p[offset0] > cb)
				  if(p[offset2] > cb)
					if(p[offset3] > cb)
					  if(p[offset5] > cb)
						if(p[offset7] > cb)
						  if(p[offset1] > cb)
							goto success_structured;
						  else
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto structured;
							else
							  goto structured;
						else
						  if(p[offset1] > cb)
							if(p[offset4] > cb)
							  goto success_structured;
							else
							  goto structured;
						  else
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto structured;
							else
							  goto structured;
					  else
						if(p[offset7] > cb)
						  if(p[offset1] > cb)
							goto success_structured;
						  else
							goto structured;
						else
						  if(p[offset1] > cb)
							if(p[offset4] > cb)
							  goto success_structured;
							else
							  goto structured;
						  else
							goto structured;
					else
					  if(p[offset7] > cb)
						if(p[offset6] > cb)
						  if(p[offset5] > cb)
							if(p[offset1] > cb)
							  goto success_structured;
							else
							  if(p[offset4] > cb)
								goto success_structured;
							  else
								goto structured;
						  else
							if(p[offset1] > cb)
							  goto success_structured;
							else
							  goto structured;
						else
						  goto structured;
					  else
						if(p[offset5] < c_b)
						  if(p[offset3] < c_b)
							if(p[offset7] < c_b)
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto structured;
							  else
								goto structured;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto structured;
				  else
					if(p[offset5] > cb)
					  if(p[offset7] > cb)
						if(p[offset6] > cb)
						  if(p[offset1] > cb)
							goto success_structured;
						  else
							if(p[offset4] > cb)
							  goto success_structured;
							else
							  goto structured;
						else
						  goto structured;
					  else
						goto structured;
					else
					  if(p[offset5] < c_b)
						if(p[offset3] < c_b)
						  if(p[offset2] < c_b)
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_structured;
							  else
								goto structured;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto structured;
							  else
								goto structured;
						  else
							if(p[offset7] < c_b)
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_homogeneous;
								else
								  goto homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  goto structured;
					  else
						goto homogeneous;
				else if(p[offset0] < c_b)
				  if(p[offset2] < c_b)
					if(p[offset7] > cb)
					  if(p[offset3] < c_b)
						if(p[offset5] < c_b)
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_structured;
							else
							  goto structured;
						  else
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_structured;
							  else
								goto structured;
							else
							  goto structured;
						else
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_structured;
							else
							  goto structured;
						  else
							goto structured;
					  else
						if(p[offset5] > cb)
						  if(p[offset3] > cb)
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto structured;
							else
							  goto structured;
						  else
							goto homogeneous;
						else
						  goto structured;
					else
					  if(p[offset7] < c_b)
						if(p[offset3] < c_b)
						  if(p[offset5] < c_b)
							if(p[offset1] < c_b)
							  goto success_structured;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_structured;
								else
								  goto structured;
							  else
								goto structured;
						  else
							if(p[offset1] < c_b)
							  goto success_structured;
							else
							  goto structured;
						else
						  if(p[offset6] < c_b)
							if(p[offset5] < c_b)
							  if(p[offset1] < c_b)
								goto success_structured;
							  else
								if(p[offset4] < c_b)
								  goto success_structured;
								else
								  goto structured;
							else
							  if(p[offset1] < c_b)
								goto success_structured;
							  else
								goto structured;
						  else
							goto structured;
					  else
						if(p[offset3] < c_b)
						  if(p[offset5] < c_b)
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  if(p[offset4] < c_b)
								if(p[offset6] < c_b)
								  goto success_homogeneous;
								else
								  goto homogeneous;
							  else
								goto homogeneous;
						  else
							if(p[offset1] < c_b)
							  if(p[offset4] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  goto homogeneous;
				  else
					if(p[offset5] > cb)
					  if(p[offset3] > cb)
						if(p[offset2] > cb)
						  if(p[offset1] > cb)
							if(p[offset4] > cb)
							  goto success_structured;
							else
							  goto structured;
						  else
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_structured;
							  else
								goto structured;
							else
							  goto structured;
						else
						  if(p[offset7] > cb)
							if(p[offset4] > cb)
							  if(p[offset6] > cb)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						goto structured;
					else
					  if(p[offset5] < c_b)
						if(p[offset7] < c_b)
						  if(p[offset6] < c_b)
							if(p[offset1] < c_b)
							  goto success_structured;
							else
							  if(p[offset4] < c_b)
								goto success_structured;
							  else
								goto structured;
						  else
							goto structured;
						else
						  goto structured;
					  else
						goto homogeneous;
				else
				  if(p[offset3] > cb)
					if(p[offset5] > cb)
					  if(p[offset2] > cb)
						if(p[offset1] > cb)
						  if(p[offset4] > cb)
							goto success_homogeneous;
						  else
							goto homogeneous;
						else
						  if(p[offset4] > cb)
							if(p[offset6] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						if(p[offset7] > cb)
						  if(p[offset4] > cb)
							if(p[offset6] > cb)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
						else
						  goto homogeneous;
					else
					  goto homogeneous;
				  else
					if(p[offset3] < c_b)
					  if(p[offset5] < c_b)
						if(p[offset2] < c_b)
						  if(p[offset1] < c_b)
							if(p[offset4] < c_b)
							  goto success_homogeneous;
							else
							  goto homogeneous;
						  else
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						else
						  if(p[offset7] < c_b)
							if(p[offset4] < c_b)
							  if(p[offset6] < c_b)
								goto success_homogeneous;
							  else
								goto homogeneous;
							else
							  goto homogeneous;
						  else
							goto homogeneous;
					  else
						goto homogeneous;
					else
					  goto homogeneous;
			}
}
success_homogeneous:
			corners[total].x = x;				
			corners[total].y = y;				
			if(++total == max_corners) {
                goto done;
            }
			goto homogeneous;				
success_structured:
			corners[total].x = x;				
			corners[total].y = y;				
			if(++total == max_corners) {
                goto done;
            }
			goto structured;				
		}									
	}										
done:
	*num_corners = total;
	return corners;								
}

//using also bisection as propsed by Edward Rosten in FAST,
//but it is based on the OAST
static int agast58_score(const unsigned char* p, int bstart)
{
    int bmin = bstart;
    int bmax = 255;
    int b = (bmax + bmin)/2;

	register int_fast16_t offset0=s_offset0;
	register int_fast16_t offset1=s_offset1;
	register int_fast16_t offset2=s_offset2;
	register int_fast16_t offset3=s_offset3;
	register int_fast16_t offset4=s_offset4;
	register int_fast16_t offset5=s_offset5;
	register int_fast16_t offset6=s_offset6;
	register int_fast16_t offset7=s_offset7;

	while(1)
	{
		register const int cb = *p + b;
		register const int c_b = *p - b;
		if(p[offset0] > cb)
		  if(p[offset2] > cb)
		    if(p[offset3] > cb)
		      if(p[offset5] > cb)
		        if(p[offset1] > cb)
		          if(p[offset4] > cb)
		            goto is_a_corner;
		          else
		            if(p[offset7] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[offset4] > cb)
		            if(p[offset6] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        if(p[offset1] > cb)
		          if(p[offset4] > cb)
		            goto is_a_corner;
		          else
		            if(p[offset7] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		    else
		      if(p[offset7] > cb)
		        if(p[offset6] > cb)
		          if(p[offset5] > cb)
		            if(p[offset1] > cb)
		              goto is_a_corner;
		            else
		              if(p[offset4] > cb)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[offset1] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		      else
		        if(p[offset5] < c_b)
		          if(p[offset3] < c_b)
		            if(p[offset7] < c_b)
		              if(p[offset4] < c_b)
		                if(p[offset6] < c_b)
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
		    if(p[offset5] > cb)
		      if(p[offset7] > cb)
		        if(p[offset6] > cb)
		          if(p[offset1] > cb)
		            goto is_a_corner;
		          else
		            if(p[offset4] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		      else
		        goto is_not_a_corner;
		    else
		      if(p[offset5] < c_b)
		        if(p[offset3] < c_b)
		          if(p[offset2] < c_b)
		            if(p[offset1] < c_b)
		              if(p[offset4] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              if(p[offset4] < c_b)
		                if(p[offset6] < c_b)
		                  goto is_a_corner;
		                else
		                  goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[offset7] < c_b)
		              if(p[offset4] < c_b)
		                if(p[offset6] < c_b)
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
		else if(p[offset0] < c_b)
		  if(p[offset2] < c_b)
		    if(p[offset7] > cb)
		      if(p[offset3] < c_b)
		        if(p[offset5] < c_b)
		          if(p[offset1] < c_b)
		            if(p[offset4] < c_b)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            if(p[offset4] < c_b)
		              if(p[offset6] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[offset1] < c_b)
		            if(p[offset4] < c_b)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        if(p[offset5] > cb)
		          if(p[offset3] > cb)
		            if(p[offset4] > cb)
		              if(p[offset6] > cb)
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
		      if(p[offset7] < c_b)
		        if(p[offset3] < c_b)
		          if(p[offset5] < c_b)
		            if(p[offset1] < c_b)
		              goto is_a_corner;
		            else
		              if(p[offset4] < c_b)
		                if(p[offset6] < c_b)
		                  goto is_a_corner;
		                else
		                  goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[offset1] < c_b)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[offset6] < c_b)
		            if(p[offset5] < c_b)
		              if(p[offset1] < c_b)
		                goto is_a_corner;
		              else
		                if(p[offset4] < c_b)
		                  goto is_a_corner;
		                else
		                  goto is_not_a_corner;
		            else
		              if(p[offset1] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        if(p[offset3] < c_b)
		          if(p[offset5] < c_b)
		            if(p[offset1] < c_b)
		              if(p[offset4] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              if(p[offset4] < c_b)
		                if(p[offset6] < c_b)
		                  goto is_a_corner;
		                else
		                  goto is_not_a_corner;
		              else
		                goto is_not_a_corner;
		          else
		            if(p[offset1] < c_b)
		              if(p[offset4] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          goto is_not_a_corner;
		  else
		    if(p[offset5] > cb)
		      if(p[offset3] > cb)
		        if(p[offset2] > cb)
		          if(p[offset1] > cb)
		            if(p[offset4] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            if(p[offset4] > cb)
		              if(p[offset6] > cb)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[offset7] > cb)
		            if(p[offset4] > cb)
		              if(p[offset6] > cb)
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
		      if(p[offset5] < c_b)
		        if(p[offset7] < c_b)
		          if(p[offset6] < c_b)
		            if(p[offset1] < c_b)
		              goto is_a_corner;
		            else
		              if(p[offset4] < c_b)
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
		  if(p[offset3] > cb)
		    if(p[offset5] > cb)
		      if(p[offset2] > cb)
		        if(p[offset1] > cb)
		          if(p[offset4] > cb)
		            goto is_a_corner;
		          else
		            goto is_not_a_corner;
		        else
		          if(p[offset4] > cb)
		            if(p[offset6] > cb)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            goto is_not_a_corner;
		      else
		        if(p[offset7] > cb)
		          if(p[offset4] > cb)
		            if(p[offset6] > cb)
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
		    if(p[offset3] < c_b)
		      if(p[offset5] < c_b)
		        if(p[offset2] < c_b)
		          if(p[offset1] < c_b)
		            if(p[offset4] < c_b)
		              goto is_a_corner;
		            else
		              goto is_not_a_corner;
		          else
		            if(p[offset4] < c_b)
		              if(p[offset6] < c_b)
		                goto is_a_corner;
		              else
		                goto is_not_a_corner;
		            else
		              goto is_not_a_corner;
		        else
		          if(p[offset7] < c_b)
		            if(p[offset4] < c_b)
		              if(p[offset6] < c_b)
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

		is_a_corner:
			bmin=b;
			goto end;

		is_not_a_corner:
			bmax=b;
			goto end;

		end:

		if(bmin == bmax - 1 || bmin == bmax)
			return bmin;
		b = (bmin + bmax) / 2;
	}
}
