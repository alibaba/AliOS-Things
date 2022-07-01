/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Data Matrix Encoding/Decoding Library.
 */
#include <float.h>
#include <stdio.h>
#include "imlib.h"
#ifdef IMLIB_ENABLE_DATAMATRICES
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define perror(str)
#define fprintf(stream, format, ...)
#define free(ptr) ({ umm_free(ptr); })
#define malloc(size) ({ void *_r = umm_malloc(size); if(!_r) fb_alloc_fail(); _r; })
#define realloc(ptr, size) ({ void *_r = umm_realloc((ptr), (size)); if(!_r) fb_alloc_fail(); _r; })
#define calloc(num, item_size) ({ void *_r = umm_calloc((num), (item_size)); if(!_r) fb_alloc_fail(); _r; })
#define assert(expression)
#define sqrt(x) fast_sqrtf(x)
#define sqrtf(x) fast_sqrtf(x)
#define floor(x) fast_floorf(x)
#define floorf(x) fast_floorf(x)
#define ceil(x) fast_ceilf(x)
#define ceilf(x) fast_ceilf(x)
#define round(x) fast_roundf(x)
#define roundf(x) fast_roundf(x)
#define atan(x) fast_atanf(x)
#define atanf(x) fast_atanf(x)
#define atan2(y, x) fast_atan2f((y), (x))
#define atan2f(y, x) fast_atan2f((y), (x))
#define exp(x) fast_expf(x)
#define expf(x) fast_expf(x)
#define cbrt(x) fast_cbrtf(x)
#define cbrtf(x) fast_cbrtf(x)
#define fabs(x) fast_fabsf(x)
#define fabsf(x) fast_fabsf(x)
#define log(x) fast_log(x)
#define logf(x) fast_log(x)
#undef log2
#define log2(x) fast_log2(x)
#undef log2f
#define log2f(x) fast_log2(x)
#define cos(x) cosf(x)
#define sin(x) sinf(x)
#define acos(x) acosf(x)
#define asin(x) asinf(x)

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtx.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtx.h
 * \brief Main libdmtx header
 */

#ifndef M_PI
#define M_PI      3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2    1.57079632679489661923
#endif

#define DmtxVersion              "0.7.5"

#define DmtxUndefined                 -1

#define DmtxPassFail        unsigned int
#define DmtxPass                       1
#define DmtxFail                       0

#define DmtxBoolean         unsigned int
#define DmtxTrue                       1
#define DmtxFalse                      0

#define DmtxFormatMatrix               0
#define DmtxFormatMosaic               1

#define DmtxSymbolSquareCount         24
#define DmtxSymbolRectCount            6

#define DmtxModuleOff               0x00
#define DmtxModuleOnRed             0x01
#define DmtxModuleOnGreen           0x02
#define DmtxModuleOnBlue            0x04
#define DmtxModuleOnRGB             0x07  /* OnRed | OnGreen | OnBlue */
#define DmtxModuleOn                0x07
#define DmtxModuleUnsure            0x08
#define DmtxModuleAssigned          0x10
#define DmtxModuleVisited           0x20
#define DmtxModuleData              0x40

#define DMTX_CHECK_BOUNDS(l,i) (assert((i) >= 0 && (i) < (l)->length && (l)->length <= (l)->capacity))

typedef enum {
   DmtxSchemeAutoFast        = -2,
   DmtxSchemeAutoBest        = -1,
   DmtxSchemeAscii           =  0,
   DmtxSchemeC40,
   DmtxSchemeText,
   DmtxSchemeX12,
   DmtxSchemeEdifact,
   DmtxSchemeBase256
} DmtxScheme;

typedef enum {
   DmtxSymbolRectAuto        = -3,
   DmtxSymbolSquareAuto      = -2,
   DmtxSymbolShapeAuto       = -1,
   DmtxSymbol10x10           =  0,
   DmtxSymbol12x12,
   DmtxSymbol14x14,
   DmtxSymbol16x16,
   DmtxSymbol18x18,
   DmtxSymbol20x20,
   DmtxSymbol22x22,
   DmtxSymbol24x24,
   DmtxSymbol26x26,
   DmtxSymbol32x32,
   DmtxSymbol36x36,
   DmtxSymbol40x40,
   DmtxSymbol44x44,
   DmtxSymbol48x48,
   DmtxSymbol52x52,
   DmtxSymbol64x64,
   DmtxSymbol72x72,
   DmtxSymbol80x80,
   DmtxSymbol88x88,
   DmtxSymbol96x96,
   DmtxSymbol104x104,
   DmtxSymbol120x120,
   DmtxSymbol132x132,
   DmtxSymbol144x144,
   DmtxSymbol8x18,
   DmtxSymbol8x32,
   DmtxSymbol12x26,
   DmtxSymbol12x36,
   DmtxSymbol16x36,
   DmtxSymbol16x48
} DmtxSymbolSize;

typedef enum {
   DmtxDirNone               = 0x00,
   DmtxDirUp                 = 0x01 << 0,
   DmtxDirLeft               = 0x01 << 1,
   DmtxDirDown               = 0x01 << 2,
   DmtxDirRight              = 0x01 << 3,
   DmtxDirHorizontal         = DmtxDirLeft  | DmtxDirRight,
   DmtxDirVertical           = DmtxDirUp    | DmtxDirDown,
   DmtxDirRightUp            = DmtxDirRight | DmtxDirUp,
   DmtxDirLeftDown           = DmtxDirLeft  | DmtxDirDown
} DmtxDirection;

typedef enum {
   DmtxSymAttribSymbolRows,
   DmtxSymAttribSymbolCols,
   DmtxSymAttribDataRegionRows,
   DmtxSymAttribDataRegionCols,
   DmtxSymAttribHorizDataRegions,
   DmtxSymAttribVertDataRegions,
   DmtxSymAttribMappingMatrixRows,
   DmtxSymAttribMappingMatrixCols,
   DmtxSymAttribInterleavedBlocks,
   DmtxSymAttribBlockErrorWords,
   DmtxSymAttribBlockMaxCorrectable,
   DmtxSymAttribSymbolDataWords,
   DmtxSymAttribSymbolErrorWords,
   DmtxSymAttribSymbolMaxCorrectable
} DmtxSymAttribute;

typedef enum {
   /* Encoding properties */
   DmtxPropScheme            = 100,
   DmtxPropSizeRequest,
   DmtxPropMarginSize,
   DmtxPropModuleSize,
   /* Decoding properties */
   DmtxPropEdgeMin           = 200,
   DmtxPropEdgeMax,
   DmtxPropScanGap,
   DmtxPropSquareDevn,
   DmtxPropSymbolSize,
   DmtxPropEdgeThresh,
   /* Image properties */
   DmtxPropWidth             = 300,
   DmtxPropHeight,
   DmtxPropPixelPacking,
   DmtxPropBitsPerPixel,
   DmtxPropBytesPerPixel,
   DmtxPropRowPadBytes,
   DmtxPropRowSizeBytes,
   DmtxPropImageFlip,
   DmtxPropChannelCount,
   /* Image modifiers */
   DmtxPropXmin              = 400,
   DmtxPropXmax,
   DmtxPropYmin,
   DmtxPropYmax,
   DmtxPropScale
} DmtxProperty;

typedef enum {
   /* Custom format */
   DmtxPackCustom            = 100,
   /* 1 bpp */
   DmtxPack1bppK             = 200,
   /* 8 bpp grayscale */
   DmtxPack8bppK             = 300,
   /* 16 bpp formats */
   DmtxPack16bppRGB          = 400,
   DmtxPack16bppRGBX,
   DmtxPack16bppXRGB,
   DmtxPack16bppBGR,
   DmtxPack16bppBGRX,
   DmtxPack16bppXBGR,
   DmtxPack16bppYCbCr,
   /* 24 bpp formats */
   DmtxPack24bppRGB          = 500,
   DmtxPack24bppBGR,
   DmtxPack24bppYCbCr,
   /* 32 bpp formats */
   DmtxPack32bppRGBX         = 600,
   DmtxPack32bppXRGB,
   DmtxPack32bppBGRX,
   DmtxPack32bppXBGR,
   DmtxPack32bppCMYK
} DmtxPackOrder;

typedef enum {
  DmtxFlipNone               = 0x00,
  DmtxFlipX                  = 0x01 << 0,
  DmtxFlipY                  = 0x01 << 1
} DmtxFlip;

typedef float DmtxMatrix3[3][3];

/**
 * @struct DmtxPixelLoc
 * @brief DmtxPixelLoc
 */
typedef struct DmtxPixelLoc_struct {
   int X;
   int Y;
} DmtxPixelLoc;

/**
 * @struct DmtxVector2
 * @brief DmtxVector2
 */
typedef struct DmtxVector2_struct {
   float          X;
   float          Y;
} DmtxVector2;

/**
 * @struct DmtxRay2
 * @brief DmtxRay2
 */
typedef struct DmtxRay2_struct {
   float          tMin;
   float          tMax;
   DmtxVector2     p;
   DmtxVector2     v;
} DmtxRay2;

typedef unsigned char DmtxByte;

/**
 * @struct DmtxByteList
 * @brief DmtxByteList
 * Use signed int for length fields instead of size_t to play nicely with RS
 * arithmetic
 */
typedef struct DmtxByteList_struct DmtxByteList;
struct DmtxByteList_struct
{
   int length;
   int capacity;
   DmtxByte *b;
};

/**
 * @struct DmtxImage
 * @brief DmtxImage
 */
typedef struct DmtxImage_struct {
   int             width;
   int             height;
   int             pixelPacking;
   int             bitsPerPixel;
   int             bytesPerPixel;
   int             rowPadBytes;
   int             rowSizeBytes;
   int             imageFlip;
   int             channelCount;
   int             channelStart[4];
   int             bitsPerChannel[4];
   unsigned char  *pxl;
} DmtxImage;

/**
 * @struct DmtxPointFlow
 * @brief DmtxPointFlow
 */
typedef struct DmtxPointFlow_struct {
   int             plane;
   int             arrive;
   int             depart;
   int             mag;
   DmtxPixelLoc    loc;
} DmtxPointFlow;

/**
 * @struct DmtxBestLine
 * @brief DmtxBestLine
 */
typedef struct DmtxBestLine_struct {
   int             angle;
   int             hOffset;
   int             mag;
   int             stepBeg;
   int             stepPos;
   int             stepNeg;
   int             distSq;
   float          devn;
   DmtxPixelLoc    locBeg;
   DmtxPixelLoc    locPos;
   DmtxPixelLoc    locNeg;
} DmtxBestLine;

/**
 * @struct DmtxRegion
 * @brief DmtxRegion
 */
typedef struct DmtxRegion_struct {

   /* Trail blazing values */
   int             jumpToPos;     /* */
   int             jumpToNeg;     /* */
   int             stepsTotal;    /* */
   DmtxPixelLoc    finalPos;      /* */
   DmtxPixelLoc    finalNeg;      /* */
   DmtxPixelLoc    boundMin;      /* */
   DmtxPixelLoc    boundMax;      /* */
   DmtxPointFlow   flowBegin;     /* */

   /* Orientation values */
   int             polarity;      /* */
   int             stepR;
   int             stepT;
   DmtxPixelLoc    locR;          /* remove if stepR works above */
   DmtxPixelLoc    locT;          /* remove if stepT works above */

   /* Region fitting values */
   int             leftKnown;     /* known == 1; unknown == 0 */
   int             leftAngle;     /* hough angle of left edge */
   DmtxPixelLoc    leftLoc;       /* known (arbitrary) location on left edge */
   DmtxBestLine    leftLine;      /* */
   int             bottomKnown;   /* known == 1; unknown == 0 */
   int             bottomAngle;   /* hough angle of bottom edge */
   DmtxPixelLoc    bottomLoc;     /* known (arbitrary) location on bottom edge */
   DmtxBestLine    bottomLine;    /* */
   int             topKnown;      /* known == 1; unknown == 0 */
   int             topAngle;      /* hough angle of top edge */
   DmtxPixelLoc    topLoc;        /* known (arbitrary) location on top edge */
   int             rightKnown;    /* known == 1; unknown == 0 */
   int             rightAngle;    /* hough angle of right edge */
   DmtxPixelLoc    rightLoc;      /* known (arbitrary) location on right edge */

   /* Region calibration values */
   int             onColor;       /* */
   int             offColor;      /* */
   int             sizeIdx;       /* Index of arrays that store Data Matrix constants */
   int             symbolRows;    /* Number of total rows in symbol including alignment patterns */
   int             symbolCols;    /* Number of total columns in symbol including alignment patterns */
   int             mappingRows;   /* Number of data rows in symbol */
   int             mappingCols;   /* Number of data columns in symbol */

   /* Transform values */
   DmtxMatrix3     raw2fit;       /* 3x3 transformation from raw image to fitted barcode grid */
   DmtxMatrix3     fit2raw;       /* 3x3 transformation from fitted barcode grid to raw image */
} DmtxRegion;

/**
 * @struct DmtxMessage
 * @brief DmtxMessage
 */
typedef struct DmtxMessage_struct {
   size_t          arraySize;     /* mappingRows * mappingCols */
   size_t          codeSize;      /* Size of encoded data (data words + error words) */
   size_t          outputSize;    /* Size of buffer used to hold decoded data */
   int             outputIdx;     /* Internal index used to store output progress */
   int             padCount;
   unsigned char  *array;         /* Pointer to internal representation of Data Matrix modules */
   unsigned char  *code;          /* Pointer to internal storage of code words (data and error) */
   unsigned char  *output;        /* Pointer to internal storage of decoded output */
} DmtxMessage;

/**
 * @struct DmtxScanGrid
 * @brief DmtxScanGrid
 */
typedef struct DmtxScanGrid_struct {
   /* set once */
   int             minExtent;     /* Smallest cross size used in scan */
   int             maxExtent;     /* Size of bounding grid region (2^N - 1) */
   int             xOffset;       /* Offset to obtain image X coordinate */
   int             yOffset;       /* Offset to obtain image Y coordinate */
   int             xMin;          /* Minimum X in image coordinate system */
   int             xMax;          /* Maximum X in image coordinate system */
   int             yMin;          /* Minimum Y in image coordinate system */
   int             yMax;          /* Maximum Y in image coordinate system */

   /* reset for each level */
   int             total;         /* Total number of crosses at this size */
   int             extent;        /* Length/width of cross in pixels */
   int             jumpSize;      /* Distance in pixels between cross centers */
   int             pixelTotal;    /* Total pixel count within an individual cross path */
   int             startPos;      /* X and Y coordinate of first cross center in pattern */

   /* reset for each cross */
   int             pixelCount;    /* Progress (pixel count) within current cross pattern */
   int             xCenter;       /* X center of current cross pattern */
   int             yCenter;       /* Y center of current cross pattern */
} DmtxScanGrid;

/**
 * @struct DmtxDecode
 * @brief DmtxDecode
 */
typedef struct DmtxDecode_struct {
   /* Options */
   int             edgeMin;
   int             edgeMax;
   int             scanGap;
   float          squareDevn;
   int             sizeIdxExpected;
   int             edgeThresh;

   /* Image modifiers */
   int             xMin;
   int             xMax;
   int             yMin;
   int             yMax;
   int             scale;

   /* Internals */
/* int             cacheComplete; */
   unsigned char  *cache;
   DmtxImage      *image;
   DmtxScanGrid    grid;
} DmtxDecode;

/* dmtxdecode.c */
extern DmtxDecode *dmtxDecodeCreate(DmtxImage *img, int scale);
extern DmtxPassFail dmtxDecodeDestroy(DmtxDecode **dec);
extern DmtxPassFail dmtxDecodeSetProp(DmtxDecode *dec, int prop, int value);
extern int dmtxDecodeGetProp(DmtxDecode *dec, int prop);
extern /*@exposed@*/ unsigned char *dmtxDecodeGetCache(DmtxDecode *dec, int x, int y);
extern DmtxPassFail dmtxDecodeGetPixelValue(DmtxDecode *dec, int x, int y, int channel, /*@out@*/ int *value);
extern DmtxMessage *dmtxDecodeMatrixRegion(DmtxDecode *dec, DmtxRegion *reg, int fix);
extern DmtxMessage *dmtxDecodeMosaicRegion(DmtxDecode *dec, DmtxRegion *reg, int fix);

/* dmtxregion.c */
extern DmtxRegion *dmtxRegionCreate(DmtxRegion *reg);
extern DmtxPassFail dmtxRegionDestroy(DmtxRegion **reg);
extern DmtxRegion *dmtxRegionFindNext(DmtxDecode *dec, int max_iterations, int *current_iterations);
extern DmtxRegion *dmtxRegionScanPixel(DmtxDecode *dec, int x, int y);
extern DmtxPassFail dmtxRegionUpdateCorners(DmtxDecode *dec, DmtxRegion *reg, DmtxVector2 p00,
      DmtxVector2 p10, DmtxVector2 p11, DmtxVector2 p01);
extern DmtxPassFail dmtxRegionUpdateXfrms(DmtxDecode *dec, DmtxRegion *reg);

/* dmtxmessage.c */
extern DmtxMessage *dmtxMessageCreate(int sizeIdx, int symbolFormat);
extern DmtxPassFail dmtxMessageDestroy(DmtxMessage **msg);

/* dmtximage.c */
extern DmtxImage *dmtxImageCreate(unsigned char *pxl, int width, int height, int pack);
extern DmtxPassFail dmtxImageDestroy(DmtxImage **img);
extern DmtxPassFail dmtxImageSetChannel(DmtxImage *img, int channelStart, int bitsPerChannel);
extern DmtxPassFail dmtxImageSetProp(DmtxImage *img, int prop, int value);
extern int dmtxImageGetProp(DmtxImage *img, int prop);
extern int dmtxImageGetByteOffset(DmtxImage *img, int x, int y);
extern DmtxPassFail dmtxImageGetPixelValue(DmtxImage *img, int x, int y, int channel, /*@out@*/ int *value);
extern DmtxPassFail dmtxImageSetPixelValue(DmtxImage *img, int x, int y, int channel, int value);
extern DmtxBoolean dmtxImageContainsInt(DmtxImage *img, int margin, int x, int y);
extern DmtxBoolean dmtxImageContainsFloat(DmtxImage *img, float x, float y);

/* dmtxvector2.c */
extern DmtxVector2 *dmtxVector2AddTo(DmtxVector2 *v1, const DmtxVector2 *v2);
extern DmtxVector2 *dmtxVector2Add(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2);
extern DmtxVector2 *dmtxVector2SubFrom(DmtxVector2 *v1, const DmtxVector2 *v2);
extern DmtxVector2 *dmtxVector2Sub(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2);
extern DmtxVector2 *dmtxVector2ScaleBy(DmtxVector2 *v, float s);
extern DmtxVector2 *dmtxVector2Scale(/*@out@*/ DmtxVector2 *vOut, const DmtxVector2 *v, float s);
extern float dmtxVector2Cross(const DmtxVector2 *v1, const DmtxVector2 *v2);
extern float dmtxVector2Norm(DmtxVector2 *v);
extern float dmtxVector2Dot(const DmtxVector2 *v1, const DmtxVector2 *v2);
extern float dmtxVector2Mag(const DmtxVector2 *v);
extern float dmtxDistanceFromRay2(const DmtxRay2 *r, const DmtxVector2 *q);
extern float dmtxDistanceAlongRay2(const DmtxRay2 *r, const DmtxVector2 *q);
extern DmtxPassFail dmtxRay2Intersect(/*@out@*/ DmtxVector2 *point, const DmtxRay2 *p0, const DmtxRay2 *p1);
extern DmtxPassFail dmtxPointAlongRay2(/*@out@*/ DmtxVector2 *point, const DmtxRay2 *r, float t);

/* dmtxmatrix3.c */
extern void dmtxMatrix3Copy(/*@out@*/ DmtxMatrix3 m0, DmtxMatrix3 m1);
extern void dmtxMatrix3Identity(/*@out@*/ DmtxMatrix3 m);
extern void dmtxMatrix3Translate(/*@out@*/ DmtxMatrix3 m, float tx, float ty);
extern void dmtxMatrix3Rotate(/*@out@*/ DmtxMatrix3 m, float angle);
extern void dmtxMatrix3Scale(/*@out@*/ DmtxMatrix3 m, float sx, float sy);
extern void dmtxMatrix3Shear(/*@out@*/ DmtxMatrix3 m, float shx, float shy);
extern void dmtxMatrix3LineSkewTop(/*@out@*/ DmtxMatrix3 m, float b0, float b1, float sz);
extern void dmtxMatrix3LineSkewTopInv(/*@out@*/ DmtxMatrix3 m, float b0, float b1, float sz);
extern void dmtxMatrix3LineSkewSide(/*@out@*/ DmtxMatrix3 m, float b0, float b1, float sz);
extern void dmtxMatrix3LineSkewSideInv(/*@out@*/ DmtxMatrix3 m, float b0, float b1, float sz);
extern void dmtxMatrix3Multiply(/*@out@*/ DmtxMatrix3 mOut, DmtxMatrix3 m0, DmtxMatrix3 m1);
extern void dmtxMatrix3MultiplyBy(DmtxMatrix3 m0, DmtxMatrix3 m1);
extern int dmtxMatrix3VMultiply(/*@out@*/ DmtxVector2 *vOut, DmtxVector2 *vIn, DmtxMatrix3 m);
extern int dmtxMatrix3VMultiplyBy(DmtxVector2 *v, DmtxMatrix3 m);
extern void dmtxMatrix3Print(DmtxMatrix3 m);

/* dmtxsymbol.c */
extern int dmtxSymbolModuleStatus(DmtxMessage *mapping, int sizeIdx, int row, int col);
extern int dmtxGetSymbolAttribute(int attribute, int sizeIdx);
extern int dmtxGetBlockDataSize(int sizeIdx, int blockIdx);

/* dmtxbytelist.c */
extern DmtxByteList dmtxByteListBuild(DmtxByte *storage, int capacity);
extern void dmtxByteListInit(DmtxByteList *list, int length, DmtxByte value, DmtxPassFail *passFail);
extern void dmtxByteListClear(DmtxByteList *list);
extern DmtxBoolean dmtxByteListHasCapacity(DmtxByteList *list);
extern void dmtxByteListCopy(DmtxByteList *dst, const DmtxByteList *src, DmtxPassFail *passFail);
extern void dmtxByteListPush(DmtxByteList *list, DmtxByte value, DmtxPassFail *passFail);
extern DmtxByte dmtxByteListPop(DmtxByteList *list, DmtxPassFail *passFail);
extern void dmtxByteListPrint(DmtxByteList *list, char *prefix);

extern char *dmtxVersion(void);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxstatic.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxstatic.h
 * \brief Static header
 */

#define DmtxAlmostZero          0.000001
#define DmtxAlmostInfinity            -1

#define DmtxValueC40Latch            230
#define DmtxValueTextLatch           239
#define DmtxValueX12Latch            238
#define DmtxValueEdifactLatch        240
#define DmtxValueBase256Latch        231

#define DmtxValueCTXUnlatch   254
#define DmtxValueEdifactUnlatch       31

#define DmtxValueAsciiPad            129
#define DmtxValueAsciiUpperShift     235
#define DmtxValueCTXShift1      0
#define DmtxValueCTXShift2      1
#define DmtxValueCTXShift3      2
#define DmtxValueFNC1                232
#define DmtxValueStructuredAppend    233
#define DmtxValue05Macro             236
#define DmtxValue06Macro             237
#define DmtxValueECI                 241

#define DmtxC40TextBasicSet            0
#define DmtxC40TextShift1              1
#define DmtxC40TextShift2              2
#define DmtxC40TextShift3              3

#define DmtxUnlatchExplicit            0
#define DmtxUnlatchImplicit            1

#define DmtxChannelValid            0x00
#define DmtxChannelUnsupportedChar  0x01 << 0
#define DmtxChannelCannotUnlatch    0x01 << 1

#undef min
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))

#undef max
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))

typedef enum {
   DmtxRangeGood,
   DmtxRangeBad,
   DmtxRangeEnd
} DmtxRange;

typedef enum {
   DmtxEdgeTop               = 0x01 << 0,
   DmtxEdgeBottom            = 0x01 << 1,
   DmtxEdgeLeft              = 0x01 << 2,
   DmtxEdgeRight             = 0x01 << 3
} DmtxEdge;

typedef enum {
   DmtxMaskBit8              = 0x01 << 0,
   DmtxMaskBit7              = 0x01 << 1,
   DmtxMaskBit6              = 0x01 << 2,
   DmtxMaskBit5              = 0x01 << 3,
   DmtxMaskBit4              = 0x01 << 4,
   DmtxMaskBit3              = 0x01 << 5,
   DmtxMaskBit2              = 0x01 << 6,
   DmtxMaskBit1              = 0x01 << 7
} DmtxMaskBit;

/**
 * @struct DmtxFollow
 * @brief DmtxFollow
 */
typedef struct DmtxFollow_struct {
   unsigned char  *ptr;
   unsigned char   neighbor;
   int             step;
   DmtxPixelLoc    loc;
} DmtxFollow;

/**
 * @struct DmtxBresLine
 * @brief DmtxBresLine
 */
typedef struct DmtxBresLine_struct {
   int             xStep;
   int             yStep;
   int             xDelta;
   int             yDelta;
   int             steep;
   int             xOut;
   int             yOut;
   int             travel;
   int             outward;
   int             error;
   DmtxPixelLoc    loc;
   DmtxPixelLoc    loc0;
   DmtxPixelLoc    loc1;
} DmtxBresLine;

typedef struct C40TextState_struct {
   int             shift;
   DmtxBoolean     upperShift;
} C40TextState;

/* dmtxregion.c */
static float RightAngleTrueness(DmtxVector2 c0, DmtxVector2 c1, DmtxVector2 c2, float angle);
static DmtxPointFlow MatrixRegionSeekEdge(DmtxDecode *dec, DmtxPixelLoc loc0);
static DmtxPassFail MatrixRegionOrientation(DmtxDecode *dec, DmtxRegion *reg, DmtxPointFlow flowBegin);
static long DistanceSquared(DmtxPixelLoc a, DmtxPixelLoc b);
static int ReadModuleColor(DmtxDecode *dec, DmtxRegion *reg, int symbolRow, int symbolCol, int sizeIdx, int colorPlane);

