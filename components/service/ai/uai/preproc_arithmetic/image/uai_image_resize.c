/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "uai_image.h"
#include "uai_common.h"
#include "math.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define FLOOR(v) ((v) < 0.0 ? ((int)floor(v)) : ((int)(v)))

static int uai_img_resize_nearest(uai_img_data_t *img_src, uai_img_data_t *img_dst)
{
    int i = 0;
    int j = 0;
	int k = 0;
	int pix_bytes  = 0;
    double scale_x = (double)img_src->colums / img_dst->colums;
    double scale_y = (double)img_src->rows   / img_dst->rows;

	switch(img_src->format) {
		case UAI_RGB888 :
			pix_bytes = 3;
			break;
		case UAI_RGB565:
			pix_bytes = 2;
			break;
		case UAI_GRAY:
			pix_bytes = 1;
			break;
		default:
			UAI_LOGE("image format %d not supported\n", img_src->format);
			return UAI_FAIL;
	}

	int dst_linesize = pix_bytes * img_dst->colums;
	int src_linesize = pix_bytes * img_src->colums;

    for (i = 0; i < img_dst->colums; i++) {
        int sx = FLOOR(i * scale_x);
        sx = min(sx, img_dst->colums - 1);
        for (j = 0; j < img_dst->rows; j++) {
            int sy = FLOOR(j * scale_y);
            sy = min(sy, img_dst->rows - 1);
			for (k = 0; k < pix_bytes; k ++) {
            	img_dst->data[i * dst_linesize + j * img_dst->colums + k] = img_src->data[sx * src_linesize + sy * img_src->colums + k];
			}
        }
    }
	return UAI_SUCCESS;
}

short saturate_cast_short(float value)
{
	int32_t n = (int32_t)value;
	if (n < 0) n = 0;
	if (n > 65535) n = 65535;
	return (short)n;
}

static int uai_img_resize_bilinear(uai_img_data_t *img_src, uai_img_data_t *img_dst)
{
	int  pix_bytes = 0;
	double scale_x = (double)img_src->colums / img_dst->colums;
	double scale_y = (double)img_src->rows / img_dst->rows;

	switch(img_src->format) {
		case UAI_RGB888 :
			pix_bytes = 3;
			break;
		case UAI_RGB565:
			pix_bytes = 2;
			break;
		case UAI_GRAY:
			pix_bytes = 1;
			break;
		default:
			UAI_LOGE("image format %d not supported\n", img_src->format);
			return UAI_FAIL;
	}

	int bit_depth = pix_bytes * 8;
	int dst_linesize = pix_bytes * img_dst->colums;
	int src_linesize = pix_bytes * img_src->colums;

	uint8_t *dataDst = img_dst->data;
	uint8_t *dataSrc = img_src->data;

	for (int i = 0; i < img_dst->rows; i++)
	{
		int sy = (int)(scale_y * i);
		int sy1 = min(sy + 1, img_src->rows - 1);
		float fv = (float)(scale_y * i - sy);

		for (int j = 0; j < img_dst->colums; j++)
		{
			int sx = (int)(scale_x * j);
			int sx1 = min(sx + 1, img_src->colums - 1);
			float v = (float)(scale_x * j - sx);

			for (int k = 0; k < pix_bytes; k++)
			{
				dataDst[i * dst_linesize + j * pix_bytes + k] =
					(1 - fv)*(1 - v) * dataSrc[sy * src_linesize + sx * pix_bytes + k] +
					(1 - fv)*v*dataSrc[sy1 * src_linesize + sx * pix_bytes + k] +
					fv * (1 - v) * dataSrc[sy * src_linesize + sx1 * pix_bytes + k] +
					fv * v * dataSrc[sy1 * src_linesize + sx1 * pix_bytes + k];
			}
		}
	}

	return UAI_SUCCESS;
}

int uai_img_resize(uai_img_data_t *img_src, uai_img_data_t *img_dst,  uai_img_inter_e inter_methord)
{
    switch (inter_methord) {
        case UAI_IMG_INTER_NEAREST:
            return uai_img_resize_nearest(img_src, img_dst);

        case UAI_IMG_INTER_BILINEAR:
            return uai_img_resize_bilinear(img_src, img_dst);

        case UAI_IMG_INTER_AREA:
            //return uai_img_resize_area(img_src, img_dst);

        case UAI_IMG_INTER_CUBIC:
            //return uai_img_resize_cubic(img_src, img_dst);

        default:
            return UAI_FAIL;
    }

    return UAI_FAIL;
}