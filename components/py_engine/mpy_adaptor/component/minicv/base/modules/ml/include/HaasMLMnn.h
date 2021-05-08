#ifndef HAAS_ML_MNN_H
#define HAAS_ML_MNN_H
#include "IHaasML.h"

class HaasMLMnn : public IHaasML
{
public:
	HaasMLMnn();
	virtual ~HaasMLMnn();

	virtual int SetInputData(const char* dataPath);
	virtual int LoadNet(const char* modePath);
	virtual int Predict();
	virtual int GetPredictResponses(char* outResult, int len);
	virtual int UnLoadNet();
};
#endif