static DmtxPassFail MatrixRegionFindSize(DmtxDecode *dec, DmtxRegion *reg);
static int CountJumpTally(DmtxDecode *dec, DmtxRegion *reg, int xStart, int yStart, DmtxDirection dir);
static DmtxPointFlow GetPointFlow(DmtxDecode *dec, int colorPlane, DmtxPixelLoc loc, int arrive);
static DmtxPointFlow FindStrongestNeighbor(DmtxDecode *dec, DmtxPointFlow center, int sign);
static DmtxFollow FollowSeek(DmtxDecode *dec, DmtxRegion *reg, int seek);
static DmtxFollow FollowSeekLoc(DmtxDecode *dec, DmtxPixelLoc loc);
static DmtxFollow FollowStep(DmtxDecode *dec, DmtxRegion *reg, DmtxFollow followBeg, int sign);
static DmtxFollow FollowStep2(DmtxDecode *dec, DmtxFollow followBeg, int sign);
static DmtxPassFail TrailBlazeContinuous(DmtxDecode *dec, DmtxRegion *reg, DmtxPointFlow flowBegin, int maxDiagonal);
static int TrailBlazeGapped(DmtxDecode *dec, DmtxRegion *reg, DmtxBresLine line, int streamDir);
static int TrailClear(DmtxDecode *dec, DmtxRegion *reg, int clearMask);
static DmtxBestLine FindBestSolidLine(DmtxDecode *dec, DmtxRegion *reg, int step0, int step1, int streamDir, int houghAvoid);
static DmtxBestLine FindBestSolidLine2(DmtxDecode *dec, DmtxPixelLoc loc0, int tripSteps, int sign, int houghAvoid);
static DmtxPassFail FindTravelLimits(DmtxDecode *dec, DmtxRegion *reg, DmtxBestLine *line);
static DmtxPassFail MatrixRegionAlignCalibEdge(DmtxDecode *dec, DmtxRegion *reg, int whichEdge);
static DmtxBresLine BresLineInit(DmtxPixelLoc loc0, DmtxPixelLoc loc1, DmtxPixelLoc locInside);
static DmtxPassFail BresLineGetStep(DmtxBresLine line, DmtxPixelLoc target, int *travel, int *outward);
static DmtxPassFail BresLineStep(DmtxBresLine *line, int travel, int outward);
/*static void WriteDiagnosticImage(DmtxDecode *dec, DmtxRegion *reg, char *imagePath);*/

/* dmtxdecode.c */
static void TallyModuleJumps(DmtxDecode *dec, DmtxRegion *reg, int tally[][24], int xOrigin, int yOrigin, int mapWidth, int mapHeight, DmtxDirection dir);
static DmtxPassFail PopulateArrayFromMatrix(DmtxDecode *dec, DmtxRegion *reg, DmtxMessage *msg);

/* dmtxdecodescheme.c */
static void DecodeDataStream(DmtxMessage *msg, int sizeIdx, unsigned char *outputStart);
static int GetEncodationScheme(unsigned char cw);
static void PushOutputWord(DmtxMessage *msg, int value);
static void PushOutputC40TextWord(DmtxMessage *msg, C40TextState *state, int value);
static void PushOutputMacroHeader(DmtxMessage *msg, int macroType);
static void PushOutputMacroTrailer(DmtxMessage *msg);
static unsigned char *DecodeSchemeAscii(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd);
static unsigned char *DecodeSchemeC40Text(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd, DmtxScheme encScheme);
static unsigned char *DecodeSchemeX12(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd);
static unsigned char *DecodeSchemeEdifact(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd);
static unsigned char *DecodeSchemeBase256(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd);

/* dmtxplacemod.c */
static int ModulePlacementEcc200(unsigned char *modules, unsigned char *codewords, int sizeIdx, int moduleOnColor);
static void PatternShapeStandard(unsigned char *modules, int mappingRows, int mappingCols, int row, int col, unsigned char *codeword, int moduleOnColor);
static void PatternShapeSpecial1(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor);
static void PatternShapeSpecial2(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor);
static void PatternShapeSpecial3(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor);
static void PatternShapeSpecial4(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor);
static void PlaceModule(unsigned char *modules, int mappingRows, int mappingCols, int row, int col,
      unsigned char *codeword, int mask, int moduleOnColor);

/* dmtxreedsol.c */
static DmtxPassFail RsDecode(unsigned char *code, int sizeIdx, int fix);
static DmtxBoolean RsComputeSyndromes(DmtxByteList *syn, const DmtxByteList *rec, int blockErrorWords);
static DmtxBoolean RsFindErrorLocatorPoly(DmtxByteList *elp, const DmtxByteList *syn, int errorWordCount, int maxCorrectable);
static DmtxBoolean RsFindErrorLocations(DmtxByteList *loc, const DmtxByteList *elp);
static DmtxPassFail RsRepairErrors(DmtxByteList *rec, const DmtxByteList *loc, const DmtxByteList *elp, const DmtxByteList *syn);

/* dmtxscangrid.c */
static DmtxScanGrid InitScanGrid(DmtxDecode *dec);
static int PopGridLocation(DmtxScanGrid *grid, /*@out@*/ DmtxPixelLoc *locPtr);
static int GetGridCoordinates(DmtxScanGrid *grid, /*@out@*/ DmtxPixelLoc *locPtr);
static void SetDerivedFields(DmtxScanGrid *grid);

/* dmtximage.c */
static int GetBitsPerPixel(int pack);

/* dmtxencodebase256.c */
static unsigned char UnRandomize255State(unsigned char value, int idx);

static const int dmtxNeighborNone = 8;
static const int dmtxPatternX[] = { -1,  0,  1,  1,  1,  0, -1, -1 };
static const int dmtxPatternY[] = { -1, -1, -1,  0,  1,  1,  1,  0 };
static const DmtxPointFlow dmtxBlankEdge = { 0, 0, 0, DmtxUndefined, { -1, -1 } };

/*@ +charint @*/

static int rHvX[] =
    {  256,  256,  256,  256,  255,  255,  255,  254,  254,  253,  252,  251,  250,  249,  248,
       247,  246,  245,  243,  242,  241,  239,  237,  236,  234,  232,  230,  228,  226,  224,
       222,  219,  217,  215,  212,  210,  207,  204,  202,  199,  196,  193,  190,  187,  184,
       181,  178,  175,  171,  168,  165,  161,  158,  154,  150,  147,  143,  139,  136,  132,
       128,  124,  120,  116,  112,  108,  104,  100,   96,   92,   88,   83,   79,   75,   71,
        66,   62,   58,   53,   49,   44,   40,   36,   31,   27,   22,   18,   13,    9,    4,
         0,   -4,   -9,  -13,  -18,  -22,  -27,  -31,  -36,  -40,  -44,  -49,  -53,  -58,  -62,
       -66,  -71,  -75,  -79,  -83,  -88,  -92,  -96, -100, -104, -108, -112, -116, -120, -124,
      -128, -132, -136, -139, -143, -147, -150, -154, -158, -161, -165, -168, -171, -175, -178,
      -181, -184, -187, -190, -193, -196, -199, -202, -204, -207, -210, -212, -215, -217, -219,
      -222, -224, -226, -228, -230, -232, -234, -236, -237, -239, -241, -242, -243, -245, -246,
      -247, -248, -249, -250, -251, -252, -253, -254, -254, -255, -255, -255, -256, -256, -256 };

static int rHvY[] =
    {    0,    4,    9,   13,   18,   22,   27,   31,   36,   40,   44,   49,   53,   58,   62,
        66,   71,   75,   79,   83,   88,   92,   96,  100,  104,  108,  112,  116,  120,  124,
       128,  132,  136,  139,  143,  147,  150,  154,  158,  161,  165,  168,  171,  175,  178,
       181,  184,  187,  190,  193,  196,  199,  202,  204,  207,  210,  212,  215,  217,  219,
       222,  224,  226,  228,  230,  232,  234,  236,  237,  239,  241,  242,  243,  245,  246,
       247,  248,  249,  250,  251,  252,  253,  254,  254,  255,  255,  255,  256,  256,  256,
       256,  256,  256,  256,  255,  255,  255,  254,  254,  253,  252,  251,  250,  249,  248,
       247,  246,  245,  243,  242,  241,  239,  237,  236,  234,  232,  230,  228,  226,  224,
       222,  219,  217,  215,  212,  210,  207,  204,  202,  199,  196,  193,  190,  187,  184,
       181,  178,  175,  171,  168,  165,  161,  158,  154,  150,  147,  143,  139,  136,  132,
       128,  124,  120,  116,  112,  108,  104,  100,   96,   92,   88,   83,   79,   75,   71,
        66,   62,   58,   53,   49,   44,   40,   36,   31,   27,   22,   18,   13,    9,    4 };

/*@ -charint @*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtx.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtx.c
 * \brief Main libdmtx source file
 */

#ifndef CALLBACK_POINT_PLOT
#define CALLBACK_POINT_PLOT(a,b,c,d)
#endif

#ifndef CALLBACK_POINT_XFRM
#define CALLBACK_POINT_XFRM(a,b,c,d)
#endif

#ifndef CALLBACK_MODULE
#define CALLBACK_MODULE(a,b,c,d,e)
#endif

#ifndef CALLBACK_MATRIX
#define CALLBACK_MATRIX(a)
#endif

#ifndef CALLBACK_FINAL
#define CALLBACK_FINAL(a,b)
#endif

