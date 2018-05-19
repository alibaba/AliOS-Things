/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal/wifi.h"
#include "hal/ota.h"

#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"
#include "hal/soc/uart.h"

#include "nrf_drv_systick.h"

#define AOS_START_STACK 2000

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE


ktask_t *g_aos_init;
ktask_t *g_aos_app = NULL;
extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);
extern int32_t hal_uart_init(uart_dev_t* uart );
extern void hw_start_hal(void);


#ifdef AOS_BINS

extern void *syscall_ktbl[];
extern char  app_info_addr;
extern k_mm_head  *g_kmm_head;
struct m_app_info_t *app_info = (struct m_app_info_t *)&app_info_addr;

static void app_pre_init(void)
{
    memcpy((void *)(app_info->data_ram_start), (void *)(app_info->data_flash_begin),
           app_info->data_ram_end - app_info->data_ram_start);

    memset((void *)(app_info->bss_start), 0, app_info->bss_end - app_info->bss_start);

    krhino_add_mm_region(g_kmm_head, (void *)(app_info->heap_start),
                        app_info->heap_end - app_info->heap_start);

#if (RHINO_CONFIG_MM_LEAKCHECK > 0)
    krhino_mm_leak_region_init((void *)(app_info->data_ram_start), (void *)(app_info->data_ram_end));
    krhino_mm_leak_region_init((void *)(app_info->bss_start), (void *)(app_info->bss_end));
#endif

}
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
	
	hal_uart_init(NULL);
    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
	nrf_drv_clock_lfclk_request(NULL);
	
	app_timer_init();
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
    //app_timer_init();
    //nrf_drv_systick_init();
    soc_init();
    
#ifdef BOOTLOADER

#else
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif

#ifdef CONFIG_AOS_CLI
    aos_cli_init();
#endif

#ifdef AOS_KV
    aos_kv_init();
#endif

#ifdef WITH_SAL
    sal_device_init();
#endif

#ifdef AOS_LOOP
    aos_loop_init();
#endif

#ifdef AOS_FOTA 
    ota_service_init();
#endif

#ifdef AOS_BINS
    app_pre_init();

    if (app_info->app_entry) {
        app_info->app_entry((void *)syscall_ktbl, 0, NULL);
    }
#else
    /*aos_framework_init();*/
    application_start(0, NULL);	
#endif

#endif
}

extern void * __isr_vector;
int main(void)
{
   /* Reallocate reset vector at the very beginning of the boot. */
   SCB->VTOR = (unsigned long) &__isr_vector;

   aos_init();
   krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);

   aos_start();
   return 0;
}


