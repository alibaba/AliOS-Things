/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef UAI_IMAGE_H
#define UAI_IMAGE_H

#include <stdint.h>
#include "uai_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UAI_IMG_INTER_NEAREST,
    UAI_IMG_INTER_BILINEAR,
    UAI_IMG_INTER_AREA,
    UAI_IMG_INTER_CUBIC
}uai_img_inter_e;

typedef enum {
    UAI_YUV444,
    UAI_YUV422,
    UAI_YUV420,
    UAI_YUV411,
    UAI_RGB888,
    UAI_RGB565,
    UAI_RGBx555,
    UAI_RGB555x,
    UAI_RGBx444,
    UAI_RGB444x,
    UAI_GRAY,
} uai_img_format_e;

typedef struct {
    uint8_t r_data;
    uint8_t g_data;
    uint8_t b_data;
}uai_img_rgb888_data_t;

typedef struct {
    uint8_t r_data:5;
    uint8_t g_data:6;
    uint8_t b_data:5;
}uai_img_rgb565_data_t;

typedef struct {
    uint32_t rows;
    uint32_t colums;
    uai_img_format_e format;
    uint8_t *data;
}uai_img_data_t;

int uai_img_resize(uai_img_data_t *img_src, uai_img_data_t *img_dst, uai_img_inter_e inter_methord);
int uai_image_split(uai_img_data_t *img_src, uai_img_data_t *img_dst, int x_start, int y_start);
int uai_image_convert_bmpfile(uai_img_data_t *img_src, uai_img_data_t *img_dst);
int uai_image_convert_ram(uai_img_data_t *img_src, uai_img_data_t *img_dst);
int uai_image_gray(uai_img_data_t *img_src, uai_img_data_t *img_dst);

#ifdef __cplusplus
}
#endif

#endif /* end of UAI_IMAGE_H */