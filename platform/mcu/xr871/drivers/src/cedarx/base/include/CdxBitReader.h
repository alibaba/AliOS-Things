/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxBitReader.h
 * Description : BitReader
 * History :
 *
 */

#ifndef CDX_BIT_READER_H
#define CDX_BIT_READER_H

#include <CdxTypes.h>

typedef struct CdxBitReaderS CdxBitReaderT;

struct CdxBitReaderOpsS
{
    void (*destroy)(CdxBitReaderT *);

    cdx_uint32 (*getBits)(CdxBitReaderT *, cdx_uint32);

    void (*skipBits)(CdxBitReaderT *, cdx_uint32);

    void (*putBits)(CdxBitReaderT *, cdx_uint32, cdx_uint32);

    cdx_uint32 (*numBitsLeft)(CdxBitReaderT *) ;

    const cdx_uint8 *(*data)(CdxBitReaderT *);
};

struct CdxBitReaderS
{
    struct CdxBitReaderOpsS *ops;
};

#ifdef __cplusplus
extern "C"
{
#endif

CdxBitReaderT *CdxBitReaderCreate(const cdx_uint8 *data, cdx_uint32 size);
void CdxBitReaderDestroy(struct CdxBitReaderS *br);
cdx_uint32 CdxBitReaderGetBits(struct CdxBitReaderS *br, cdx_uint32 n);
void CdxBitReaderSkipBits(struct CdxBitReaderS *br, cdx_uint32 n);
void CdxBitReaderPutBits(struct CdxBitReaderS *br, cdx_uint32 x, cdx_uint32 n);
cdx_uint32 CdxBitReaderNumBitsLeft(struct CdxBitReaderS *br) ;
const cdx_uint8 *CdxBitReaderData(struct CdxBitReaderS *br);

#ifdef __cplusplus
}
#endif

#endif
