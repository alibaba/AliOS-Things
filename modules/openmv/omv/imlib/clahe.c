/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Contrast Limited Adaptive Histogram Equalization.
 */
#include "imlib.h"
#define BYTE_IMAGE

/*
 * ANSI C code from the article
 * "Contrast Limited Adaptive Histogram Equalization"
 * by Karel Zuiderveld, karel@cv.ruu.nl
 * in "Graphics Gems IV", Academic Press, 1994
 *
 *
 *  These functions implement Contrast Limited Adaptive Histogram Equalization.
 *  The main routine (CLAHE) expects an input image that is stored contiguously in
 *  memory;  the CLAHE output image overwrites the original input image and has the
 *  same minimum and maximum values (which must be provided by the user).
 *  This implementation assumes that the X- and Y image resolutions are an integer
 *  multiple of the X- and Y sizes of the contextual regions. A check on various other
 *  error conditions is performed.
 *
 *  #define the symbol BYTE_IMAGE to make this implementation suitable for
 *  8-bit images. The maximum number of contextual regions can be redefined
 *  by changing uiMAX_REG_X and/or uiMAX_REG_Y; the use of more than 256
 *  contextual regions is not recommended.
 *
 *  The code is ANSI-C and is also C++ compliant.
 *
 *  Author: Karel Zuiderveld, Computer Vision Research Group,
 *           Utrecht, The Netherlands (karel@cv.ruu.nl)
 */

#ifdef BYTE_IMAGE
typedef unsigned char kz_pixel_t;        /* for 8 bit-per-pixel images */
#define uiNR_OF_GREY (256)
#else
typedef unsigned short kz_pixel_t;       /* for 12 bit-per-pixel images (default) */
# define uiNR_OF_GREY (4096)
#endif

/******** Prototype of CLAHE function. Put this in a separate include file. *****/
int CLAHE(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min,
          kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
          unsigned int uiNrBins, float fCliplimit);

/*********************** Local prototypes ************************/
static void ClipHistogram (unsigned long*, unsigned int, unsigned long);
static void MakeHistogram (kz_pixel_t*, unsigned int, unsigned int, unsigned int,
                           unsigned long*, unsigned int, kz_pixel_t*);
static void MapHistogram (unsigned long*, kz_pixel_t, kz_pixel_t,
                          unsigned int, unsigned long);
static void MakeLut (kz_pixel_t*, kz_pixel_t, kz_pixel_t, unsigned int);
static void Interpolate (kz_pixel_t*, int, unsigned long*, unsigned long*,
                         unsigned long*, unsigned long*, unsigned int, unsigned int, kz_pixel_t*);

/**************  Start of actual code **************/
const unsigned int uiMAX_REG_X = 16;      /* max. # contextual regions in x-direction */
const unsigned int uiMAX_REG_Y = 16;      /* max. # contextual regions in y-direction */

/************************** main function CLAHE ******************/
int CLAHE (kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes,
           kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
           unsigned int uiNrBins, float fCliplimit)
