/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include "object_api.h"

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static obj_class_t   aos_event_class;                         /* event object class */
obj_class_t         *aos_event_class_id = &aos_event_class;        /* event class id */

typedef struct aos_obj_event {
    obj_core_t      obj_core;
    kevent_t        event;
} aos_obj_event_t;

static void *obj_event_create_func(const char *name, va_list vaList)
{
    aos_obj_event_t    *event_id;
    uint32_t    value;
    int         ret = 0;
    /* get the initial value from the arg list */
    value = va_arg(vaList, uint32_t);
    /* create a posix event object */
    if ((event_id = (aos_obj_event_t *) obj_alloc(aos_event_class_id)) == NULL) {
        obj_errno = ENOSPC;
        return (NULL);
    }

    if ((name == NULL) || (*name == '\0')) {
        name = "no_name_event";
    }

    /* create a aos event */
    ret = krhino_event_create(&event_id->event, name, value);
    if (ret != 0) {
        obj_free(aos_event_class_id, (obj_core_t *) event_id);
        obj_errno = ENOSPC;
        return (NULL);
    }

    /* initialize the event object core */
    obj_core_init((obj_core_t *) event_id, aos_event_class_id);

    /* mark object memory to be freed during automatic resource reclamation */
    OBJ_SET_DEALLOC_MEM(&event_id->obj_core);
    /* return the ID */
    return (void *)event_id;
}

static int   obj_event_destroy_func(void *obj_id, RHINO_BOOL dealloc)
{
    aos_obj_event_t *event_id = (aos_obj_event_t *) obj_id;

    if (obj_verify(obj_id, aos_event_class_id) != OK) {
        obj_errno = EINVAL;
        return (ERROR);                         /* invalid object */
    }

    /* Invalidate and wait until no-one else is using this event */
    obj_core_invalidate((obj_core_t *)obj_id);     /* invalidate the object */

    /*
     * Prevent task deletion to ensure the counting event and any
     * owned objects are deleted.
     */

    krhino_event_del(&event_id->event);
    /* terminate the event object and delete the event */
    obj_core_deinit((obj_core_t *)obj_id);

    if (dealloc) {
        obj_free(aos_event_class_id, (obj_core_t *)event_id);
    }

    return (OK);
}

int   aos_obj_event_lib_init(void)
{
    return (class_init(aos_event_class_id, sizeof(struct aos_obj_event),
                       (obj_create_func_t) obj_event_create_func, obj_event_destroy_func,
                       NULL, NULL, NULL, EVENT_CLASS_TYPE, 0));
}

int   aos_event_open(aos_hdl_t *event, const char *name, int o_flag, uint32_t   value)
{
    int    obj_open_mode = 0;   /* flags translated for obj_open */
    aos_obj_event_t *obj_event;            /* kernel ID of the event object */
    int   tem_errno;

    /* event must have a name */
    if ((event == NULL) || (name == NULL) || (*name == '\0') || strlen(name) > OBJ_NAME_MAX) {
        return -EINVAL;
    }

    /* translate flag to the obj_open equivalents */
    if ((o_flag & O_CREAT) != 0) {
        obj_open_mode |= OM_CREATE;
    }

    if ((o_flag & O_EXCL) != 0) {
        obj_open_mode |= OM_EXCL;
    }

    /* open the object */
    obj_event = (aos_obj_event_t *) obj_open(name, obj_open_mode, aos_event_class_id, value);

    if (obj_event == NULL) {
        /* event creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            tem_errno = EEXIST;
        }

        else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            tem_errno = ENOENT;
        } else {
            tem_errno = ENOSPC;
        }

        /* return failure */
        return -tem_errno;
    }

    event->hdl = obj_event;
    return 0;
}


int   aos_event_new(aos_hdl_t *event, uint32_t   value)
{
    aos_obj_event_t *event_id;     /* kernel object ID of the new event */
    int   temp_errno;
    /* validate the descriptor buffer pointer */

    if (event == NULL) {
        return -EINVAL;
    }
    /* create a posix event */
    event_id = (aos_obj_event_t *) obj_open(NULL, OM_CREATE, aos_event_class_id, value);

    if (event_id == NULL) {
        /* event creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            temp_errno = EEXIST;
        } else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            temp_errno = ENOENT;
        } else {
            temp_errno = ENOSPC;
        }
        return -temp_errno;
    }

    /* write the new event ID into the caller's buffer.*/
    event->hdl = (void *)event_id;

    return 0;
}

static int   _event_destroy(aos_obj_event_t *event_id, RHINO_BOOL   dealloc)
{

    if (obj_verify(event_id, aos_event_class_id) != OK) {
        /* invalid object */
        return -EINVAL;
    }

    /* this interface is allowed only for unnamed events */
    if (OBJ_IS_NAMED(event_id)) {
        return -EINVAL;
    }

    /* Invalidate and wait until no-one else is using this event */
    obj_core_invalidate((obj_core_t *)event_id);     /* invalidate the object */

    krhino_event_del(&event_id->event);
    obj_core_deinit((obj_core_t *)event_id);

    if (dealloc) {
        obj_free(aos_event_class_id, (obj_core_t *)event_id);
    }

    return 0;
}

void aos_event_free(aos_hdl_t *event)
{
    if (event == NULL) {
        return;
    }

    _event_destroy(event->hdl, RHINO_TRUE);
    event->hdl = NULL;
}

int   aos_event_close(aos_hdl_t *event)
{
    int   ret;
    if (event == NULL) {
        return -EINVAL;
    }

    /* call the common routine with the kernel object ID */
    ret = obj_close(event->hdl, aos_event_class_id);
    if (ret == ERROR) {
        return  -EINVAL;
    }

    return 0;
}

int   aos_event_get(aos_hdl_t *event, uint32_t   value, uint8_t opt, uint32_t   *actl_value, uint32_t   timeout)
{
    kstat_t ret;
    aos_obj_event_t *obj_event;

    if (event == NULL || event->hdl == NULL) {
        return -EINVAL;
    }

    obj_event = (aos_obj_event_t *)(event->hdl);

    if (obj_verify(obj_event, aos_event_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_event_get(&obj_event->event, value, opt,
                               actl_value, RHINO_WAIT_FOREVER);

    } else {
        ret = krhino_event_get(&obj_event->event, value, opt,
                               actl_value, MS2TICK(timeout));
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else if ((ret == RHINO_BLK_TIMEOUT) || (ret == RHINO_NO_PEND_WAIT)) {
        return -ETIMEDOUT;
    }
    /* rhino err return is positive */
    return -ret;
}

int   aos_event_set(aos_hdl_t *event, uint32_t value, uint8_t opt)
{
    aos_obj_event_t *obj_event;
    int   ret;
    if (event == NULL || event->hdl == NULL) {
        return -EINVAL;
    }

    obj_event = (aos_obj_event_t *)(event->hdl);

    if (obj_verify(obj_event, aos_event_class_id) != OK) {
        return -EINVAL;
    }

    ret = krhino_event_set(&obj_event->event, value, opt);
    if (ret == 0) {
        return 0;
    }
    return -ret;
}

int  aos_event_unlink(const char *name)
{
    int         status = ERROR;

    /* check if name is valid */
    if ((name == NULL) || (*name == '\0')) {
        return -ENOENT;
    }

    /* check name length */
    if (strlen(name) > NAME_MAX) {
        return -ENAMETOOLONG;
    }

    status = obj_unlink(name, aos_event_class_id);

    if (status == ERROR) {
        return -ENOENT;
    }

    return 0;
}