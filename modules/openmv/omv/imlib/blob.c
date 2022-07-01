/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Blob detection code.
 */
#include "imlib.h"

typedef struct xylr {
    int16_t x, y, l, r, t_l, b_l;
}
xylr_t;

static float sign(float x)
{
    return x / fabsf(x);
}

static int sum_m_to_n(int m, int n)
{
    return ((n * (n + 1)) - (m * (m - 1))) / 2;
}

static int sum_2_m_to_n(int m, int n)
{
    return ((n * (n + 1) * ((2 * n) + 1)) - (m * (m - 1) * ((2 * m) - 1))) / 6;
}

static int cumulative_moving_average(int avg, int x, int n)
{
    return (x + (n * avg)) / (n + 1);
}

static void bin_up(uint16_t *hist, uint16_t size, unsigned int max_size, uint16_t **new_hist, uint16_t *new_size)
{
    int start = -1;

    for (int i = 0; i < size; i++) {
        if (hist[i]) {
            start = i;
            break;
        }
    }

    if (start != -1) {
        int end = start;

        for (int i = start + 1; i < size; i++) {
            if (!hist[i]) {
                break;
            }
            end = i;
        }

        int bin_count = end - start + 1; // >= 1
        *new_size = IM_MIN(max_size, bin_count);
        *new_hist = xalloc0((*new_size) * sizeof(uint16_t));
        float div_value = (*new_size) / ((float) bin_count); // Reversed so we can multiply below.

        for (int i = 0; i < bin_count; i++) {
            (*new_hist)[fast_floorf(i*div_value)] += hist[start+i];
        }
    }
}

static void merge_bins(int b_dst_start, int b_dst_end, uint16_t **b_dst_hist, uint16_t *b_dst_hist_len,
                       int b_src_start, int b_src_end, uint16_t **b_src_hist, uint16_t *b_src_hist_len,
                       unsigned int max_size)
{
    int start = IM_MIN(b_dst_start, b_src_start);
    int end = IM_MAX(b_dst_end, b_src_end);

    int bin_count = end - start + 1; // >= 1
    uint16_t new_size = IM_MIN(max_size, bin_count);
    uint16_t *new_hist = xalloc0(new_size * sizeof(uint16_t));
    float div_value = new_size / ((float) bin_count); // Reversed so we can multiply below.

    int b_dst_bin_count = b_dst_end - b_dst_start + 1; // >= 1
    uint16_t b_dst_new_size = IM_MIN((*b_dst_hist_len), b_dst_bin_count);
    float b_dst_div_value = b_dst_new_size / ((float) b_dst_bin_count); // Reversed so we can multiply below.

    int b_src_bin_count = b_src_end - b_src_start + 1; // >= 1
    uint16_t b_src_new_size = IM_MIN((*b_src_hist_len), b_src_bin_count);
    float b_src_div_value = b_src_new_size / ((float) b_src_bin_count); // Reversed so we can multiply below.

    for(int i = 0; i < bin_count; i++) {
        if ((b_dst_start <= (i + start)) && ((i + start) <= b_dst_end)) {
            int index = fast_floorf((i + start - b_dst_start) * b_dst_div_value);
            new_hist[fast_floorf(i*div_value)] += (*b_dst_hist)[index];
            (*b_dst_hist)[index] = 0; // prevent from adding again...
        }
        if ((b_src_start <= (i + start)) && ((i + start) <= b_src_end)) {
            int index = fast_floorf((i + start - b_src_start) * b_src_div_value);
            new_hist[fast_floorf(i*div_value)] += (*b_src_hist)[index];
            (*b_src_hist)[index] = 0; // prevent from adding again...
        }
    }

    xfree(*b_dst_hist);
    xfree(*b_src_hist);

    *b_dst_hist_len = new_size;
    (*b_dst_hist) = new_hist;
    *b_src_hist_len = 0;
    (*b_src_hist) = NULL;
}

static float calc_roundness(float blob_a, float blob_b, float blob_c)
{
    float roundness_div = fast_sqrtf((blob_b * blob_b) + ((blob_a - blob_c) * (blob_a - blob_c)));
    float roundness_sin = IM_DIV(blob_b, roundness_div);
    float roundness_cos = IM_DIV(blob_a - blob_c, roundness_div);
    float roundness_add = (blob_a + blob_c) / 2;
    float roundness_cos_mul = (blob_a - blob_c) / 2;
    float roundness_sin_mul = blob_b / 2;

    float roundness_0 = roundness_add + (roundness_cos * roundness_cos_mul) + (roundness_sin * roundness_sin_mul);
    float roundness_1 = roundness_add + (roundness_cos * roundness_cos_mul) - (roundness_sin * roundness_sin_mul);
    float roundness_2 = roundness_add - (roundness_cos * roundness_cos_mul) + (roundness_sin * roundness_sin_mul);
    float roundness_3 = roundness_add - (roundness_cos * roundness_cos_mul) - (roundness_sin * roundness_sin_mul);

    float roundness_max = IM_MAX(roundness_0, IM_MAX(roundness_1, IM_MAX(roundness_2, roundness_3)));
    float roundness_min = IM_MIN(roundness_0, IM_MIN(roundness_1, IM_MIN(roundness_2, roundness_3)));

    return IM_DIV(roundness_min, roundness_max);
}

