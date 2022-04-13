#ifndef DATA_INPUT_VIDEO_FILE_FFMPEG_H
#define DATA_INPUT_VIDEO_FILE_FFMPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#include "HaasCommonImage.h"
#include "HaasCommonVideo.h"
#include "HaasVideoCodecFFmpeg.h"

class DataInputVideoFileFFmpeg
{
    public:
        DataInputVideoFileFFmpeg();
        ~DataInputVideoFileFFmpeg();

        int Open(const char *filename);
        void Close();
        bool CheckStream();
        ImageBuffer_t* GetImageData();
    private:
        int Parse();
        VideoCodecID_t mCodecID;
        PixelFmt_t     mFormat;
        ImageSize_t    mFrameSize;
        AVFormatContext *pFormatCtx;
        AVBSFContext *mAbsCtx;
        AVPacket *mPacket;
        int32_t mVideoIndex;
        VideoCodecFFmpeg* mVideoCodecFFmpeg;
        bool mHasStream;
};

#endif
