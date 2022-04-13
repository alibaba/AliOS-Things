#ifndef HAAS_IMAGE_RESIZE_CPU_H
#define HAAS_IMAGE_RESIZE_CPU_H

#include "HaasCommonImage.h"
#include "ImageColorCommon.h"

class ImageResizeCpu
{
    public:
        ImageResizeCpu();
        ~ImageResizeCpu();
        void ResizeRGBInterleaved(
                unsigned char *pucSrcData,
                unsigned char *pucDstData,
                int iWidthSrc,
                int iPixelHeightSrc,
                int iWidthDst,
                int iPixelHeightDst);
    private:
        void H_RESIZE_3C1L(
                int j,
                int* piXInSrc,
                unsigned short* pusCoefBufX,
                unsigned char* pucDownRowSrc,
                unsigned short* pusCoefPixelTableDown,
                unsigned short* ausTmpCoefBufX);

        void V_RESIZE_3C(
                int j,
                unsigned short* pusCoefPixelTable,
                unsigned short* pusCoefPixelTableDown,
                unsigned short* ausTmpCoefBufY,
                unsigned char* pucDstData);

        void H_RESIZE_3C2L(int j,
                int* piXInSrc,
                unsigned short* pusCoefBufX,
                unsigned char* pucUpRowSrc,
                unsigned char* pucDownRowSrc,
                unsigned short* pusCoefPixelTable,
                unsigned short* pusCoefPixelTableDown,
                unsigned short* ausTmpCoefBufX);

        void ResizeThreeChannelsInterleaved(
                unsigned char *pucSrcData,
                unsigned char *pucDstData,
                int iWidthSrc,
                int iPixelHeightSrc,
                int iWidthDst,
                int iPixelHeightDst);

        void ComputeCoef(
                int iWidthDst,
                int iHeightDst,
                int iWidthSrc,
                int iHeightSrc,
                double dScaleX,
                double dScaleY,
                int *&piXInSrc,
                int *&piYInSrc,
                unsigned short *&pusCoefBufX,
                unsigned short *&pusCoefBufY,
                bool &bPusCoefXMalloc,
                bool& bPusCoefYMalloc,
                bool& bpiXInSrcMalloc,
                bool& bpiYInSrcMalloc);

        void ResizeThreeChannelsInterleavedZoom(
                unsigned char *pucSrcData,
                unsigned char *pucDstData,
                int iWidthSrc,
                int iPixelHeightSrc,
                int iWidthDst,
                int iPixelHeightDst);
        unsigned short saturate_cast_unshort(float v)
        {
            int tmp = (int)lrintf(v);
            return (unsigned short)((unsigned)tmp <= 65535 ? tmp : tmp > 0 ? 65535 : 0);
        }
        int easy_floor(float fValue)
        {
            int i = (int)(fValue);
            return (i - (i > fValue));
        }
};
#endif
