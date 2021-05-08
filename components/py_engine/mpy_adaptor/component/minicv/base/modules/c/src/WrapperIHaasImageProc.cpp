
#include "WrapperIHaasImageProc.h"
#include "IHaasImageProc.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "IHaasImageProc.h"

void* ImageProcCreateInstance(ImageProcType_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc =  IHaasImageProcInstance(type);
    if (mIHaasImageProc == NULL)
    {
        LOG_D("IHaasInstance failed %s;\n", __func__);
        return NULL;
    }
    return (void*)mIHaasImageProc;
#else
    return NULL;
#endif
}

void ImageProcDestoryInstance(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return;
    }
    delete mIHaasImageProc;

#endif
}

int ImageProcOpen(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->Open();
    return ret;
#else
    return 0;
#endif
}

int ImageProcClose(void* instance)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->Close();
    return ret;
#else
    return 0;
#endif
}

int ImageProcCvtColor(void* instance, const ImageBuffer_t* src, PixelFmt_t dst_format,
        ImageBuffer_t** dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->CvtColor(src, dst_format, dst);
    return ret;
#else
    return 0;
#endif
}

int ImageProcResize(void* instance, const ImageBuffer_t* src, const ImageSize_t dst_size,
        ImageBuffer_t** dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->Resize(src, dst_size, dst);
    return ret;
#else
    return 0;
#endif
}

int ImageProcImgCopy(void* instance, const ImageBuffer_t* src, ImageBuffer_t** dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->ImgCopy(src, dst);
    return ret;
#else
    return 0;
#endif
}

int ImageProcRectangle(void* instance, const ImageBuffer_t* src, int32_t left, int32_t top,
        int32_t right, int32_t bottom)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->Rectangle(src, left, top, right, bottom);
    return ret;
#else
    return 0;
#endif
}

int ImageProcCircle(void* instance, const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
        int32_t radius)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->Circle(src, center_x, center_y, radius);
    return ret;
#else
    return 0;
#endif
}

int ImageProcDrawMarker(void* instance, const ImageBuffer_t* src, Point_t *pt, int32_t type)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->DrawMarker(src, pt, type);
    return ret;
#else
    return 0;
#endif
}

int ImageProcFillPoly(void* instance, const ImageBuffer_t* src, Point_t** pts, int32_t color)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->FillPoly(src, pts, color);
    return ret;
#else
    return 0;
#endif
}

int ImageProcPutText(void* instance, const ImageBuffer_t* src, char* text, int32_t left,
        int32_t top)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->PutText(src, text, left, top);
    return ret;
#else
    return 0;
#endif
}

int ImageProcDilateErode(void* instance, const ImageBuffer_t* src, ImageBuffer_t* dst,
        int32_t iMode, int32_t iThresh)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->DilateErode(src, dst, iMode, iThresh);
    return ret;
#else
    return 0;
#endif
}

void ImageProcImageAdd(void* instance, const ImageBuffer_t* src, ImageBuffer_t* added,
        ImageBuffer_t* dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return;
    }
    mIHaasImageProc->ImageAdd(src, added, dst);
#endif
}

void ImageProcImageMinus(void* instance, const ImageBuffer_t* src, ImageBuffer_t* minused,
        ImageBuffer_t* dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return;
    }
    mIHaasImageProc->ImageMinus(src, minused, dst);
#endif
}

void ImageProcImageElementMultiply(void* instance, const ImageBuffer_t* src,
        ImageBuffer_t* multiplied, ImageBuffer_t* dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return;
    }
    mIHaasImageProc->ImageElementMultiply(src, multiplied, dst);
#endif
}

void ImageProcImageElementDivide(void* instance, const ImageBuffer_t* src,
        ImageBuffer_t* divied, ImageBuffer_t* dst)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return;
    }
    mIHaasImageProc->ImageElementDivide(src, divied, dst);
#endif
}

int ImageProcWriteImageToFile(void* instance, const ImageBuffer_t* src, char* file_name)
{
    LOG_D("entern %s;\n", __func__);
#if 0
    IHaasImageProc* mIHaasImageProc = (IHaasImageProc*)instance;
    if (mIHaasImageProc == NULL)
    {
        LOG_D("mIHaasImageProc is NULL %s;\n", __func__);
        return -1;
    }
    int ret = mIHaasImageProc->WriteImageToFile(src, file_name);
    return ret;
#else
    return 0;
#endif
}

