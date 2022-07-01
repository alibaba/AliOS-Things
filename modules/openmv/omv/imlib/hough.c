/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Hough Transform feature extraction.
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_FIND_LINES
void imlib_find_lines(list_t *out, image_t *ptr, rectangle_t *roi, unsigned int x_stride, unsigned int y_stride,
                      uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin)
{
    int r_diag_len, r_diag_len_div, theta_size, r_size, hough_divide = 1; // divides theta and rho accumulators

    for (;;) { // shrink to fit...
        r_diag_len = fast_roundf(fast_sqrtf((roi->w * roi->w) + (roi->h * roi->h)));
        r_diag_len_div = (r_diag_len + hough_divide - 1) / hough_divide;
        theta_size = 1 + ((180 + hough_divide - 1) / hough_divide) + 1; // left & right padding
        r_size = (r_diag_len_div * 2) + 1; // -r_diag_len to +r_diag_len
        if ((sizeof(uint32_t) * theta_size * r_size) <= fb_avail()) break;
        hough_divide = hough_divide << 1; // powers of 2...
        if (hough_divide > 4) fb_alloc_fail(); // support 1, 2, 4
    }

    uint32_t *acc = fb_alloc0(sizeof(uint32_t) * theta_size * r_size, FB_ALLOC_NO_HINT);

    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    int mag = (abs(x_acc) + abs(y_acc)) / 2;
                    if (mag < 126)
                    	continue;

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
                    if (theta < 0) theta += 180;
                    int rho = (fast_roundf(((x - roi->x) * cos_table[theta]) +
                                ((y - roi->y) * sin_table[theta])) / hough_divide) + r_diag_len_div;
                    int acc_index = (rho * theta_size) + ((theta / hough_divide) + 1); // add offset
                    acc[acc_index] += mag;
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ptr->w;

                    int mag = (abs(x_acc) + abs(y_acc)) / 2;
                    if (mag < 126)
                    	continue;

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
                    if (theta < 0) theta += 180;
                    int rho = (fast_roundf(((x - roi->x) * cos_table[theta]) +
                                ((y - roi->y) * sin_table[theta])) / hough_divide) + r_diag_len_div;
                    int acc_index = (rho * theta_size) + ((theta / hough_divide) + 1); // add offset
                    acc[acc_index] += mag;
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ptr->w;

                    int mag = (abs(x_acc) + abs(y_acc)) / 2;
                    if (mag < 126)
                    	continue;

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 180; // * (180 / PI)
                    if (theta < 0) theta += 180;
                    int rho = (fast_roundf(((x - roi->x) * cos_table[theta]) +
                                ((y - roi->y) * sin_table[theta])) / hough_divide) + r_diag_len_div;
                    int acc_index = (rho * theta_size) + ((theta / hough_divide) + 1); // add offset
                    acc[acc_index] += mag;
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    list_init(out, sizeof(find_lines_list_lnk_data_t));

    for (int y = 1, yy = r_size - 1; y < yy; y++) {
        uint32_t *row_ptr = acc + (theta_size * y);

        for (int x = 1, xx = theta_size - 1; x < xx; x++) {
            if ((row_ptr[x] >= threshold)
            &&  (row_ptr[x] >= row_ptr[x-theta_size-1])
            &&  (row_ptr[x] >= row_ptr[x-theta_size])
            &&  (row_ptr[x] >= row_ptr[x-theta_size+1])
            &&  (row_ptr[x] >= row_ptr[x-1])
            &&  (row_ptr[x] >= row_ptr[x+1])
            &&  (row_ptr[x] >= row_ptr[x+theta_size-1])
            &&  (row_ptr[x] >= row_ptr[x+theta_size])
            &&  (row_ptr[x] >= row_ptr[x+theta_size+1])) {

                find_lines_list_lnk_data_t lnk_line;
                memset(&lnk_line, 0, sizeof(find_lines_list_lnk_data_t));

                lnk_line.magnitude = row_ptr[x];
                lnk_line.theta = (x - 1) * hough_divide; // remove offset
                lnk_line.rho = (y - r_diag_len_div) * hough_divide;

                list_push_back(out, &lnk_line);
            }
        }
    }

    fb_free(); // acc

    for (;;) { // Merge overlapping.
        bool merge_occured = false;

        list_t out_temp;
        list_init(&out_temp, sizeof(find_lines_list_lnk_data_t));

        while (list_size(out)) {
            find_lines_list_lnk_data_t lnk_line;
            list_pop_front(out, &lnk_line);

            for (size_t k = 0, l = list_size(out); k < l; k++) {
                find_lines_list_lnk_data_t tmp_line;
                list_pop_front(out, &tmp_line);

                int theta_0_temp = lnk_line.theta;
                int theta_1_temp = tmp_line.theta;
                int rho_0_temp = lnk_line.rho;
                int rho_1_temp = tmp_line.rho;

                if (rho_0_temp < 0) {
                    rho_0_temp = -rho_0_temp;
                    theta_0_temp += 180;
                }

                if (rho_1_temp < 0) {
                    rho_1_temp = -rho_1_temp;
                    theta_1_temp += 180;
                }

                int theta_diff = abs(theta_0_temp - theta_1_temp);
                int theta_diff_2 = (theta_diff >= 180) ? (360 - theta_diff) : theta_diff;

                bool theta_merge = theta_diff_2 < theta_margin;
                bool rho_merge = abs(rho_0_temp - rho_1_temp) < rho_margin;

                if (theta_merge && rho_merge) {
                    uint32_t magnitude = lnk_line.magnitude + tmp_line.magnitude;
                    float sin_mean = ((sin_table[theta_0_temp] * lnk_line.magnitude)
                            + (sin_table[theta_1_temp] * tmp_line.magnitude)) / magnitude;
                    float cos_mean = ((cos_table[theta_0_temp] * lnk_line.magnitude)
                            + (cos_table[theta_1_temp] * tmp_line.magnitude)) / magnitude;

                    lnk_line.theta = fast_roundf(fast_atan2f(sin_mean, cos_mean) * 57.295780) % 360; // * (180 / PI)
                    if (lnk_line.theta < 0) lnk_line.theta += 360;
                    lnk_line.rho = fast_roundf(((rho_0_temp * lnk_line.magnitude) + (rho_1_temp * tmp_line.magnitude)) / magnitude);
                    lnk_line.magnitude = magnitude / 2;

                    if (lnk_line.theta >= 180) {
                        lnk_line.rho = -lnk_line.rho;
                        lnk_line.theta -= 180;
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

    for (size_t i = 0, j = list_size(out); i < j; i++) {
        find_lines_list_lnk_data_t lnk_line;
        list_pop_front(out, &lnk_line);

        if ((45 <= lnk_line.theta) && (lnk_line.theta < 135)) {
            // y = (r - x cos(t)) / sin(t)
            lnk_line.line.x1 = 0;
            lnk_line.line.y1 = fast_roundf((lnk_line.rho - (lnk_line.line.x1 * cos_table[lnk_line.theta])) / sin_table[lnk_line.theta]);
            lnk_line.line.x2 = roi->w - 1;
            lnk_line.line.y2 = fast_roundf((lnk_line.rho - (lnk_line.line.x2 * cos_table[lnk_line.theta])) / sin_table[lnk_line.theta]);
        } else {
            // x = (r - y sin(t)) / cos(t);
            lnk_line.line.y1 = 0;
            lnk_line.line.x1 = fast_roundf((lnk_line.rho - (lnk_line.line.y1 * sin_table[lnk_line.theta])) / cos_table[lnk_line.theta]);
            lnk_line.line.y2 = roi->h - 1;
            lnk_line.line.x2 = fast_roundf((lnk_line.rho - (lnk_line.line.y2 * sin_table[lnk_line.theta])) / cos_table[lnk_line.theta]);
        }

        if(lb_clip_line(&lnk_line.line, 0, 0, roi->w, roi->h)) {
            lnk_line.line.x1 += roi->x;
            lnk_line.line.y1 += roi->y;
            lnk_line.line.x2 += roi->x;
            lnk_line.line.y2 += roi->y;

            // Move rho too.
            lnk_line.rho += fast_roundf((roi->x * cos_table[lnk_line.theta]) + (roi->y * sin_table[lnk_line.theta]));
            list_push_back(out, &lnk_line);
        }
    }
}
#endif //IMLIB_ENABLE_FIND_LINES

#ifdef IMLIB_ENABLE_FIND_LINE_SEGMENTS
// Note this function is not used anymore, see lsd.c
void imlib_find_line_segments(list_t *out, image_t *ptr, rectangle_t *roi, unsigned int x_stride, unsigned int y_stride,
                              uint32_t threshold, unsigned int theta_margin, unsigned int rho_margin,
                              uint32_t segment_threshold)
{
    const unsigned int max_theta_diff = 15;
    const unsigned int max_gap_pixels = 5;

    list_t temp_out;
    imlib_find_lines(&temp_out, ptr, roi, x_stride, y_stride, threshold, theta_margin, rho_margin);
    list_init(out, sizeof(find_lines_list_lnk_data_t));

    const int r_diag_len = fast_roundf(fast_sqrtf((roi->w * roi->w) + (roi->h * roi->h))) * 2;
    int *theta_buffer = fb_alloc(sizeof(int) * r_diag_len, FB_ALLOC_NO_HINT);
    uint32_t *mag_buffer = fb_alloc(sizeof(uint32_t) * r_diag_len, FB_ALLOC_NO_HINT);
    point_t *point_buffer = fb_alloc(sizeof(point_t) * r_diag_len, FB_ALLOC_NO_HINT);

    for (size_t i = 0; list_size(&temp_out); i++) {
        find_lines_list_lnk_data_t lnk_data;
        list_pop_front(&temp_out, &lnk_data);

        list_t line_out;
        list_init(&line_out, sizeof(find_lines_list_lnk_data_t));

        for (int k = -2; k <= 2; k += 2) {
            line_t l;

            if (abs(lnk_data.line.x2 - lnk_data.line.x1) >= abs(lnk_data.line.y2 - lnk_data.line.y1)) {
                // the line is more horizontal than vertical
                l.x1 = lnk_data.line.x1;
                l.y1 = lnk_data.line.y1 + k;
                l.x2 = lnk_data.line.x2;
                l.y2 = lnk_data.line.y2 + k;
            } else {
                // the line is more vertical than horizontal
                l.x1 = lnk_data.line.x1 + k;
                l.y1 = lnk_data.line.y1;
                l.x2 = lnk_data.line.x2 + k;
                l.y2 = lnk_data.line.y2;
            }

            if(!lb_clip_line(&l, 0, 0, ptr->w, ptr->h)) {
                continue;
            }

            find_lines_list_lnk_data_t tmp_line;
            tmp_line.magnitude = lnk_data.magnitude;
            tmp_line.theta = lnk_data.theta;
            tmp_line.rho = lnk_data.rho;

            size_t index = trace_line(ptr, &l, theta_buffer, mag_buffer, point_buffer);
            unsigned int max_gap = 0;

            for (size_t j = 0; j < index; j++) {
                int theta_diff = abs(tmp_line.theta - theta_buffer[j]);
                int theta_diff_2 = (theta_diff >= 90) ? (180 - theta_diff) : theta_diff;
                bool ok = (mag_buffer[j] >= segment_threshold) && (theta_diff_2 <= max_theta_diff);

                if (!max_gap) {
                    if (ok) {
                        max_gap = max_gap_pixels + 1; // (start) auto connect segments max_gap_pixels px apart...
                        tmp_line.line.x1 = point_buffer[j].x;
                        tmp_line.line.y1 = point_buffer[j].y;
                        tmp_line.line.x2 = point_buffer[j].x;
                        tmp_line.line.y2 = point_buffer[j].y;
                    }
                } else {
                    if (ok) {
                        max_gap = max_gap_pixels + 1; // (reset) auto connect segments max_gap_pixels px apart...
                        tmp_line.line.x2 = point_buffer[j].x;
                        tmp_line.line.y2 = point_buffer[j].y;
                    } else if (!--max_gap) {
                        list_push_back(&line_out, &tmp_line);
                    }
                }
            }

            if (max_gap) {
                list_push_back(&line_out, &tmp_line);
            }
        }

        merge_alot(&line_out, max_gap_pixels + 1, max_theta_diff);

        while (list_size(&line_out)) {
            find_lines_list_lnk_data_t lnk_line;
            list_pop_front(&line_out, &lnk_line);
            list_push_back(out, &lnk_line);
        }
    }

    merge_alot(out, max_gap_pixels + 1, max_theta_diff);

    fb_free(); // point_buffer
    fb_free(); // mag_buffer
    fb_free(); // theta_buffer
}
#endif //IMLIB_ENABLE_FIND_LINE_SEGMENTS

#ifdef IMLIB_ENABLE_FIND_CIRCLES
void imlib_find_circles(list_t *out, image_t *ptr, rectangle_t *roi, unsigned int x_stride, unsigned int y_stride,
                        uint32_t threshold, unsigned int x_margin, unsigned int y_margin, unsigned int r_margin,
                        unsigned int r_min, unsigned int r_max, unsigned int r_step)
{
    uint16_t *theta_acc = fb_alloc0(sizeof(uint16_t) * roi->w * roi->h, FB_ALLOC_NO_HINT);
    uint16_t *magnitude_acc = fb_alloc0(sizeof(uint16_t) * roi->w * roi->h, FB_ALLOC_NO_HINT);

    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ((ptr->w + UINT32_T_MASK) >> UINT32_T_SHIFT);

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 360; // * (180 / PI)
                    if (theta < 0) theta += 360;
                    int magnitude = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
                    int index = (roi->w * (y - roi->y)) + (x - roi->x);

                    theta_acc[index] = theta;
                    magnitude_acc[index] = magnitude;
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ptr->w;

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x - 1);
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x + 1);
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ptr->w;

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 360; // * (180 / PI)
                    if (theta < 0) theta += 360;
                    int magnitude = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
                    int index = (roi->w * (y - roi->y)) + (x - roi->x);

                    theta_acc[index] = theta;
                    magnitude_acc[index] = magnitude;
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y + 1, yy = roi->y + roi->h - 1; y < yy; y += y_stride) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x + (y % x_stride) + 1, xx = roi->x + roi->w - 1; x < xx; x += x_stride) {
                    int pixel; // Sobel Algorithm Below
                    int x_acc = 0;
                    int y_acc = 0;

                    row_ptr -= ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[0,0] -> pixel * +1
                    y_acc += pixel * +1; // y[0,0] -> pixel * +1

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                         // x[0,1] -> pixel * 0
                    y_acc += pixel * +2; // y[0,1] -> pixel * +2

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[0,2] -> pixel * -1
                    y_acc += pixel * +1; // y[0,2] -> pixel * +1

                    row_ptr += ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +2; // x[1,0] -> pixel * +2
                                         // y[1,0] -> pixel * 0

                    // pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                    // x[1,1] -> pixel * 0
                    // y[1,1] -> pixel * 0

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -2; // x[1,2] -> pixel * -2
                                         // y[1,2] -> pixel * 0

                    row_ptr += ptr->w;

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x - 1));
                    x_acc += pixel * +1; // x[2,0] -> pixel * +1
                    y_acc += pixel * -1; // y[2,0] -> pixel * -1

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                                         // x[2,1] -> pixel * 0
                    y_acc += pixel * -2; // y[2,1] -> pixel * -2

                    pixel = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x + 1));
                    x_acc += pixel * -1; // x[2,2] -> pixel * -1
                    y_acc += pixel * -1; // y[2,2] -> pixel * -1

                    row_ptr -= ptr->w;

                    int theta = fast_roundf((x_acc ? fast_atan2f(y_acc, x_acc) : 1.570796f) * 57.295780) % 360; // * (180 / PI)
                    if (theta < 0) theta += 360;
                    int magnitude = fast_roundf(fast_sqrtf((x_acc * x_acc) + (y_acc * y_acc)));
                    int index = (roi->w * (y - roi->y)) + (x - roi->x);

                    theta_acc[index] = theta;
                    magnitude_acc[index] = magnitude;
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    // Theta Direction (% 180)
    //
    // 0,0         X_MAX
    //
    //     090
    // 000     000
    //     090
    //
    // Y_MAX

    // Theta Direction (% 360)
    //
    // 0,0         X_MAX
    //
    //     090
    // 000     180
    //     270
    //
    // Y_MAX

    list_init(out, sizeof(find_circles_list_lnk_data_t));

    for (int r = r_min, rr = r_max; r < rr; r += r_step) { // ignore r = 0/1
        int a_size, b_size, hough_divide = 1; // divides a and b accumulators
        int hough_shift = 0;
        int w_size = roi->w - (2 * r);
        int h_size = roi->h - (2 * r);

        for (;;) { // shrink to fit...
            a_size = 1 + ((w_size + hough_divide - 1) / hough_divide) + 1; // left & right padding
            b_size = 1 + ((h_size + hough_divide - 1) / hough_divide) + 1; // top & bottom padding
            if ((sizeof(uint32_t) * a_size * b_size) <= fb_avail()) break;
            hough_divide = hough_divide << 1; // powers of 2...
            hough_shift++;
            if (hough_divide > 4) fb_alloc_fail(); // support 1, 2, 4
        }

        uint32_t *acc = fb_alloc0(sizeof(uint32_t) * a_size * b_size, FB_ALLOC_NO_HINT);
        int16_t *rcos = fb_alloc(sizeof(int16_t)*360, FB_ALLOC_NO_HINT);
        int16_t *rsin = fb_alloc(sizeof(int16_t)*360, FB_ALLOC_NO_HINT);
        for (int i=0; i<360; i++)
        {
            rcos[i] = (int16_t)roundf(r * cos_table[i]);
            rsin[i] = (int16_t)roundf(r * sin_table[i]);
        }

        for (int y = 0, yy = roi->h; y < yy; y++) {
            for (int x = 0, xx = roi->w; x < xx; x++) {
                int index = (roi->w * y) + x;
                int theta = theta_acc[index];
                int magnitude = magnitude_acc[index];
                if (!magnitude) continue;

                // We have to do the below step twice because the gradient may be pointing inside or outside the circle.
                // Only graidents pointing inside of the circle sum up to produce a large magnitude.
                for (;;) { // Hi to lo edge direction
                    int a = x + rcos[theta] - r;
                    if ((a < 0) || (w_size <= a)) break; // circle doesn't fit in the window
                    int b = y + rsin[theta] - r;
                    if ((b < 0) || (h_size <= b)) break; // circle doesn't fit in the window
                    int acc_index = (((b >> hough_shift) + 1) * a_size) + ((a >> hough_shift) + 1); // add offset

                    int acc_value = acc[acc_index] += magnitude;
                    acc[acc_index] = acc_value;
                    break;
                }

                for (;;) { // Lo to hi edge direction
                    int a = x - rcos[theta] - r;
                    if ((a < 0) || (w_size <= a)) break; // circle doesn't fit in the window
                    int b = y - rsin[theta] - r;
                    if ((b < 0) || (h_size <= b)) break; // circle doesn't fit in the window
                    int acc_index = (((b >> hough_shift) + 1) * a_size) + ((a >> hough_shift) + 1); // add offset

                    int acc_value = acc[acc_index] += magnitude;
                    acc[acc_index] = acc_value;
                    break;
                }
            }
        }

        for (int y = 1, yy = b_size - 1; y < yy; y++) {
            uint32_t *row_ptr = acc + (a_size * y);
            uint32_t val;
            for (int x = 1, xx = a_size - 1; x < xx; x++) {
                val = row_ptr[x];
                if ((val >= threshold)
                &&  (val >= row_ptr[x-a_size-1])
                &&  (val >= row_ptr[x-a_size])
                &&  (val >= row_ptr[x-a_size+1])
                &&  (val >= row_ptr[x-1])
                &&  (val >= row_ptr[x+1])
                &&  (val >= row_ptr[x+a_size-1])
                &&  (val >= row_ptr[x+a_size])
                &&  (val >= row_ptr[x+a_size+1])) {

                    find_circles_list_lnk_data_t lnk_data;
                    lnk_data.magnitude = val;
                    lnk_data.p.x = ((x - 1) << hough_shift) + r + roi->x; // remove offset
                    lnk_data.p.y = ((y - 1) << hough_shift) + r + roi->y; // remove offset
                    lnk_data.r = r;

                    list_push_back(out, &lnk_data);
                    if (val > row_ptr[x+1])
                       x++; // can skip the next pixel
                }
            }
        }

        fb_free(); // rsin
        fb_free(); // rcos
        fb_free(); // acc
    }

    fb_free(); // magnitude_acc
    fb_free(); // theta_acc

    for (;;) { // Merge overlapping.
        bool merge_occured = false;

        list_t out_temp;
        list_init(&out_temp, sizeof(find_circles_list_lnk_data_t));

        while (list_size(out)) {
            find_circles_list_lnk_data_t lnk_data;
            list_pop_front(out, &lnk_data);

            for (size_t k = 0, l = list_size(out); k < l; k++) {
                find_circles_list_lnk_data_t tmp_data;
                list_pop_front(out, &tmp_data);

                bool x_diff_ok = abs(lnk_data.p.x - tmp_data.p.x) < x_margin;
                bool y_diff_ok = abs(lnk_data.p.y - tmp_data.p.y) < y_margin;
                bool r_diff_ok = abs(lnk_data.r - tmp_data.r) < r_margin;

                if (x_diff_ok && y_diff_ok && r_diff_ok) {
                    uint32_t magnitude = lnk_data.magnitude + tmp_data.magnitude;
                    lnk_data.p.x = ((lnk_data.p.x * lnk_data.magnitude) + (tmp_data.p.x * tmp_data.magnitude)) / magnitude;
                    lnk_data.p.y = ((lnk_data.p.y * lnk_data.magnitude) + (tmp_data.p.y * tmp_data.magnitude)) / magnitude;
                    lnk_data.r = ((lnk_data.r * lnk_data.magnitude) + (tmp_data.r * tmp_data.magnitude)) / magnitude;
                    lnk_data.magnitude = magnitude / 2;
                    merge_occured = true;
                } else {
                    list_push_back(out, &tmp_data);
                }
            }

            list_push_back(&out_temp, &lnk_data);
        }

        list_copy(out, &out_temp);

        if (!merge_occured) {
            break;
        }
    }
}
#endif //IMLIB_ENABLE_FIND_CIRCLES
