/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __PLAT_TYPES_H__
#define __PLAT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"
#ifdef FPGA
#define __TCMDATA __attribute__((section(".tcmdata")))
#define __RAMCODE __attribute__((section(".ramcode")))
#define __RAMDATA __attribute__((section(".ramdata")))
#define __PSRAMCODE 
#define __PSRAMDATA
#define __PSRAMBSS 
#define __SRAM_EXT_BSS __attribute__((section(".sram_bss")))

#ifdef RESTRICTED_RAM
#define __RRAMCODE __attribute__((section(".rram_text")))
#define __RRAMDATA __attribute__((section(".rram_data")))
#define __RRAMBSS __attribute__((section(".rram_bss")))
#else
#define __RRAMCODE
#define __RRAMDATA
#define __RRAMBSS
#endif
#define __SRAMCODE __attribute__((section(".sram_text")))
#define __SRAMDATA __attribute__((section(".sram_data")))
#define __SRAMBSS __attribute__((section(".sram_bss")))
#define __AUDMA __attribute__((section(".audma")))
#define __FSRAMCODE __attribute__((section(".fast_text_sram")))
#define __BOOTSRAMCODE   __attribute__((section(".boot_text_sram")))

#ifdef __FAR_FIELD_2000IWA__
#define __RX_CODE
#else
#define __RX_CODE  __SRAMCODE
#endif

#else /*FPGA*/

#define __TCMDATA __attribute__((section(".tcmdata")))
#define __RAMCODE __attribute__((section(".ramcode")))
#define __RAMDATA __attribute__((section(".ramdata")))

#ifdef RESTRICTED_RAM
#define __RRAMCODE __attribute__((section(".rram_text")))
#define __RRAMDATA __attribute__((section(".rram_data")))
#define __RRAMBSS __attribute__((section(".rram_bss")))
#else
#define __RRAMCODE
#define __RRAMDATA
#define __RRAMBSS
#endif
#define __SRAMCODE __attribute__((section(".sram_text")))
#define __SRAMDATA __attribute__((section(".sram_data")))
#define __SRAMBSS __attribute__((section(".sram_bss")))
#define __AUDMA __attribute__((section(".audma")))
#define __FSRAMCODE __attribute__((section(".fast_text_sram")))
#define __BOOTSRAMCODE   __attribute__((section(".boot_text_sram")))

#if defined(CHIP_HAS_PSRAM) && defined(PSRAM_ENABLE)
#define __PSRAMCODE __attribute__((section(".psram_text")))
#define __PSRAMDATA __attribute__((section(".psram_data")))
#define __PSRAMBSS __attribute__((section(".psram_bss")))
#elif defined(CHIP_HAS_PSRAMUHS) && defined(PSRAMUHS_ENABLE)
#define __PSRAMUHSCODE		__attribute__((section(".psramuhs_text")))
#define __PSRAMUHSDATA	__attribute__((section(".psramuhs_data")))
#define __PSRAMUHSBSS		__attribute__((section(".psramuhs_bss")))
#define __PSRAMCODE		__PSRAMUHSCODE
#define __PSRAMDATA		__PSRAMUHSDATA
// #define __PSRAMBSS			__PSRAMUHSBSS
#define __PSRAMBSS
#else
#define __PSRAMCODE
#define __PSRAMDATA
#define __PSRAMBSS
#endif

#define __SRAM_EXT_CODE  __PSRAMCODE
#define __SRAM_EXT_DATA  __PSRAMDATA
#define __SRAM_EXT_BSS     __PSRAMBSS

#ifdef __FAR_FIELD_2000IWA__
#define __RX_CODE
#else
#define __RX_CODE  __SRAMCODE
#endif

#endif /*FPGA*/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef X32_F
#define X32_F "x"
#endif
#ifndef X16_F
#define X16_F "x"
#endif
#ifndef U32_F
#define U32_F "d"
#endif
#ifndef S32_F
#define S32_F "d"
#endif
#ifndef S16_F
#define S16_F "d"
#endif
#ifndef U16_F
#define U16_F "d"
#endif

typedef long long __int64;
typedef long long __le64;

typedef unsigned short __le16;
typedef unsigned int __le32;

typedef unsigned short __be16;
typedef unsigned int __be32;
typedef unsigned short __u16;
typedef unsigned char __u8;
typedef  short __s16;
typedef  char __s8;
typedef unsigned int __u32;
typedef unsigned long u32;
typedef  int __s32;

typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */
typedef void           (*FUNCPtr)(void*);

typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */

typedef unsigned char UCHAR;
typedef char CHAR;
typedef unsigned char* PUCHAR;
typedef signed char* PCHAR;

typedef unsigned char  UINT8;
typedef unsigned char  uint8;
typedef unsigned short UINT16;
typedef unsigned short uint16;
typedef unsigned int   UINT32;                                                                                                                                          typedef unsigned int   uint32;                                                                                                                                                                             
typedef unsigned int   uint32;   

typedef  signed char  INT8;
typedef signed char int8;
typedef  short INT16;
typedef  short int16;
typedef  int   INT32;
typedef  int   int32;

typedef uint8_t u8_t;
typedef int8_t s8_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;

typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned long               u32;
typedef unsigned long long          u64;

