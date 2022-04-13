#ifndef DATA_INPUT_USB_CAMERA_LINUX_H
#define DATA_INPUT_USB_CAMERA_LINUX_H

#include <string>
#include "HaasCommonImage.h"

typedef struct {
	uint32_t resolution;
	uint32_t format;
} UsbCameraCap_t;

typedef struct {
	void *start;
	uint32_t offset;
	size_t length;
} UsbCameraBuffer;

class DataInputUsbCameraLinux
{
public:
	DataInputUsbCameraLinux();
	virtual ~DataInputUsbCameraLinux();
	int Open(int cameraNum);
	int Close();
	int Start();
	int Stop();
	int GetCapability(UsbCameraCap_t *cap);
	int SetResolution(ImageSize_t size);
	int SetFrameRate(uint32_t rate);
	int SetPixelFormat(PixelFmt_t fmt);
	int RequestImage(ImageBuffer_t** image, int32_t timeout);
	int ReleaseImage(ImageBuffer_t* image);

private:
	int InitDevice();
	int UninitDevice();
	int InitBuffer();
	int UninitBuffer();
	uint32_t GetV4l2PixFmt(PixelFmt_t fmt);

	int devIndex;
	std::string devName;
	bool isRunning;
	int devFd;
	PixelFmt_t devPixFmt;
	uint32_t devWidth;
	uint32_t devHeight;
	uint32_t devFrameRate;

	uint32_t bufferNum;
	UsbCameraBuffer *bufferList;
	ImageBuffer_t *imageInfoList;
};

#endif