/*   pImage - Pointer to the input/output image
 *   uiXRes - Image resolution in the X direction
 *   uiYRes - Image resolution in the Y direction
 *   Min - Minimum greyvalue of input image (also becomes minimum of output image)
 *   Max - Maximum greyvalue of input image (also becomes maximum of output image)
 *   uiNrX - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)
 *   uiNrY - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)
 *   uiNrBins - Number of greybins for histogram ("dynamic range")
 *   float fCliplimit - Normalized cliplimit (higher values give more contrast)
 * The number of "effective" greylevels in the output image is set by uiNrBins; selecting
 * a small value (eg. 128) speeds up processing and still produce an output image of
 * good quality. The output image will have the same minimum and maximum value as the input
 * image. A clip limit smaller than 1 results in standard (non-contrast limited) AHE.
 */
{
    unsigned int uiX, uiY;                /* counters */
    unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */
    unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */
    unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */
    kz_pixel_t* pImPointer;               /* pointer to image */
    kz_pixel_t aLUT[uiNR_OF_GREY];        /* lookup table used for scaling of input image */
    unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/
    unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */

    if (uiNrX > uiMAX_REG_X) return -1;   /* # of regions x-direction too large */
    if (uiNrY > uiMAX_REG_Y) return -2;   /* # of regions y-direction too large */
    if (uiXRes % uiNrX) return -3;        /* x-resolution no multiple of uiNrX */
    if (uiYRes % uiNrY) return -4;        /* y-resolution no multiple of uiNrY */
    if (Max >= uiNR_OF_GREY) return -5;   /* maximum too large */
    if (Min >= Max) return -6;            /* minimum equal or larger than maximum */
    if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */
    if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */
    if (uiNrBins == 0) uiNrBins = 128;    /* default value when not specified */

    pulMapArray=(unsigned long *)fb_alloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins, FB_ALLOC_NO_HINT);
    if (pulMapArray == 0) return -8;      /* Not enough memory! (try reducing uiNrBins) */

    uiXSize = uiXRes/uiNrX; uiYSize = uiYRes/uiNrY;  /* Actual size of contextual regions */
    ulNrPixels = (unsigned long)uiXSize * (unsigned long)uiYSize;

    if(fCliplimit > 0.0) {                /* Calculate actual cliplimit  */
       ulClipLimit = (unsigned long) (fCliplimit * (uiXSize * uiYSize) / uiNrBins);
       ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;
    }
    else ulClipLimit = 1UL<<14;           /* Large value, do not clip (AHE) */
    MakeLut(aLUT, Min, Max, uiNrBins);    /* Make lookup table for mapping of greyvalues */
    /* Calculate greylevel mappings for each contextual region */
    for (uiY = 0, pImPointer = pImage; uiY < uiNrY; uiY++) {
        for (uiX = 0; uiX < uiNrX; uiX++, pImPointer += uiXSize) {
            pulHist = &pulMapArray[uiNrBins * (uiY * uiNrX + uiX)];
            MakeHistogram(pImPointer,uiXRes,uiXSize,uiYSize,pulHist,uiNrBins,aLUT);
            ClipHistogram(pulHist, uiNrBins, ulClipLimit);
            MapHistogram(pulHist, Min, Max, uiNrBins, ulNrPixels);
        }
        pImPointer += (uiYSize - 1) * uiXRes;             /* skip lines, set pointer */
    }

    /* Interpolate greylevel mappings to get CLAHE image */
    for (pImPointer = pImage, uiY = 0; uiY <= uiNrY; uiY++) {
        if (uiY == 0) {                                   /* special case: top row */
            uiSubY = uiYSize >> 1;  uiYU = 0; uiYB = 0;
        }
        else {
            if (uiY == uiNrY) {                           /* special case: bottom row */
                uiSubY = (uiYSize+1) >> 1;        uiYU = uiNrY-1;  uiYB = uiYU;
            }
            else {                                        /* default values */
                uiSubY = uiYSize; uiYU = uiY - 1; uiYB = uiYU + 1;
            }
        }
        for (uiX = 0; uiX <= uiNrX; uiX++) {
            if (uiX == 0) {                               /* special case: left column */
                uiSubX = uiXSize >> 1; uiXL = 0; uiXR = 0;
            }
            else {
                if (uiX == uiNrX) {                       /* special case: right column */
                    uiSubX = (uiXSize+1) >> 1;  uiXL = uiNrX - 1; uiXR = uiXL;
                }
                else {                                    /* default values */
                    uiSubX = uiXSize; uiXL = uiX - 1; uiXR = uiXL + 1;
                }
            }

            pulLU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXL)];
            pulRU = &pulMapArray[uiNrBins * (uiYU * uiNrX + uiXR)];
            pulLB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXL)];
            pulRB = &pulMapArray[uiNrBins * (uiYB * uiNrX + uiXR)];
            Interpolate(pImPointer,uiXRes,pulLU,pulRU,pulLB,pulRB,uiSubX,uiSubY,aLUT);
            pImPointer += uiSubX;                         /* set pointer on next matrix */
        }
        pImPointer += (uiSubY - 1) * uiXRes;
    }
    fb_free();                                            /* free space for histograms */
    return 0;                                             /* return status OK */
}

void ClipHistogram (unsigned long* pulHistogram, unsigned int
                    uiNrGreylevels, unsigned long ulClipLimit)
