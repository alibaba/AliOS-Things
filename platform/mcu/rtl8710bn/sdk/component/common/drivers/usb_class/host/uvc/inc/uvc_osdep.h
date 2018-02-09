#ifndef _UVC_OSDEP_WRAP_H_
#define _UVC_OSDEP_WRAP_H_

//#include "rtl_utility.h"
#include "platform/platform_stdlib.h"
#include "basic_types.h"
#include "osdep_api.h"
#include "usb_defs.h"

#include "errno.h"
#include "dlist.h"



#define UVC_LAYER_DEBUG  0
#if     UVC_LAYER_DEBUG
#define UVC_PRINTF(fmt, args...)	printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define UVC_ERROR(fmt, args...)		printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define UVC_WARN(fmt, args...)		printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define FUN_ENTER                      	//printf("\n\r%s ==>\n", __func__)
#define FUN_EXIT                        //printf("\n\r%s <==\n", __func__)
#define FUN_TRACE                       //printf("\n\r%s:%d \n", __func__, __LINE__)
#else
#define UVC_PRINTF(fmt, args...)
#define UVC_ERROR(fmt, args...)		printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define UVC_WARN(fmt, args...)		printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define FUN_ENTER
#define FUN_EXIT
#define FUN_TRACE
#endif

/* add by Ian -- define uvc task priority */
#define UVC_TASK_PRIORITY       2

#ifndef __u8
#define __u8 	u8
#endif
#ifndef __u16
#define __u16	u16
#endif
#ifndef __u32
#define __u32	u32
#endif
#ifndef __u64
#define __u64	u64
#endif
#ifndef __s8
#define __s8	s8
#endif
#ifndef __s16
#define __s16	s16
#endif
#ifndef __s32
#define __s32	s32
#endif
#ifndef __s64
#define __s64	s64
#endif

#ifndef gfp_t
#define gfp_t	u32
#endif

#define ALIGN(x, a, type_of_x)	(((x) + ((type_of_x)(a) - 1)) & ~((type_of_x)(a) - 1))

#ifndef IS_ALIGNED
#define IS_ALIGNED(x, a, type_of_x)		(((x) & ((type_of_x)(a) - 1)) == 0)
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif 

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (32)
#endif
#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG (32)
#endif

/* Atomic integer operations */
#ifndef atomic_set
	#define atomic_set(v, i)		RTL_ATOMIC_SET((v), (i))
#endif
#ifndef atomic_read
	#define atomic_read(v)			RTL_ATOMIC_READ((v))
#endif
#ifndef atomic_add
	#define atomic_add(v, i)		RTL_ATOMIC_ADD((v), (i))
#endif
#ifndef atomic_sub
	#define atomic_sub(v, i)		RTL_ATOMIC_SUB((v), (i))
#endif
#ifndef atomic_inc
	#define atomic_inc(v)			RTL_ATOMIC_INC((v))
#endif
#ifndef atomic_dec
	#define atomic_dec(v)			RTL_ATOMIC_DEC((v))
#endif         

#ifndef MEDIA_PAD_FL_SINK
#define MEDIA_PAD_FL_SINK               (1 << 0)
#endif
#ifndef MEDIA_PAD_FL_SOURCE
#define MEDIA_PAD_FL_SOURCE             (1 << 1)
#endif
#ifndef MEDIA_PAD_FL_MUST_CONNECT
#define MEDIA_PAD_FL_MUST_CONNECT       (1 << 2)
#endif

static inline u16 __get_unaligned_le16(const u8 *p)
{
         return p[0] | p[1] << 8;
}

static inline u32 __get_unaligned_le32(const u8 *p)
{
         return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}
 
static inline u64 __get_unaligned_le64(const u8 *p)
{
         return (u64)__get_unaligned_le32(p + 4) << 32 |
                __get_unaligned_le32(p);
}
 
static inline void __put_unaligned_le16(u16 val, u8 *p)
{
         *p++ = val;
         *p++ = val >> 8;
}
 
static inline void __put_unaligned_le32(u32 val, u8 *p)
{
         __put_unaligned_le16(val >> 16, p + 2);
         __put_unaligned_le16(val, p);
}
 
