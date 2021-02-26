#include "uai_image.h"
#include "uai_common.h"

int uai_image_split(uai_img_data_t *img_src, uai_img_data_t *img_dst, int x_start, int y_start)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int pix_bytes = 0;

	if(img_src->format != img_dst->format) {
		return UAI_FAIL;
	}

	switch(img_src->format) {
		case UAI_RGB888 :
			pix_bytes = 3;
			break;
		case UAI_RGB565:
			pix_bytes = 2;
			break;
		default:
			UAI_LOGE("image format not supported\n", img_src->format);
			return UAI_FAIL;
	}

	if (img_dst->rows + y_start > img_src->rows) {
		return UAI_FAIL;
	}
	if (img_dst->colums + x_start > img_src->colums) {
		return UAI_FAIL;
	}

	int dst_linesize = pix_bytes * img_dst->colums;
	int src_linesize = pix_bytes * img_src->colums;

	for (i = 0; i < img_dst->rows; i++) {
		for (j = 0; j < img_dst->colums; j++) {
			for (k = 0; k < pix_bytes; k ++) {
				img_dst->data[i*dst_linesize + j * pix_bytes + k] = img_src->data[(i + y_start) * src_linesize + (j + x_start) * pix_bytes + k];
			}
		}
	}
	return UAI_SUCCESS;
}