void imlib_find_blobs(list_t *out, image_t *ptr, rectangle_t *roi, unsigned int x_stride, unsigned int y_stride,
                      list_t *thresholds, bool invert, unsigned int area_threshold, unsigned int pixels_threshold,
                      bool merge, int margin,
                      bool (*threshold_cb)(void*,find_blobs_list_lnk_data_t*), void *threshold_cb_arg,
                      bool (*merge_cb)(void*,find_blobs_list_lnk_data_t*,find_blobs_list_lnk_data_t*), void *merge_cb_arg,
                      unsigned int x_hist_bins_max, unsigned int y_hist_bins_max)
{
    // Same size as the image so we don't have to translate.
    image_t bmp;
    bmp.w = ptr->w;
    bmp.h = ptr->h;
    bmp.bpp = IMAGE_BPP_BINARY;
    bmp.data = fb_alloc0(image_size(&bmp), FB_ALLOC_NO_HINT);

    uint16_t *x_hist_bins = NULL;
    if (x_hist_bins_max) x_hist_bins = fb_alloc(ptr->w * sizeof(uint16_t), FB_ALLOC_NO_HINT);

    uint16_t *y_hist_bins = NULL;
    if (y_hist_bins_max) y_hist_bins = fb_alloc(ptr->h * sizeof(uint16_t), FB_ALLOC_NO_HINT);

    lifo_t lifo;
    size_t lifo_len;
    lifo_alloc_all(&lifo, &lifo_len, sizeof(xylr_t));

    list_init(out, sizeof(find_blobs_list_lnk_data_t));

    size_t code = 0;
    for (list_lnk_t *it = iterator_start_from_head(thresholds); it; it = iterator_next(it)) {
        color_thresholds_list_lnk_data_t lnk_data;
        iterator_get(thresholds, it, &lnk_data);

        switch(ptr->bpp) {
            case IMAGE_BPP_BINARY: {
                for (int y = roi->y, yy = roi->y + roi->h, y_max = yy - 1; y < yy; y += y_stride) {
                    uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = roi->x + (y % x_stride), xx = roi->x + roi->w, x_max = xx - 1; x < xx; x += x_stride) {
                        if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x))
                        && COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x), &lnk_data, invert)) {
                            int old_x = x;
                            int old_y = y;

                            float corners_acc[FIND_BLOBS_CORNERS_RESOLUTION];
                            point_t corners[FIND_BLOBS_CORNERS_RESOLUTION];
                            int corners_n[FIND_BLOBS_CORNERS_RESOLUTION];
                            // These values are initialized to their maximum before we minimize.
                            for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                corners[i].x = IM_MAX(IM_MIN(x_max * sign(cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), x_max), 0);
                                corners[i].y = IM_MAX(IM_MIN(y_max * sign(sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), y_max), 0);
                                corners_acc[i] = (corners[i].x * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                                 (corners[i].y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                corners_n[i] = 1;
                            }

                            int blob_pixels = 0;
                            int blob_perimeter = 0;
                            int blob_cx = 0;
                            int blob_cy = 0;
                            long long blob_a = 0;
                            long long blob_b = 0;
                            long long blob_c = 0;

                            if (x_hist_bins) memset(x_hist_bins, 0, ptr->w * sizeof(uint16_t));
                            if (y_hist_bins) memset(y_hist_bins, 0, ptr->h * sizeof(uint16_t));

                            // Scanline Flood Fill Algorithm //

                            for(;;) {
                                int left = x, right = x;
                                uint32_t *row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                                uint32_t *bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);

                                while ((left > roi->x)
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, left - 1))
                                && COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, left - 1), &lnk_data, invert)) {
                                    left--;
                                }

                                while ((right < (roi->x + roi->w - 1))
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, right + 1))
                                && COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, right + 1), &lnk_data, invert)) {
                                    right++;
                                }

                                for (int i = left; i <= right; i++) {
                                    IMAGE_SET_BINARY_PIXEL_FAST(bmp_row, i);
                                }

                                int sum = sum_m_to_n(left, right);
                                int sum_2 = sum_2_m_to_n(left, right);
                                int cnt = right - left + 1;
                                int avg = sum / cnt;

                                for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                    int x_new = (cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] > 0) ? left :
                                                ((cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] == 0) ? avg :
                                                                                                  right);
                                    float z = (x_new * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                              (y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                    if (z < corners_acc[i]) {
                                        corners_acc[i] = z;
                                        corners[i].x = x_new;
                                        corners[i].y = y;
                                        corners_n[i] = 1;
                                    } else if (z == corners_acc[i]) {
                                        corners[i].x = cumulative_moving_average(corners[i].x, x_new, corners_n[i]);
                                        corners[i].y = cumulative_moving_average(corners[i].y, y, corners_n[i]);
                                        corners_n[i] += 1;
                                    }
                                }

                                blob_pixels += cnt;
                                blob_perimeter += 2;
                                blob_cx += sum;
                                blob_cy += y * cnt;
                                blob_a += sum_2;
                                blob_b += y * sum;
                                blob_c += y * y * cnt;

                                if (y_hist_bins) y_hist_bins[y] += cnt;
                                if (x_hist_bins) for (int i = left; i <= right; i++) x_hist_bins[i] += 1;

                                int top_left = left;
                                int bot_left = left;
                                bool break_out = false;
                                for(;;) {
                                    if (lifo_size(&lifo) < lifo_len) {

                                        if (y > roi->y) {
                                            row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y - 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y - 1);

                                            bool recurse = false;
                                            for (int i = top_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = i + 1; // Don't test the same pixel again...
                                                    context.b_l = bot_left;
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y - 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }

                                        if (y < (roi->y + roi->h - 1)) {
                                            row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y + 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y + 1);

                                            bool recurse = false;
                                            for (int i = bot_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = top_left;
                                                    context.b_l = i + 1; // Don't test the same pixel again...
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y + 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }
                                    } else {
                                        blob_perimeter += (right - left + 1) * 2;
                                    }

                                    if (!lifo_size(&lifo)) {
                                        break_out = true;
                                        break;
                                    }

                                    xylr_t context;
                                    lifo_dequeue(&lifo, &context);
                                    x = context.x;
                                    y = context.y;
                                    left = context.l;
                                    right = context.r;
                                    top_left = context.t_l;
                                    bot_left = context.b_l;
                                }

                                if (break_out) {
                                    break;
                                }
                            }

                            rectangle_t rect;
                            rect.x = corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x; // l
                            rect.y = corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y; // t
                            rect.w = corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x - corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x + 1; // r - l + 1
                            rect.h = corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y - corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y + 1; // b - t + 1

                            if (((rect.w * rect.h) >= area_threshold) && (blob_pixels >= pixels_threshold)) {

                                // http://www.cse.usf.edu/~r1k/MachineVisionBook/MachineVision.files/MachineVision_Chapter2.pdf
                                // https://www.strchr.com/standard_deviation_in_one_pass
                                //
                                // a = sigma(x*x) + (mx*sigma(x)) + (mx*sigma(x)) + (sigma()*mx*mx)
                                // b = sigma(x*y) + (mx*sigma(y)) + (my*sigma(x)) + (sigma()*mx*my)
                                // c = sigma(y*y) + (my*sigma(y)) + (my*sigma(y)) + (sigma()*my*my)
                                //
                                // blob_a = sigma(x*x)
                                // blob_b = sigma(x*y)
                                // blob_c = sigma(y*y)
                                // blob_cx = sigma(x)
                                // blob_cy = sigma(y)
                                // blob_pixels = sigma()

                                float b_mx = blob_cx / ((float) blob_pixels);
                                float b_my = blob_cy / ((float) blob_pixels);
                                int mx = fast_roundf(b_mx); // x centroid
                                int my = fast_roundf(b_my); // y centroid
                                int small_blob_a = blob_a - ((mx * blob_cx) + (mx * blob_cx)) + (blob_pixels * mx * mx);
                                int small_blob_b = blob_b - ((mx * blob_cy) + (my * blob_cx)) + (blob_pixels * mx * my);
                                int small_blob_c = blob_c - ((my * blob_cy) + (my * blob_cy)) + (blob_pixels * my * my);

                                find_blobs_list_lnk_data_t lnk_blob;
                                memcpy(lnk_blob.corners, corners, FIND_BLOBS_CORNERS_RESOLUTION * sizeof(point_t));
                                memcpy(&lnk_blob.rect, &rect, sizeof(rectangle_t));
                                lnk_blob.pixels = blob_pixels;
                                lnk_blob.perimeter = blob_perimeter;
                                lnk_blob.code = 1 << code;
                                lnk_blob.count = 1;
                                lnk_blob.centroid_x = b_mx;
                                lnk_blob.centroid_y = b_my;
                                lnk_blob.rotation = (small_blob_a != small_blob_c) ? (fast_atan2f(2 * small_blob_b, small_blob_a - small_blob_c) / 2.0f) : 0.0f;
                                lnk_blob.roundness = calc_roundness(small_blob_a, small_blob_b, small_blob_c);
                                lnk_blob.x_hist_bins_count = 0;
                                lnk_blob.x_hist_bins = NULL;
                                lnk_blob.y_hist_bins_count = 0;
                                lnk_blob.y_hist_bins = NULL;
                                // These store the current average accumulation.
                                lnk_blob.centroid_x_acc = lnk_blob.centroid_x * lnk_blob.pixels;
                                lnk_blob.centroid_y_acc = lnk_blob.centroid_y * lnk_blob.pixels;
                                lnk_blob.rotation_acc_x = cosf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.rotation_acc_y = sinf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.roundness_acc = lnk_blob.roundness * lnk_blob.pixels;

                                if (x_hist_bins) {
                                    bin_up(x_hist_bins, ptr->w, x_hist_bins_max, &lnk_blob.x_hist_bins, &lnk_blob.x_hist_bins_count);
                                }

                                if (y_hist_bins) {
                                    bin_up(y_hist_bins, ptr->h, y_hist_bins_max, &lnk_blob.y_hist_bins, &lnk_blob.y_hist_bins_count);
                                }

                                bool add_to_list = threshold_cb_arg == NULL;
                                if (!add_to_list) {
                                    // Protect ourselves from caught exceptions in the callback
                                    // code from freeing our fb_alloc() stack.
                                    fb_alloc_mark();
                                    fb_alloc_mark_permanent();
                                    add_to_list = threshold_cb(threshold_cb_arg, &lnk_blob);
                                    fb_alloc_free_till_mark_past_mark_permanent();
                                }

                                if (add_to_list) {
                                    list_push_back(out, &lnk_blob);
                                } else {
                                    if (lnk_blob.x_hist_bins) xfree(lnk_blob.x_hist_bins);
                                    if (lnk_blob.y_hist_bins) xfree(lnk_blob.y_hist_bins);
                                }
                            }

                            x = old_x;
                            y = old_y;
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                for (int y = roi->y, yy = roi->y + roi->h, y_max = yy - 1; y < yy; y += y_stride) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = roi->x + (y % x_stride), xx = roi->x + roi->w, x_max = xx - 1; x < xx; x += x_stride) {
                        if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x))
                        && COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x), &lnk_data, invert)) {
                            int old_x = x;
                            int old_y = y;

                            float corners_acc[FIND_BLOBS_CORNERS_RESOLUTION];
                            point_t corners[FIND_BLOBS_CORNERS_RESOLUTION];
                            int corners_n[FIND_BLOBS_CORNERS_RESOLUTION];
                            // These values are initialized to their maximum before we minimize.
                            for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                corners[i].x = IM_MAX(IM_MIN(x_max * sign(cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), x_max), 0);
                                corners[i].y = IM_MAX(IM_MIN(y_max * sign(sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), y_max), 0);
                                corners_acc[i] = (corners[i].x * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                                 (corners[i].y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                corners_n[i] = 1;
                            }

                            int blob_pixels = 0;
                            int blob_perimeter = 0;
                            int blob_cx = 0;
                            int blob_cy = 0;
                            long long blob_a = 0;
                            long long blob_b = 0;
                            long long blob_c = 0;

                            if (x_hist_bins) memset(x_hist_bins, 0, ptr->w * sizeof(uint16_t));
                            if (y_hist_bins) memset(y_hist_bins, 0, ptr->h * sizeof(uint16_t));

                            // Scanline Flood Fill Algorithm //

                            for(;;) {
                                int left = x, right = x;
                                uint8_t *row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                                uint32_t *bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);

                                while ((left > roi->x)
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, left - 1))
                                && COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left - 1), &lnk_data, invert)) {
                                    left--;
                                }

                                while ((right < (roi->x + roi->w - 1))
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, right + 1))
                                && COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right + 1), &lnk_data, invert)) {
                                    right++;
                                }

                                for (int i = left; i <= right; i++) {
                                    IMAGE_SET_BINARY_PIXEL_FAST(bmp_row, i);
                                }

                                int sum = sum_m_to_n(left, right);
                                int sum_2 = sum_2_m_to_n(left, right);
                                int cnt = right - left + 1;
                                int avg = sum / cnt;

                                for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                    int x_new = (cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] > 0) ? left :
                                                ((cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] == 0) ? avg :
                                                                                                  right);
                                    float z = (x_new * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                              (y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                    if (z < corners_acc[i]) {
                                        corners_acc[i] = z;
                                        corners[i].x = x_new;
                                        corners[i].y = y;
                                        corners_n[i] = 1;
                                    } else if (z == corners_acc[i]) {
                                        corners[i].x = cumulative_moving_average(corners[i].x, x_new, corners_n[i]);
                                        corners[i].y = cumulative_moving_average(corners[i].y, y, corners_n[i]);
                                        corners_n[i] += 1;
                                    }
                                }

                                blob_pixels += cnt;
                                blob_perimeter += 2;
                                blob_cx += sum;
                                blob_cy += y * cnt;
                                blob_a += sum_2;
                                blob_b += y * sum;
                                blob_c += y * y * cnt;

                                if (y_hist_bins) y_hist_bins[y] += cnt;
                                if (x_hist_bins) for (int i = left; i <= right; i++) x_hist_bins[i] += 1;

                                int top_left = left;
                                int bot_left = left;
                                bool break_out = false;
                                for(;;) {
                                    if (lifo_size(&lifo) < lifo_len) {

                                        if (y > roi->y) {
                                            row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y - 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y - 1);

                                            bool recurse = false;
                                            for (int i = top_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = i + 1; // Don't test the same pixel again...
                                                    context.b_l = bot_left;
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y - 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }

                                        if (y < (roi->y + roi->h - 1)) {
                                            row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y + 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y + 1);

                                            bool recurse = false;
                                            for (int i = bot_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = top_left;
                                                    context.b_l = i + 1; // Don't test the same pixel again...
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y + 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }
                                    } else {
                                        blob_perimeter += (right - left + 1) * 2;
                                    }

                                    if (!lifo_size(&lifo)) {
                                        break_out = true;
                                        break;
                                    }

                                    xylr_t context;
                                    lifo_dequeue(&lifo, &context);
                                    x = context.x;
                                    y = context.y;
                                    left = context.l;
                                    right = context.r;
                                    top_left = context.t_l;
                                    bot_left = context.b_l;
                                }

                                if (break_out) {
                                    break;
                                }
                            }

                            rectangle_t rect;
                            rect.x = corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x; // l
                            rect.y = corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y; // t
                            rect.w = corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x - corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x + 1; // r - l + 1
                            rect.h = corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y - corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y + 1; // b - t + 1

                            if (((rect.w * rect.h) >= area_threshold) && (blob_pixels >= pixels_threshold)) {

                                // http://www.cse.usf.edu/~r1k/MachineVisionBook/MachineVision.files/MachineVision_Chapter2.pdf
                                // https://www.strchr.com/standard_deviation_in_one_pass
                                //
                                // a = sigma(x*x) + (mx*sigma(x)) + (mx*sigma(x)) + (sigma()*mx*mx)
                                // b = sigma(x*y) + (mx*sigma(y)) + (my*sigma(x)) + (sigma()*mx*my)
                                // c = sigma(y*y) + (my*sigma(y)) + (my*sigma(y)) + (sigma()*my*my)
                                //
                                // blob_a = sigma(x*x)
                                // blob_b = sigma(x*y)
                                // blob_c = sigma(y*y)
                                // blob_cx = sigma(x)
                                // blob_cy = sigma(y)
                                // blob_pixels = sigma()

                                float b_mx = blob_cx / ((float) blob_pixels);
                                float b_my = blob_cy / ((float) blob_pixels);
                                int mx = fast_roundf(b_mx); // x centroid
                                int my = fast_roundf(b_my); // y centroid
                                int small_blob_a = blob_a - ((mx * blob_cx) + (mx * blob_cx)) + (blob_pixels * mx * mx);
                                int small_blob_b = blob_b - ((mx * blob_cy) + (my * blob_cx)) + (blob_pixels * mx * my);
                                int small_blob_c = blob_c - ((my * blob_cy) + (my * blob_cy)) + (blob_pixels * my * my);

                                find_blobs_list_lnk_data_t lnk_blob;
                                memcpy(lnk_blob.corners, corners, FIND_BLOBS_CORNERS_RESOLUTION * sizeof(point_t));
                                memcpy(&lnk_blob.rect, &rect, sizeof(rectangle_t));
                                lnk_blob.pixels = blob_pixels;
                                lnk_blob.perimeter = blob_perimeter;
                                lnk_blob.code = 1 << code;
                                lnk_blob.count = 1;
                                lnk_blob.centroid_x = b_mx;
                                lnk_blob.centroid_y = b_my;
                                lnk_blob.rotation = (small_blob_a != small_blob_c) ? (fast_atan2f(2 * small_blob_b, small_blob_a - small_blob_c) / 2.0f) : 0.0f;
                                lnk_blob.roundness = calc_roundness(small_blob_a, small_blob_b, small_blob_c);
                                lnk_blob.x_hist_bins_count = 0;
                                lnk_blob.x_hist_bins = NULL;
                                lnk_blob.y_hist_bins_count = 0;
                                lnk_blob.y_hist_bins = NULL;
                                // These store the current average accumulation.
                                lnk_blob.centroid_x_acc = lnk_blob.centroid_x * lnk_blob.pixels;
                                lnk_blob.centroid_y_acc = lnk_blob.centroid_y * lnk_blob.pixels;
                                lnk_blob.rotation_acc_x = cosf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.rotation_acc_y = sinf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.roundness_acc = lnk_blob.roundness * lnk_blob.pixels;

                                if (x_hist_bins) {
                                    bin_up(x_hist_bins, ptr->w, x_hist_bins_max, &lnk_blob.x_hist_bins, &lnk_blob.x_hist_bins_count);
                                }

                                if (y_hist_bins) {
                                    bin_up(y_hist_bins, ptr->h, y_hist_bins_max, &lnk_blob.y_hist_bins, &lnk_blob.y_hist_bins_count);
                                }

                                bool add_to_list = threshold_cb_arg == NULL;
                                if (!add_to_list) {
                                    // Protect ourselves from caught exceptions in the callback
                                    // code from freeing our fb_alloc() stack.
                                    fb_alloc_mark();
                                    fb_alloc_mark_permanent();
                                    add_to_list = threshold_cb(threshold_cb_arg, &lnk_blob);
                                    fb_alloc_free_till_mark_past_mark_permanent();
                                }

                                if (add_to_list) {
                                    list_push_back(out, &lnk_blob);
                                } else {
                                    if (lnk_blob.x_hist_bins) xfree(lnk_blob.x_hist_bins);
                                    if (lnk_blob.y_hist_bins) xfree(lnk_blob.y_hist_bins);
                                }
                            }

                            x = old_x;
                            y = old_y;
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                for (int y = roi->y, yy = roi->y + roi->h, y_max = yy - 1; y < yy; y += y_stride) {
                    uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                    uint32_t *bmp_row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);
                    for (int x = roi->x + (y % x_stride), xx = roi->x + roi->w, x_max = xx - 1; x < xx; x += x_stride) {
                        if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row_ptr, x))
                        && COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x), &lnk_data, invert)) {
                            int old_x = x;
                            int old_y = y;

                            float corners_acc[FIND_BLOBS_CORNERS_RESOLUTION];
                            point_t corners[FIND_BLOBS_CORNERS_RESOLUTION];
                            int corners_n[FIND_BLOBS_CORNERS_RESOLUTION];
                            // Ensures that maximum goes all the way to the edge of the image.
                            for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                corners[i].x = IM_MAX(IM_MIN(x_max * sign(cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), x_max), 0);
                                corners[i].y = IM_MAX(IM_MIN(y_max * sign(sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]), y_max), 0);
                                corners_acc[i] = (corners[i].x * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                                 (corners[i].y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                corners_n[i] = 1;
                            }

                            int blob_pixels = 0;
                            int blob_perimeter = 0;
                            int blob_cx = 0;
                            int blob_cy = 0;
                            long long blob_a = 0;
                            long long blob_b = 0;
                            long long blob_c = 0;

                            if (x_hist_bins) memset(x_hist_bins, 0, ptr->w * sizeof(uint16_t));
                            if (y_hist_bins) memset(y_hist_bins, 0, ptr->h * sizeof(uint16_t));

                            // Scanline Flood Fill Algorithm //

                            for(;;) {
                                int left = x, right = x;
                                uint16_t *row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                                uint32_t *bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y);

                                while ((left > roi->x)
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, left - 1))
                                && COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, left - 1), &lnk_data, invert)) {
                                    left--;
                                }

                                while ((right < (roi->x + roi->w - 1))
                                && (!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, right + 1))
                                && COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, right + 1), &lnk_data, invert)) {
                                    right++;
                                }

                                for (int i = left; i <= right; i++) {
                                    IMAGE_SET_BINARY_PIXEL_FAST(bmp_row, i);
                                }

                                int sum = sum_m_to_n(left, right);
                                int sum_2 = sum_2_m_to_n(left, right);
                                int cnt = right - left + 1;
                                int avg = sum / cnt;

                                for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                                    int x_new = (cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] > 0) ? left :
                                                ((cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i] == 0) ? avg :
                                                                                                  right);
                                    float z = (x_new * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                              (y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                                    if (z < corners_acc[i]) {
                                        corners_acc[i] = z;
                                        corners[i].x = x_new;
                                        corners[i].y = y;
                                        corners_n[i] = 1;
                                    } else if (z == corners_acc[i]) {
                                        corners[i].x = cumulative_moving_average(corners[i].x, x_new, corners_n[i]);
                                        corners[i].y = cumulative_moving_average(corners[i].y, y, corners_n[i]);
                                        corners_n[i] += 1;
                                    }
                                }

                                blob_pixels += cnt;
                                blob_perimeter += 2;
                                blob_cx += sum;
                                blob_cy += y * cnt;
                                blob_a += sum_2;
                                blob_b += y * sum;
                                blob_c += y * y * cnt;

                                if (y_hist_bins) y_hist_bins[y] += cnt;
                                if (x_hist_bins) for (int i = left; i <= right; i++) x_hist_bins[i] += 1;

                                int top_left = left;
                                int bot_left = left;
                                bool break_out = false;
                                for(;;) {
                                    if (lifo_size(&lifo) < lifo_len) {

                                        if (y > roi->y) {
                                            row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y - 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y - 1);

                                            bool recurse = false;
                                            for (int i = top_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = i + 1; // Don't test the same pixel again...
                                                    context.b_l = bot_left;
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y - 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }

                                        if (y < (roi->y + roi->h - 1)) {
                                            row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y + 1);
                                            bmp_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(&bmp, y + 1);

                                            bool recurse = false;
                                            for (int i = bot_left; i <= right; i++) {
                                                bool ok = true; // Does nothing if thresholding is skipped.

                                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(bmp_row, i))
                                                && (ok = COLOR_THRESHOLD_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i), &lnk_data, invert))) {
                                                    xylr_t context;
                                                    context.x = x;
                                                    context.y = y;
                                                    context.l = left;
                                                    context.r = right;
                                                    context.t_l = top_left;
                                                    context.b_l = i + 1; // Don't test the same pixel again...
                                                    lifo_enqueue(&lifo, &context);
                                                    x = i;
                                                    y = y + 1;
                                                    recurse = true;
                                                    break;
                                                }

                                                blob_perimeter += (!ok) && (i != left) && (i != right);
                                            }
                                            if (recurse) {
                                                break;
                                            }
                                        } else {
                                            blob_perimeter += right - left + 1;
                                        }
                                    } else {
                                        blob_perimeter += (right - left + 1) * 2;
                                    }

                                    if (!lifo_size(&lifo)) {
                                        break_out = true;
                                        break;
                                    }

                                    xylr_t context;
                                    lifo_dequeue(&lifo, &context);
                                    x = context.x;
                                    y = context.y;
                                    left = context.l;
                                    right = context.r;
                                    top_left = context.t_l;
                                    bot_left = context.b_l;
                                }

                                if (break_out) {
                                    break;
                                }
                            }

                            rectangle_t rect;
                            rect.x = corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x; // l
                            rect.y = corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y; // t
                            rect.w = corners[(FIND_BLOBS_CORNERS_RESOLUTION*2)/4].x - corners[(FIND_BLOBS_CORNERS_RESOLUTION*0)/4].x + 1; // r - l + 1
                            rect.h = corners[(FIND_BLOBS_CORNERS_RESOLUTION*3)/4].y - corners[(FIND_BLOBS_CORNERS_RESOLUTION*1)/4].y + 1; // b - t + 1

                            if (((rect.w * rect.h) >= area_threshold) && (blob_pixels >= pixels_threshold)) {

                                // http://www.cse.usf.edu/~r1k/MachineVisionBook/MachineVision.files/MachineVision_Chapter2.pdf
                                // https://www.strchr.com/standard_deviation_in_one_pass
                                //
                                // a = sigma(x*x) + (mx*sigma(x)) + (mx*sigma(x)) + (sigma()*mx*mx)
                                // b = sigma(x*y) + (mx*sigma(y)) + (my*sigma(x)) + (sigma()*mx*my)
                                // c = sigma(y*y) + (my*sigma(y)) + (my*sigma(y)) + (sigma()*my*my)
                                //
                                // blob_a = sigma(x*x)
                                // blob_b = sigma(x*y)
                                // blob_c = sigma(y*y)
                                // blob_cx = sigma(x)
                                // blob_cy = sigma(y)
                                // blob_pixels = sigma()

                                float b_mx = blob_cx / ((float) blob_pixels);
                                float b_my = blob_cy / ((float) blob_pixels);
                                int mx = fast_roundf(b_mx); // x centroid
                                int my = fast_roundf(b_my); // y centroid
                                int small_blob_a = blob_a - ((mx * blob_cx) + (mx * blob_cx)) + (blob_pixels * mx * mx);
                                int small_blob_b = blob_b - ((mx * blob_cy) + (my * blob_cx)) + (blob_pixels * mx * my);
                                int small_blob_c = blob_c - ((my * blob_cy) + (my * blob_cy)) + (blob_pixels * my * my);

                                find_blobs_list_lnk_data_t lnk_blob;
                                memcpy(lnk_blob.corners, corners, FIND_BLOBS_CORNERS_RESOLUTION * sizeof(point_t));
                                memcpy(&lnk_blob.rect, &rect, sizeof(rectangle_t));
                                lnk_blob.pixels = blob_pixels;
                                lnk_blob.perimeter = blob_perimeter;
                                lnk_blob.code = 1 << code;
                                lnk_blob.count = 1;
                                lnk_blob.centroid_x = b_mx;
                                lnk_blob.centroid_y = b_my;
                                lnk_blob.rotation = (small_blob_a != small_blob_c) ? (fast_atan2f(2 * small_blob_b, small_blob_a - small_blob_c) / 2.0f) : 0.0f;
                                lnk_blob.roundness = calc_roundness(small_blob_a, small_blob_b, small_blob_c);
                                lnk_blob.x_hist_bins_count = 0;
                                lnk_blob.x_hist_bins = NULL;
                                lnk_blob.y_hist_bins_count = 0;
                                lnk_blob.y_hist_bins = NULL;
                                // These store the current average accumulation.
                                lnk_blob.centroid_x_acc = lnk_blob.centroid_x * lnk_blob.pixels;
                                lnk_blob.centroid_y_acc = lnk_blob.centroid_y * lnk_blob.pixels;
                                lnk_blob.rotation_acc_x = cosf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.rotation_acc_y = sinf(lnk_blob.rotation) * lnk_blob.pixels;
                                lnk_blob.roundness_acc = lnk_blob.roundness * lnk_blob.pixels;

                                if (x_hist_bins) {
                                    bin_up(x_hist_bins, ptr->w, x_hist_bins_max, &lnk_blob.x_hist_bins, &lnk_blob.x_hist_bins_count);
                                }

                                if (y_hist_bins) {
                                    bin_up(y_hist_bins, ptr->h, y_hist_bins_max, &lnk_blob.y_hist_bins, &lnk_blob.y_hist_bins_count);
                                }

                                bool add_to_list = threshold_cb_arg == NULL;
                                if (!add_to_list) {
                                    // Protect ourselves from caught exceptions in the callback
                                    // code from freeing our fb_alloc() stack.
                                    fb_alloc_mark();
                                    fb_alloc_mark_permanent();
                                    add_to_list = threshold_cb(threshold_cb_arg, &lnk_blob);
                                    fb_alloc_free_till_mark_past_mark_permanent();
                                }

                                if (add_to_list) {
                                    list_push_back(out, &lnk_blob);
                                } else {
                                    if (lnk_blob.x_hist_bins) xfree(lnk_blob.x_hist_bins);
                                    if (lnk_blob.y_hist_bins) xfree(lnk_blob.y_hist_bins);
                                }
                            }

                            x = old_x;
                            y = old_y;
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }

        code += 1;
    }

    lifo_free(&lifo);
    if (y_hist_bins) fb_free();
    if (x_hist_bins) fb_free();
    fb_free(); // bitmap

    if (merge) {
        for(;;) {
            bool merge_occured = false;

            list_t out_temp;
            list_init(&out_temp, sizeof(find_blobs_list_lnk_data_t));

            while(list_size(out)) {
                find_blobs_list_lnk_data_t lnk_blob;
                list_pop_front(out, &lnk_blob);

                for (size_t k = 0, l = list_size(out); k < l; k++) {
                    find_blobs_list_lnk_data_t tmp_blob;
                    list_pop_front(out, &tmp_blob);

                    rectangle_t temp;
                    temp.x = IM_MAX(IM_MIN(tmp_blob.rect.x - margin, INT16_MAX), INT16_MIN);
                    temp.y = IM_MAX(IM_MIN(tmp_blob.rect.y - margin, INT16_MAX), INT16_MIN);
                    temp.w = IM_MAX(IM_MIN(tmp_blob.rect.w + (margin * 2), INT16_MAX), 0);
                    temp.h = IM_MAX(IM_MIN(tmp_blob.rect.h + (margin * 2), INT16_MAX), 0);

                    if (rectangle_overlap(&(lnk_blob.rect), &temp)
                    && ((merge_cb_arg == NULL) || merge_cb(merge_cb_arg, &lnk_blob, &tmp_blob))) {
                        // Have to merge these first before merging rects.
                        if (x_hist_bins_max) merge_bins(lnk_blob.rect.x, lnk_blob.rect.x + lnk_blob.rect.w - 1, &lnk_blob.x_hist_bins, &lnk_blob.x_hist_bins_count,
                                                        tmp_blob.rect.x, tmp_blob.rect.x + tmp_blob.rect.w - 1, &tmp_blob.x_hist_bins, &tmp_blob.x_hist_bins_count,
                                                        x_hist_bins_max);
                        if (y_hist_bins_max) merge_bins(lnk_blob.rect.y, lnk_blob.rect.y + lnk_blob.rect.h - 1, &lnk_blob.y_hist_bins, &lnk_blob.y_hist_bins_count,
                                                        tmp_blob.rect.y, tmp_blob.rect.y + tmp_blob.rect.h - 1, &tmp_blob.y_hist_bins, &tmp_blob.y_hist_bins_count,
                                                        y_hist_bins_max);
                        // Merge corners...
                        for (int i = 0; i < FIND_BLOBS_CORNERS_RESOLUTION; i++) {
                            float z_dst = (lnk_blob.corners[i].x * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                          (lnk_blob.corners[i].y * sin_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                            float z_src = (tmp_blob.corners[i].x * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]) +
                                          (tmp_blob.corners[i].y * cos_table[FIND_BLOBS_ANGLE_RESOLUTION*i]);
                            if (z_src < z_dst) {
                                lnk_blob.corners[i].x = tmp_blob.corners[i].x;
                                lnk_blob.corners[i].y = tmp_blob.corners[i].y;
                            }
                        }
                        // Merge rects...
                        rectangle_united(&(lnk_blob.rect), &(tmp_blob.rect));
                        // Merge counters...
                        lnk_blob.pixels += tmp_blob.pixels; // won't overflow
                        lnk_blob.perimeter += tmp_blob.perimeter; // won't overflow
                        lnk_blob.code |= tmp_blob.code; // won't overflow
                        lnk_blob.count += tmp_blob.count; // won't overflow
                        // Merge accumulators...
                        lnk_blob.centroid_x_acc += tmp_blob.centroid_x_acc;
                        lnk_blob.centroid_y_acc += tmp_blob.centroid_y_acc;
                        lnk_blob.rotation_acc_x += tmp_blob.rotation_acc_x;
                        lnk_blob.rotation_acc_y += tmp_blob.rotation_acc_y;
                        lnk_blob.roundness_acc += tmp_blob.roundness_acc;
                        // Compute current values...
                        lnk_blob.centroid_x = lnk_blob.centroid_x_acc / lnk_blob.pixels;
                        lnk_blob.centroid_y = lnk_blob.centroid_y_acc / lnk_blob.pixels;
                        lnk_blob.rotation = fast_atan2f(lnk_blob.rotation_acc_y / lnk_blob.pixels,
                                                        lnk_blob.rotation_acc_x / lnk_blob.pixels);
                        lnk_blob.roundness = lnk_blob.roundness_acc / lnk_blob.pixels;
                        merge_occured = true;
                    } else {
                        list_push_back(out, &tmp_blob);
                    }
                }

                list_push_back(&out_temp, &lnk_blob);
            }

            list_copy(out, &out_temp);

            if (!merge_occured) {
                break;
            }
        }
    }
}

