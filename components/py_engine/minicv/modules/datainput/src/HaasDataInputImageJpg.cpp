
#include "HaasDataInputImageJpg.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputImageJpg::HaasDataInputImageJpg()
{
    LOG_D("entern\n");
}

HaasDataInputImageJpg::~HaasDataInputImageJpg()
{
    LOG_D("entern\n");
}

int HaasDataInputImageJpg::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageJpg::Open(int cameraNum)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageJpg::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

bool HaasDataInputImageJpg::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputImageJpg::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputImageJpg::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

