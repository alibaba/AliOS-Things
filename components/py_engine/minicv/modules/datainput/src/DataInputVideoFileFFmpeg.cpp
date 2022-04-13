
#include "HaasLog.h"
#include "HaasErrno.h"
#include "DataInputVideoFileFFmpeg.h"


DataInputVideoFileFFmpeg::DataInputVideoFileFFmpeg()
    : pFormatCtx(NULL), mAbsCtx(NULL), mPacket(NULL),
    mVideoIndex(-1), mVideoCodecFFmpeg(NULL), mHasStream(false)
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
    av_register_all();
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 10, 100)
    avcodec_register_all();
#endif
}

DataInputVideoFileFFmpeg::~DataInputVideoFileFFmpeg()
{

}

bool DataInputVideoFileFFmpeg::CheckStream()
{
    return mHasStream;
}

int DataInputVideoFileFFmpeg::Open(const char *filename)
{
    LOG_D("entern\n");
    if (access(filename, F_OK) != 0) {
        LOG_E("filename[%s] not exist.\n", filename);
        return -1;
    }

    pFormatCtx = avformat_alloc_context();
    mPacket = av_packet_alloc();

    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        LOG_E("Couldn't open input file:[%s]\n", filename);
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOG_E("Couldn't find stream information.\n");
        return -1;
    }

    av_dump_format(pFormatCtx, 0, filename, false);

    mVideoIndex = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOG_E("AVMEDIA_TYPE_VIDEO codec_id = %d;\n", pFormatCtx->streams[i]->codecpar->codec_id);
            mCodecID = (VideoCodecID_t)pFormatCtx->streams[i]->codecpar->codec_id;
            LOG_E("AVMEDIA_TYPE_VIDEO pix_fmt = 0x%x;\n", pFormatCtx->streams[i]->codecpar->format);
            mFormat = (PixelFmt_t)pFormatCtx->streams[i]->codecpar->format;
            LOG_E("AVMEDIA_TYPE_VIDEO w = 0x%x;h = 0x%x;\n", pFormatCtx->streams[i]->codecpar->width, pFormatCtx->streams[i]->codecpar->height);
            mFrameSize.width = pFormatCtx->streams[i]->codecpar->width;
            mFrameSize.height = pFormatCtx->streams[i]->codecpar->height;
            mVideoIndex = i;
            break;
        }
    }
    if (mVideoIndex == -1) {
        LOG_E("Didn't find a video stream.\n");
        return -1;
    }

    const AVBitStreamFilter *absFilter = av_bsf_get_by_name("h264_mp4toannexb");
    av_bsf_alloc(absFilter, &mAbsCtx);
    if (avcodec_parameters_copy(mAbsCtx->par_in, pFormatCtx->streams[mVideoIndex]->codecpar) < 0) {
        LOG_E("Couldn't set stream bsf.\n");
        return -1;
    }
    av_bsf_init(mAbsCtx);

    mVideoCodecFFmpeg = new VideoCodecFFmpeg();
    if (mVideoCodecFFmpeg == NULL)
    {
        LOG_D("new VideoCodecFFmpeg failed\n");
        return -1;
    }

    mHasStream = true;
    LOG_D("out\n");
    return 0;

}

void DataInputVideoFileFFmpeg::Close()
{
    mHasStream = false;
    if (pFormatCtx)
        avformat_free_context(pFormatCtx);

    if (mPacket)
        av_packet_free(&mPacket);

    if (mAbsCtx)
        av_bsf_free(&mAbsCtx);

    if (mVideoCodecFFmpeg != NULL)
        delete mVideoCodecFFmpeg;
}

ImageBuffer_t* DataInputVideoFileFFmpeg::GetImageData()
{
    ImageBuffer_t* image = NULL;
    int ret = -1;
    image = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
    do {
    ret = Parse();
    } while(ret == -1);
    if (ret == 2)
    {
        mHasStream = false;
        free(image);
        return NULL;
    }
    mVideoCodecFFmpeg->Decode(mPacket, &image);
    return image;
}

int DataInputVideoFileFFmpeg::Parse()
{
    LOG_E("entern");
    if (mVideoIndex == -1) {
        LOG_E("Invalid Video.\n");
        return -1;
    }

    av_packet_unref(mPacket);//unref pre av_bsf_receive_packet first
    if (av_bsf_receive_packet(mAbsCtx, mPacket) == 0) {

        if (mPacket->pts == AV_NOPTS_VALUE)
            mPacket->pts = 0;

        LOG_D("return B.\n");
        return 0;
    }

    if (av_read_frame(pFormatCtx, mPacket) >= 0)
    {
        if (mPacket->stream_index == mVideoIndex)
        {
            if (av_bsf_send_packet(mAbsCtx, mPacket) != 0) {
                av_packet_unref(mPacket);
                LOG_D("av_bsf_send_packet failed.\n");
                return -1;
            }
            if (av_bsf_receive_packet(mAbsCtx, mPacket) == 0) {

                if (mPacket->pts == AV_NOPTS_VALUE)
                    mPacket->pts = 0;

                LOG_D("return A.\n");
                return 0;
            } else {
                LOG_D("av_bsf_receive_packet failed.\n");
            }
        } else {
            LOG_D("stream_index isn't AVMEDIA_TYPE_VIDEO.\n");
        }
    } else {
    LOG_E("return 2");
        return 2;
    }
    LOG_E("out");
    return -1;
}
