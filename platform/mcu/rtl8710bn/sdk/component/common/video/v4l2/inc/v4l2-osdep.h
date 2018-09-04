#ifndef _V4L2_OSDEP_H_
#define _V4L2_OSDEP_H_

#include "platform/platform_stdlib.h"
#include "basic_types.h"
//#include "osdep_api.h"  //deprecated use osdep_service.h instead
#include "osdep_service.h"
#include "usb_defs.h"
#include "usb_errno.h"
//#include "hal_util.h"
#include "dlist.h"

#define V4L2_LAYER_DEBUG 0
#if     V4L2_LAYER_DEBUG
#define V4L2_PRINTF(fmt, args...)	printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define V4L2_ERROR(fmt, args...)        printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define V4L2_PRINTF(fmt, args...)
#define V4L2_ERROR(fmt, args...)        printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#endif

/* misc items */
#ifndef ssize_t
#define ssize_t SSIZE_T
#endif
#ifndef size_t
#define size_t SIZE_T
#endif

#ifndef __user
#define __user
#endif

#ifndef loff_t
#define loff_t long
#endif
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

#ifndef spinlock_t
#define spinlock_t _lock
#endif

#ifndef gfp_t
#define gfp_t	u32
#endif

#ifndef _atomic_spin_lock_irqsave
#define _atomic_spin_lock_irqsave(p, flags) 		save_and_cli()
#endif
#ifndef _atomic_spin_unlock_irqrestore
#define _atomic_spin_unlock_irqrestore(p, flags)	restore_flags()
#endif
#ifndef local_irq_save
#define local_irq_save(flags)				save_and_cli()
#endif
#ifndef local_irq_restore
#define local_irq_restore(flags)			restore_flags()
#endif
#ifndef cris_atomic_save
#define cris_atomic_save(addr, flags) 			local_irq_save(flags)
#endif
#ifndef cris_atomic_restore
#define cris_atomic_restore(addr, flags) 		local_irq_restore(flags)
#endif

/*
 * abs() handles unsigned and signed longs, ints, shorts and chars.  For all
 * input types abs() returns a signed long.
 * abs() should not be used for 64-bit types (s64, u64, long long) - use abs64()
 * for those.
 */

#ifndef abs
#define abs(x) ((x >= 0) ? (x) : (x * -1))
#endif

#ifndef min 
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef min_t 
#define min_t(type, x, y) ({                    \
         type __min1 = (x);                      \
         type __min2 = (y);                      \
         (__min1 < __min2) ? (__min1) : (__min2); })
#endif
#ifndef max_t
#define max_t(type, x, y) ({                    \
         type __max1 = (x);                      \
         type __max2 = (y);                      \
         (__max1 > __max2) ? (__max1) : (__max2); })
#endif
#ifndef max_tt
#define max_tt(ret,type,x,y) do{                    \
         type __max1 = (x);                      \
         type __max2 = (y);                      \
         ret = (__max1 > __max2) ? (__max1) : (__max2); }while(0) 
#endif
#ifndef min_tt 
#define min_tt(ret,type, x, y) do{                    \
         type __min1 = (x);                      \
         type __min2 = (y);                      \
         ret = (__min1 < __min2) ? (__min1) : (__min2); }while(0)
#endif 


/**
* container_of - cast a member of a structure out to the containing structure
* @p(ptr): 	   the pointer to the member.
* @t(type):	   the type of the container struct this is embedded in.
* @m(member):	   the name of the member within the struct.
*
*/
#ifndef container_of 
	#define container_of(ptr, type, member) \
			((type *)((char *)(ptr)-(SIZE_T)(&((type *)0)->member)))
#endif
	
	/**
	* list_next_entry - get the next element in list
	* @pos: 	   the type * to cursor
	* @member:	   the name of the list_struct within the struct.
	*/

#define list_next_entry(pos, member, type)				\
			 list_entry((pos)->member.next, type, member)

	/**
	* list_for_each_entry_continue - continue iteration over list of given type
	* @pos: 	   the type * to use as a loop cursor.
	* @head:	   the head for your list.
	* @member:	   the name of the list_struct within the struct.
	*
	* Continue to iterate over list of given type, continuing after
	* the current position.
	*/

