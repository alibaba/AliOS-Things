/************************************************************************/
/**	\file	cc.h
 *	\brief	LwIP definitions for Blackfin/VisualDSP++ platform
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _ARCH_CC_H_
#define _ARCH_CC_H_

// remove warning for snmp files
#pragma diag(suppress: 191)
// remove warning for misaligned pointers
#pragma diag(suppress: 1482)

#include <limits.h>
#include <string.h>

// blackfin platform is little endian
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
extern u8_t memp_memory_PBUF_POOL_base[] __attribute__ ((aligned (32)));
#endif

// compiler hints for packing structures
// using MEM_ALIGNMENT=4, ETH_PAD_SIZE=2 and RXDWA option
// protocol headers are already 32 bits aligned so we don't need packing
#if 1
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#else
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN _Pragma("pack(2)")
#define PACK_STRUCT_END   _Pragma("pack()")
#endif

// byte-swapping
#include <ccblkfn.h> 
#define LWIP_PLATFORM_BYTESWAP 1
#define LWIP_PLATFORM_HTONS(x) ((u16_t)byteswap2((s16_t)(x)))
#define LWIP_PLATFORM_HTONL(x) ((u32_t)byteswap4((s32_t)(x)))

// checksum algorithm
#define LWIP_CHKSUM_ALGORITHM 2

// prototypes for printf(), fflush() and abort()
#include <stdio.h>
#include <stdlib.h>

// random function
#define LWIP_RAND rand

// errno
#define ERRNO
extern "C" int* _Geterrno(void);
#define errno (*(_Geterrno()))

// diagnostic output
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)	do { printf x; } while(0)
#else
#define LWIP_PLATFORM_DIAG(x)
#endif

// assert output
#ifndef LWIP_NOASSERT
#define LWIP_PLATFORM_ASSERT(x) do { printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); fflush(NULL); abort(); } while(0)
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif

#endif // _ARCH_CC_H_
