/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifdef RHINO_CONFIG_IPC

#include <stdatomic.h>
#include "k_api.h"
#include "utask.h"
#include "debug_api.h"

#include "kipc_queue.h"
#include "kipc_perf.h"
#include "kipc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int ipc_read_msg_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
{
    struct ipc_msg_desc_t desc;
    int first = q->read.first;
    int pos = first + q->read.blk_size;;

    if (first == q->write.second) {
        return -RHINO_IPC_QUEUE_EMPTY;
    }

    aos_ipc_memcpy(&desc, q->read.buf + first, q->read.blk_size);
    if (dlen < desc.len) {
        return -RHINO_IPC_BUF_LIMITED;
    }

    return pos >= q->read.tsize ? pos - q->read.tsize : pos;
}

int ipc_read_msg_cpy(ipc_queue_t *q, int start,
                     uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    struct ipc_msg_desc_t desc;

    aos_ipc_memcpy(&desc, q->read.buf + start, q->read.blk_size);
    aos_ipc_memcpy(data, desc.buf, desc.len);
    krhino_mm_free(desc.buf);
    *dlen = desc.len;

    return RHINO_SUCCESS;
}

int ipc_read_msg_wakeup(ipc_queue_t *q)
{
#if 0
    int second = q->read.second;
    int next;

    if ((second < q->write.second && second + IPC_CACHE_LINE_SIZE >= q->write.second)) {
        /* packet is ready for read is limited */
        return -RHINO_IPC_QUEUE_EMPTY;
    }
    next = second + IPC_CACHE_LINE_SIZE;
    if (second > q->write.second && next >= q->read.tsize &&
        next >= q->write.second + IPC_CACHE_LINE_SIZE) {
        return -RHINO_IPC_QUEUE_EMPTY;
    }

    return RHINO_SUCCESS;
#endif

    return q->write.second != q->read.first ? RHINO_SUCCESS : -RHINO_RINGBUF_EMPTY;
}

int ipc_write_msg_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
{
    int first = q->write.first;
    int pos = first + q->write.blk_size;

    if (first < q->read.second && pos >= q->read.second) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    if (first >= q->read.second && pos >= q->read.tsize &&
        pos >= q->read.second + q->read.tsize) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    if (dst_buf != NULL && *dst_buf == NULL) {
        *dst_buf = (uint8_t *)krhino_mm_alloc(dlen);
        if (*dst_buf == NULL) {
            return -RHINO_NO_MEM;
        }
    }

    return pos >= q->write.tsize ? pos - q->write.tsize : pos;
}

int ipc_write_msg_cpy(ipc_queue_t *q, int start,
                      uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    struct ipc_msg_desc_t desc = {
        .len = *dlen,
        .buf = dst_buf
    };

    aos_ipc_memcpy(dst_buf, data, *dlen);
    aos_ipc_memcpy(q->write.buf + start, &desc, q->write.blk_size);

    return RHINO_SUCCESS;
}

int ipc_write_msg_wakeup(ipc_queue_t *q)
{
    int first = q->write.first;
    int next;

    if (first < q->read.second && first + q->write.blk_size > q->read.second) {
        /* packet is ready for read is limited */
        return -RHINO_IPC_QUEUE_FULL;
    }
    next = first + q->write.blk_size;
    if (first > q->read.second && next >= q->write.tsize &&
        next >= q->read.second + q->read.blk_size) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    return RHINO_SUCCESS;
}

int ipc_msg_delete(ipc_queue_t *q)
{
    int start;
    struct ipc_msg_desc_t desc;

    for (start = q->read.second; start != q->write.second;) {
        aos_ipc_memcpy(&desc, q->read.buf + start, q->read.blk_size);
        krhino_mm_free(desc.buf);
        start = start + q->read.blk_size;
        if (start >= q->read.tsize) {
            start -= q->read.tsize;
        }
    }
    krhino_mm_free(q->read.buf);

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif
