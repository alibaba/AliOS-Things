/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/* Implement aos semaphore apis on freertos kernel.
 *
 * Don't use these semaphore apis in interrupt service routines,
 *
 * The configSUPPORT_DYNAMIC_ALLOCATION must be set to 1 in FreeRTOSConfig.h,
 * or left undefined.
 *
 * Implement counting semaphore by default to keep same behavior with rhino.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "aos/kernel.h"
#include "aos/errno.h"

#define MAXCOUNT 0xffffffffu

aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
{
    SemaphoreHandle_t xSemaphore;

    if (sem == NULL)
        return -EINVAL;

    xSemaphore = xSemaphoreCreateCounting(MAXCOUNT, count);
    *sem = xSemaphore;
    return (xSemaphore != NULL) ? 0 : -ENOMEM;
}

aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    SemaphoreHandle_t xSemaphore;

    (void) options;
    if (sem == NULL)
        return -EINVAL;

    xSemaphore = xSemaphoreCreateCounting(MAXCOUNT, count);
    *sem = xSemaphore;
    return (xSemaphore != NULL) ? 0 : -ENOMEM;
}

void aos_sem_free(aos_sem_t *sem)
{
    if ((sem == NULL) || (*sem == NULL))
        return;

    vSemaphoreDelete((SemaphoreHandle_t)*sem);
    *sem = NULL;
}

aos_status_t aos_sem_wait(aos_sem_t *sem, uint32_t timeout)
{
    BaseType_t ret;

    // CHECK_HANDLE(sem);

    ret = xSemaphoreTake((SemaphoreHandle_t)*sem,
        (timeout == AOS_WAIT_FOREVER) ? portMAX_DELAY : pdMS_TO_TICKS(timeout));

    return (ret == pdTRUE) ? 0 : -1;
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL)
        return;

    xSemaphoreGive((SemaphoreHandle_t)*sem);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL)
        return;

    /* TODO: Keep atomicity between getting count and singal all. */
    aos_sem_signal(sem);

    return;
}

bool aos_sem_is_valid(aos_sem_t *sem)
{
    if ((sem == NULL) || (*sem == NULL))
        return false;

    return true;
}