#define list_for_each_entry_continue(pos, head, member, type)		\
		for (pos = list_next_entry(pos, member, type);			\
			&pos->member != (head); 					\
				pos = list_next_entry(pos, member, type))           
           

/* spin lock */
#ifndef spin_lock_init
	#define spin_lock_init(plock) 			rtw_spinlock_init((plock))
#endif
#ifndef spin_lock_free
	#define spin_lock_free(plock) 			rtw_spinlock_free((plock))
#endif
#ifndef spin_lock
	#define spin_lock(plock) 			rtw_spin_lock((plock))
#endif
#ifndef spin_unlock 
	#define spin_unlock(plock) 			rtw_spin_unlock((plock))
#endif


/* mutex */
#ifndef Mutex
	#define Mutex _mutex
#endif
#ifndef Mutex_Init
	#define Mutex_Init(pmutex)			rtw_mutex_init((pmutex))
#endif
#ifndef Mutex_Lock
	#define Mutex_Lock(pmutex)			rtw_mutex_get((pmutex))
#endif
#ifndef Mutex_Unlock
	#define Mutex_Unlock(pmutex)		rtw_mutex_put((pmutex))	
#endif
#ifndef Mutex_Free
	#define Mutex_Free(pmutex)			rtw_mutex_free((pmutex))
#endif

/* semaphore */
#ifndef Sema
        #define Sema _sema
#endif
#ifndef Sema_Init
	#define Sema_Init(pmutex, pval)			rtw_init_sema(pmutex, pval)
#endif
#ifndef Sema_Free
	#define Sema_Free(pmutex)				rtw_free_sema(pmutex)
#endif
#ifndef Sema_Up
	#define Sema_Up(pmutex)				rtw_up_sema(pmutex)
#endif
#ifndef Sema_Down
	#define Sema_Down(pmutex)				rtw_down_sema(pmutex)
#endif
#ifndef Sema_Down_WithTimeout
	#define Sema_Down_WithTimeout(pmutex,ptimeout)			rtw_down_timeout_sema(pmutex,ptimeout) //
#endif

/* Atomic integer operations */
#ifndef atomic_set
	#define atomic_set(v, i)		ATOMIC_SET((v), (i))
#endif
#ifndef atomic_read
	#define atomic_read(v)			ATOMIC_READ((v))
#endif
#ifndef atomic_add
	#define atomic_add(v, i)		ATOMIC_ADD((v), (i))
#endif
#ifndef atomic_sub
	#define atomic_sub(v, i)		ATOMIC_SUB((v), (i))
#endif
#ifndef atomic_inc
	#define atomic_inc(v)			ATOMIC_INC((v))
#endif
#ifndef atomic_dec
	#define atomic_dec(v)			ATOMIC_DEC((v))
#endif

           
// IOCTL ...
#ifndef _IOC_NRBITS
#define _IOC_NRBITS 	(8) 	
#endif
#ifndef _IOC_TYPEBITS
#define _IOC_TYPEBITS 	(8) 	
#endif
#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS 	(14) 	
#endif
#ifndef _IOC_DIRBITS
#define _IOC_DIRBITS 	(2) 	
#endif
#ifndef _IOC_NRMASK
#define _IOC_NRMASK 	((1 << _IOC_NRBITS) - 1) 
#endif
#ifndef _IOC_TYPEMASK
#define _IOC_TYPEMASK 	((1 << _IOC_TYPEBITS) - 1)
#endif
#ifndef _IOC_SIZEMASK
#define _IOC_SIZEMASK 	((1 << _IOC_SIZEBITS) - 1)
#endif
#ifndef _IOC_DIRMASK
#define _IOC_DIRMASK 	((1 << _IOC_DIRBITS) - 1) 
#endif
#ifndef _IOC_NRSHIFT 
#define _IOC_NRSHIFT 	(0) 
#endif
#ifndef _IOC_TYPESHIFT
#define _IOC_TYPESHIFT 	(_IOC_NRSHIFT + _IOC_NRBITS) 
#endif
#ifndef _IOC_SIZESHIFT
#define _IOC_SIZESHIFT 	(_IOC_TYPESHIFT + _IOC_TYPEBITS) 
#endif
#ifndef _IOC_DIRSHIFT
#define _IOC_DIRSHIFT 	(_IOC_SIZESHIFT + _IOC_SIZEBITS) 
#endif 

