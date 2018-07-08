#ifndef LWIP_HDR_ARCH_CC_H
#define LWIP_HDR_ARCH_CC_H

#include "cs_types.h"
#include "stdio.h"
#include "time.h"
#include "stdint.h"
#define LWIP_NO_STDINT_H 1
#define LWIP_NO_INTTYPES_H 1
#define LWIP_NO_LIMITS_H 1
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#define BYTE_ORDER  LITTLE_ENDIAN

#define u8_t UINT8
#define s8_t signed char
#define u16_t UINT16
#define s16_t INT16
#define u32_t UINT32
#define s32_t INT32
//I'm Not sure
#define mem_ptr_t UINT32

#define X8_F  "02x"
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"
#define SZT_F "ld"

#ifndef _PTRDIFF_T_DEFINED
typedef int ptrdiff_t;
#define PTRDIFF(p1, p2, type)   ((p1) - (p2))
#define _PTRDIFF_T_DEFINED
#endif
#define LWIP_TIMEVAL_PRIVATE 0
#define LWIP_RAND() ((u32_t)rand())

#define LWIP_PLATFORM_ASSERT(message)    sys_arch_assert((u8_t*)message)
#define LWIP_PLATFORM_DIAG(x)	do {sys_arch_printf x;} while(0)

#endif
