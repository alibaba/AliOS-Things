
#include "HaasDataInputImageGif.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputImageGif::HaasDataInputImageGif()
{
    LOG_D("entern\n");
}

HaasDataInputImageGif::~HaasDataInputImageGif()
{
    LOG_D("entern\n");
}

int HaasDataInputImageGif::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageGif::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageGif::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputImageGif::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputImageGif::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageGif::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

