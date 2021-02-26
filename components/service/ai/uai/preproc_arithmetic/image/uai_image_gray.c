/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "uai_image.h"

#define RGB565_TO_B(pixel)  ((pixel & 0x1f) << 3)
#define RGB565_TO_G(pixel)  ((pixel & 0x7e0) >> 3)
#define RGB565_TO_R(pixel)  ((pixel & 0xf800) >> 8)

int uai_image_gray(uai_img_data_t *img_src, uai_img_data_t *img_dst)
{
	if (img_src->format != UAI_RGB565) {
		return -1;   /* not support other format now */
	}

	int x = 0;
	int y = 0;
	int k = 0;
	uint16_t *buffer = (uint16_t *)img_src->data;
	uint32_t index;

	for (x = 0; x < img_dst->rows; x++) {
		index = x * img_dst->colums;
		for (y = 0; y < img_dst->colums; y++) {
			uint32_t tmp_data = (uint32_t)((buffer[index + y] >> 8) & 0xff) | ((buffer[index + y] << 8) & 0xff00);
			img_dst->data[(x * img_dst->colums + y)] = (uint8_t)((1140 * RGB565_TO_B(tmp_data) + 5870 * RGB565_TO_G(tmp_data) + 2989 * RGB565_TO_R(tmp_data))/10000);
		}
	}
}