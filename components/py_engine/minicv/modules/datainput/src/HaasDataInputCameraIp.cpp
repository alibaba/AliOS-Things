
#include "HaasDataInputCameraIp.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputCameraIp::HaasDataInputCameraIp()
{
    LOG_D("entern\n");
}

HaasDataInputCameraIp::~HaasDataInputCameraIp()
{
    LOG_D("entern\n");
}

int HaasDataInputCameraIp::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraIp::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputCameraIp::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputCameraIp::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraIp::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraIp::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

