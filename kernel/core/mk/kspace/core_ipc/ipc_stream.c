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

#define IPC_STREAM_HEAD_LEN (sizeof(size_t))

#ifdef __cplusplus
extern "C" {
#endif

int ipc_read_stream_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
{
    int pos;
    size_t len;
    int first = q->read.first;
    int rest = q->read.tsize - first;

    if (first == q->write.second) {
        return -RHINO_IPC_QUEUE_EMPTY;
    }

    if (rest >= IPC_STREAM_HEAD_LEN) {
        aos_ipc_memcpy(&len, q->read.buf + first, IPC_STREAM_HEAD_LEN);
    } else {
        aos_ipc_memcpy(&len, q->read.buf + first, rest);
        aos_ipc_memcpy((char *)&len + rest, q->read.buf, IPC_STREAM_HEAD_LEN - rest);
    }

    if (dlen < len) {
        return -RHINO_IPC_BUF_LIMITED;
    }

    pos = first + IPC_STREAM_HEAD_LEN + len;

    return pos >= q->read.tsize ? pos - q->read.tsize : pos;
}

int ipc_read_stream_cpy(ipc_queue_t *q, int start,
                        uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    size_t len;
    int rest = q->read.tsize - start;
    int pos = start + IPC_STREAM_HEAD_LEN;

    if (pos >= q->read.tsize) {
        pos -= q->read.tsize;
    }

    if (rest >= IPC_STREAM_HEAD_LEN) {
        aos_ipc_memcpy(&len, q->read.buf + start, IPC_STREAM_HEAD_LEN);
    } else {
        aos_ipc_memcpy(&len, q->read.buf + start, rest);
        aos_ipc_memcpy((char *)&len + rest, q->read.buf, IPC_STREAM_HEAD_LEN - rest);
    }

    rest = q->read.tsize - pos;
    if (rest >= len) {
        aos_ipc_memcpy(data, q->read.buf + pos, len);
    } else {
        aos_ipc_memcpy(data, q->read.buf + pos, rest);
        aos_ipc_memcpy(data + rest, q->read.buf, len - rest);
    }
    *dlen = len;

    return RHINO_SUCCESS;
}

int ipc_read_stream_wakeup(ipc_queue_t *q)
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

int ipc_write_stream_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen)
{
    int pos;
    int first = q->write.first;
    int pkt_len = IPC_STREAM_HEAD_LEN + dlen;

    if (pkt_len >= q->write.tsize) {
        return -RHINO_IPC_BUF_LIMITED;
    }

    pos = first + pkt_len;

    if (first < q->read.second && pos >= q->read.second) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    if (first >= q->read.second && pos >= q->read.tsize &&
        pos >= q->read.second + q->read.tsize) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    return pos >= q->write.tsize ? pos - q->write.tsize : pos;
}

int ipc_write_stream_cpy(ipc_queue_t *q, int start,
                         uint8_t *data, size_t *dlen, uint8_t *dst_buf)
{
    int rest = q->write.tsize - start;
    int pos = start + IPC_STREAM_HEAD_LEN;

    if (pos >= q->write.tsize) {
        pos -= q->write.tsize;
    }

    if (rest >= IPC_STREAM_HEAD_LEN) {
        aos_ipc_memcpy(q->write.buf + start, dlen, IPC_STREAM_HEAD_LEN);
    } else {
        aos_ipc_memcpy(q->write.buf + start, dlen, rest);
        aos_ipc_memcpy(q->write.buf, (char *)dlen + rest, IPC_STREAM_HEAD_LEN - rest);
    }

    rest = q->write.tsize - pos;
    if (rest > *dlen) {
        aos_ipc_memcpy(q->write.buf + pos, data, *dlen);
    } else {
        aos_ipc_memcpy(q->write.buf + pos, data, rest);
        aos_ipc_memcpy(q->write.buf, data + rest, *dlen - rest);
    }

    return RHINO_SUCCESS;
}

int ipc_write_stream_wakeup(ipc_queue_t *q)
{
    int first = q->write.first;
    int next;

    if (first < q->read.second && first + IPC_STREAM_HEAD_LEN > q->read.second) {
        /* packet is ready for read is limited */
        return -RHINO_IPC_QUEUE_FULL;
    }
    next = first + IPC_STREAM_HEAD_LEN;
    if (first > q->read.second && next >= q->read.tsize &&
        next >= q->read.second + IPC_STREAM_HEAD_LEN) {
        return -RHINO_IPC_QUEUE_FULL;
    }

    return RHINO_SUCCESS;
}

int ipc_stream_delete(ipc_queue_t *q)
{
    krhino_mm_free(q->read.buf);

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif
