
#include "base/modules/c/include/WrapperIHaasVideoCodec.h"
#include "base/include/IHaasVideoCodec.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"

#define LOG_TAG "WRAPPER_VIDEOCODEC"

void* VideoCodecCreateInstance(VideoCodecType_t type)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec =  IHaasVideoCodecInstance(type);
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasVideoCodec;
#else
    return NULL;
#endif
}

void VideoCodecDestoryInstance(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasVideoCodec;
#endif
}

int VideoCodecOpen(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->Open();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecClose(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->Close();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecDecodeConfig(void* instance, DecodeConfig_t* config)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->DecodeConfig(config);
    return ret;
#else
    return 0;
#endif
}

int VideoCodecStartDecode(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->StartDecode();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecStopDecode(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->StopDecode();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecGetDecodeImageData(void* instance, void *pkt, ImageBuffer_t** image)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->GetDecodeImageData(pkt, image);
    return ret;
#else
    return 0;
#endif
}

int VideoCodecEncodeConfig(void* instance, EncodeConfig_t* config)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->EncodeConfig(config);
    return ret;
#else
    return 0;
#endif
}

int VideoCodecStartEncode(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->StartEncode();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecStopEncode(void* instance)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->StopEncode();
    return ret;
#else
    return 0;
#endif
}

int VideoCodecGetEncodePacketData(void* instance, VideoPacket_t **data)
{
    LOGD(LOG_TAG, "entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOGD(LOG_TAG, "mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->GetEncodePacketData(data);
    return ret;
#else
    return 0;
#endif
}

