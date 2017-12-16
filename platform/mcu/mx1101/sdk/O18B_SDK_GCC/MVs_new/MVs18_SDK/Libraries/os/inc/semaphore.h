/*
 * linux/include/asm-arm/semaphore.h
 * maintainer: Halley
 * Changelog:
 *	2012-03-20	borrow from linux open source tree & modify by huangyucai
 *	2013-12-09	add the wait queue initialization dynamicly by huangyucai
 *	2014-02-08	add the semaphore deadlock check machanism by huangyucai
 *	2014-02-17	add the semaphore recursive operation for compatible with strandard freertos 
 *				semaphore operation by huangyucai
 */
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//#include <linux/linkage.h>
//#include <linux/spinlock.h>
//#include <linux/wait.h>
//#include <linux/rwsem.h>

//#include <asm/atomic.h>
//#include <asm/locks.h>
#include "type.h"
#include "list.h"
#include "locks.h"

/*
 * define wait queue head & wait queue struct
 */
typedef struct _wait_queue_head_t
{
	xList head;
	spinlock_t lock;
} wait_queue_head_t, WAIT_QUEUE_HEAD;

typedef struct _wait_queue_t
{
	xListItem item;
} wait_queue_t, WAIT_QUEUE;

typedef struct semaphore
{
	atomic_t count;
	int sleepers;
	wait_queue_head_t wait;
	wait_queue_head_t timeo;
	char name[16];
	struct list_head list;
	char recursive;
}SEMAPHORE;

static void init_waitqueue_head(wait_queue_head_t* wait);

/*
 * Macros for declaration and initialisaton of the datatypes
 */
#define __WAITQUEUE_INITIALIZER(name, task)		\
{												\
	/*.item = */								\
	{											\
		/*.xItemValue = (unsigned long)task */0UL,\
		/*.pxNext = */ NULL,					\
		/*.pxPrevious = */ NULL,				\
		/*.pvOwner = */ NULL,					\
		/*.pvContainer = */ NULL,				\
	},\
}

#define DECLARE_WAITQUEUE(name, task)	\
	wait_queue_t name = __WAITQUEUE_INITIALIZER(name, task);\
	listSET_LIST_ITEM_OWNER(&name.item, (void*)(task));

#define __WAIT_QUEUE_HEAD_INITIALIZER(wait) 	\
{												\
	/*.head = */ XLIST_INITIALIZER((wait).head),\
	/*.lock = */ __SPIN_LOCK_UNLOCKED((wait).lock),\
}

#define __SEMAPHORE_INIT(name, cnt, recursv)		\
{	\
	/*.count = */ATOMIC_INIT(cnt),	\
	/*.sleepers = */0,		\
	/*.wait	= */__WAIT_QUEUE_HEAD_INITIALIZER((name).wait),\
	/*.timeo = */__WAIT_QUEUE_HEAD_INITIALIZER((name).timeo),\
	/*.name = */#name,\
	/*.list = */LIST_HEAD_INIT((name).list), \
	/*.recursive = */recursv, \
}

/*
 * static semaphore/mutex declaration
 */
#define DECLARE_SEMAPHORE(name, count)	\
	struct semaphore name = __SEMAPHORE_INIT(name, count, 0)

#define DECLARE_SEMAPHORE_RECURSIVE(name, count)	\
	struct semaphore name = __SEMAPHORE_INIT(name, count, 1)

#define DECLARE_MUTEX(name)		DECLARE_SEMAPHORE(name, 1)
#define DECLARE_MUTEX_RECURSIVE(name)		DECLARE_SEMAPHORE_RECURSIVE(name, 1)

#define DECLARE_MUTEX_LOCKED(name)	DECLARE_SEMAPHORE(name, 0)
#define DECLARE_MUTEX_RECURSIVE_LOCKED(name)	DECLARE_SEMAPHORE_RECURSIVE(name, 0)

/*
 * dynamic semaphore/mutex declaration
 */
#define SEMA_INIT(sem, val) \
{\
	memset(&(sem), 0, sizeof(struct semaphore)); \
	_atomic_set(&(sem).count, val); \
	(sem).sleepers = 0; \
	init_waitqueue_head(&(sem).wait); \
	init_waitqueue_head(&(sem).timeo); \
	mvsnprintf((sem).name, sizeof((sem).name), "0x%08X",(unsigned int)&(sem)); \
	INIT_LIST_HEAD(&(sem).list);	\
}

#define SEMA_INIT_RECURSIVE(sem, val) \
{ \
	SEMA_INIT(sem, val);\
	sem.recursive = 1; \
}

#define INIT_MUTEX(sem)						SEMA_INIT(sem, 1)
#define INIT_MUTEX_RECURSIVE(sem)			SEMA_INIT_RECURSIVE(sem, 1)

#define INIT_MUTEX_LOCKED(sem)				SEMA_INIT(sem, 0)
#define INIT_MUTEX_RECURSIVE_LOCKED(sem)	SEMA_INIT_RECURSIVE(sem, 0)

/*
 * special register calling convention
 */
void __down_failed(void);
int  __down_interruptible_failed(void);
int  __down_trylock_failed(void);
void __up_wakeup(void);

//extern __asm
void __down_op(void* ptr, void* fail);
int __down_op_ret_1(void* ptr, void* fail, unsigned int timeo);
int __down_op_ret_2(void* ptr, void* fail, unsigned int timeo);
void __up_op(void* ptr, void* wake);

/*
 * This is ugly, but we want the default case to fall through.
 * "__down" is the actual routine that waits...
 */
#define	__OSDOWN(sema)	(__down_op((void*)sema, (void*)__down_failed), (int)0)
/*
 * This is ugly, but we want the default case to fall through.
 * "__down_interruptible" is the actual routine that waits...
 */
#define	__OSDOWN_TRYLOCK(sema)	__down_op_ret_1((void*)sema, (void*)__down_trylock_failed, 0)
#define	__OSDOWN_TIMEOUT(sema, timeo)	__down_op_ret_2((void*)sema, (void*)__down_interruptible_failed, timeo)
/*
 * Note! This is subtle. We jump to wake people up only if
 * the semaphore was negative (== somebody was waiting on it).
 * The default case (no contention) will result in NO
 * jumps for both down() and up().
 */
#define	__OSUP(sema)	__up_op((void*)sema, (void*)__up_wakeup)

static void init_waitqueue_head(wait_queue_head_t* wait)
{
	xList* list = &wait->head;
	spinlock_t* lock = &wait->lock; 

	lock->raw_lock.lock = 0 /*__RAW_SPIN_LOCK_UNLOCKED*/;
	//vListInitialise(list);
	//for compiling reason,substitude the above API with following statements
	list->pxIndex = (xListItem*)&(list->xListEnd);
	list->xListEnd.xItemValue = (unsigned long) - 1;
	list->xListEnd.pxNext = (xListItem*)&(list->xListEnd);
	list->xListEnd.pxPrevious = (xListItem*)&(list->xListEnd);
	list->uxNumberOfItems = (unsigned long)0U;
}

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__SEMAPHORE_H

