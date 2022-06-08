
#include "base/modules/c/include/WrapperIHaasDataInput.h"
#include "base/include/IHaasDataInput.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"

#define LOG_TAG "WRAPPER_DATAINPUT"

void* DataInputCreateInstance(DataInputType_t type)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput =  IHaasDataInputInstance(type);
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "IHaasDataInputInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasDataInput;
#else
    return NULL;
#endif
}

void DataInputDestoryInstance(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasDataInput;
#endif
}

int DataInputOpen(void* instance, char* filename)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->Open(filename);
    return ret;
#else
    return 0;
#endif
}

int DataInputOpen2(void* instance, int cameraNum)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->Open(cameraNum);
    return ret;
#else
    return 0;
#endif
}

int DataInputClose(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->Close();
    return ret;
#else
    return 0;
#endif
}

bool DataInputCheckDataReady(void* instance)
{
    LOGD(LOG_TAG, "entern %s; intance = %p;\n", __func__, instance);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    LOGD(LOG_TAG, "entern %s; mIHaasDataInput = %p;\n", __func__, mIHaasDataInput);
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return false;
    }
    bool ret = mIHaasDataInput->CheckDataReady();
    return ret;
#else
    return 0;
#endif
}

int DataInputRequestData(void* instance, ImageBuffer_t** image, int32_t timeout)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->RequestData(image, timeout);
    return ret;
#else
    return 0;
#endif
}

int DataInputReleaseData(void* instance, ImageBuffer_t* image)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOGD(LOG_TAG, "mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->ReleaseData(image);
    return ret;
#else
    return 0;
#endif
}

