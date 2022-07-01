/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Debayering Functions
 */
#include "imlib.h"

void imlib_debayer_line_to_binary(int x_start, int x_end, int y_row, uint32_t *dst_row_ptr, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    int y_row_odd = y_row & 1;
    int y = (y_row / 2) * 2;
    uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

    // keep row pointers in bounds
    if (y == 0) {
        rowptr_bgbg_1 = src->data;
        rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
        rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
        rowptr_grgr_0 = rowptr_grgr_2;
    } else if (y == h_limit_m_1) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else if (y >= h_limit) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_grgr_0;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else { // get 4 neighboring rows
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
    }

    // If the image is an odd width this will go for the last loop and we drop the last column.
    if (!y_row_odd) { // even
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int y0 = ((r_pixels_0 * 38) + (g_pixels_0 * 75) + (b_pixels_0 * 15)) >> 7;
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, i, (y0 >> 7) & 0x1);

            if (x != w_limit) {
                IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, i + 1, (y0 >> 23) & 0x1);
            }
        }
    } else { // odd
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            int r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            int g1 = (row_bgbg_1 >> 16) & 0xFF;
            int g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            int b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int y1 = ((r_pixels_1 * 38) + (g_pixels_1 * 75) + (b_pixels_1 * 15)) >> 7;
            IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, i, (y1 >> 7) & 0x1);

            if (x != w_limit) {
                IMAGE_PUT_BINARY_PIXEL_FAST(dst_row_ptr, i + 1, (y1 >> 23) & 0x1);
            }
        }
    }
}

// Does no bounds checking on the destination.
void imlib_debayer_image_to_binary(image_t *dst, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    // If the image is an odd height this will go for the last loop and we drop the last row.
    for (int y = 0; y < src_h; y += 2) {
        uint32_t *row_ptr_e = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, y);
        uint32_t *row_ptr_o = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(dst, y + 1);
        uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

        // keep row pointers in bounds
        if (y == 0) {
            rowptr_bgbg_1 = src->data;
            rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
            rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
            rowptr_grgr_0 = rowptr_grgr_2;
        } else if (y == h_limit_m_1) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else if (y >= h_limit) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_grgr_0;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else { // get 4 neighboring rows
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
        }

        // If the image is an odd width this will go for the last loop and we drop the last column.
        for (int x = 0; x < src_w; x += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int y0 = ((r_pixels_0 * 38) + (g_pixels_0 * 75) + (b_pixels_0 * 15)) >> 7;
            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr_e, x, (y0 >> 7) & 0x1);

            if (x != w_limit) {
                IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr_e, x + 1, (y0 >> 23) & 0x1);
            }

            if (y == h_limit) {
                break;
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            g1 = (row_bgbg_1 >> 16) & 0xFF;
            g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int y1 = ((r_pixels_1 * 38) + (g_pixels_1 * 75) + (b_pixels_1 * 15)) >> 7;
            IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr_o, x, (y1 >> 7) & 0x1);

            if (x != w_limit) {
                IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr_o, x + 1, (y1 >> 23) & 0x1);
            }
        }
    }
}

void imlib_debayer_line_to_grayscale(int x_start, int x_end, int y_row, uint8_t *dst_row_ptr, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    int y_row_odd = y_row & 1;
    int y = (y_row / 2) * 2;
    uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

    // keep row pointers in bounds
    if (y == 0) {
        rowptr_bgbg_1 = src->data;
        rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
        rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
        rowptr_grgr_0 = rowptr_grgr_2;
    } else if (y == h_limit_m_1) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else if (y >= h_limit) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_grgr_0;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else { // get 4 neighboring rows
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
    }

    // If the image is an odd width this will go for the last loop and we drop the last column.
    if (!y_row_odd) { // even
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int y0 = ((r_pixels_0 * 38) + (g_pixels_0 * 75) + (b_pixels_0 * 15)) >> 7;
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, i, y0);

            if (x != w_limit) {
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, i + 1, y0 >> 16);
            }
        }
    } else { // odd
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            int r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            int g1 = (row_bgbg_1 >> 16) & 0xFF;
            int g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            int b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int y1 = ((r_pixels_1 * 38) + (g_pixels_1 * 75) + (b_pixels_1 * 15)) >> 7;
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, i, y1);

            if (x != w_limit) {
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(dst_row_ptr, i + 1, y1 >> 16);
            }
        }
    }
}

