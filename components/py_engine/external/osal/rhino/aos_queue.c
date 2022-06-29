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

#if (RHINO_CONFIG_BUF_QUEUE > 0)

aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(kbuf_queue_t) + ((buf == NULL) ? size : 0);
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (buf == NULL) ? ((void *)q + sizeof(kbuf_queue_t)) : buf;

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msg, uint32_t options)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    (void)options;
    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(kbuf_queue_t) + size;
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (void *)q + sizeof(kbuf_queue_t);

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msg);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL || *queue == NULL) {
        return;
    }

    krhino_buf_queue_del((kbuf_queue_t *)*queue);

    aos_free(*queue);

    *queue = NULL;
}

aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    int ret;

    CHECK_HANDLE(queue);

    ret = krhino_buf_queue_send((kbuf_queue_t *)*queue, msg, size);
    return rhino2stderrno(ret);
}

aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg,
                   size_t *size)
{
    int ret;

    CHECK_HANDLE(queue);
    if (ms == AOS_WAIT_FOREVER) {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, RHINO_WAIT_FOREVER, msg, size);
    } else {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, MS2TICK(ms), msg, size);
    }
    return rhino2stderrno(ret);
}

bool aos_queue_is_valid(aos_queue_t *queue)
{
    kbuf_queue_t *k_queue;

    if (queue == NULL) {
        return false;
    }

    k_queue = (kbuf_queue_t *)*queue;

    if (k_queue == NULL) {
        return false;
    }

    if (k_queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        return false;
    }

    return true;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue)) {
        return NULL;
    }

    return ((kbuf_queue_t *)*queue)->buf;
}

aos_status_t aos_queue_get_count(aos_queue_t *queue)
{
    kbuf_queue_info_t info;
    CHECK_HANDLE(queue);
    kstat_t ret = krhino_buf_queue_info_get((kbuf_queue_t *)*queue, &info);

    return ret == RHINO_SUCCESS ? info.cur_num : -EPERM;
}
#endif



