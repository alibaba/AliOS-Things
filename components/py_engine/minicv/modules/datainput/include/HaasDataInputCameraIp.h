#ifndef HAAS_DATA_INPUT_CAMERA_IP_H
#define HAAS_DATA_INPUT_CAMERA_IP_H

#include "IHaasDataInput.h"

class HaasDataInputCameraIp : public IHaasDataInput
{
public:
	HaasDataInputCameraIp();
	virtual ~HaasDataInputCameraIp();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
