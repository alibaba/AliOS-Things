/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include "object_api.h"

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static obj_class_t   aos_mutex_class;                     /* mutex object class */
obj_class_t         *aos_mutex_class_id = &aos_mutex_class;

typedef struct aos_obj_mutex {
    obj_core_t      obj_core;
    kmutex_t        mutex;
} aos_obj_mutex_t;

static void *obj_mutex_create_func(const char *name, va_list vaList)
{
    aos_obj_mutex_t    *mutex_id;
    int ret = 0;

    if ((mutex_id = (aos_obj_mutex_t *) obj_alloc(aos_mutex_class_id)) == NULL) {
        obj_errno = ENOSPC;
        return (NULL);
    }

    if ((name == NULL) || (*name == '\0')) {
        name = "no_name_mutex";
    }

    /* create a aos mutex */
    ret = krhino_mutex_create(&mutex_id->mutex, name);
    if (ret != 0) {
        obj_free(aos_mutex_class_id, (obj_core_t *) mutex_id);
        obj_errno = ENOSPC;
        return (NULL);
    }

    /* initialize the mutex object core */
    obj_core_init((obj_core_t *) mutex_id, aos_mutex_class_id);

    /* mark object memory to be freed during automatic resource reclamation */
    OBJ_SET_DEALLOC_MEM(&mutex_id->obj_core);
    /* return the ID */
    return (void *)mutex_id;
}

static int obj_mutex_destroy_func(void *obj_id, RHINO_BOOL dealloc)
{
    aos_obj_mutex_t *mutex_id = (aos_obj_mutex_t *) obj_id;
    if (obj_verify(obj_id, aos_mutex_class_id) != OK) {
        obj_errno = EINVAL;
        return (ERROR);                         /* invalid object */
    }
    /* Invalidate and wait until no-one else is using this mutex */
    obj_core_invalidate((obj_core_t *)obj_id);     /* invalidate the object */

    krhino_mutex_del(&mutex_id->mutex);
    /* terminate the mutex object and delete the mutex */
    obj_core_deinit((obj_core_t *)obj_id);

    if (dealloc) {
        obj_free(aos_mutex_class_id, (obj_core_t *)mutex_id);
    }

    return (OK);
}

int aos_obj_mutex_lib_init(void)
{
    return (class_init(aos_mutex_class_id, sizeof(struct aos_obj_mutex),
                       (obj_create_func_t) obj_mutex_create_func, obj_mutex_destroy_func,
                       NULL, NULL, NULL, MUTEX_CLASS_TYPE, 0));
}

int aos_mutex_new(aos_hdl_t *mutex)
{
    aos_obj_mutex_t *mutex_id;     /* kernel object ID of the new mutex */
    int temp_errno;
    /* validate the descriptor buffer pointer */

    if (mutex == NULL) {
        return -EINVAL;
    }
    /* create a posix mutex */
    mutex_id = (aos_obj_mutex_t *) obj_open(NULL, OM_CREATE, aos_mutex_class_id);

    if (mutex_id == NULL) {
        /* mutex creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            temp_errno = EEXIST;
        } else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            temp_errno = ENOENT;
        } else {
            temp_errno = ENOSPC;
        }
        return -temp_errno;
    }

    /* write the new mutex ID into the caller's buffer.*/
    mutex->hdl = (void *)mutex_id;

    return 0;
}

void aos_mutex_free(aos_hdl_t *mutex)
{
    aos_obj_mutex_t *mutex_id;
    if (mutex == NULL || mutex->hdl == NULL) {
        return;
    }
    mutex_id = mutex->hdl;
    if (obj_verify(mutex_id, aos_mutex_class_id) != OK) {
        /* invalid object */
        return;
    }

    /* this interface is allowed only for unnamed mutexs */
    if (OBJ_IS_NAMED(mutex_id)) {
        return;
    }

    /* Invalidate and wait until no-one else is using this mutex */
    obj_core_invalidate((obj_core_t *)mutex_id);     /* invalidate the object */

    krhino_mutex_del(&mutex_id->mutex);
    obj_core_deinit((obj_core_t *)mutex_id);

    obj_free(aos_mutex_class_id, (obj_core_t *)mutex_id);
    mutex->hdl = NULL;
}

int aos_mutex_lock(aos_hdl_t *mutex, uint32_t timeout)
{
    kstat_t ret;
    aos_obj_mutex_t *obj_mutex;

    if (mutex == NULL || mutex->hdl == NULL) {
        return -EINVAL;
    }

    obj_mutex = (aos_obj_mutex_t *)(mutex->hdl);

    if (obj_verify(obj_mutex, aos_mutex_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    ret = krhino_mutex_lock(&obj_mutex->mutex,
                            timeout == AOS_WAIT_FOREVER ? RHINO_WAIT_FOREVER : MS2TICK(timeout));
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        return AOS_MUTEX_NESTED;
        /* ret = RHINO_SUCCESS; */
    }  else if ((ret == RHINO_BLK_TIMEOUT) || (ret == RHINO_NO_PEND_WAIT)) {
        return -ETIMEDOUT;
    }

    /* rhino err return is positive */
    return -ret;
}


int aos_mutex_unlock(aos_hdl_t *mutex)
{
    int ret;
    aos_obj_mutex_t *obj_mutex;
    if (mutex == NULL || mutex->hdl == NULL) {
        return -EINVAL;
    }

    obj_mutex = (aos_obj_mutex_t *)(mutex->hdl);

    if (obj_verify(obj_mutex, aos_mutex_class_id) != OK) {
        return -EINVAL;
    }

    ret = krhino_mutex_unlock(&obj_mutex->mutex);
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }
    return -ret;
}

int aos_mutex_is_valid(aos_hdl_t *mutex)
{

    if (mutex == NULL || mutex->hdl == NULL) {
        return  0;
    }

    return 1;
}