/*
 * Direction bits.
 */
#ifndef _IOC_NONE
#define _IOC_NONE (0U) 
#endif
#ifndef _IOC_WRITE
#define _IOC_WRITE (1U) 
#endif
#ifndef _IOC_READ
#define _IOC_READ (2U) 
#endif 


/*
 * combine the four dir，type，nr，size parameters to one cmd parameter
 *
 */
#ifndef _IOC
#define _IOC(dir,type,nr,size) \
       (((dir) << _IOC_DIRSHIFT) | \
        ((type) << _IOC_TYPESHIFT) | \
        ((nr) << _IOC_NRSHIFT) | \
        ((size) << _IOC_SIZESHIFT))
#endif
/*
 * used to create IOCTL cmd 
 */
#ifndef _IO
#define _IO(type,nr)        	_IOC(_IOC_NONE,(type), (nr), 0) 
#endif
#ifndef _IOR  
#define _IOR(type,nr,size)     	_IOC(_IOC_READ,(type), (nr), sizeof(size)) 
#endif
#ifndef _IOW
#define _IOW(type,nr,size)    	_IOC(_IOC_WRITE,(type), (nr), sizeof(size))
#endif
#ifndef _IOWR
#define _IOWR(type,nr,size) 	_IOC(_IOC_READ|_IOC_WRITE, (type), (nr), sizeof(size))
#endif
/* 
 * used to decode ioctl infoations..
 */
#ifndef _IOC_DIR
#define _IOC_DIR(nr) (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#endif
#ifndef _IOC_TYPE
#define _IOC_TYPE(nr) (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#endif
#ifndef _IOC_NR
#define _IOC_NR(nr) (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#endif
#ifndef _IOC_SIZE
#define _IOC_SIZE(nr) (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)
#endif 
/* ...and for the drivers/sound files... */
#ifndef IOC_IN
#define IOC_IN (_IOC_WRITE << _IOC_DIRSHIFT)
#endif
#ifndef IOC_OUT
#define IOC_OUT (_IOC_READ << _IOC_DIRSHIFT)
#endif
#ifndef IOC_INOUT
#define IOC_INOUT ((_IOC_WRITE|_IOC_READ) << _IOC_DIRSHIFT)
#endif
#ifndef IOCSIZE_MASK
#define IOCSIZE_MASK (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#endif
#ifndef IOCSIZE_SHIFT
#define IOCSIZE_SHIFT (_IOC_SIZESHIFT)
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif
#ifndef BITS_PER_LONG
#define BITS_PER_LONG (32)
#endif
#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG (32)
#endif
#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif
#ifndef BIT_ULL
#define BIT_ULL(nr)             (1ULL << (nr))
#endif
#ifndef BIT_MASK
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
#endif
#ifndef BIT_WORD
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#endif
#ifndef BIT_ULL_MASK
#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#endif
#ifndef BIT_ULL_WORD
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)
#endif
#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE           (8)
#endif
#ifndef BITS_TO_LONGS
#define BITS_TO_LONGS(nr)       DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#endif

/* __ffs to find out the first exist 1 offset, from bit 0 ~ 31 */
#ifndef ffz
#define ffz(x)  __ffs(~(x))                     
#endif
/* __builtin_constant_p is gcc build-in function to check the parameter is const or not */
#ifndef small_const_nbits
#define small_const_nbits(nbits) \
         (__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG)
#endif
/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static inline unsigned long __ffs(unsigned long word)
{
	int num = 0;

#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {                         
		num += 16;                                      
		word >>= 16;
	}
	if ((word & 0xff) == 0) {                           
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	
	return num;                                         
}

/**
  * __fls - find last (most-significant) set bit in a long word
  * @word: the word to search
  *
  * Undefined if no set bit exists, so code should check against 0 first.
  */
