
#include "WrapperIHaasML.h"
#include "IHaasML.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasML.h"

void* MLCreateInstance(MLEngineType_t type)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML =  IHaasMLInstance(type);
    if (mIHaasML == NULL)
    {
        LOG_D("IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasML;
}

void MLDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasML;
}

int MLConfig(void* instance, char *key, char *secret, char *region_id,
        char *endpoint, char *url)
{
    LOG_D("key = %s;\n", key);
    LOG_D("secret = %s;\n", secret);
    LOG_D("region_id = %s;\n", region_id);
    LOG_D("endpoint = %s;\n", endpoint);
    LOG_D("url = %s;\n", url);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->Config(key, secret, region_id, endpoint, url);
    return ret;
}

int MLSetInputData(void* instance, const char* dataPath)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->SetInputData(dataPath);
    return ret;
}

int MLLoadNet(void* instance, const char* modePath)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->LoadNet(modePath);
    return ret;
}

int MLPredict(void* instance)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->Predict();
    return ret;
}

int MLGetPredictResponses(void* instance, char* outResult, int len)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->GetPredictResponses(outResult, len);
    LOG_D("out %s; ret = %d;\n", __func__, ret);
    return ret;
}

int MLUnLoadNet(void* instance)
{
    LOG_D("entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOG_D("mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->UnLoadNet();
    return ret;
}

