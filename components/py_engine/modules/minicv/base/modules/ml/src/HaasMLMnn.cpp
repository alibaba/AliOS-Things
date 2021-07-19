
#include "base/modules/ml/include/HaasMLMnn.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"

#define LOG_TAG "HAAS_ML_MNN"

HaasMLMnn::HaasMLMnn()
{
    LOGD(LOG_TAG, "entern\n");
}

HaasMLMnn::~HaasMLMnn()
{
    LOGD(LOG_TAG, "entern\n");
}

int HaasMLMnn::SetInputData(const char* dataPath)
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLMnn::LoadNet(const char* modePath)
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLMnn::Predict()
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLMnn::GetPredictResponses(char* outResult, int len)
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLMnn::UnLoadNet()
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}
