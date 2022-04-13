/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef KIPC_QUEUE_H
#define KIPC_QUEUE_H

#include <stdint.h>
#include <stddef.h>

#include "k_obj.h"
#include "kipc_perf.h"

#ifndef RHINO_CACHE_LINE_SIZE
    #define IPC_CACHE_LINE_SIZE    (32)
#else
    #define IPC_CACHE_LINE_SIZE    (RHINO_CACHE_LINE_SIZE)
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

struct ipc_queue_t;

typedef struct {
    /* const attribute of queue */
    int tsize;    /* total size of queue's buffer */
    int blk_size; /* size of element used for FIFO */
    uint8_t *buf; /* buffer of queue */
    /* compute next of operation */
    int (*next)(struct ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
    /* copy packet to or from buffer of queue for en_queue or de_enqueu */
    int (*cpy)(struct ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
    /* it's need to wakeup peer or not */
    int (*wakeup)(struct ipc_queue_t *q);
    int (*delete)(struct ipc_queue_t *q);
    int dump0[IPC_CACHE_LINE_SIZE / sizeof(int) - 7];  /* align with IPC_CACHE_LINE_SIZE */

    /* variable attribute of queue */
    int first;
    int access;   /* access count of read or write */
    int state;    /* state of ipc queue */
    int dump1[IPC_CACHE_LINE_SIZE / sizeof(int) - 3];  /* align with IPC_CACHE_LINE_SIZE */
    int second;
    int dump2[IPC_CACHE_LINE_SIZE / sizeof(int) - 1];  /* align with IPC_CACHE_LINE_SIZE */
    blk_obj_t blk_obj;  /* blk list */
} __attribute__((aligned(IPC_CACHE_LINE_SIZE))) ipc_queue_desc_t;

typedef struct ipc_queue_t {
    ipc_queue_desc_t read;
    ipc_queue_desc_t write;
#if IPC_PERF_STATISTIC_ENALBE
    struct ipc_perf_stats_t perf;
#endif  /* end of IPC_PERF_STATISTIC_ENALBE */
} ipc_queue_t;

typedef struct ipc_msg_desc_t {
    size_t len;
    uint8_t *buf;
} ipc_msg_desc_t;

enum {
    KIPC_QUEUE_TYP_STREAM = 0,
    KIPC_QUEUE_TYP_MSG,
    KIPC_QUEUE_TYP_FIFO,
};

enum {
    KIPC_PEND_CLR_RX = 0x01,
    KIPC_PEND_CLR_TX = 0x02,
};

enum {
    /* one means that the prefetch is preparing for a write to the memory address and zero,
     * the default, means that the prefetch is preparing for a read
     */
    KIPC_PREFETCH_FOR_RD = 0,
    KIPC_PREFETCH_FOR_WR = 1,

    /* zero means that the data has no temporal locality, so it need not be left in the cache after the access.
     * A value of three means that the data has a high degree of temporal locality and should be left in all levels of cache
     * possible. Values of one and two mean, respectively, a low or moderate degree of temporal locality
     */
    KIPC_PREFETCH_CACHE_LOCAL_LVL_0 = 0,
    KIPC_PREFETCH_CACHE_LOCAL_LVL_1 = 1,
    KIPC_PREFETCH_CACHE_LOCAL_LVL_2 = 2,
    KIPC_PREFETCH_CACHE_LOCAL_LVL_3 = 3
};

int ipc_read_stream_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_read_stream_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_read_stream_wakeup(ipc_queue_t *q);
int ipc_stream_delete(ipc_queue_t *q);

int ipc_write_stream_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_write_stream_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_write_stream_wakeup(ipc_queue_t *q);

int ipc_read_msg_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_read_msg_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_read_msg_wakeup(ipc_queue_t *q);
int ipc_msg_delete(ipc_queue_t *q);

int ipc_write_msg_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_write_msg_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_write_msg_wakeup(ipc_queue_t *q);

int ipc_read_fifo_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_read_fifo_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_read_fifo_wakeup(ipc_queue_t *q);
int ipc_fifo_delete(ipc_queue_t *q);

int ipc_write_fifo_next(ipc_queue_t *q, uint8_t **dst_buf, size_t dlen);
int ipc_write_fifo_cpy(ipc_queue_t *q, int start, uint8_t *data, size_t *dlen, uint8_t *dst_buf);
int ipc_write_fifo_wakeup(ipc_queue_t *q);

kstat_t kipc_queue_recv(ipc_queue_t *q, void *msg, size_t *size, tick_t ticks);
kstat_t kipc_queue_send(ipc_queue_t *q, void *msg, size_t msg_size, tick_t ticks);

kstat_t kipc_queue_create(ipc_queue_t **queue, int type, size_t tlen, size_t blk_size);
kstat_t kipc_queue_enable(ipc_queue_t *queue, int flag);
kstat_t kipc_queue_disable(ipc_queue_t *queue, int flag);
kstat_t kipc_queue_del(ipc_queue_t *queue);

int kipc_queue_full(ipc_queue_t *q);
int kipc_queue_empty(ipc_queue_t *q);

#endif