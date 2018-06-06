/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>

#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "xtensa_rtos.h"
#include "smp_sync.h"
#include "k_api.h"

extern volatile uint64_t cpu_flag;
extern int printf(char *fmt, ...);


void os_wait_allcore(void)
{
    uint8_t loop = 1;

    while (loop) {
        //ets_printf( "os_wait_allcoreApp cpu up.,cpu_flag:%d,RHINO_CONFIG_CPU_NUM:%d\r\n",cpu_flag,RHINO_CONFIG_CPU_NUM);
        
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


void cpu_spin_lock_unnest(kspinlock_t *lock)
{
    unsigned long tmp;

    __asm__ __volatile__(
            "       movi    %0, 0\n"
            "       wsr     %0, scompare1\n"
            "1:     movi    %0, 1\n"
            "       s32c1i  %0, %1, 0\n"
            "       bnez    %0, 1b\n"
            : "=&a" (tmp)
            : "a" (&lock->owner)
            : "memory");

}


void cpu_spin_unlock_unnest(kspinlock_t *lock)
{
    unsigned long tmp;
    
    __asm__ __volatile__(
            "       movi    %0, 0\n"
            "       s32ri   %0, %1, 0\n"
            : "=&a" (tmp)
            : "a" (&lock->owner)
            : "memory");

}


#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
void cpu_spin_lock(kspinlock_t *lock, const char *fnName, int32_t line)
#else
void cpu_spin_lock(kspinlock_t *lock)
#endif
{
    uint32_t res;
    uint32_t recCnt;
    unsigned int irqStatus;
    uint32_t cnt=(1<<16);
    if ( (lock->owner & KRHINO_SPINLOCK_MAGIC_MASK) != KRHINO_SPINLOCK_MAGIC_VAL ) {
        //ets_printf("ERROR: vPortCPUAcquireMutex: mux %p is uninitialized (0x%X)! Called from %s line %d.\n", lock, lock->owner, fnName, line);
        lock->owner=KRHINO_SPINLOCK_FREE_VAL;
    }

    do {
        //Lock mux if it's currently unlocked
        res=(cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT)|KRHINO_SPINLOCK_MAGIC_VAL;
        osPortCompareSet(&lock->owner, KRHINO_SPINLOCK_FREE_VAL, &res);
        //If it wasn't free and we're the owner of the lock, we are locking recursively.
        if ( (res != KRHINO_SPINLOCK_FREE_VAL) && (((res&KRHINO_SPINLOCK_VAL_MASK)>>KRHINO_SPINLOCK_VAL_SHIFT) == cpu_cur_get()) ) {
            //Mux was already locked by us. Just bump the recurse count by one.
            recCnt=(res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT;
            recCnt++;
            
            #ifdef RHINO_CONFIG_SPINLOCK_DEBUG
            //ets_printf("Recursive lock: recCnt=%d last non-recursive lock %s line %d, curr %s line %d\n", recCnt, lock->last_lockfile, lock->last_lockline, fnName, line);
            #endif
            lock->owner=KRHINO_SPINLOCK_MAGIC_VAL|(recCnt<<KRHINO_SPINLOCK_CNT_SHIFT)|(cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT);
            break;
        }
        cnt--;
        if (cnt==0) {
            #ifdef RHINO_CONFIG_SPINLOCK_DEBUG    
            printf("Timeout on mux! last non-recursive lock %s line %d, curr %s line %d\n", lock->last_lockfile, lock->last_lockline, fnName, line);
            #endif
            printf("lock value %X,xPortGetCoreID():%d\r\n", lock->owner,cpu_cur_get());
        }
    } while (res!=KRHINO_SPINLOCK_FREE_VAL);
#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
    if (res==KRHINO_SPINLOCK_FREE_VAL) { //initial lock
        lock->last_lockfile=fnName;
        lock->last_lockline=line;
    }
#endif

}


#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
int32_t cpu_spin_unlock(kspinlock_t *lock, const char *fnName, int32_t line)
#else
int32_t cpu_spin_unlock(kspinlock_t *lock)
#endif
{
    uint32_t res=0;
    uint32_t recCnt;
    unsigned int irqStatus;
    int32_t ret=0;
#ifdef RHINO_CONFIG_SPINLOCK_DEBUG
        const char *lastLockedFn=lock->last_lockfile;
        int lastLockedLine=lock->last_lockline;
        lock->last_lockfile=fnName;
        lock->last_lockline=line;
#endif
    
    if ( (lock->owner & KRHINO_SPINLOCK_MAGIC_MASK) != KRHINO_SPINLOCK_MAGIC_VAL ) printf("ERROR: vPortCPUReleaseMutex: mux %p is uninitialized (0x%X)!\n", lock, lock->owner);
    //Unlock mux if it's currently locked with a recurse count of 0
    res=KRHINO_SPINLOCK_FREE_VAL;
    osPortCompareSet(&lock->owner, (cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT)|KRHINO_SPINLOCK_MAGIC_VAL, &res);

    if ( ((res&KRHINO_SPINLOCK_VAL_MASK)>>KRHINO_SPINLOCK_VAL_SHIFT) == cpu_cur_get() ) {
        //Lock is valid, we can return safely. Just need to check if it's a recursive lock; if so we need to decrease the refcount.
         if ( ((res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT)!=0) {
            //We locked this, but the reccount isn't zero. Decrease refcount and continue.
            recCnt=(res&KRHINO_SPINLOCK_CNT_MASK)>>KRHINO_SPINLOCK_CNT_SHIFT;
            recCnt--;
            
            #ifdef RHINO_CONFIG_SPINLOCK_DEBUG
			//ets_printf("Recursive unlock: recCnt=%d last locked %s line %d, curr %s line %d\n", recCnt, lastLockedFn, lastLockedLine, fnName, line);
            #endif            
            lock->owner=KRHINO_SPINLOCK_MAGIC_VAL|(recCnt<<KRHINO_SPINLOCK_CNT_SHIFT)|(cpu_cur_get()<<KRHINO_SPINLOCK_VAL_SHIFT);
        }
        else
        {
			//ets_printf("Recursive unlock: finish last locked %s line %d, curr %s line %d\n",lastLockedFn, lastLockedLine, fnName, line);
        }
    } else if ( res == KRHINO_SPINLOCK_FREE_VAL ) {
        printf("ERROR: cpu_spin_unlock: lock %p was already unlocked!\n", lock);
        ret=-1;
    } else {
        printf("ERROR: vPortCPUReleaseMutex: mux %p wasn't locked by this core (%d) but by core %d (ret=%x, mux=%x).\n", lock, cpu_cur_get(), ((res&KRHINO_SPINLOCK_VAL_MASK)>>KRHINO_SPINLOCK_VAL_SHIFT), res, lock->owner);
        ret=-1;
    }
    return ret;

}


void unlock_spin(void)
{
    //ets_printf("unlock_spin,core:%d\r\n",cpu_cur_get());
    krhino_spin_unlock(&g_sys_lock);
}


