/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include "project.h"
#include <k_api.h>
#include <stdio.h>
#include <aos/init.h>
#include "uart_port.h"
#include "hw.h"
#include "hw_conf.h"
#include "spi.h"
#include "hw_rtc.h"
#include "asr_timer.h"

#ifdef CERTIFICATION
#define AOS_START_STACK 256
#else
#define AOS_START_STACK 512
#endif

ktask_t *g_aos_init;
aos_task_t task;
static kinit_t kinit;
extern int application_start(int argc, char **argv);
extern void GpioIsrEntry (void);

void board_init(void);
#ifdef CERTIFICATION
    char *ptr;
    extern void test_certificate(void);
    extern void log_no_cli_init(void);
#endif
void PendSV_Handler(void);
#ifdef AOS_KV
int aos_kv_init(void);
#endif
#ifdef VCALL_RHINO
extern void dumpsys_cli_init(void);
#endif
static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

void SysTick_IRQ(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

static void sys_init(void)
{
    board_init();
    var_init();
#ifdef AOS_KV
    aos_kv_init();
    uint32_t baudrate;
    int len;
    if(aos_kv_get("sys_baud", &baudrate, &len) == 0) {
        HW_Set_MFT_Baud(baudrate);
    }
#endif

    default_UART_Init();

    global_irq_StartEx(GpioIsrEntry);

    BoardInitMcu();

#ifndef CERTIFICATION
#ifdef CONFIG_LINKWAN_AT
#else
//    aos_cli_init();
#endif
#ifdef VCALL_RHINO
    dumpsys_cli_init();
#endif
    application_start(kinit.argc,kinit.argv);
#else

    log_no_cli_init();
    test_certificate();
#endif

    while(1)
    {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND*10);
    }
}

static void sys_start(void)
{
    kstat_t stat;
    aos_init();

    stat = krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    if(stat != RHINO_SUCCESS)
    {
        return;
    }

    SpiInit();
     /* Configure SysTick timer to generate interrupt every 1 ms */
    CySysTickStart();
    CySysTickSetReload(CYDEV_BCLK__SYSCLK__HZ/RHINO_CONFIG_TICKS_PER_SECOND);
    CyIntSetSysVector(CY_INT_PEND_SV_IRQN, PendSV_Handler);
    CySysTickEnable();
    CySysTickSetCallback(0, SysTick_IRQ);
    /* set wco */
    Asr_Timer_Init();
    RtcInit();

    aos_start();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    sys_start();
    return 0;
}