static inline void __put_unaligned_le64(u64 val, u8 *p)
{
         __put_unaligned_le32(val >> 32, p + 4);
         __put_unaligned_le32(val, p);
}
 
static inline u16 get_unaligned_le16(const void *p)
{
         return __get_unaligned_le16((const u8 *)p);
}
 
static inline u32 get_unaligned_le32(const void *p)
{
         return __get_unaligned_le32((const u8 *)p);
}
 
static inline u64 get_unaligned_le64(const void *p)
{
         return __get_unaligned_le64((const u8 *)p);
}
 
static inline void put_unaligned_le16(u16 val, void *p)
{
         __put_unaligned_le16(val, p);
}
 
static inline void put_unaligned_le32(u32 val, void *p)
{
         __put_unaligned_le32(val, p);
}
 
static inline void put_unaligned_le64(u64 val, void *p)
{
         __put_unaligned_le64(val, p);
}
 
/**
* kmemdup - duplicate region of memory
*
* @src: memory region to duplicate
* @len: memory region length
* @gfp: GFP mask to use
*/
static inline void *kmemdup(const void *src, size_t len, gfp_t gfp)
{
         void *p;
 
         //p = kmalloc_track_caller(len, gfp);
	 //p = kmalloc(len, gfp);
         p = rtw_malloc(len);
	 if (p)
                 memcpy(p, src, len);
         return p;
}
#ifndef __force
#define __force        __attribute__((force))
#endif

#if 0
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64; 
typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;
#endif
//edit by Ian -- remove duplicated definitions
#if 0
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64; 
typedef __u16 __sum16;
typedef __u32 __wsum;
#endif

#ifndef __le16
#define __le16 __u16
#endif
#ifndef __be16
#define __be16 __u16
#endif
#ifndef __le32
#define __le32 __u32
#endif
#ifndef __be32
#define __be32 __u32
#endif
static inline __u32 le32_to_cpup(const __le32 *p)
{
        //return (__force __u32)*p;
	return (__u32)*p;
}
static inline __u16 le16_to_cpup(const __le16 *p)
{
        //return (__force __u16)*p;
	return (__u16)*p;
}


/* Endian macros */
#ifndef htonl
#define htonl(x)	rtk_cpu_to_be32(x)
#endif
          
#ifndef ntohl
#define ntohl(x)	rtk_be32_to_cpu(x)
#endif

#ifndef htons
#define htons(x)	rtk_cpu_to_be16(x)
#endif

#ifndef ntohs
#define ntohs(x)	rtk_be16_to_cpu(x)
#endif

#ifndef cpu_to_le32
#define cpu_to_le32(x)	rtk_cpu_to_le32(x)
#endif

#ifndef le32_to_cpu
#define le32_to_cpu(x)	rtk_le32_to_cpu(x)
#endif

#ifndef cpu_to_le16
#define cpu_to_le16(x)	rtk_cpu_to_le16(x)
#endif

#ifndef le16_to_cpu
#define le16_to_cpu(x)	rtk_le16_to_cpu(x)
#endif

#ifndef cpu_to_be32
#define cpu_to_be32(x)	rtk_cpu_to_be32(x)
#endif
  
#ifndef be32_to_cpu
#define be32_to_cpu(x)	rtk_be32_to_cpu(x)
#endif
        
#ifndef cpu_to_be16
#define cpu_to_be16(x)	rtk_cpu_to_be16(x)
#endif

#ifndef be16_to_cpu
#define be16_to_cpu(x)	rtk_be16_to_cpu(x)
#endif

/* Parameters used to convert the timespec values: */
#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC    1000L
#endif
#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC   1000L
#endif
#ifndef NSEC_PER_USEC
#define NSEC_PER_USEC   1000L
#endif
#ifndef NSEC_PER_MSEC
#define NSEC_PER_MSEC   1000000L
#endif
#ifndef USEC_PER_SEC
#define USEC_PER_SEC    1000000L
#endif
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC    1000000000L
#endif
#ifndef FSEC_PER_SEC
#define FSEC_PER_SEC    1000000000000000LL
#endif
#ifndef TIME_T_MAX
#define TIME_T_MAX      (time_t)((1UL << ((sizeof(time_t) << 3) - 1)) - 1)
#endif


