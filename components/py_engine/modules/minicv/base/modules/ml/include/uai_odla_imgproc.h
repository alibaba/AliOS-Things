
#ifndef HAAS_UAI_IMG_PROC_H
#define HAAS_UAI_IMG_PROC_H
#include "stdio.h"
#include "uai_odla.h"
#include "uai_load.h"
#include "uai_quant.h"
#include "uai_image.h"
#include "aos/kernel.h"

#define RGB565_TO_R(pixel)  ((pixel & 0x1f) << 3)
#define RGB565_TO_G(pixel)  ((pixel & 0x7e0) >> 3)
#define RGB565_TO_B(pixel)  ((pixel & 0xf800) >> 8)

void uai_get_split_img_data(uint8_t *ori_img_data, uint32_t ori_img_width, uint32_t ori_img_height, uint8_t *img_data, int img_width, int img_height, int start_x, int start_y)
{
	uint8_t *temp_data;

	uai_img_data_t ori_img;
	ori_img.rows   = ori_img_height;
	ori_img.colums = ori_img_width;
	ori_img.format = UAI_RGB565;
	ori_img.data   = ori_img_data;

	uai_img_data_t new_img;
	new_img.rows   = img_height;
	new_img.colums = img_width;
	new_img.format = UAI_RGB565;
	new_img.data   = img_data;

	uai_image_split(&ori_img, &new_img, start_x, start_y);
}

void uai_get_resize_img(uint8_t *img_data, int img_width, int img_height, uint8_t *resized_img, int resize_width, int resize_height)
{
    uai_img_data_t new_img;
	new_img.rows   = img_height;
	new_img.colums = img_width;
	new_img.format = UAI_GRAY;
	new_img.data   = img_data;

    uai_img_data_t resize_img;
	resize_img.rows   = resize_height;
	resize_img.colums = resize_width;
	resize_img.format = UAI_GRAY;
	resize_img.data   = resized_img;

	uai_img_resize(&new_img, &resize_img, UAI_IMG_INTER_BILINEAR);
}

void uai_convert_img(uint8_t *ori_img_data, int img_width, int img_height, uint8_t *new_img_data)
{
    uai_img_data_t old_img;
	old_img.rows   = img_height;
	old_img.colums = img_width;
	old_img.format = UAI_RGB565;
	old_img.data   = ori_img_data;

    uai_img_data_t new_img;
	new_img.rows   = img_height;
	new_img.colums = img_width;
	new_img.format = UAI_RGB888;
	new_img.data   = new_img_data;

	uai_image_convert_ram(&old_img, &new_img);
}

void uai_get_gray_img(uint8_t *ori_img_data, int img_width, int img_height, uint8_t *new_img_data)
{
	uai_img_data_t old_img;
	old_img.rows = img_height;
	old_img.colums = img_width;
	old_img.format = UAI_RGB565;
	old_img.data = ori_img_data;

	uai_img_data_t new_img;
	new_img.rows = img_height;
	new_img.colums = img_width;
	new_img.format = UAI_GRAY;
	new_img.data = new_img_data;

	uai_image_gray(&old_img, &new_img);
}
#endif
