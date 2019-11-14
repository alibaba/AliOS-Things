/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal/wifi.h"

#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"
#include "aos/hal/uart.h"

#include "nrf_drv_systick.h"

#ifdef AOS_COMP_PWRMGMT
int pwrmgmt_init();
#endif /* AOS_COMP_PWRMGMT */

#if POWER_ENABLED
#include "nrf_drv_power.h"
#endif

#ifndef NRF52810_XXAA
#define AOS_START_STACK 2000
#else
#define AOS_START_STACK 300
#endif

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE


ktask_t *g_aos_init;
ktask_t *g_aos_app = NULL;
extern int application_start(int argc, char **argv);
extern int32_t hal_uart_init(uart_dev_t *uart );
extern void hw_start_hal(void);

#ifdef AOS_BINS
#include "aos/init.h"

extern void *kmbins_tbl[];
extern struct m_app_info_t *app_info;
extern void app_pre_init(void);

#endif

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void soc_init(void)
{
    ret_code_t err_code;
#if POWER_ENABLED
    const nrf_drv_power_config_t m_drv_power_config =
    {
       .dcdcen = 1,
    #if NRF_POWER_HAS_VDDH
       .dcdcenhv = 0
    #endif
    };
    nrf_drv_power_init(&m_drv_power_config );
#endif

    hal_uart_init(NULL);
    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    //  app_timer_init();
    /* Configure LED-pins as outputs */
    bsp_board_leds_init();

    /* Init systick driver */
    nrf_drv_systick_init();

    /* Activate deep sleep mode */
    //    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

}


static void sys_init(void)
{
    int i = 0;

    soc_init();

#ifdef AOS_COMP_VFS
    vfs_init();
#endif

#ifdef AOS_COMP_CLI
    aos_cli_init();
#endif

#ifdef AOS_COMP_ULOG
    ulog_init();
#endif

#ifdef AOS_COMP_KV
    aos_kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    vfs_device_init();
    aos_loop_init();
#endif

#ifdef AOS_COMP_PWRMGMT
    pwrmgmt_init();
#endif /* AOS_COMP_PWRMGMT */

    aos_show_welcome();

#ifdef AOS_BINS
    app_pre_init();

    if (app_info->app_entry) {
        app_info->app_entry((void *)kmbins_tbl, 0, NULL);
    }
#else
    application_start(0, NULL);
#endif

}

extern void *__isr_vector;
int main(void)
{
    /* Reallocate reset vector at the very beginning of the boot. */
    SCB->VTOR = (unsigned long) &__isr_vector;

    aos_init();
    krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);

    aos_start();
    return 0;
}


