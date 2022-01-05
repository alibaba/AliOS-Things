/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdbool.h>
#include <aos/errno.h>
#include <aos_hal_wdg.h>
#include <esp_task_wdt.h>
#include <esp_err.h>

static inline int32_t esp_err_to_std(esp_err_t error)
{
    switch (error) {
    case ESP_OK:
        return 0;
    case ESP_FAIL:
        return -1;
    case ESP_ERR_NO_MEM:
        return -ENOMEM;
    case ESP_ERR_INVALID_ARG:
        return -EINVAL;
    case ESP_ERR_NOT_SUPPORTED:
        return -ENOSYS;
    case ESP_ERR_TIMEOUT:
        return -ETIMEDOUT;
    default:
       return -1;
    }
}

int32_t aos_hal_wdg_init(wdg_dev_t *wdg)
{
    esp_err_t ret;

    if (wdg == NULL)
        return -EINVAL;

    /* Initialize TWDT */
    ret = esp_err_to_std(esp_task_wdt_init(wdg->config.timeout, true));
    if (ret < 0)
        return ret;

    /* Subscribe current task to the TWDT. */
    ret = esp_err_to_std(esp_task_wdt_add(NULL));
    if (ret < 0)
        return ret;

    ret = esp_err_to_std(esp_task_wdt_status(NULL));
    return ret;
}

void aos_hal_wdg_reload(wdg_dev_t *wdg)
{
    esp_err_t ret;

    ret = esp_task_wdt_reset();
    if (ret != ESP_OK)
        printf("Error:%s:%d:ret:%d\n", __func__, __LINE__, ret);
}

int32_t aos_hal_wdg_finalize(wdg_dev_t *wdg)
{
    esp_err_t ret;

    ret = esp_err_to_std(esp_task_wdt_delete(NULL));
    if (ret < 0)
        return ret;
    ret = esp_err_to_std(esp_task_wdt_status(NULL));
    if (ret != ESP_ERR_NOT_FOUND)
        return -1;

    /* Try to deinit, maybe fail if there is still task subscribes. */
    esp_task_wdt_deinit();

    return 0;
}
