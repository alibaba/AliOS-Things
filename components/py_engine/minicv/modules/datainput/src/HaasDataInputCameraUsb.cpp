
#include "HaasDataInputCameraUsb.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasDataInputCameraUsb::HaasDataInputCameraUsb()
#ifdef SYSTEM_OS_LINUX
	: mUsbCameraLinux(NULL)
#endif
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    mUsbCameraLinux = new DataInputUsbCameraLinux();
#endif
}

HaasDataInputCameraUsb::~HaasDataInputCameraUsb()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    delete mUsbCameraLinux;
#endif
}

int HaasDataInputCameraUsb::Open(char *filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasDataInputCameraUsb::Open(int cameraNum)
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    int ret = -1;
    ImageSize_t size;
    ret = mUsbCameraLinux->Open(cameraNum);
    if (ret != 0) {
        LOG_E("Failed to open CameraNum = %d;\n", cameraNum);
        return STATUS_ENODEV;
    }

    ret = mUsbCameraLinux->SetPixelFormat(PIXEL_FMT_YUYV422);
    if (ret != 0) {
        LOG_E("Failed to set format PIXEL_FMT_YUYV422\n");
        return STATUS_ERROR;
    }

    size.width = 640;
    size.height = 480;
    ret = mUsbCameraLinux->SetResolution(size);
    if (ret != 0) {
        LOG_E("Failed to set SetResolution 640x480\n");
        return STATUS_ERROR;
    }

    ret = mUsbCameraLinux->SetFrameRate(30);
    if (ret != 0) {
        LOG_E("Failed to set fps 30\n");
        return STATUS_ERROR;
    }

    ret = mUsbCameraLinux->Start();
    if (ret != 0) {
        LOG_E("Failed to start camera loop\n");
        return STATUS_ERROR;
    }
#endif
    return STATUS_OK;
}

int HaasDataInputCameraUsb::Close()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    int ret = -1;
    ret = mUsbCameraLinux->Close();
    if (ret != 0) {
        LOG_E("Failed to close camera device\n");
        return STATUS_ERROR;
    }
#endif
    return STATUS_OK;
}

bool HaasDataInputCameraUsb::CheckDataReady()
{
    LOG_D("entern\n");
    return true;
}

int HaasDataInputCameraUsb::RequestData(ImageBuffer_t** image, int32_t timeout)
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    int ret = -1;
    // For UVC camera, the first 3 frames are dropped.
    for (int i = 0; i < 3; i++) {
        if (mUsbCameraLinux->RequestImage(image, 1000) == 0)
            mUsbCameraLinux->ReleaseImage(*image);
    }
    ret = mUsbCameraLinux->RequestImage(image, 1000);
    if (ret != 0) {
        LOG_E("Failed to get camera data\n");
        return STATUS_ERROR;
    }
#endif
    return STATUS_OK;
}

int HaasDataInputCameraUsb::ReleaseData(ImageBuffer_t* image)
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    int ret = -1;
    ret = mUsbCameraLinux->ReleaseImage(image);
    if (ret != 0) {
        LOG_E("Failed to release camera data\n");
        return STATUS_ERROR;
    }
#endif
    LOG_D("out\n");
    return STATUS_OK;
}

