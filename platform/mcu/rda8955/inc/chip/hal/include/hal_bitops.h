
#ifndef _ASM_BITOPS_H
#define _ASM_BITOPS_H

#define SZLONG_LOG 5
#define SZLONG_MASK 31UL
#define __bi_flags
#define __bi_local_irq_save(x)
#define __bi_local_irq_restore(x)

/*
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void set_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	*a |= mask;
	__bi_local_irq_restore(flags);
}

/*
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static __inline__ void __set_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a |= mask;
}

/*
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static __inline__ void clear_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	*a &= ~mask;
	__bi_local_irq_restore(flags);
}

/*
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit() is atomic and may not be reordered.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void change_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	*a ^= mask;
	__bi_local_irq_restore(flags);
}

/*
 * __change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static __inline__ void __change_bit(int nr, volatile void * addr)
{
	unsigned long * m = ((unsigned long *) addr) + (nr >> 5);

	*m ^= 1UL << (nr & 31);
}

/*
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_set_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	int retval;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	retval = (mask & *a) != 0;
	*a |= mask;
	__bi_local_irq_restore(flags);

	return retval;
}

/*
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static __inline__ int __test_and_set_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	int retval;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a |= mask;

	return retval;
}

/*
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_clear_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	int retval;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	retval = (mask & *a) != 0;
	*a &= ~mask;
	__bi_local_irq_restore(flags);

	return retval;
}

/*
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static __inline__ int __test_and_clear_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	int retval;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a &= ~mask;

	return retval;
}

/*
 * test_and_change_bit - Change a bit and return its new value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static __inline__ int test_and_change_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask, retval;
	__bi_flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	__bi_local_irq_save(flags);
	retval = (mask & *a) != 0;
	*a ^= mask;
	__bi_local_irq_restore(flags);

	return retval;
}

/*
 * __test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static __inline__ int __test_and_change_bit(int nr, volatile void * addr)
{
	volatile unsigned long *a = addr;
	unsigned long mask;
	int retval;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a ^= mask;

	return retval;
}



/*
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int test_bit(int nr, volatile void *addr)
{
	return 1UL & (((const volatile unsigned long *) addr)[nr >> SZLONG_LOG] >> (nr & SZLONG_MASK));
}

#endif

