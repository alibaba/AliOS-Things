/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>
#include <stdio.h>
#include <stdlib.h>

#include "osal.h"
#include "wifinetstack.h"
#include "idmanage/pbuf.h"
#include "security/drv_security.h"
#include "phy/drv_phy.h"
#include "soc_defs.h"
#include "ieee80211_mgmt.h"
#include "ieee80211_mac.h"
#include "sta_func.h"
#include "wifi_api.h"
#include "netstack.h"
#include "netstack_def.h"
#include "rf/rf_api.h"
#include "wdt/drv_wdt.h"
#include "gpio/drv_gpio.h"
#include "drv_uart.h"

#define AOS_START_STACK (2048 + 512)

ktask_t *g_radio_init;

static void ssvradio_init_task(void *pdata)
{
    PBUF_Init();
    NETSTACK_RADIO.init();
    drv_sec_init();
    netstack_init(NULL);
    OS_TaskDelete(NULL);
}

extern uint32_t SAVED_PC;
extern void dump_ir();
static void aos_wdt_process() {
    printf("IPC:%lxh\n", SAVED_PC);
    //ktask_t *cur = krhino_cur_task_get();
    ktask_t *task = g_active_task[0];
    printf("TP:%xh\n", task);
    printf("TN:%s\n", task->task_name);
    printf("g_intrpt_nested_level[0]=%d\n", g_intrpt_nested_level[0]);
    dump_ir();
}

static void temperature_compensation_task(void *pdata)
{
    //drv_gpio_set_dir(GPIO_10, GPIO_DIR_OUT);
    //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_LOW);
    printf("%s\n", __func__);
    OS_MsDelay(1*1000);
    load_rf_table_from_flash();
    write_reg_rf_table();
#if defined(CONFIG_ENABLE_WDT)
    drv_wdt_init();
    drv_wdt_enable(SYS_WDT, 6000);
    drv_wdt_register_isr(SYS_WDT, 255, aos_wdt_process);
#endif
    while(1)
    {
        //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_LOW);
        //OS_MsDelay(1000);
        //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_HIGH);
        //OS_MsDelay(1000);
        OS_MsDelay(3*1000);
        do_temerature_compensation();
#if defined(CONFIG_ENABLE_WDT)
        drv_wdt_kick(SYS_WDT);
#endif
    }
    OS_TaskDelete(NULL);
}

ktask_t *g_aos_init;
static kinit_t kinit;
static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);
extern void flash_partition_init();

static void system_init(void)
{
    flash_partition_init();

    hw_start_hal();

    var_init();

    aos_components_init(&kinit);

#ifndef AOS_BINS
    application_start(0, NULL);
#endif
}

static void do_awss_reset();
void isr_gpio_12()
{
    drv_gpio_intc_clear(GPIO_12);
    //aos_schedule_call(do_awss_reset, NULL);
    REG32(0xc0000c00) = '?';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '2';
//    REG32(0xc0000c00) = '\n';
}
extern void do_awss_active();
void isr_gpio_11()
{
    drv_gpio_intc_clear(GPIO_11);
    //aos_schedule_call(do_awss_active, NULL);
    REG32(0xc0000c00) = '!';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '\n';
}

void APP_Init(void)
{
    board_init();

    OS_Init();
    OS_MemInit();
    OS_PsramInit();

    drv_gpio_set_dir(GPIO_12, GPIO_DIR_IN);
    drv_gpio_set_dir(GPIO_11, GPIO_DIR_IN);
    drv_gpio_set_pull(GPIO_12, GPIO_PULL_UP);
    drv_gpio_set_pull(GPIO_11, GPIO_PULL_UP);

    drv_gpio_intc_trigger_mode(GPIO_12, GPIO_INTC_FALLING_EDGE);
    drv_gpio_intc_trigger_mode(GPIO_11, GPIO_INTC_FALLING_EDGE);

    drv_gpio_register_isr(GPIO_12, isr_gpio_12);
    drv_gpio_register_isr(GPIO_11, isr_gpio_11);

    OS_TaskCreate(ssvradio_init_task, "ssvradio_init", 512, NULL, 1, NULL);
    OS_TaskCreate(temperature_compensation_task, "rf temperature compensation", 256+128, NULL, 15, NULL);
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)system_init, 1);

    OS_StartScheduler();
}
