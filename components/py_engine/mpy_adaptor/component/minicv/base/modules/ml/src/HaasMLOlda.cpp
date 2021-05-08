
#include "HaasMLOlda.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "uai_quant.h"
#include "uai_scale_data.h"
#include "uai_img_data.h"
#include "uai_odla_inference.h"
#include "uai_odla_imgproc.h"
#include "HaasImageCodecPng.h"

HaasMLOlda::HaasMLOlda()
    : input_scale(0),
      input_shift(0),
      norm_img_data(NULL)
{
    LOG_D("entern\n");
    norm_img_data = (uint8_t *)malloc(DEC_IMG_WIDTH * DEC_IMG_HEIGHT);
    memset(norm_img_data, 0, DEC_IMG_WIDTH * DEC_IMG_HEIGHT);
    memset(out_output_Softmax, 0, 21);
}

HaasMLOlda::~HaasMLOlda()
{
    LOG_D("entern\n");
    free(norm_img_data);
    norm_img_data = NULL;
}

int HaasMLOlda::SetInputData(const char* dataPath)
{
    LOG_D("entern path = %s;\n", dataPath);

    HaasImageCodecPng * mHaasImageCodecPng = new HaasImageCodecPng();
    ImageBuffer_t *image = mHaasImageCodecPng->ImgDecode(dataPath);
    if (image == NULL)
    {
        LOG_D("mHaasImageCodecBmp->ImgDecode failed\n");
    }
    uint8_t *gray_img = NULL;
    LOG_D("bitmap pixels = %p;width = %d; height = %d;\n", image->address[0], image->width, image->height);
    gray_img = (uint8_t*)malloc(image->width * image->height);
    memset(gray_img, 0, image->width * image->height);

    uai_get_gray_img((uint8_t*)image->address[0], image->width, image->height, gray_img);
    uai_get_resize_img(gray_img, image->width, image->height, norm_img_data, DEC_IMG_WIDTH, DEC_IMG_HEIGHT);
    free(gray_img);

    free(image->address[0]);
    free(image);
    LOG_D("call delete mHaasImageCodecBmp\n");
    delete mHaasImageCodecPng;
    return STATUS_OK;
}

int HaasMLOlda::LoadNet(const char* modePath)
{
    LOG_D("entern\n");
    char data_src[128];
    uai_model_quant_scale_data_t *model_scale = NULL;

    memset(data_src, 0, 128);
    snprintf(data_src, 128, "mem:%u,size:%u", uai_scale_data, uai_scale_data_len);
    uai_load_model_scale_config(data_src);
    model_scale = uai_load_model_scale();

    input_scale = model_scale->head.input_scale;
    input_shift = model_scale->head.input_shift;

    return STATUS_OK;
}

int HaasMLOlda::Predict()
{
    LOG_D("entern\n");
    uai_quant_normalize_uint8(norm_img_data, DEC_IMG_WIDTH * DEC_IMG_HEIGHT, input_scale,     input_shift);

    alios_b0((const int8_t*)norm_img_data, out_output_Softmax);
    return STATUS_OK;
}

int HaasMLOlda::GetPredictResponses(char* outResult, int len)
{
    LOG_D("entern\n");
    if (outResult == NULL || len < 3)
    {
        LOG_E("par is illegal outResult = %p; len = %d;", outResult, len);
        return STATUS_ERROR;
    }
    memset(outResult, 0, len);
    uai_get_img_dec_result(out_output_Softmax, 21, outResult);
    LOG_E("AI RECOGNIZE number is: %s\n\n", outResult);
    LOG_D("score: \n");
    for(int i = 0; i < 21; i++) {
        LOG_D("[%d]: %d\n", i, out_output_Softmax[i]);
    }
    return STATUS_OK;
}

int HaasMLOlda::UnLoadNet()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLOlda::uai_get_img_dec_result(int8_t output_softmax[], int result_num, char *result)
{
	int max_id = 0;
	int i = 0;

	for (int i = 0; i < result_num; i++) {
		if (output_softmax[i] > output_softmax[max_id]) {
			max_id = i;
		}
	}

	if (output_softmax[max_id] != 0) {
		if (max_id <= 9) {
			result[0] = 0x30 + max_id;
			result[1] = 0;
		}
		else {
			result[0] = 0x30 + max_id - 10;
			result[1] = '.';
			result[2] = 0;
		}
	}

	return 0;
}
