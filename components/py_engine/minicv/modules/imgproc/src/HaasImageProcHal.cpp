
#include "HaasImageProcHal.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"

HaasImageProcHal::HaasImageProcHal()
{
    LOG_D("entern\n");
}

HaasImageProcHal::~HaasImageProcHal()
{
    LOG_D("entern\n");
}

int HaasImageProcHal::Open()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::CvtColor(const ImageBuffer_t* src, PixelFmt_t dst_format,
        ImageBuffer_t** dst)
{
    LOG_I("%s:entern cpu\n", __func__);
    return STATUS_OK;
}

int HaasImageProcHal::Resize(const ImageBuffer_t* src, const ImageSize_t dst_size,
        ImageBuffer_t** dst)
{
    LOG_I("%s:entern\n", __func__);
    return STATUS_OK;
}

int HaasImageProcHal::ImgCopy(const ImageBuffer_t* src, ImageBuffer_t** dst)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::Rectangle(const ImageBuffer_t* src, int32_t left, int32_t top,
        int32_t right, int32_t bottom)
{
    LOG_I("%s:entern\n", __func__);
    return STATUS_OK;
}

int HaasImageProcHal::Circle(const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
        int32_t radius)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::DrawMarker(const ImageBuffer_t* src, Point_t *pt, int32_t type)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::FillPoly(const ImageBuffer_t* src, Point_t** pts, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcHal::PutText(const ImageBuffer_t* src, char* text, int32_t left,
        int32_t top)
{
    LOG_I("%s:entern\n", __func__);
    return STATUS_OK;
}

int HaasImageProcHal::DilateErode(const ImageBuffer_t* src, ImageBuffer_t* dst,
        int32_t iMode, int32_t iThresh)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasImageProcHal::ImageAdd(const ImageBuffer_t* src, ImageBuffer_t* added,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcHal::ImageMinus(const ImageBuffer_t* src, ImageBuffer_t* minused,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcHal::ImageElementMultiply(const ImageBuffer_t* src,
        ImageBuffer_t* multiplied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcHal::ImageElementDivide(const ImageBuffer_t* src,
        ImageBuffer_t* divied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

int HaasImageProcHal::WriteImageToFile(const ImageBuffer_t* src, char* file_name)
{
    LOG_D("entern\n");
    return STATUS_OK;
}
