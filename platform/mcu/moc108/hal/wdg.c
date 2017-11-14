/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "mico_rtos.h"
#include "wdt_pub.h"
#include "icu_pub.h"

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t para;

    para = PWD_ARM_WATCHDOG_CLK_BIT;
    icu_ctrl(CMD_CLK_PWR_UP, &para);
    wdt_ctrl(WCMD_SET_PERIOD, &wdg->config.timeout);
    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    (void)wdg;

    wdt_ctrl(WCMD_CLEAR_COUNTER, 0);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    uint32_t para;
    (void)wdg;

    para = PWD_ARM_WATCHDOG_CLK_BIT;
    icu_ctrl(CMD_CLK_PWR_DOWN, &para);
    return 0;
}