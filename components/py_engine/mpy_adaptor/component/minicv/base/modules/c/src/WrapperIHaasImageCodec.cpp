#include "WrapperIHaasImageCodec.h"
#include "IHaasImageCodec.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasImageCodec.h"

void* ImageCodecCreateInstance(CodecImageType_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec =  IHaasImageCodecInstance(type);
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasImageCodec;
#else
    return NULL;
#endif
}

void ImageCodecDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasImageCodec;
#endif
}

int ImageCodecImgRead(void* instance, ImageBuffer_t **image, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgRead(image, filename);
    return ret;
#else
    return 0;
#endif
}

int ImageCodecImgReadMulti(void* instance, ImageBuffer_t **images, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgReadMulti(images, filename);
    return ret;
#else
    return 0;
#endif
}

int ImageCodecImgWrite(void* instance, ImageBuffer_t *image, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgWrite(image, filename);
    return ret;
#else
    return 0;
#endif
}

int ImageCodecImgWriteMulti(void* instance, ImageBuffer_t **images, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgWriteMulti(images, filename);
    return ret;
#else
    return 0;
#endif
}

int ImageCodecImgDecode(void* instance, void *addr, ImageBuffer_t **image)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgDecode(addr, image);
    return ret;
#else
    return 0;
#endif
}

ImageBuffer_t * ImageCodecImgDecode2(void* instance, const char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return NULL;
    }
    ImageBuffer_t * buffer = mIHaasImageCodec->ImgDecode(filename);
    return buffer;
#else
    return NULL;
#endif
}

int ImageCodecImgEncode(void* instance, void *addr, ImageBuffer_t ** image)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->ImgEncode(addr, image);
    return ret;
#else
    return 0;
#endif
}

int ImageCodechaveImageReader(void* instance, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->haveImageReader(filename);
    return ret;
#else
    return 0;
#endif
}

int ImageCodechaveImageWriter(void* instance, char * filename)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageCodec* mIHaasImageCodec = (IHaasImageCodec*)instance;
    if (mIHaasImageCodec == NULL)
    {
        LOG_D("mIHaasImageCodec is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageCodec->haveImageWriter(filename);
    return ret;
#else
    return 0;
#endif
}

