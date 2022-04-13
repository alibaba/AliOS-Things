#ifndef HAAS_VIDEO_CODEC_FFMPEG_H
#define HAAS_VIDEO_CODEC_FFMPEG_H

#include "IHaasVideoCodec.h"
#include "HaasCommonImage.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class HaasVideoCodecFFmpeg : public IHaasVideoCodec
{
public:
	HaasVideoCodecFFmpeg();
	virtual ~HaasVideoCodecFFmpeg();

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
private:
    AVCodecContext *CodecContext;
    AVCodec *Codec;
    AVFrame *FrameDecode;
};
#endif
