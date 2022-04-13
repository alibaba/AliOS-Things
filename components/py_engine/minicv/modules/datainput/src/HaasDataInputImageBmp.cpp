
#include "HaasDataInputImageBmp.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputImageBmp::HaasDataInputImageBmp()
{
    LOG_D("entern\n");
}

HaasDataInputImageBmp::~HaasDataInputImageBmp()
{
    LOG_D("entern\n");
}

int HaasDataInputImageBmp::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageBmp::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageBmp::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputImageBmp::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputImageBmp::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageBmp::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

