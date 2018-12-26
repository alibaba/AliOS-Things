/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxBinary.h
 * Description : Binary
 * History :
 *
 */

#ifndef CDX_BINARY_H
#define CDX_BINARY_H
#include <CdxTypes.h>

static inline cdx_uint32 GetU8(const cdx_uint8 *data)
{
    return (cdx_uint32)(*data);
}

static inline cdx_uint32 GetLE16(const cdx_uint8 *data)
{
    return GetU8(data) | (GetU8(data + 1) << 8);
}

static inline cdx_uint32 GetLE32(const cdx_uint8 *data)
{
    return GetLE16(data) | (GetLE16(data + 2) << 16);
}

static inline cdx_uint64 GetLE64(const cdx_uint8 *data)
{
    return ((cdx_uint64)GetLE32(data)) | (((cdx_uint64)GetLE32(data + 4)) << 32);
}

static inline cdx_uint32 GetBE16(const cdx_uint8 *data)
{
    return (GetU8(data) << 8) | GetU8(data + 1);
}

static inline cdx_uint32 GetBE32(const cdx_uint8 *data)
{
    return (GetBE16(data) << 16) | GetBE16(data + 2);
}

static inline cdx_uint64 GetBE64(const cdx_uint8 *data)
{
    return (((cdx_uint64)GetBE32(data)) << 32) | ((cdx_uint64)GetBE32(data + 4));
}

#endif
