#ifndef HAAS_DATA_INPUT_CAMERA_USB_H
#define HAAS_DATA_INPUT_CAMERA_USB_H

#include "IHaasDataInput.h"
#ifdef SYSTEM_OS_LINUX
#include "DataInputUsbCameraLinux.h"
#endif

class HaasDataInputCameraUsb : public IHaasDataInput
{
public:
	HaasDataInputCameraUsb();
	virtual ~HaasDataInputCameraUsb();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
private:
#ifdef SYSTEM_OS_LINUX
    DataInputUsbCameraLinux * mUsbCameraLinux;
#endif

};

#endif