static inline unsigned long __fls(unsigned long word)
{
         int num = BITS_PER_LONG - 1;
 
 #if BITS_PER_LONG == 64
         if (!(word & (~0ul << 32))) {
                 num -= 32;
                 word <<= 32;
         }
 #endif
         if (!(word & (~0ul << (BITS_PER_LONG-16)))) {
                 num -= 16;
                 word <<= 16;
         }
         if (!(word & (~0ul << (BITS_PER_LONG-8)))) {
                 num -= 8;
                 word <<= 8;
         }
         if (!(word & (~0ul << (BITS_PER_LONG-4)))) {
                 num -= 4;
                 word <<= 4;
         }
         if (!(word & (~0ul << (BITS_PER_LONG-2)))) {
                 num -= 2;
                 word <<= 2;
         }
         if (!(word & (~0ul << (BITS_PER_LONG-1))))
                num -= 1;
        return num;
}

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int test_bit(int nr, const volatile unsigned long *addr)
{
        return ((1UL << (nr & 31)) & (addr[nr >> 5])) != 0;
}



/*
* Find the first cleared bit in a memory region.
*/
static inline unsigned long find_first_zero_bit(
	const unsigned long *addr, unsigned long size)
{
        const unsigned long *p = addr;
        unsigned long result = 0;
        unsigned long tmp;

        while (size & ~(BITS_PER_LONG-1)) {
                if (~(tmp = *(p++)))
                        goto found;
                result += BITS_PER_LONG;
                size -= BITS_PER_LONG;
        }
        if (!size)
                return result;

        tmp = (*p) | (~0UL << size);
        if (tmp == ~0UL)        /* Are any bits zero? */
                return result + size;   /* Nope. */
found:

	return result + ffz(tmp);
	//tmp = 0;
	//return result;

}

static inline unsigned long find_next_zero_bit(
	const unsigned long *addr,unsigned long size, unsigned long offset)
{
    const unsigned long *p = addr + BIT_WORD(offset); // offset位于p指向的long地址32位空?
    unsigned long result = offset & ~(BITS_PER_LONG-1); // offset是第result?4字?
    unsigned long tmp;

    if (offset >= size)
        return size;
    size -= result;                                     // ?整32位整倍?上
    offset %= BITS_PER_LONG;                            // offset位于32位的第几位
    if (offset) {                                       // offset不在一?long?据的第0位上,在1-31位中[luther.gliethttp]
        tmp = *(p++);
        tmp |= ~0UL >> (BITS_PER_LONG - offset);        // ?0-offset?据填充上1.
        if (size < BITS_PER_LONG)                       // 不足32bits
            goto found_first;
        if (~tmp)                                       // 取非非0?明含有0值
            goto found_middle;
        size -= BITS_PER_LONG;                          // 如果上面~tmp等于0,那么?明?*p?据?32位全1.[luther.gliethttp]
        result += BITS_PER_LONG;
    }
    while (size & ~(BITS_PER_LONG-1)) {                 // 好了,?行到?里,我?的offset已??在4字?的第0位上,下面?行
        if (~(tmp = *(p++)))                            // 4字?快速查?.如果~tmp非0,?明?32位?据中含有0?据,找到.[luther.gliethttp]
            goto found_middle;
        result += BITS_PER_LONG;                        // 到下一?4字?空?
        size -= BITS_PER_LONG;                          // ?少4字??据
    }
    if (!size)                                          // size等于0,?明首先size等于4字?整倍?,其次所有?据已?查完,
        return result;                                  // 所有?据全部?1,?有??0位,result等于size.[luther.gliethttp]
    tmp = *p;                                           // size不是32位整倍?,?剩几?bit?有?查,???行下面?查工作.[luther.gliethtp]

found_first:
    tmp |= ~0UL << size;                                // ?在0-size?有效?据,size-31?未使用空?,所以先?size-31置成全1.
    if (tmp == ~0UL)    /* Are any bits zero? */        // 如果tmp全1,那么?明就?找找1?
        return result + size;    /* Nope. */             // result+size就等于函??入的??size大小.[luther.gliethttp]
found_middle:
    return result + ffz(tmp);                           // 我?在32位?据的0-31中??必定存在0位值,?算他是第几位.[luther.gliethttp]
}

