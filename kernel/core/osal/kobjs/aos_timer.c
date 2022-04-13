/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include "object_api.h"

#if (AOS_KOBJ_TIMER > 0)
#define MS2TICK(ms) krhino_ms_to_ticks(ms)

typedef void (*timer_func)(void *, void *);
static obj_class_t   aos_timer_class;                         /* timer object class */
obj_class_t         *aos_timer_class_id = &aos_timer_class;

typedef struct aos_obj_timer {
    obj_core_t      obj_core;
    ktimer_t        timer;
} aos_obj_timer_t;

static void *obj_timer_create_func(const char *name, va_list vaList)
{
    aos_obj_timer_t    *timer_id;
    int ret;
    timer_func  func;
    void *arg;
    uint32_t ms;
    uint8_t  repeat;
    uint8_t  auto_run;

    func = va_arg(vaList, timer_func);
    arg = va_arg(vaList, void *);
    ms = va_arg(vaList, uint32_t);
    repeat = va_arg(vaList, uint8_t);
    auto_run = va_arg(vaList, uint8_t);

    if ((timer_id = (aos_obj_timer_t *) obj_alloc(aos_timer_class_id)) == NULL) {
        obj_errno = ENOSPC;
        return (NULL);
    }

    if ((name == NULL) || (*name == '\0')) {
        name = "no_name_timer";
    }

    if (repeat == 0) {
        ret = krhino_timer_create(&timer_id->timer, name,
                                  (timer_cb_t)func, MS2TICK(ms), 0, arg, 1);
    } else {
        ret = krhino_timer_create(&timer_id->timer, name,
                                  (timer_cb_t)func, MS2TICK(ms), MS2TICK(ms), arg, 1);
    }

    if (ret != 0) {
        obj_free(aos_timer_class_id, (obj_core_t *) timer_id);
        obj_errno = ENOSPC;
        return (NULL);
    }

    /* initialize the timer object core */
    obj_core_init((obj_core_t *) timer_id, aos_timer_class_id);

    /* mark object memory to be freed during automatic resource reclamation */
    OBJ_SET_DEALLOC_MEM(&timer_id->obj_core);
    /* return the ID */
    return (void *)timer_id;
}

static int obj_timer_destroy_func(void *obj_id, RHINO_BOOL dealloc)
{
    aos_obj_timer_t *timer_id = (aos_obj_timer_t *) obj_id;
    int ret;

    if (obj_verify(obj_id, aos_timer_class_id) != OK) {
        obj_errno = EINVAL;
        return (ERROR);                         /* invalid object */
    }

    /* Invalidate and wait until no-one else is using this timer */
    obj_core_invalidate((obj_core_t *)obj_id);     /* invalidate the object */

    ret = aos_timer_stop(&timer_id->timer);
    ret = krhino_timer_del(&timer_id->timer);
    /* terminate the timer object and delete the timer */
    obj_core_deinit((obj_core_t *)obj_id);
    if (dealloc) {
        obj_free(aos_timer_class_id, (obj_core_t *)timer_id);
    }
    return (OK);
}

int aos_obj_timer_lib_init(void)
{
    return (class_init(aos_timer_class_id, sizeof(struct aos_obj_timer),
                       (obj_create_func_t) obj_timer_create_func, obj_timer_destroy_func,
                       NULL, NULL, NULL, TIMER_CLASS_TYPE, 0));
}


int aos_timer_new(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, uint8_t repeat)
{
    return aos_timer_new_ext(timer, fn, arg, ms, repeat, 1);
}
int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint8_t repeat, uint8_t  auto_run)
{
    aos_obj_timer_t *timer_id;     /* kernel object ID of the new timer */
    int temp_errno;
    /* validate the descriptor buffer pointer */

    if (timer == NULL) {
        return -EINVAL;
    }
    /* create a posix timer */
    timer_id = (aos_obj_timer_t *) obj_open(NULL, OM_CREATE, aos_timer_class_id, fn, arg, ms, repeat, (int)auto_run);

    if (timer_id == NULL) {
        /* timer creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            temp_errno = EEXIST;
        } else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            temp_errno = ENOENT;
        } else {
            temp_errno = ENOSPC;
        }
        return -temp_errno;
    }

    /* write the new timer ID into the caller's buffer.*/
    timer->hdl = (void *)timer_id;

    return 0;
}

void aos_timer_free(aos_hdl_t *timer)
{
    aos_obj_timer_t *timer_id;
    if (timer == NULL || timer->hdl == NULL) {
        return -EINVAL;
    }
    timer_id = timer->hdl;
    if (obj_verify(timer_id, aos_timer_class_id) != OK) {
        /* invalid object */
        return -EINVAL;
    }

    /* this interface is allowed only for unnamed timers */
    if (OBJ_IS_NAMED(timer_id)) {
        return -EINVAL;
    }

    /* Invalidate and wait until no-one else is using this timer */
    obj_core_invalidate((obj_core_t *)timer_id);     /* invalidate the object */

    krhino_timer_del(&timer_id->timer);
    obj_core_deinit((obj_core_t *)timer_id);

    obj_free(aos_timer_class_id, (obj_core_t *)timer_id);

    timer->hdl = NULL;
    return 0;
}

int aos_timer_start(aos_timer_t *timer)
{
    int ret;
    aos_obj_timer_t *obj_timer;

    if (timer == NULL || timer->hdl == NULL) {
        return -EINVAL;
    }

    obj_timer = (aos_obj_timer_t *)timer->hdl;
    if (obj_verify(obj_timer, aos_timer_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }
    ret = krhino_timer_start(&obj_timer->timer);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_stop(aos_timer_t *timer)
{
    int ret;
    aos_obj_timer_t *obj_timer;
    if (timer == NULL || timer->hdl == NULL) {
        return -EINVAL;
    }
    obj_timer = (aos_obj_timer_t *)timer->hdl;
    ret = krhino_timer_stop(&obj_timer->timer);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_change(aos_timer_t *timer, uint32_t ms)
{
    int ret;
    aos_obj_timer_t *obj_timer;
    if (timer == NULL || timer->hdl == NULL) {
        return -EINVAL;
    }
    obj_timer = (aos_obj_timer_t *)timer->hdl;

    ret = krhino_timer_change(&obj_timer->timer, MS2TICK(ms), MS2TICK(ms));
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_change_once(aos_timer_t *timer, uint32_t ms)
{
    int ret;
    aos_obj_timer_t *obj_timer;
    if (timer == NULL || timer->hdl == NULL) {
        return -EINVAL;
    }
    obj_timer = (aos_obj_timer_t *)timer->hdl;

    ret = krhino_timer_change(&obj_timer->timer, MS2TICK(ms), 0);

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_is_valid(aos_hdl_t *timer)
{

    if (timer == NULL || timer->hdl == NULL) {
        return 0;
    }

    return 1;
}

#endif
