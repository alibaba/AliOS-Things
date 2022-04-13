/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "2ndboot.h"
#include "sys.h"
#include "cmsis.h"
#include "hal_wdt.h"
#include "hal_bootmode.h"

static int set_silent_reboot_flag(void)
{
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_RESERVED_BIT24);
    return 0;
}

static void app_wdt_irq_handle(enum HAL_WDT_ID_T id, uint32_t status)
{
    analog_aud_codec_mute();
    set_silent_reboot_flag();
}

void wdg_init(unsigned int ms)
{
    uint32_t lock = int_lock();

    hal_wdt_set_irq_callback(HAL_WDT_ID_0, app_wdt_irq_handle);
    hal_wdt_set_timeout(HAL_WDT_ID_0, ms /1000);
    hal_wdt_start(HAL_WDT_ID_0);

	int_unlock(lock);
}

void wdg_feed(void)
{
    hal_wdt_ping(HAL_WDT_ID_0);
}

void wdg_finish()
{
    uint32_t lock = 0;

    lock = int_lock();
    hal_wdt_stop(HAL_WDT_ID_0);
    int_unlock(lock);
}