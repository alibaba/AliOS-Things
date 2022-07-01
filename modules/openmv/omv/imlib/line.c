/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Line functions.
 */
#include "imlib.h"

static void pixel_magnitude(image_t *ptr, int x, int y, int *theta, uint32_t *mag)
{
    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
            int pixel; // Sobel Algorithm Below
            int x_acc = 0;
            int y_acc = 0;

            if (y != 0) row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +1; // x[0,0] -> pixel * +1
            y_acc += pixel * +1; // y[0,0] -> pixel * +1

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                 // x[0,1] -> pixel * 0
            y_acc += pixel * +2; // y[0,1] -> pixel * +2

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -1; // x[0,2] -> pixel * -1
            y_acc += pixel * +1; // y[0,2] -> pixel * +1

            if (y != 0) row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                 // y[1,0] -> pixel * 0

            // pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
            // x[1,1] -> pixel * 0
            // y[1,1] -> pixel * 0

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                 // y[1,2] -> pixel * 0

            if (y != (ptr->h - 1)) row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +1; // x[2,0] -> pixel * +1
            y_acc += pixel * -1; // y[2,0] -> pixel * -1

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                 // x[2,1] -> pixel * 0
            y_acc += pixel * -2; // y[2,1] -> pixel * -2

            pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -1; // x[2,2] -> pixel * -1
            y_acc += pixel * -1; // y[2,2] -> pixel * -1

            if (y != (ptr->h - 1)) row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

            *theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
            if (*theta < 0) *theta += 180;
            *mag = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
            int pixel; // Sobel Algorithm Below... w/ Scharr...
            int x_acc = 0;
            int y_acc = 0;

            if (y != 0) row_ptr -= ptr->w;

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0));
            x_acc += pixel * +1; // x[0,0] -> pixel * +1
            y_acc += pixel * +1; // y[0,0] -> pixel * +1

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                 // x[0,1] -> pixel * 0
            y_acc += pixel * +2; // y[0,1] -> pixel * +2

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1));
            x_acc += pixel * -1; // x[0,2] -> pixel * -1
            y_acc += pixel * +1; // y[0,2] -> pixel * +1

            if (y != 0) row_ptr += ptr->w;

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0));
            x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                 // y[1,0] -> pixel * 0

            // pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
            // x[1,1] -> pixel * 0
            // y[1,1] -> pixel * 0

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1));
            x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                 // y[1,2] -> pixel * 0

            if (y != (ptr->h - 1)) row_ptr += ptr->w;

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0));
            x_acc += pixel * +1; // x[2,0] -> pixel * +1
            y_acc += pixel * -1; // y[2,0] -> pixel * -1

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                 // x[2,1] -> pixel * 0
            y_acc += pixel * -2; // y[2,1] -> pixel * -2

            pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1));
            x_acc += pixel * -1; // x[2,2] -> pixel * -1
            y_acc += pixel * -1; // y[2,2] -> pixel * -1

            if (y != (ptr->h - 1)) row_ptr -= ptr->w;

            *theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
            if (*theta < 0) *theta += 180;
            *mag = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
            break;
        }
        case IMAGE_BPP_RGB565: {
            uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
            int pixel; // Sobel Algorithm Below... w/ Scharr...
            int x_acc = 0;
            int y_acc = 0;

            if (y != 0) row_ptr -= ptr->w;

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +1; // x[0,0] -> pixel * +1
            y_acc += pixel * +1; // y[0,0] -> pixel * +1

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                 // x[0,1] -> pixel * 0
            y_acc += pixel * +2; // y[0,1] -> pixel * +2

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -1; // x[0,2] -> pixel * -1
            y_acc += pixel * +1; // y[0,2] -> pixel * +1

            if (y != 0) row_ptr += ptr->w;

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                 // y[1,0] -> pixel * 0

            // pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
            // x[1,1] -> pixel * 0
            // y[1,1] -> pixel * 0

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                 // y[1,2] -> pixel * 0

            if (y != (ptr->h - 1)) row_ptr += ptr->w;

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MAX(x - 1, 0)));
            x_acc += pixel * +1; // x[2,0] -> pixel * +1
            y_acc += pixel * -1; // y[2,0] -> pixel * -1

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                 // x[2,1] -> pixel * 0
            y_acc += pixel * -2; // y[2,1] -> pixel * -2

            pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, IM_MIN(x + 1, ptr->w - 1)));
            x_acc += pixel * -1; // x[2,2] -> pixel * -1
            y_acc += pixel * -1; // y[2,2] -> pixel * -1

            if (y != (ptr->h - 1)) row_ptr -= ptr->w;

            *theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
            if (*theta < 0) *theta += 180;
            *mag = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
            break;
        }
        default: {
            break;
        }
    }
}

