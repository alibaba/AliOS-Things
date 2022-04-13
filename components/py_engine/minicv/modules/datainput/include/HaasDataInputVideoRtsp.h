#ifndef HAAS_DATA_INPUT_VIDEO_RTSP_H
#define HAAS_DATA_INPUT_VIDEO_RTSP_H

#include "IHaasDataInput.h"

class HaasDataInputVideoRtsp : public IHaasDataInput
{
public:
	HaasDataInputVideoRtsp();
	virtual ~HaasDataInputVideoRtsp();

	virtual int Open(char *filename);
	virtual int Open(int cameraNum);
	virtual int Close();
	virtual bool CheckDataReady();
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout);
	virtual int ReleaseData(ImageBuffer_t* image);
};

#endif
