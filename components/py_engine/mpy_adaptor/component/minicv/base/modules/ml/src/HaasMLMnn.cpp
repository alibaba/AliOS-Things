
#include "HaasMLMnn.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasMLMnn::HaasMLMnn()
{
    LOG_D("entern\n");
}

HaasMLMnn::~HaasMLMnn()
{
    LOG_D("entern\n");
}

int HaasMLMnn::SetInputData(const char* dataPath)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLMnn::LoadNet(const char* modePath)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLMnn::Predict()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLMnn::GetPredictResponses(char* outResult, int len)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLMnn::UnLoadNet()
{
    LOG_D("entern\n");
    return STATUS_OK;
}
