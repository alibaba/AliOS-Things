/*
* Copyright (c) 2008-2017 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : BaseUtils.h
* Description :
* History :
*   Author  : Khan <chengkan@allwinnertech.com>
*   Date    : 2017/3/2
*/

#ifndef BASEUTILS_H
#define BASEUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <CdxTypes.h>
#include <CdxParser.h>
#include <CdxStream.h>
#include <CdxMemory.h>

typedef enum _kbool
{
    kfalse = 0,
    ktrue
}kbool;

typedef enum _kerrno
{
    KERR_NONE = 0,
    KERR_NULL_PTR = -1000,
    KERR_INVAILD_DATA,
    KERR_OUT_OF_RANGE,
    KERR_NO_DATA,
    KERR_NO_MEM,
    KERR_RESERVED,
}kerrno;

static __inline cdx_uint16 U16_AT(const cdx_uint8 *ptr) {
    return ptr[0] << 8 | ptr[1];
}

static __inline cdx_uint32 U32_AT(const cdx_uint8 *ptr) {
    return ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
}

static __inline cdx_uint64 U64_AT(const cdx_uint8 *ptr) {
    return ((cdx_uint64)U32_AT(ptr)) << 32 | U32_AT(ptr + 4);
}

static __inline cdx_uint16 bswap16(cdx_uint16 x)
{
    x= (x>>8) | (x<<8);
    return x;
}


#endif
