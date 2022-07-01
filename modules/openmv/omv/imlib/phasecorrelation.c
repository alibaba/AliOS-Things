/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Phase correlation.
 */
#include "imlib.h"
#include "fft.h"

void imlib_logpolar_int(image_t *dst, image_t *src, rectangle_t *roi, bool linear, bool reverse)
{
    int w = roi->w; // == dst_w
    int h = roi->h; // == dst_h
    int w_2 = w / 2;
    int h_2 = h / 2;
    float rho_scale = fast_sqrtf((w_2 * w_2) + (h_2 * h_2));
    if (!linear) rho_scale = fast_log(rho_scale);
    const float m_pi_1_5 = 1.5f * M_PI;
    const float m_pi_1_5_d = IM_RAD2DEG(m_pi_1_5);
    const float m_pi_2_0 = 2.0f * M_PI;
    const float m_pi_2_0_d = IM_RAD2DEG(m_pi_2_0);
    const int m_pi_2_0_d_i = m_pi_2_0_d;
    float theta_scale_d = m_pi_2_0_d / (w - 2);
    float theta_scale_inv = w / m_pi_2_0;

    if (!reverse) {
        rho_scale /= h;

        switch (src->bpp) {
            case IMAGE_BPP_BINARY: {
                uint32_t *tmp = (uint32_t *) src->data;
                int tmp_w = src->w, tmp_h = src->h, tmp_x = roi->x + w_2 - 1, tmp_y = roi->y + h_2;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, y);
                    float rho = y * rho_scale;
                    if (!linear) rho = fast_expf(rho);
                    for (int x = 0, xx = w_2; x < xx; x++) {

                        int theta = fast_roundf(m_pi_1_5_d - (x * theta_scale_d));
                        if (theta < 0) theta += m_pi_2_0_d_i; // wrap for table access
                        int sourceX = tmp_x + fast_roundf(rho * cos_table[theta]); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * sin_table[theta]); // rounding is necessary

                        if ((0 <= sourceX) && (0 <= sourceY) && (sourceY < tmp_h)) { // plot the 2 symmetrical pixels
                            uint32_t *ptr, pixel;
                            ptr = tmp + (((tmp_w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY);
                            pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, pixel);
                            pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, tmp_w-1-sourceX);
                            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, w-1-x, pixel);
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                uint8_t *tmp = (uint8_t *) src->data;
                int tmp_w = src->w, tmp_h = src->h, tmp_x = roi->x + w_2 - 1, tmp_y = roi->y + h_2;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, y);
                    float rho = y * rho_scale;
                    if (!linear) rho = fast_expf(rho);
                    for (int x = 0, xx = w_2; x < xx; x++) {

                        int theta = fast_roundf(m_pi_1_5_d - (x * theta_scale_d));
                        if (theta < 0) theta += m_pi_2_0_d_i; // wrap for table access
                        int sourceX = tmp_x + fast_roundf(rho * cos_table[theta]); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * sin_table[theta]); // rounding is necessary

                        if ((0 <= sourceX) && (0 <= sourceY) && (sourceY < tmp_h)) { // plot the 2 symmetrical pixels
                            uint8_t *ptr, pixel;
                            ptr = tmp + (tmp_w * sourceY);
                            pixel = ptr[sourceX];
                            row_ptr[x] = pixel;
                            pixel = ptr[tmp_w - 1 - sourceX];
                            row_ptr[w - 1 - x] = pixel;
                        }
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                uint16_t *tmp = (uint16_t *) src->data;
                int tmp_w = src->w, tmp_h = src->h, tmp_x = roi->x + w_2 - 1, tmp_y = roi->y + h_2;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, y);
                    float rho = y * rho_scale;
                    if (!linear) rho = fast_expf(rho);
                    for (int x = 0, xx = w_2; x < xx; x++) {

                        int theta = fast_roundf(m_pi_1_5_d - (x * theta_scale_d));
                        if (theta < 0) theta += m_pi_2_0_d_i; // wrap for table access
                        int sourceX = tmp_x + fast_roundf(rho * cos_table[theta]); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * sin_table[theta]); // rounding is necessary

                        if ((0 <= sourceX) && (0 <= sourceY) && (sourceY < tmp_h)) { // plot the 2 symmetrical pixels
                            uint16_t *ptr, pixel;
                            ptr = tmp + (tmp_w * sourceY);
                            pixel = ptr[sourceX];
                            row_ptr[x] = pixel;
                            pixel = ptr[tmp_w - 1 - sourceX];
                            row_ptr[w - 1 - x] = pixel;
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    } else {
        float rho_scale_inv = (h - 1) / rho_scale;
        switch (src->bpp) {
            case IMAGE_BPP_BINARY: {
                uint32_t *tmp = (uint32_t *) src->data;
                int tmp_w = src->w, tmp_x = roi->x, tmp_y = roi->y;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, y);
                    int y_2 = y - h_2;
                    int y_2_2 = y_2 * y_2;

                    for (int x = 0, xx = w_2; x < xx; x++) {
                        int x_2 = x - w_2;
                        int x_2_2 = x_2 * x_2;

                        float rho = fast_sqrtf(x_2_2 + y_2_2);
                        if (!linear) rho = fast_log(rho);
                        float theta = m_pi_1_5 - fast_atan2f(y_2, x_2);
                        int sourceX = tmp_x + fast_roundf(theta * theta_scale_inv); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * rho_scale_inv); // rounding is necessary

                        // plot the 2 symmetrical pixels
                        uint32_t *ptr, pixel;
                        ptr = tmp + (((tmp_w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY);
                        pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX);
                        IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, pixel);
                        pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, tmp_w-1-sourceX);
                        IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, w-1-x, pixel);
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                uint8_t *tmp = (uint8_t *) src->data;
                int tmp_w = src->w, tmp_x = roi->x, tmp_y = roi->y;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, y);
                    int y_2 = y - h_2;
                    int y_2_2 = y_2 * y_2;

                    for (int x = 0, xx = w_2; x < xx; x++) {
                        int x_2 = x - w_2;
                        int x_2_2 = x_2 * x_2;

                        float rho = fast_sqrtf(x_2_2 + y_2_2);
                        if (!linear) rho = fast_log(rho);
                        float theta = m_pi_1_5 - fast_atan2f(y_2, x_2);
                        int sourceX = tmp_x + fast_roundf(theta * theta_scale_inv); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * rho_scale_inv); // rounding is necessary

                        // plot the 2 symmetrical pixels
                        uint8_t *ptr, pixel;
                        ptr = tmp + (tmp_w * sourceY);
                        pixel = ptr[sourceX];
                        row_ptr[x] = pixel;
                        pixel = ptr[tmp_w - 1 - sourceX];
                        row_ptr[w - 1 - x] = pixel;
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                uint16_t *tmp = (uint16_t *) src->data;
                int tmp_w = src->w, tmp_x = roi->x, tmp_y = roi->y;

                for (int y = 0, yy = h; y < yy; y++) {
                    uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, y);
                    int y_2 = y - h_2;
                    int y_2_2 = y_2 * y_2;

                    for (int x = 0, xx = w_2; x < xx; x++) {
                        int x_2 = x - w_2;
                        int x_2_2 = x_2 * x_2;

                        float rho = fast_sqrtf(x_2_2 + y_2_2);
                        if (!linear) rho = fast_log(rho);
                        float theta = m_pi_1_5 - fast_atan2f(y_2, x_2);
                        int sourceX = tmp_x + fast_roundf(theta * theta_scale_inv); // rounding is necessary
                        int sourceY = tmp_y + fast_roundf(rho * rho_scale_inv); // rounding is necessary

                        // plot the 2 symmetrical pixels
                        uint16_t *ptr, pixel;
                        ptr = tmp + (tmp_w * sourceY);
                        pixel = ptr[sourceX];
                        row_ptr[x] = pixel;
                        pixel = ptr[tmp_w - 1 - sourceX];
                        row_ptr[w - 1 - x] = pixel;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

#if defined(IMLIB_ENABLE_LOGPOLAR) || defined(IMLIB_ENABLE_LINPOLAR)
void imlib_logpolar(image_t *img, bool linear, bool reverse)
{
    image_t img_2;
    img_2.w = img->w;
    img_2.h = img->h;
    img_2.bpp = img->bpp;

    rectangle_t rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = img->w;
    rect.h = img->h;

    size_t size = image_size(img);
    img_2.data = fb_alloc(size, FB_ALLOC_NO_HINT);
    memcpy(img_2.data, img->data, size);
    memset(img->data, 0, size);

    imlib_logpolar_int(img, &img_2, &rect, linear, reverse);

    fb_free();
}
#endif //defined(IMLIB_ENABLE_LOGPOLAR) || defined(IMLIB_ENABLE_LINPOLAR)

#ifdef IMLIB_ENABLE_FIND_DISPLACEMENT
// Note that both ROI widths and heights must be equal.
void imlib_phasecorrelate(image_t *img0, image_t *img1, rectangle_t *roi0, rectangle_t *roi1, bool logpolar, bool fix_rotation_scale,
                          float *x_translation, float *y_translation, float *rotation, float *scale, float *response)
{
    // Step 1 - Get Rotation/Scale Differences
    if ((!logpolar) && fix_rotation_scale) {
        fft2d_controller_t fft0, fft1;

        fft2d_alloc(&fft0, img0, roi0);
        fft2d_alloc(&fft1, img1, roi1);

        fft2d_run(&fft0);
        fft2d_run(&fft1);

        fft2d_mag(&fft0);
        fft2d_mag(&fft1);

        fft2d_swap(&fft0);
        fft2d_swap(&fft1);

        fft2d_logpolar(&fft0);
        fft2d_logpolar(&fft1);

        fft2d_run_again(&fft0);
        fft2d_run_again(&fft1);

        int w = (1 << fft0.w_pow2);
        int h = (1 << fft0.h_pow2);

        for (int i = 0, j = h * w * 2; i < j; i += 2) {
            float ga_r = fft0.data[i+0];
            float ga_i = fft0.data[i+1];
            float gb_r = fft1.data[i+0];
            float gb_i = -fft1.data[i+1]; // complex conjugate...
            float hp_r = (ga_r * gb_r) - (ga_i * gb_i); // hadamard product
            float hp_i = (ga_r * gb_i) + (ga_i * gb_r); // hadamard product
            float mag = 1 / fast_sqrtf((hp_r*hp_r)+(hp_i*hp_i)); // magnitude
            // Replace first fft with phase correlation...
            fft0.data[i+0] = hp_r * mag;
            fft0.data[i+1] = hp_i * mag;
        }

        ifft2d_run(&fft0);

        float sum = 0;
        float max = 0;
        int off_x = 0;
        int off_y = 0;

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                // Note that the output of the FFT is packed with real data in both
                // the real and imaginary parts... (right side of the array is zero).
                float f_r = fft0.data[(i * w * 2) + j];
                sum += f_r;
                if (f_r > max) {
                    max = f_r;
                    off_x = j;
                    off_y = i;
                }
            }
        }

        float tmp_response = max / sum; // normalize this to [0:1].

        float f_sum = 0;
        float f_off_x = 0;
        float f_off_y = 0;

        for (int i = -2; i < 2; i++) {
            for (int j = -2; j < 2; j++) {

                // Wrap around
                int new_x = off_x + j;
                if (new_x < 0) new_x += w;
                if (new_x >= w) new_x -= w;

                // Wrap around
                int new_y = off_y + i;
                if (new_y < 0) new_y += h;
                if (new_y >= h) new_y -= h;

                // Compute centroid.
                float f_r = fft0.data[(new_y * w * 2) + new_x];
                f_off_x += (off_x + j) * f_r; // don't use new_x here
                f_off_y += (off_y + i) * f_r; // don't use new_y here
                f_sum += f_r;
            }
        }

        f_off_x /= f_sum;
        f_off_y /= f_sum;

        // FFT Shift X
        if (f_off_x >= (w/2.0f)) {
            f_off_x = f_off_x - w;
        } else {
            f_off_x = f_off_x;
        }

        // FFT Shift Y
        if (f_off_y >= (h/2.0f)) {
            f_off_y = -(f_off_y - h);
        } else {
            f_off_y = -f_off_y;
        }

        if ((f_off_x < (-w/2.0f))
        || ((w/2.0f) <= f_off_x)
        || (f_off_y < (-h/2.0f))
        || ((h/2.0f) <= f_off_y)
        || isnanf(f_off_x)
        || isinff(f_off_x)
        || isnanf(f_off_y)
        || isinff(f_off_y)
        || isnanf(tmp_response)
        || isinff(tmp_response)) { // Noise Filter
            f_off_x = 0;
            f_off_y = 0;
            tmp_response = 0;
        }

        fft2d_dealloc(); // fft1
        fft2d_dealloc(); // fft0

        float w_2 = roi0->w / 2.0f;
        float h_2 = roi0->h / 2.0f;
        float rho_scale = fast_log(fast_sqrtf((w_2 * w_2) + (h_2 * h_2))) / roi0->h;
        float theta_scale = (2 * M_PI) / roi0->w;

        *rotation = f_off_x * theta_scale;
        *scale = (f_off_y * rho_scale) + 1;
    } else {
        *rotation = 0;
        *scale = 0;
    }

    image_t img0_fixed;
    rectangle_t roi0_fixed;

    // Step 2 - Fix Rotation/Scale Differences
    if ((!logpolar) && fix_rotation_scale) {

        img0_fixed.w = roi0->w;
        img0_fixed.h = roi0->h;
        img0_fixed.bpp = img0->bpp;
        img0_fixed.data = fb_alloc(image_size(&img0_fixed), FB_ALLOC_NO_HINT);

        roi0_fixed.x = 0;
        roi0_fixed.y = 0;
        roi0_fixed.w = roi0->w;
        roi0_fixed.h = roi0->h;

        switch(img0->bpp) {
            case IMAGE_BPP_BINARY: {
                for (int y = roi0->y, yy = roi0->y + roi0->h; y < yy; y++) {
                    uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img0, y);
                    for (int x = roi0->x, xx = roi0->x + roi0->w; x < xx; x++) {
                        IMAGE_PUT_BINARY_PIXEL(&img0_fixed, x, y, IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                    }
                }
                break;
            }
            case IMAGE_BPP_GRAYSCALE: {
                for (int y = roi0->y, yy = roi0->y + roi0->h; y < yy; y++) {
                    uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img0, y);
                    for (int x = roi0->x, xx = roi0->x + roi0->w; x < xx; x++) {
                        IMAGE_PUT_GRAYSCALE_PIXEL(&img0_fixed, x, y, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                    }
                }
                break;
            }
            case IMAGE_BPP_RGB565: {
                for (int y = roi0->y, yy = roi0->y + roi0->h; y < yy; y++) {
                    uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img0, y);
                    for (int x = roi0->x, xx = roi0->x + roi0->w; x < xx; x++) {
                        IMAGE_PUT_RGB565_PIXEL(&img0_fixed, x, y, IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                    }
                }
                break;
            }
            default: {
                memset(img0_fixed.data, 0, image_size(&img0_fixed));
                break;
            }
        }

        imlib_rotation_corr(&img0_fixed, 0, 0, *rotation, 0, 0, *scale, 60, NULL);
    } else {
        memcpy(&img0_fixed, img0, sizeof(image_t));
        memcpy(&roi0_fixed, roi0, sizeof(rectangle_t));
    }

    // Step 3 - Get Translation Differences
    {
        image_t img0alt, img1alt;
        rectangle_t roi0alt, roi1alt;

        if (logpolar) {
            img0alt.w = roi0_fixed.w;
            img0alt.h = roi0_fixed.h;
            img0alt.bpp = img0_fixed.bpp;
            img0alt.data = fb_alloc0(image_size(&img0alt), FB_ALLOC_NO_HINT);
            imlib_logpolar_int(&img0alt, &img0_fixed, &roi0_fixed, false, false);
            roi0alt.x = 0;
            roi0alt.y = 0;
            roi0alt.w = roi0_fixed.w;
            roi0alt.h = roi0_fixed.h;

            img1alt.w = roi1->w;
            img1alt.h = roi1->h;
            img1alt.bpp = img1->bpp;
            img1alt.data = fb_alloc0(image_size(&img1alt), FB_ALLOC_NO_HINT);
            imlib_logpolar_int(&img1alt, img1, roi1, false, false);
            roi1alt.x = 0;
            roi1alt.y = 0;
            roi1alt.w = roi1->w;
            roi1alt.h = roi1->h;
        }

        fft2d_controller_t fft0, fft1;

        fft2d_alloc(&fft0, logpolar ? &img0alt : &img0_fixed, logpolar ? &roi0alt : &roi0_fixed);
        fft2d_alloc(&fft1, logpolar ? &img1alt : img1, logpolar ? &roi1alt : roi1);

        fft2d_run(&fft0);
        fft2d_run(&fft1);

        int w = (1 << fft0.w_pow2);
        int h = (1 << fft0.h_pow2);

        for (int i = 0, j = h * w * 2; i < j; i += 2) {
            float ga_r = fft0.data[i+0];
            float ga_i = fft0.data[i+1];
            float gb_r = fft1.data[i+0];
            float gb_i = -fft1.data[i+1]; // complex conjugate...
            float hp_r = (ga_r * gb_r) - (ga_i * gb_i); // hadamard product
            float hp_i = (ga_r * gb_i) + (ga_i * gb_r); // hadamard product
            float mag = 1 / fast_sqrtf((hp_r*hp_r)+(hp_i*hp_i)); // magnitude
            fft0.data[i+0] = hp_r * mag;
            fft0.data[i+1] = hp_i * mag;
        }

        ifft2d_run(&fft0);

        float sum = 0;
        float max = 0;
        int off_x = 0;
        int off_y = 0;

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                // Note that the output of the FFT is packed with real data in both
                // the real and imaginary parts... (right side of the array is zero).
                float f_r = fft0.data[(i * w * 2) + j];
                sum += f_r;
                if (f_r > max) {
                    max = f_r;
                    off_x = j;
                    off_y = i;
                }
            }
        }

        *response = max / sum; // normalize this to [0:1].

        float f_sum = 0;
        float f_off_x = 0;
        float f_off_y = 0;

        for (int i = -2; i < 2; i++) {
            for (int j = -2; j < 2; j++) {

                // Wrap around
                int new_x = off_x + j;
                if (new_x < 0) new_x += w;
                if (new_x >= w) new_x -= w;

                // Wrap around
                int new_y = off_y + i;
                if (new_y < 0) new_y += h;
                if (new_y >= h) new_y -= h;

                // Compute centroid.
                float f_r = fft0.data[(new_y * w * 2) + new_x];
                f_off_x += (off_x + j) * f_r; // don't use new_x here
                f_off_y += (off_y + i) * f_r; // don't use new_y here
                f_sum += f_r;
            }
        }

        f_off_x /= f_sum;
        f_off_y /= f_sum;

        // FFT Shift X
        if (f_off_x >= (w/2.0f)) {
            *x_translation = f_off_x - w;
        } else {
            *x_translation = f_off_x;
        }

        // FFT Shift Y
        if (f_off_y >= (h/2.0f)) {
            *y_translation = -(f_off_y - h);
        } else {
            *y_translation = -f_off_y;
        }

        if ((*x_translation < (-w/2.0f))
        || ((w/2.0f) <= *x_translation)
        || (*y_translation < (-h/2.0f))
        || ((h/2.0f) <= *y_translation)
        || isnanf(*x_translation)
        || isinff(*x_translation)
        || isnanf(*y_translation)
        || isinff(*y_translation)
        || isnanf(*response)
        || isinff(*response)) { // Noise Filter
            *x_translation = 0;
            *y_translation = 0;
            *response = 0;
        }

        fft2d_dealloc(); // fft1
        fft2d_dealloc(); // fft0

        if (logpolar) {
            fb_free(); // img1alt
            fb_free(); // img0alt

            float w_2 = roi0->w / 2.0f;
            float h_2 = roi0->h / 2.0f;
            float rho_scale = fast_log(fast_sqrtf((w_2 * w_2) + (h_2 * h_2))) / roi0->h;
            float theta_scale = (2 * M_PI) / roi0->w;

            *rotation = *x_translation * theta_scale;
            *scale = (*y_translation * rho_scale) + 1;
            *x_translation = 0;
            *y_translation = 0;
        }
    }

    if ((!logpolar) && fix_rotation_scale) fb_free();
}
#endif //IMLIB_ENABLE_FIND_DISPLACEMENT
