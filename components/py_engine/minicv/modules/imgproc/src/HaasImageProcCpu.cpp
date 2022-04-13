
#include "HaasImageProcCpu.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"
#include "SkBitmap.h"
#include "SkCanvas.h"

HaasImageProcCpu::HaasImageProcCpu()
    : mImageCvtColorCpu(NULL),
      mImageResizeCpu(NULL)
{
    LOG_D("entern\n");
    mImageCvtColorCpu = new ImageCvtColorCpu();
    mImageResizeCpu = new ImageResizeCpu();
}

HaasImageProcCpu::~HaasImageProcCpu()
{
    LOG_D("entern\n");
    delete(mImageCvtColorCpu);
    delete(mImageResizeCpu);
}

int HaasImageProcCpu::Open()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::CvtColor(const ImageBuffer_t* src, PixelFmt_t dst_format,
        ImageBuffer_t** dst)
{
    LOG_I("%s:entern cpu\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

    if (dst == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

    LOG_I("%s:cpu \n", __func__);
	int width = src->width;
	int height = src->height;
	PixelFmt_t in_format = src->format;
	PixelFmt_t out_format = dst_format;
	*dst = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
	(*dst)->width = width;
	(*dst)->height = height;
	(*dst)->size = ImageUtils::GetSize(out_format, width, height);
	(*dst)->format = out_format;
	(*dst)->address[0] = (uint8_t*)malloc((*dst)->size);

    LOG_I("%s:cpu  in_format = %d;out_format = %d; PIXEL_FMT_RGBA = %d;\n", __func__, in_format, out_format, PIXEL_FMT_RGBA);
#if 1
        int num_elements = width * height *3;
        uint8_t *srcdata = src->address[0];
        uint8_t *dstdata = (*dst)->address[0];
        int i = 0, j = 0;
    if (in_format == PIXEL_FMT_YUV420P) {
            LOG_I("%s:PIXEL_FMT_YUV420P->ARGB\n", __func__);
        mImageCvtColorCpu->I420ToARGB(
                src->address[0],
                src->linesize[0],
                src->address[1],
                src->linesize[1],
                src->address[2],
                src->linesize[2],
                dstdata, src->width *4, src->width, src->height);
    }
    else if (in_format == PIXEL_FMT_RGBA) {
            LOG_I("%s:RGBA->BGR\n", __func__);
#if 0
        for(i=0; i<num_elements; i+=3, j+=4)
        {
            //LOG_I("%s:cpu 3 i = %d;j = %d;\n", __func__, i, j);
            dstdata[i] = srcdata[j];
            dstdata[i+1] = srcdata[j+1];
            dstdata[i+2] = srcdata[j+2];
        }
#else

        mImageCvtColorCpu->RGBAToBGR(srcdata, dstdata, width, height);
#endif
    }
    else if (in_format == PIXEL_FMT_ARGB)
    {
        LOG_I("%s:ARGB->BGR\n", __func__);
#if 0
        for(i=0; i<num_elements; i+=3, j+=4)
        {
            dstdata[i] = srcdata[j +1];
            dstdata[i+1] = srcdata[j+2];
            dstdata[i+2] = srcdata[j+3];
        }
#else
        mImageCvtColorCpu->ARGBToBGR(srcdata, dstdata, width, height);
#endif
    }
    else if (in_format == PIXEL_FMT_YUYV422)
    {
        if (dst_format == PIXEL_FMT_ARGB)
        {
            LOG_I("%s:xYUV422->RGBA\n", __func__);
            mImageCvtColorCpu->YUY2ToARGB(srcdata,
                    width * 2,
                    dstdata,
                    width * 4,
                    width,
                    height);
        }
        else
        {
            LOG_I("%s:xYUV422->BGR\n", __func__);
            mImageCvtColorCpu->YUY2ToBGR(srcdata,
                    width * 2,
                    dstdata,
                    width * 3,
                    width,
                    height);

        }
    }
    else
    {
        LOG_I("%s:BGR->RGBA\n", __func__);
        for(i=0; i<num_elements; i+=3, j+=4)
        {
            //LOG_I("%s:cpu 3 i = %d;j = %d;\n", __func__, i, j);
            dstdata[j] = srcdata[i +2];
            dstdata[j+1] = srcdata[i+1];
            dstdata[j+2] = srcdata[i];
            dstdata[j+3] = 0XFF;
        }

    }
#else
	int num_elements = width * height;

    uint8_t *srcdata = src->address[0];
    uint8_t *dest1 = (*dst)->address[0];
    uint8_t *dest2 = dest1 + num_elements;
    uint8_t *dest3 = dest2 + num_elements;
    int i = 0, j = 0;
    for(i=0; i<num_elements; ++i, j+=3)
    {
        dest1[i] = srcdata[j];
        dest2[i] = srcdata[j+1];
        dest3[i] = srcdata[j+2];
    }
#endif

    LOG_I("%s:xout cpu\n", __func__);
    return STATUS_OK;
}

int HaasImageProcCpu::Resize(const ImageBuffer_t* src, const ImageSize_t dst_size,
        ImageBuffer_t** dst)
{
    LOG_I("%s:entern\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int stride = src->stride;
	int width = src->width;
	int height = src->height;
	int format = src->format;

	*dst = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
	(*dst)->width = dst_size.width;
	(*dst)->stride = dst_size.width;
	(*dst)->height = dst_size.height;
	(*dst)->size = ImageUtils::GetSize(src->format, dst_size.width, dst_size.height);
	(*dst)->format = src->format;
	(*dst)->address[0] = (uint8_t*)malloc((*dst)->size);

    LOG_I("%s:format = %d;PIXEL_FMT_RGB24 = %d;stride = %d;\n", __func__, format, PIXEL_FMT_RGB24, stride);
    LOG_I("%s:src width = %d; height = %d; dst width = %d;height = %d;\n", __func__, width, height, dst_size.width, dst_size.height);

    LOG_I("%s:resize call ResizeRGBInterleaved\n", __func__);
    mImageResizeCpu->ResizeRGBInterleaved(src->address[0], (*dst)->address[0], width, height, dst_size.width, dst_size.height);
    LOG_I("%s:out\n", __func__);
    return STATUS_OK;
}

int HaasImageProcCpu::ImgCopy(const ImageBuffer_t* src, ImageBuffer_t** dst)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::Rectangle(const ImageBuffer_t* src, int32_t left, int32_t top,
        int32_t right, int32_t bottom)
{
    LOG_I("%s:entern\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int width = src->width;
	int height = src->height;
    int height_stride = height;
	int src_format = src->format;
    LOG_I("%s: width = %d;height = %d; format = %d;\n", __func__, width, height, src_format);

    SkBitmap bitmap;
    bitmap.setConfig(SkBitmap::Config::kARGB_8888_Config, width, height);
    bitmap.setPixels((void *)src->address[0]);
    SkCanvas Canvas(bitmap);
    SkRect rect;
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(SkIntToScalar(3));
    paint.setColor(SK_ColorRED);
    rect.set((SkScalar)left, (SkScalar)top, (SkScalar)right, (SkScalar)bottom);
    bitmap.lockPixels();
    Canvas.drawRect(rect, paint);
    bitmap.unlockPixels();

    LOG_I("%s:left = %d;top = %d;right = %d;bottom = %d;\n", __func__,left, top, right, bottom);
    LOG_I("%s:out\n", __func__);
    return STATUS_OK;
}

int HaasImageProcCpu::Circle(const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
        int32_t radius)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::DrawMarker(const ImageBuffer_t* src, Point_t *pt, int32_t type)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::FillPoly(const ImageBuffer_t* src, Point_t** pts, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcCpu::PutText(const ImageBuffer_t* src, char* text, int32_t left,
        int32_t top)
{
    LOG_I("%s:entern\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int width = src->width;
	int height = src->height;
    int height_stride = height;
	int src_format = src->format;
    LOG_I("%s: width = %d;height = %d; format = %d;\n", __func__, width, height, src_format);
    LOG_I("%s: text = %s;\n", __func__, text);

    SkBitmap bitmap;
    bitmap.setConfig(SkBitmap::Config::kARGB_8888_Config, width, height);
    bitmap.setPixels((void *)src->address[0]);
    SkCanvas Canvas(bitmap);
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(SkIntToScalar(2));
    paint.setColor(SK_ColorRED);
    paint.setTextSize(SkIntToScalar(25));
    bitmap.lockPixels();
    Canvas.drawText(text, 100, left, top, paint);
    bitmap.unlockPixels();

    LOG_I("%s:out\n", __func__);
    return STATUS_OK;
}

int HaasImageProcCpu::DilateErode(const ImageBuffer_t* src, ImageBuffer_t* dst,
        int32_t iMode, int32_t iThresh)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasImageProcCpu::ImageAdd(const ImageBuffer_t* src, ImageBuffer_t* added,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcCpu::ImageMinus(const ImageBuffer_t* src, ImageBuffer_t* minused,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcCpu::ImageElementMultiply(const ImageBuffer_t* src,
        ImageBuffer_t* multiplied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcCpu::ImageElementDivide(const ImageBuffer_t* src,
        ImageBuffer_t* divied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

int HaasImageProcCpu::WriteImageToFile(const ImageBuffer_t* src, char* file_name)
{
    LOG_D("entern\n");
    return STATUS_OK;
}
