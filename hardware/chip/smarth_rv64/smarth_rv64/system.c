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

/*
#if defined(CONFIG_KERNEL_RHINO)
#define CONFIG_SYSTICK_HZ RHINO_CONFIG_TICKS_PER_SECOND
#endif
*/

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

int g_system_clock = IHS_VALUE;
extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);

int32_t drv_get_cpu_id(void)
{
    return 0;
}

#ifdef CONFIG_KERNEL_NONE
static void _system_init_for_baremetal(void)
{
    __enable_excp_irq();

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, 0);    //10ms

    mm_heap_initialize();
}


#if 0
void next_jump_addr(void)
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}
static void _system_jump_s_mode(void)
{
	/* set MSTATUS_MPP, MSTATUS_MPIE,  */
	uint64_t mstatus = __get_MSTATUS();
	mstatus |= (0x01 << 11);
	mstatus |= (0x0 << 7);
	__set_MSTATUS(mstatus);
	/* set MEPC */
	__set_MEPC(next_jump_addr);
	/* set satp 0 */
	/* mret */
	__MRET();
	next_jump_addr();
}
#endif

#endif

#ifndef CONFIG_KERNEL_NONE
static void _system_init_for_kernel(void)
{
    irq_vectors_init();

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, 0);    //10ms
    drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#else
    extern void aos_heap_set();
    aos_heap_set();
#endif
}
#endif

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    int i;

    /* enable theadisaee */
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

    /* enable msoft interrupt */
    uint32_t mie = __get_MIE();
    mie |= (1 << 3);
    __set_MIE(mie);
    //drv_irq_enable(Machine_Software_IRQn);

    /* enable interrupt 18 */
    PLIC->PLIC_H0_MIE[0] = 0xffffffff;
#ifdef CONFIG_KERNEL_NONE
    _system_init_for_baremetal();
#else
    _system_init_for_kernel();
#endif

    //_system_jump_s_mode();
}