extern char *
dmtxVersion(void)
{
   return DmtxVersion;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxencodebase256.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2011 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxencodebase256.c
 * \brief Base 256 encoding rules
 */

/**
 * \brief  Unrandomize 255 state
 * \param  value
 * \param  idx
 * \return Unrandomized value
 */
static unsigned char
UnRandomize255State(unsigned char value, int idx)
{
   int pseudoRandom;
   int tmp;

   pseudoRandom = ((149 * idx) % 255) + 1;
   tmp = value - pseudoRandom;
   if(tmp < 0)
      tmp += 256;

   assert(tmp >= 0 && tmp < 256);

   return (unsigned char)tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxdecode.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 * Copyright 2009 Mackenzie Straight. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxdecode.c
 * \brief Decode regions
 */

/**
 * \brief  Initialize decode struct with default values
 * \param  img
 * \return Initialized DmtxDecode struct
 */
extern DmtxDecode *
dmtxDecodeCreate(DmtxImage *img, int scale)
{
   DmtxDecode *dec;
   int width, height;

   dec = (DmtxDecode *)calloc(1, sizeof(DmtxDecode));
   if(dec == NULL)
      return NULL;

   width = dmtxImageGetProp(img, DmtxPropWidth) / scale;
   height = dmtxImageGetProp(img, DmtxPropHeight) / scale;

   dec->edgeMin = DmtxUndefined;
   dec->edgeMax = DmtxUndefined;
   dec->scanGap = 1;
   dec->squareDevn = cos(50 * (M_PI/180));
   dec->sizeIdxExpected = DmtxSymbolShapeAuto;
   dec->edgeThresh = 10;

   dec->xMin = 0;
   dec->xMax = width - 1;
   dec->yMin = 0;
   dec->yMax = height - 1;
   dec->scale = scale;

   dec->cache = (unsigned char *)calloc(width * height, sizeof(unsigned char));
   if(dec->cache == NULL) {
      free(dec);
      return NULL;
   }

   dec->image = img;
   dec->grid = InitScanGrid(dec);

   return dec;
}

/**
 * \brief  Deinitialize decode struct
 * \param  dec
 * \return void
 */
extern DmtxPassFail
dmtxDecodeDestroy(DmtxDecode **dec)
{
   if(dec == NULL || *dec == NULL)
      return DmtxFail;

   if((*dec)->cache != NULL)
      free((*dec)->cache);

   free(*dec);

   *dec = NULL;

   return DmtxPass;
}

/**
 * \brief  Set decoding behavior property
 * \param  dec
 * \param  prop
 * \param  value
 * \return DmtxPass | DmtxFail
 */
extern DmtxPassFail
dmtxDecodeSetProp(DmtxDecode *dec, int prop, int value)
{
   switch(prop) {
      case DmtxPropEdgeMin:
         dec->edgeMin = value;
         break;
      case DmtxPropEdgeMax:
         dec->edgeMax = value;
         break;
      case DmtxPropScanGap:
         dec->scanGap = value; /* XXX Should this be scaled? */
         break;
      case DmtxPropSquareDevn:
         dec->squareDevn = cos(value * (M_PI/180.0));
         break;
      case DmtxPropSymbolSize:
         dec->sizeIdxExpected = value;
         break;
      case DmtxPropEdgeThresh:
         dec->edgeThresh = value;
         break;
      /* Min and Max values arrive unscaled */
      case DmtxPropXmin:
         dec->xMin = value / dec->scale;
         break;
      case DmtxPropXmax:
         dec->xMax = value / dec->scale;
         break;
      case DmtxPropYmin:
         dec->yMin = value / dec->scale;
         break;
      case DmtxPropYmax:
         dec->yMax = value / dec->scale;
         break;
      default:
         break;
   }

   if(dec->squareDevn <= 0.0 || dec->squareDevn >= 1.0)
      return DmtxFail;

   if(dec->scanGap < 1)
      return DmtxFail;

   if(dec->edgeThresh < 1 || dec->edgeThresh > 100)
      return DmtxFail;

   /* Reinitialize scangrid in case any inputs changed */
   dec->grid = InitScanGrid(dec);

   return DmtxPass;
}

/**
 * \brief  Get decoding behavior property
 * \param  dec
 * \param  prop
 * \return value
 */
extern int
dmtxDecodeGetProp(DmtxDecode *dec, int prop)
{
   switch(prop) {
      case DmtxPropEdgeMin:
         return dec->edgeMin;
      case DmtxPropEdgeMax:
         return dec->edgeMax;
      case DmtxPropScanGap:
         return dec->scanGap;
      case DmtxPropSquareDevn:
         return (int)(acos(dec->squareDevn) * 180.0/M_PI);
      case DmtxPropSymbolSize:
         return dec->sizeIdxExpected;
      case DmtxPropEdgeThresh:
         return dec->edgeThresh;
      case DmtxPropXmin:
         return dec->xMin;
      case DmtxPropXmax:
         return dec->xMax;
      case DmtxPropYmin:
         return dec->yMin;
      case DmtxPropYmax:
         return dec->yMax;
      case DmtxPropScale:
         return dec->scale;
      case DmtxPropWidth:
         return dmtxImageGetProp(dec->image, DmtxPropWidth) / dec->scale;
      case DmtxPropHeight:
         return dmtxImageGetProp(dec->image, DmtxPropHeight) / dec->scale;
      default:
         break;
   }

   return DmtxUndefined;
}

/**
 * \brief  Returns xxx
 * \param  img
 * \param  Scaled x coordinate
 * \param  Scaled y coordinate
 * \return Scaled pixel offset
 */
extern unsigned char *
dmtxDecodeGetCache(DmtxDecode *dec, int x, int y)
{
//   int width, height;

   assert(dec != NULL);

/* if(dec.cacheComplete == DmtxFalse)
      CacheImage(); */

// Scale is always 1, so we can do it quicker
//   width = dmtxDecodeGetProp(dec, DmtxPropWidth);
//   height = dmtxDecodeGetProp(dec, DmtxPropHeight);

   if(x < 0 || x >= dec->image->width || y < 0 || y >= dec->image->height)
      return NULL;

   return &(dec->cache[y * dec->image->width + x]);
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxDecodeGetPixelValue(DmtxDecode *dec, int x, int y, int channel, int *value)
{
   int xUnscaled, yUnscaled;
   DmtxPassFail err;

   xUnscaled = x * dec->scale;
   yUnscaled = y * dec->scale;

/* Remove spherical lens distortion */
/* int width, height;
   float radiusPow2, radiusPow4;
   float factor;
   DmtxVector2 pointShifted;
   DmtxVector2 correctedPoint;

   width = dmtxImageGetProp(img, DmtxPropWidth);
   height = dmtxImageGetProp(img, DmtxPropHeight);

   pointShifted.X = point.X - width/2.0;
   pointShifted.Y = point.Y - height/2.0;

   radiusPow2 = pointShifted.X * pointShifted.X + pointShifted.Y * pointShifted.Y;
   radiusPow4 = radiusPow2 * radiusPow2;

   factor = 1 + (k1 * radiusPow2) + (k2 * radiusPow4);

   correctedPoint.X = pointShifted.X * factor + width/2.0;
   correctedPoint.Y = pointShifted.Y * factor + height/2.0;

   return correctedPoint; */

   err = dmtxImageGetPixelValue(dec->image, xUnscaled, yUnscaled, channel, value);

   return err;
}

/**
 * \brief  Fill the region covered by the quadrilateral given by (p0,p1,p2,p3) in the cache.
 */
static void
CacheFillQuad(DmtxDecode *dec, DmtxPixelLoc p0, DmtxPixelLoc p1, DmtxPixelLoc p2, DmtxPixelLoc p3)
{
   DmtxBresLine lines[4];
   DmtxPixelLoc pEmpty = { 0, 0 };
   unsigned char *cache;
   int *scanlineMin, *scanlineMax;
   int minY, maxY, sizeY, posY, posX;
   int i, idx;

   lines[0] = BresLineInit(p0, p1, pEmpty);
   lines[1] = BresLineInit(p1, p2, pEmpty);
   lines[2] = BresLineInit(p2, p3, pEmpty);
   lines[3] = BresLineInit(p3, p0, pEmpty);

   minY = dec->yMax;
   maxY = 0;

   minY = min(minY, p0.Y); maxY = max(maxY, p0.Y);
   minY = min(minY, p1.Y); maxY = max(maxY, p1.Y);
   minY = min(minY, p2.Y); maxY = max(maxY, p2.Y);
   minY = min(minY, p3.Y); maxY = max(maxY, p3.Y);

   sizeY = maxY - minY + 1;

   scanlineMin = (int *)malloc(sizeY * sizeof(int));
   scanlineMax = (int *)calloc(sizeY, sizeof(int));

   assert(scanlineMin); /* XXX handle this better */
   assert(scanlineMax); /* XXX handle this better */

   for(i = 0; i < sizeY; i++)
      scanlineMin[i] = dec->xMax;

   for(i = 0; i < 4; i++) {
      while(lines[i].loc.X != lines[i].loc1.X || lines[i].loc.Y != lines[i].loc1.Y) {
         idx = lines[i].loc.Y - minY;
         scanlineMin[idx] = min(scanlineMin[idx], lines[i].loc.X);
         scanlineMax[idx] = max(scanlineMax[idx], lines[i].loc.X);
         BresLineStep(lines + i, 1, 0);
      }
   }

   for(posY = minY; posY < maxY && posY < dec->yMax; posY++) {
      idx = posY - minY;
      for(posX = scanlineMin[idx]; posX < scanlineMax[idx] && posX < dec->xMax; posX++) {
         cache = dmtxDecodeGetCache(dec, posX, posY);
         if(cache != NULL)
            *cache |= 0x80;
      }
   }

   free(scanlineMin);
   free(scanlineMax);
}

/**
 * \brief  Convert fitted Data Matrix region into a decoded message
 * \param  dec
 * \param  reg
 * \param  fix
 * \return Decoded message
 */
extern DmtxMessage *
dmtxDecodeMatrixRegion(DmtxDecode *dec, DmtxRegion *reg, int fix)
{
   DmtxMessage *msg;
   DmtxVector2 topLeft, topRight, bottomLeft, bottomRight;
   DmtxPixelLoc pxTopLeft, pxTopRight, pxBottomLeft, pxBottomRight;

   msg = dmtxMessageCreate(reg->sizeIdx, DmtxFormatMatrix);
   if(msg == NULL)
      return NULL;

   if(PopulateArrayFromMatrix(dec, reg, msg) != DmtxPass) {
      dmtxMessageDestroy(&msg);
      return NULL;
   }

   /* maybe place remaining logic into new dmtxDecodePopulatedArray()
      function so other people can pass in their own arrays */

   ModulePlacementEcc200(msg->array, msg->code,
         reg->sizeIdx, DmtxModuleOnRed | DmtxModuleOnGreen | DmtxModuleOnBlue);

   if(RsDecode(msg->code, reg->sizeIdx, fix) == DmtxFail)
   {
      dmtxMessageDestroy(&msg);
      return NULL;
   }

   topLeft.X = bottomLeft.X = topLeft.Y = topRight.Y = -0.1;
   topRight.X = bottomRight.X = bottomLeft.Y = bottomRight.Y = 1.1;

   dmtxMatrix3VMultiplyBy(&topLeft, reg->fit2raw);
   dmtxMatrix3VMultiplyBy(&topRight, reg->fit2raw);
   dmtxMatrix3VMultiplyBy(&bottomLeft, reg->fit2raw);
   dmtxMatrix3VMultiplyBy(&bottomRight, reg->fit2raw);

   pxTopLeft.X = (int)(0.5 + topLeft.X);
   pxTopLeft.Y = (int)(0.5 + topLeft.Y);
   pxBottomLeft.X = (int)(0.5 + bottomLeft.X);
   pxBottomLeft.Y = (int)(0.5 + bottomLeft.Y);
   pxTopRight.X = (int)(0.5 + topRight.X);
   pxTopRight.Y = (int)(0.5 + topRight.Y);
   pxBottomRight.X = (int)(0.5 + bottomRight.X);
   pxBottomRight.Y = (int)(0.5 + bottomRight.Y);

   CacheFillQuad(dec, pxTopLeft, pxTopRight, pxBottomRight, pxBottomLeft);

   DecodeDataStream(msg, reg->sizeIdx, NULL);

   return msg;
}

/**
 * \brief  Convert fitted Data Mosaic region into a decoded message
 * \param  dec
 * \param  reg
 * \param  fix
 * \return Decoded message
 */
extern DmtxMessage *
dmtxDecodeMosaicRegion(DmtxDecode *dec, DmtxRegion *reg, int fix)
{
   int offset;
   int colorPlane;
   DmtxMessage *oMsg, *rMsg, *gMsg, *bMsg;

   colorPlane = reg->flowBegin.plane;

   /**
    * Consider performing a color cube fit here to identify exact RGB of
    * all 6 "cube-like" corners based on pixels located within region. Then
    * force each sample pixel to the "cube-like" corner based o which one
    * is nearest "sqrt(dr^2+dg^2+db^2)" (except sqrt is unnecessary).
    * colorPlane = reg->flowBegin.plane;
    *
    * To find RGB values of primary colors, perform something like a
    * histogram except instead of going from black to color N, go from
    * (127,127,127) to color. Use color bins along with distance to
    * identify value. An additional method will be required to get actual
    * RGB instead of just a plane in 3D. */

   reg->flowBegin.plane = 0; /* kind of a hack */
   rMsg = dmtxDecodeMatrixRegion(dec, reg, fix);

   reg->flowBegin.plane = 1; /* kind of a hack */
   gMsg = dmtxDecodeMatrixRegion(dec, reg, fix);

   reg->flowBegin.plane = 2; /* kind of a hack */
   bMsg = dmtxDecodeMatrixRegion(dec, reg, fix);

   reg->flowBegin.plane = colorPlane;

   oMsg = dmtxMessageCreate(reg->sizeIdx, DmtxFormatMosaic);

   if(oMsg == NULL || rMsg == NULL || gMsg == NULL || bMsg == NULL) {
      dmtxMessageDestroy(&oMsg);
      dmtxMessageDestroy(&rMsg);
      dmtxMessageDestroy(&gMsg);
      dmtxMessageDestroy(&bMsg);
      return NULL;
   }

   offset = 0;
   memcpy(oMsg->output + offset, rMsg->output, rMsg->outputIdx);
   offset += rMsg->outputIdx;
   memcpy(oMsg->output + offset, gMsg->output, gMsg->outputIdx);
   offset += gMsg->outputIdx;
   memcpy(oMsg->output + offset, bMsg->output, bMsg->outputIdx);
   offset += bMsg->outputIdx;

   oMsg->outputIdx = offset;

   dmtxMessageDestroy(&rMsg);
   dmtxMessageDestroy(&gMsg);
   dmtxMessageDestroy(&bMsg);

   return oMsg;
}

/**
 *
 *
 */
extern unsigned char *
dmtxDecodeCreateDiagnostic(DmtxDecode *dec, int *totalBytes, int *headerBytes, int style)
{
   int i, row, col;
   int width, height;
   int widthDigits, heightDigits;
   int count, channelCount;
   int rgb[3];
   float shade;
   unsigned char *pnm, *output, *cache;

   width = dmtxDecodeGetProp(dec, DmtxPropWidth);
   height = dmtxDecodeGetProp(dec, DmtxPropHeight);
   channelCount = dmtxImageGetProp(dec->image, DmtxPropChannelCount);

   style = 1; /* this doesn't mean anything yet */

   /* Count width digits */
   for(widthDigits = 0, i = width; i > 0; i /= 10)
      widthDigits++;

   /* Count height digits */
   for(heightDigits = 0, i = height; i > 0; i /= 10)
      heightDigits++;

   *headerBytes = widthDigits + heightDigits + 9;
   *totalBytes = *headerBytes + width * height * 3;

   pnm = (unsigned char *)malloc(*totalBytes);
   if(pnm == NULL)
      return NULL;

#ifdef _VISUALC_
   count = sprintf_s((char *)pnm, *headerBytes + 1, "P6\n%d %d\n255\n", width, height);
#else
   count = snprintf((char *)pnm, *headerBytes + 1, "P6\n%d %d\n255\n", width, height);
#endif

   if(count != *headerBytes) {
      free(pnm);
      return NULL;
   }

   output = pnm + (*headerBytes);
   for(row = height - 1; row >= 0; row--) {
      for(col = 0; col < width; col++) {
         cache = dmtxDecodeGetCache(dec, col, row);
         if(cache == NULL) {
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 128;
         }
         else if(*cache & 0x40) {
            rgb[0] = 255;
            rgb[1] = 0;
            rgb[2] = 0;
         }
         else {
            shade = (*cache & 0x80) ? 0.0 : 0.7;
            for(i = 0; i < 3; i++) {
               if(i < channelCount)
                  dmtxDecodeGetPixelValue(dec, col, row, i, &rgb[i]);
               else
                  dmtxDecodeGetPixelValue(dec, col, row, 0, &rgb[i]);

               rgb[i] += (int)(shade * (float)(255 - rgb[i]) + 0.5);
               if(rgb[i] > 255)
                  rgb[i] = 255;
            }
         }
         *(output++) = (unsigned char)rgb[0];
         *(output++) = (unsigned char)rgb[1];
         *(output++) = (unsigned char)rgb[2];
      }
   }
   assert(output == pnm + *totalBytes);

   return pnm;
}

/**
 * \brief  Increment counters used to determine module values
 * \param  img
 * \param  reg
 * \param  tally
 * \param  xOrigin
 * \param  yOrigin
 * \param  mapWidth
 * \param  mapHeight
 * \param  dir
 * \return void
 */
static void
TallyModuleJumps(DmtxDecode *dec, DmtxRegion *reg, int tally[][24], int xOrigin, int yOrigin, int mapWidth, int mapHeight, DmtxDirection dir)
{
   int extent, weight;
   int travelStep;
   int symbolRow, symbolCol;
   int mapRow, mapCol;
   int lineStart, lineStop;
   int travelStart, travelStop;
   int *line, *travel;
   int jumpThreshold;
   int darkOnLight;
   int color;
   int statusPrev, statusModule;
   int tPrev, tModule;

   assert(dir == DmtxDirUp || dir == DmtxDirLeft || dir == DmtxDirDown || dir == DmtxDirRight);

   travelStep = (dir == DmtxDirUp || dir == DmtxDirRight) ? 1 : -1;

   /* Abstract row and column progress using pointers to allow grid
      traversal in all 4 directions using same logic */

   if((dir & DmtxDirHorizontal) != 0x00) {
      line = &symbolRow;
      travel = &symbolCol;
      extent = mapWidth;
      lineStart = yOrigin;
      lineStop = yOrigin + mapHeight;
      travelStart = (travelStep == 1) ? xOrigin - 1 : xOrigin + mapWidth;
      travelStop = (travelStep == 1) ? xOrigin + mapWidth : xOrigin - 1;
   }
   else {
      assert(dir & DmtxDirVertical);
      line = &symbolCol;
      travel = &symbolRow;
      extent = mapHeight;
      lineStart = xOrigin;
      lineStop = xOrigin + mapWidth;
      travelStart = (travelStep == 1) ? yOrigin - 1: yOrigin + mapHeight;
      travelStop = (travelStep == 1) ? yOrigin + mapHeight : yOrigin - 1;
   }


   darkOnLight = (int)(reg->offColor > reg->onColor);
   jumpThreshold = abs((int)(0.4 * (reg->offColor - reg->onColor) + 0.5));

   assert(jumpThreshold >= 0);

   for(*line = lineStart; *line < lineStop; (*line)++) {

      /* Capture tModule for each leading border module as normal but
         decide status based on predictable barcode border pattern */

      *travel = travelStart;
      color = ReadModuleColor(dec, reg, symbolRow, symbolCol, reg->sizeIdx, reg->flowBegin.plane);
      tModule = (darkOnLight) ? reg->offColor - color : color - reg->offColor;

      statusModule = (travelStep == 1 || (*line & 0x01) == 0) ? DmtxModuleOnRGB : DmtxModuleOff;

      weight = extent;

      while((*travel += travelStep) != travelStop) {

         tPrev = tModule;
         statusPrev = statusModule;

         /* For normal data-bearing modules capture color and decide
            module status based on comparison to previous "known" module */

         color = ReadModuleColor(dec, reg, symbolRow, symbolCol, reg->sizeIdx, reg->flowBegin.plane);
         tModule = (darkOnLight) ? reg->offColor - color : color - reg->offColor;

         if(statusPrev == DmtxModuleOnRGB) {
            if(tModule < tPrev - jumpThreshold)
               statusModule = DmtxModuleOff;
            else
               statusModule = DmtxModuleOnRGB;
         }
         else if(statusPrev == DmtxModuleOff) {
            if(tModule > tPrev + jumpThreshold)
               statusModule = DmtxModuleOnRGB;
            else
               statusModule = DmtxModuleOff;
         }

         mapRow = symbolRow - yOrigin;
         mapCol = symbolCol - xOrigin;
         assert(mapRow < 24 && mapCol < 24);

         if(statusModule == DmtxModuleOnRGB)
            tally[mapRow][mapCol] += (2 * weight);

         weight--;
      }

      assert(weight == 0);
   }
}

/**
 * \brief  Populate array with codeword values based on module colors
 * \param  msg
 * \param  img
 * \param  reg
 * \return DmtxPass | DmtxFail
 */
static DmtxPassFail
PopulateArrayFromMatrix(DmtxDecode *dec, DmtxRegion *reg, DmtxMessage *msg)
{
   int weightFactor;
   int mapWidth, mapHeight;
   int xRegionTotal, yRegionTotal;
   int xRegionCount, yRegionCount;
   int xOrigin, yOrigin;
   int mapCol, mapRow;
   int colTmp, rowTmp, idx;
   int *tally_temp = malloc(sizeof(int) * 24 * 24); int (*tally)[24] = (int (*)[24]) tally_temp; // [24][24]; /* Large enough to map largest single region */

/* memset(msg->array, 0x00, msg->arraySize); */

   /* Capture number of regions present in barcode */
   xRegionTotal = dmtxGetSymbolAttribute(DmtxSymAttribHorizDataRegions, reg->sizeIdx);
   yRegionTotal = dmtxGetSymbolAttribute(DmtxSymAttribVertDataRegions, reg->sizeIdx);

   /* Capture region dimensions (not including border modules) */
   mapWidth = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionCols, reg->sizeIdx);
   mapHeight = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionRows, reg->sizeIdx);

   weightFactor = 2 * (mapHeight + mapWidth + 2);
   assert(weightFactor > 0);

   /* Tally module changes for each region in each direction */
   for(yRegionCount = 0; yRegionCount < yRegionTotal; yRegionCount++) {

      /* Y location of mapping region origin in symbol coordinates */
      yOrigin = yRegionCount * (mapHeight + 2) + 1;

      for(xRegionCount = 0; xRegionCount < xRegionTotal; xRegionCount++) {

         /* X location of mapping region origin in symbol coordinates */
         xOrigin = xRegionCount * (mapWidth + 2) + 1;

         memset(tally, 0x00, 24 * 24 * sizeof(int));
         TallyModuleJumps(dec, reg, tally, xOrigin, yOrigin, mapWidth, mapHeight, DmtxDirUp);
         TallyModuleJumps(dec, reg, tally, xOrigin, yOrigin, mapWidth, mapHeight, DmtxDirLeft);
         TallyModuleJumps(dec, reg, tally, xOrigin, yOrigin, mapWidth, mapHeight, DmtxDirDown);
         TallyModuleJumps(dec, reg, tally, xOrigin, yOrigin, mapWidth, mapHeight, DmtxDirRight);

         /* Decide module status based on final tallies */
         for(mapRow = 0; mapRow < mapHeight; mapRow++) {
            for(mapCol = 0; mapCol < mapWidth; mapCol++) {

               rowTmp = (yRegionCount * mapHeight) + mapRow;
               rowTmp = yRegionTotal * mapHeight - rowTmp - 1;
               colTmp = (xRegionCount * mapWidth) + mapCol;
               idx = (rowTmp * xRegionTotal * mapWidth) + colTmp;

               if(tally[mapRow][mapCol]/(float)weightFactor >= 0.5)
                  msg->array[idx] = DmtxModuleOnRGB;
               else
                  msg->array[idx] = DmtxModuleOff;

               msg->array[idx] |= DmtxModuleAssigned;
            }
         }
      }
   }

   free(tally_temp);

   return DmtxPass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxdecodescheme.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxdecodescheme.c
 */

/**
 * \brief  Translate encoded data stream into final output
 * \param  msg
 * \param  sizeIdx
 * \param  outputStart
 * \return void
 */
static void
DecodeDataStream(DmtxMessage *msg, int sizeIdx, unsigned char *outputStart)
{
   DmtxBoolean macro = DmtxFalse;
   DmtxScheme encScheme;
   unsigned char *ptr, *dataEnd;

   msg->output = (outputStart == NULL) ? msg->output : outputStart;
   msg->outputIdx = 0;

   ptr = msg->code;
   dataEnd = ptr + dmtxGetSymbolAttribute(DmtxSymAttribSymbolDataWords, sizeIdx);

   /* Print macro header if first codeword triggers it */
   if(*ptr == DmtxValue05Macro || *ptr == DmtxValue06Macro) {
      PushOutputMacroHeader(msg, *ptr);
      macro = DmtxTrue;
   }

   while(ptr < dataEnd) {

      encScheme = GetEncodationScheme(*ptr);
      if(encScheme != DmtxSchemeAscii)
         ptr++;

      switch(encScheme) {
         case DmtxSchemeAscii:
            ptr = DecodeSchemeAscii(msg, ptr, dataEnd);
            break;
         case DmtxSchemeC40:
         case DmtxSchemeText:
            ptr = DecodeSchemeC40Text(msg, ptr, dataEnd, encScheme);
            break;
         case DmtxSchemeX12:
            ptr = DecodeSchemeX12(msg, ptr, dataEnd);
            break;
         case DmtxSchemeEdifact:
            ptr = DecodeSchemeEdifact(msg, ptr, dataEnd);
            break;
         case DmtxSchemeBase256:
            ptr = DecodeSchemeBase256(msg, ptr, dataEnd);
            break;
         default:
            /* error */
            break;
      }
   }

   /* Print macro trailer if required */
   if(macro == DmtxTrue)
      PushOutputMacroTrailer(msg);
}

/**
 * \brief  Determine next encodation scheme
 * \param  encScheme
 * \param  cw
 * \return Pointer to next undecoded codeword
 */
static int
GetEncodationScheme(unsigned char cw)
{
   DmtxScheme encScheme;

   switch(cw) {
      case DmtxValueC40Latch:
         encScheme = DmtxSchemeC40;
         break;
      case DmtxValueTextLatch:
         encScheme = DmtxSchemeText;
         break;
      case DmtxValueX12Latch:
         encScheme = DmtxSchemeX12;
         break;
      case DmtxValueEdifactLatch:
         encScheme = DmtxSchemeEdifact;
         break;
      case DmtxValueBase256Latch:
         encScheme = DmtxSchemeBase256;
         break;
      default:
         encScheme = DmtxSchemeAscii;
         break;
   }

   return encScheme;
}

/**
 *
 *
 */
static void
PushOutputWord(DmtxMessage *msg, int value)
{
   assert(value >= 0 && value < 256);

   msg->output[msg->outputIdx++] = (unsigned char)value;
}

/**
 *
 *
 */
static void
PushOutputC40TextWord(DmtxMessage *msg, C40TextState *state, int value)
{
   assert(value >= 0 && value < 256);

   msg->output[msg->outputIdx] = (unsigned char)value;

   if(state->upperShift == DmtxTrue) {
      assert(value < 128);
      msg->output[msg->outputIdx] += 128;
   }

   msg->outputIdx++;

   state->shift = DmtxC40TextBasicSet;
   state->upperShift = DmtxFalse;
}

/**
 *
 *
 */
static void
PushOutputMacroHeader(DmtxMessage *msg, int macroType)
{
   PushOutputWord(msg, '[');
   PushOutputWord(msg, ')');
   PushOutputWord(msg, '>');
   PushOutputWord(msg, 30); /* ASCII RS */
   PushOutputWord(msg, '0');

   assert(macroType == DmtxValue05Macro || macroType == DmtxValue06Macro);
   if(macroType == DmtxValue05Macro)
      PushOutputWord(msg, '5');
   else
      PushOutputWord(msg, '6');

   PushOutputWord(msg, 29); /* ASCII GS */
}

/**
 *
 *
 */
static void
PushOutputMacroTrailer(DmtxMessage *msg)
{
   PushOutputWord(msg, 30); /* ASCII RS */
   PushOutputWord(msg, 4);  /* ASCII EOT */
}

/**
 * \brief  Decode stream assuming standard ASCII encodation
 * \param  msg
 * \param  ptr
 * \param  dataEnd
 * \return Pointer to next undecoded codeword
 */
static unsigned char *
DecodeSchemeAscii(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd)
{
   int upperShift;
   int codeword, digits;

   upperShift = DmtxFalse;

   while(ptr < dataEnd) {

      codeword = (int)(*ptr);

      if(GetEncodationScheme(*ptr) != DmtxSchemeAscii)
         return ptr;
      else
         ptr++;

      if(upperShift == DmtxTrue) {
         PushOutputWord(msg, codeword + 127);
         upperShift = DmtxFalse;
      }
      else if(codeword == DmtxValueAsciiUpperShift) {
         upperShift = DmtxTrue;
      }
      else if(codeword == DmtxValueAsciiPad) {
         assert(dataEnd >= ptr);
         assert(dataEnd - ptr <= INT_MAX);
         msg->padCount = (int)(dataEnd - ptr);
         return dataEnd;
      }
      else if(codeword <= 128) {
         PushOutputWord(msg, codeword - 1);
      }
      else if(codeword <= 229) {
         digits = codeword - 130;
         PushOutputWord(msg, digits/10 + '0');
         PushOutputWord(msg, digits - (digits/10)*10 + '0');
      }
   }

   return ptr;
}

/**
 * \brief  Decode stream assuming C40 or Text encodation
 * \param  msg
 * \param  ptr
 * \param  dataEnd
 * \param  encScheme
 * \return Pointer to next undecoded codeword
 */
static unsigned char *
DecodeSchemeC40Text(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd, DmtxScheme encScheme)
{
   int i;
   int packed;
   int c40Values[3];
   C40TextState state;

   state.shift = DmtxC40TextBasicSet;
   state.upperShift = DmtxFalse;

   assert(encScheme == DmtxSchemeC40 || encScheme == DmtxSchemeText);

   /* Unlatch is implied if only one codeword remains */
   if(dataEnd - ptr < 2)
      return ptr;

   while(ptr < dataEnd) {

      /* FIXME Also check that ptr+1 is safe to access */
      packed = (*ptr << 8) | *(ptr+1);
      c40Values[0] = ((packed - 1)/1600);
      c40Values[1] = ((packed - 1)/40) % 40;
      c40Values[2] =  (packed - 1) % 40;
      ptr += 2;

      for(i = 0; i < 3; i++) {
         if(state.shift == DmtxC40TextBasicSet) { /* Basic set */
            if(c40Values[i] <= 2) {
               state.shift = c40Values[i] + 1;
            }
            else if(c40Values[i] == 3) {
               PushOutputC40TextWord(msg, &state, ' ');
            }
            else if(c40Values[i] <= 13) {
               PushOutputC40TextWord(msg, &state, c40Values[i] - 13 + '9'); /* 0-9 */
            }
            else if(c40Values[i] <= 39) {
               if(encScheme == DmtxSchemeC40) {
                  PushOutputC40TextWord(msg, &state, c40Values[i] - 39 + 'Z'); /* A-Z */
               }
               else if(encScheme == DmtxSchemeText) {
                  PushOutputC40TextWord(msg, &state, c40Values[i] - 39 + 'z'); /* a-z */
               }
            }
         }
         else if(state.shift == DmtxC40TextShift1) { /* Shift 1 set */
            PushOutputC40TextWord(msg, &state, c40Values[i]); /* ASCII 0 - 31 */
         }
         else if(state.shift == DmtxC40TextShift2) { /* Shift 2 set */
            if(c40Values[i] <= 14) {
               PushOutputC40TextWord(msg, &state, c40Values[i] + 33); /* ASCII 33 - 47 */
            }
            else if(c40Values[i] <= 21) {
               PushOutputC40TextWord(msg, &state, c40Values[i] + 43); /* ASCII 58 - 64 */
            }
            else if(c40Values[i] <= 26) {
               PushOutputC40TextWord(msg, &state, c40Values[i] + 69); /* ASCII 91 - 95 */
            }
            else if(c40Values[i] == 27) {
               PushOutputC40TextWord(msg, &state, 0x1d); /* FNC1 -- XXX depends on position? */
            }
            else if(c40Values[i] == 30) {
               state.upperShift = DmtxTrue;
               state.shift = DmtxC40TextBasicSet;
            }
         }
         else if(state.shift == DmtxC40TextShift3) { /* Shift 3 set */
            if(encScheme == DmtxSchemeC40) {
               PushOutputC40TextWord(msg, &state, c40Values[i] + 96);
            }
            else if(encScheme == DmtxSchemeText) {
               if(c40Values[i] == 0)
                  PushOutputC40TextWord(msg, &state, c40Values[i] + 96);
               else if(c40Values[i] <= 26)
                  PushOutputC40TextWord(msg, &state, c40Values[i] - 26 + 'Z'); /* A-Z */
               else
                  PushOutputC40TextWord(msg, &state, c40Values[i] - 31 + 127); /* { | } ~ DEL */
            }
         }
      }

      /* Unlatch if codeword 254 follows 2 codewords in C40/Text encodation */
      if(*ptr == DmtxValueCTXUnlatch)
         return ptr + 1;

      /* Unlatch is implied if only one codeword remains */
      if(dataEnd - ptr < 2)
         return ptr;
   }

   return ptr;
}

/**
 * \brief  Decode stream assuming X12 encodation
 * \param  msg
 * \param  ptr
 * \param  dataEnd
 * \return Pointer to next undecoded codeword
 */
static unsigned char *
DecodeSchemeX12(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd)
{
   int i;
   int packed;
   int x12Values[3];

   /* Unlatch is implied if only one codeword remains */
   if(dataEnd - ptr < 2)
      return ptr;

   while(ptr < dataEnd) {

      /* FIXME Also check that ptr+1 is safe to access */
      packed = (*ptr << 8) | *(ptr+1);
      x12Values[0] = ((packed - 1)/1600);
      x12Values[1] = ((packed - 1)/40) % 40;
      x12Values[2] =  (packed - 1) % 40;
      ptr += 2;

      for(i = 0; i < 3; i++) {
         if(x12Values[i] == 0)
            PushOutputWord(msg, 13);
         else if(x12Values[i] == 1)
            PushOutputWord(msg, 42);
         else if(x12Values[i] == 2)
            PushOutputWord(msg, 62);
         else if(x12Values[i] == 3)
            PushOutputWord(msg, 32);
         else if(x12Values[i] <= 13)
            PushOutputWord(msg, x12Values[i] + 44);
         else if(x12Values[i] <= 90)
            PushOutputWord(msg, x12Values[i] + 51);
      }

      /* Unlatch if codeword 254 follows 2 codewords in C40/Text encodation */
      if(*ptr == DmtxValueCTXUnlatch)
         return ptr + 1;

      /* Unlatch is implied if only one codeword remains */
      if(dataEnd - ptr < 2)
         return ptr;
   }

   return ptr;
}

/**
 * \brief  Decode stream assuming EDIFACT encodation
 * \param  msg
 * \param  ptr
 * \param  dataEnd
 * \return Pointer to next undecoded codeword
 */
static unsigned char *
DecodeSchemeEdifact(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd)
{
   int i;
   unsigned char unpacked[4];

   /* Unlatch is implied if fewer than 3 codewords remain */
   if(dataEnd - ptr < 3)
      return ptr;

   while(ptr < dataEnd) {

      /* FIXME Also check that ptr+2 is safe to access -- shouldn't be a
         problem because I'm guessing you can guarantee there will always
         be at least 3 error codewords */
      unpacked[0] = (*ptr & 0xfc) >> 2;
      unpacked[1] = (*ptr & 0x03) << 4 | (*(ptr+1) & 0xf0) >> 4;
      unpacked[2] = (*(ptr+1) & 0x0f) << 2 | (*(ptr+2) & 0xc0) >> 6;
      unpacked[3] = *(ptr+2) & 0x3f;

      for(i = 0; i < 4; i++) {

         /* Advance input ptr (4th value comes from already-read 3rd byte) */
         if(i < 3)
            ptr++;

         /* Test for unlatch condition */
         if(unpacked[i] == DmtxValueEdifactUnlatch) {
            assert(msg->output[msg->outputIdx] == 0); /* XXX dirty why? */
            return ptr;
         }

         PushOutputWord(msg, unpacked[i] ^ (((unpacked[i] & 0x20) ^ 0x20) << 1));
      }

      /* Unlatch is implied if fewer than 3 codewords remain */
      if(dataEnd - ptr < 3)
         return ptr;
   }

   return ptr;

/* XXX the following version should be safer, but requires testing before replacing the old version
   int bits = 0;
   int bitCount = 0;
   int value;

   while(ptr < dataEnd) {

      if(bitCount < 6) {
         bits = (bits << 8) | *(ptr++);
         bitCount += 8;
      }

      value = bits >> (bitCount - 6);
      bits -= (value << (bitCount - 6));
      bitCount -= 6;

      if(value == 0x1f) {
         assert(bits == 0); // should be padded with zero-value bits
         return ptr;
      }
      PushOutputWord(msg, value ^ (((value & 0x20) ^ 0x20) << 1));

      // Unlatch implied if just completed triplet and 1 or 2 words are left
      if(bitCount == 0 && dataEnd - ptr - 1 > 0 && dataEnd - ptr - 1 < 3)
         return ptr;
   }

   assert(bits == 0); // should be padded with zero-value bits
   assert(bitCount == 0); // should be padded with zero-value bits
   return ptr;
*/
}

/**
 * \brief  Decode stream assuming Base 256 encodation
 * \param  msg
 * \param  ptr
 * \param  dataEnd
 * \return Pointer to next undecoded codeword
 */
static unsigned char *
DecodeSchemeBase256(DmtxMessage *msg, unsigned char *ptr, unsigned char *dataEnd)
{
   int d0, d1;
   int idx;
   unsigned char *ptrEnd;

   /* Find positional index used for unrandomizing */
   assert(ptr + 1 >= msg->code);
   assert(ptr + 1 - msg->code <= INT_MAX);
   idx = (int)(ptr + 1 - msg->code);

   d0 = UnRandomize255State(*(ptr++), idx++);
   if(d0 == 0) {
      ptrEnd = dataEnd;
   }
   else if(d0 <= 249) {
      ptrEnd = ptr + d0;
   }
   else {
      d1 = UnRandomize255State(*(ptr++), idx++);
      ptrEnd = ptr + (d0 - 249) * 250 + d1;
   }

   if(ptrEnd > dataEnd)
      fb_alloc_fail(); // exit(40); /* XXX needs cleaner error handling */

   while(ptr < ptrEnd)
      PushOutputWord(msg, UnRandomize255State(*(ptr++), idx++));

   return ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxmessage.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxmessage.c
 * \brief Data message handling
 */

/**
 * \brief  Allocate memory for message
 * \param  sizeIdx
 * \param  symbolFormat DmtxFormatMatrix | DmtxFormatMosaic
 * \return Address of allocated memory
 */
extern DmtxMessage *
dmtxMessageCreate(int sizeIdx, int symbolFormat)
{
   DmtxMessage *message;
   int mappingRows, mappingCols;

   assert(symbolFormat == DmtxFormatMatrix || symbolFormat == DmtxFormatMosaic);

   mappingRows = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixRows, sizeIdx);
   mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, sizeIdx);

   message = (DmtxMessage *)calloc(1, sizeof(DmtxMessage));
   if(message == NULL)
      return NULL;

   message->arraySize = sizeof(unsigned char) * mappingRows * mappingCols;

   message->array = (unsigned char *)calloc(1, message->arraySize);
   if(message->array == NULL) {
      perror("Calloc failed");
      dmtxMessageDestroy(&message);
      return NULL;
   }

   message->codeSize = sizeof(unsigned char) *
         dmtxGetSymbolAttribute(DmtxSymAttribSymbolDataWords, sizeIdx) +
         dmtxGetSymbolAttribute(DmtxSymAttribSymbolErrorWords, sizeIdx);

   if(symbolFormat == DmtxFormatMosaic)
      message->codeSize *= 3;

   message->code = (unsigned char *)calloc(message->codeSize, sizeof(unsigned char));
   if(message->code == NULL) {
      perror("Calloc failed");
      dmtxMessageDestroy(&message);
      return NULL;
   }

   /* XXX not sure if this is the right place or even the right approach.
      Trying to allocate memory for the decoded data stream and will
      initially assume that decoded data will not be larger than 2x encoded data */
   message->outputSize = sizeof(unsigned char) * message->codeSize * 10;
   message->output = (unsigned char *)calloc(message->outputSize, sizeof(unsigned char));
   if(message->output == NULL) {
      perror("Calloc failed");
      dmtxMessageDestroy(&message);
      return NULL;
   }

   return message;
}

/**
 * \brief  Free memory previously allocated for message
 * \param  message
 * \return void
 */
extern DmtxPassFail
dmtxMessageDestroy(DmtxMessage **msg)
{
   if(msg == NULL || *msg == NULL)
      return DmtxFail;

   if((*msg)->array != NULL)
      free((*msg)->array);

   if((*msg)->code != NULL)
      free((*msg)->code);

   if((*msg)->output != NULL)
      free((*msg)->output);

   free(*msg);

   *msg = NULL;

   return DmtxPass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxregion.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxregion.c
 * \brief Detect barcode regions
 */

#define DMTX_HOUGH_RES 180

/**
 * \brief  Create copy of existing region struct
 * \param  None
 * \return Initialized DmtxRegion struct
 */
extern DmtxRegion *
dmtxRegionCreate(DmtxRegion *reg)
{
   DmtxRegion *regCopy;

   regCopy = (DmtxRegion *)malloc(sizeof(DmtxRegion));
   if(regCopy == NULL)
      return NULL;

   memcpy(regCopy, reg, sizeof(DmtxRegion));

   return regCopy;
}

/**
 * \brief  Destroy region struct
 * \param  reg
 * \return void
 */
extern DmtxPassFail
dmtxRegionDestroy(DmtxRegion **reg)
{
   if(reg == NULL || *reg == NULL)
      return DmtxFail;

   free(*reg);

   *reg = NULL;

   return DmtxPass;
}

/**
 * \brief  Find next barcode region
 * \param  dec Pointer to DmtxDecode information struct
 * \return Detected region (if found)
 */
extern DmtxRegion *
dmtxRegionFindNext(DmtxDecode *dec, int max_iterations, int *current_iterations)
{
   int locStatus;
   DmtxPixelLoc loc;
   DmtxRegion   *reg;

   /* Continue until we find a region or run out of chances */
   for(; *current_iterations < max_iterations; *current_iterations += 1) {
      locStatus = PopGridLocation(&(dec->grid), &loc);
      if(locStatus == DmtxRangeEnd)
         break;

      /* Scan location for presence of valid barcode region */
      reg = dmtxRegionScanPixel(dec, loc.X, loc.Y);
      if(reg != NULL)
         return reg;
   }

   return NULL;
}

/**
 * \brief  Scan individual pixel for presence of barcode edge
 * \param  dec Pointer to DmtxDecode information struct
 * \param  loc Pixel location
 * \return Detected region (if any)
 */
extern DmtxRegion *
dmtxRegionScanPixel(DmtxDecode *dec, int x, int y)
{
   unsigned char *cache;
   DmtxRegion reg;
   DmtxPointFlow flowBegin;
   DmtxPixelLoc loc;

   loc.X = x;
   loc.Y = y;

   cache = dmtxDecodeGetCache(dec, loc.X, loc.Y);
   if(cache == NULL)
      return NULL;

   if((int)(*cache & 0x80) != 0x00)
      return NULL;

   /* Test for presence of any reasonable edge at this location */
   flowBegin = MatrixRegionSeekEdge(dec, loc);
   if(flowBegin.mag < (int)(dec->edgeThresh * 7.65 + 0.5))
      return NULL;

   memset(&reg, 0x00, sizeof(DmtxRegion));

   /* Determine barcode orientation */
   if(MatrixRegionOrientation(dec, &reg, flowBegin) == DmtxFail)
      return NULL;
   if(dmtxRegionUpdateXfrms(dec, &reg) == DmtxFail)
      return NULL;

   /* Define top edge */
   if(MatrixRegionAlignCalibEdge(dec, &reg, DmtxEdgeTop) == DmtxFail)
      return NULL;
   if(dmtxRegionUpdateXfrms(dec, &reg) == DmtxFail)
      return NULL;

   /* Define right edge */
   if(MatrixRegionAlignCalibEdge(dec, &reg, DmtxEdgeRight) == DmtxFail)
      return NULL;
   if(dmtxRegionUpdateXfrms(dec, &reg) == DmtxFail)
      return NULL;

   CALLBACK_MATRIX(&reg);

   /* Calculate the best fitting symbol size */
   if(MatrixRegionFindSize(dec, &reg) == DmtxFail)
      return NULL;

   /* Found a valid matrix region */
   return dmtxRegionCreate(&reg);
}

/**
 *
 *
 */
static DmtxPointFlow
MatrixRegionSeekEdge(DmtxDecode *dec, DmtxPixelLoc loc)
{
   int i;
   int strongIdx;
   int channelCount;
   DmtxPointFlow flow, flowPlane[3];
   DmtxPointFlow flowPos, flowPosBack;
   DmtxPointFlow flowNeg, flowNegBack;

   channelCount = dec->image->channelCount;

   /* Find whether red, green, or blue shows the strongest edge */
   strongIdx = 0;
   for(i = 0; i < channelCount; i++) {
      flowPlane[i] = GetPointFlow(dec, i, loc, dmtxNeighborNone);
      if(i > 0 && flowPlane[i].mag > flowPlane[strongIdx].mag)
         strongIdx = i;
   }

   if(flowPlane[strongIdx].mag < 10)
      return dmtxBlankEdge;

   flow = flowPlane[strongIdx];

   flowPos = FindStrongestNeighbor(dec, flow, +1);
   flowNeg = FindStrongestNeighbor(dec, flow, -1);
   if(flowPos.mag != 0 && flowNeg.mag != 0) {
      flowPosBack = FindStrongestNeighbor(dec, flowPos, -1);
      flowNegBack = FindStrongestNeighbor(dec, flowNeg, +1);
      if(flowPos.arrive == (flowPosBack.arrive+4)%8 &&
            flowNeg.arrive == (flowNegBack.arrive+4)%8) {
         flow.arrive = dmtxNeighborNone;
         CALLBACK_POINT_PLOT(flow.loc, 1, 1, 1);
         return flow;
      }
   }

   return dmtxBlankEdge;
}

/**
 *
 *
 */
static DmtxPassFail
MatrixRegionOrientation(DmtxDecode *dec, DmtxRegion *reg, DmtxPointFlow begin)
{
   int cross;
   int minArea;
   int scale;
   int symbolShape;
   int maxDiagonal;
   DmtxPassFail err;
   DmtxBestLine line1x, line2x;
   DmtxBestLine line2n, line2p;
   DmtxFollow fTmp;

   if(dec->sizeIdxExpected == DmtxSymbolSquareAuto ||
         (dec->sizeIdxExpected >= DmtxSymbol10x10 &&
         dec->sizeIdxExpected <= DmtxSymbol144x144))
      symbolShape = DmtxSymbolSquareAuto;
   else if(dec->sizeIdxExpected == DmtxSymbolRectAuto ||
         (dec->sizeIdxExpected >= DmtxSymbol8x18 &&
         dec->sizeIdxExpected <= DmtxSymbol16x48))
      symbolShape = DmtxSymbolRectAuto;
   else
      symbolShape = DmtxSymbolShapeAuto;

   if(dec->edgeMax != DmtxUndefined) {
      if(symbolShape == DmtxSymbolRectAuto)
         maxDiagonal = (int)(1.23 * dec->edgeMax + 0.5); /* sqrt(5/4) + 10% */
      else
         maxDiagonal = (int)(1.56 * dec->edgeMax + 0.5); /* sqrt(2) + 10% */
   }
   else {
      maxDiagonal = DmtxUndefined;
   }

   /* Follow to end in both directions */
   err = TrailBlazeContinuous(dec, reg, begin, maxDiagonal);
   if(err == DmtxFail || reg->stepsTotal < 40) {
      TrailClear(dec, reg, 0x40);
      return DmtxFail;
   }

   /* Filter out region candidates that are smaller than expected */
   if(dec->edgeMin != DmtxUndefined) {
      scale = dmtxDecodeGetProp(dec, DmtxPropScale);

      if(symbolShape == DmtxSymbolSquareAuto)
         minArea = (dec->edgeMin * dec->edgeMin)/(scale * scale);
      else
         minArea = (2 * dec->edgeMin * dec->edgeMin)/(scale * scale);

      if((reg->boundMax.X - reg->boundMin.X) * (reg->boundMax.Y - reg->boundMin.Y) < minArea) {
         TrailClear(dec, reg, 0x40);
         return DmtxFail;
      }
   }

   line1x = FindBestSolidLine(dec, reg, 0, 0, +1, DmtxUndefined);
   if(line1x.mag < 5) {
      TrailClear(dec, reg, 0x40);
      return DmtxFail;
   }

   err = FindTravelLimits(dec, reg, &line1x);
   if(line1x.distSq < 100 || line1x.devn * 10 >= sqrt((float)line1x.distSq)) {
      TrailClear(dec, reg, 0x40);
      return DmtxFail;
   }
   assert(line1x.stepPos >= line1x.stepNeg);

   fTmp = FollowSeek(dec, reg, line1x.stepPos + 5);
   line2p = FindBestSolidLine(dec, reg, fTmp.step, line1x.stepNeg, +1, line1x.angle);

   fTmp = FollowSeek(dec, reg, line1x.stepNeg - 5);
   line2n = FindBestSolidLine(dec, reg, fTmp.step, line1x.stepPos, -1, line1x.angle);
   if(max(line2p.mag, line2n.mag) < 5)
      return DmtxFail;

   if(line2p.mag > line2n.mag) {
      line2x = line2p;
      err = FindTravelLimits(dec, reg, &line2x);
      if(line2x.distSq < 100 || line2x.devn * 10 >= sqrt((float)line2x.distSq))
         return DmtxFail;

      cross = ((line1x.locPos.X - line1x.locNeg.X) * (line2x.locPos.Y - line2x.locNeg.Y)) -
            ((line1x.locPos.Y - line1x.locNeg.Y) * (line2x.locPos.X - line2x.locNeg.X));
      if(cross > 0) {
         /* Condition 2 */
         reg->polarity = +1;
         reg->locR = line2x.locPos;
         reg->stepR = line2x.stepPos;
         reg->locT = line1x.locNeg;
         reg->stepT = line1x.stepNeg;
         reg->leftLoc = line1x.locBeg;
         reg->leftAngle = line1x.angle;
         reg->bottomLoc = line2x.locBeg;
         reg->bottomAngle = line2x.angle;
         reg->leftLine = line1x;
         reg->bottomLine = line2x;
      }
      else {
         /* Condition 3 */
         reg->polarity = -1;
         reg->locR = line1x.locNeg;
         reg->stepR = line1x.stepNeg;
         reg->locT = line2x.locPos;
         reg->stepT = line2x.stepPos;
         reg->leftLoc = line2x.locBeg;
         reg->leftAngle = line2x.angle;
         reg->bottomLoc = line1x.locBeg;
         reg->bottomAngle = line1x.angle;
         reg->leftLine = line2x;
         reg->bottomLine = line1x;
      }
   }
   else {
      line2x = line2n;
      err = FindTravelLimits(dec, reg, &line2x);
      if(line2x.distSq < 100 || line2x.devn / sqrt((float)line2x.distSq) >= 0.1)
         return DmtxFail;

      cross = ((line1x.locNeg.X - line1x.locPos.X) * (line2x.locNeg.Y - line2x.locPos.Y)) -
            ((line1x.locNeg.Y - line1x.locPos.Y) * (line2x.locNeg.X - line2x.locPos.X));
      if(cross > 0) {
         /* Condition 1 */
         reg->polarity = -1;
         reg->locR = line2x.locNeg;
         reg->stepR = line2x.stepNeg;
         reg->locT = line1x.locPos;
         reg->stepT = line1x.stepPos;
         reg->leftLoc = line1x.locBeg;
         reg->leftAngle = line1x.angle;
         reg->bottomLoc = line2x.locBeg;
         reg->bottomAngle = line2x.angle;
         reg->leftLine = line1x;
         reg->bottomLine = line2x;
      }
      else {
         /* Condition 4 */
         reg->polarity = +1;
         reg->locR = line1x.locPos;
         reg->stepR = line1x.stepPos;
         reg->locT = line2x.locNeg;
         reg->stepT = line2x.stepNeg;
         reg->leftLoc = line2x.locBeg;
         reg->leftAngle = line2x.angle;
         reg->bottomLoc = line1x.locBeg;
         reg->bottomAngle = line1x.angle;
         reg->leftLine = line2x;
         reg->bottomLine = line1x;
      }
   }
/* CALLBACK_POINT_PLOT(reg->locR, 2, 1, 1);
   CALLBACK_POINT_PLOT(reg->locT, 2, 1, 1); */

   reg->leftKnown = reg->bottomKnown = 1;

   return DmtxPass;
}

/**
 *
 *
 */
static long
DistanceSquared(DmtxPixelLoc a, DmtxPixelLoc b)
{
   long xDelta, yDelta;

   xDelta = a.X - b.X;
   yDelta = a.Y - b.Y;

   return (xDelta * xDelta) + (yDelta * yDelta);
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxRegionUpdateCorners(DmtxDecode *dec, DmtxRegion *reg, DmtxVector2 p00,
      DmtxVector2 p10, DmtxVector2 p11, DmtxVector2 p01)
{
   float xMax, yMax;
   float tx, ty, phi, shx, scx, scy, skx, sky;
   float dimOT, dimOR, dimTX, dimRX, ratio;
   DmtxVector2 vOT, vOR, vTX, vRX, vTmp;
   DmtxMatrix3 m, mtxy, mphi, mshx, mscx, mscy, mscxy, msky, mskx;

   xMax = (float)(dmtxDecodeGetProp(dec, DmtxPropWidth) - 1);
   yMax = (float)(dmtxDecodeGetProp(dec, DmtxPropHeight) - 1);

   if(p00.X < 0.0 || p00.Y < 0.0 || p00.X > xMax || p00.Y > yMax ||
         p01.X < 0.0 || p01.Y < 0.0 || p01.X > xMax || p01.Y > yMax ||
         p10.X < 0.0 || p10.Y < 0.0 || p10.X > xMax || p10.Y > yMax)
      return DmtxFail;

   dimOT = dmtxVector2Mag(dmtxVector2Sub(&vOT, &p01, &p00)); /* XXX could use MagSquared() */
   dimOR = dmtxVector2Mag(dmtxVector2Sub(&vOR, &p10, &p00));
   dimTX = dmtxVector2Mag(dmtxVector2Sub(&vTX, &p11, &p01));
   dimRX = dmtxVector2Mag(dmtxVector2Sub(&vRX, &p11, &p10));

   /* Verify that sides are reasonably long */
   if(dimOT <= 8.0 || dimOR <= 8.0 || dimTX <= 8.0 || dimRX <= 8.0)
      return DmtxFail;

   /* Verify that the 4 corners define a reasonably fat quadrilateral */
   ratio = dimOT / dimRX;
   if(ratio <= 0.5 || ratio >= 2.0)
      return DmtxFail;

   ratio = dimOR / dimTX;
   if(ratio <= 0.5 || ratio >= 2.0)
      return DmtxFail;

   /* Verify this is not a bowtie shape */
   if(dmtxVector2Cross(&vOR, &vRX) <= 0.0 ||
         dmtxVector2Cross(&vOT, &vTX) >= 0.0)
      return DmtxFail;

   if(RightAngleTrueness(p00, p10, p11, M_PI_2) <= dec->squareDevn)
      return DmtxFail;
   if(RightAngleTrueness(p10, p11, p01, M_PI_2) <= dec->squareDevn)
      return DmtxFail;

   /* Calculate values needed for transformations */
   tx = -1 * p00.X;
   ty = -1 * p00.Y;
   dmtxMatrix3Translate(mtxy, tx, ty);

   phi = atan2(vOT.X, vOT.Y);
   dmtxMatrix3Rotate(mphi, phi);
   dmtxMatrix3Multiply(m, mtxy, mphi);

   dmtxMatrix3VMultiply(&vTmp, &p10, m);
   shx = -vTmp.Y / vTmp.X;
   dmtxMatrix3Shear(mshx, 0.0, shx);
   dmtxMatrix3MultiplyBy(m, mshx);

   scx = 1.0/vTmp.X;
   dmtxMatrix3Scale(mscx, scx, 1.0);
   dmtxMatrix3MultiplyBy(m, mscx);

   dmtxMatrix3VMultiply(&vTmp, &p11, m);
   scy = 1.0/vTmp.Y;
   dmtxMatrix3Scale(mscy, 1.0, scy);
   dmtxMatrix3MultiplyBy(m, mscy);

   dmtxMatrix3VMultiply(&vTmp, &p11, m);
   skx = vTmp.X;
   dmtxMatrix3LineSkewSide(mskx, 1.0, skx, 1.0);
   dmtxMatrix3MultiplyBy(m, mskx);

   dmtxMatrix3VMultiply(&vTmp, &p01, m);
   sky = vTmp.Y;
   dmtxMatrix3LineSkewTop(msky, sky, 1.0, 1.0);
   dmtxMatrix3Multiply(reg->raw2fit, m, msky);

   /* Create inverse matrix by reverse (avoid straight matrix inversion) */
   dmtxMatrix3LineSkewTopInv(msky, sky, 1.0, 1.0);
   dmtxMatrix3LineSkewSideInv(mskx, 1.0, skx, 1.0);
   dmtxMatrix3Multiply(m, msky, mskx);

   dmtxMatrix3Scale(mscxy, 1.0/scx, 1.0/scy);
   dmtxMatrix3MultiplyBy(m, mscxy);

   dmtxMatrix3Shear(mshx, 0.0, -shx);
   dmtxMatrix3MultiplyBy(m, mshx);

   dmtxMatrix3Rotate(mphi, -phi);
   dmtxMatrix3MultiplyBy(m, mphi);

   dmtxMatrix3Translate(mtxy, -tx, -ty);
   dmtxMatrix3Multiply(reg->fit2raw, m, mtxy);

   return DmtxPass;
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxRegionUpdateXfrms(DmtxDecode *dec, DmtxRegion *reg)
{
   float radians;
   DmtxRay2 rLeft, rBottom, rTop, rRight;
   DmtxVector2 p00, p10, p11, p01;

   assert(reg->leftKnown != 0 && reg->bottomKnown != 0);

   /* Build ray representing left edge */
   rLeft.p.X = (float)reg->leftLoc.X;
   rLeft.p.Y = (float)reg->leftLoc.Y;
   radians = reg->leftAngle * (M_PI/DMTX_HOUGH_RES);
   rLeft.v.X = cos(radians);
   rLeft.v.Y = sin(radians);
   rLeft.tMin = 0.0;
   rLeft.tMax = dmtxVector2Norm(&rLeft.v);

   /* Build ray representing bottom edge */
   rBottom.p.X = (float)reg->bottomLoc.X;
   rBottom.p.Y = (float)reg->bottomLoc.Y;
   radians = reg->bottomAngle * (M_PI/DMTX_HOUGH_RES);
   rBottom.v.X = cos(radians);
   rBottom.v.Y = sin(radians);
   rBottom.tMin = 0.0;
   rBottom.tMax = dmtxVector2Norm(&rBottom.v);

   /* Build ray representing top edge */
   if(reg->topKnown != 0) {
      rTop.p.X = (float)reg->topLoc.X;
      rTop.p.Y = (float)reg->topLoc.Y;
      radians = reg->topAngle * (M_PI/DMTX_HOUGH_RES);
      rTop.v.X = cos(radians);
      rTop.v.Y = sin(radians);
      rTop.tMin = 0.0;
      rTop.tMax = dmtxVector2Norm(&rTop.v);
   }
   else {
      rTop.p.X = (float)reg->locT.X;
      rTop.p.Y = (float)reg->locT.Y;
      radians = reg->bottomAngle * (M_PI/DMTX_HOUGH_RES);
      rTop.v.X = cos(radians);
      rTop.v.Y = sin(radians);
      rTop.tMin = 0.0;
      rTop.tMax = rBottom.tMax;
   }

   /* Build ray representing right edge */
   if(reg->rightKnown != 0) {
      rRight.p.X = (float)reg->rightLoc.X;
      rRight.p.Y = (float)reg->rightLoc.Y;
      radians = reg->rightAngle * (M_PI/DMTX_HOUGH_RES);
      rRight.v.X = cos(radians);
      rRight.v.Y = sin(radians);
      rRight.tMin = 0.0;
      rRight.tMax = dmtxVector2Norm(&rRight.v);
   }
   else {
      rRight.p.X = (float)reg->locR.X;
      rRight.p.Y = (float)reg->locR.Y;
      radians = reg->leftAngle * (M_PI/DMTX_HOUGH_RES);
      rRight.v.X = cos(radians);
      rRight.v.Y = sin(radians);
      rRight.tMin = 0.0;
      rRight.tMax = rLeft.tMax;
   }

   /* Calculate 4 corners, real or imagined */
   if(dmtxRay2Intersect(&p00, &rLeft, &rBottom) == DmtxFail)
      return DmtxFail;

   if(dmtxRay2Intersect(&p10, &rBottom, &rRight) == DmtxFail)
      return DmtxFail;

   if(dmtxRay2Intersect(&p11, &rRight, &rTop) == DmtxFail)
      return DmtxFail;

   if(dmtxRay2Intersect(&p01, &rTop, &rLeft) == DmtxFail)
      return DmtxFail;

   if(dmtxRegionUpdateCorners(dec, reg, p00, p10, p11, p01) != DmtxPass)
      return DmtxFail;

   return DmtxPass;
}

/**
 *
 *
 */
static float
RightAngleTrueness(DmtxVector2 c0, DmtxVector2 c1, DmtxVector2 c2, float angle)
{
   DmtxVector2 vA, vB;
   DmtxMatrix3 m;

   dmtxVector2Norm(dmtxVector2Sub(&vA, &c0, &c1));
   dmtxVector2Norm(dmtxVector2Sub(&vB, &c2, &c1));

   dmtxMatrix3Rotate(m, angle);
   dmtxMatrix3VMultiplyBy(&vB, m);

   return dmtxVector2Dot(&vA, &vB);
}

void Matrix3VMultFast(DmtxVector2 *vIn, DmtxMatrix3 m)
{
   float w;
   float x, y;
    
   w = vIn->X*m[0][2] + vIn->Y*m[1][2] + m[2][2];
   if(fabsf(w) <= DmtxAlmostZero) {
      vIn->X = FLT_MAX;
      vIn->Y = FLT_MAX;
      return;
   }

   x = (vIn->X*m[0][0] + vIn->Y*m[1][0] + m[2][0])/w;
   y = (vIn->X*m[0][1] + vIn->Y*m[1][1] + m[2][1])/w;
   vIn->X = x; vIn->Y = y;
   return;
}

/**
 * \brief  Read color of Data Matrix module location
 * \param  dec
 * \param  reg
 * \param  symbolRow
 * \param  symbolCol
 * \param  sizeIdx
 * \return Averaged module color
 */
static int
ReadModuleColor(DmtxDecode *dec, DmtxRegion *reg, int symbolRow, int symbolCol,
      int sizeIdx, int colorPlane)
{
   int err;
   int i;
   int symbolRows, symbolCols;
   int color, colorTmp;
   float sampleX[] = { 0.5, 0.4, 0.5, 0.6, 0.5 };
   float sampleY[] = { 0.5, 0.5, 0.4, 0.5, 0.6 };
   DmtxVector2 p;

   symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, sizeIdx);
   symbolCols = dmtxGetSymbolAttribute(DmtxSymAttribSymbolCols, sizeIdx);

   color = colorTmp = 0;
    if (dec->image->channelCount == 1) // quicker for grayscale
    {
        int x, y;
        for(i = 0; i < 5; i++) {

           p.X = (1.0/symbolCols) * (symbolCol + sampleX[i]);
           p.Y = (1.0/symbolRows) * (symbolRow + sampleY[i]);

//           dmtxMatrix3VMultiplyBy(&p, reg->fit2raw);
            Matrix3VMultFast(&p, reg->fit2raw);
            x = (int)(p.X + 0.5f);
            y = (int)(p.Y + 0.5f);
            if (x >= 0 && y >= 0 && x < dec->image->width && y < dec->image->height)
                colorTmp = dec->image->pxl[(dec->image->height - 1 - y) * dec->image->rowSizeBytes + x];
 //          err = dmtxDecodeGetPixelValue(dec, (int)(p.X + 0.5), (int)(p.Y + 0.5),
 //                colorPlane, &colorTmp);
           color += colorTmp;
        }
    }
    else
    {
       for(i = 0; i < 5; i++) {

          p.X = (1.0/symbolCols) * (symbolCol + sampleX[i]);
          p.Y = (1.0/symbolRows) * (symbolRow + sampleY[i]);

          dmtxMatrix3VMultiplyBy(&p, reg->fit2raw);

          err = dmtxDecodeGetPixelValue(dec, (int)(p.X + 0.5), (int)(p.Y + 0.5),
                colorPlane, &colorTmp);
          color += colorTmp;
       }
    }

   return color/5;
}

/**
 * \brief  Determine barcode size, expressed in modules
 * \param  image
 * \param  reg
 * \return DmtxPass | DmtxFail
 */
static DmtxPassFail
MatrixRegionFindSize(DmtxDecode *dec, DmtxRegion *reg)
{
   int row, col;
   int sizeIdxBeg, sizeIdxEnd;
   int sizeIdx, bestSizeIdx;
   int symbolRows, symbolCols;
   int jumpCount, errors;
   int color;
   int colorOnAvg, bestColorOnAvg;
   int colorOffAvg, bestColorOffAvg;
   int contrast, bestContrast;
   DmtxImage *img;

   img = dec->image;
   bestSizeIdx = DmtxUndefined;
   bestContrast = 0;
   bestColorOnAvg = bestColorOffAvg = 0;

   if(dec->sizeIdxExpected == DmtxSymbolShapeAuto) {
      sizeIdxBeg = 0;
      sizeIdxEnd = DmtxSymbolSquareCount + DmtxSymbolRectCount;
   }
   else if(dec->sizeIdxExpected == DmtxSymbolSquareAuto) {
      sizeIdxBeg = 0;
      sizeIdxEnd = DmtxSymbolSquareCount;
   }
   else if(dec->sizeIdxExpected == DmtxSymbolRectAuto) {
      sizeIdxBeg = DmtxSymbolSquareCount;
      sizeIdxEnd = DmtxSymbolSquareCount + DmtxSymbolRectCount;
   }
   else {
      sizeIdxBeg = dec->sizeIdxExpected;
      sizeIdxEnd = dec->sizeIdxExpected + 1;
   }

   /* Test each barcode size to find best contrast in calibration modules */
   for(sizeIdx = sizeIdxBeg; sizeIdx < sizeIdxEnd; sizeIdx++) {

      symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, sizeIdx);
      symbolCols = dmtxGetSymbolAttribute(DmtxSymAttribSymbolCols, sizeIdx);
      colorOnAvg = colorOffAvg = 0;

      /* Sum module colors along horizontal calibration bar */
      row = symbolRows - 1;
      for(col = 0; col < symbolCols; col++) {
         color = ReadModuleColor(dec, reg, row, col, sizeIdx, reg->flowBegin.plane);
         if((col & 0x01) != 0x00)
            colorOffAvg += color;
         else
            colorOnAvg += color;
      }

      /* Sum module colors along vertical calibration bar */
      col = symbolCols - 1;
      for(row = 0; row < symbolRows; row++) {
         color = ReadModuleColor(dec, reg, row, col, sizeIdx, reg->flowBegin.plane);
         if((row & 0x01) != 0x00)
            colorOffAvg += color;
         else
            colorOnAvg += color;
      }

      colorOnAvg = (colorOnAvg * 2)/(symbolRows + symbolCols);
      colorOffAvg = (colorOffAvg * 2)/(symbolRows + symbolCols);

      contrast = abs(colorOnAvg - colorOffAvg);
      if(contrast < 20)
         continue;

      if(contrast > bestContrast) {
         bestContrast = contrast;
         bestSizeIdx = sizeIdx;
         bestColorOnAvg = colorOnAvg;
         bestColorOffAvg = colorOffAvg;
      }
   }

   /* If no sizes produced acceptable contrast then call it quits */
   if(bestSizeIdx == DmtxUndefined || bestContrast < 20)
      return DmtxFail;

   reg->sizeIdx = bestSizeIdx;
   reg->onColor = bestColorOnAvg;
   reg->offColor = bestColorOffAvg;

   reg->symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, reg->sizeIdx);
   reg->symbolCols = dmtxGetSymbolAttribute(DmtxSymAttribSymbolCols, reg->sizeIdx);
   reg->mappingRows = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixRows, reg->sizeIdx);
   reg->mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, reg->sizeIdx);

   /* Tally jumps on horizontal calibration bar to verify sizeIdx */
   jumpCount = CountJumpTally(dec, reg, 0, reg->symbolRows - 1, DmtxDirRight);
   errors = abs(1 + jumpCount - reg->symbolCols);
   if(jumpCount < 0 || errors > 2)
      return DmtxFail;

   /* Tally jumps on vertical calibration bar to verify sizeIdx */
   jumpCount = CountJumpTally(dec, reg, reg->symbolCols - 1, 0, DmtxDirUp);
   errors = abs(1 + jumpCount - reg->symbolRows);
   if(jumpCount < 0 || errors > 2)
      return DmtxFail;

   /* Tally jumps on horizontal finder bar to verify sizeIdx */
   errors = CountJumpTally(dec, reg, 0, 0, DmtxDirRight);
   if(jumpCount < 0 || errors > 2)
      return DmtxFail;

   /* Tally jumps on vertical finder bar to verify sizeIdx */
   errors = CountJumpTally(dec, reg, 0, 0, DmtxDirUp);
   if(errors < 0 || errors > 2)
      return DmtxFail;

   /* Tally jumps on surrounding whitespace, else fail */
   errors = CountJumpTally(dec, reg, 0, -1, DmtxDirRight);
   if(errors < 0 || errors > 2)
      return DmtxFail;

   errors = CountJumpTally(dec, reg, -1, 0, DmtxDirUp);
   if(errors < 0 || errors > 2)
      return DmtxFail;

   errors = CountJumpTally(dec, reg, 0, reg->symbolRows, DmtxDirRight);
   if(errors < 0 || errors > 2)
      return DmtxFail;

   errors = CountJumpTally(dec, reg, reg->symbolCols, 0, DmtxDirUp);
   if(errors < 0 || errors > 2)
      return DmtxFail;

   return DmtxPass;
}

