
#include "HaasDataInputVideoRtsp.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputVideoRtsp::HaasDataInputVideoRtsp()
{
    LOG_D("entern\n");
}

HaasDataInputVideoRtsp::~HaasDataInputVideoRtsp()
{
    LOG_D("entern\n");
}

int HaasDataInputVideoRtsp::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtsp::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputVideoRtsp::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputVideoRtsp::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtsp::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputVideoRtsp::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