/* This function performs clipping of the histogram and redistribution of bins.
 * The histogram is clipped and the number of excess pixels is counted. Afterwards
 * the excess pixels are equally redistributed across the whole histogram (providing
 * the bin count is smaller than the cliplimit).
 */
{
    unsigned long* pulBinPointer, *pulEndPointer, *pulHisto;
    unsigned long ulNrExcess, ulUpper, ulBinIncr, ulStepSize, i;
    long lBinExcess;

    ulNrExcess = 0;  pulBinPointer = pulHistogram;
    for (i = 0; i < uiNrGreylevels; i++) { /* calculate total number of excess pixels */
        lBinExcess = (long) pulBinPointer[i] - (long) ulClipLimit;
        if (lBinExcess > 0) ulNrExcess += lBinExcess;   /* excess in current bin */
    };

    /* Second part: clip histogram and redistribute excess pixels in each bin */
    ulBinIncr = ulNrExcess / uiNrGreylevels;            /* average binincrement */
    ulUpper =  ulClipLimit - ulBinIncr;    /* Bins larger than ulUpper set to cliplimit */

    for (i = 0; i < uiNrGreylevels; i++) {
      if (pulHistogram[i] > ulClipLimit) pulHistogram[i] = ulClipLimit; /* clip bin */
      else {
          if (pulHistogram[i] > ulUpper) {              /* high bin count */
              ulNrExcess -= pulHistogram[i] - ulUpper; pulHistogram[i]=ulClipLimit;
          }
          else {                                        /* low bin count */
              ulNrExcess -= ulBinIncr; pulHistogram[i] += ulBinIncr;
          }
       }
    }

    while (ulNrExcess) { /* Redistribute remaining excess  */
        pulEndPointer = &pulHistogram[uiNrGreylevels]; pulHisto = pulHistogram;

        while (ulNrExcess && pulHisto < pulEndPointer) {
            ulStepSize = uiNrGreylevels / ulNrExcess;
            if (ulStepSize < 1) ulStepSize = 1;         /* stepsize at least 1 */
            for (pulBinPointer=pulHisto; pulBinPointer < pulEndPointer && ulNrExcess;
                 pulBinPointer += ulStepSize) {
                if (*pulBinPointer < ulClipLimit) {
                    (*pulBinPointer)++;  ulNrExcess--;  /* reduce excess */
                }
            }
            pulHisto++;  /* restart redistributing on other bin location */
        }
    }
}

void MakeHistogram (kz_pixel_t* pImage, unsigned int uiXRes,
                    unsigned int uiSizeX, unsigned int uiSizeY,
                    unsigned long* pulHistogram,
                    unsigned int uiNrGreylevels, kz_pixel_t* pLookupTable)
/* This function classifies the greylevels present in the array image into
 * a greylevel histogram. The pLookupTable specifies the relationship
 * between the greyvalue of the pixel (typically between 0 and 4095) and
 * the corresponding bin in the histogram (usually containing only 128 bins).
 */
{
    kz_pixel_t* pImagePointer;
    unsigned int i;

    for (i = 0; i < uiNrGreylevels; i++) pulHistogram[i] = 0L; /* clear histogram */

    for (i = 0; i < uiSizeY; i++) {
                pImagePointer = &pImage[uiSizeX];
                while (pImage < pImagePointer) pulHistogram[pLookupTable[*pImage++]]++;
                pImagePointer += uiXRes;
                pImage = &pImagePointer[-(int)uiSizeX]; /* go to bdeginning of next row */
    }
}

void MapHistogram (unsigned long* pulHistogram, kz_pixel_t Min, kz_pixel_t Max,
                   unsigned int uiNrGreylevels, unsigned long ulNrOfPixels)
/* This function calculates the equalized lookup table (mapping) by
 * cumulating the input histogram. Note: lookup table is rescaled in range [Min..Max].
 */
{
    unsigned int i;  unsigned long ulSum = 0;
    const float fScale = ((float)(Max - Min)) / ulNrOfPixels;
    const unsigned long ulMin = (unsigned long) Min;

    for (i = 0; i < uiNrGreylevels; i++) {
                ulSum += pulHistogram[i]; pulHistogram[i]=(unsigned long)(ulMin+ulSum*fScale);
                if (pulHistogram[i] > Max) pulHistogram[i] = Max;
    }
}

void MakeLut (kz_pixel_t * pLUT, kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrBins)
/* To speed up histogram clipping, the input image [Min,Max] is scaled down to
 * [0,uiNrBins-1]. This function calculates the LUT.
 */
{
    int i;
    const kz_pixel_t BinSize = (kz_pixel_t) (1 + (Max - Min) / uiNrBins);

    for (i = Min; i <= Max; i++)  pLUT[i] = (i - Min) / BinSize;
}

void Interpolate (kz_pixel_t * pImage, int uiXRes, unsigned long * pulMapLU,
                  unsigned long * pulMapRU, unsigned long * pulMapLB,  unsigned long * pulMapRB,
                  unsigned int uiXSize, unsigned int uiYSize, kz_pixel_t * pLUT)
