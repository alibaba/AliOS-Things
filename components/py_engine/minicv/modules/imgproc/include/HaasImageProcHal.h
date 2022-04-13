#ifndef HAAS_IMAGE_PROC_HAL_H
#define HAAS_IMAGE_PROC_HAL_H

#include <string>
#include "IHaasImageProc.h"

class HaasImageProcHal : public IHaasImageProc
{
    public:
        HaasImageProcHal();
        virtual ~HaasImageProcHal();

        virtual int Open();
        virtual int Close();
        virtual int CvtColor(const ImageBuffer_t* src, PixelFmt_t dst_format,
                ImageBuffer_t** dst);
        virtual int Resize(const ImageBuffer_t* src, const ImageSize_t dst_size,
                ImageBuffer_t** dst);
        virtual int ImgCopy(const ImageBuffer_t* src, ImageBuffer_t** dst);
        virtual int Rectangle(const ImageBuffer_t* src, int32_t left, int32_t top,
                int32_t right, int32_t bottom);
        virtual int Circle(const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
                int32_t radius);
        virtual int DrawMarker(const ImageBuffer_t* src, Point_t *pt, int32_t type);
        virtual int FillPoly(const ImageBuffer_t* src, Point_t** pts, int32_t color);
        virtual int PutText(const ImageBuffer_t* src, char* text, int32_t left,
                int32_t top);
        virtual int DilateErode(const ImageBuffer_t* src, ImageBuffer_t* dst,
                int32_t iMode, int32_t iThresh);
        virtual void ImageAdd(const ImageBuffer_t* src, ImageBuffer_t* added,
                ImageBuffer_t* dst);
        virtual void ImageMinus(const ImageBuffer_t* src, ImageBuffer_t* minused,
                ImageBuffer_t* dst);
        virtual void ImageElementMultiply(const ImageBuffer_t* src,
                ImageBuffer_t* multiplied, ImageBuffer_t* dst);
        virtual void ImageElementDivide(const ImageBuffer_t* src,
                ImageBuffer_t* divied, ImageBuffer_t* dst);
        virtual int WriteImageToFile(const ImageBuffer_t* src, char* file_name);
};
#endif
