/************************************************************************/
/**	\file	cc.h
 *	\brief	LwIP definitions for Blackfin/VisualDSP++ platform
 *
 *	\date	2011/09/28
 *	\author	Stephane Lesage & Pierre Audenard for LwIP
 *
 */
/************************************************************************/
#ifndef _ARCH_CC_H_
#define _ARCH_CC_H_

#include <limits.h>
#include <string.h>
#include <errno.h>
#undef EDOM
#undef EILSEQ
#undef ENOENT
#undef ERANGE

// C674x platform is little endian
#undef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN

// basic types
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

// addresses are 32-bits long
typedef u32_t mem_ptr_t;

// printf format specifiers
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

// PBUF_POOL needs to be aligned to cache line size
#ifndef __cplusplus
extern u8_t memp_memory_PBUF_POOL_base[] __attribute__ ((aligned (128)));
#endif

// compiler hints for packing structures
// using MEM_ALIGNMENT=4, ETH_PAD_SIZE=2 and RXBUFFEROFFSET=2 option
// protocol headers are already 32 bits aligned so we don't need packing
#if 1
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#else
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT  __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#endif

// byte-swapping
#define LWIP_PLATFORM_BYTESWAP 1
#define LWIP_PLATFORM_HTONS(x) ((u16_t)_swap4((u32_t)x))
#define LWIP_PLATFORM_HTONL(x) (_rotl(_swap4(x), 16))

// checksum algorithm
#define LWIP_CHKSUM_ALGORITHM 2

// prototypes for printf(), fflush() and abort()
#include <stdio.h>
#include <stdlib.h>

// random function
#define LWIP_RAND rand

// diagnostic output
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)	do { printf x; } while(0)
#else
#define LWIP_PLATFORM_DIAG(x)
#endif

// assert output
#ifndef LWIP_NOASSERT
#define LWIP_PLATFORM_ASSERT(x) do { printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); asm( " SWBP 0" ); } while(0)
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif

#endif // _ARCH_CC_H_
