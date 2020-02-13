/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "pin_mux.h"

#include "aos/hal/i2c.h"
#include "aos/hal/gpio.h"
#include "aos/hal/spi.h"

#include "aos/init.h"
#include "aos/kernel.h"

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hal/wifi.h"

#define AOS_START_STACK 1536

#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_MAX_SSID_NAME_SIZE

ktask_t *g_aos_init;
ktask_t *g_aos_app = NULL;
static kinit_t kinit;
extern int application_start(int argc, char **argv);

extern hal_wifi_module_t qca_4002_wmi;

#ifdef DEV_SAL_MK3060
extern hal_wifi_module_t aos_wifi_module_mk3060;
#endif

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

static int init_wifi()
{
   int ret;
#ifdef DEV_SAL_MK3060
   PRINTF("Register WMI Wifi mk3060");
   hal_wifi_register_module(&aos_wifi_module_mk3060);
#else
   PRINTF("Register WMI Wifi 0x%x", &qca_4002_wmi);
   hal_wifi_register_module(&qca_4002_wmi);
#endif
   ret = hal_wifi_init();
   PRINTF("hal_wifi_init return %d", ret);
}


extern void hw_start_hal(void);
extern int sensor_init(void);
#if defined(AOS_SENSOR_TEMP_SENSIRION_HTS221) || defined(AOS_SENSOR_HUMI_SENSIRION_HTS221)
extern i2c_dev_t HTS221_ctx;
#endif

static void sys_init(void)
{
    int i = 0;

    init_wifi();

    var_init();
#ifdef BOOTLOADER

#else
#if defined(AOS_SENSOR_TEMP_SENSIRION_HTS221) || defined(AOS_SENSOR_HUMI_SENSIRION_HTS221)
    HTS221_ctx.port = 0;
#endif

    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif

#endif
}

static int i2c_init(void)
{
    i2c_dev_t i2c_dev;
    i2c_dev.port = 0;
    hal_i2c_init(&i2c_dev);
    return 0;
}

static int spi_init(void)
{
    spi_dev_t spi_dev;
    spi_dev.port = 1;
    hal_spi_init(&spi_dev);
    return 0;
}

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

    i2c_init();
    spi_init();
    board_led_init();
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <k_api.h>
#include "ulog/ulog.h"

#include "aos/hal/timer.h"
#include "network/hal/base.h"
#include <hal/wifi.h>

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

    core_frequency = CLOCK_GetCoreClkFreq();

    SysTick_Config(core_frequency / RHINO_CONFIG_TICKS_PER_SECOND); //10ms

    aos_init();
    krhino_task_dyn_create(&g_aos_app, "aos-init", 0, AOS_DEFAULT_APP_PRI, 0, AOS_START_STACK, (task_entry_t)sys_init, 1);
    aos_start();
    return 0;
}

