#ifndef HAAS_DATA_INPUT_IMAGE_BMP_H
#define HAAS_DATA_INPUT_IMAGE_BMP_H

#include "IHaasDataInput.h"

class HaasDataInputImageBmp : public IHaasDataInput
{
public:
	HaasDataInputImageBmp();
	virtual ~HaasDataInputImageBmp();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
