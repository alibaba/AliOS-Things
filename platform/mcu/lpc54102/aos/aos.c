/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "pin_mux.h"
 
#include <aos/aos.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal/wifi.h"
#include "hal/ota.h"

#define AOS_START_STACK 1536

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;
ktask_t *g_aos_app = NULL;
extern int application_start(int argc, char **argv);
extern int aos_framework_init(void);

extern hal_wifi_module_t qca_4002_wmi;

static int init_wifi()
{
   int ret;
   PRINTF("Register WMI Wifi 0x%x", &qca_4002_wmi);
   hal_wifi_register_module(&qca_4002_wmi);
   ret = hal_wifi_init();
   PRINTF("hal_wifi_init return %d", ret);
}


extern void hw_start_hal(void);

static void sys_init(void)
{
    int i = 0;

    init_wifi();

#ifdef BOOTLOADER

#else
#ifdef AOS_VFS
    vfs_init();
    vfs_device_init();
#endif

#ifdef CONFIG_AOS_CLI
//    aos_cli_init();
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

    aos_framework_init();
    application_start(0, NULL);	
#endif
}
extern struct hal_ota_module_s hal_lpc54102_ota_module;
static void platform_init(void)
{
    uint32_t port_state = 0;

    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Board pin, clock, debug console init */
    /* attach 12 MHz clock to USART0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    /* enable clock for GPIO*/
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Sram1);
    CLOCK_EnableClock(kCLOCK_Sram2);


    BOARD_InitPins();
    BOARD_BootClockPLL96M(); /* Rev B device can only support max core frequency to 96Mhz.
                                Rev C device can support 100Mhz,use BOARD_BootClockPLL100M() to boot core to 100Mhz. 
                                DEVICE_ID1 register in SYSCON shows the device version.
                                More details please refer to user manual and errata. */
    BOARD_InitDebugConsole();	

#ifdef AOS_FOTA
    hal_ota_register_module(&hal_lpc54102_ota_module);
#endif

}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>
#include <hal/wifi.h>
#include <hal/ota.h>

#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)


void hal_reboot(void)
{
    NVIC_SystemReset();
}

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
}

int32_t hal_timer_start(timer_dev_t *tmr)
{
    return krhino_timer_start(tmr->priv);
}


void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}


void hw_start_hal(void)
{
    PRINTF("start-----------hal\n");
}


int main(void)
{
   uint32_t core_frequency = 0;
   platform_init();
	
   aos_init();
   krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
   core_frequency = CLOCK_GetCoreClkFreq();

   SysTick_Config(core_frequency / 100); //10ms
   aos_start();
   return 0;
}