//int find_next_zero_bit(const void * p, int size, int offset);
//int find_first_bit(const unsigned long *p, unsigned size);
//int find_next_bit(const unsigned long *p, int size, int offset);
/**
* set_bit - Atomically set a bit in memory
* @nr: the bit to set
* @addr: the address to start counting from
*
* This function is atomic and may not be reordered.  See __set_bit()
* if you do not require the atomic guarantees.
*
* Note: there are no guarantees that this function will not be reordered
* on non x86 architectures, so if you are writing portable code,
* make sure not to rely on its reordering guarantees.
*
* Note that @nr may be almost arbitrarily large; this function is not
* restricted to acting on a single-word quantity.
*/
static inline void set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long flags;

	//taskENTER_CRITICAL();
	_atomic_spin_lock_irqsave(p, flags);
	*p  |= mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	//taskEXIT_CRITICAL();
}

/**
* clear_bit - Clears a bit in memory
* @nr: Bit to clear
* @addr: Address to start counting from
*
* clear_bit() is atomic and may not be reordered.  However, it does
* not contain a memory barrier, so if it is used for locking purposes,
* you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
* in order to ensure changes are visible on other processors.
*/
static inline void clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	//taskENTER_CRITICAL();
	*p &= ~mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	//taskEXIT_CRITICAL();
}
 
/**
* change_bit - Toggle a bit in memory
* @nr: Bit to change
* @addr: Address to start counting from
*
* change_bit() is atomic and may not be reordered. It may be
* reordered on other architectures than x86.
* Note that @nr may be almost arbitrarily large; this function is not
* restricted to acting on a single-word quantity.
*/
static inline void change_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long flags;

	//taskENTER_CRITICAL();
	_atomic_spin_lock_irqsave(p, flags);
	*p ^= mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	//taskEXIT_CRITICAL();
}
 
/**
* test_and_set_bit - Set a bit and return its old value
* @nr: Bit to set
* @addr: Address to count from
*
* This operation is atomic and cannot be reordered.
* It may be reordered on other architectures than x86.
* It also implies a memory barrier.
*/
static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old;
	//unsigned long flags;

	//taskENTER_CRITICAL();
	
	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old | mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	//taskEXIT_CRITICAL();

	return (old & mask) != 0;
}

/**
* test_and_clear_bit - Clear a bit and return its old value
* @nr: Bit to clear
* @addr: Address to count from
*
* This operation is atomic and cannot be reordered.
* It can be reorderdered on other architectures other than x86.
* It also implies a memory barrier.
*/
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old;
	unsigned long flags;

	//taskENTER_CRITICAL();
	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old & ~mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	//taskEXIT_CRITICAL();

	return (old & mask) != 0;
}
/**
* test_and_change_bit - Change a bit and return its old value
* @nr: Bit to change
* @addr: Address to count from
*
* This operation is atomic and cannot be reordered.
* It also implies a memory barrier.
*/
static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old;
	unsigned long flags;

	
	_atomic_spin_lock_irqsave(p, flags);
	old = *p;
	*p = old ^ mask;
	_atomic_spin_unlock_irqrestore(p, flags);
	

	return (old & mask) != 0;
}


static inline int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);
	unsigned long result = 0;

	for (k = 0; k < nr; k++)
	result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
	return result != 0;
}

static inline int bitmap_andnot(unsigned long *dst, const unsigned long *src1,
			const unsigned long *src2, int nbits)
{
//	if (small_const_nbits(nbits))
//		return (*dst = *src1 & ~(*src2)) != 0;
	return __bitmap_andnot(dst, src1, src2, nbits);
}

static inline int atomic_inc_return(volatile atomic_t *v)
{
         unsigned long flags;
         int retval;
         cris_atomic_save(v, flags);
         retval = ++(v->counter);
         cris_atomic_restore(v, flags);
         return retval;
}

typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64; 
typedef __u16 __sum16;
typedef __u32 __wsum;

#endif /*_V4L2_OSDEP_H_*/