void imlib_flood_fill_int(image_t *out, image_t *img, int x, int y,
                          int seed_threshold, int floating_threshold,
                          flood_fill_call_back_t cb, void *data)
{
    lifo_t lifo;
    size_t lifo_len;
    lifo_alloc_all(&lifo, &lifo_len, sizeof(xylr_t));

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            for(int seed_pixel = IMAGE_GET_BINARY_PIXEL(img, x, y);;) {
                int left = x, right = x;
                uint32_t *row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                uint32_t *out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);

                while ((left > 0)
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, left - 1))
                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, left - 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, left - 1),
                                      IMAGE_GET_BINARY_PIXEL_FAST(row, left), floating_threshold)) {
                    left--;
                }

                while ((right < (img->w - 1))
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, right + 1))
                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, right + 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, right + 1),
                                      IMAGE_GET_BINARY_PIXEL_FAST(row, right), floating_threshold)) {
                    right++;
                }

                for (int i = left; i <= right; i++) {
                    IMAGE_SET_BINARY_PIXEL_FAST(out_row, i);
                }

                int top_left = left;
                int bot_left = left;
                bool break_out = false;
                for(;;) {
                    if (lifo_size(&lifo) < lifo_len) {
                        uint32_t *old_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);

                        if (y > 0) {
                            row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y - 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y - 1);

                            bool recurse = false;
                            for (int i = top_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i),
                                                      IMAGE_GET_BINARY_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = i + 1; // Don't test the same pixel again...
                                    context.b_l = bot_left;
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y - 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }

                        if (y < (img->h - 1)) {
                            row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y + 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y + 1);

                            bool recurse = false;
                            for (int i = bot_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_BINARY(IMAGE_GET_BINARY_PIXEL_FAST(row, i),
                                                      IMAGE_GET_BINARY_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = top_left;
                                    context.b_l = i + 1; // Don't test the same pixel again...
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y + 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }
                    }

                    if (cb) cb(img, y, left, right, data);

                    if (!lifo_size(&lifo)) {
                        break_out = true;
                        break;
                    }

                    xylr_t context;
                    lifo_dequeue(&lifo, &context);
                    x = context.x;
                    y = context.y;
                    left = context.l;
                    right = context.r;
                    top_left = context.t_l;
                    bot_left = context.b_l;
                }

                if (break_out) {
                    break;
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for(int seed_pixel = IMAGE_GET_GRAYSCALE_PIXEL(img, x, y);;) {
                int left = x, right = x;
                uint8_t *row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint32_t *out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);

                while ((left > 0)
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, left - 1))
                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left - 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left - 1),
                                         IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, left), floating_threshold)) {
                    left--;
                }

                while ((right < (img->w - 1))
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, right + 1))
                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right + 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right + 1),
                                         IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, right), floating_threshold)) {
                    right++;
                }

                for (int i = left; i <= right; i++) {
                    IMAGE_SET_BINARY_PIXEL_FAST(out_row, i);
                }

                int top_left = left;
                int bot_left = left;
                bool break_out = false;
                for(;;) {
                    if (lifo_size(&lifo) < lifo_len) {
                        uint8_t *old_row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);

                        if (y > 0) {
                            row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y - 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y - 1);

                            bool recurse = false;
                            for (int i = top_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i),
                                                         IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = i + 1; // Don't test the same pixel again...
                                    context.b_l = bot_left;
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y - 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }

                        if (y < (img->h - 1)) {
                            row = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y + 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y + 1);

                            bool recurse = false;
                            for (int i = bot_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_GRAYSCALE(IMAGE_GET_GRAYSCALE_PIXEL_FAST(row, i),
                                                         IMAGE_GET_GRAYSCALE_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = top_left;
                                    context.b_l = i + 1; // Don't test the same pixel again...
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y + 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }
                    }

                    if (cb) cb(img, y, left, right, data);

                    if (!lifo_size(&lifo)) {
                        break_out = true;
                        break;
                    }

                    xylr_t context;
                    lifo_dequeue(&lifo, &context);
                    x = context.x;
                    y = context.y;
                    left = context.l;
                    right = context.r;
                    top_left = context.t_l;
                    bot_left = context.b_l;
                }

                if (break_out) {
                    break;
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for(int seed_pixel = IMAGE_GET_RGB565_PIXEL(img, x, y);;) {
                int left = x, right = x;
                uint16_t *row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                uint32_t *out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y);

                while ((left > 0)
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, left - 1))
                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, left - 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, left - 1),
                                      IMAGE_GET_RGB565_PIXEL_FAST(row, left), floating_threshold)) {
                    left--;
                }

                while ((right < (img->w - 1))
                && (!IMAGE_GET_BINARY_PIXEL_FAST(out_row, right + 1))
                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, right + 1), seed_pixel, seed_threshold)
                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, right + 1),
                                      IMAGE_GET_RGB565_PIXEL_FAST(row, right), floating_threshold)) {
                    right++;
                }

                for (int i = left; i <= right; i++) {
                    IMAGE_SET_BINARY_PIXEL_FAST(out_row, i);
                }

                int top_left = left;
                int bot_left = left;
                bool break_out = false;
                for(;;) {
                    if (lifo_size(&lifo) < lifo_len) {
                        uint16_t *old_row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);

                        if (y > 0) {
                            row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y - 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y - 1);

                            bool recurse = false;
                            for (int i = top_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i),
                                                      IMAGE_GET_RGB565_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = i + 1; // Don't test the same pixel again...
                                    context.b_l = bot_left;
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y - 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }

                        if (y < (img->h - 1)) {
                            row = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y + 1);
                            out_row = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(out, y + 1);

                            bool recurse = false;
                            for (int i = bot_left; i <= right; i++) {
                                if ((!IMAGE_GET_BINARY_PIXEL_FAST(out_row, i))
                                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i), seed_pixel, seed_threshold)
                                && COLOR_BOUND_RGB565(IMAGE_GET_RGB565_PIXEL_FAST(row, i),
                                                      IMAGE_GET_RGB565_PIXEL_FAST(old_row, i), floating_threshold)) {
                                    xylr_t context;
                                    context.x = x;
                                    context.y = y;
                                    context.l = left;
                                    context.r = right;
                                    context.t_l = top_left;
                                    context.b_l = i + 1; // Don't test the same pixel again...
                                    lifo_enqueue(&lifo, &context);
                                    x = i;
                                    y = y + 1;
                                    recurse = true;
                                    break;
                                }
                            }
                            if (recurse) {
                                break;
                            }
                        }
                    }

                    if (cb) cb(img, y, left, right, data);

                    if (!lifo_size(&lifo)) {
                        break_out = true;
                        break;
                    }

                    xylr_t context;
                    lifo_dequeue(&lifo, &context);
                    x = context.x;
                    y = context.y;
                    left = context.l;
                    right = context.r;
                    top_left = context.t_l;
                    bot_left = context.b_l;
                }

                if (break_out) {
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    lifo_free(&lifo);
}