// http://www.brackeen.com/vga/source/djgpp20/lines.c.html
// http://www.brackeen.com/vga/source/bc31/lines.c.html
static bool merge_line(line_t *big, line_t *small, unsigned int threshold)
{
    int dx = big->x2 - big->x1; // the horizontal distance of the line
    int dy = big->y2 - big->y1; // the vertical distance of the line
    int dxabs = abs(dx);
    int dyabs = abs(dy);
    int sdx = (dx < 0) ? -1 :((dx > 0) ? 1 : 0);
    int sdy = (dy < 0) ? -1 :((dy > 0) ? 1 : 0);
    int x = dyabs >> 1; // correct
    int y = dxabs >> 1; // correct
    int px = big->x1;
    int py = big->y1;

    int x_diff_0 = (px - small->x1);
    int y_diff_0 = (py - small->y1);
    if (fast_roundf(fast_sqrtf((x_diff_0 * x_diff_0) + (y_diff_0 * y_diff_0))) <= threshold) return true;
    int x_diff_1 = (px - small->x2);
    int y_diff_1 = (py - small->y2);
    if (fast_roundf(fast_sqrtf((x_diff_1 * x_diff_1) + (y_diff_1 * y_diff_1))) <= threshold) return true;

    if (dxabs >= dyabs) { // the line is more horizontal than vertical
        for(int i = 0; i < dxabs; i++) {
            y += dyabs;

            if (y >= dxabs) {
                y -= dxabs;
                py += sdy;
            }

            px += sdx;

            x_diff_0 = (px - small->x1);
            y_diff_0 = (py - small->y1);
            if (fast_roundf(fast_sqrtf((x_diff_0 * x_diff_0) + (y_diff_0 * y_diff_0))) <= threshold) return true;
            x_diff_1 = (px - small->x2);
            y_diff_1 = (py - small->y2);
            if (fast_roundf(fast_sqrtf((x_diff_1 * x_diff_1) + (y_diff_1 * y_diff_1))) <= threshold) return true;
        }
    } else { // the line is more vertical than horizontal
        for(int i = 0; i < dyabs; i++) {
            x += dxabs;

            if (x >= dyabs) {
                x -= dyabs;
                px += sdx;
            }

            py += sdy;

            x_diff_0 = (px - small->x1);
            y_diff_0 = (py - small->y1);
            if (fast_roundf(fast_sqrtf((x_diff_0 * x_diff_0) + (y_diff_0 * y_diff_0))) <= threshold) return true;
            x_diff_1 = (px - small->x2);
            y_diff_1 = (py - small->y2);
            if (fast_roundf(fast_sqrtf((x_diff_1 * x_diff_1) + (y_diff_1 * y_diff_1))) <= threshold) return true;
        }
    }

    return false;
}

