
#include "HaasVideoCodecFFmpeg.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasVideoCodecFFmpeg::HaasVideoCodecFFmpeg()
    : CodecContext(NULL),
      Codec(NULL),
      FrameDecode(NULL)
{
    LOG_D("entern\n");
}

HaasVideoCodecFFmpeg::~HaasVideoCodecFFmpeg()
{
    LOG_D("entern\n");
}

int HaasVideoCodecFFmpeg::Open()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 10, 100)
    avcodec_register_all();
#endif
    Codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!Codec) {
        LOG_E("Codec(%d) not found in FFmpeg\n", AV_CODEC_ID_H264);
        return -1;
    }

    CodecContext = avcodec_alloc_context3(Codec);
    if (!CodecContext) {
        LOG_E("Could not allocate video codec context\n");
        return -1;
    }

    if (avcodec_open2(CodecContext, Codec, NULL) < 0 ) {
        LOG_E("avcodec_open2 failed, Could not open codec\n");
        return -1;
    }

    FrameDecode = av_frame_alloc();
    if (FrameDecode == NULL) {
        LOG_E("av_frame_alloc failed\n");
        return -1;
    }
#endif
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::Close()
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
	if (FrameDecode != NULL) {
		av_frame_free(&FrameDecode);
	}

	if (CodecContext != NULL) {
		avcodec_close(CodecContext);
		avcodec_free_context(&CodecContext);
	}
#endif
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::DecodeConfig(DecodeConfig_t* config)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::StartDecode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::StopDecode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::GetDecodeImageData(void *pkt, ImageBuffer_t** image)
{
    LOG_D("entern\n");
#ifdef SYSTEM_OS_LINUX
    int ret;
    int retryTime = 0;  // Unit: ms
    int timeout = 1000;  // Unit: ms
	if (pkt == NULL || pkt->data == NULL || pkt->size <= 0) {
		LOG_E("pkt->data=%p, pkt->size=%d is invalid\n",
				pkt->data, pkt->size);
		return -1;
	}

    if (*image == NULL)
    {
        LOG_D("*image is illegal\n");
		return -1;
    }

	while (1) {
		ret = avcodec_send_packet(CodecContext, pkt);
		if (ret == 0) {
			break;
		}

		if (timeout >= 0) {
			if (retryTime >= timeout) {
				LOG_D("%dms time out\n", timeout);
				ret = -1;
				break;
			} else {
				usleep(10*1000);
				retryTime += 10;
			}
		} else {
			usleep(10*1000);
		}
	}

	if (ret < 0) {
		LOG_E("avcodec_send_packet failed: %d\n", /*av_err2str_private(ret)*/ret);
		return -1;
	}
    LOG_E("out AVVideo::Decode\n");
    LOG_E("out AVVideo::Decode start recevice data\n");

    int flag = 1;
    while(flag == 1) {
        ret = avcodec_receive_frame(CodecContext, FrameDecode);
        if (ret == 0) {
            LOG_D("[%d] data=%p, linesize=%d, width=%d, height=%d\n",
                    CodecContext->frame_number, FrameDecode->data[0],
                    FrameDecode->linesize[0], FrameDecode->width, FrameDecode->height);
           // LOG_D("frameData y_src = 0x%x;u_src = 0x%x; v_src = 0x%x; x-src = 0x%x;\n",
           // *FrameDecode->data[0], *FrameDecode->data[1],
           // *FrameDecode->data[2], *FrameDecode->data[3]);
            LOG_D("frameData y_src = 0x%x;u_src = 0x%x; v_src = 0x%x; x-src = 0x%x;\n", 
                    FrameDecode->data[0], FrameDecode->data[1],
                    FrameDecode->data[2], FrameDecode->data[3]);
            LOG_D("frameData line y_src = 0x%x;u_src = 0x%x; v_src = 0x%x;x-src = 0x%x;\n", 
                    FrameDecode->linesize[0], FrameDecode->linesize[1],
                    FrameDecode->linesize[2], FrameDecode->linesize[3]);
            flag = 0;
        } else {
            LOG_E("avcodec_receive_frame failed, ret=%d:\n", ret);
            usleep(10*1000);
        }
    }

    LOG_E("start set image info\n");
    (*image)->width = FrameDecode->width;
    (*image)->stride = FrameDecode->width * 1.5;
    (*image)->height = FrameDecode->height;
    (*image)->size = ImageUtils::GetSize(PIXEL_FMT_YUV420P, FrameDecode->width, FrameDecode->height);
    (*image)->format = PIXEL_FMT_YUV420P;
    (*image)->address[0] = (uint8_t*)FrameDecode->data[0];
    (*image)->address[1] = (uint8_t*)FrameDecode->data[1];
    (*image)->address[2] = (uint8_t*)FrameDecode->data[2];
    (*image)->linesize[0] = (uint32_t)FrameDecode->linesize[0];
    (*image)->linesize[1] = (uint32_t)FrameDecode->linesize[1];
    (*image)->linesize[2] = (uint32_t)FrameDecode->linesize[2];
    LOG_E("out AVVideo::Decode end recevice data\n");
#endif
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::EncodeConfig(EncodeConfig_t* config)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::StartEncode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::StopEncode()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasVideoCodecFFmpeg::GetEncodePacketData(VideoPacket_t **data)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

