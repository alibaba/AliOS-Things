/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

#include "sys/xip.h"
#include "sys/dbgcon.h"
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
#include "wifi_api.h"
#include "rf/rf_api.h"
#include "wdt/drv_wdt.h"
#include "gpio/drv_gpio.h"

#define AOS_START_STACK (2048+512)

ktask_t *g_radio_init;
static void ssvradio_init_task(void *pdata)
{
    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
    netstack_init(NULL);
    OS_TaskDelete(NULL);
}

static void temperature_compensation_task(void *pdata)
{
    //drv_gpio_set_dir(GPIO_10, GPIO_DIR_OUT);
    //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_LOW);
    printf("%s\n", __func__);
    OS_MsDelay(1*1000);
    load_rf_table_from_flash();
    write_reg_rf_table();
    drv_wdt_init();
    drv_wdt_enable(SYS_WDT, 6000);
    while(1)
    {
        //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_LOW);
        //OS_MsDelay(1000);
        //drv_gpio_set_logic(GPIO_10, GPIO_LOGIC_HIGH);
        //OS_MsDelay(1000);
        OS_MsDelay(3*1000);
        do_temerature_compensation();
        drv_wdt_kick(SYS_WDT);
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
extern void board_init();
static void system_init(void)
{
    board_init();
    hw_start_hal();
    var_init();
    aos_kernel_init(&kinit);
}

static void do_awss_reset();
void isr_gpio_12()
{
    drv_gpio_intc_clear(GPIO_12);
    aos_schedule_call(do_awss_reset, NULL);
    REG32(0xc0000c00) = '?';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '2';
//    REG32(0xc0000c00) = '\n';
}
extern void do_awss_active();
void isr_gpio_11()
{
    drv_gpio_intc_clear(GPIO_11);
    aos_schedule_call(do_awss_active, NULL);
    REG32(0xc0000c00) = '!';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '1';
//    REG32(0xc0000c00) = '\n';
}

static void app_start(void)
{
    xip_init();
    xip_enter();
    flash_init();
    
    dbgcon_init(UART_SPR_BAUD_921600);
    
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
    OS_TaskCreate(temperature_compensation_task, "rf temperature compensation", 256, NULL, 1, NULL);
    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)system_init, 1);
    
    OS_StartScheduler();
}

void APP_Init(void)
{
    app_start();
}
