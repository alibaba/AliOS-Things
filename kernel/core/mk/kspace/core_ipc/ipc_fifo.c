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

int ipc_read_fifo_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
{
    int first = q->read.first;
    int pos = first + q->read.blk_size;

    if (first == q->write.second) {
        return -RHINO_IPC_QUEUE_EMPTY;
    }

    if (dlen < q->read.blk_size) {
        return -RHINO_IPC_BUF_LIMITED;
    }

    return pos >= q->read.tsize ? pos - q->read.tsize : pos;
}

int ipc_read_fifo_cpy(ipc_queue_t *q, int start,
                      uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    aos_ipc_memcpy(data, q->read.buf + start, q->read.blk_size);
    *dlen = q->read.blk_size;

    return RHINO_SUCCESS;
}

int ipc_read_fifo_wakeup(ipc_queue_t *q)
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
#endif

    return q->write.second != q->read.first ? RHINO_SUCCESS : -RHINO_RINGBUF_EMPTY;
}

int ipc_write_fifo_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
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

    return pos >= q->write.tsize ? pos - q->write.tsize : pos;
}

int ipc_write_fifo_cpy(ipc_queue_t *q, int start,
                       uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    aos_ipc_memcpy(q->write.buf + start, data, q->write.blk_size);
    return RHINO_SUCCESS;
}

int ipc_write_fifo_wakeup(ipc_queue_t *q)
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

int ipc_fifo_delete(ipc_queue_t *q)
{
    krhino_mm_free(q->read.buf);

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif
