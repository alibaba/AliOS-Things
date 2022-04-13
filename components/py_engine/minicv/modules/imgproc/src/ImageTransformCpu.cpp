
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"
#include "ImageTransformCpu.h"

#define GetRowSrcInfo(i, iRowInSrc, iOffetSrc, piYInSrc, iStepSrc, iPixelHeightSrc, pucSrcData, pucUpRowSrc, pucDownRowSrc, pusCoefBufY, ausTmpCoefBufY)\
    iRowInSrc = piYInSrc[i];\
    iOffetSrc = iRowInSrc*iStepSrc;\
    pucUpRowSrc = pucSrcData + iOffetSrc;\
    pucDownRowSrc = pucUpRowSrc + iStepSrc;\
    if(iRowInSrc>=(iPixelHeightSrc-1))\
        pucDownRowSrc = pucUpRowSrc;\
    else if(iRowInSrc<0)\
        pucUpRowSrc = pucDownRowSrc;\
    ausTmpCoefBufY[0] = pusCoefBufY[((i)<<1)];\
    ausTmpCoefBufY[1] = pusCoefBufY[((i)<<1)+1];

ImageResizeCpu::ImageResizeCpu() {}

ImageResizeCpu::~ImageResizeCpu() {}


void ImageResizeCpu::H_RESIZE_3C1L(int j, int* piXInSrc, unsigned short* pusCoefBufX, unsigned char* pucDownRowSrc,
                          unsigned short* pusCoefPixelTableDown, unsigned short* ausTmpCoefBufX)
{
    int iColInSrc = piXInSrc[((j)<<1)], iNextColInSrc = piXInSrc[((j)<<1)+1];
    ausTmpCoefBufX[0] = (pusCoefBufX[((j)<<1)]); ausTmpCoefBufX[1] = (pusCoefBufX[((j)<<1)+1]);
    int iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc] * ausTmpCoefBufX[1];
    pusCoefPixelTableDown[3 * j] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);
    iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc + 1] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc + 1] * ausTmpCoefBufX[1];
    pusCoefPixelTableDown[3 * j + 1] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);
    iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc + 2] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc + 2] * ausTmpCoefBufX[1];
    pusCoefPixelTableDown[3 * j + 2] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);
}

void ImageResizeCpu::V_RESIZE_3C(int j, unsigned short* pusCoefPixelTable, unsigned short* pusCoefPixelTableDown, unsigned short* ausTmpCoefBufY, unsigned char* pucDstData)
{
    int iInterpValHorizon1 = pusCoefPixelTable[3*j], iInterpValHorizon2 = pusCoefPixelTableDown[3*j];
    pucDstData[3*j] = (((ausTmpCoefBufY[0] * (iInterpValHorizon1))>>RESIZE_RIGHT_SHIFT_SCALE2) + ((ausTmpCoefBufY[1] * (iInterpValHorizon2))>>RESIZE_RIGHT_SHIFT_SCALE2)+2) >> 2;
    iInterpValHorizon1 = pusCoefPixelTable[3*j+1]; iInterpValHorizon2 = pusCoefPixelTableDown[3*j+1];
    pucDstData[3*j+1] = (((ausTmpCoefBufY[0] * (iInterpValHorizon1))>>RESIZE_RIGHT_SHIFT_SCALE2) + ((ausTmpCoefBufY[1] * (iInterpValHorizon2))>>RESIZE_RIGHT_SHIFT_SCALE2)+2) >> 2;
    iInterpValHorizon1 = pusCoefPixelTable[3*j+2]; iInterpValHorizon2 = pusCoefPixelTableDown[3*j+2];
    pucDstData[3*j+2] = (((ausTmpCoefBufY[0] * (iInterpValHorizon1))>>RESIZE_RIGHT_SHIFT_SCALE2) + ((ausTmpCoefBufY[1] * (iInterpValHorizon2))>>RESIZE_RIGHT_SHIFT_SCALE2)+2) >> 2;
}

void ImageResizeCpu::H_RESIZE_3C2L(int j, int* piXInSrc, unsigned short* pusCoefBufX, unsigned char* pucUpRowSrc, unsigned char* pucDownRowSrc,
        unsigned short* pusCoefPixelTable, unsigned short* pusCoefPixelTableDown, unsigned short* ausTmpCoefBufX)
{
    int iColInSrc = piXInSrc[((j)<<1)], iNextColInSrc = piXInSrc[((j)<<1) + 1];
    ausTmpCoefBufX[0] = (pusCoefBufX[((j)<<1)]); ausTmpCoefBufX[1] = (pusCoefBufX[((j)<<1) + 1]);

    int iInterpValHorizon1 = pucUpRowSrc[3 * iColInSrc] * ausTmpCoefBufX[0] + pucUpRowSrc[3 * iNextColInSrc] * ausTmpCoefBufX[1];
    int iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc] * ausTmpCoefBufX[1];
    pusCoefPixelTable[3 * j] = (iInterpValHorizon1 >> RESIZE_RIGHT_SHIFT_SCALE1);
    pusCoefPixelTableDown[3 * j] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);

    iInterpValHorizon1 = pucUpRowSrc[3 * iColInSrc + 1] * ausTmpCoefBufX[0] + pucUpRowSrc[3 * iNextColInSrc + 1] * ausTmpCoefBufX[1];
    iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc + 1] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc + 1] * ausTmpCoefBufX[1];
    pusCoefPixelTable[3 * j + 1] = (iInterpValHorizon1 >> RESIZE_RIGHT_SHIFT_SCALE1);
    pusCoefPixelTableDown[3 * j + 1] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);

    iInterpValHorizon1 = pucUpRowSrc[3 * iColInSrc + 2] * ausTmpCoefBufX[0] + pucUpRowSrc[3 * iNextColInSrc + 2] * ausTmpCoefBufX[1];
    iInterpValHorizon2 = pucDownRowSrc[3 * iColInSrc + 2] * ausTmpCoefBufX[0] + pucDownRowSrc[3 * iNextColInSrc + 2] * ausTmpCoefBufX[1];
    pusCoefPixelTable[3 * j + 2] = (iInterpValHorizon1 >> RESIZE_RIGHT_SHIFT_SCALE1);
    pusCoefPixelTableDown[3 * j + 2] = (iInterpValHorizon2 >> RESIZE_RIGHT_SHIFT_SCALE1);
}

void ImageResizeCpu::ResizeThreeChannelsInterleaved(unsigned char *pucSrcData, unsigned char *pucDstData, int iWidthSrc,
        int iPixelHeightSrc, int iWidthDst, int iPixelHeightDst)
{
    if(iWidthSrc == iWidthDst && iPixelHeightDst == iPixelHeightSrc)
    {
        memcpy(pucDstData, pucSrcData, iWidthDst*iPixelHeightDst*3);
        return;
    }
    double dScaleX = (double)iWidthDst/iWidthSrc;
    double dScaleY = (double)iPixelHeightDst/iPixelHeightSrc;

    double dInvScaleX = 1. / dScaleX;
    double dInvScaleY = 1. / dScaleY;

    int iStepDst = iWidthDst*3;
    int iStepSrc = iWidthSrc*3;

    // for the (x,y) in pstDstImage image, the following two store the corresponding x/y coordinate in pstSrcImage image
    int *piXInSrc = new int[2*iWidthDst];
    int *piYInSrc = new int[iPixelHeightDst];
    unsigned short *pusCoefBufX = new unsigned short[iWidthDst*2];
    unsigned short *pusCoefBufY = new unsigned short[iPixelHeightDst*2];
    bool bPusCoefXMalloc, bPusCoefYMalloc, bpiXInSrcMalloc, bpiYInSrcMalloc;
    /*** compute the bilinear coefficient on the horizontal and vertical direction **/
    ComputeCoef(iWidthDst, iPixelHeightDst, iWidthSrc, iPixelHeightSrc, dInvScaleX, dInvScaleY, piXInSrc, piYInSrc, pusCoefBufX, pusCoefBufY,
            bPusCoefXMalloc, bPusCoefYMalloc, bpiXInSrcMalloc, bpiYInSrcMalloc);

    /** coef table **/
    unsigned short* pusCoefPixelTable = (unsigned short*)malloc(iWidthDst*sizeof(unsigned short)*6);
    unsigned short* pusCoefPixelTableDown = pusCoefPixelTable + 3*iWidthDst;

    unsigned short ausTmpCoefBufX[2];
    unsigned short ausTmpCoefBufY[2];
    uint8_t* pucUpRowSrc = nullptr, *pucDownRowSrc = nullptr;

    int iRowInSrc = 0, iOffetSrc;

    for( int i = 0; i < iPixelHeightDst; ++i, pucDstData+=iStepDst)
    {
        GetRowSrcInfo(i, iRowInSrc, iOffetSrc, piYInSrc, iStepSrc, iPixelHeightSrc, pucSrcData, pucUpRowSrc, pucDownRowSrc, pusCoefBufY, ausTmpCoefBufY);

        {
            int j = 0;
            for (; j <= iWidthDst-4; j+=4)
            {
                H_RESIZE_3C2L(j, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+1, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+2, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+3, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
            }
            for (; j < iWidthDst; j++)
            {
                H_RESIZE_3C2L(j, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
            }
        }
        int j = 0;
#ifdef _ALIECV_OPT_NEON
        j = ResizeLinear_neon(pucDstData, iWidthDst*3, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY[0], ausTmpCoefBufY[1]);
        j /= 3;
#else
        for (; j <= iWidthDst-4; j+=4)
        {
            V_RESIZE_3C(j, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+1, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+2, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+3, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
        }
#endif
        for (; j < iWidthDst; ++j)
        {
            V_RESIZE_3C(j, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
        }
    }

    delete[] piXInSrc;
    delete[] piYInSrc;
    delete[] pusCoefBufX;
    delete[] pusCoefBufY;
    free(pusCoefPixelTable);
}

/** compute coef, save the next col index in src, so piXInSrc need more mem then before **/
void ImageResizeCpu::ComputeCoef(int iWidthDst, int iHeightDst, int iWidthSrc, int iHeightSrc, double dScaleX, double dScaleY, int *&piXInSrc, int *&piYInSrc,
        unsigned short *&pusCoefBufX, unsigned short *&pusCoefBufY,
        bool &bPusCoefXMalloc, bool& bPusCoefYMalloc, bool& bpiXInSrcMalloc, bool& bpiYInSrcMalloc)
{
    if(pusCoefBufX == nullptr)
    {
        bPusCoefXMalloc = true;
        pusCoefBufX = (unsigned short*)malloc(iWidthDst*sizeof(unsigned short)*2);
    }
    if(pusCoefBufY == nullptr)
    {
        bPusCoefYMalloc = true;
        pusCoefBufY = (unsigned short*)malloc(iHeightDst*sizeof(unsigned short)*2);
    }
    if(piXInSrc == nullptr)
    {
        bpiXInSrcMalloc = true;
        piXInSrc = (int*)malloc(2*iWidthDst*sizeof(int));
    }
    if(piYInSrc == nullptr)
    {
        bpiYInSrcMalloc = true;
        piYInSrc = (int*)malloc(iHeightDst*sizeof(int));
    }
    for (int i = 0; i < iWidthDst; ++i)
    {
        float fx = (float)((i + 0.5) * dScaleX - 0.5);
        int iColInSrc = easy_floor(fx), iNextColInSrc = 0;
        fx -= iColInSrc;
        iNextColInSrc = iColInSrc + 1;
        if(iColInSrc<0)
        {
            fx = 0;
            iColInSrc = 0;
            iNextColInSrc = iColInSrc + 1;
        }
        else if(iColInSrc >= iWidthSrc-1)
        {
            fx = 0;
            iColInSrc = iWidthSrc-1;
            iNextColInSrc = iColInSrc;
        }
        piXInSrc[2*i] = iColInSrc;
        piXInSrc[2*i+1] = iNextColInSrc;

        pusCoefBufX[i*2] = saturate_cast_unshort((1.f-fx)*RESIZE_LEFT_SHIFT_SCALE);
        pusCoefBufX[i*2+1] = saturate_cast_unshort(fx*RESIZE_LEFT_SHIFT_SCALE);
    }
    for (int i = 0; i < iHeightDst; ++i)
    {
        float fy = (float)((i + 0.5) * dScaleY - 0.5);
        int iRowInSrc = easy_floor(fy);
        fy -= iRowInSrc;
        piYInSrc[i] = iRowInSrc;

        pusCoefBufY[i*2] = saturate_cast_unshort((1.f - fy) * RESIZE_LEFT_SHIFT_SCALE);
        pusCoefBufY[i*2+1] = saturate_cast_unshort(fy*RESIZE_LEFT_SHIFT_SCALE);
    }
}

void ImageResizeCpu::ResizeThreeChannelsInterleavedZoom(unsigned char *pucSrcData, unsigned char *pucDstData, int iWidthSrc,
        int iPixelHeightSrc, int iWidthDst, int iPixelHeightDst)
{
    if(iWidthSrc == iWidthDst && iPixelHeightDst == iPixelHeightSrc)
    {
        memcpy(pucDstData, pucSrcData, iWidthDst*iPixelHeightDst*3);
        return;
    }
    double dScaleX = (double)iWidthDst/iWidthSrc;
    double dScaleY = (double)iPixelHeightDst/iPixelHeightSrc;

    double dInvScaleX = 1. / dScaleX;
    double dInvScaleY = 1. / dScaleY;

    int iStepDst = iWidthDst*3;
    int iStepSrc = iWidthSrc*3;

    // for the (x,y) in pstDstImage image, the following two store the corresponding x/y coordinate in pstSrcImage image
    int *piXInSrc = new int[2*iWidthDst];
    int *piYInSrc = new int[iPixelHeightDst];
    unsigned short *pusCoefBufX = new unsigned short[iWidthDst*2];
    unsigned short *pusCoefBufY = new unsigned short[iPixelHeightDst*2];
    bool bPusCoefXMalloc, bPusCoefYMalloc, bpiXInSrcMalloc, bpiYInSrcMalloc;
    /*** compute the bilinear coefficient on the horizontal and vertical direction **/
    ComputeCoef(iWidthDst, iPixelHeightDst, iWidthSrc, iPixelHeightSrc, dInvScaleX, dInvScaleY, piXInSrc, piYInSrc, pusCoefBufX, pusCoefBufY,
            bPusCoefXMalloc, bPusCoefYMalloc, bpiXInSrcMalloc, bpiYInSrcMalloc);

    /** coef table **/
    unsigned short* pusCoefPixelTable = (unsigned short*)malloc(iWidthDst*sizeof(unsigned short)*6);
    unsigned short* pusCoefPixelTableDown = pusCoefPixelTable + 3*iWidthDst;

    unsigned short ausTmpCoefBufX[2];
    unsigned short ausTmpCoefBufY[2];
    uint8_t* pucUpRowSrc = nullptr, *pucDownRowSrc = nullptr;

    int iLastRowInSrc = 0x7FFFFFFF;
    bool bTableNeedWritten = false, bFirstLine = true;
    int iRowInSrc = 0, iOffetSrc;

    for( int i = 0; i < iPixelHeightDst; ++i, pucDstData+=iStepDst)
    {
        GetRowSrcInfo(i, iRowInSrc, iOffetSrc, piYInSrc, iStepSrc, iPixelHeightSrc, pucSrcData, pucUpRowSrc, pucDownRowSrc, pusCoefBufY, ausTmpCoefBufY);

        if(iLastRowInSrc == iRowInSrc)
            bTableNeedWritten = false;
        else
            bTableNeedWritten = true;

        if(bFirstLine && bTableNeedWritten)
        {
            bFirstLine = false;
            int j = 0;
            for (; j <= iWidthDst-4; j+=4)
            {
                H_RESIZE_3C2L(j, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+1, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+2, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C2L(j+3, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
            }
            for (; j < iWidthDst; j++)
            {
                H_RESIZE_3C2L(j, piXInSrc, pusCoefBufX, pucUpRowSrc, pucDownRowSrc, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufX);
            }

        }
        else if(bTableNeedWritten)
        {
            bFirstLine = false;
            int j = 0;
            memcpy(pusCoefPixelTable, pusCoefPixelTableDown, iWidthDst*sizeof(unsigned short)*3);
            for (; j <= iWidthDst-4; j+=4)
            {
                H_RESIZE_3C1L(j, piXInSrc, pusCoefBufX, pucDownRowSrc, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C1L(j+1, piXInSrc, pusCoefBufX, pucDownRowSrc, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C1L(j+2, piXInSrc, pusCoefBufX, pucDownRowSrc, pusCoefPixelTableDown, ausTmpCoefBufX);
                H_RESIZE_3C1L(j+3, piXInSrc, pusCoefBufX, pucDownRowSrc, pusCoefPixelTableDown, ausTmpCoefBufX);
            }
            for (; j < iWidthDst; j++)
            {
                H_RESIZE_3C1L(j, piXInSrc, pusCoefBufX, pucDownRowSrc, pusCoefPixelTableDown, ausTmpCoefBufX);
            }
        }
        int j = 0;
#ifdef _ALIECV_OPT_NEON
        j = ResizeLinear_neon(pucDstData, iWidthDst*3, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY[0], ausTmpCoefBufY[1]);
        j /= 3;
#else
        for (; j <= iWidthDst-4; j+=4)
        {
            V_RESIZE_3C(j, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+1, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+2, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
            V_RESIZE_3C(j+3, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
        }
#endif
        for (; j < iWidthDst; ++j)
        {
            V_RESIZE_3C(j, pusCoefPixelTable, pusCoefPixelTableDown, ausTmpCoefBufY, pucDstData);
        }
        iLastRowInSrc = iRowInSrc;
    }

    delete[] piXInSrc;
    delete[] piYInSrc;
    delete[] pusCoefBufX;
    delete[] pusCoefBufY;
    free(pusCoefPixelTable);
}

void ImageResizeCpu::ResizeRGBInterleaved(unsigned char *pucSrcData, unsigned char *pucDstData, int iWidthSrc, int iPixelHeightSrc, int iWidthDst, int iPixelHeightDst)
{
    LOG_I("%s:xxxentern cpu\n", __func__);
    if(iPixelHeightDst==iPixelHeightSrc && iWidthDst==iWidthSrc)
    {
        memcpy(pucDstData, pucSrcData, iPixelHeightSrc*iWidthSrc*3*sizeof(unsigned char));
        return;
    }
    if(/*iWidthDst > iWidthSrc && */iPixelHeightDst > iPixelHeightSrc)
        ResizeThreeChannelsInterleavedZoom(pucSrcData, pucDstData, iWidthSrc, iPixelHeightSrc, iWidthDst, iPixelHeightDst);
    else
        ResizeThreeChannelsInterleaved(pucSrcData, pucDstData, iWidthSrc, iPixelHeightSrc, iWidthDst, iPixelHeightDst);
    LOG_I("%s:out cpu\n", __func__);
}
