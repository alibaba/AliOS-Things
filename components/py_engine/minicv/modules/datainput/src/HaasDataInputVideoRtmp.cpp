
#include "HaasDataInputVideoRtmp.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputVideoRtmp::HaasDataInputVideoRtmp()
{
    LOG_D("entern\n");
}

HaasDataInputVideoRtmp::~HaasDataInputVideoRtmp()
{
    LOG_D("entern\n");
}

int HaasDataInputVideoRtmp::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtmp::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtmp::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputVideoRtmp::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputVideoRtmp::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtmp::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

