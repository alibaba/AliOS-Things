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

#if (RHINO_CONFIG_EVENT_FLAG > 0)

aos_status_t aos_event_new(aos_event_t *event, uint32_t value)
{
    int ret;
    if (event == NULL) {
        return -EINVAL;
    }

    ret = krhino_event_dyn_create((kevent_t **)(event), "AOS", value);

    return rhino2stderrno(ret);
}

aos_status_t aos_event_create(aos_event_t *event, uint32_t value, uint32_t options)
{
    int ret;

    (void)options;
    if (event == NULL) {
        return -EINVAL;
    }
    ret = krhino_event_dyn_create((kevent_t **)(event), "AOS", value);

    return rhino2stderrno(ret);
}

void aos_event_free(aos_event_t *event)
{
    if (event == NULL || *event == NULL) {
        return;
    }

    (void)krhino_event_dyn_del((kevent_t *)*event);

    *event = NULL;
}

aos_status_t aos_event_get(aos_event_t *event, uint32_t value, uint32_t opt,
                           uint32_t *actl_value, uint32_t timeout)
{
    kstat_t ret;

    CHECK_HANDLE(event);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_event_get((kevent_t *)*event, value, opt, actl_value, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_event_get((kevent_t *)*event, value, opt, actl_value, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

aos_status_t aos_event_set(aos_event_t *event, uint32_t flags, uint32_t opt)
{
    kstat_t ret;

    CHECK_HANDLE(event);

    ret = krhino_event_set((kevent_t *)*event, flags, opt);

    return rhino2stderrno(ret);
}

bool aos_event_is_valid(aos_event_t *event)
{
    kevent_t *k_event;

    if (event == NULL) {
        return false;
    }

    k_event = (kevent_t *)*event;

    if (k_event == NULL) {
        return false;
    }

    if (k_event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        return false;
    }

    return true;
}

#endif

