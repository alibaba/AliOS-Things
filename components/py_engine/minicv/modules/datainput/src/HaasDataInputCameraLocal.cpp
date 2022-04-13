
#include "HaasDataInputCameraLocal.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputCameraLocal::HaasDataInputCameraLocal()
{
    LOG_D("entern\n");
}

HaasDataInputCameraLocal::~HaasDataInputCameraLocal()
{
    LOG_D("entern\n");
}

int HaasDataInputCameraLocal::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraLocal::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraLocal::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputCameraLocal::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputCameraLocal::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraLocal::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

