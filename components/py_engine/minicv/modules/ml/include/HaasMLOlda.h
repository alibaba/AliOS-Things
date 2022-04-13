#ifndef HAAS_ML_OLDA_H
#define HAAS_ML_OLDA_H
#include "IHaasML.h"

class HaasMLOlda : public IHaasML
{
public:
	HaasMLOlda();
	virtual ~HaasMLOlda();

	virtual int SetInputData(const char* dataPath);
	virtual int LoadNet(const char* modePath);
	virtual int Predict();
	virtual int GetPredictResponses(char* outResult, int len);
	virtual int UnLoadNet();
private:
    int uai_get_img_dec_result(int8_t output_softmax[], int result_num, char *result);
    uint32_t  input_scale;
    uint32_t  input_shift;
    uint8_t *norm_img_data;
    int8_t out_output_Softmax[21];
};
#endif
