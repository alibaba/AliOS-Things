#ifndef _GENERIC_H_
#define _GENERIC_H_
#include <stdbool.h>
#include "include.h"

typedef void (*FUNCPTR)(void);
typedef void (*FUNC_1PARAM_PTR)(void *ctxt);
typedef void (*FUNC_2PARAM_PTR)(void *arg, uint8_t vif_idx);

#define MAX(x, y)                  (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                  (((x) < (y)) ? (x) : (y))
#define max(x, y)                  (((x) > (y)) ? (x) : (y))
#define min(x, y)                  (((x) < (y)) ? (x) : (y))

extern void bk_printf(const char *fmt, ...);
#define as_printf (bk_printf("%s:%d\r\n",__FUNCTION__,__LINE__))


#if (0 == CFG_RELEASE_FIRMWARE)
#define ASSERT_EQ(a, b)                             \
{                                                   \
    if ((a) != (b))                                 \
    {                                               \
        bk_printf("%s:%d %d!=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_NE(a, b)                             \
{                                                   \
    if ((a) == (b))                                 \
    {                                               \
        bk_printf("%s:%d %d==%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_GT(a, b)                             \
{                                                   \
    if ((a) <= (b))                                 \
    {                                               \
        bk_printf("%s:%d %d<=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_GE(a, b)                             \
{                                                   \
    if ((a) < (b))                                 \
    {                                               \
        bk_printf("%s:%d %d<%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_LT(a, b)                             \
{                                                   \
    if ((a) >= (b))                                 \
    {                                               \
        bk_printf("%s:%d %d>=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_LE(a, b)                             \
{                                                   \
    if ((a) > (b))                                 \
    {                                               \
        bk_printf("%s:%d %d>%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT(exp)                                 \
{                                                   \
    if ( !(exp) )                                   \
    {                                               \
    	as_printf;							     	\
        while(1);                                   \
    }                                               \
} 
#else
#define ASSERT_EQ(exp)
#define ASSERT_NE(exp)
#define ASSERT_GT(exp)
#define ASSERT_GE(exp)
#define ASSERT_LT(exp)
#define ASSERT_LE(exp)
#define ASSERT(exp)
#endif

#define BUG_ON(exp)               ASSERT(!(exp))

#ifndef NULL
#define NULL                     (0L)
#endif

#ifndef NULLPTR
#define NULLPTR                  ((void *)0)
#endif

#define BIT(i)                   (1UL << (i))

static inline __uint16_t __bswap16(__uint16_t _x)
{

	return ((__uint16_t)((_x >> 8) | ((_x << 8) & 0xff00)));
}

static inline __uint32_t __bswap32(__uint32_t _x)
{

	return ((__uint32_t)((_x >> 24) | ((_x >> 8) & 0xff00) |
	    ((_x << 8) & 0xff0000) | ((_x << 24) & 0xff000000)));
}

static inline __uint64_t __bswap64(__uint64_t _x)
{

	return ((__uint64_t)((_x >> 56) | ((_x >> 40) & 0xff00) |
	    ((_x >> 24) & 0xff0000) | ((_x >> 8) & 0xff000000) |
	    ((_x << 8) & ((__uint64_t)0xff << 32)) |
	    ((_x << 24) & ((__uint64_t)0xff << 40)) |
	    ((_x << 40) & ((__uint64_t)0xff << 48)) | ((_x << 56))));
}

#define __swab16(x) __bswap16((__u8 *)&(x))
#define __swab32(x) __bswap32((__u8 *)&(x))

#define cpu_to_le16(x)   (x)
#define cpu_to_le32(x)   (x)

#define __cpu_to_be32(x) __swab32((x))
#define __be32_to_cpu(x) __swab32((x))
#define __cpu_to_be16(x) __swab16((x))
#define __be16_to_cpu(x) __swab16((x))


#define	__htonl(_x)	__bswap32(_x)
#define	__htons(_x)	__bswap16(_x)
#define	__ntohl(_x)	__bswap32(_x)
#define	__ntohs(_x)	__bswap16(_x)

#define ___htonl(x) __cpu_to_be32(x)
#define ___htons(x) __cpu_to_be16(x)
#define ___ntohl(x) __be32_to_cpu(x)
#define ___ntohs(x) __be16_to_cpu(x)

#if (!CFG_SUPPORT_RTT)
#define htons(x) __htons(x)
#define ntohs(x) __ntohs(x)
#define htonl(x) __htonl(x)
#define ntohl(x) __ntohl(x)
#endif

#endif // _GENERIC_H_

