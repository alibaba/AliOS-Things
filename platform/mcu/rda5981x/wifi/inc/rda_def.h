#ifndef _RDA_DEF_H
#define _RDA_DEF_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>

#ifndef FALSE
#define FALSE               (0)
#endif
#ifndef TRUE
#define TRUE                (1)
#endif

#define R_TRUE            TRUE    /* boolean true  */
#define R_FALSE           FALSE   /* boolean fails */
#define R_ERR             (-1)
#define R_NOERR           (0)

#define r_printf          printf
#define r_malloc          aos_malloc
#define r_calloc          calloc
#define r_free            aos_free
#define r_memset          memset
#define r_memcpy          memcpy
#define r_strncmp         strncmp
#define r_strcmp          strcmp
#define r_strlen          strlen
#define r_strstr          strstr
#define r_strcpy          strcpy
#define r_strncpy         strncpy
#define r_memmove         memmove
#define r_memcmp          memcmp

typedef signed char             r_s8;           /* 8bit integer type */
typedef signed short            r_s16;          /* 16bit integer type */
typedef signed int              r_s32;          /* 32bit integer type */
typedef signed long long        r_s64;          /* 64bit integer type */
typedef unsigned char           r_u8;           /* 8bit unsigned integer type */
typedef unsigned short          r_u16;          /* 16bit unsigned integer type */
typedef unsigned int            r_u32;          /* 32bit unsigned integer type */
typedef unsigned long long      r_u64;          /* 64bit unsigned integer type */
typedef bool                    r_bool;         /* boolean type */
typedef void                    r_void;         /* void */

typedef unsigned int            size_t;

#ifdef __CC_ARM                         /* ARM Compiler */
    #define r_inline                   static __inline
#elif defined (__IAR_SYSTEMS_ICC__)     /* IAR Compiler */
    #define r_inline                   static inline
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define r_inline                   static inline
#endif

#ifndef BIT
#define BIT(x)                          (1ul << (x))

/*****************************************************************************/
/* Constant Definitions                                                      */
/*****************************************************************************/

#define BIT0                            (1ul << 0)
#define BIT1                            (1ul << 1)
#define BIT2                            (1ul << 2)
#define BIT3                            (1ul << 3)
#define BIT4                            (1ul << 4)
#define BIT5                            (1ul << 5)
#define BIT6                            (1ul << 6)
#define BIT7                            (1ul << 7)
#define BIT8                            (1ul << 8)
#define BIT9                            (1ul << 9)
#define BIT10                           (1ul << 10)
#define BIT11                           (1ul << 11)
#define BIT12                           (1ul << 12)
#define BIT13                           (1ul << 13)
#define BIT14                           (1ul << 14)
#define BIT15                           (1ul << 15)
#define BIT16                           (1ul << 16)
#define BIT17                           (1ul << 17)
#define BIT18                           (1ul << 18)
#define BIT19                           (1ul << 19)
#define BIT20                           (1ul << 20)
#define BIT21                           (1ul << 21)
#define BIT22                           (1ul << 22)
#define BIT23                           (1ul << 23)
#define BIT24                           (1ul << 24)
#define BIT25                           (1ul << 25)
#define BIT26                           (1ul << 26)
#define BIT27                           (1ul << 27)
#define BIT28                           (1ul << 28)
#define BIT29                           (1ul << 29)
#define BIT30                           (1ul << 30)
#define BIT31                           (1ul << 31)
#endif

#endif  /* _RDA_DEF_H */

