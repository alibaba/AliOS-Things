/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/wdt.h>
#include "aos/hal/gpio.h"
#include "hal_iomux_haas1000.h"

static gpio_dev_t wdg_gpio = {0, 0, NULL};
static int watchdog_flag = 0;

csi_error_t csi_wdt_init(csi_wdt_t *wdt, uint32_t idx)
{
    if(!wdt)
        return CSI_ERROR;

    printf("%s - %d\r\n", __func__, __LINE__);
    wdt->dev.idx = idx;

    if(watchdog_flag == 1) {
        printf("%s - wdt already init\r\n", __func__);
        return;
    }

    if(wdg_gpio.port == 0) {
        wdg_gpio.port   = HAL_IOMUX_PIN_P4_5;
        wdg_gpio.config = OUTPUT_PUSH_PULL;
        hal_gpio_init(&wdg_gpio);
    }

    watchdog_flag = 1;

    return CSI_OK;
}

void csi_wdt_uninit(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);

    if(watchdog_flag == 0) {
        return;
    }

    hal_gpio_finalize(&wdg_gpio);

    if(wdt) {
        printf("%s - %d\r\n", __func__, __LINE__);
        wdt->dev.idx = -1;
    }
}

csi_error_t csi_wdt_set_timeout(csi_wdt_t *wdt, uint32_t ms)
{

    printf("%s - %d, ignore timeout value set\r\n", __func__, __LINE__);

    return CSI_OK;
}

csi_error_t csi_wdt_start(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);
    return CSI_OK;
}

void csi_wdt_stop(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);
}

csi_error_t csi_wdt_feed(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);

    if(watchdog_flag == 0) {
        printf("%s - watchdog is not init yet\r\n", __func__);
        return;
    }

    hal_gpio_output_toggle(&wdg_gpio);

    return CSI_OK;
}

static uint32_t wdt_irqhandler(void *Data)
{
    printf("%s - %d\r\n", __func__, __LINE__);
    return CSI_OK;
}

csi_error_t csi_wdt_attach_callback(csi_wdt_t *wdt, void *callback, void *arg)
{
    printf("%s - %d\r\n", __func__, __LINE__);
    return CSI_OK;
}

void csi_wdt_detach_callback(csi_wdt_t *wdt)
{
    printf("%s - %d\r\n", __func__, __LINE__);
    return;
}