void merge_alot(list_t *out, int threshold, int theta_threshold)
{
    for (;;) {
        bool merge_occured = false;

        list_t out_temp;
        list_init(&out_temp, sizeof(find_lines_list_lnk_data_t));

        while (list_size(out)) {
            find_lines_list_lnk_data_t lnk_line;
            list_pop_front(out, &lnk_line);

            for (size_t k = 0, l = list_size(out); k < l; k++) {
                find_lines_list_lnk_data_t tmp_line;
                list_pop_front(out, &tmp_line);

                int x_diff_0 = (lnk_line.line.x2 - lnk_line.line.x1);
                int y_diff_0 = (lnk_line.line.y2 - lnk_line.line.y1);
                int length_0 = fast_roundf(fast_sqrtf((x_diff_0 * x_diff_0) + (y_diff_0 * y_diff_0)));

                int x_diff_1 = (tmp_line.line.x2 - tmp_line.line.x1);
                int y_diff_1 = (tmp_line.line.y2 - tmp_line.line.y1);
                int length_1 = fast_roundf(fast_sqrtf((x_diff_1 * x_diff_1) + (y_diff_1 * y_diff_1)));

                int theta_diff = abs(lnk_line.theta - tmp_line.theta);
                int theta_diff_2 = (theta_diff >= 90) ? (180 - theta_diff) : theta_diff;

                if ((theta_diff_2 <= theta_threshold) && merge_line((length_0 > length_1) ?
                            &lnk_line.line : &tmp_line.line, (length_0 <= length_1) ? &lnk_line.line : &tmp_line.line, threshold)) {

                    if (abs(x_diff_0) >= abs(y_diff_0)) { // the line is more horizontal than vertical
                        if (x_diff_0 < 0) { // Make sure x slope is positive for the next part.
                            int temp_x = lnk_line.line.x1;
                            lnk_line.line.x1 = lnk_line.line.x2;
                            lnk_line.line.x2 = temp_x;
                            int temp_y = lnk_line.line.y1;
                            lnk_line.line.y1 = lnk_line.line.y2;
                            lnk_line.line.y2 = temp_y;
                            x_diff_0 = (lnk_line.line.x2 - lnk_line.line.x1);
                            y_diff_0 = (lnk_line.line.y2 - lnk_line.line.y1);
                        }

                        if (x_diff_1 < 0) { // Make sure x slope is positive for the next part.
                            int temp_x = tmp_line.line.x1;
                            tmp_line.line.x1 = tmp_line.line.x2;
                            tmp_line.line.x2 = temp_x;
                            int temp_y = tmp_line.line.y1;
                            tmp_line.line.y1 = tmp_line.line.y2;
                            tmp_line.line.y2 = temp_y;
                            x_diff_1 = (tmp_line.line.x2 - tmp_line.line.x1);
                            y_diff_1 = (tmp_line.line.y2 - tmp_line.line.y1);
                        }

                        if (length_0 > length_1) {
                            int x_min = IM_MIN(lnk_line.line.x1, tmp_line.line.x1);
                            int x_max = IM_MAX(lnk_line.line.x2, tmp_line.line.x2);
                            lnk_line.line.y1 = lnk_line.line.y1 - ((y_diff_0 * (lnk_line.line.x1 - x_min)) / x_diff_0);
                            lnk_line.line.x1 = x_min;
                            lnk_line.line.y2 = lnk_line.line.y2 + ((y_diff_0 * (x_max - lnk_line.line.x2)) / x_diff_0);
                            lnk_line.line.x2 = x_max;
                        } else {
                            int x_min = IM_MIN(tmp_line.line.x1, lnk_line.line.x1);
                            int x_max = IM_MAX(tmp_line.line.x2, lnk_line.line.x2);
                            lnk_line.line.y1 = tmp_line.line.y1 - ((y_diff_0 * (tmp_line.line.x1 - x_min)) / x_diff_0);
                            lnk_line.line.x1 = x_min;
                            lnk_line.line.y2 = tmp_line.line.y2 + ((y_diff_0 * (x_max - tmp_line.line.x2)) / x_diff_0);
                            lnk_line.line.x2 = x_max;
                        }
                    } else { // the line is more vertical than horizontal
                        if (y_diff_0 < 0) { // Make sure y slope is positive for the next part.
                            int temp_x = lnk_line.line.x1;
                            lnk_line.line.x1 = lnk_line.line.x2;
                            lnk_line.line.x2 = temp_x;
                            int temp_y = lnk_line.line.y1;
                            lnk_line.line.y1 = lnk_line.line.y2;
                            lnk_line.line.y2 = temp_y;
                            x_diff_0 = (lnk_line.line.x2 - lnk_line.line.x1);
                            y_diff_0 = (lnk_line.line.y2 - lnk_line.line.y1);
                        }

                        if (y_diff_1 < 0) { // Make sure y slope is positive for the next part.
                            int temp_x = tmp_line.line.x1;
                            tmp_line.line.x1 = tmp_line.line.x2;
                            tmp_line.line.x2 = temp_x;
                            int temp_y = tmp_line.line.y1;
                            tmp_line.line.y1 = tmp_line.line.y2;
                            tmp_line.line.y2 = temp_y;
                            x_diff_1 = (tmp_line.line.x2 - tmp_line.line.x1);
                            y_diff_1 = (tmp_line.line.y2 - tmp_line.line.y1);
                        }

                        if (length_0 > length_1) {
                            int y_min = IM_MIN(lnk_line.line.y1, tmp_line.line.y1);
                            int y_max = IM_MAX(lnk_line.line.y2, tmp_line.line.y2);
                            lnk_line.line.x1 = lnk_line.line.x1 - ((x_diff_0 * (lnk_line.line.y1 - y_min)) / y_diff_0);
                            lnk_line.line.y1 = y_min;
                            lnk_line.line.x2 = lnk_line.line.x2 + ((x_diff_0 * (y_max - lnk_line.line.y2)) / y_diff_0);
                            lnk_line.line.y2 = y_max;
                        } else {
                            int y_min = IM_MIN(tmp_line.line.y1, lnk_line.line.y1);
                            int y_max = IM_MAX(tmp_line.line.y2, lnk_line.line.y2);
                            lnk_line.line.x1 = tmp_line.line.x1 - ((x_diff_0 * (tmp_line.line.y1 - y_min)) / y_diff_0);
                            lnk_line.line.y1 = y_min;
                            lnk_line.line.x2 = tmp_line.line.x2 + ((x_diff_0 * (y_max - tmp_line.line.y2)) / y_diff_0);
                            lnk_line.line.y2 = y_max;
                        }
                    }

                    merge_occured = true;
                } else {
                    list_push_back(out, &tmp_line);
                }
            }

            list_push_back(&out_temp, &lnk_line);
        }

        list_copy(out, &out_temp);

        if (!merge_occured) {
            break;
        }
    }
}