#ifndef __GFP_WAIT
#define __GFP_WAIT             					(0x10u)
#endif
#ifndef __GFP_HIGH
#define __GFP_HIGH             					(0x20u)
#endif
#ifndef __GFP_IO
#define __GFP_IO               					(0x40u)
#endif
#ifndef __GFP_FS
#define __GFP_FS               					(0x80u)
#endif
#ifndef GFP_NOIO
#define GFP_NOIO 						(0x10u)
#endif
#ifndef __GFP_NOWARN
#define __GFP_NOWARN          					(0x200u)
#endif
#ifndef GFP_KERNEL
#define GFP_KERNEL	(__GFP_WAIT | __GFP_IO | __GFP_FS)
#endif

#if 0
#ifndef copy_from_user
#define copy_from_user(to, from, sz) rtw_memcpy((to), (from), (sz))
#endif
#ifndef copy_to_user
#define copy_to_user(to, from, sz)   rtw_memcpy((to), (from), (sz))
#endif
#endif

int inline copy_from_user(void* to, void* from, u32 sz)
{
        memcpy(to, from, sz);
        return TRUE;
}

int inline copy_to_user(void* to, void* from, u32 sz)
{
        memcpy(to, from, sz);
        return TRUE;
}

typedef u32             compat_caddr_t; //used for compatibility in uvc_v4l2.c

/**
* strlcpy - Copy a %NUL terminated string into a sized buffer
* @dest: Where to copy the string to
* @src: Where to copy the string from
* @size: size of destination buffer
*
* Compatible with *BSD: the result is always a valid
* NUL-terminated string that fits in the buffer (unless,
* of course, the buffer size is zero). It does not pad
* out the result like strncpy() does.
*/
static inline size_t strlcpy(char *dest, const char *src, size_t size)
{
         size_t ret = _strlen(src);
 
         if (size) {
                 size_t len = (ret >= size) ? size - 1 : ret;
                 memcpy(dest, src, len);
                 dest[len] = '\0';
         }
         return ret;
}

/**
* clamp - return a value clamped to a given range with strict typechecking
* @val: current value
* @min: minimum allowable value
* @max: maximum allowable value
*
* This macro does strict typechecking of min/max to make sure they are of the
* same type as val.  See the unnecessary pointer comparisons.
*/

#ifndef clamp
#define clamp(new_val, val, min, max, type) do{           \
         type __val = (val);              	\
         type __min = (min);              	\
         type __max = (max);              	\
         (void) (&__val == &__min);             \
         (void) (&__val == &__max);             \
         __val = (__val < __min) ? __min: __val;  \
         new_val = (__val > __max) ? __max: __val; }while(0)
#endif

/*
 * Compile time versions of __arch_hweightN()
 */
#ifndef __const_hweight8
#define __const_hweight8(w)             \
       ( (!!((w) & (1ULL << 0))) +       \
         (!!((w) & (1ULL << 1))) +       \
         (!!((w) & (1ULL << 2))) +       \
         (!!((w) & (1ULL << 3))) +       \
         (!!((w) & (1ULL << 4))) +       \
         (!!((w) & (1ULL << 5))) +       \
         (!!((w) & (1ULL << 6))) +       \
         (!!((w) & (1ULL << 7))) )
