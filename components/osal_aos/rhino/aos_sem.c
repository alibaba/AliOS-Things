/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC > 0) && (RHINO_CONFIG_SEM > 0))
aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
{
    kstat_t ret;
    ksem_t *s;

    if (sem == NULL) {
        return -EINVAL;
    }

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    kstat_t ret;
    ksem_t *s;

    (void)options;
    if (sem == NULL) {
        return -EINVAL;
    }

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL) {
        return;
    }
    krhino_sem_del((ksem_t *)*sem);

    aos_free(*sem);

    *sem = NULL;
}

aos_status_t aos_sem_wait(aos_sem_t *sem, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(sem);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_sem_take((ksem_t *)*sem, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_sem_take((ksem_t *)*sem, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL) {
        return;
    }

    krhino_sem_give((ksem_t *)*sem);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    if (sem == NULL || *sem == NULL) {
        return;
    }

    krhino_sem_give_all((ksem_t *)*sem);
}

bool aos_sem_is_valid(aos_sem_t *sem)
{
    ksem_t *k_sem;

    if (sem == NULL) {
        return false;
    }

    k_sem = *sem;

    if (k_sem == NULL) {
        return false;
    }

    if (k_sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        return false;
    }

    return true;
}

#endif
