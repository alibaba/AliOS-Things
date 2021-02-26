/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "uai_image.h"
#include "uai_common.h"

#define RGB565_TO_R(pixel)  ((pixel & 0x1f) << 3)
#define RGB565_TO_G(pixel)  ((pixel & 0x7e0) >> 3)
#define RGB565_TO_B(pixel)  ((pixel & 0xf800) >> 8)

int uai_image_convert_bmpfile(uai_img_data_t *img_src, uai_img_data_t *img_dst)
{
	int x = 0;
	int y = 0;
	int k = 0;
	uint16_t *buffer = (uint16_t *)img_src->data;
	uint32_t index;

	if ((img_src->format != UAI_RGB565) || (img_dst->format != UAI_RGB888)) {
		return UAI_FAIL;
	}

	if ((img_dst->rows != img_src->rows) || (img_dst->colums != img_src->colums)) {
		return UAI_FAIL;
	}

	for (x = 0; x < img_dst->rows; x++) {
		index = (img_dst->rows - 1 - x) * img_dst->colums;
		for (y = 0; y < img_dst->colums; y++) {
			uint16_t tmp_data = ((buffer[index + y] >> 8) & 0xff) | ((buffer[index + y] << 8) & 0xff00);
			img_dst->data[(x * img_dst->colums + y) * 3] =     RGB565_TO_R(tmp_data);
			img_dst->data[(x * img_dst->colums + y) * 3 + 1] = RGB565_TO_G(tmp_data);
			img_dst->data[(x * img_dst->colums + y) * 3 + 2] = RGB565_TO_B(tmp_data);
		}
	}
}

int uai_image_convert_ram(uai_img_data_t *img_src, uai_img_data_t *img_dst)
{
	int x = 0;
	int y = 0;
	int k = 0;
	uint16_t *buffer = (uint16_t *)img_src->data;
	uint32_t index;

	if ((img_src->format != UAI_RGB565) || (img_dst->format != UAI_RGB888)) {
		return UAI_FAIL;
	}

	if ((img_dst->rows != img_src->rows) || (img_dst->colums != img_src->colums)) {
		return UAI_FAIL;
	}

	for (x = 0; x < img_dst->rows; x++) {
		index = x * img_dst->colums;
		for (y = 0; y < img_dst->colums; y++) {
			uint16_t tmp_data = ((buffer[index + y] >> 8) & 0xff) | ((buffer[index + y] << 8) & 0xff00);
			img_dst->data[(x * img_dst->colums + y) * 3] = RGB565_TO_B(tmp_data);
			img_dst->data[(x * img_dst->colums + y) * 3 + 1] = RGB565_TO_G(tmp_data);
			img_dst->data[(x * img_dst->colums + y) * 3 + 2] =  RGB565_TO_R(tmp_data);
		}
	}
}