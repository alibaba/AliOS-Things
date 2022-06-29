/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/* Implement aos mutex apis on freertos kernel.
 *
 * Don't use these mutex apis in interrupt service routines.
 *
 * The configSUPPORT_DYNAMIC_ALLOCATION and configUSE_RECURSIVE_MUTEXES must both
 * be set to 1 in FreeRTOSConfig.h.
 *
 * Implement recursive mutex by default to keep same behavior with rhino.
 * Non-recursive mutex may be supported in future by options arguments.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "aos/kernel.h"
#include "aos/errno.h"

aos_status_t aos_mutex_new(aos_mutex_t *mutex)
{
    SemaphoreHandle_t xMutex;

    if (mutex == NULL)
        return -EINVAL;

    xMutex = xSemaphoreCreateRecursiveMutex();
    if (xMutex == NULL)
        return -1;

    *mutex = xMutex;
    return 0;
}

aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options)
{
    SemaphoreHandle_t xMutex;

    (void)options;
    if (mutex == NULL)
        return -EINVAL;

    xMutex = xSemaphoreCreateRecursiveMutex();
    if (xMutex == NULL)
        return -1;

    *mutex = xMutex;
    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL || *mutex == NULL)
        return;

    vSemaphoreDelete((SemaphoreHandle_t)*mutex);

    *mutex = NULL;
}

aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout)
{
    BaseType_t ret;

    // CHECK_HANDLE(mutex);

    ret = xSemaphoreTakeRecursive((SemaphoreHandle_t)*mutex,
        (timeout == AOS_WAIT_FOREVER) ? portMAX_DELAY : pdMS_TO_TICKS(timeout));

    return (ret == pdTRUE) ? 0 : -ETIMEDOUT;
}

aos_status_t aos_mutex_unlock(aos_mutex_t *mutex)
{
    BaseType_t ret;

    // CHECK_HANDLE(mutex);

    ret = xSemaphoreGiveRecursive((SemaphoreHandle_t)*mutex);
    return (ret == pdTRUE) ? 0 : -1;
}

bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    if ((mutex == NULL) || (*mutex == NULL))
        return false;

    return true;
}
