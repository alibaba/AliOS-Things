
#include "base/modules/c/include/WrapperIHaasML.h"
#include "base/include/IHaasML.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"

#define LOG_TAG "WRAPPER_ML"

void* MLCreateInstance(MLEngineType_t type)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML* mIHaasML =  IHaasMLInstance(type);
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasML;
}

void MLDestoryInstance(void *instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasML;
}

int MLConfig(void *instance, char *key, char *secret, char *endpoint,
        char *bucket)
{
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endpoint = %s;\n", endpoint);
    LOGD(LOG_TAG, "bucket = %s;\n", bucket);

    IHaasML *mIHaasML = (IHaasML *)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->Config(key, secret, endpoint, bucket);
    return ret;
}

int MLSetInputData(void *instance, const char *dataPath, const char *compareDataPath)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->SetInputData(dataPath, compareDataPath);
    return ret;
}

int MLLoadNet(void *instance, const char *modePath)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML* mIHaasML = (IHaasML*)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->LoadNet(modePath);
    return ret;
}

int MLPredict(void *instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML *mIHaasML = (IHaasML *)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->Predict();
    return ret;
}

int MLGetPredictResponses(void *instance, char *outResult, int len)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML *mIHaasML = (IHaasML *)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->GetPredictResponses(outResult, len);
    LOGD(LOG_TAG, "out %s; ret = %d;\n", __func__, ret);
    return ret;
}

int MLUnLoadNet(void *instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
    IHaasML *mIHaasML = (IHaasML *)instance;
    if (mIHaasML == NULL)
    {
        LOGD(LOG_TAG, "mIHaasML is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasML->UnLoadNet();
    return ret;
}

