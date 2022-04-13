/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <k_api.h>
#include <fcntl.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include "object_api.h"

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static obj_class_t   aos_queue_class;                         /* queue object class */
obj_class_t         *aos_queue_class_id = &aos_queue_class;
typedef struct aos_obj_queue {
    obj_core_t       obj_core;
    kbuf_queue_t     queue;
} aos_obj_queue_t;

static void *obj_queue_create_func(const char *name, va_list vaList)
{
    aos_obj_queue_t    *queue_id;
    void               *buf;
    size_t           size;
    size_t           max;
    int                ret = 0;

    size = va_arg(vaList, size_t);
    max = va_arg(vaList, size_t);

    if ((queue_id = (aos_obj_queue_t *) obj_alloc_extra(aos_queue_class_id, size, &buf)) == NULL) {
        obj_errno = ENOSPC;
        return (NULL);
    }

    if ((name == NULL) || (*name == '\0')) {
        name = "no_name_queue";
    }

    /* create a aos queue */
    ret = krhino_buf_queue_create(&queue_id->queue, name, buf, size, max);
    if (ret != 0) {
        obj_free(aos_queue_class_id, (obj_core_t *) queue_id);
        obj_errno = ENOSPC;

        return (NULL);
    }

    /* initialize the queue object core */
    obj_core_init((obj_core_t *) queue_id, aos_queue_class_id);

    /* mark object memory to be freed during automatic resource reclamation */
    OBJ_SET_DEALLOC_MEM(&queue_id->obj_core);
    /* return the ID */
    return (void *)queue_id;
}

static int obj_queue_destroy_func(void *obj_id, RHINO_BOOL dealloc)
{
    aos_obj_queue_t *queue_id = (aos_obj_queue_t *) obj_id;


    if (obj_verify(obj_id, aos_queue_class_id) != OK) {
        obj_errno = EINVAL;
        return (ERROR);                         /* invalid object */
    }

    /* Invalidate and wait until no-one else is using this queue */
    obj_core_invalidate((obj_core_t *)obj_id);     /* invalidate the object */

    krhino_buf_queue_del(&queue_id->queue);
    /* terminate the queue object and delete the queue */
    obj_core_deinit((obj_core_t *)obj_id);

    if (dealloc) {
        obj_free(aos_queue_class_id, (obj_core_t *)queue_id);
    }
    return (OK);
}

int aos_obj_queue_lib_init(void)
{
    return (class_init(aos_queue_class_id, sizeof(struct aos_obj_queue),
                       (obj_create_func_t) obj_queue_create_func, obj_queue_destroy_func,
                       NULL, NULL, NULL, QUEUE_CLASS_TYPE, 0));
}

int aos_queue_open(aos_hdl_t *queue, const char *name, int flag, size_t size, size_t max_msg)
{
    int  obj_open_mode = 0;   /* flags translated for obj_open */
    aos_obj_queue_t *obj_queue;            /* kernel ID of the queue object */
    int tem_errno;
    /* queue must have a name */
    if ((queue == NULL) || (name == NULL) || (*name == '\0') || strlen(name) > OBJ_NAME_MAX) {
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
    obj_queue = (aos_obj_queue_t *) obj_open(name, obj_open_mode, aos_queue_class_id, size, max_msg);

    if (obj_queue == NULL) {
        /* queue creation error */
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
    queue->hdl = obj_queue;
    return 0;
}

int aos_queue_new(aos_hdl_t *queue, size_t size, size_t max_msg)
{
    aos_obj_queue_t *queue_id;     /* kernel object ID of the new queue */
    int temp_errno;

    if (queue == NULL) {
        return -EINVAL;
    }
    /* create a posix queue */
    queue_id = (aos_obj_queue_t *) obj_open(NULL, OM_CREATE, aos_queue_class_id, size, max_msg);

    if (queue_id == NULL) {
        /* queue creation error */
        if (obj_errno == K_OBJ_ERRNO_OBJ_NAME_CLASH) {
            temp_errno = EEXIST;
        } else if (obj_errno == K_OBJ_ERRNO_OBJ_NOT_FOUND) {
            temp_errno = ENOENT;
        } else {
            temp_errno = ENOSPC;
        }

        return -temp_errno;
    }
    /* write the new queue ID into the caller's buffer.*/
    queue->hdl = (void *)queue_id;

    return 0;
}

int aos_queue_close(aos_hdl_t *queue)
{
    int ret;
    if (queue == NULL) {
        return -EINVAL;
    }

    /* call the common routine with the kernel object ID */
    ret = obj_close(queue->hdl, aos_queue_class_id);
    if (ret == ERROR) {
        return  -EINVAL;
    }

    return 0;
}

void aos_queue_free(aos_hdl_t *queue)
{
    aos_obj_queue_t *queue_id;
    if (queue == NULL || queue->hdl == NULL) {
        return;
    }
    queue_id = queue->hdl;
    if (obj_verify(queue_id, aos_queue_class_id) != OK) {
        /* invalid object */
        return;
    }

    /* this interface is allowed only for unnamed queues */
    if (OBJ_IS_NAMED(queue_id)) {
        return;
    }

    /* Invalidate and wait until no-one else is using this queue */
    obj_core_invalidate((obj_core_t *)queue_id);     /* invalidate the object */

    krhino_buf_queue_del(&queue_id->queue);
    obj_core_deinit((obj_core_t *)queue_id);

    obj_free(aos_queue_class_id, (obj_core_t *)queue_id);
    queue->hdl = NULL;
    return;
}

int aos_queue_send(aos_hdl_t *queue, void *msg, size_t size)
{
    int ret = 0;
    aos_obj_queue_t *obj_queue;

    if ((queue == NULL) || (msg == NULL)) {
        return -EINVAL;
    }

    obj_queue = (aos_obj_queue_t *)(queue->hdl);
    if (obj_queue == NULL) {
        return -EINVAL;
    }

    if (obj_verify(obj_queue, aos_queue_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    ret = krhino_buf_queue_send(&obj_queue->queue, msg, size);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return -ret;
}

int aos_queue_recv(aos_hdl_t *queue, uint32_t ms, void *msg, size_t *size)
{
    int ret;
    aos_obj_queue_t *obj_queue;

    if ((queue == NULL) || (msg == NULL)) {
        return -EINVAL;
    }

    obj_queue = (aos_obj_queue_t *)(queue->hdl);
    if (obj_queue == NULL) {
        return -EINVAL;
    }

    if (obj_verify(obj_queue, aos_queue_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    if (ms == AOS_WAIT_FOREVER) {
        ret = krhino_buf_queue_recv(&obj_queue->queue, RHINO_WAIT_FOREVER, msg, size);
    } else {
        ret = krhino_buf_queue_recv(&obj_queue->queue, MS2TICK(ms), msg, size);
    }

    if (ret == RHINO_SUCCESS) {
        return 0;
    }  else if ((ret == RHINO_BLK_TIMEOUT) || (ret == RHINO_NO_PEND_WAIT)) {
        return -ETIMEDOUT;
    }

    return -ret;
}

int aos_queue_get_count(aos_hdl_t *queue)
{
    kbuf_queue_info_t info;
    int ret;
    aos_obj_queue_t *obj_queue;

    if ((queue == NULL)) {
        return -EINVAL;
    }

    obj_queue = (aos_obj_queue_t *)(queue->hdl);
    if (obj_queue == NULL) {
        return -EINVAL;
    }
    if (obj_verify(obj_queue, aos_queue_class_id) != OK) {
        return -EINVAL;   /* invalid object */
    }

    ret = krhino_buf_queue_info_get(&obj_queue->queue, &info);

    return ret == RHINO_SUCCESS ? info.cur_num : -EPERM;
}

int aos_queue_is_valid(aos_hdl_t *queue)
{

    if (queue == NULL || queue->hdl == NULL) {
        return  0;
    }

    return 1;
}

int  aos_queue_unlink(const char *name)
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

    status = obj_unlink(name, aos_queue_class_id);

    if (status == ERROR) {
        return -ENOENT;
    }

    return 0;
}
