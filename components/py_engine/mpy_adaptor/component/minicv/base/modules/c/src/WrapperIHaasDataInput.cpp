
#include "WrapperIHaasDataInput.h"
#include "IHaasDataInput.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasDataInput.h"

void* DataInputCreateInstance(DataInputType_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput =  IHaasDataInputInstance(type);
    if (mIHaasDataInput == NULL)
    {
        LOG_D("IHaasDataInputInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasDataInput;
#else
    return NULL;
#endif
}

void DataInputDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasDataInput;
#endif
}

int DataInputOpen(void* instance, char* filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
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
    LOG_D("entern %s; intance = %p;\n", __func__, instance);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    LOG_D("entern %s; mIHaasDataInput = %p;\n", __func__, mIHaasDataInput);
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasDataInput* mIHaasDataInput = (IHaasDataInput*)instance;
    if (mIHaasDataInput == NULL)
    {
        LOG_D("mIHaasDataInput is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasDataInput->ReleaseData(image);
    return ret;
#else
    return 0;
#endif
}

