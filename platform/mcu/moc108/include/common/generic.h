#ifndef _GENERIC_H_
#define _GENERIC_H_

typedef void (*FUNCPTR)(void);
typedef void (*FUNC_1PARAM_PTR)(void *arg);
typedef void (*FUNC_2PARAM_PTR)(void *ctxt, void *private);

#define MAX(x, y)                  (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                  (((x) < (y)) ? (x) : (y))
#define max(x, y)                  (((x) > (y)) ? (x) : (y))
#define min(x, y)                  (((x) < (y)) ? (x) : (y))

#if (0 == CFG_RELEASE_FIRMWARE)
#define ASSERT(exp)                                 \
{                                                   \
    if ( !(exp) )                                   \
    {                                               \
        while(1);                                   \
    }                                               \
} 
#else
#define ASSERT(exp)
#endif

#define BUG_ON(exp)              ASSERT(!(exp))

#ifndef FALSE
#define FALSE                    (0)
#define false                    (0)
#endif

#ifndef TRUE
#define TRUE                     (1)
#define true                     (1)
#endif

#ifndef NULL
#define NULL                     (0L)
#endif

#ifndef NULLPTR
#define NULLPTR                  ((void *)0)
#endif

#define BIT(i)                   (1UL << (i))


static __u16  ___swab16(__u8 *x)
{
	return (__u16)(
		(((__u16)x[0]) << 8) |
		(((__u16)x[1]) << 0));

}

static __u32  ___swab32(__u8 *x)
{
	return (__u32)(
		(((__u32)(x[0])) << 24) |
		(((__u32)(x[1])) << 16) |
		(((__u32)(x[2])) << 8)  |
		(((__u32)(x[3])) << 0));
}

#define __swab16(x) ___swab16((__u8 *)&(x))
#define __swab32(x) ___swab32((__u8 *)&(x))

#define cpu_to_le16(x)   (x)
#define cpu_to_le32(x)   (x)

#define __cpu_to_be32(x) __swab32((x))
#define __be32_to_cpu(x) __swab32((x))
#define __cpu_to_be16(x) __swab16((x))
#define __be16_to_cpu(x) __swab16((x))

#define ___htonl(x) __cpu_to_be32(x)
#define ___htons(x) __cpu_to_be16(x)
#define ___ntohl(x) __be32_to_cpu(x)
#define ___ntohs(x) __be16_to_cpu(x)

#define htonl(x) ___htonl(x)
#define ntohl(x) ___ntohl(x)
#define htons(x) ___htons(x)
#define ntohs(x) ___ntohs(x)

#endif // _GENERIC_H_
