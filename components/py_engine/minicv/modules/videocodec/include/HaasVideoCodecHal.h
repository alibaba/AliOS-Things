#ifndef HAAS_VIDEO_CODEC_HAL_H
#define HAAS_VIDEO_CODEC_HAL_H

#include "IHaasVideoCodec.h"

class HaasVideoCodecHal : public IHaasVideoCodec
{
public:
	HaasVideoCodecHal();
	virtual ~HaasVideoCodecHal();

	virtual int Open();
	virtual int Close();

	virtual int DecodeConfig(DecodeConfig_t* config);
	virtual int StartDecode();
	virtual int StopDecode();
	virtual int GetDecodeImageData(void *pkt, ImageBuffer_t** image);

	virtual int EncodeConfig(EncodeConfig_t* config);
	virtual int StartEncode();
	virtual int StopEncode();
	virtual int GetEncodePacketData(VideoPacket_t **data);
};
#endif
