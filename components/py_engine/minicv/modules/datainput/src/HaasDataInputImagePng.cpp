
#include "HaasDataInputImagePng.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputImagePng::HaasDataInputImagePng()
{
    LOG_D("entern\n");
}

HaasDataInputImagePng::~HaasDataInputImagePng()
{
    LOG_D("entern\n");
}

int HaasDataInputImagePng::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImagePng::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImagePng::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputImagePng::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputImagePng::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImagePng::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