// http://www.brackeen.com/vga/source/djgpp20/lines.c.html
// http://www.brackeen.com/vga/source/bc31/lines.c.html
size_t trace_line(image_t *ptr, line_t *l, int *theta_buffer, uint32_t *mag_buffer, point_t *point_buffer)
{
    int dx = l->x2 - l->x1; // the horizontal distance of the line
    int dy = l->y2 - l->y1; // the vertical distance of the line
    int dxabs = abs(dx);
    int dyabs = abs(dy);
    int sdx = (dx < 0) ? -1 :((dx > 0) ? 1 : 0);
    int sdy = (dy < 0) ? -1 :((dy > 0) ? 1 : 0);
    int x = dyabs >> 1; // correct
    int y = dxabs >> 1; // correct
    int px = l->x1;
    int py = l->y1;

    size_t index = 0;

    pixel_magnitude(ptr, px, py, theta_buffer + index, mag_buffer + index);
    point_buffer[index++] = (point_t) {.x = px, .y = py};

    if (dxabs >= dyabs) { // the line is more horizontal than vertical
        for(int i = 0; i < dxabs; i++) {
            y += dyabs;

            if (y >= dxabs) {
                y -= dxabs;
                py += sdy;
            }

            px += sdx;

            pixel_magnitude(ptr, px, py, theta_buffer + index, mag_buffer + index);
            point_buffer[index++] = (point_t) {.x = px, .y = py};
        }
    } else { // the line is more vertical than horizontal
        for(int i = 0; i < dyabs; i++) {
            x += dxabs;

            if (x >= dyabs) {
                x -= dyabs;
                px += sdx;
            }

            py += sdy;

            pixel_magnitude(ptr, px, py, theta_buffer + index, mag_buffer + index);
            point_buffer[index++] = (point_t) {.x = px, .y = py};
        }
    }

    return index;
}
