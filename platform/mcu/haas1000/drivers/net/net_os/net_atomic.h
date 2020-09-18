#ifndef _NET_ATOMIC_H__
#define _NET_ATOMIC_H__
#include "net_defs.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "net_debug.h"
#include "hal_chipid.h"

static __INLINE void atomic_set(atomic32_t *at, int val)
{
    uint32_t ret;
    do {
        __LDREXW((volatile uint32_t *)&at->counter);
    } while ((ret = __STREXW(val, (volatile uint32_t *)&at->counter)));
}

static __INLINE int atomic_read(volatile atomic32_t *v)
{
    return (*(volatile int *)&(v)->counter);
}

static  __INLINE void atomic_add(int i, volatile atomic32_t *v)
{
    uint32_t ret;
    int result;

busy:
    result = __LDREXW((volatile uint32_t *)&v->counter);
    result += i;
    ret = __STREXW(result, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
}

static __INLINE void atomic_sub(int i, volatile atomic32_t *v)
{
    uint32_t ret;
    int result;

busy:
    result = __LDREXW((volatile uint32_t *)&v->counter);
    result -= i;
    ret = __STREXW(result, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
}

static __INLINE int atomic_add_return(int i, volatile atomic32_t *v)
{
    uint32_t ret;
    int result;

busy:
    result = __LDREXW((volatile uint32_t *)&v->counter);
    result += i;
    ret = __STREXW(result, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
    return result;
}

static __INLINE int  atomic_sub_return(int i, atomic32_t *v)
{
    uint32_t ret;
    int result;

busy:
    result = __LDREXW((volatile uint32_t *)&v->counter);
    result -= i;
    ret = __STREXW(result, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
    return result;
}

#define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)

#define atomic64_read(v)	(*(volatile uint64_t *)&(v)->counter)
static inline void atomic64_set(atomic64_t *v, uint64_t i)
{
    uint32_t ret;

busy:
    __LDREXW((volatile uint32_t *)&v->counter);
    ret = __STREXW((uint32_t)i, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
    __LDREXW((volatile uint32_t *)&v->counter + 1);
    ret = __STREXW((uint32_t)(i >> 32), (volatile uint32_t *)&v->counter + 1);
    if(ret != 0)
        goto busy;
}

static inline long atomic64_add_return(long i, atomic64_t *v)
{
    uint64_t tmp;

    tmp = atomic64_read(v);
    tmp += i;
    atomic64_set(v, tmp);
    return tmp;
}

static inline int atomic_xchg(volatile atomic32_t *v, int i)
{
    uint32_t ret;
    int result;

busy:
    result = __LDREXW((volatile uint32_t *)&v->counter);
    ret = __STREXW(i, (volatile uint32_t *)&v->counter);
    if(ret != 0)
        goto busy;
    return result;
}

extern osMutexId g_rcu_lock_mutex_id;

#ifdef NEED_KEEP
static __INLINE void rcu_read_lock(void)
{
    osMutexWait(g_rcu_lock_mutex_id, osWaitForever);
}

static __INLINE void rcu_read_unlock(void)
{
	osMutexRelease(g_rcu_lock_mutex_id);
}
#else
#define rcu_read_lock()
#define rcu_read_unlock();
#endif

#define rtnl_lock()
#define  rtnl_unlock()

extern void sys_atomic_init(void);
extern void sys_atomic_uinit(void);
#endif

