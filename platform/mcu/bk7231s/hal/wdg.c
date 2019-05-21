/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "rtos_pub.h"
#include "wdt_pub.h"
#include "icu_pub.h"

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t para;

    para = PWD_ARM_WATCHDOG_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &para);
    sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &wdg->config.timeout);
    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    (void)wdg;

    sddev_control(WDT_DEV_NAME, WCMD_RELOAD_PERIOD, 0);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    uint32_t para;
    (void)wdg;

    para = PWD_ARM_WATCHDOG_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &para);
    return 0;
}
