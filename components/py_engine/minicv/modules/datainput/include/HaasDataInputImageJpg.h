#ifndef HAAS_DATA_INPUT_IMAGE_JPG_H
#define HAAS_DATA_INPUT_IMAGE_JPG_H

#include "IHaasDataInput.h"

class HaasDataInputImageJpg : public IHaasDataInput
{
public:
	HaasDataInputImageJpg();
	virtual ~HaasDataInputImageJpg();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