/**
 * \brief  Count the number of number of transitions between light and dark
 * \param  img
 * \param  reg
 * \param  xStart
 * \param  yStart
 * \param  dir
 * \return Jump count
 */
static int
CountJumpTally(DmtxDecode *dec, DmtxRegion *reg, int xStart, int yStart, DmtxDirection dir)
{
   int x, xInc = 0;
   int y, yInc = 0;
   int state = DmtxModuleOn;
   int jumpCount = 0;
   int jumpThreshold;
   int tModule, tPrev;
   int darkOnLight;
   int color;

   assert(xStart == 0 || yStart == 0);
   assert(dir == DmtxDirRight || dir == DmtxDirUp);

   if(dir == DmtxDirRight)
      xInc = 1;
   else
      yInc = 1;

   if(xStart == -1 || xStart == reg->symbolCols ||
         yStart == -1 || yStart == reg->symbolRows)
      state = DmtxModuleOff;

   darkOnLight = (int)(reg->offColor > reg->onColor);
   jumpThreshold = abs((int)(0.4 * (reg->onColor - reg->offColor) + 0.5));
   color = ReadModuleColor(dec, reg, yStart, xStart, reg->sizeIdx, reg->flowBegin.plane);
   tModule = (darkOnLight) ? reg->offColor - color : color - reg->offColor;

   for(x = xStart + xInc, y = yStart + yInc;
         (dir == DmtxDirRight && x < reg->symbolCols) ||
         (dir == DmtxDirUp && y < reg->symbolRows);
         x += xInc, y += yInc) {

      tPrev = tModule;
      color = ReadModuleColor(dec, reg, y, x, reg->sizeIdx, reg->flowBegin.plane);
      tModule = (darkOnLight) ? reg->offColor - color : color - reg->offColor;

      if(state == DmtxModuleOff) {
         if(tModule > tPrev + jumpThreshold) {
            jumpCount++;
            state = DmtxModuleOn;
         }
      }
      else {
         if(tModule < tPrev - jumpThreshold) {
            jumpCount++;
            state = DmtxModuleOff;
         }
      }
   }

   return jumpCount;
}

