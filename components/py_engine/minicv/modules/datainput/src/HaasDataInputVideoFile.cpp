
#include "HaasDataInputVideoFile.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputVideoFile::HaasDataInputVideoFile()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    mDataInputVideoFileFFmpeg = new DataInputVideoFileFFmpeg();
    if (mDataInputVideoFileFFmpeg == NULL)
    {
        LOG_D("new DataInputVideoFileFFmpeg failed\n");
    }
#endif
}

HaasDataInputVideoFile::~HaasDataInputVideoFile()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    if (mDataInputVideoFileFFmpeg != NULL)
    {
        delete mDataInputVideoFileFFmpeg;
    }
#endif
}

int HaasDataInputVideoFile::Open(char *filename)
{
    LOG_D("entern filename = %s;\n", filename);
#ifdef SYSTEM_OS_LINUX
    mDataInputVideoFileFFmpeg->Open(filename);
#endif
    LOG_D("out filename = %s;\n", filename);
    return STATUS_OK;
}

int HaasDataInputVideoFile::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoFile::Close()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    mDataInputVideoFileFFmpeg->Close();
#endif
    LOG_D("out\n");
    return STATUS_OK;
}

bool HaasDataInputVideoFile::CheckDataReady()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    return mDataInputVideoFileFFmpeg->CheckStream();
#else
    return false;
#endif
}

int HaasDataInputVideoFile::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    if (image == NULL)
    {
        LOG_D("illegal image par\n");
        return -1;
    }

    if (*image != NULL)
    {
        LOG_D("*image is not NULL & free ptr\n");
        free(*image);
    }

#ifdef SYSTEM_OS_LINUX
    *image = mDataInputVideoFileFFmpeg->GetImageData();
#endif
    if (*image == NULL)
    {
        LOG_I("get image_video  failed %s;\n", __func__);
        return -1;
    }
    return STATUS_OK;
}

int HaasDataInputVideoFile::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    if (image != NULL)
    {
        free(image);
    }
    return STATUS_OK;
}

