/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxTypes.h
 * Description : Types
 * History :
 *
 */

#ifndef CDX_TYPES_H
#define CDX_TYPES_H

/* This is not safe as ((void*)0) for C, and you cannot define NULL as
 * ((void*)0) for C++, so don't bother to define your own NULL.
#ifdef NULL
#undef NULL
#endif
#define NULL 0
*/

#define CDX_SUCCESS 0
#define CDX_FAILURE (-1)

typedef long long cdx_int64;
typedef unsigned long long cdx_uint64;

typedef int cdx_int32;
typedef unsigned int cdx_uint32;

typedef short cdx_int16;
typedef unsigned short cdx_uint16;

typedef unsigned char cdx_uint8;

/* For gcc, char is default to unsigned on ARM and signed on x86 platform. So
 * typedef char cdx_int8 is wrong.
 */
typedef signed char cdx_int8;

typedef unsigned long cdx_ulong;
typedef long cdx_long;

typedef char cdx_char;
typedef int cdx_bool;
typedef unsigned long cdx_size;
typedef signed long cdx_ssize;

typedef float cdx_float;

typedef struct CdxBufferS CdxBufferT;
typedef struct CdxListNodeS CdxListNodeT;
typedef struct CdxListS CdxListT;

#ifdef AWP_DEBUG
#define CDX_INTERFACE
#else
#define CDX_INTERFACE static inline
#endif

#ifdef __cplusplus
#define CDX_EXTERN extern "C"
#else
#define CDX_EXTERN
#endif

#define CDX_API CDX_EXTERN

typedef cdx_int32 cdx_err;

#define CDX_TRUE 1
#define CDX_FALSE 0

#define CedarXMin(a, b) ((a) < (b) ? (a) : (b))

#define CdxOffsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define CdxContainerOf(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - CdxOffsetof(type,member) ); })

#define CdxInt8PtrTypeCheck(ptr) (ptr + (ptr - (cdx_int8*)ptr))

#define CdxUint8PtrTypeCheck(ptr) (ptr + (ptr - (cdx_uint8*)ptr))

#define CdxInt16PtrTypeCheck(ptr) (ptr + (ptr - (cdx_int16*)ptr))

#define CdxUint16PtrTypeCheck(ptr) (ptr + (ptr - (cdx_uint16*)ptr))

#define CdxInt32PtrTypeCheck(ptr) (ptr + (ptr - (cdx_int32*)ptr))

#define CdxUint32PtrTypeCheck(ptr) (ptr + (ptr - (cdx_uint32*)ptr))

#define CdxInt64PtrTypeCheck(ptr) (ptr + (ptr - (cdx_int64*)ptr))

#define CdxUint64PtrTypeCheck(ptr) (ptr + (ptr - (cdx_uint64*)ptr))

enum CdxMediaTypeE
{
    CDX_MEDIA_UNKNOWN = -1,
    CDX_MEDIA_VIDEO = 0,
    CDX_MEDIA_AUDIO,
    CDX_MEDIA_SUBTITLE,
    CDX_MEDIA_METADATA,
    CDX_MEDIA_DATA,
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#endif