// Does no bounds checking on the destination.
void imlib_debayer_image_to_grayscale(image_t *dst, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    // If the image is an odd height this will go for the last loop and we drop the last row.
    for (int y = 0; y < src_h; y += 2) {
        uint8_t *row_ptr_e = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, y);
        uint8_t *row_ptr_o = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst, y + 1);
        uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

        // keep row pointers in bounds
        if (y == 0) {
            rowptr_bgbg_1 = src->data;
            rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
            rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
            rowptr_grgr_0 = rowptr_grgr_2;
        } else if (y == h_limit_m_1) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else if (y >= h_limit) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_grgr_0;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else { // get 4 neighboring rows
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
        }

        // If the image is an odd width this will go for the last loop and we drop the last column.
        for (int x = 0; x < src_w; x += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int y0 = ((r_pixels_0 * 38) + (g_pixels_0 * 75) + (b_pixels_0 * 15)) >> 7;
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr_e, x, y0);

            if (x != w_limit) {
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr_e, x + 1, y0 >> 16);
            }

            if (y == h_limit) {
                break;
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            g1 = (row_bgbg_1 >> 16) & 0xFF;
            g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int y1 = ((r_pixels_1 * 38) + (g_pixels_1 * 75) + (b_pixels_1 * 15)) >> 7;
            IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr_o, x, y1);

            if (x != w_limit) {
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr_o, x + 1, y1 >> 16);
            }
        }
    }
}

void imlib_debayer_line_to_rgb565(int x_start, int x_end, int y_row, uint16_t *dst_row_ptr, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    int y_row_odd = y_row & 1;
    int y = (y_row / 2) * 2;
    uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

    // keep row pointers in bounds
    if (y == 0) {
        rowptr_bgbg_1 = src->data;
        rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
        rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
        rowptr_grgr_0 = rowptr_grgr_2;
    } else if (y == h_limit_m_1) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else if (y >= h_limit) {
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_grgr_0;
        rowptr_bgbg_3 = rowptr_bgbg_1;
    } else { // get 4 neighboring rows
        rowptr_grgr_0 = src->data + ((y - 1) * src_w);
        rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
        rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
        rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
    }

    // If the image is an odd width this will go for the last loop and we drop the last column.
    if (!y_row_odd) { // even
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int rgb565_0 = ((r_pixels_0 << 8) & 0xf800f800) |
                           ((g_pixels_0 << 3) & 0x07e007e0) |
                         ((b_pixels_0 >> 3) & 0x001f001f);

            if (x == w_limit) { // just put bottom
                IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, i, rgb565_0);
            } else { // put both
                *((uint32_t *) (dst_row_ptr + i)) = rgb565_0;
            }
        }
    } else { // odd
        for (int x = x_start, i = 0; x < x_end; x += 2, i += 2) {
            uint32_t row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            int r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            int g1 = (row_bgbg_1 >> 16) & 0xFF;
            int g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            int b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int rgb565_1 = ((r_pixels_1 << 8) & 0xf800f800) |
                           ((g_pixels_1 << 3) & 0x07e007e0) |
                           ((b_pixels_1 >> 3) & 0x001f001f);

            if (x == w_limit) { // just put bottom
                IMAGE_PUT_RGB565_PIXEL_FAST(dst_row_ptr, i, rgb565_1);
            } else { // put both
                *((uint32_t *) (dst_row_ptr + i)) = rgb565_1;
            }
        }
    }
}

