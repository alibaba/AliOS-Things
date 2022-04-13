/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include "aos/kernel.h"
#include "aos/init.h"
#include <k_api.h>
#if 0
#include <debug_api.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "aos/hal/wdg.h"
#include "aos/hal/gpio.h"
#include "mt_gic_v3.h"
#include "arch_timer.h"
#ifdef MAS_AGENT_SUPPORT
#include "mas_agent.h"
#endif
#include "mt_gpio.h"
#include "mt_i2c.h"
#include "hal_uart.h"

#ifdef CONFIG_NET_LWIP
#include "ethernetif.h"
#include "network/hal/wifi.h"
#endif

#if AOS_COMP_INPUTCORE
#include "drv_mouse.h"
#endif
#if AOS_COMP_FBDEV
#include "drv_clcd.h"
#endif

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern unsigned int __text_start[];
extern unsigned int _heap_end[];

extern void hw_start_hal(void);
extern void aos_maintask(void *arg);

void chip_tick_init(void)
{
    vConfigureTickInterrupt();
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}

void chip_stduart_init(void)
{
    hw_start_hal();
}

void chip_nand_init(void)
{
#ifdef CONFIG_NAND_FLASH
    nand_init();
    //factory_storage_init();
#endif
}

static void hardware_cpu_caches_off(void)
{
  /* 0. Setup caches */
    asm volatile("mrc p15, 0, r1, c1, c0, 0 ;" // Read System Control Register (SCTLR)
         "bic r1, r1, #1 ;" // mmu off
         "bic r1, r1, #(1 << 12) ;" // i-cache off
         "bic r1, r1, #(1 << 2) ;" // d-cache & L2-$ off
         "mcr p15, 0, r1, c1, c0, 0 ;" // Write System Control Register (SCTLR)
         : /* Outputs */
         : /* Inputs */
         : "r1" /* clobbered */
    );
    /* 1. MMU, L1$ disable */
    asm volatile("mrc p15, 0, r1, c1, c0, 0 ;" // Read System Control Register (SCTLR)
        "bic r1, r1, #1 ;" // mmu off
        "bic r1, r1, #(1 << 12) ;" // i-cache off
        "bic r1, r1, #(1 << 2) ;" // d-cache & L2-$ off
        "mcr p15, 0, r1, c1, c0, 0;" // Write System Control Register (SCTLR)
        : /* Outputs */
        : /* Inputs */
        : "r1" /* clobbered */
    );
    /* 2. invalidate: L1$, TLB, branch predictor */
    asm volatile("mov r0, #0 ;"
        "mcr p15, 0, r0, c7, c5, 0 ;" /* Invalidate Instruction Cache */
        "mcr p15, 0, r0, c7, c5, 6 ;" /* Invalidate branch prediction array */
        "mcr p15, 0, r0, c8, c7, 0 ;" /* Invalidate entire Unified Main TLB */
        "mcr p15, 0, r0, c8, c6, 0 ;" /* Invalidate entire data TLB */
        "dsb ;" /* Data sync barrier */
        "isb ;" /* Instruction sync barrier */
        : /* Outputs */
        : /* Inputs */
        : "r0" /* clobbered */
    );
}

void sys_start(void)
{
    //hardware_cpu_caches_off();
    aos_init();
    gic_init();

    //platform_init_interrupts();

#if (RHINO_CONFIG_CPU_NUM > 1)
    /*int cross core init*/
    os_crosscore_int_init();
    krhino_task_cpu_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, aos_maintask, 0, 1);
#else
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, aos_maintask, 1);
#endif

    aos_start();

    return;
}

#if (RHINO_CONFIG_CPU_NUM > 1)
void sys_start_secondary(void)
{
    //hardware_cpu_caches_off();

    /*int cross core init*/
    os_crosscore_int_init();

    printf("Starting scheduler on CPU:%d.\r\n", cpu_cur_get());

#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif

    cpu_first_task_start();

    return;
}
#endif

/* If the address form addr to addr +len overrides the kernel's address space,
 * return true, otherwise return false */
bool hal_addr_overlap_kernel(void *addr, size_t len)
{
    char  *addr_start = addr;
    char  *addr_end   = addr + len;

    if (((uintptr_t)addr_start < (uintptr_t)&_heap_end) ||
        ((uintptr_t)addr_end > (uintptr_t)&__text_start)) {
        return false;
    }

    return true;
}

/* for aos_printf in rhino.c */
int hal_uart_print(char *buf, int size)
{
    uart_dev_t uart_stdio;

    char *outbuf = (char *)buf;
    int   wbytes = size;
    int   i      = 0;

    if (buf == NULL) {
        return 0;
    }

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;

    while(wbytes-- > 0) {
        if (*(outbuf + i) == '\n') {
            hal_uart_send(&uart_stdio, "\r", 1, AOS_WAIT_FOREVER);
        }
        hal_uart_send(&uart_stdio, outbuf + i, 1, AOS_WAIT_FOREVER);
        i++;
    }
    return size;
}
