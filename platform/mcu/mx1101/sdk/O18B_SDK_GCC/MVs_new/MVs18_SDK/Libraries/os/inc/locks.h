/*
 *  linux/include/asm-arm/locks.h
 *
 *  Copyright (C) 2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Interrupt safe locking assembler.
 * Changelog:
 *	2012-03-19	borrow from linux open source tree & modify for compatible with ARMCC by huangyucai
 *	2012-03-20	porting atomic operation from linux & modify for compatble with ARMCC by huangyucai
 */
//  maintainer: Halley

#ifndef 	__LOCKS_H__
#define 	__LOCKS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include	"type.h"

#define	SET_IRQ_MASK()	__set_BASEPRI(CFG_SYSCALL_IRQMAX_PRIO)
#define	CLR_IRQ_MASK()	__set_BASEPRI(!CFG_SYSCALL_IRQMAX_PRIO)

#define	SET_IRQ_MASK_FROMISR(x)	(x) = __get_BASEPRI(); SET_IRQ_MASK()
#define	CLR_IRQ_MASK_FROMISR(x)	__set_BASEPRI(x)


#define	SMP_MB	__DMB
/*
 * SPIN_LOCK_UNLOCKED and RW_LOCK_UNLOCKED defeat lockdep state tracking and
 * are hence deprecated.
 * Please use DEFINE_SPINLOCK()/DEFINE_RWLOCK() or
 * __SPIN_LOCK_UNLOCKED()/__RW_LOCK_UNLOCKED() as appropriate.
 */
#define __RAW_SPIN_LOCK_UNLOCKED    {0}
#define __RAW_RW_LOCK_UNLOCKED      {0}

# define __SPIN_LOCK_UNLOCKED(lockname) \
		    /*(spinlock_t)*/    {/*.raw_lock = */__RAW_SPIN_LOCK_UNLOCKED}
#define __RW_LOCK_UNLOCKED(lockname) \
		    /*(rwlock_t)*/  {/*.raw_lock = */__RAW_RW_LOCK_UNLOCKED}
#define ATOMIC_INIT(i)  {(i)}

#define DEFINE_SPINLOCK(x)  spinlock_t x = __SPIN_LOCK_UNLOCKED(x)
#define DEFINE_RWLOCK(x)    rwlock_t x = __RW_LOCK_UNLOCKED(x)

typedef struct
{
	volatile unsigned int lock;
} raw_spinlock_t;

typedef struct
{
	raw_spinlock_t raw_lock;
} spinlock_t, SPINLOCK;

typedef struct
{
	volatile unsigned int lock;
} raw_rwlock_t;

typedef struct
{
	raw_rwlock_t raw_lock;
} rwlock_t, RWLOCK;

typedef struct
{
	volatile int counter;
} atomic_t, ATOMIC;

/*
 * ATOMIC OPS
 */
//extern __asm void _atomic_set(atomic_t* v, int32_t i);

//extern __asm void _atomic_add(int i, atomic_t *v);

//extern __asm int32_t _atomic_add_return(int32_t i, atomic_t* v);

//extern __asm int32_t _atomic_sub_return(int32_t i, atomic_t* v);

//extern __asm void _atomic_clear_mask(uint32_t mask, uint32_t* addr);

#define __ATOMIC_SET(v, i)		_atomic_set(v, i);
#define __ATOMIC_ADD_RETURN(i, v)		_atomic_add_return(i, v);
#define __ATOMIC_SUB_RETURN(i, v)		_atomic_sub_return(i, v);
#define __ATOMIC_CLEAR_MASK(mask, addr)		_atomic_clear_mask(mask, addr);

#define	__lockfunc
/*
 * SPINLOCKS
 */
extern void __lockfunc _spin_lock(spinlock_t* lock);
extern int32_t __lockfunc _spin_trylock(spinlock_t* lock);
extern void __lockfunc _spin_unlock(spinlock_t* lock);
extern void __lockfunc _spin_lock_irq(spinlock_t* lock);
extern void __lockfunc _spin_unlock_irq(spinlock_t* lock);
extern uint32_t __lockfunc _spin_lock_irqsave(spinlock_t* lock);
extern void __lockfunc _spin_unlock_irqrestore(spinlock_t* lock, uint32_t flags);
#define	__SPIN_LOCK(lock)		_spin_lock(lock)
#define	__SPIN_TRYLOCK(lock)	_spin_trylock(lock)
#define	__SPIN_UNLOCK(lock)		_spin_unlock(lock)

#define	__SPIN_LOCK_IRQ(lock)	_spin_lock_irq(lock)
#define	__SPIN_UNLOCK_IRQ(lock)	_spin_unlock_irq(lock)

#define	__SPIN_LOCK_IRQSAVE(lock, flags)		(flags = _spin_lock_irqsave(lock))
#define	__SPIN_UNLOCK_IRQRESTORE(lock, flags)		_spin_unlock_irqrestore(lock, flags)

/*
 * RWLOCK
 */
extern void __lockfunc _read_lock(rwlock_t* lock);
extern int32_t __lockfunc _read_trylock(rwlock_t* lock);
extern void __lockfunc _read_unlock(rwlock_t* lock);
extern void __lockfunc _read_lock_irq(rwlock_t* lock);
extern void __lockfunc _read_unlock_irq(rwlock_t* lock);
extern void __lockfunc _write_lock(rwlock_t* lock);
extern int32_t __lockfunc _write_trylock(rwlock_t* lock);
extern void __lockfunc _write_unlock(rwlock_t* lock);
extern uint32_t __lockfunc _read_lock_irqsave(rwlock_t* lock);
extern void __lockfunc _read_unlock_irqrestore(rwlock_t* lock, uint32_t flags);
extern uint32_t __lockfunc _write_lock_irqsave(rwlock_t* lock);
extern void __lockfunc _write_unlock_irqrestore(rwlock_t* lock, uint32_t flags);
extern void __lockfunc _write_lock_irq(rwlock_t* lock);
extern void __lockfunc _write_unlock_irq(rwlock_t* lock);

#define	__READ_LOCK(lock)			_read_lock(lock)
#define	__READ_TRYLOCK(lock)		_read_trylock(lock)
#define	__READ_UNLOCK(lock)		_read_unlock(lock)


#define	__READ_LOCK_IRQ(lock)			_read_lock_irq(lock)
#define	__READ_UNLOCK_IRQ(lock)		_read_unlock_irq(lock)

#define	__WRITE_LOCK(lock)			_write_lock(lock)
#define	__WRITE_TRYLOCK(lock)			_write_trylock(lock)
#define	__WRITE_UNLOCK(lock)			_write_unlock(lock)

#define	__READ_LOCK_IRQSAVE(lock, flags)	(flags = _read_lock_irqsave(lock))
#define	__READ_UNLOCK_IRQRESTORE(lock, flags)	_read_unlock_irqrestore(lock, flags)

#define	__WRITE_LOCK_IRQSAVE(lock, flags)	(flags = _write_lock_irqsave(lock))
#define	__WRITE_UNLOCK_IRQRESTORE(lock, flags)		_write_unlock_irqrestore(lock, flags)

#define	__WRITE_LOCK_IRQ(lock)	_write_lock_irq(lock)
#define	__WRITE_UNLOCK_IRQ(lock)	_write_unlock_irq(lock)

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__LOCKS_H