/* pImage      - pointer to input/output image
 * uiXRes      - resolution of image in x-direction
 * pulMap*     - mappings of greylevels from histograms
 * uiXSize     - uiXSize of image submatrix
 * uiYSize     - uiYSize of image submatrix
 * pLUT        - lookup table containing mapping greyvalues to bins
 * This function calculates the new greylevel assignments of pixels within a submatrix
 * of the image with size uiXSize and uiYSize. This is done by a bilinear interpolation
 * between four different mappings in order to eliminate boundary artifacts.
 * It uses a division; since division is often an expensive operation, I added code to
 * perform a logical shift instead when feasible.
 */
{
    const unsigned int uiIncr = uiXRes-uiXSize; /* Pointer increment after processing row */
    kz_pixel_t GreyValue; unsigned int uiNum = uiXSize*uiYSize; /* Normalization factor */

    unsigned int uiXCoef, uiYCoef, uiXInvCoef, uiYInvCoef, uiShift = 0;

    if (uiNum & (uiNum - 1)) /* If uiNum is not a power of two, use division */
    for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;
         uiYCoef++, uiYInvCoef--,pImage+=uiIncr) {
        for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;
             uiXCoef++, uiXInvCoef--) {
            GreyValue = pLUT[*pImage];             /* get histogram bin value */
            *pImage++ = (kz_pixel_t ) ((uiYInvCoef * (uiXInvCoef*pulMapLU[GreyValue]
                                      + uiXCoef * pulMapRU[GreyValue])
                                      + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue]
                                      + uiXCoef * pulMapRB[GreyValue])) / uiNum);
        }
    }
    else {                   /* avoid the division and use a right shift instead */
        while (uiNum >>= 1) uiShift++;             /* Calculate 2log of uiNum */
        for (uiYCoef = 0, uiYInvCoef = uiYSize; uiYCoef < uiYSize;
             uiYCoef++, uiYInvCoef--,pImage+=uiIncr) {
             for (uiXCoef = 0, uiXInvCoef = uiXSize; uiXCoef < uiXSize;
               uiXCoef++, uiXInvCoef--) {
               GreyValue = pLUT[*pImage];         /* get histogram bin value */
               *pImage++ = (kz_pixel_t)((uiYInvCoef* (uiXInvCoef * pulMapLU[GreyValue]
                                      + uiXCoef * pulMapRU[GreyValue])
                                      + uiYCoef * (uiXInvCoef * pulMapLB[GreyValue]
                                      + uiXCoef * pulMapRB[GreyValue])) >> uiShift);
            }
        }
    }
}

void imlib_clahe_histeq(image_t *img, float clip_limit, image_t *mask)
{
    int xTileSize = IM_MAX(uiMAX_REG_X >> (10 - IM_MIN(IM_LOG2_32(img->w), 10)), 2);
    int yTileSize = IM_MAX(uiMAX_REG_Y >> (10 - IM_MIN(IM_LOG2_32(img->h), 10)), 2);
    int pImageW = img->w + ((img->w % xTileSize) ? (xTileSize - (img->w % xTileSize)) : 0);
    int pImageH = img->h + ((img->h % yTileSize) ? (yTileSize - (img->h % yTileSize)) : 0);
    int xOffset = (pImageW - img->w) / 2;
    int yOffset = (pImageH - img->h) / 2;

    image_t temp;
    temp.w = img->w;
    temp.h = img->h;
    temp.bpp = img->bpp;
    temp.data = fb_alloc0(pImageW * pImageH * sizeof(kz_pixel_t), FB_ALLOC_NO_HINT);

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset,
                        COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x)));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset,
                        IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset,
                        COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x)));
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    CLAHE((kz_pixel_t *) temp.data,
          pImageW, pImageH,
          COLOR_GRAYSCALE_MIN, COLOR_GRAYSCALE_MAX,
          xTileSize, yTileSize,
          COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN + 1,
          clip_limit);

    switch(img->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x,
                        COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset)));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x,
                        IMAGE_GET_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset));
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *clahe_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&temp, y + yOffset);
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) continue;
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x,
                        imlib_yuv_to_rgb(IMAGE_GET_GRAYSCALE_PIXEL_FAST(clahe_row_ptr, x + xOffset),
                                         COLOR_RGB565_TO_U(pixel),
                                         COLOR_RGB565_TO_V(pixel)));
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    fb_free();
}