/**
 *
 *
 */
static DmtxPointFlow
GetPointFlow(DmtxDecode *dec, int colorPlane, DmtxPixelLoc loc, int arrive)
{
   static const int coefficient[] = {  0,  1,  2,  1,  0, -1, -2, -1 };
   int err;
   int patternIdx, coefficientIdx;
   int compass, compassMax;
   int mag[4] = { 0 };
   int xAdjust, yAdjust;
   int color, colorPattern[8];
   DmtxPointFlow flow;
    
    // check boundary conditions outside of the loop
    if (loc.X <= 0 || loc.Y <= 0 || loc.X >= dec->image->width-1 || loc.Y >= dec->image->height-1)
        return dmtxBlankEdge; // one or more pixels are past an edge
    if (dec->image->channelCount == 1) // grayscale, do it quicker
    {
        uint8_t *s;
        s = &dec->image->pxl[(dec->image->height - 1 - loc.Y) * dec->image->rowSizeBytes + loc.X];
        for (patternIdx=0; patternIdx < 8; patternIdx++)
        {
            colorPattern[patternIdx] = s[dmtxPatternX[patternIdx] - dmtxPatternY[patternIdx] * dec->image->rowSizeBytes];
        }
    }
    else
    {
       for(patternIdx = 0; patternIdx < 8; patternIdx++) {
          xAdjust = loc.X + dmtxPatternX[patternIdx];
          yAdjust = loc.Y + dmtxPatternY[patternIdx];
          err = dmtxDecodeGetPixelValue(dec, xAdjust, yAdjust, colorPlane,
                &colorPattern[patternIdx]);
          if(err == DmtxFail)
             return dmtxBlankEdge;
       }
    }

   /* Calculate this pixel's flow intensity for each direction (-45, 0, 45, 90) */
   compassMax = 0;
   for(compass = 0; compass < 4; compass++) {

      /* Add portion from each position in the convolution matrix pattern */
      for(patternIdx = 0; patternIdx < 8; patternIdx++) {

         color = colorPattern[patternIdx];
         coefficientIdx = (patternIdx - compass + 8) % 8;
//         if(coefficient[coefficientIdx] == 0)
//            continue;

         mag[compass] += color * coefficient[coefficientIdx];
      }

      /* Identify strongest compass flow */
      if(compass != 0 && abs(mag[compass]) > abs(mag[compassMax]))
         compassMax = compass;
   }

   /* Convert signed compass direction into unique flow directions (0-7) */
   flow.plane = colorPlane;
   flow.arrive = arrive;
   flow.depart = (mag[compassMax] > 0) ? compassMax + 4 : compassMax;
   flow.mag = abs(mag[compassMax]);
   flow.loc = loc;

   return flow;
}

/**
 *
 *
 */
static DmtxPointFlow
FindStrongestNeighbor(DmtxDecode *dec, DmtxPointFlow center, int sign)
{
   int i;
   int strongIdx;
   int attempt, attemptDiff;
   int occupied;
   unsigned char *cache;
   DmtxPixelLoc loc;
   DmtxPointFlow flow[8];

   attempt = (sign < 0) ? center.depart : (center.depart+4)%8;

   occupied = 0;
   strongIdx = DmtxUndefined;
   for(i = 0; i < 8; i++) {

      loc.X = center.loc.X + dmtxPatternX[i];
      loc.Y = center.loc.Y + dmtxPatternY[i];

      cache = dmtxDecodeGetCache(dec, loc.X, loc.Y);
      if(cache == NULL)
         continue;

      if((int)(*cache & 0x80) != 0x00) {
         if(++occupied > 2)
            return dmtxBlankEdge;
         else
            continue;
      }

      attemptDiff = abs(attempt - i);
      if(attemptDiff > 4)
         attemptDiff = 8 - attemptDiff;
      if(attemptDiff > 1)
         continue;

      flow[i] = GetPointFlow(dec, center.plane, loc, i);

      if(strongIdx == DmtxUndefined || flow[i].mag > flow[strongIdx].mag ||
            (flow[i].mag == flow[strongIdx].mag && ((i & 0x01) != 0))) {
         strongIdx = i;
      }
   }

   return (strongIdx == DmtxUndefined) ? dmtxBlankEdge : flow[strongIdx];
}

/**
 *
 *
 */
static DmtxFollow
FollowSeek(DmtxDecode *dec, DmtxRegion *reg, int seek)
{
   int i;
   int sign;
   DmtxFollow follow;

   follow.loc = reg->flowBegin.loc;
   follow.step = 0;
   follow.ptr = dmtxDecodeGetCache(dec, follow.loc.X, follow.loc.Y);
   assert(follow.ptr != NULL);
   follow.neighbor = *follow.ptr;

   sign = (seek > 0) ? +1 : -1;
   for(i = 0; i != seek; i += sign) {
      follow = FollowStep(dec, reg, follow, sign);
      assert(follow.ptr != NULL);
      assert(abs(follow.step) <= reg->stepsTotal);
   }

   return follow;
}

/**
 *
 *
 */
static DmtxFollow
FollowSeekLoc(DmtxDecode *dec, DmtxPixelLoc loc)
{
   DmtxFollow follow;

   follow.loc = loc;
   follow.step = 0;
   follow.ptr = dmtxDecodeGetCache(dec, follow.loc.X, follow.loc.Y);
   assert(follow.ptr != NULL);
   follow.neighbor = *follow.ptr;

   return follow;
}


/**
 *
 *
 */
static DmtxFollow
FollowStep(DmtxDecode *dec, DmtxRegion *reg, DmtxFollow followBeg, int sign)
{
   int patternIdx;
   int stepMod;
   int factor;
   DmtxFollow follow;

   assert(abs(sign) == 1);
   assert((int)(followBeg.neighbor & 0x40) != 0x00);

   factor = reg->stepsTotal + 1;
   if(sign > 0)
      stepMod = (factor + (followBeg.step % factor)) % factor;
   else
      stepMod = (factor - (followBeg.step % factor)) % factor;

   /* End of positive trail -- magic jump */
   if(sign > 0 && stepMod == reg->jumpToNeg) {
      follow.loc = reg->finalNeg;
   }
   /* End of negative trail -- magic jump */
   else if(sign < 0 && stepMod == reg->jumpToPos) {
      follow.loc = reg->finalPos;
   }
   /* Trail in progress -- normal jump */
   else {
      patternIdx = (sign < 0) ? followBeg.neighbor & 0x07 : ((followBeg.neighbor & 0x38) >> 3);
      follow.loc.X = followBeg.loc.X + dmtxPatternX[patternIdx];
      follow.loc.Y = followBeg.loc.Y + dmtxPatternY[patternIdx];
   }

   follow.step = followBeg.step + sign;
   follow.ptr = dmtxDecodeGetCache(dec, follow.loc.X, follow.loc.Y);
   assert(follow.ptr != NULL);
   follow.neighbor = *follow.ptr;

   return follow;
}

/**
 *
 *
 */
static DmtxFollow
FollowStep2(DmtxDecode *dec, DmtxFollow followBeg, int sign)
{
   int patternIdx;
   DmtxFollow follow;

   assert(abs(sign) == 1);
   assert((int)(followBeg.neighbor & 0x40) != 0x00);

   patternIdx = (sign < 0) ? followBeg.neighbor & 0x07 : ((followBeg.neighbor & 0x38) >> 3);
   follow.loc.X = followBeg.loc.X + dmtxPatternX[patternIdx];
   follow.loc.Y = followBeg.loc.Y + dmtxPatternY[patternIdx];

   follow.step = followBeg.step + sign;
   follow.ptr = dmtxDecodeGetCache(dec, follow.loc.X, follow.loc.Y);
   assert(follow.ptr != NULL);
   follow.neighbor = *follow.ptr;

   return follow;
}

/**
 * vaiiiooo
 * --------
 * 0x80 v = visited bit
 * 0x40 a = assigned bit
 * 0x38 u = 3 bits points upstream 0-7
 * 0x07 d = 3 bits points downstream 0-7
 */
static DmtxPassFail
TrailBlazeContinuous(DmtxDecode *dec, DmtxRegion *reg, DmtxPointFlow flowBegin, int maxDiagonal)
{
   int posAssigns, negAssigns, clears;
   int sign;
   int steps;
   unsigned char *cache, *cacheNext, *cacheBeg;
   DmtxPointFlow flow, flowNext;
   DmtxPixelLoc boundMin, boundMax;

   boundMin = boundMax = flowBegin.loc;
   cacheBeg = dmtxDecodeGetCache(dec, flowBegin.loc.X, flowBegin.loc.Y);
   if(cacheBeg == NULL)
      return DmtxFail;
   *cacheBeg = (0x80 | 0x40); /* Mark location as visited and assigned */

   reg->flowBegin = flowBegin;

   posAssigns = negAssigns = 0;
   for(sign = 1; sign >= -1; sign -= 2) {

      flow = flowBegin;
      cache = cacheBeg;

      for(steps = 0; ; steps++) {

         if(maxDiagonal != DmtxUndefined && (boundMax.X - boundMin.X > maxDiagonal ||
               boundMax.Y - boundMin.Y > maxDiagonal))
            break;

         /* Find the strongest eligible neighbor */
         flowNext = FindStrongestNeighbor(dec, flow, sign);
         if(flowNext.mag < 50)
            break;

         /* Get the neighbor's cache location */
         cacheNext = dmtxDecodeGetCache(dec, flowNext.loc.X, flowNext.loc.Y);
         if(cacheNext == NULL)
            break;
         assert(!(*cacheNext & 0x80));

         /* Mark departure from current location. If flowing downstream
          * (sign < 0) then departure vector here is the arrival vector
          * of the next location. Upstream flow uses the opposite rule. */
         *cache |= (sign < 0) ? flowNext.arrive : flowNext.arrive << 3;

         /* Mark known direction for next location */
         /* If testing downstream (sign < 0) then next upstream is opposite of next arrival */
         /* If testing upstream (sign > 0) then next downstream is opposite of next arrival */
         *cacheNext = (sign < 0) ? (((flowNext.arrive + 4)%8) << 3) : ((flowNext.arrive + 4)%8);
         *cacheNext |= (0x80 | 0x40); /* Mark location as visited and assigned */
         if(sign > 0)
            posAssigns++;
         else
            negAssigns++;
         cache = cacheNext;
         flow = flowNext;

         if(flow.loc.X > boundMax.X)
            boundMax.X = flow.loc.X;
         else if(flow.loc.X < boundMin.X)
            boundMin.X = flow.loc.X;
         if(flow.loc.Y > boundMax.Y)
            boundMax.Y = flow.loc.Y;
         else if(flow.loc.Y < boundMin.Y)
            boundMin.Y = flow.loc.Y;

/*       CALLBACK_POINT_PLOT(flow.loc, (sign > 0) ? 2 : 3, 1, 2); */
      }

      if(sign > 0) {
         reg->finalPos = flow.loc;
         reg->jumpToNeg = steps;
      }
      else {
         reg->finalNeg = flow.loc;
         reg->jumpToPos = steps;
      }
   }
   reg->stepsTotal = reg->jumpToPos + reg->jumpToNeg;
   reg->boundMin = boundMin;
   reg->boundMax = boundMax;

   /* Clear "visited" bit from trail */
   clears = TrailClear(dec, reg, 0x80);
   assert(posAssigns + negAssigns == clears - 1);

   /* XXX clean this up ... redundant test above */
   if(maxDiagonal != DmtxUndefined && (boundMax.X - boundMin.X > maxDiagonal ||
         boundMax.Y - boundMin.Y > maxDiagonal))
      return DmtxFail;

   return DmtxPass;
}

/**
 * recives bresline, and follows strongest neighbor unless it involves
 * ratcheting bresline inward or backward (although back + outward is allowed).
 *
 */
static int
TrailBlazeGapped(DmtxDecode *dec, DmtxRegion *reg, DmtxBresLine line, int streamDir)
{
   unsigned char *beforeCache, *afterCache;
   DmtxBoolean onEdge;
   int distSq, distSqMax;
   int travel, outward;
   int xDiff, yDiff;
   int steps;
   int stepDir, dirMap[] = { 0, 1, 2, 7, 8, 3, 6, 5, 4 };
   DmtxPassFail err;
   DmtxPixelLoc beforeStep, afterStep;
   DmtxPointFlow flow, flowNext;
   DmtxPixelLoc loc0;
   int xStep, yStep;

   loc0 = line.loc;
   flow = GetPointFlow(dec, reg->flowBegin.plane, loc0, dmtxNeighborNone);
   distSqMax = (line.xDelta * line.xDelta) + (line.yDelta * line.yDelta);
   steps = 0;
   onEdge = DmtxTrue;

   beforeStep = loc0;
   beforeCache = dmtxDecodeGetCache(dec, loc0.X, loc0.Y);
   if(beforeCache == NULL)
      return DmtxFail;
   else
      *beforeCache = 0x00; /* probably should just overwrite one direction */

   do {
      if(onEdge == DmtxTrue) {
         flowNext = FindStrongestNeighbor(dec, flow, streamDir);
         if(flowNext.mag == DmtxUndefined)
            break;

         err = BresLineGetStep(line, flowNext.loc, &travel, &outward);
         if(flowNext.mag < 50 || outward < 0 || (outward == 0 && travel < 0)) {
            onEdge = DmtxFalse;
         }
         else {
            BresLineStep(&line, travel, outward);
            flow = flowNext;
         }
      }

      if(onEdge == DmtxFalse) {
         BresLineStep(&line, 1, 0);
         flow = GetPointFlow(dec, reg->flowBegin.plane, line.loc, dmtxNeighborNone);
         if(flow.mag > 50)
            onEdge = DmtxTrue;
      }

      afterStep = line.loc;
      afterCache = dmtxDecodeGetCache(dec, afterStep.X, afterStep.Y);
      if(afterCache == NULL)
         break;

      /* Determine step direction using pure magic */
      xStep = afterStep.X - beforeStep.X;
      yStep = afterStep.Y - beforeStep.Y;
      assert(abs(xStep) <= 1 && abs(yStep) <= 1);
      stepDir = dirMap[3 * yStep + xStep + 4];
      assert(stepDir != 8);

      if(streamDir < 0) {
         *beforeCache |= (0x40 | stepDir);
         *afterCache = (((stepDir + 4)%8) << 3);
      }
      else {
         *beforeCache |= (0x40 | (stepDir << 3));
         *afterCache = ((stepDir + 4)%8);
      }

      /* Guaranteed to have taken one step since top of loop */
      xDiff = line.loc.X - loc0.X;
      yDiff = line.loc.Y - loc0.Y;
      distSq = (xDiff * xDiff) + (yDiff * yDiff);

      beforeStep = line.loc;
      beforeCache = afterCache;
      steps++;

   } while(distSq < distSqMax);

   return steps;
}

/**
 *
 *
 */
static int
TrailClear(DmtxDecode *dec, DmtxRegion *reg, int clearMask)
{
   int clears;
   DmtxFollow follow;

   assert((clearMask | 0xff) == 0xff);

   /* Clear "visited" bit from trail */
   clears = 0;
   follow = FollowSeek(dec, reg, 0);
   while(abs(follow.step) <= reg->stepsTotal) {
      assert((int)(*follow.ptr & clearMask) != 0x00);
      *follow.ptr &= (clearMask ^ 0xff);
      follow = FollowStep(dec, reg, follow, +1);
      clears++;
   }

   return clears;
}

/**
 *
 *
 */
static DmtxBestLine
FindBestSolidLine(DmtxDecode *dec, DmtxRegion *reg, int step0, int step1, int streamDir, int houghAvoid)
{
   int *hough_temp = calloc(3 * DMTX_HOUGH_RES, sizeof(int)); int (*hough)[DMTX_HOUGH_RES] = (int (*)[DMTX_HOUGH_RES]) hough_temp; // [3][DMTX_HOUGH_RES] = { { 0 } };
   int houghMin, houghMax;
   char *houghTest = malloc(DMTX_HOUGH_RES); // [DMTX_HOUGH_RES];
   int i;
   int step;
   int sign;
   int tripSteps;
   int angleBest;
   int hOffset, hOffsetBest;
   int xDiff, yDiff;
   int dH;
   DmtxRay2 rH;
   DmtxFollow follow;
   DmtxBestLine line;
   DmtxPixelLoc rHp;

   memset(&line, 0x00, sizeof(DmtxBestLine));
   memset(&rH, 0x00, sizeof(DmtxRay2));
   angleBest = 0;
   hOffset = hOffsetBest = 0;

   /* Always follow path flowing away from the trail start */
   if(step0 != 0) {
      if(step0 > 0) {
         sign = +1;
         tripSteps = (step1 - step0 + reg->stepsTotal) % reg->stepsTotal;
      }
      else {
         sign = -1;
         tripSteps = (step0 - step1 + reg->stepsTotal) % reg->stepsTotal;
      }
      if(tripSteps == 0)
         tripSteps = reg->stepsTotal;
   }
   else if(step1 != 0) {
      sign = (step1 > 0) ? +1 : -1;
      tripSteps = abs(step1);
   }
   else if(step1 == 0) {
      sign = +1;
      tripSteps = reg->stepsTotal;
   }
   assert(sign == streamDir);

   follow = FollowSeek(dec, reg, step0);
   rHp = follow.loc;

   line.stepBeg = line.stepPos = line.stepNeg = step0;
   line.locBeg = follow.loc;
   line.locPos = follow.loc;
   line.locNeg = follow.loc;

   /* Predetermine which angles to test */
   for(i = 0; i < DMTX_HOUGH_RES; i++) {
      if(houghAvoid == DmtxUndefined) {
         houghTest[i] = 1;
      }
      else {
         houghMin = (houghAvoid + DMTX_HOUGH_RES/6) % DMTX_HOUGH_RES;
         houghMax = (houghAvoid - DMTX_HOUGH_RES/6 + DMTX_HOUGH_RES) % DMTX_HOUGH_RES;
         if(houghMin > houghMax)
            houghTest[i] = (i > houghMin || i < houghMax) ? 1 : 0;
         else
            houghTest[i] = (i > houghMin && i < houghMax) ? 1 : 0;
      }
   }

   /* Test each angle for steps along path */
   for(step = 0; step < tripSteps; step++) {

      xDiff = follow.loc.X - rHp.X;
      yDiff = follow.loc.Y - rHp.Y;

      /* Increment Hough accumulator */
      for(i = 0; i < DMTX_HOUGH_RES; i++) {

         if((int)houghTest[i] == 0)
            continue;

         dH = (rHvX[i] * yDiff) - (rHvY[i] * xDiff);
         if(dH >= -384 && dH <= 384) {

            if(dH > 128)
               hOffset = 2;
            else if(dH >= -128)
               hOffset = 1;
            else
               hOffset = 0;

            hough[hOffset][i]++;

            /* New angle takes over lead */
            if(hough[hOffset][i] > hough[hOffsetBest][angleBest]) {
               angleBest = i;
               hOffsetBest = hOffset;
            }
         }
      }

/*    CALLBACK_POINT_PLOT(follow.loc, (sign > 1) ? 4 : 3, 1, 2); */

      follow = FollowStep(dec, reg, follow, sign);
   }

   line.angle = angleBest;
   line.hOffset = hOffsetBest;
   line.mag = hough[hOffsetBest][angleBest];

   free(houghTest);
   free(hough_temp);

   return line;
}

/**
 *
 *
 */
static DmtxBestLine
FindBestSolidLine2(DmtxDecode *dec, DmtxPixelLoc loc0, int tripSteps, int sign, int houghAvoid)
{
   int *hough_temp = calloc(3 * DMTX_HOUGH_RES, sizeof(int)); int (*hough)[DMTX_HOUGH_RES] = (int (*)[DMTX_HOUGH_RES]) hough_temp; // [3][DMTX_HOUGH_RES] = { { 0 } };
   int houghMin, houghMax;
   char *houghTest = malloc(DMTX_HOUGH_RES); // [DMTX_HOUGH_RES];
   int i;
   int step;
   int angleBest;
   int hOffset, hOffsetBest;
   int xDiff, yDiff;
   int dH;
   DmtxRay2 rH;
   DmtxBestLine line;
   DmtxPixelLoc rHp;
   DmtxFollow follow;

   memset(&line, 0x00, sizeof(DmtxBestLine));
   memset(&rH, 0x00, sizeof(DmtxRay2));
   angleBest = 0;
   hOffset = hOffsetBest = 0;

   follow = FollowSeekLoc(dec, loc0);
   rHp = line.locBeg = line.locPos = line.locNeg = follow.loc;
   line.stepBeg = line.stepPos = line.stepNeg = 0;

   /* Predetermine which angles to test */
   for(i = 0; i < DMTX_HOUGH_RES; i++) {
      if(houghAvoid == DmtxUndefined) {
         houghTest[i] = 1;
      }
      else {
         houghMin = (houghAvoid + DMTX_HOUGH_RES/6) % DMTX_HOUGH_RES;
         houghMax = (houghAvoid - DMTX_HOUGH_RES/6 + DMTX_HOUGH_RES) % DMTX_HOUGH_RES;
         if(houghMin > houghMax)
            houghTest[i] = (i > houghMin || i < houghMax) ? 1 : 0;
         else
            houghTest[i] = (i > houghMin && i < houghMax) ? 1 : 0;
      }
   }

   /* Test each angle for steps along path */
   for(step = 0; step < tripSteps; step++) {

      xDiff = follow.loc.X - rHp.X;
      yDiff = follow.loc.Y - rHp.Y;

      /* Increment Hough accumulator */
      for(i = 0; i < DMTX_HOUGH_RES; i++) {

         if((int)houghTest[i] == 0)
            continue;

         dH = (rHvX[i] * yDiff) - (rHvY[i] * xDiff);
         if(dH >= -384 && dH <= 384) {
            if(dH > 128)
               hOffset = 2;
            else if(dH >= -128)
               hOffset = 1;
            else
               hOffset = 0;

            hough[hOffset][i]++;

            /* New angle takes over lead */
            if(hough[hOffset][i] > hough[hOffsetBest][angleBest]) {
               angleBest = i;
               hOffsetBest = hOffset;
            }
         }
      }

/*    CALLBACK_POINT_PLOT(follow.loc, (sign > 1) ? 4 : 3, 1, 2); */

      follow = FollowStep2(dec, follow, sign);
   }

   line.angle = angleBest;
   line.hOffset = hOffsetBest;
   line.mag = hough[hOffsetBest][angleBest];

   free(houghTest);
   free(hough_temp);

   return line;
}

/**
 *
 *
 */
static DmtxPassFail
FindTravelLimits(DmtxDecode *dec, DmtxRegion *reg, DmtxBestLine *line)
{
   int i;
   int distSq, distSqMax;
   int xDiff, yDiff;
   int posRunning, negRunning;
   int posTravel, negTravel;
   int posWander, posWanderMin, posWanderMax, posWanderMinLock, posWanderMaxLock;
   int negWander, negWanderMin, negWanderMax, negWanderMinLock, negWanderMaxLock;
   int cosAngle, sinAngle;
   DmtxFollow followPos, followNeg;
   DmtxPixelLoc loc0, posMax, negMax;

   /* line->stepBeg is already known to sit on the best Hough line */
   followPos = followNeg = FollowSeek(dec, reg, line->stepBeg);
   loc0 = followPos.loc;

   cosAngle = rHvX[line->angle];
   sinAngle = rHvY[line->angle];

   distSqMax = 0;
   posMax = negMax = followPos.loc;

   posTravel = negTravel = 0;
   posWander = posWanderMin = posWanderMax = posWanderMinLock = posWanderMaxLock = 0;
   negWander = negWanderMin = negWanderMax = negWanderMinLock = negWanderMaxLock = 0;

   for(i = 0; i < reg->stepsTotal/2; i++) {

      posRunning = (int)(i < 10 || abs(posWander) < abs(posTravel));
      negRunning = (int)(i < 10 || abs(negWander) < abs(negTravel));

      if(posRunning != 0) {
         xDiff = followPos.loc.X - loc0.X;
         yDiff = followPos.loc.Y - loc0.Y;
         posTravel = (cosAngle * xDiff) + (sinAngle * yDiff);
         posWander = (cosAngle * yDiff) - (sinAngle * xDiff);

         if(posWander >= -3*256 && posWander <= 3*256) {
            distSq = DistanceSquared(followPos.loc, negMax);
            if(distSq > distSqMax) {
               posMax = followPos.loc;
               distSqMax = distSq;
               line->stepPos = followPos.step;
               line->locPos = followPos.loc;
               posWanderMinLock = posWanderMin;
               posWanderMaxLock = posWanderMax;
            }
         }
         else {
            posWanderMin = min(posWanderMin, posWander);
            posWanderMax = max(posWanderMax, posWander);
         }
      }
      else if(!negRunning) {
         break;
      }

      if(negRunning != 0) {
         xDiff = followNeg.loc.X - loc0.X;
         yDiff = followNeg.loc.Y - loc0.Y;
         negTravel = (cosAngle * xDiff) + (sinAngle * yDiff);
         negWander = (cosAngle * yDiff) - (sinAngle * xDiff);

         if(negWander >= -3*256 && negWander < 3*256) {
            distSq = DistanceSquared(followNeg.loc, posMax);
            if(distSq > distSqMax) {
               negMax = followNeg.loc;
               distSqMax = distSq;
               line->stepNeg = followNeg.step;
               line->locNeg = followNeg.loc;
               negWanderMinLock = negWanderMin;
               negWanderMaxLock = negWanderMax;
            }
         }
         else {
            negWanderMin = min(negWanderMin, negWander);
            negWanderMax = max(negWanderMax, negWander);
         }
      }
      else if(!posRunning) {
         break;
      }

/*  CALLBACK_POINT_PLOT(followPos.loc, 2, 1, 2);
    CALLBACK_POINT_PLOT(followNeg.loc, 4, 1, 2); */

      followPos = FollowStep(dec, reg, followPos, +1);
      followNeg = FollowStep(dec, reg, followNeg, -1);
   }
   line->devn = max(posWanderMaxLock - posWanderMinLock, negWanderMaxLock - negWanderMinLock)/256;
   line->distSq = distSqMax;

/* CALLBACK_POINT_PLOT(posMax, 2, 1, 1);
   CALLBACK_POINT_PLOT(negMax, 2, 1, 1); */

   return DmtxPass;
}

