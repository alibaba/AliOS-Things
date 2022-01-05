/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. Oct 2018
 ******************************************************************************/

#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>
#include <drv_irq.h>
#include <csi_kernel.h>

int g_system_clock = IHS_VALUE;
extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);

int32_t drv_get_cpu_id(void)
{
    return 0;
}

static void _system_init_for_kernel(void)
{
    irq_vectors_init();

    csi_coret_config(drv_get_sys_freq() / RHINO_CONFIG_TICKS_PER_SECOND, 0);
    drv_irq_enable(CORET_IRQn);

    extern void aos_heap_set();
    aos_heap_set();
}

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    int i;

    /* enable theadisaee(Enable the extended instructions) */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    __set_MXSTATUS(mxstatus);

    g_system_clock = IHS_VALUE;

    /* set PLIC_PER */
    PLIC->PLIC_PER = 0x1;

    for (i = 0; i < 32; i++) {
        PLIC->PLIC_IP[i] = 0;
    }
    for (i = 0; i < 1023; i++) {
        PLIC->PLIC_H0_MCLAIM = i;
    }

    for (i = 0; i < 1023; i++) {
        PLIC->PLIC_PRIO[i] = 31;
    }

    /* set hart threshold 0, enable all interrupt */
    PLIC->PLIC_H0_MTH = 0;

    /* enable cache */
    csi_dcache_enable();
    csi_icache_enable();

    /* enable interrupt 18 */
    PLIC->PLIC_H0_MIE[0] = 0xffffffff;
    _system_init_for_kernel();
}

