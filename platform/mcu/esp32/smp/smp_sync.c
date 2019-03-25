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
#include "k_api.h"

extern volatile uint64_t g_cpu_flag;
extern int printf(char *fmt, ...);


void os_wait_allcore(void)
{
    uint8_t loop = 1;

    while (loop) {
        //ets_printf( "os_wait_allcoreApp cpu up.,g_cpu_flag:%d,RHINO_CONFIG_CPU_NUM:%d\r\n",g_cpu_flag,RHINO_CONFIG_CPU_NUM);
        krhino_task_sleep(10);

    switch (RHINO_CONFIG_CPU_NUM) {
        case 2:
            if (g_cpu_flag == 3u) {
                loop = 0;
            }
            break;
        case 3:
            if (g_cpu_flag == 7u) {
                loop = 0;
            }
            break;
        case 4:
            if (g_cpu_flag == 15u) {
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



void os_unlock_sys_spin(void)
{
    //ets_printf("os_unlock_sys_spin,core:%d\r\n",cpu_cur_get());
    krhino_spin_unlock(&g_sys_lock);
}


