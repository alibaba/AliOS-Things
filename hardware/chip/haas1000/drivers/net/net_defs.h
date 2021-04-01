#ifndef _NET_DEFS_H_
#define _NET_DEFS_H_

#define __LINUX_ERRNO_EXTENSIONS__

#include "plat_types.h"
#include "cmsis.h"
#include <string.h>

#ifndef __ASSEMBLY__

//#define NEED_KEEP

#ifndef USHRT_MAX
#define USHRT_MAX	((u16)(~0U))
#endif

#ifndef SHRT_MAX
#define SHRT_MAX	((s16)(USHRT_MAX>>1))
#endif

#ifndef SHRT_MIN
#define SHRT_MIN	((s16)(-SHRT_MAX - 1))
#endif

#ifndef INT_MAX
#define INT_MAX		((int)(~0U>>1))
#endif

#ifndef INT_MIN
#define INT_MIN		(-INT_MAX - 1)
#endif

#ifndef UINT_MAX
#define UINT_MAX	(~0U)
#endif

#ifndef LONG_MAX
#define LONG_MAX	((long)(~0UL>>1))
#endif

#ifndef LONG_MIN
#define LONG_MIN	(-LONG_MAX - 1)
#endif

#ifndef ULONG_MAX
#define ULONG_MAX	(~0UL)
#endif

#ifndef LLONG_MIN
#define LLONG_MIN	(-LLONG_MAX - 1)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX	(~0ULL)
#endif

#define N_FALSE 0
#define N_TRUE (!N_FALSE)

#define BIT(nr)			(1UL << (nr))

#define	MAX_SCHEDULE_TIMEOUT	LONG_MAX

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif /* min */

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif /* max */

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

#ifndef NULL
#define NULL            ((void *) 0)
#endif

#ifndef __PLAT_TYPES_H__
typedef  char __s8;
typedef unsigned char __u8;

typedef  short __s16;
typedef unsigned short __u16;

typedef  int __s32;
typedef unsigned int __u32;

typedef  long long __s64;
#endif
typedef unsigned long long __u64;

#ifndef __PLAT_TYPES_H__
typedef unsigned char UCHAR;
typedef signed char CHAR;
typedef unsigned char* PUCHAR;
typedef signed char* PCHAR;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

typedef  char  INT8;
typedef  short INT16;
typedef  int   INT32;

typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;
typedef unsigned int u32_t;
typedef signed int s32_t;
#endif
/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

#ifndef __PLAT_TYPES_H__
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
#endif
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

/*
 * aligned_u64 should be used in defining kernel<->userspace ABIs to avoid
 * common 32/64-bit compat problems.
 * 64-bit values align to 4-byte boundaries on x86_32 (and possibly other
 * architectures) and to 8-byte boundaries on 64-bit architectures.  The new
 * aligned_64 type enforces 8-byte alignment so that structs containing
 * aligned_64 values have the same alignment on 32-bit and 64-bit architectures.
 * No conversions are necessary between 32-bit user-space and a 64-bit kernel.
 */
#define __aligned_16   __attribute__((aligned(2)))
#define __aligned_u16 __u16 __attribute__((aligned(2)))
#define __aligned_u32 __u32 __attribute__((aligned(4)))
#define __aligned_u64 __u64 __attribute__((aligned(8)))
#define __aligned_be64 __be64 __attribute__((aligned(8)))
#define __aligned_le64 __le64 __attribute__((aligned(8)))

#define __init
#define __must_check
#ifndef __pure
#define __pure
#endif
#define might_sleep()
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#define likely(x) (x)
#define unlikely(x) (x)

#define PTR_ERR(x) ((int)x)
#define IS_ERR(x) (((int)x > 0)?N_FALSE:N_TRUE)

typedef struct {
	int counter;
} atomic32_t;

typedef struct {
	uint64_t counter;
} atomic64_t;

typedef enum {
	GFP_KERNEL = 0,
	GFP_ATOMIC,
	__GFP_HIGHMEM,
	__GFP_HIGH
} gfp_t;

#define GFP_DMA GFP_KERNEL

#define barrier
#define EXPORT_SYMBOL(x)

#define smp_mb(x)

#define net_swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(char *)(&((type *)0)->member)))

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
/**
 * struct rcu_head - callback structure for use with RCU
 * @next: next update requests in a list
 * @func: actual update function to call after the grace period.
 */
struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *head);
};

static inline void * __must_check ERR_PTR(long error)
{
	return (void *) error;
}

#define BITS_PER_LONG 32

/* Defined for the NFSv3 protocol */
#define EBADHANDLE	521	/* Illegal NFS file handle */
#define ENOTSYNC	522	/* Update synchronization mismatch */
#define EBADCOOKIE	523	/* Cookie is stale */
#define ENOTSUPP	524	/* Operation is not supported */
#define ETOOSMALL	525	/* Buffer or request is too small */
#define ESERVERFAULT	526	/* An untranslatable error occurred */
#define EBADTYPE	527	/* Type not supported by server */
#define EJUKEBOX	528	/* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED	529	/* iocb queued, will get completion event */

typedef struct
{
	unsigned int tm_year; 
	unsigned int tm_mon;
	unsigned int tm_mday;
	unsigned int tm_hour;
	unsigned int tm_min;
	unsigned int tm_sec;
        bool tm_isdst;
} tm_t;

#endif /*  __ASSEMBLY__ */
#endif /* _NET_DEFS_H_ */

