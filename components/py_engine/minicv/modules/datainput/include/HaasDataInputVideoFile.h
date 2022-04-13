#ifndef HAAS_DATA_INPUT_VIDEO_FILE_H
#define HAAS_DATA_INPUT_VIDEO_FILE_H

#include "IHaasDataInput.h"
#ifdef SYSTEM_OS_LINUX
#include "DataInputVideoFileFFmpeg.h"
#endif

class HaasDataInputVideoFile : public IHaasDataInput
{
public:
	HaasDataInputVideoFile();
	virtual ~HaasDataInputVideoFile();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
#ifdef SYSTEM_OS_LINUX
private:
    DataInputVideoFileFFmpeg * mDataInputVideoFileFFmpeg;
#endif
};

#endif
