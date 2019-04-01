#ifndef _SOC_TYPES_H_
#define _SOC_TYPES_H_

#include <stdlib.h>
#include <stdint.h>

typedef int8_t          __s8;
typedef uint8_t         __u8;
typedef int16_t         __s16;
typedef uint16_t        __u16;
typedef int32_t         __s32;
typedef uint32_t        __u32;
typedef uint64_t        __u64;
typedef int64_t         __s64;

typedef int8_t          s8;
typedef uint8_t         u8;
typedef int16_t         s16;
typedef uint16_t        u16;
typedef int32_t         s32;
typedef uint32_t        u32;
typedef uint64_t        u64;
typedef int64_t         s64;
typedef char            c8;
typedef char            char_t;

typedef int_fast8_t     sfast8;
typedef int_least8_t    sleast8;
typedef int_fast16_t    sfast16;
typedef int_least16_t   sleast16;
typedef int_fast32_t    sfast32;
typedef int_least32_t   sleast32;
typedef uint_fast8_t    ufast8;
typedef uint_least8_t   uleast8;
typedef uint_fast16_t   ufast16;
typedef uint_least16_t  uleast16;
typedef uint_fast32_t   ufast32;
typedef uint_least32_t  uleast32;

typedef int8_t          s8_t;
typedef uint8_t         u8_t;
typedef int16_t         s16_t;
typedef uint16_t        u16_t;
typedef int32_t         s32_t;
typedef uint32_t        u32_t;

#ifndef bool
#define bool            u8
#endif

typedef volatile u32                SSV6XXX_REG;

typedef u16 __le16;
typedef u32 __le32;
typedef u64 __le64;

typedef u16 le16;
typedef u32 le32;
typedef u64 le64;

typedef u16 __be16;
typedef u32 __be32;
typedef u64 __be64;

typedef u16 be16;
typedef u32 be32;
typedef u64 be64;

#ifndef NULL
#define NULL                        ((void *)0)
#endif

#ifndef true
#define true                        1
#endif

#ifndef false
#define false                       0
#endif

#ifndef TRUE
#define TRUE                        1
#endif

#ifndef FALSE
#define FALSE                       0
#endif

#define _LE16(x)                        ((u16)(x))
#define _BE16(x)                        ((u16) ((((x)&0xFF) << 8) | (((x)&0xFF00)>>8)))

#define IS_EQUAL(a, b)                  ( (a) == (b) )
#define SET_BIT(v, b)					( (v) |= (0x01<<b) )
#define CLEAR_BIT(v, b)			    	( (v) &= ~(0x01<<b) )
#define IS_BIT_SET(v, b)				( (v) & (0x01<<(b) ) )
#define ABS(x)                          ((x < 0) ? (-x) : (x))

typedef struct ssv_cabrio_reg_st {
    u32 address;
    u32 data;
} ssv_cabrio_reg;

typedef union uip_ipaddr_t {
  uint8_t  u8[4];			/* Initializer, must come first. */
  uint16_t u16[2];
} uip_ipaddr_t;

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__));
#else
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__; __pragma( pack(pop) )
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define EMPTY
#if !defined(ETH_ALEN)
#define ETH_ALEN 6
#endif

#endif /* end of include guard: TYPES_H */
