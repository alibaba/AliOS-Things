
#include "WrapperIHaasVideoCodec.h"
#include "IHaasVideoCodec.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasVideoCodec.h"

void* VideoCodecCreateInstance(VideoCodecType_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec =  IHaasVideoCodecInstance(type);
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasVideoCodec;
#else
    return NULL;
#endif
}

void VideoCodecDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasVideoCodec;
#endif
}

int VideoCodecOpen(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
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
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasVideoCodec* mIHaasVideoCodec = (IHaasVideoCodec*)instance;
    if (mIHaasVideoCodec == NULL)
    {
        LOG_D("mIHaasVideoCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasVideoCodec->GetEncodePacketData(data);
    return ret;
#else
    return 0;
#endif
}