// Does no bounds checking on the destination.
void imlib_debayer_image_to_rgb565(image_t *dst, image_t *src)
{
    int src_w = src->w, w_limit = src_w - 1, w_limit_m_1 = w_limit - 1;
    int src_h = src->h, h_limit = src_h - 1, h_limit_m_1 = h_limit - 1;

    // If the image is an odd height this will go for the last loop and we drop the last row.
    for (int y = 0; y < src_h; y += 2) {
        uint16_t *row_ptr_e = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, y);
        uint16_t *row_ptr_o = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst, y + 1);
        uint8_t *rowptr_grgr_0, *rowptr_bgbg_1, *rowptr_grgr_2, *rowptr_bgbg_3;

        // keep row pointers in bounds
        if (y == 0) {
            rowptr_bgbg_1 = src->data;
            rowptr_grgr_2 = rowptr_bgbg_1 + ((src_h >= 2) ? src_w : 0);
            rowptr_bgbg_3 = rowptr_bgbg_1 + ((src_h >= 3) ? (src_w * 2) : 0);
            rowptr_grgr_0 = rowptr_grgr_2;
        } else if (y == h_limit_m_1) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else if (y >= h_limit) {
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_grgr_0;
            rowptr_bgbg_3 = rowptr_bgbg_1;
        } else { // get 4 neighboring rows
            rowptr_grgr_0 = src->data + ((y - 1) * src_w);
            rowptr_bgbg_1 = rowptr_grgr_0 + src_w;
            rowptr_grgr_2 = rowptr_bgbg_1 + src_w;
            rowptr_bgbg_3 = rowptr_grgr_2 + src_w;
        }

        // If the image is an odd width this will go for the last loop and we drop the last column.
        for (int x = 0; x < src_w; x += 2) {
            uint32_t row_grgr_0, row_bgbg_1, row_grgr_2, row_bgbg_3;

            // keep pixels in bounds
            if (x == 0) {
                if (src_w >= 4) {
                    row_grgr_0 = *((uint32_t *) rowptr_grgr_0);
                    row_bgbg_1 = *((uint32_t *) rowptr_bgbg_1);
                    row_grgr_2 = *((uint32_t *) rowptr_grgr_2);
                    row_bgbg_3 = *((uint32_t *) rowptr_bgbg_3);
                } else if (src_w >= 3) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0) | (*(rowptr_grgr_0 + 2) << 16);
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1) | (*(rowptr_bgbg_1 + 2) << 16);
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2) | (*(rowptr_grgr_2 + 2) << 16);
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3) | (*(rowptr_bgbg_3 + 2) << 16);
                } else if (src_w >= 2) {
                    row_grgr_0 = *((uint16_t *) rowptr_grgr_0);
                    row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                    row_bgbg_1 = *((uint16_t *) rowptr_bgbg_1);
                    row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                    row_grgr_2 = *((uint16_t *) rowptr_grgr_2);
                    row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                    row_bgbg_3 = *((uint16_t *) rowptr_bgbg_3);
                    row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
                } else {
                    row_grgr_0 = *(rowptr_grgr_0) * 0x01010101;
                    row_bgbg_1 = *(rowptr_bgbg_1) * 0x01010101;
                    row_grgr_2 = *(rowptr_grgr_2) * 0x01010101;
                    row_bgbg_3 = *(rowptr_bgbg_3) * 0x01010101;
                }
                // The starting point needs to be offset by 1. The below patterns are actually
                // rgrg, gbgb, rgrg, and gbgb. So, shift left and backfill the missing border pixel.
                row_grgr_0 = (row_grgr_0 << 8) | __UXTB_RORn(row_grgr_0, 8);
                row_bgbg_1 = (row_bgbg_1 << 8) | __UXTB_RORn(row_bgbg_1, 8);
                row_grgr_2 = (row_grgr_2 << 8) | __UXTB_RORn(row_grgr_2, 8);
                row_bgbg_3 = (row_bgbg_3 << 8) | __UXTB_RORn(row_bgbg_3, 8);
            } else if (x == w_limit_m_1) {
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 2));
                row_grgr_0 = (row_grgr_0 >> 8) | ((row_grgr_0 << 8) & 0xff000000);
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 2));
                row_bgbg_1 = (row_bgbg_1 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 2));
                row_grgr_2 = (row_grgr_2 >> 8) | ((row_grgr_2 << 8) & 0xff000000);
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 2));
                row_bgbg_3 = (row_bgbg_3 >> 8) | ((row_bgbg_1 << 8) & 0xff000000);
            } else if (x >= w_limit) {
                row_grgr_0 = *((uint16_t *) (rowptr_grgr_0 + x - 1));
                row_grgr_0 = (row_grgr_0 << 16) | row_grgr_0;
                row_bgbg_1 = *((uint16_t *) (rowptr_bgbg_1 + x - 1));
                row_bgbg_1 = (row_bgbg_1 << 16) | row_bgbg_1;
                row_grgr_2 = *((uint16_t *) (rowptr_grgr_2 + x - 1));
                row_grgr_2 = (row_grgr_2 << 16) | row_grgr_2;
                row_bgbg_3 = *((uint16_t *) (rowptr_bgbg_3 + x - 1));
                row_bgbg_3 = (row_bgbg_3 << 16) | row_bgbg_3;
            } else { // get 4 neighboring rows
                row_grgr_0 = *((uint32_t *) (rowptr_grgr_0 + x - 1));
                row_bgbg_1 = *((uint32_t *) (rowptr_bgbg_1 + x - 1));
                row_grgr_2 = *((uint32_t *) (rowptr_grgr_2 + x - 1));
                row_bgbg_3 = *((uint32_t *) (rowptr_bgbg_3 + x - 1));
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_01 = __UHADD8(row_grgr_0, row_grgr_2);
            int row_1g = __UHADD8(row_bgbg_1, __PKHTB(row_bgbg_1, row_bgbg_1, 16));

            int r_pixels_0 = __UXTB16(__UHADD8(row_01, __PKHTB(row_01, row_01, 16)));
            int g_pixels_0 = __UXTB16(__UHADD8(row_1g, __PKHTB(row_1g, row_01, 8)));
            int b_pixels_0 = __UXTB16_RORn(__UHADD8(row_bgbg_1, __PKHBT(row_bgbg_1, row_bgbg_1, 16)), 8);
            #else

            int r0 = ((row_grgr_0 & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r2 = (((row_grgr_0 >> 16) & 0xFF) + ((row_grgr_2 >> 16) & 0xFF)) >> 1;
            int r_pixels_0 = (r2 << 16) | ((r0 + r2) >> 1);

            int g0 = (row_grgr_0 >> 8) & 0xFF;
            int g1 = (((row_bgbg_1 >> 16) & 0xFF) + (row_bgbg_1 & 0xFF)) >> 1;
            int g2 = (row_grgr_2 >> 8) & 0xFF;
            int g_pixels_0 = (row_bgbg_1 & 0xFF0000) | ((((g0 + g2) >> 1) + g1) >> 1);

            int b1 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_1 >> 8) & 0xFF)) >> 1;
            int b_pixels_0 = (b1 << 16) | ((row_bgbg_1 >> 8) & 0xFF);

            #endif

            int rgb565_0 = ((r_pixels_0 << 8) & 0xf800f800) |
                           ((g_pixels_0 << 3) & 0x07e007e0) |
                           ((b_pixels_0 >> 3) & 0x001f001f);

            if (x == w_limit) { // just put bottom
                IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr_e, x, rgb565_0);
            } else { // put both
                *((uint32_t *) (row_ptr_e + x)) = rgb565_0;
            }

            if (y == h_limit) {
                break;
            }

            #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
            int row_13 = __UHADD8(row_bgbg_1, row_bgbg_3);
            int row_2g = __UHADD8(row_grgr_2, __PKHBT(row_grgr_2, row_grgr_2, 16));

            int r_pixels_1 = __UXTB16(__UHADD8(row_grgr_2, __PKHTB(row_grgr_2, row_grgr_2, 16)));
            int g_pixels_1 = __UXTB16_RORn(__UHADD8(row_2g, __PKHBT(row_2g, row_13, 8)), 8);
            int b_pixels_1 = __UXTB16_RORn(__UHADD8(row_13, __PKHBT(row_13, row_13, 16)), 8);
            #else

            r2 = (((row_grgr_2 >> 16) & 0xFF) + (row_grgr_2 & 0xFF)) >> 1;
            int r_pixels_1 = (row_grgr_2 & 0xFF0000) | r2;

            g1 = (row_bgbg_1 >> 16) & 0xFF;
            g2 = (((row_grgr_2 >> 24) & 0xFF) + ((row_grgr_2 >> 8) & 0xFF)) >> 1;
            int g3 = (row_bgbg_3 >> 16) & 0xFF;
            int g_pixels_1 = (((((g1 + g3) >> 1) + g2) >> 1) << 16) | ((row_grgr_2 >> 8) & 0xFF);

            b1 = (((row_bgbg_1 >> 8) & 0xFF) + ((row_bgbg_3 >> 8) & 0xFF)) >> 1;
            int b3 = (((row_bgbg_1 >> 24) & 0xFF) + ((row_bgbg_3 >> 24) & 0xFF)) >> 1;
            int b_pixels_1 = (((b1 + b3) >> 1) << 16) | b1;

            #endif

            int rgb565_1 = ((r_pixels_1 << 8) & 0xf800f800) |
                           ((g_pixels_1 << 3) & 0x07e007e0) |
                           ((b_pixels_1 >> 3) & 0x001f001f);

            if (x == w_limit) { // just put bottom
                IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr_o, x, rgb565_1);
            } else { // put both
                *((uint32_t *) (row_ptr_o + x)) = rgb565_1;
            }
        }
    }
}