/**
 *
 *
 */
static DmtxPassFail
MatrixRegionAlignCalibEdge(DmtxDecode *dec, DmtxRegion *reg, int edgeLoc)
{
   int streamDir;
   int steps;
   int avoidAngle;
   int symbolShape;
   DmtxVector2 pTmp;
   DmtxPixelLoc loc0, loc1, locOrigin;
   DmtxBresLine line;
   DmtxFollow follow;
   DmtxBestLine bestLine;

   /* Determine pixel coordinates of origin */
   pTmp.X = 0.0;
   pTmp.Y = 0.0;
   dmtxMatrix3VMultiplyBy(&pTmp, reg->fit2raw);
   locOrigin.X = (int)(pTmp.X + 0.5);
   locOrigin.Y = (int)(pTmp.Y + 0.5);

   if(dec->sizeIdxExpected == DmtxSymbolSquareAuto ||
         (dec->sizeIdxExpected >= DmtxSymbol10x10 &&
         dec->sizeIdxExpected <= DmtxSymbol144x144))
      symbolShape = DmtxSymbolSquareAuto;
   else if(dec->sizeIdxExpected == DmtxSymbolRectAuto ||
         (dec->sizeIdxExpected >= DmtxSymbol8x18 &&
         dec->sizeIdxExpected <= DmtxSymbol16x48))
      symbolShape = DmtxSymbolRectAuto;
   else
      symbolShape = DmtxSymbolShapeAuto;

   /* Determine end locations of test line */
   if(edgeLoc == DmtxEdgeTop) {
      streamDir = reg->polarity * -1;
      avoidAngle = reg->leftLine.angle;
      follow = FollowSeekLoc(dec, reg->locT);
      pTmp.X = 0.8;
      pTmp.Y = (symbolShape == DmtxSymbolRectAuto) ? 0.2 : 0.6;
   }
   else {
      assert(edgeLoc == DmtxEdgeRight);
      streamDir = reg->polarity;
      avoidAngle = reg->bottomLine.angle;
      follow = FollowSeekLoc(dec, reg->locR);
      pTmp.X = (symbolShape == DmtxSymbolSquareAuto) ? 0.7 : 0.9;
      pTmp.Y = 0.8;
   }

   dmtxMatrix3VMultiplyBy(&pTmp, reg->fit2raw);
   loc1.X = (int)(pTmp.X + 0.5);
   loc1.Y = (int)(pTmp.Y + 0.5);

   loc0 = follow.loc;
   line = BresLineInit(loc0, loc1, locOrigin);
   steps = TrailBlazeGapped(dec, reg, line, streamDir);

   bestLine = FindBestSolidLine2(dec, loc0, steps, streamDir, avoidAngle);
   if(bestLine.mag < 5) {
      ;
   }

   if(edgeLoc == DmtxEdgeTop) {
      reg->topKnown = 1;
      reg->topAngle = bestLine.angle;
      reg->topLoc = bestLine.locBeg;
   }
   else {
      reg->rightKnown = 1;
      reg->rightAngle = bestLine.angle;
      reg->rightLoc = bestLine.locBeg;
   }

   return DmtxPass;
}

/**
 *
 *
 */
static DmtxBresLine
BresLineInit(DmtxPixelLoc loc0, DmtxPixelLoc loc1, DmtxPixelLoc locInside)
{
   int cp;
   DmtxBresLine line;
   DmtxPixelLoc *locBeg, *locEnd;

   /* XXX Verify that loc0 and loc1 are inbounds */

   /* Values that stay the same after initialization */
   line.loc0 = loc0;
   line.loc1 = loc1;
   line.xStep = (loc0.X < loc1.X) ? +1 : -1;
   line.yStep = (loc0.Y < loc1.Y) ? +1 : -1;
   line.xDelta = abs(loc1.X - loc0.X);
   line.yDelta = abs(loc1.Y - loc0.Y);
   line.steep = (int)(line.yDelta > line.xDelta);

   /* Take cross product to determine outward step */
   if(line.steep != 0) {
      /* Point first vector up to get correct sign */
      if(loc0.Y < loc1.Y) {
         locBeg = &loc0;
         locEnd = &loc1;
      }
      else {
         locBeg = &loc1;
         locEnd = &loc0;
      }
      cp = (((locEnd->X - locBeg->X) * (locInside.Y - locEnd->Y)) -
            ((locEnd->Y - locBeg->Y) * (locInside.X - locEnd->X)));

      line.xOut = (cp > 0) ? +1 : -1;
      line.yOut = 0;
   }
   else {
      /* Point first vector left to get correct sign */
      if(loc0.X > loc1.X) {
         locBeg = &loc0;
         locEnd = &loc1;
      }
      else {
         locBeg = &loc1;
         locEnd = &loc0;
      }
      cp = (((locEnd->X - locBeg->X) * (locInside.Y - locEnd->Y)) -
            ((locEnd->Y - locBeg->Y) * (locInside.X - locEnd->X)));

      line.xOut = 0;
      line.yOut = (cp > 0) ? +1 : -1;
   }

   /* Values that change while stepping through line */
   line.loc = loc0;
   line.travel = 0;
   line.outward = 0;
   line.error = (line.steep) ? line.yDelta/2 : line.xDelta/2;

/* CALLBACK_POINT_PLOT(loc0, 3, 1, 1);
   CALLBACK_POINT_PLOT(loc1, 3, 1, 1); */

   return line;
}

/**
 *
 *
 */
static DmtxPassFail
BresLineGetStep(DmtxBresLine line, DmtxPixelLoc target, int *travel, int *outward)
{
   /* Determine necessary step along and outward from Bresenham line */
   if(line.steep != 0) {
      *travel = (line.yStep > 0) ? target.Y - line.loc.Y : line.loc.Y - target.Y;
      BresLineStep(&line, *travel, 0);
      *outward = (line.xOut > 0) ? target.X - line.loc.X : line.loc.X - target.X;
      assert(line.yOut == 0);
   }
   else {
      *travel = (line.xStep > 0) ? target.X - line.loc.X : line.loc.X - target.X;
      BresLineStep(&line, *travel, 0);
      *outward = (line.yOut > 0) ? target.Y - line.loc.Y : line.loc.Y - target.Y;
      assert(line.xOut == 0);
   }

   return DmtxPass;
}

/**
 *
 *
 */
static DmtxPassFail
BresLineStep(DmtxBresLine *line, int travel, int outward)
{
   int i;
   DmtxBresLine lineNew;

   lineNew = *line;

   assert(abs(travel) < 2);
   assert(abs(outward) >= 0);

   /* Perform forward step */
   if(travel > 0) {
      lineNew.travel++;
      if(lineNew.steep != 0) {
         lineNew.loc.Y += lineNew.yStep;
         lineNew.error -= lineNew.xDelta;
         if(lineNew.error < 0) {
            lineNew.loc.X += lineNew.xStep;
            lineNew.error += lineNew.yDelta;
         }
      }
      else {
         lineNew.loc.X += lineNew.xStep;
         lineNew.error -= lineNew.yDelta;
         if(lineNew.error < 0) {
            lineNew.loc.Y += lineNew.yStep;
            lineNew.error += lineNew.xDelta;
         }
      }
   }
   else if(travel < 0) {
      lineNew.travel--;
      if(lineNew.steep != 0) {
         lineNew.loc.Y -= lineNew.yStep;
         lineNew.error += lineNew.xDelta;
         if(lineNew.error >= lineNew.yDelta) {
            lineNew.loc.X -= lineNew.xStep;
            lineNew.error -= lineNew.yDelta;
         }
      }
      else {
         lineNew.loc.X -= lineNew.xStep;
         lineNew.error += lineNew.yDelta;
         if(lineNew.error >= lineNew.xDelta) {
            lineNew.loc.Y -= lineNew.yStep;
            lineNew.error -= lineNew.xDelta;
         }
      }
   }

   for(i = 0; i < outward; i++) {
      /* Outward steps */
      lineNew.outward++;
      lineNew.loc.X += lineNew.xOut;
      lineNew.loc.Y += lineNew.yOut;
   }

   *line = lineNew;

   return DmtxPass;
}

/**
 *
 *
 */
#ifdef NOTDEFINED
static void
WriteDiagnosticImage(DmtxDecode *dec, DmtxRegion *reg, char *imagePath)
{
   int row, col;
   int width, height;
   unsigned char *cache;
   int rgb[3];
   FILE *fp;
   DmtxVector2 p;
   DmtxImage *img;

   assert(reg != NULL);

   fp = fopen(imagePath, "wb");
   if(fp == NULL) {
      exit(3);
   }

   width = dmtxDecodeGetProp(dec, DmtxPropWidth);
   height = dmtxDecodeGetProp(dec->image, DmtxPropHeight);

   img = dmtxImageCreate(NULL, width, height, DmtxPack24bppRGB);

   /* Populate image */
   for(row = 0; row < height; row++) {
      for(col = 0; col < width; col++) {

         cache = dmtxDecodeGetCache(dec, col, row);
         if(cache == NULL) {
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 128;
         }
         else {
            dmtxDecodeGetPixelValue(dec, col, row, 0, &rgb[0]);
            dmtxDecodeGetPixelValue(dec, col, row, 1, &rgb[1]);
            dmtxDecodeGetPixelValue(dec, col, row, 2, &rgb[2]);

            p.X = col;
            p.Y = row;
            dmtxMatrix3VMultiplyBy(&p, reg->raw2fit);

            if(p.X < 0.0 || p.X > 1.0 || p.Y < 0.0 || p.Y > 1.0) {
               rgb[0] = 0;
               rgb[1] = 0;
               rgb[2] = 128;
            }
            else if(p.X + p.Y > 1.0) {
               rgb[0] += (0.4 * (255 - rgb[0]));
               rgb[1] += (0.4 * (255 - rgb[1]));
               rgb[2] += (0.4 * (255 - rgb[2]));
            }
         }

         dmtxImageSetRgb(img, col, row, rgb);
      }
   }

   /* Write additional markers */
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;
   dmtxImageSetRgb(img, reg->topLoc.X, reg->topLoc.Y, rgb);
   dmtxImageSetRgb(img, reg->rightLoc.X, reg->rightLoc.Y, rgb);

   /* Write image to PNM file */
   fprintf(fp, "P6\n%d %d\n255\n", width, height);
   for(row = height - 1; row >= 0; row--) {
      for(col = 0; col < width; col++) {
         dmtxImageGetRgb(img, col, row, rgb);
         fwrite(rgb, sizeof(char), 3, fp);
      }
   }

   dmtxImageDestroy(&img);

   fclose(fp);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxsymbol.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxsymbol.c
 * \brief Data Matrix symbol attributes
 */

/**
 * \brief  Retrieve property based on symbol size
 * \param  attribute
 * \param  sizeIdx
 * \return Attribute value
 */
extern int
dmtxGetSymbolAttribute(int attribute, int sizeIdx)
{
   static const int symbolRows[] = { 10, 12, 14, 16, 18, 20,  22,  24,  26,
                                                 32, 36, 40,  44,  48,  52,
                                                 64, 72, 80,  88,  96, 104,
                                                        120, 132, 144,
                                                  8,  8, 12,  12,  16,  16 };

   static const int symbolCols[] = { 10, 12, 14, 16, 18, 20,  22,  24,  26,
                                                 32, 36, 40,  44,  48,  52,
                                                 64, 72, 80,  88,  96, 104,
                                                        120, 132, 144,
                                                 18, 32, 26,  36,  36,  48 };

   static const int dataRegionRows[] = { 8, 10, 12, 14, 16, 18, 20, 22, 24,
                                                    14, 16, 18, 20, 22, 24,
                                                    14, 16, 18, 20, 22, 24,
                                                            18, 20, 22,
                                                     6,  6, 10, 10, 14, 14 };

   static const int dataRegionCols[] = { 8, 10, 12, 14, 16, 18, 20, 22, 24,
                                                    14, 16, 18, 20, 22, 24,
                                                    14, 16, 18, 20, 22, 24,
                                                            18, 20, 22,
                                                    16, 14, 24, 16, 16, 22 };

   static const int horizDataRegions[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                    2, 2, 2, 2, 2, 2,
                                                    4, 4, 4, 4, 4, 4,
                                                          6, 6, 6,
                                                    1, 2, 1, 2, 2, 2 };

   static const int interleavedBlocks[] = { 1, 1, 1, 1, 1, 1, 1,  1, 1,
                                                     1, 1, 1, 1,  1, 2,
                                                     2, 4, 4, 4,  4, 6,
                                                           6, 8, 10,
                                                     1, 1, 1, 1,  1, 1 };

   static const int symbolDataWords[] = { 3, 5, 8,  12,   18,   22,   30,   36,  44,
                                                    62,   86,  114,  144,  174, 204,
                                                   280,  368,  456,  576,  696, 816,
                                                              1050, 1304, 1558,
                                                     5,   10,   16,   22,   32,  49 };

   static const int blockErrorWords[] = { 5, 7, 10, 12, 14, 18, 20, 24, 28,
                                                    36, 42, 48, 56, 68, 42,
                                                    56, 36, 48, 56, 68, 56,
                                                            68, 62, 62,
                                                     7, 11, 14, 18, 24, 28 };

   static const int blockMaxCorrectable[] = { 2, 3, 5,  6,  7,  9,  10,  12,  14,
                                                       18, 21, 24,  28,  34,  21,
                                                       28, 18, 24,  28,  34,  28,
                                                               34,  31,  31,
                                                   3,  5,  7,   9,  12,  14 };

   if(sizeIdx < 0 || sizeIdx >= DmtxSymbolSquareCount + DmtxSymbolRectCount)
      return DmtxUndefined;

   switch(attribute) {
      case DmtxSymAttribSymbolRows:
         return symbolRows[sizeIdx];
      case DmtxSymAttribSymbolCols:
         return symbolCols[sizeIdx];
      case DmtxSymAttribDataRegionRows:
         return dataRegionRows[sizeIdx];
      case DmtxSymAttribDataRegionCols:
         return dataRegionCols[sizeIdx];
      case DmtxSymAttribHorizDataRegions:
         return horizDataRegions[sizeIdx];
      case DmtxSymAttribVertDataRegions:
         return (sizeIdx < DmtxSymbolSquareCount) ? horizDataRegions[sizeIdx] : 1;
      case DmtxSymAttribMappingMatrixRows:
         return dataRegionRows[sizeIdx] *
               dmtxGetSymbolAttribute(DmtxSymAttribVertDataRegions, sizeIdx);
      case DmtxSymAttribMappingMatrixCols:
         return dataRegionCols[sizeIdx] * horizDataRegions[sizeIdx];
      case DmtxSymAttribInterleavedBlocks:
         return interleavedBlocks[sizeIdx];
      case DmtxSymAttribBlockErrorWords:
         return blockErrorWords[sizeIdx];
      case DmtxSymAttribBlockMaxCorrectable:
         return blockMaxCorrectable[sizeIdx];
      case DmtxSymAttribSymbolDataWords:
         return symbolDataWords[sizeIdx];
      case DmtxSymAttribSymbolErrorWords:
         return blockErrorWords[sizeIdx] * interleavedBlocks[sizeIdx];
      case DmtxSymAttribSymbolMaxCorrectable:
         return blockMaxCorrectable[sizeIdx] * interleavedBlocks[sizeIdx];
   }

   return DmtxUndefined;
}

/**
 * \brief  Retrieve data size for a specific symbol size and block number
 * \param  sizeIdx
 * \param  blockIdx
 * \return Attribute value
 */
extern int
dmtxGetBlockDataSize(int sizeIdx, int blockIdx)
{
   int symbolDataWords;
   int interleavedBlocks;
   int count;

   symbolDataWords = dmtxGetSymbolAttribute(DmtxSymAttribSymbolDataWords, sizeIdx);
   interleavedBlocks = dmtxGetSymbolAttribute(DmtxSymAttribInterleavedBlocks, sizeIdx);

   if(symbolDataWords < 1 || interleavedBlocks < 1)
      return DmtxUndefined;

   count = (int)(symbolDataWords/interleavedBlocks);

   return (sizeIdx == DmtxSymbol144x144 && blockIdx < 8) ? count + 1 : count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxplacemod.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxplacemod.c
 * \brief Data Matrix module placement
 */

/**
 * receives symbol row and col and returns status
 * DmtxModuleOn / !DmtxModuleOn (DmtxModuleOff)
 * DmtxModuleAssigned
 * DmtxModuleVisited
 * DmtxModuleData / !DmtxModuleData (DmtxModuleAlignment)
 * row and col are expressed in symbol coordinates, so (0,0) is the intersection of the "L"
 */
int
dmtxSymbolModuleStatus(DmtxMessage *message, int sizeIdx, int symbolRow, int symbolCol)
{
   int symbolRowReverse;
   int mappingRow, mappingCol;
   int dataRegionRows, dataRegionCols;
   int symbolRows, mappingCols;

   dataRegionRows = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionRows, sizeIdx);
   dataRegionCols = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionCols, sizeIdx);
   symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, sizeIdx);
   mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, sizeIdx);

   symbolRowReverse = symbolRows - symbolRow - 1;
   mappingRow = symbolRowReverse - 1 - 2 * (symbolRowReverse / (dataRegionRows+2));
   mappingCol = symbolCol - 1 - 2 * (symbolCol / (dataRegionCols+2));

   /* Solid portion of alignment patterns */
   if(symbolRow % (dataRegionRows+2) == 0 ||
         symbolCol % (dataRegionCols+2) == 0)
      return (DmtxModuleOnRGB | (!DmtxModuleData));

   /* Horinzontal calibration bars */
   if((symbolRow+1) % (dataRegionRows+2) == 0)
      return (((symbolCol & 0x01) ? 0 : DmtxModuleOnRGB) | (!DmtxModuleData));

   /* Vertical calibration bars */
   if((symbolCol+1) % (dataRegionCols+2) == 0)
      return (((symbolRow & 0x01) ? 0 : DmtxModuleOnRGB) | (!DmtxModuleData));

   /* Data modules */
   return (message->array[mappingRow * mappingCols + mappingCol] | DmtxModuleData);
}

/**
 * \brief  Logical relationship between bit and module locations
 * \param  modules
 * \param  codewords
 * \param  sizeIdx
 * \param  moduleOnColor
 * \return Number of codewords read
 */
