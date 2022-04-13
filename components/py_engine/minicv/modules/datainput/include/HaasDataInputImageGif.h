#ifndef HAAS_DATA_INPUT_IMAGE_GIF_H
#define HAAS_DATA_INPUT_IMAGE_GIF_H

#include "IHaasDataInput.h"

class HaasDataInputImageGif : public IHaasDataInput
{
public:
	HaasDataInputImageGif();
	virtual ~HaasDataInputImageGif();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
