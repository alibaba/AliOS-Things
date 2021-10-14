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

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
aos_status_t aos_mutex_new(aos_mutex_t *mutex)
{
    kstat_t   ret;
    kmutex_t *m;

    if (mutex == NULL) {
        return -EINVAL;
    }

    m = aos_malloc(sizeof(kmutex_t));
    if (m == NULL) {
        return -ENOMEM;
    }

    ret = krhino_mutex_create(m, "AOS");

    if (ret != RHINO_SUCCESS) {
        aos_free(m);
        return rhino2stderrno(ret);
    }
    *mutex = m;

    return 0;
}

aos_status_t aos_mutex_create(aos_mutex_t *mutex, uint32_t options)
{
    kstat_t   ret;
    kmutex_t *m;

    (void)options;
    if (mutex == NULL) {
        return -EINVAL;
    }

    m = aos_malloc(sizeof(kmutex_t));
    if (m == NULL) {
        return -ENOMEM;
    }

    ret = krhino_mutex_create(m, "AOS");

    if (ret != RHINO_SUCCESS) {
        aos_free(m);
        return rhino2stderrno(ret);
    }
    *mutex = m;

    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    if (mutex == NULL || *mutex == NULL) {
        return;
    }
    krhino_mutex_del((kmutex_t *)*mutex);

    aos_free(*mutex);

    *mutex = NULL;
}

aos_status_t aos_mutex_lock(aos_mutex_t *mutex, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, MS2TICK(timeout));
    }

    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

aos_status_t aos_mutex_unlock(aos_mutex_t *mutex)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    ret = krhino_mutex_unlock((kmutex_t *)*mutex);
    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    kmutex_t *k_mutex;

    if (mutex == NULL) {
        return false;
    }

    k_mutex = (kmutex_t *)*mutex;

    if (k_mutex == NULL) {
        return false;
    }

    if (k_mutex->blk_obj.obj_type != RHINO_MUTEX_OBJ_TYPE) {
        return false;
    }

    return true;
}
#endif