#endif
#ifndef hweight8
#define hweight8(w)  __const_hweight8(w)
#endif
#ifndef BITMAP_LAST_WORD_MASK
#define BITMAP_LAST_WORD_MASK(nbits)                                    \
(                                                                       \
         ((nbits) % BITS_PER_LONG) ?                                     \
                 (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL               \
)
#endif
/**
  * hweightN - returns the hamming weight of a N-bit word
  * @x: the word to weigh
  *
  * The Hamming Weight of a number is the total number of bits set in it.
  */ 
static inline unsigned int hweight32(unsigned int w)
{
         unsigned int res = w - ((w >> 1) & 0x55555555);
         res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
         res = (res + (res >> 4)) & 0x0F0F0F0F;
         res = res + (res >> 8);
         return (res + (res >> 16)) & 0x000000FF;
}
 
static inline unsigned long hweight64(__u64 w)
{
#if BITS_PER_LONG == 32
         return hweight32((unsigned int)(w >> 32)) + hweight32((unsigned int)w);
#elif BITS_PER_LONG == 64
         __u64 res = w - ((w >> 1) & 0x5555555555555555ul);
         res = (res & 0x3333333333333333ul) + ((res >> 2) & 0x3333333333333333ul);
         res = (res + (res >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
         res = res + (res >> 8);
         res = res + (res >> 16);
         return (res + (res >> 32)) & 0x00000000000000FFul;
#endif
}


static inline unsigned long hweight_long(unsigned long w)
{
         return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

static inline int __bitmap_weight(const unsigned long *bitmap, int bits)
{
         int k, w = 0, lim = bits/BITS_PER_LONG;
 
         for (k = 0; k < lim; k++)
                 w += hweight_long(bitmap[k]);
 
         if (bits % BITS_PER_LONG)
                 w += hweight_long(bitmap[k] & BITMAP_LAST_WORD_MASK(bits));
 
         return w;
}

static inline int bitmap_weight(const unsigned long *src, int nbits)
{
 //        if (small_const_nbits(nbits))
 //               return hweight_long(*src & BITMAP_LAST_WORD_MASK(nbits));
         return __bitmap_weight(src, nbits);
}

/**
  * memweight - count the total number of bits set in memory area
  * @ptr: pointer to the start of the area
  * @bytes: the size of the area
  */
static inline size_t memweight(const void *ptr, size_t bytes)
{
         size_t ret = 0;
         size_t longs;
         const unsigned char *bitmap = ptr;
  
          for (; bytes > 0 && ((unsigned long)bitmap) % sizeof(long);
                          bytes--, bitmap++)
                  ret += hweight8(*bitmap);
  
          longs = bytes / sizeof(long);
          if (longs) {
                  //BUG_ON(longs >= INT_MAX / BITS_PER_LONG);
                  ret += bitmap_weight((unsigned long *)bitmap, longs * BITS_PER_LONG);
                  bytes -= longs * sizeof(long);
                  bitmap += longs * sizeof(long);
          }
          /*
           * The reason that this last loop is distinct from the preceding
           * bitmap_weight() call is to compute 1-bits in the last region smaller
           * than sizeof(long) properly on big-endian systems.
           */
          for (; bytes > 0; bytes--, bitmap++)
                  ret += hweight8(*bitmap);
  
          return ret;
}

 /**
  * strlcat - Append a length-limited, %NUL-terminated string to another
  * @dest: The string to be appended to
* @src: The string to append to it
* @count: The size of the destination buffer.
*/
static inline size_t strlcat(char *dest, const char *src, size_t count)
{
         size_t dsize = _strlen(dest);
         size_t len = _strlen(src);
         size_t res = dsize + len;
 
         /* This would be a bug */
         //BUG_ON(dsize >= count);
 
         dest += dsize;
         count -= dsize;
         if (len >= count)
                 len = count-1;
         memcpy(dest, src, len);
         dest[len] = 0;
         return res;
}


/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic_dec_and_test(atomic_t *v)
{
	atomic_dec(v);
	if (v->counter == 0)
		return TRUE;
	else
		return FALSE;
}

/**
 * kcalloc - allocate memory for an array. The memory is set to zero.
 * @n: number of elements.
 * @size: element size.
 * @flags: the type of memory to allocate (see kmalloc).
 */
static inline void *kcalloc(size_t n, size_t size, gfp_t flags)
{
         //return RtlZmalloc(((n) * (size)));
          return rtw_zmalloc(((n) * (size)));
}

#ifndef GFP_ATOMIC
#define GFP_ATOMIC	GFP_KERNEL
#endif
#ifndef offsetof
#define offsetof(s,m) (size_t)&(((s *)0)->m)
#endif

//enum linux kernel version
#ifndef   KERNEL_VERSION
#define   KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#ifndef LINUX_VERSION_CODE
#define LINUX_VERSION_CODE KERNEL_VERSION(3, 12, 0)
#endif

#endif //_UVC_OSDEP_WRAP_H_