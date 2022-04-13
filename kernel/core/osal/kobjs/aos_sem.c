/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <fcntl.h>
#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include "object_api.h"

#if ((RHINO_CONFIG_SEM > 0))
#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static obj_class_t   aos_sem_class;                         /* semaphore object class */
obj_class_t         *aos_sem_class_id = &aos_sem_class;        /* semaphore class id */

typedef struct aos_obj_sem {
    obj_core_t      obj_core;
    ksem_t          sem;
} aos_obj_sem_t;


static void *obj_sem_create_func(const char *name, va_list vaList)
{
    aos_obj_sem_t    *sem_id;
    uint32_t  value;
    int  ret = 0;
    /* get the initial value from the arg list */
    value = va_arg(vaList, uint32_t);
    /* create a posix semaphore object */
    if ((sem_id = (aos_obj_sem_t *) obj_alloc(aos_sem_class_id)) == NULL) {
        obj_errno = ENOSPC;
        return (NULL);
    }

    if ((name == NULL) || (*name == '\0')) {
        name = "no_name_sem";
    }

    /* create a aos semaphore */
    ret = krhino_sem_create(&sem_id->sem, name, value);
    if (ret != 0) {
        obj_free(aos_sem_class_id, (obj_core_t *) sem_id);
        obj_errno = ENOSPC;
        return (NULL);
    }

    /* initialize the semaphore object core */
    obj_core_init((obj_core_t *) sem_id, aos_sem_class_id);

    /* mark object memory to be freed during automatic resource reclamation */
    OBJ_SET_DEALLOC_MEM(&sem_id->obj_core);
    /* return the ID */
    return (void *)sem_id;
}

static int  obj_sem_destroy_func(void *obj_id, RHINO_BOOL dealloc)
{
    aos_obj_sem_t *sem_id = (aos_obj_sem_t *) obj_id;

    if (obj_verify(obj_id, aos_sem_class_id) != OK) {
        obj_errno = EINVAL;
        return (ERROR);                         /* invalid object */
    }

    /* Invalidate and wait until no-one else is using this semaphore */
    obj_core_invalidate((obj_core_t *)obj_id);     /* invalidate the object */

    krhino_sem_del(&sem_id->sem);
    /* terminate the semaphore object and delete the sem */
    obj_core_deinit((obj_core_t *)obj_id);

    if (dealloc) {
        obj_free(aos_sem_class_id, (obj_core_t *)sem_id);
    }

    return (OK);
}

int  aos_obj_sem_lib_init(void)
{
    return (class_init(aos_sem_class_id, sizeof(struct aos_obj_sem),
                       (obj_create_func_t) obj_sem_create_func, obj_sem_destroy_func,
                       NULL, NULL, NULL, SEM_CLASS_TYPE, 0));
}

