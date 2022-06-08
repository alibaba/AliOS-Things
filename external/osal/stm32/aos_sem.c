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
#include "aos_kernel.h"
#include "aos_errno.h"

aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
{
    return 0;
}

aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    return;
}

aos_status_t aos_sem_wait(aos_sem_t *sem, uint32_t timeout)
{
    return 0;
}

void aos_sem_signal(aos_sem_t *sem)
{
    return;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    return;
}

bool aos_sem_is_valid(aos_sem_t *sem)
{
    return true;
}
