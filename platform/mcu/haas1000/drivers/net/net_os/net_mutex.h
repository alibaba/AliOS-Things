#ifndef __NET_MUTEX_H__
#define __NET_MUTEX_H__
#include "net_defs.h"
#include "cmsis_os.h"
#include "net_debug.h"

struct net_mutex{
    osMutexId mutex_id;
#if (osCMSIS >= 0x20000U)
    osMutexAttr_t mutex_attr;
#else
    uint32_t  os_mutex_cb[3];
#endif
};

typedef struct net_mutex spinlock_t;

#define net_mutex_lock(lock) \
{\
    osStatus ret; \
    NET_ASSERT((lock)->mutex_id);\
    ret = osMutexWait((lock)->mutex_id, osWaitForever);\
	if (ret) { \
        printf("%s,%d,mutex lock fail.ret,%d\n", __func__, __LINE__, (int)ret); \
    } \
}

static __INLINE int net_mutex_trlock(struct net_mutex *lock)
{
    int ret = 0;
    NET_ASSERT(lock->mutex_id);
    ret = osMutexWait(lock->mutex_id, 0);
    return ret?0:1;
}

#define net_mutex_unlock(lock) \
{ \
    osStatus ret; \
    NET_ASSERT((lock)->mutex_id); \
    ret = osMutexRelease((lock)->mutex_id); \
    if (ret) { \
        printf("%s,%d,mutex unlock fail.ret,%d\n", __func__, __LINE__, (int)ret); \
    } \
}

static __INLINE void mutex_destroy(struct net_mutex *lock)
{
    NET_ASSERT(lock->mutex_id);
    osMutexDelete(lock->mutex_id);
    lock->mutex_id = NULL;
}

static __INLINE unsigned int spin_lock_irqsave(spinlock_t *lock, unsigned long * cpu_sr)
{
    cpu_sr = cpu_sr;
    lock = lock;
    NET_ASSERT(lock->mutex_id);
    osMutexWait(lock->mutex_id, osWaitForever);
    return 0;
}

static __INLINE void spin_unlock_irqrestore(spinlock_t *lock, unsigned long cpu_sr)
{
    cpu_sr = cpu_sr;
    osMutexRelease(lock->mutex_id);
}
osMutexId osMutexCreateEx (osMutexDef_t *mutex_def,uint32 caller) ;

static __INLINE void spin_lock_init(spinlock_t* lock)
{
    if (lock->mutex_id != NULL)
        osMutexDelete(lock->mutex_id);

#if (osCMSIS >= 0x20000U)
    memset(&lock->mutex_attr, 0, sizeof(osMutexAttr_t));
#ifdef FREERTOS_SUPPORT
    lock->mutex_attr.attr_bits = osMutexRecursive | osMutexPrioInherit;
#else /* FREERTOS_SUPPORT */
    lock->mutex_attr.attr_bits = osMutexRecursive | osMutexPrioInherit | osMutexRobust;
#endif /* FREERTOS_SUPPORT */

    lock->mutex_id = osMutexNew(&lock->mutex_attr);
#else
    osMutexDef_t osmutexdef;
    memset(&osmutexdef, 0, sizeof(osmutexdef));
#ifdef osCMSIS_RTX
    osmutexdef.mutex = lock->os_mutex_cb;
    memset(lock->os_mutex_cb, 0, sizeof(lock->os_mutex_cb));
#endif
    lock->mutex_id = osMutexCreate(&osmutexdef);
#endif

    NET_ASSERT(lock->mutex_id);
}

static __INLINE void spin_lock_destroy(spinlock_t* lock)
{
    NET_ASSERT(lock->mutex_id);
    osMutexDelete(lock->mutex_id);
    lock->mutex_id = NULL;
}

#define spin_lock(lock) \
{\
    osStatus ret; \
    NET_ASSERT((lock)->mutex_id);\
    ret = osMutexWait((lock)->mutex_id, osWaitForever);\
	if (ret) { \
        printf("%s,%d,mutex lock fail.ret,%d\n", __func__, __LINE__, (int)ret); \
    } \
}

#define spin_unlock(lock) \
{ \
    osStatus ret; \
    NET_ASSERT((lock)->mutex_id); \
    ret = osMutexRelease((lock)->mutex_id); \
    if (ret) { \
        printf("%s,%d,mutex unlock fail.ret,%d\n", __func__, __LINE__, (int)ret); \
    } \
}
#define spin_lock_bh(lock) spin_lock(lock)
#define spin_unlock_bh(lock) spin_unlock(lock)

extern void local_bh_enable(void);
extern void local_bh_disable(void);
extern void net_mutex_init_def(struct net_mutex* mutex);
#define  net_mutex_init(mutex){\
						net_mutex_init_def((mutex));\
						NET_ASSERT((mutex)->mutex_id);\
}
						 //printf("%s,%d,.mutex_id=0x%x\n", __func__, __LINE__, (uint32)(mutex)->mutex_id);
				//}

extern int net_mutex_trlock(struct net_mutex *lock);
#endif


