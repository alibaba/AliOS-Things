/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_spin_lock.h"

#if (RHINO_CONFIG_CPU_NUM > 1)

/*not use for linuxhost*/

extern int printf(const char *fmt, ...);

#define KRHINO_SPINLOCK_FREE_VAL    0xB33FFFFFu
#define KRHINO_SPINLOCK_MAGIC_VAL   0xB33F0000u
#define KRHINO_SPINLOCK_MAGIC_MASK  0xFFFF0000u
#define KRHINO_SPINLOCK_MAGIC_SHIFT 16
#define KRHINO_SPINLOCK_CNT_MASK    0x0000FF00u
#define KRHINO_SPINLOCK_CNT_SHIFT   8
#define KRHINO_SPINLOCK_VAL_MASK    0x000000FFu
#define KRHINO_SPINLOCK_VAL_SHIFT   0

#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
void k_cpu_spin_lock(kspinlock_t *lock, const char *fnName, int32_t line)
#else
void k_cpu_spin_lock(kspinlock_t *lock)
#endif
{
    uint32_t res;
    uint32_t recCnt;
    uint32_t cnt = (1<<16);

    if ((lock->owner & KRHINO_SPINLOCK_MAGIC_MASK) != KRHINO_SPINLOCK_MAGIC_VAL) {
        /*printf("ERROR: k_cpu_spin_lock: spinlock %p is uninitialized (0x%X)! Called from %s line %d.\n", lock, lock->owner, fnName, line);*/
        lock->owner = KRHINO_SPINLOCK_FREE_VAL;
    }

    do {
        /*Lock mux if it's currently unlocked*/
        res = (cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT)|KRHINO_SPINLOCK_MAGIC_VAL;
        osPortCompareSet(&lock->owner, KRHINO_SPINLOCK_FREE_VAL, &res);
        /*If it wasn't free and we're the owner of the lock, we are locking recursively.*/
        if ((res != KRHINO_SPINLOCK_FREE_VAL) && (((res&KRHINO_SPINLOCK_VAL_MASK)>>KRHINO_SPINLOCK_VAL_SHIFT) == cpu_cur_get())) {
            /*Mux was already locked by us. Just bump the recurse count by one.*/
            recCnt = (res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT;
            recCnt++;

            #ifdef RHINO_CONFIG_SPINLOCK_DEBUG
            /*printf("Recursive lock: recCnt=%d last non-recursive lock %s line %d, curr %s line %d\n", recCnt, lock->last_lockfile, lock->last_lockline, fnName, line);*/
            #endif
            lock->owner = KRHINO_SPINLOCK_MAGIC_VAL|(recCnt<<KRHINO_SPINLOCK_CNT_SHIFT)|(cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT);
            break;
        }

        cnt--;

        if (cnt == 0) {
            #ifdef RHINO_CONFIG_SPINLOCK_DEBUG
            printf("Timeout on mux! last non-recursive lock %s line %d, curr %s line %d\n", lock->last_lockfile, lock->last_lockline, fnName, line);
            #endif
            printf("lock value %X,cpu_cur_get():%d\r\n", lock->owner,cpu_cur_get());
        }
    } while (res != KRHINO_SPINLOCK_FREE_VAL);

#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
    if (res == KRHINO_SPINLOCK_FREE_VAL) {
        lock->last_lockfile = fnName;
        lock->last_lockline = line;
    }
#endif

}

#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
void k_cpu_spin_unlock(kspinlock_t *lock, const char *fnName, int32_t line)
#else
void k_cpu_spin_unlock(kspinlock_t *lock)
#endif
{
    uint32_t res = 0;
    uint32_t recCnt;
#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
    const char *lastLockedFn   = lock->last_lockfile;
    int         lastLockedLine = lock->last_lockline;
    lock->last_lockfile = fnName;
    lock->last_lockline = line;
#endif

    if ((lock->owner & KRHINO_SPINLOCK_MAGIC_MASK) != KRHINO_SPINLOCK_MAGIC_VAL) {
        printf("ERROR: k_cpu_spin_unlock: spinlock %p is uninitialized (0x%X)!\n", lock, lock->owner);
    }
    /* Unlock if it's currently locked with a recurse count of 0*/
    res = KRHINO_SPINLOCK_FREE_VAL;
    osPortCompareSet(&lock->owner, (cpu_cur_get() << KRHINO_SPINLOCK_VAL_SHIFT) | KRHINO_SPINLOCK_MAGIC_VAL, &res);

    if ( ((res&KRHINO_SPINLOCK_VAL_MASK) >> KRHINO_SPINLOCK_VAL_SHIFT) == cpu_cur_get() ) {
        /* Lock is valid, we can return safely. Just need to check if it's a recursive lock; if so we need to decrease the refcount.*/
         if ( ((res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT)!=0) {
            /* We locked this, but the reccount isn't zero. Decrease refcount and continue.*/
            recCnt = (res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT;
            recCnt--;

            lock->owner = KRHINO_SPINLOCK_MAGIC_VAL | (recCnt << KRHINO_SPINLOCK_CNT_SHIFT) | (cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT);
        }
    } else if ( res == KRHINO_SPINLOCK_FREE_VAL ) {
        printf("ERROR: cpu_spin_unlock: lock %p was already unlocked!\n", lock);
    } else {
        printf("ERROR: cpu_spin_unlock: lock %p wasn't locked by this core (%d) but by core %d (ret=%x, lock=%x).\n", lock, cpu_cur_get(), ((res&KRHINO_SPINLOCK_VAL_MASK)>>KRHINO_SPINLOCK_VAL_SHIFT), res, lock->owner);
    }
    return;

}

extern volatile uint64_t cpu_flag;

void k_wait_allcores(void)
{
    uint8_t loop = 1;

    while (loop) {
        switch (RHINO_CONFIG_CPU_NUM) {
            case 2:
                if (cpu_flag == 2u) {
                    loop = 0;
                }
                break;
            case 3:
                if (cpu_flag == 6u) {
                    loop = 0;
                }
                break;
            case 4:
                if (cpu_flag == 14u) {
                    loop = 0;
                }
                break;
            default:
                printf("too many cpus!!!\n");
                break;
        }
    }
}

#endif

