/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <board.h>
#include <aos/hal/wdg.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_iwdg.h"

#ifdef HAL_IWDG_MODULE_ENABLED

#define LSI_FREQ (40000)

IWDG_HandleTypeDef hiwdg;

int32_t hal_wdg_init(wdg_dev_t *wdg)
{
    uint32_t fclock = 0;
    uint32_t reload = 0;

    if (wdg == NULL) {
        return -1;
    }

    /* only support one watchdog */
    if (wdg->port >= PORT_WDG_SIZE) {
        return -1;
    }

    fclock = LSI_FREQ;

    hiwdg.Instance       = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;

    reload = (wdg->config.timeout) * fclock / 256 / 1000;
    if ((reload > 0xFFF) || (reload == 0)) {
        return -1;
    }

    hiwdg.Init.Reload    = reload;

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        return -1;
    }
    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg)
{
    if (wdg == NULL) {
        return;
    }

    /* only support one watchdog */
    if (wdg->port >= PORT_WDG_SIZE) {
        return;
    }

    HAL_IWDG_Refresh(&hiwdg);
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg)
{
    /* Once started, can't stop the watchdog */
    return -1;
}

#endif