typedef unsigned char               U8;
typedef unsigned short              U16;
typedef unsigned int               U32;
typedef unsigned long long          U64;

typedef char                        s8;
typedef short                       s16;
typedef long                        s32;
typedef long long                   s64;

//typedef char                        ascii;
typedef unsigned char               byte;           /*  unsigned 8-bit data     */
typedef unsigned short              word;           /*  unsigned 16-bit data    */
typedef unsigned long               dword;          /*  unsigned 32-bit data    */


/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifndef __I
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif


#define BITFIELD_VAL(field, value)          (((value) & (field ## _MASK >> field ## _SHIFT)) << field ## _SHIFT)
#define SET_BITFIELD(reg, field, value)     (((reg) & ~field ## _MASK) | BITFIELD_VAL(field, value))
#define GET_BITFIELD(reg, field)            (((reg) & field ## _MASK) >> field ## _SHIFT)


/* Frequently used macros */

#ifndef ALIGN
#define ALIGN(val,exp)                  (((val) + ((exp)-1)) & ~((exp)-1))
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)                   (sizeof(a)/sizeof(a[0]))
#endif
#define LAST_ELEMENT(x)                 (&x[ARRAY_SIZE(x)-1])
#define BOUND(x, min, max)              ( (x) < (min) ? (min) : ((x) > (max) ? (max):(x)) )
#define ROUND_SIZEOF(t)                 ((sizeof(t)+sizeof(int)-1)&~(sizeof(int)-1))

#ifndef MAX
#define MAX(a,b)                        (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)                        (((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(x)                          ((x<0)?(-(x)):(x))
#endif

#define TO_STRING_A(s)                  # s
#define TO_STRING(s)                    TO_STRING_A(s)

#ifdef __GNUC__

/* Remove const cast-away warnings from gcc -Wcast-qual */
#define __UNCONST(a)                    ((void *)(unsigned long)(const void *)(a))

/// From http://www.ibm.com/developerworks/linux/library/l-gcc-hacks/
/// Macro to use in a if statement to tell the compiler this branch
/// is likely taken, and optimize accordingly.
#define LIKELY(x)                       __builtin_expect(!!(x), 1)
/// Macro to use in a if statement to tell the compiler this branch
/// is unlikely take, and optimize accordingly.
#define UNLIKELY(x)                     __builtin_expect(!!(x), 0)

/// For packing structure
#define PACKED                          __attribute__((packed))

/// To describe alignment
#define ALIGNED(a)                      __attribute__((aligned(a)))

/// For possibly unused functions or variables (e.g., debugging stuff)
#define POSSIBLY_UNUSED                 __attribute__((unused))

/// For functions or variables must be emitted even if not referenced
#define USED                            __attribute__((used))

/// For inline functions
#define ALWAYS_INLINE                   __attribute__((always_inline))

// For functions never inlined
#define NOINLINE                        __attribute__((noinline))

// For functions not caring performance
#ifdef __ARMCC_VERSION
#define OPT_SIZE
#else
#define OPT_SIZE                        __attribute__((optimize("Os")))
#endif

// For functions not returning
#define NORETURN                        __attribute__((noreturn))

// For ASM functions in C
#ifdef __arm__
#define NAKED                           __attribute__((naked))
#else
#define NAKED                           __attribute__((error("Unsupport naked functions")))
#endif

// For weak symbols
#define WEAK                            __attribute__((weak))

// Structure offset
#ifndef OFFSETOF
#define OFFSETOF(TYPE, MEMBER)          ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * CONTAINER_OF - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - OFFSETOF(type,member) );})
#endif

#else // Not GCC

#define __UNCONST(a)
#define LIKELY(x)
#define UNLIKELY(x)
#define PACKED
#define ALIGNED(a)
#define POSSIBLY_UNUSED
#define USED
#define ALWAYS_INLINE
#define NOINLINE
#define OPT_SIZE
#define NORETURN
#define NAKED
#define WEAK
#define OFFSETOF(TYPE, MEMBER)

#endif // Not GCC

/// C preprocessor conditional check
/// --------------------------------
#define GCC_VERSION (__GNUC__ * 10000 \
                       + __GNUC_MINOR__ * 100 \
                       + __GNUC_PATCHLEVEL__)

#if defined(__GNUC__) && (GCC_VERSION >= 40600) && !defined(__cplusplus)

// GCC 4.6 or later
#define STATIC_ASSERT(e, m)             _Static_assert(e, m)

#elif defined(__GNUC__) && (GCC_VERSION >= 40300) && defined(__cplusplus) && (__cplusplus >= 201103L)

#define STATIC_ASSERT(e, m)             static_assert(e, m)

#else // No built-in static assert

/// FROM: http://www.pixelbeat.org/programming/gcc/static_assert.html
#define ASSERT_CONCAT_(a, b)            a##b
#define ASSERT_CONCAT(a, b)             ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
#define STATIC_ASSERT(e, m) \
    enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) };
#else
/* This can't be used twice on the same line so ensure if using in headers
 * that the headers are not included twice (by wrapping in #ifndef...#endif)
 * Note it doesn't cause an issue when used on same line of separate modules
 * compiled with gcc -combine -fwhole-program.  */
#define STATIC_ASSERT(e, m) \
    enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) };
#endif

#endif // No built-in static assert
/// --------------------------------

#ifdef __cplusplus
}
#endif

#endif