static int
ModulePlacementEcc200(unsigned char *modules, unsigned char *codewords, int sizeIdx, int moduleOnColor)
{
   int row, col, chr;
   int mappingRows, mappingCols;

   assert(moduleOnColor & (DmtxModuleOnRed | DmtxModuleOnGreen | DmtxModuleOnBlue));

   mappingRows = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixRows, sizeIdx);
   mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, sizeIdx);

   /* Start in the nominal location for the 8th bit of the first character */
   chr = 0;
   row = 4;
   col = 0;

   do {
      /* Repeatedly first check for one of the special corner cases */
      if((row == mappingRows) && (col == 0))
         PatternShapeSpecial1(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows-2) && (col == 0) && (mappingCols%4 != 0))
         PatternShapeSpecial2(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows-2) && (col == 0) && (mappingCols%8 == 4))
         PatternShapeSpecial3(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows+4) && (col == 2) && (mappingCols%8 == 0))
         PatternShapeSpecial4(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);

      /* Sweep upward diagonally, inserting successive characters */
      do {
         if((row < mappingRows) && (col >= 0) &&
               !(modules[row*mappingCols+col] & DmtxModuleVisited))
            PatternShapeStandard(modules, mappingRows, mappingCols, row, col, &(codewords[chr++]), moduleOnColor);
         row -= 2;
         col += 2;
      } while ((row >= 0) && (col < mappingCols));
      row += 1;
      col += 3;

      /* Sweep downward diagonally, inserting successive characters */
      do {
         if((row >= 0) && (col < mappingCols) &&
               !(modules[row*mappingCols+col] & DmtxModuleVisited))
            PatternShapeStandard(modules, mappingRows, mappingCols, row, col, &(codewords[chr++]), moduleOnColor);
         row += 2;
         col -= 2;
      } while ((row < mappingRows) && (col >= 0));
      row += 3;
      col += 1;
      /* ... until the entire modules array is scanned */
   } while ((row < mappingRows) || (col < mappingCols));

   /* If lower righthand corner is untouched then fill in the fixed pattern */
   if(!(modules[mappingRows * mappingCols - 1] &
         DmtxModuleVisited)) {

      modules[mappingRows * mappingCols - 1] |= moduleOnColor;
      modules[(mappingRows * mappingCols) - mappingCols - 2] |= moduleOnColor;
   } /* XXX should this fixed pattern also be used in reading somehow? */

   /* XXX compare that chr == region->dataSize here */
   return chr; /* XXX number of codewords read off */
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  row
 * \param  col
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeStandard(unsigned char *modules, int mappingRows, int mappingCols, int row, int col, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, row-2, col-2, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-2, col-1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col-2, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col-1, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col,   codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col-2, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col,   codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial1(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 2, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 2, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 3, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial2(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-3, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-2, 0, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-4, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-3, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial3(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-3, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-2, 0, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 2, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 3, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial4(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, mappingCols-1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-3, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-3, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-2, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  row
 * \param  col
 * \param  codeword
 * \param  mask
 * \param  moduleOnColor
 * \return void
 */
static void
PlaceModule(unsigned char *modules, int mappingRows, int mappingCols, int row, int col, unsigned char *codeword, int mask, int moduleOnColor)
{
   if(row < 0) {
      row += mappingRows;
      col += 4 - ((mappingRows+4)%8);
   }
   if(col < 0) {
      col += mappingCols;
      row += 4 - ((mappingCols+4)%8);
   }

   /* If module has already been assigned then we are decoding the pattern into codewords */
   if((modules[row*mappingCols+col] & DmtxModuleAssigned) != 0) {
      if((modules[row*mappingCols+col] & moduleOnColor) != 0)
         *codeword |= mask;
      else
         *codeword &= (0xff ^ mask);
   }
   /* Otherwise we are encoding the codewords into a pattern */
   else {
      if((*codeword & mask) != 0x00)
         modules[row*mappingCols+col] |= moduleOnColor;

      modules[row*mappingCols+col] |= DmtxModuleAssigned;
   }

   modules[row*mappingCols+col] |= DmtxModuleVisited;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxreedsol.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2011 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * ---------------------------------------------------------
 * Portions of this file were derived from the Reed-Solomon
 * encoder/decoder released by Simon Rockliff in June 1991.
 * ---------------------------------------------------------
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxreedsol.c
 */

/**
 * TODO:
 *   o try doxygen using using the JavaDoc style and JAVADOC_AUTOBRIEF = YES
 *   o switch doxygen to simplified syntax, and using "\file" instead of "@file"
 */

#define NN                      255
#define MAX_ERROR_WORD_COUNT     68

/* GF add (a + b) */
#define GfAdd(a,b) \
   ((a) ^ (b))

/* GF multiply (a * b) */
#define GfMult(a,b) \
   (((a) == 0 || (b) == 0) ? 0 : antilog301[(log301[(a)] + log301[(b)]) % NN])

/* GF multiply by antilog (a * alpha**b) */
#define GfMultAntilog(a,b) \
   (((a) == 0) ? 0 : antilog301[(log301[(a)] + (b)) % NN])

/* GF(256) log values using primitive polynomial 301 */
static DmtxByte log301[] =
   { 255,   0,   1, 240,   2, 225, 241,  53,   3,  38, 226, 133, 242,  43,  54, 210,
       4, 195,  39, 114, 227, 106, 134,  28, 243, 140,  44,  23,  55, 118, 211, 234,
       5, 219, 196,  96,  40, 222, 115, 103, 228,  78, 107, 125, 135,   8,  29, 162,
     244, 186, 141, 180,  45,  99,  24,  49,  56,  13, 119, 153, 212, 199, 235,  91,
       6,  76, 220, 217, 197,  11,  97, 184,  41,  36, 223, 253, 116, 138, 104, 193,
     229,  86,  79, 171, 108, 165, 126, 145, 136,  34,   9,  74,  30,  32, 163,  84,
     245, 173, 187, 204, 142,  81, 181, 190,  46,  88, 100, 159,  25, 231,  50, 207,
      57, 147,  14,  67, 120, 128, 154, 248, 213, 167, 200,  63, 236, 110,  92, 176,
       7, 161,  77, 124, 221, 102, 218,  95, 198,  90,  12, 152,  98,  48, 185, 179,
      42, 209,  37, 132, 224,  52, 254, 239, 117, 233, 139,  22, 105,  27, 194, 113,
     230, 206,  87, 158,  80, 189, 172, 203, 109, 175, 166,  62, 127, 247, 146,  66,
     137, 192,  35, 252,  10, 183,  75, 216,  31,  83,  33,  73, 164, 144,  85, 170,
     246,  65, 174,  61, 188, 202, 205, 157, 143, 169,  82,  72, 182, 215, 191, 251,
      47, 178,  89, 151, 101,  94, 160, 123,  26, 112, 232,  21,  51, 238, 208, 131,
      58,  69, 148,  18,  15,  16,  68,  17, 121, 149, 129,  19, 155,  59, 249,  70,
     214, 250, 168,  71, 201, 156,  64,  60, 237, 130, 111,  20,  93, 122, 177, 150 };

/* GF(256) antilog values using primitive polynomial 301 */
static DmtxByte antilog301[] =
   {   1,   2,   4,   8,  16,  32,  64, 128,  45,  90, 180,  69, 138,  57, 114, 228,
     229, 231, 227, 235, 251, 219, 155,  27,  54, 108, 216, 157,  23,  46,  92, 184,
      93, 186,  89, 178,  73, 146,   9,  18,  36,  72, 144,  13,  26,  52, 104, 208,
     141,  55, 110, 220, 149,   7,  14,  28,  56, 112, 224, 237, 247, 195, 171, 123,
     246, 193, 175, 115, 230, 225, 239, 243, 203, 187,  91, 182,  65, 130,  41,  82,
     164, 101, 202, 185,  95, 190,  81, 162, 105, 210, 137,  63, 126, 252, 213, 135,
      35,  70, 140,  53, 106, 212, 133,  39,  78, 156,  21,  42,  84, 168, 125, 250,
     217, 159,  19,  38,  76, 152,  29,  58, 116, 232, 253, 215, 131,  43,  86, 172,
     117, 234, 249, 223, 147,  11,  22,  44,  88, 176,  77, 154,  25,  50, 100, 200,
     189,  87, 174, 113, 226, 233, 255, 211, 139,  59, 118, 236, 245, 199, 163, 107,
     214, 129,  47,  94, 188,  85, 170, 121, 242, 201, 191,  83, 166,  97, 194, 169,
     127, 254, 209, 143,  51, 102, 204, 181,  71, 142,  49,  98, 196, 165, 103, 206,
     177,  79, 158,  17,  34,  68, 136,  61, 122, 244, 197, 167,  99, 198, 161, 111,
     222, 145,  15,  30,  60, 120, 240, 205, 183,  67, 134,  33,  66, 132,  37,  74,
     148,   5,  10,  20,  40,  80, 160, 109, 218, 153,  31,  62, 124, 248, 221, 151,
       3,   6,  12,  24,  48,  96, 192, 173, 119, 238, 241, 207, 179,  75, 150,   0 };

/**
 * Decode xyz.
 * More detailed description.
 * \param code
 * \param sizeIdx
 * \param fix
 * \return Function success (DmtxPass|DmtxFail)
 */
#undef CHKPASS
#define CHKPASS { if(passFail == DmtxFail) return DmtxFail; }
static DmtxPassFail
RsDecode(unsigned char *code, int sizeIdx, int fix)
{
   int i;
   int blockStride, blockIdx;
   int blockDataWords, blockErrorWords, blockTotalWords, blockMaxCorrectable;
   int symbolDataWords, symbolErrorWords, symbolTotalWords;
   DmtxBoolean error, repairable;
   DmtxPassFail passFail;
   unsigned char *word;
   DmtxByte elpStorage[MAX_ERROR_WORD_COUNT];
   DmtxByte synStorage[MAX_ERROR_WORD_COUNT+1];
   DmtxByte recStorage[NN];
   DmtxByte locStorage[NN];
   DmtxByteList elp = dmtxByteListBuild(elpStorage, sizeof(elpStorage));
   DmtxByteList syn = dmtxByteListBuild(synStorage, sizeof(synStorage));
   DmtxByteList rec = dmtxByteListBuild(recStorage, sizeof(recStorage));
   DmtxByteList loc = dmtxByteListBuild(locStorage, sizeof(locStorage));

   blockStride = dmtxGetSymbolAttribute(DmtxSymAttribInterleavedBlocks, sizeIdx);
   blockErrorWords = dmtxGetSymbolAttribute(DmtxSymAttribBlockErrorWords, sizeIdx);
   blockMaxCorrectable = dmtxGetSymbolAttribute(DmtxSymAttribBlockMaxCorrectable, sizeIdx);
   symbolDataWords = dmtxGetSymbolAttribute(DmtxSymAttribSymbolDataWords, sizeIdx);
   symbolErrorWords = dmtxGetSymbolAttribute(DmtxSymAttribSymbolErrorWords, sizeIdx);
   symbolTotalWords = symbolDataWords + symbolErrorWords;

   /* For each interleaved block */
   for(blockIdx = 0; blockIdx < blockStride; blockIdx++)
   {
      /* Data word count depends on blockIdx due to special case at 144x144 */
      blockDataWords = dmtxGetBlockDataSize(sizeIdx, blockIdx);
      blockTotalWords = blockErrorWords + blockDataWords;

      /* Populate received list (rec) with data and error codewords */
      dmtxByteListInit(&rec, 0, 0, &passFail); CHKPASS;

      /* Start with final error word and work backward */
      word = code + symbolTotalWords + blockIdx - blockStride;
      for(i = 0; i < blockErrorWords; i++)
      {
         dmtxByteListPush(&rec, *word, &passFail); CHKPASS;
         word -= blockStride;
      }

      /* Start with final data word and work backward */
      word = code + blockIdx + (blockStride * (blockDataWords - 1));
      for(i = 0; i < blockDataWords; i++)
      {
         dmtxByteListPush(&rec, *word, &passFail); CHKPASS;
         word -= blockStride;
      }

      /* Compute syndromes (syn) */
      error = RsComputeSyndromes(&syn, &rec, blockErrorWords);

      /* Error(s) detected: Attempt repair */
      if(error)
      {
         /* Find error locator polynomial (elp) */
         repairable = RsFindErrorLocatorPoly(&elp, &syn, blockErrorWords, blockMaxCorrectable);
         if(!repairable)
            return DmtxFail;

         /* Find error positions (loc) */
         repairable = RsFindErrorLocations(&loc, &elp);
         if(!repairable)
            return DmtxFail;

         /* Find error values and repair */
         RsRepairErrors(&rec, &loc, &elp, &syn);
      }

      /*
       * Overwrite output with correct/corrected values
       */

      /* Start with first data word and work forward */
      word = code + blockIdx;
      for(i = 0; i < blockDataWords; i++)
      {
         *word = dmtxByteListPop(&rec, &passFail); CHKPASS;
         word += blockStride;
      }

      /* Start with first error word and work forward */
      word = code + symbolDataWords + blockIdx;
      for(i = 0; i < blockErrorWords; i++)
      {
         *word = dmtxByteListPop(&rec, &passFail); CHKPASS;
         word += blockStride;
      }
   }

   return DmtxPass;
}

/**
 * Populate generator polynomial.
 * Assume we have received bits grouped into mm-bit symbols in rec[i],
 * i=0..(nn-1),  and rec[i] is index form (ie as powers of alpha). We first
 * compute the 2*tt syndromes by substituting alpha**i into rec(X) and
 * evaluating, storing the syndromes in syn[i], i=1..2tt (leave syn[0] zero).
 * \param syn
 * \param rec
 * \param blockErrorWords
 * \return Are error(s) present? (DmtxPass|DmtxFail)
 */
/* XXX this CHKPASS isn't doing what we want ... really need a error reporting strategy */
#undef CHKPASS
#define CHKPASS { if(passFail == DmtxFail) return DmtxTrue; }
static DmtxBoolean
RsComputeSyndromes(DmtxByteList *syn, const DmtxByteList *rec, int blockErrorWords)
{
   int i, j;
   DmtxPassFail passFail;
   DmtxBoolean error = DmtxFalse;

   /* Initialize all coefficients to 0 */
   dmtxByteListInit(syn, blockErrorWords + 1, 0, &passFail); CHKPASS;

   for(i = 1; i < syn->length; i++)
   {
      /* Calculate syndrome at i */
      for(j = 0; j < rec->length; j++) /* alternatively: j < blockTotalWords */
         syn->b[i] = GfAdd(syn->b[i], GfMultAntilog(rec->b[j], i*j));

      /* Non-zero syndrome indicates presence of error(s) */
      if(syn->b[i] != 0)
         error = DmtxTrue;
   }

   return error;
}

/**
 * Find the error location polynomial using Berlekamp-Massey.
 * More detailed description.
 * \param elpOut
 * \param syn
 * \param errorWordCount
 * \param maxCorrectable
 * \return Is block repairable? (DmtxTrue|DmtxFalse)
 */
/* XXX this CHKPASS isn't doing what we want ... really need a error reporting strategy */
#undef CHKPASS
#define CHKPASS { if(passFail == DmtxFail) { free(elpStorage_temp); return DmtxFalse; } }
static DmtxBoolean
RsFindErrorLocatorPoly(DmtxByteList *elpOut, const DmtxByteList *syn, int errorWordCount, int maxCorrectable)
{
   int i, iNext, j;
   int m, mCmp, lambda;
   DmtxByte disTmp, disStorage[MAX_ERROR_WORD_COUNT+1];
   DmtxByte *elpStorage_temp = malloc(sizeof(DmtxByte) * (MAX_ERROR_WORD_COUNT+2) * MAX_ERROR_WORD_COUNT); DmtxByte (*elpStorage)[MAX_ERROR_WORD_COUNT] = (DmtxByte (*)[MAX_ERROR_WORD_COUNT]) elpStorage_temp; // [MAX_ERROR_WORD_COUNT+2][MAX_ERROR_WORD_COUNT];
   DmtxByteList dis, elp[MAX_ERROR_WORD_COUNT+2];
   DmtxPassFail passFail;

   dis = dmtxByteListBuild(disStorage, sizeof(disStorage));
   dmtxByteListInit(&dis, 0, 0, &passFail); CHKPASS;

   for(i = 0; i < MAX_ERROR_WORD_COUNT + 2; i++)
   {
      elp[i] = dmtxByteListBuild(elpStorage[i], sizeof(elpStorage[i]));
      dmtxByteListInit(&elp[i], 0, 0, &passFail); CHKPASS;
   }

   /* iNext = 0 */
   dmtxByteListPush(&elp[0], 1, &passFail); CHKPASS;
   dmtxByteListPush(&dis, 1, &passFail); CHKPASS;

   /* iNext = 1 */
   dmtxByteListPush(&elp[1], 1, &passFail); CHKPASS;
   dmtxByteListPush(&dis, syn->b[1], &passFail); CHKPASS;

   for(iNext = 2, i = 1; /* explicit break */; i = iNext++)
   {
      if(dis.b[i] == 0)
      {
         /* Simple case: Copy directly from previous iteration */
         dmtxByteListCopy(&elp[iNext], &elp[i], &passFail); CHKPASS;
      }
      else
      {
         /* Find earlier iteration (m) that provides maximal (m - lambda) */
         for(m = 0, mCmp = 1; mCmp < i; mCmp++)
            if(dis.b[mCmp] != 0 && (mCmp - elp[mCmp].length) >= (m - elp[m].length))
               m = mCmp;

         /* Calculate error location polynomial elp[i] (set 1st term) */
         for(lambda = elp[m].length - 1, j = 0; j <= lambda; j++)
            elp[iNext].b[j+i-m] = antilog301[(NN - log301[dis.b[m]] +
                  log301[dis.b[i]] + log301[elp[m].b[j]]) % NN];

         /* Calculate error location polynomial elp[i] (add 2nd term) */
         for(lambda = elp[i].length - 1, j = 0; j <= lambda; j++)
            elp[iNext].b[j] = GfAdd(elp[iNext].b[j], elp[i].b[j]);

         elp[iNext].length = max(elp[i].length, elp[m].length + i - m);
      }

      lambda = elp[iNext].length - 1;
      if(i == errorWordCount || i >= lambda + maxCorrectable)
         break;

      /* Calculate discrepancy dis.b[i] */
      for(disTmp = syn->b[iNext], j = 1; j <= lambda; j++)
         disTmp = GfAdd(disTmp, GfMult(syn->b[iNext-j], elp[iNext].b[j]));

      assert(dis.length == iNext);
      dmtxByteListPush(&dis, disTmp, &passFail); CHKPASS;
   }

   dmtxByteListCopy(elpOut, &elp[iNext], &passFail); CHKPASS;

   free(elpStorage_temp);

   return (lambda <= maxCorrectable) ? DmtxTrue : DmtxFalse;
}

/**
 * Find roots of the error locator polynomial (Chien Search).
 * If the degree of elp is <= tt, we substitute alpha**i, i=1..n into the elp
 * to get the roots, hence the inverse roots, the error location numbers.
 * If the number of errors located does not equal the degree of the elp, we
 * have more than tt errors and cannot correct them.
 * \param loc
 * \param elp
 * \return Is block repairable? (DmtxTrue|DmtxFalse)
 */
#undef CHKPASS
#define CHKPASS { if(passFail == DmtxFail) return DmtxFalse; }
static DmtxBoolean
RsFindErrorLocations(DmtxByteList *loc, const DmtxByteList *elp)
{
   int i, j;
   int lambda = elp->length - 1;
   DmtxPassFail passFail;
   DmtxByte q, regStorage[MAX_ERROR_WORD_COUNT];
   DmtxByteList reg = dmtxByteListBuild(regStorage, sizeof(regStorage));

   dmtxByteListCopy(&reg, elp, &passFail); CHKPASS;
   dmtxByteListInit(loc, 0, 0, &passFail); CHKPASS;

   for(i = 1; i <= NN; i++)
   {
      for(q = 1, j = 1; j <= lambda; j++)
      {
         reg.b[j] = GfMultAntilog(reg.b[j], j);
         q = GfAdd(q, reg.b[j]);
      }

      if(q == 0)
      {
         dmtxByteListPush(loc, NN - i, &passFail); CHKPASS;
      }
   }

   return (loc->length == lambda) ? DmtxTrue : DmtxFalse;
}

/**
 * Find the error values and repair.
 * Solve for the error value at the error location and correct the error. The
 * procedure is that found in Lin and Costello.
 * For the cases where the number of errors is known to be too large to
 * correct, the information symbols as received are output (the advantage of
 * systematic encoding is that hopefully some of the information symbols will
 * be okay and that if we are in luck, the errors are in the parity part of
 * the transmitted codeword).
 * \param rec
 * \param loc
 * \param elp
 * \param syn
 */
#undef CHKPASS
#define CHKPASS { if(passFail == DmtxFail) return DmtxFail; }
static DmtxPassFail
RsRepairErrors(DmtxByteList *rec, const DmtxByteList *loc, const DmtxByteList *elp, const DmtxByteList *syn)
{
   int i, j, q;
   int lambda = elp->length - 1;
   DmtxPassFail passFail;
   DmtxByte zVal, root, err;
   DmtxByte zStorage[MAX_ERROR_WORD_COUNT+1];
   DmtxByteList z = dmtxByteListBuild(zStorage, sizeof(zStorage));

   /* Form polynomial z(x) */
   dmtxByteListPush(&z, 1, &passFail); CHKPASS;
   for(i = 1; i <= lambda; i++)
   {
      for(zVal = GfAdd(syn->b[i], elp->b[i]), j = 1; j < i; j++)
         zVal= GfAdd(zVal, GfMult(elp->b[i-j], syn->b[j]));
      dmtxByteListPush(&z, zVal, &passFail); CHKPASS;
   }

   for(i = 0; i < lambda; i++)
   {
      /* Calculate numerator of error term */
      root = NN - loc->b[i];

      for(err = 1, j = 1; j <= lambda; j++)
         err = GfAdd(err, GfMultAntilog(z.b[j], j * root));

      if(err == 0)
         continue;

      /* Calculate denominator of error term */
      for(q = 0, j = 0; j < lambda; j++)
      {
         if(j != i)
            q += log301[1 ^ antilog301[(loc->b[j] + root) % NN]];
      }
      q %= NN;

      err = GfMultAntilog(err, NN - q);
      rec->b[loc->b[i]] = GfAdd(rec->b[loc->b[i]], err);
   }

   return DmtxPass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxscangrid.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxscangrid.c
 * \brief Scan grid tracking
 */

/**
 * \brief  Initialize scan grid pattern
 * \param  dec
 * \return Initialized grid
 */
static DmtxScanGrid
InitScanGrid(DmtxDecode *dec)
{
   int scale, smallestFeature;
   int xExtent, yExtent, maxExtent;
   int extent;
   DmtxScanGrid grid;

   memset(&grid, 0x00, sizeof(DmtxScanGrid));

   scale = dmtxDecodeGetProp(dec, DmtxPropScale);
   smallestFeature = dmtxDecodeGetProp(dec, DmtxPropScanGap) / scale;

   grid.xMin = dmtxDecodeGetProp(dec, DmtxPropXmin);
   grid.xMax = dmtxDecodeGetProp(dec, DmtxPropXmax);
   grid.yMin = dmtxDecodeGetProp(dec, DmtxPropYmin);
   grid.yMax = dmtxDecodeGetProp(dec, DmtxPropYmax);

   /* Values that get set once */
   xExtent = grid.xMax - grid.xMin;
   yExtent = grid.yMax - grid.yMin;
   maxExtent = (xExtent > yExtent) ? xExtent : yExtent;

   assert(maxExtent > 1);

   for(extent = 1; extent < maxExtent; extent = ((extent + 1) * 2) - 1)
      if(extent <= smallestFeature)
         grid.minExtent = extent;

   grid.maxExtent = extent;

   grid.xOffset = (grid.xMin + grid.xMax - grid.maxExtent) / 2;
   grid.yOffset = (grid.yMin + grid.yMax - grid.maxExtent) / 2;

   /* Values that get reset for every level */
   grid.total = 1;
   grid.extent = grid.maxExtent;

   SetDerivedFields(&grid);

   return grid;
}

/**
 * \brief  Return the next good location (which may be the current location),
 *         and advance grid progress one position beyond that. If no good
 *         locations remain then return DmtxRangeEnd.
 * \param  grid
 * \return void
 */
static int
PopGridLocation(DmtxScanGrid *grid, DmtxPixelLoc *locPtr)
{
   int locStatus;

   do {
      locStatus = GetGridCoordinates(grid, locPtr);

      /* Always leave grid pointing at next available location */
      grid->pixelCount++;

   } while(locStatus == DmtxRangeBad);

   return locStatus;
}

/**
 * \brief  Extract current grid position in pixel coordinates and return
 *         whether location is good, bad, or end
 * \param  grid
 * \return Pixel location
 */
static int
GetGridCoordinates(DmtxScanGrid *grid, DmtxPixelLoc *locPtr)
{
   int count, half, quarter;
   DmtxPixelLoc loc;

   /* Initially pixelCount may fall beyond acceptable limits. Update grid
    * state before testing coordinates */

   /* Jump to next cross pattern horizontally if current column is done */
   if(grid->pixelCount >= grid->pixelTotal) {
      grid->pixelCount = 0;
      grid->xCenter += grid->jumpSize;
   }

   /* Jump to next cross pattern vertically if current row is done */
   if(grid->xCenter > grid->maxExtent) {
      grid->xCenter = grid->startPos;
      grid->yCenter += grid->jumpSize;
   }

   /* Increment level when vertical step goes too far */
   if(grid->yCenter > grid->maxExtent) {
      grid->total *= 4;
      grid->extent /= 2;
      SetDerivedFields(grid);
   }

   if(grid->extent == 0 || grid->extent < grid->minExtent) {
      locPtr->X = locPtr->Y = -1;
      return DmtxRangeEnd;
   }

   count = grid->pixelCount;

   assert(count < grid->pixelTotal);

   if(count == grid->pixelTotal - 1) {
      /* center pixel */
      loc.X = grid->xCenter;
      loc.Y = grid->yCenter;
   }
   else {
      half = grid->pixelTotal / 2;
      quarter = half / 2;

      /* horizontal portion */
      if(count < half) {
         loc.X = grid->xCenter + ((count < quarter) ? (count - quarter) : (half - count));
         loc.Y = grid->yCenter;
      }
      /* vertical portion */
      else {
         count -= half;
         loc.X = grid->xCenter;
         loc.Y = grid->yCenter + ((count < quarter) ? (count - quarter) : (half - count));
      }
   }

   loc.X += grid->xOffset;
   loc.Y += grid->yOffset;

   *locPtr = loc;

   if(loc.X < grid->xMin || loc.X > grid->xMax ||
         loc.Y < grid->yMin || loc.Y > grid->yMax)
      return DmtxRangeBad;

   return DmtxRangeGood;
}

/**
 * \brief  Update derived fields based on current state
 * \param  grid
 * \return void
 */
static void
SetDerivedFields(DmtxScanGrid *grid)
{
   grid->jumpSize = grid->extent + 1;
   grid->pixelTotal = 2 * grid->extent - 1;
   grid->startPos = grid->extent / 2;
   grid->pixelCount = 0;
   grid->xCenter = grid->yCenter = grid->startPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtximage.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtximage.c
 * \brief Image handling
 */

/**
 * libdmtx stores image data as a large one-dimensional array of packed pixels,
 * reading from the array when scanning barcodes and writing to it when creating
 * a barcode. Beyond this interaction the calling program is responsible for
 * populating and dispatching pixels between the image array and the outside
 * world, whether that means loading an image from a file, acquiring camera
 * input, displaying output to a screen, saving to disk, etc...
 *
 * By default, libdmtx treats the first pixel of an image array as the top-left
 * corner of the physical image, with the final pixel landing at the bottom-
 * right. However, if mapping a pixel buffer this way produces an inverted
 * image the calling program can specify DmtxFlipY at image creation time to
 * remove the inversion. This has a negligible effect on performance since it
 * only modifies the pixel mapping math, and does not alter any pixel data.
 *
 * Regardless of how an image is stored internally, all libdmtx functions
 * consider coordinate (0,0) to mathematically represent the bottom-left pixel
 * location of an image using a right-handed coordinate system.
 *
 *                (0,HEIGHT-1)        (WIDTH-1,HEIGHT-1)
 *
 *          array pos = 0,1,2,3,...-----------+
 *                      |                     |
 *                      |                     |
 *                      |       libdmtx       |
 *                      |        image        |
 *                      |     coordinates     |
 *                      |                     |
 *                      |                     |
 *                      +---------...,N-2,N-1,N = array pos
 *
 *                    (0,0)              (WIDTH-1,0)
 *
 * Notes:
 *   - OpenGL pixel arrays obtained with glReadPixels() are stored
 *     bottom-to-top; use DmtxFlipY
 *   - Many popular image formats (e.g., PNG, GIF) store rows
 *     top-to-bottom; use DmtxFlipNone
 */

/**
 * \brief  XXX
 * \param  XXX
 * \return XXX
 */
extern DmtxImage *
dmtxImageCreate(unsigned char *pxl, int width, int height, int pack)
{
   DmtxPassFail err;
   DmtxImage *img;

   if(pxl == NULL || width < 1 || height < 1)
      return NULL;

   img = (DmtxImage *)calloc(1, sizeof(DmtxImage));
   if(img == NULL)
      return NULL;

   img->pxl = pxl;
   img->width = width;
   img->height = height;
   img->pixelPacking = pack;
   img->bitsPerPixel = GetBitsPerPixel(pack);
   img->bytesPerPixel = img->bitsPerPixel/8;
   img->rowPadBytes = 0;
   img->rowSizeBytes = img->width * img->bytesPerPixel + img->rowPadBytes;
   img->imageFlip = DmtxFlipNone;

   /* Leave channelStart[] and bitsPerChannel[] with zeros from calloc */
   img->channelCount = 0;

   switch(pack) {
      case DmtxPackCustom:
         break;
      case DmtxPack1bppK:
         err = dmtxImageSetChannel(img, 0, 1);
         return NULL; /* unsupported packing order */
/*       break; */
      case DmtxPack8bppK:
         err = dmtxImageSetChannel(img, 0, 8);
         break;
      case DmtxPack16bppRGB:
      case DmtxPack16bppBGR:
      case DmtxPack16bppYCbCr:
         err = dmtxImageSetChannel(img,  0, 5);
         err = dmtxImageSetChannel(img,  5, 5);
         err = dmtxImageSetChannel(img, 10, 5);
         break;
      case DmtxPack24bppRGB:
      case DmtxPack24bppBGR:
      case DmtxPack24bppYCbCr:
      case DmtxPack32bppRGBX:
      case DmtxPack32bppBGRX:
         err = dmtxImageSetChannel(img,  0, 8);
         err = dmtxImageSetChannel(img,  8, 8);
         err = dmtxImageSetChannel(img, 16, 8);
         break;
      case DmtxPack16bppRGBX:
      case DmtxPack16bppBGRX:
         err = dmtxImageSetChannel(img,  0, 5);
         err = dmtxImageSetChannel(img,  5, 5);
         err = dmtxImageSetChannel(img, 10, 5);
         break;
      case DmtxPack16bppXRGB:
      case DmtxPack16bppXBGR:
         err = dmtxImageSetChannel(img,  1, 5);
         err = dmtxImageSetChannel(img,  6, 5);
         err = dmtxImageSetChannel(img, 11, 5);
         break;
      case DmtxPack32bppXRGB:
      case DmtxPack32bppXBGR:
         err = dmtxImageSetChannel(img,  8, 8);
         err = dmtxImageSetChannel(img, 16, 8);
         err = dmtxImageSetChannel(img, 24, 8);
         break;
      case DmtxPack32bppCMYK:
         err = dmtxImageSetChannel(img,  0, 8);
         err = dmtxImageSetChannel(img,  8, 8);
         err = dmtxImageSetChannel(img, 16, 8);
         err = dmtxImageSetChannel(img, 24, 8);
         break;
      default:
         return NULL;
   }

   return img;
}

/**
 * \brief  Free libdmtx image memory
 * \param  img pointer to img location
 * \return DmtxFail | DmtxPass
 */
extern DmtxPassFail
dmtxImageDestroy(DmtxImage **img)
{
   if(img == NULL || *img == NULL)
      return DmtxFail;

   free(*img);

   *img = NULL;

   return DmtxPass;
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxImageSetChannel(DmtxImage *img, int channelStart, int bitsPerChannel)
{
   if(img->channelCount >= 4) /* IMAGE_MAX_CHANNEL */
      return DmtxFail;

   /* New channel extends beyond pixel data */
/* if(channelStart + bitsPerChannel > img->bitsPerPixel)
      return DmtxFail; */

   img->bitsPerChannel[img->channelCount] = bitsPerChannel;
   img->channelStart[img->channelCount] = channelStart;
   (img->channelCount)++;

   return DmtxPass;
}

/**
 * \brief  Set image property
 * \param  img pointer to image
 * \return image width
 */
extern DmtxPassFail
dmtxImageSetProp(DmtxImage *img, int prop, int value)
{
   if(img == NULL)
      return DmtxFail;

   switch(prop) {
      case DmtxPropRowPadBytes:
         img->rowPadBytes = value;
         img->rowSizeBytes = img->width * (img->bitsPerPixel/8) + img->rowPadBytes;
         break;
      case DmtxPropImageFlip:
         img->imageFlip = value;
         break;
      default:
         break;
   }

   return DmtxPass;
}

/**
 * \brief  Get image width
 * \param  img pointer to image
 * \return image width
 */
extern int
dmtxImageGetProp(DmtxImage *img, int prop)
{
   if(img == NULL)
      return DmtxUndefined;

   switch(prop) {
      case DmtxPropWidth:
         return img->width;
      case DmtxPropHeight:
         return img->height;
      case DmtxPropPixelPacking:
         return img->pixelPacking;
      case DmtxPropBitsPerPixel:
         return img->bitsPerPixel;
      case DmtxPropBytesPerPixel:
         return img->bytesPerPixel;
      case DmtxPropRowPadBytes:
         return img->rowPadBytes;
      case DmtxPropRowSizeBytes:
         return img->rowSizeBytes;
      case DmtxPropImageFlip:
         return img->imageFlip;
      case DmtxPropChannelCount:
         return img->channelCount;
      default:
         break;
   }

   return DmtxUndefined;
}

/**
 * \brief  Returns pixel offset for image
 * \param  img
 * \param  x coordinate
 * \param  y coordinate
 * \return pixel byte offset
 */
extern int
dmtxImageGetByteOffset(DmtxImage *img, int x, int y)
{
   assert(img != NULL);
   assert(!(img->imageFlip & DmtxFlipX)); /* DmtxFlipX is not an option */

   if(dmtxImageContainsInt(img, 0, x, y) == DmtxFalse)
      return DmtxUndefined;

   if(img->imageFlip & DmtxFlipY)
      return (y * img->rowSizeBytes + x * img->bytesPerPixel);

   return ((img->height - y - 1) * img->rowSizeBytes + x * img->bytesPerPixel);
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxImageGetPixelValue(DmtxImage *img, int x, int y, int channel, int *value)
{
   int offset;
/* unsigned char *pixelPtr;
   int pixelValue;
   int mask;
   int bitShift; */

   assert(img != NULL);
   assert(channel < img->channelCount);

   offset = dmtxImageGetByteOffset(img, x, y);
   if(offset == DmtxUndefined)
      return DmtxFail;

   switch(img->bitsPerChannel[channel]) {
      case 1:
/*       assert(img->bitsPerPixel == 1);
         mask = 0x01 << (7 - offset%8);
         *value = (img->pxl[offset/8] & mask) ? 255 : 0; */
         break;
      case 5:
         /* XXX might be expensive if we want to scale perfect 0-255 range */
/*       assert(img->bitsPerPixel == 16);
         pixelPtr = img->pxl + (offset * (img->bitsPerPixel/8));
         pixelValue = (*pixelPtr << 8) | (*(pixelPtr+1));
         bitShift = img->bitsPerPixel - 5 - img->channelStart[channel];
         mask = 0x1f << bitShift;
         *value = (((pixelValue & mask) >> bitShift) << 3); */
         break;
      case 8:
         assert(img->channelStart[channel] % 8 == 0);
         assert(img->bitsPerPixel % 8 == 0);
         *value = img->pxl[offset + channel];
         break;
   }

   return DmtxPass;
}

/**
 *
 *
 */
extern DmtxPassFail
dmtxImageSetPixelValue(DmtxImage *img, int x, int y, int channel, int value)
{
   int offset;
/* unsigned char *pixelPtr; */
/* int pixelValue; */
/* int mask; */
/* int bitShift; */

   assert(img != NULL);
   assert(channel < img->channelCount);

   offset = dmtxImageGetByteOffset(img, x, y);
   if(offset == DmtxUndefined)
      return DmtxFail;

   switch(img->bitsPerChannel[channel]) {
      case 1:
/*       assert(img->bitsPerPixel == 1);
         mask = 0x01 << (7 - offset%8);
         *value = (img->pxl[offset/8] & mask) ? 255 : 0; */
         break;
      case 5:
         /* XXX might be expensive if we want to scale perfect 0-255 range */
/*       assert(img->bitsPerPixel == 16);
         pixelPtr = img->pxl + (offset * (img->bitsPerPixel/8));
         pixelValue = (*pixelPtr << 8) | (*(pixelPtr+1));
         bitShift = img->bitsPerPixel - 5 - img->channelStart[channel];
         mask = 0x1f << bitShift;
         *value = (((pixelValue & mask) >> bitShift) << 3); */
         break;
      case 8:
         assert(img->channelStart[channel] % 8 == 0);
         assert(img->bitsPerPixel % 8 == 0);
         img->pxl[offset + channel] = value;
         break;
   }

   return DmtxPass;
}

/**
 * \brief  Test whether image contains a coordinate expressed in integers
 * \param  img
 * \param  margin width
 * \param  x coordinate
 * \param  y coordinate
 * \return DmtxTrue | DmtxFalse
 */
extern DmtxBoolean
dmtxImageContainsInt(DmtxImage *img, int margin, int x, int y)
{
   assert(img != NULL);

   if(x - margin >= 0 && x + margin < img->width &&
         y - margin >= 0 && y + margin < img->height)
      return DmtxTrue;

   return DmtxFalse;
}

/**
 * \brief  Test whether image contains a coordinate expressed in floating points
 * \param  img
 * \param  x coordinate
 * \param  y coordinate
 * \return DmtxTrue | DmtxFalse
 */
extern DmtxBoolean
dmtxImageContainsFloat(DmtxImage *img, float x, float y)
{
   assert(img != NULL);

   if(x >= 0.0 && x < (float)img->width && y >= 0.0 && y < (float)img->height)
      return DmtxTrue;

   return DmtxFalse;
}

/**
 *
 *
 */
static int
GetBitsPerPixel(int pack)
{
   switch(pack) {
      case DmtxPack1bppK:
         return 1;
      case DmtxPack8bppK:
         return 8;
      case DmtxPack16bppRGB:
      case DmtxPack16bppRGBX:
      case DmtxPack16bppXRGB:
      case DmtxPack16bppBGR:
      case DmtxPack16bppBGRX:
      case DmtxPack16bppXBGR:
      case DmtxPack16bppYCbCr:
         return 16;
      case DmtxPack24bppRGB:
      case DmtxPack24bppBGR:
      case DmtxPack24bppYCbCr:
         return  24;
      case DmtxPack32bppRGBX:
      case DmtxPack32bppXRGB:
      case DmtxPack32bppBGRX:
      case DmtxPack32bppXBGR:
      case DmtxPack32bppCMYK:
         return  32;
      default:
         break;
   }

   return DmtxUndefined;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxbytelist.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2010 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file file.c
 */

/**
 *
 *
 */
extern DmtxByteList
dmtxByteListBuild(DmtxByte *storage, int capacity)
{
   DmtxByteList list;

   list.b = storage;
   list.capacity = capacity;
   list.length = 0;

   return list;
}

/**
 *
 *
 */
extern void
dmtxByteListInit(DmtxByteList *list, int length, DmtxByte value, DmtxPassFail *passFail)
{
   if(length > list->capacity)
   {
      *passFail = DmtxFail;
   }
   else
   {
      list->length = length;
      memset(list->b, value, sizeof(DmtxByte) * list->capacity);
      *passFail = DmtxPass;
   }
}

/**
 *
 *
 */
extern void
dmtxByteListClear(DmtxByteList *list)
{
   memset(list->b, 0x00, sizeof(DmtxByte) * list->capacity);
   list->length = 0;
}

/**
 *
 *
 */
extern DmtxBoolean
dmtxByteListHasCapacity(DmtxByteList *list)
{
   return (list->length < list->capacity) ? DmtxTrue : DmtxFalse;
}

/**
 *
 *
 */
extern void
dmtxByteListCopy(DmtxByteList *dst, const DmtxByteList *src, DmtxPassFail *passFail)
{
   int length;

   if(dst->capacity < src->length)
   {
      *passFail = DmtxFail; /* dst must be large enough to hold src data */
   }
   else
   {
      /* Copy as many bytes as dst can hold or src can provide (smaller of two) */
      length = (dst->capacity < src->capacity) ? dst->capacity : src->capacity;

      dst->length = src->length;
      memcpy(dst->b, src->b, sizeof(unsigned char) * length);
      *passFail = DmtxPass;
   }
}

/**
 *
 *
 */
extern void
dmtxByteListPush(DmtxByteList *list, DmtxByte value, DmtxPassFail *passFail)
{
   if(list->length >= list->capacity)
   {
      *passFail = DmtxFail;
   }
   else
   {
      list->b[list->length++] = value;
      *passFail = DmtxPass;
   }
}

/**
 *
 *
 */
extern DmtxByte
dmtxByteListPop(DmtxByteList *list, DmtxPassFail *passFail)
{
   *passFail = (list->length > 0) ? DmtxPass : DmtxFail;

   return list->b[--(list->length)];
}

/**
 *
 *
 */
extern void
dmtxByteListPrint(DmtxByteList *list, char *prefix)
{
   int i;

   if(prefix != NULL)
      fprintf(stdout, "%s", prefix);

   for(i = 0; i < list->length; i++)
      fprintf(stdout, " %d", list->b[i]);

   fputc('\n', stdout);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxvector2.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* libdmtx - Data Matrix Encoding/Decoding Library
* Copyright 2008, 2009 Mike Laughton. All rights reserved.
*
* See LICENSE file in the main project directory for full
* terms of use and distribution.
*
* Contact: Mike Laughton <mike@dragonflylogic.com>
*
* \file dmtxvector2.c
* \brief 2D Vector math
*/

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2AddTo(DmtxVector2 *v1, const DmtxVector2 *v2)
{
  v1->X += v2->X;
  v1->Y += v2->Y;

  return v1;
}

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2Add(DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2)
{
  *vOut = *v1;

  return dmtxVector2AddTo(vOut, v2);
}

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2SubFrom(DmtxVector2 *v1, const DmtxVector2 *v2)
{
  v1->X -= v2->X;
  v1->Y -= v2->Y;

  return v1;
}

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2Sub(DmtxVector2 *vOut, const DmtxVector2 *v1, const DmtxVector2 *v2)
{
  *vOut = *v1;

  return dmtxVector2SubFrom(vOut, v2);
}

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2ScaleBy(DmtxVector2 *v, float s)
{
  v->X *= s;
  v->Y *= s;

  return v;
}

/**
*
*
*/
extern DmtxVector2 *
dmtxVector2Scale(DmtxVector2 *vOut, const DmtxVector2 *v, float s)
{
  *vOut = *v;

  return dmtxVector2ScaleBy(vOut, s);
}

/**
*
*
*/
extern float
dmtxVector2Cross(const DmtxVector2 *v1, const DmtxVector2 *v2)
{
  return (v1->X * v2->Y) - (v1->Y * v2->X);
}

/**
*
*
*/
extern float
dmtxVector2Norm(DmtxVector2 *v)
{
  float mag;

  mag = dmtxVector2Mag(v);

  if(mag <= DmtxAlmostZero)
     return -1.0; /* XXX this doesn't look clean */

  dmtxVector2ScaleBy(v, 1/mag);

  return mag;
}

/**
*
*
*/
extern float
dmtxVector2Dot(const DmtxVector2 *v1, const DmtxVector2 *v2)
{
  return (v1->X * v2->X) + (v1->Y * v2->Y);
}

/**
*
*
*/
extern float
dmtxVector2Mag(const DmtxVector2 *v)
{
  return sqrt(v->X * v->X + v->Y * v->Y);
}

/**
*
*
*/
extern float
dmtxDistanceFromRay2(const DmtxRay2 *r, const DmtxVector2 *q)
{
  DmtxVector2 vSubTmp;

  /* Assumes that v is a unit vector */
  assert(fabs(1.0 - dmtxVector2Mag(&(r->v))) <= DmtxAlmostZero);

  return dmtxVector2Cross(&(r->v), dmtxVector2Sub(&vSubTmp, q, &(r->p)));
}

/**
*
*
*/
extern float
dmtxDistanceAlongRay2(const DmtxRay2 *r, const DmtxVector2 *q)
{
  DmtxVector2 vSubTmp;

#ifdef DEBUG
  /* Assumes that v is a unit vector */
  if(fabs(1.0 - dmtxVector2Mag(&(r->v))) > DmtxAlmostZero) {
     ; /* XXX big error goes here */
  }
#endif

  return dmtxVector2Dot(dmtxVector2Sub(&vSubTmp, q, &(r->p)), &(r->v));
}

/**
*
*
*/
extern DmtxPassFail
dmtxRay2Intersect(DmtxVector2 *point, const DmtxRay2 *p0, const DmtxRay2 *p1)
{
  float numer, denom;
  DmtxVector2 w;

  denom = dmtxVector2Cross(&(p1->v), &(p0->v));
  if(fabs(denom) <= DmtxAlmostZero)
     return DmtxFail;

  dmtxVector2Sub(&w, &(p1->p), &(p0->p));
  numer = dmtxVector2Cross(&(p1->v), &w);

  return dmtxPointAlongRay2(point, p0, numer/denom);
}

/**
*
*
*/
extern DmtxPassFail
dmtxPointAlongRay2(DmtxVector2 *point, const DmtxRay2 *r, float t)
{
  DmtxVector2 vTmp;

  /* Ray should always have unit length of 1 */
  assert(fabs(1.0 - dmtxVector2Mag(&(r->v))) <= DmtxAlmostZero);

  dmtxVector2Scale(&vTmp, &(r->v), t);
  dmtxVector2Add(point, &(r->p), &vTmp);

  return DmtxPass;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "dmtxmatrix3.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxmatrix3.c
 * \brief 2D Matrix (3x3) math
 */

/**
 * \brief  Copy matrix contents
 * \param  m0 Copy target
 * \param  m1 Copy source
 * \return void
 */
extern void
dmtxMatrix3Copy(DmtxMatrix3 m0, DmtxMatrix3 m1)
{
   memcpy(m0, m1, sizeof(DmtxMatrix3));
}

/**
 * \brief  Generate identity transformation matrix
 * \param  m Generated matrix
 * \return void
 *
 *      | 1  0  0 |
 *  m = | 0  1  0 |
 *      | 0  0  1 |
 *
 *                  Transform "m"
 *            (doesn't change anything)
 *                       |\
 *  (0,1)  x----o     +--+ \    (0,1)  x----o
 *         |    |     |     \          |    |
 *         |    |     |     /          |    |
 *         +----*     +--+ /           +----*
 *  (0,0)     (1,0)      |/     (0,0)     (1,0)
 *
 */
extern void
dmtxMatrix3Identity(DmtxMatrix3 m)
{
   static DmtxMatrix3 tmp = { {1, 0, 0},
                              {0, 1, 0},
                              {0, 0, 1} };
   dmtxMatrix3Copy(m, tmp);
}

/**
 * \brief  Generate translate transformation matrix
 * \param  m Generated matrix
 * \param  tx
 * \param  ty
 * \return void
 *
 *      | 1  0  0 |
 *  m = | 0  1  0 |
 *      | tx ty 1 |
 *
 *                  Transform "m"
 *                      _____    (tx,1+ty)  x----o  (1+tx,1+ty)
 *                      \   |               |    |
 *  (0,1)  x----o       /   |      (0,1)  +-|--+ |
 *         |    |      /  /\|             | +----*  (1+tx,ty)
 *         |    |      \ /                |    |
 *         +----*       `                 +----+
 *  (0,0)     (1,0)                (0,0)     (1,0)
 *
 */
void dmtxMatrix3Translate(DmtxMatrix3 m, float tx, float ty)
{
   dmtxMatrix3Identity(m);
   m[2][0] = tx;
   m[2][1] = ty;
}

/**
 * \brief  Generate rotate transformation
 * \param  m Generated matrix
 * \param  angle
 * \return void
 *
 *     |  cos(a)  sin(a)  0 |
 * m = | -sin(a)  cos(a)  0 |
 *     |  0       0       1 |
 *                                       o
 *                  Transform "m"      /   `
 *                       ___         /       `
 *  (0,1)  x----o      |/   \       x          *  (cos(a),sin(a))
 *         |    |      '--   |       `        /
 *         |    |        ___/          `    /  a
 *         +----*                        `+  - - - - - -
 *  (0,0)     (1,0)                     (0,0)
 *
 */
extern void
dmtxMatrix3Rotate(DmtxMatrix3 m, float angle)
{
   float sinAngle, cosAngle;

   sinAngle = sin(angle);
   cosAngle = cos(angle);

   dmtxMatrix3Identity(m);
   m[0][0] = cosAngle;
   m[0][1] = sinAngle;
   m[1][0] = -sinAngle;
   m[1][1] = cosAngle;
}

/**
 * \brief  Generate scale transformation matrix
 * \param  m Generated matrix
 * \param  sx
 * \param  sy
 * \return void
 *
 *     | sx 0  0 |
 * m = | 0  sy 0 |
 *     | 0  0  1 |
 *
 *                  Transform "m"
 *                      _____     (0,sy)  x-------o (sx,sy)
 *                      \   |             |       |
 *  (0,1)  x----o       /   |      (0,1)  +----+  |
 *         |    |      /  /\|             |    |  |
 *         |    |      \ /                |    |  |
 *         +----*       `                 +----+--*
 *  (0,0)     (1,0)                (0,0)            (sx,0)
 *
 */
extern void
dmtxMatrix3Scale(DmtxMatrix3 m, float sx, float sy)
{
   dmtxMatrix3Identity(m);
   m[0][0] = sx;
   m[1][1] = sy;
}

/**
 * \brief  Generate shear transformation matrix
 * \param  m Generated matrix
 * \param  shx
 * \param  shy
 * \return void
 *
 *     | 0    shy  0 |
 * m = | shx  0    0 |
 *     | 0    0    1 |
 */
extern void
dmtxMatrix3Shear(DmtxMatrix3 m, float shx, float shy)
{
   dmtxMatrix3Identity(m);
   m[1][0] = shx;
   m[0][1] = shy;
}

/**
 * \brief  Generate top line skew transformation
 * \param  m
 * \param  b0
 * \param  b1
 * \param  sz
 * \return void
 *
 *     | b1/b0    0    (b1-b0)/(sz*b0) |
 * m = |   0    sz/b0         0        |
 *     |   0      0           1        |
 *
 *     (sz,b1)  o
 *             /|    Transform "m"
 *            / |
 *           /  |        +--+
 *          /   |        |  |
 * (0,b0)  x    |        |  |
 *         |    |      +-+  +-+
 * (0,sz)  +----+       \    /    (0,sz)  x----o
 *         |    |        \  /             |    |
 *         |    |         \/              |    |
 *         +----+                         +----+
 *  (0,0)    (sz,0)                (0,0)    (sz,0)
 *
 */
extern void
dmtxMatrix3LineSkewTop(DmtxMatrix3 m, float b0, float b1, float sz)
{
   assert(b0 >= DmtxAlmostZero);

   dmtxMatrix3Identity(m);
   m[0][0] = b1/b0;
   m[1][1] = sz/b0;
   m[0][2] = (b1 - b0)/(sz*b0);
}

/**
 * \brief  Generate top line skew transformation (inverse)
 * \param  m
 * \param  b0
 * \param  b1
 * \param  sz
 * \return void
 */
extern void
dmtxMatrix3LineSkewTopInv(DmtxMatrix3 m, float b0, float b1, float sz)
{
   assert(b1 >= DmtxAlmostZero);

   dmtxMatrix3Identity(m);
   m[0][0] = b0/b1;
   m[1][1] = b0/sz;
   m[0][2] = (b0 - b1)/(sz*b1);
}

/**
 * \brief  Generate side line skew transformation
 * \param  m
 * \param  b0
 * \param  b1
 * \param  sz
 * \return void
 */
extern void
dmtxMatrix3LineSkewSide(DmtxMatrix3 m, float b0, float b1, float sz)
{
   assert(b0 >= DmtxAlmostZero);

   dmtxMatrix3Identity(m);
   m[0][0] = sz/b0;
   m[1][1] = b1/b0;
   m[1][2] = (b1 - b0)/(sz*b0);
}

/**
 * \brief  Generate side line skew transformation (inverse)
 * \param  m
 * \param  b0
 * \param  b1
 * \param  sz
 * \return void
 */
extern void
dmtxMatrix3LineSkewSideInv(DmtxMatrix3 m, float b0, float b1, float sz)
{
   assert(b1 >= DmtxAlmostZero);

   dmtxMatrix3Identity(m);
   m[0][0] = b0/sz;
   m[1][1] = b0/b1;
   m[1][2] = (b0 - b1)/(sz*b1);
}

/**
 * \brief  Multiply two matrices to create a third
 * \param  mOut
 * \param  m0
 * \param  m1
 * \return void
 */
extern void
dmtxMatrix3Multiply(DmtxMatrix3 mOut, DmtxMatrix3 m0, DmtxMatrix3 m1)
{
   int i, j, k;
   float val;

   for(i = 0; i < 3; i++) {
      for(j = 0; j < 3; j++) {
         val = 0.0;
         for(k = 0; k < 3; k++) {
            val += m0[i][k] * m1[k][j];
         }
         mOut[i][j] = val;
      }
   }
}

/**
 * \brief  Multiply two matrices in place
 * \param  m0
 * \param  m1
 * \return void
 */
extern void
dmtxMatrix3MultiplyBy(DmtxMatrix3 m0, DmtxMatrix3 m1)
{
   DmtxMatrix3 mTmp;

   dmtxMatrix3Copy(mTmp, m0);
   dmtxMatrix3Multiply(m0, mTmp, m1);
}

/**
 * \brief  Multiply vector and matrix
 * \param  vOut Vector (output)
 * \param  vIn Vector (input)
 * \param  m Matrix to be multiplied
 * \return DmtxPass | DmtxFail
 */
extern int
dmtxMatrix3VMultiply(DmtxVector2 *vOut, DmtxVector2 *vIn, DmtxMatrix3 m)
{
   float w;

   w = vIn->X*m[0][2] + vIn->Y*m[1][2] + m[2][2];
   if(fabs(w) <= DmtxAlmostZero) {
      vOut->X = FLT_MAX;
      vOut->Y = FLT_MAX;
      return DmtxFail;
   }

   vOut->X = (vIn->X*m[0][0] + vIn->Y*m[1][0] + m[2][0])/w;
   vOut->Y = (vIn->X*m[0][1] + vIn->Y*m[1][1] + m[2][1])/w;

   return DmtxPass;
}

/**
 * \brief  Multiply vector and matrix in place
 * \param  v Vector (input and output)
 * \param  m Matrix to be multiplied
 * \return DmtxPass | DmtxFail
 */
extern int
dmtxMatrix3VMultiplyBy(DmtxVector2 *v, DmtxMatrix3 m)
{
   int success;
   DmtxVector2 vOut;

   success = dmtxMatrix3VMultiply(&vOut, v, m);
   *v = vOut;

   return success;
}

/**
 * \brief  Print matrix contents to STDOUT
 * \param  m
 * \return void
 */
extern void
dmtxMatrix3Print(DmtxMatrix3 m)
{
   fprintf(stdout, "%8.8f\t%8.8f\t%8.8f\n", m[0][0], m[0][1], m[0][2]);
   fprintf(stdout, "%8.8f\t%8.8f\t%8.8f\n", m[1][0], m[1][1], m[1][2]);
   fprintf(stdout, "%8.8f\t%8.8f\t%8.8f\n", m[2][0], m[2][1], m[2][2]);
   fprintf(stdout, "\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void imlib_find_datamatrices(list_t *out, image_t *ptr, rectangle_t *roi, int effort)
{
    uint8_t *grayscale_image = (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->data : fb_alloc(roi->w * roi->h, FB_ALLOC_NO_HINT);
    umm_init_x(fb_avail());

    DmtxImage *image = dmtxImageCreate(grayscale_image,
                                       (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->w : roi->w,
                                       (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? ptr->h : roi->h,
                                       DmtxPack8bppK);
    DmtxDecode *decode = dmtxDecodeCreate(image, 1);
    dmtxDecodeSetProp(decode, DmtxPropXmin, (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->x : 0);
    dmtxDecodeSetProp(decode, DmtxPropYmin, (ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->y : 0);
    dmtxDecodeSetProp(decode, DmtxPropXmax, ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->x : 0) + (roi->w - 1));
    dmtxDecodeSetProp(decode, DmtxPropYmax, ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? roi->y : 0) + (roi->h - 1));

    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        default: {
            memset(grayscale_image, 0, roi->w * roi->h);
            break;
        }
    }

    list_init(out, sizeof(find_datamatrices_list_lnk_data_t));

    int max_iterations = effort;
    int current_iterations = 0;
    for (DmtxRegion *region = dmtxRegionFindNext(decode, max_iterations, &current_iterations); region; region = dmtxRegionFindNext(decode, max_iterations, &current_iterations)) {
        DmtxMessage *message = dmtxDecodeMatrixRegion(decode, region, DmtxUndefined);

        if (message) {
            find_datamatrices_list_lnk_data_t lnk_data;

            DmtxVector2 p[4];

            p[0].X = p[0].Y = p[1].Y = p[3].X = 0.0;
            p[1].X = p[3].Y = p[2].X = p[2].Y = 1.0;

            dmtxMatrix3VMultiplyBy(&p[0], region->fit2raw);
            dmtxMatrix3VMultiplyBy(&p[1], region->fit2raw);
            dmtxMatrix3VMultiplyBy(&p[2], region->fit2raw);
            dmtxMatrix3VMultiplyBy(&p[3], region->fit2raw);

            int height = dmtxDecodeGetProp(decode, DmtxPropHeight);

            rectangle_init(&(lnk_data.rect),
                           fast_roundf(p[0].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x),
                           height - 1 - fast_roundf(p[0].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y),
                           0,
                           0);

            for (size_t k = 1, l = (sizeof(p) / sizeof(p[0])); k < l; k++) {
                rectangle_t temp;
                rectangle_init(&temp,
                               fast_roundf(p[k].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x),
                               height - 1 - fast_roundf(p[k].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y),
                               0,
                               0);
                rectangle_united(&(lnk_data.rect), &temp);
            }

            // Add corners...
            lnk_data.corners[0].x =              fast_roundf(p[3].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x); // top-left
            lnk_data.corners[0].y = height - 1 - fast_roundf(p[3].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y); // top-left
            lnk_data.corners[1].x =              fast_roundf(p[2].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x); // top-right
            lnk_data.corners[1].y = height - 1 - fast_roundf(p[2].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y); // top-right
            lnk_data.corners[2].x =              fast_roundf(p[1].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x); // bottom-right
            lnk_data.corners[2].y = height - 1 - fast_roundf(p[1].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y); // bottom-right
            lnk_data.corners[3].x =              fast_roundf(p[0].X) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->x); // bottom-left
            lnk_data.corners[3].y = height - 1 - fast_roundf(p[0].Y) + ((ptr->bpp == IMAGE_BPP_GRAYSCALE) ? 0 : roi->y); // bottom-left

            // Payload is NOT already null terminated.
            lnk_data.payload_len = message->outputIdx;
            lnk_data.payload = xalloc(message->outputIdx);
            memcpy(lnk_data.payload, message->output, message->outputIdx);

            int rotate = fast_roundf((((2 * M_PI) + fast_atan2f(p[1].Y - p[0].Y, p[1].X - p[0].X)) * 180) / M_PI);
            if(rotate >= 360) rotate -= 360;

            lnk_data.rotation = rotate;
            lnk_data.rows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, region->sizeIdx);
            lnk_data.columns = dmtxGetSymbolAttribute(DmtxSymAttribSymbolCols, region->sizeIdx);
            lnk_data.capacity = dmtxGetSymbolAttribute(DmtxSymAttribSymbolDataWords, region->sizeIdx);
            lnk_data.padding = message->padCount;

            list_push_back(out, &lnk_data);

            dmtxMessageDestroy(&message);
        }

        dmtxRegionDestroy(&region);
    }

    dmtxDecodeDestroy(&decode);
    dmtxImageDestroy(&image);

    fb_free(); // umm_init_x();
    if (ptr->bpp != IMAGE_BPP_GRAYSCALE) fb_free(); // grayscale_image;
}

#pragma GCC diagnostic pop
#endif //IMLIB_ENABLE_DATAMATRICES
