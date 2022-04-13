#ifndef HAAS_DATA_INPUT_CAMERA_LOCALH
#define HAAS_DATA_INPUT_CAMERA_LOCALH

#include "IHaasDataInput.h"

class HaasDataInputCameraLocal : public IHaasDataInput
{
public:
	HaasDataInputCameraLocal();
	virtual ~HaasDataInputCameraLocal();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
