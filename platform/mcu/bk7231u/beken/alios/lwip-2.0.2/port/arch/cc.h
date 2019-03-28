/*
 * cc.h       - Architecture environment, some compiler specific, some
 *             environment specific (probably should move env stuff 
 *             to sys_arch.h.)
 *
 * Typedefs for the types used by lwip -
 *   u8_t, s8_t, u16_t, s16_t, u32_t, s32_t, mem_ptr_t
 *
 * Compiler hints for packing lwip's structures -
 *   PACK_STRUCT_FIELD(x)
 *   PACK_STRUCT_STRUCT
 *   PACK_STRUCT_BEGIN
 *   PACK_STRUCT_END
 *
 * Platform specific diagnostic output -
 *   LWIP_PLATFORM_DIAG(x)    - non-fatal, print a message.
 *   LWIP_PLATFORM_ASSERT(x)  - fatal, print message and abandon execution.
 *   Portability defines for printf formatters:
 *   U16_F, S16_F, X16_F, U32_F, S32_F, X32_F, SZT_F
 *
 * "lightweight" synchronization mechanisms -
 *   SYS_ARCH_DECL_PROTECT(x) - declare a protection state variable.
 *   SYS_ARCH_PROTECT(x)      - enter protection mode.
 *   SYS_ARCH_UNPROTECT(x)    - leave protection mode.
 *
 * If the compiler does not provide memset() this file must include a
 * definition of it, or include a file which defines it.
 *
 * This file must either include a system-local <errno.h> which defines
 * the standard *nix error codes, or it should #define LWIP_PROVIDE_ERRNO
 * to make lwip/arch.h define the codes which are used throughout.
 */
 
#ifndef __CC_H__
#define __CC_H__

#include "typedef.h"
#include "uart_pub.h"
// for sys interrupt
#include "ll.h"

/*
 *   Typedefs for the types used by lwip -
 *   u8_t, s8_t, u16_t, s16_t, u32_t, s32_t, mem_ptr_t
 */
typedef unsigned   char    u8_t;      /* Unsigned 8 bit quantity         */
typedef signed     char    s8_t;      /* Signed    8 bit quantity        */
typedef unsigned   short   u16_t;     /* Unsigned 16 bit quantity        */
typedef signed     short   s16_t;     /* Signed   16 bit quantity        */
typedef unsigned   long    u32_t;     /* Unsigned 32 bit quantity        */
typedef signed     long    s32_t;     /* Signed   32 bit quantity        */
//typedef unsigned   long    mem_ptr_t; /* Unsigned 32 bit quantity        */
typedef int			       intptr_t;
typedef unsigned int       uintptr_t;

#define LWIP_MAILBOX_QUEUE  1
#define LWIP_TIMEVAL_PRIVATE 0
#define LWIP_NO_INTTYPES_H  1

#if defined(__GNUC__)
 #define PACK_STRUCT_BEGIN
 #define PACK_STRUCT_STRUCT __attribute__((packed))
 #define PACK_STRUCT_FIELD(x) x
#elif defined(__ICCARM__)
 #define PACK_STRUCT_BEGIN __packed
 #define PACK_STRUCT_STRUCT
 #define PACK_STRUCT_FIELD(x) x
#else
 #define PACK_STRUCT_BEGIN
 #define PACK_STRUCT_STRUCT
 #define PACK_STRUCT_FIELD(x) x
#endif


/*
 *  Platform specific diagnostic output -
 *   LWIP_PLATFORM_DIAG(x)    - non-fatal, print a message.
 *   LWIP_PLATFORM_ASSERT(x)  - fatal, print message and abandon execution.
 *   Portability defines for printf formatters:
 *   U16_F, S16_F, X16_F, U32_F, S32_F, X32_F, SZT_F
 */
#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   fatal_prf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {fatal_prf x ;} while(0)
#endif 
 
#if LWIP_NO_INTTYPES_H
#define U8_F  "2d"
#define X8_F  "2x"
#define U16_F "4d"
#define S16_F "4d"
#define X16_F "4x"
#define U32_F "8ld"
#define S32_F "8ld"
#define X32_F "8lx"
#define SZT_F U32_F
#endif

/*
 * unknow defination
 */
// cup byte order 
#ifndef BYTE_ORDER
#define BYTE_ORDER          LITTLE_ENDIAN
#endif

#define LWIP_RAND()        ((uint32_t)rand())
#endif
// eof