int  aos_sem_open(aos_hdl_t *sem, const char *name, int flag, uint32_t value)
{
    int   obj_open_mode = 0;   /* flags translated for obj_open */
    aos_obj_sem_t *obj_sem;            /* kernel ID of the semaphore object */
    int  tem_errno;

    /* semaphore must have a name */
    if ((sem == NULL) || (name == NULL) || (*name == '\0') || strlen(name) > OBJ_NAME_MAX) {
        return -EINVAL;
    }

    /* translate flag to the obj_open equivalents */
    if ((flag & O_CREAT) != 0) {
        obj_open_mode |= OM_CREATE;
    }

    if ((flag & O_EXCL) != 0) {
        obj_open_mode |= OM_EXCL;
    }
    /* open the object */
    obj_sem = (aos_obj_sem_t *) obj_open(name, obj_open_mode, aos_sem_class_id, value);

    if (obj_sem == NULL) {
        /* semaphore creation error */
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

    sem->hdl = obj_sem;
    return 0;
}

int  aos_sem_new(aos_hdl_t *sem, uint32_t count)
{
    aos_obj_sem_t *sem_id;     /* kernel object ID of the new semaphore */
    int  temp_errno;
    /* validate the descriptor buffer pointer */

    if (sem == NULL) {
        return -EINVAL;
    }
    /* create a posix semaphore */
    sem_id = (aos_obj_sem_t *) obj_open(NULL, OM_CREATE, aos_sem_class_id, count);

    if (sem_id == NULL) {
        /* semaphore creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            temp_errno = EEXIST;
        } else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            temp_errno = ENOENT;
        } else {
            temp_errno = ENOSPC;
        }
        return -temp_errno;
    }

    /* write the new semaphore ID into the caller's buffer.*/
    sem->hdl = (void *)sem_id;

    return 0;
}

void aos_sem_free(aos_hdl_t *sem)
{
    aos_obj_sem_t *sem_id;
    if (sem == NULL || sem->hdl == NULL) {
        return;
    }
    sem_id = sem->hdl;

    if (obj_verify(sem_id, aos_sem_class_id) != OK) {
        /* invalid object */
        return;
    }

    /* this interface is allowed only for unnamed semaphores */
    if (OBJ_IS_NAMED(sem_id)) {
        return;
    }

    /* Invalidate and wait until no-one else is using this semaphore */
    obj_core_invalidate((obj_core_t *)sem_id);     /* invalidate the object */

    krhino_sem_del(&sem_id->sem);
    obj_core_deinit((obj_core_t *)sem_id);

    obj_free(aos_sem_class_id, (obj_core_t *)sem_id);
    sem->hdl = NULL;
}

int  aos_sem_close(aos_hdl_t *sem)
{
    int  ret;
    if (sem == NULL) {
        return -EINVAL;
    }

    /* call the common routine with the kernel object ID */
    ret = obj_close(sem->hdl, aos_sem_class_id);
    if (ret == ERROR) {
        return  -EINVAL;
    }

    return 0;
}

int  aos_sem_wait(aos_hdl_t *sem, uint32_t timeout)
{
    kstat_t ret;
    aos_obj_sem_t *obj_sem;

    if (sem == NULL || sem->hdl == NULL) {
        return -EINVAL;
    }

    obj_sem = (aos_obj_sem_t *)(sem->hdl);

    if (obj_verify(obj_sem, aos_sem_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_sem_take(&obj_sem->sem, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_sem_take(&obj_sem->sem, MS2TICK(timeout));
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }  else if ((ret == RHINO_BLK_TIMEOUT) || (ret == RHINO_NO_PEND_WAIT)) {
        return -ETIMEDOUT;
    }
    /* rhino err return is positive */
    return -ret;
}

void aos_sem_signal(aos_hdl_t *sem)
{
    aos_obj_sem_t *obj_sem;
    if (sem == NULL || sem->hdl == NULL) {
        return;
    }

    obj_sem = (aos_obj_sem_t *)(sem->hdl);

    if (obj_verify(obj_sem, aos_sem_class_id) != OK) {
        return;
    }

    krhino_sem_give(&obj_sem->sem);
}

int  aos_sem_is_valid(aos_hdl_t *sem)
{
    if (sem == NULL || sem->hdl == NULL) {
        return 0;
    }
    return 1;
}

void aos_sem_signal_all(aos_hdl_t *sem)
{
    aos_obj_sem_t *obj_sem;
    if (sem == NULL || sem->hdl == NULL) {
        return;
    }

    obj_sem = (aos_obj_sem_t *)(sem->hdl);

    if (obj_verify(obj_sem, aos_sem_class_id) != OK) {
        return ;    /* invalid object */
    }
    krhino_sem_give_all(&obj_sem->sem);
}

int  aos_sem_getvalue(aos_hdl_t *sem, uint32_t *val)
{
    int           ret;

    aos_obj_sem_t *obj_sem;
    if (sem == NULL || sem->hdl == NULL) {
        return -EINVAL;
    }

    /* validate the user's buffer for semaphore value */
    if (val == NULL) {
        return -EINVAL;    /* EBADF? */
    }

    obj_sem = (aos_obj_sem_t *)(sem->hdl);

    if (obj_verify(obj_sem, aos_sem_class_id) != OK) {
        return -EINVAL;;    /* invalid object */
    }

    ret = krhino_sem_count_get(&obj_sem->sem, (sem_count_t *)val);
    /* call the common routine with the kernel object ID */

    return (-ret);
}

int  aos_sem_unlink(const char *name)
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

    status = obj_unlink(name, aos_sem_class_id);

    if (status == ERROR) {
        return -ENOENT;
    }

    return 0;
}
#endif
