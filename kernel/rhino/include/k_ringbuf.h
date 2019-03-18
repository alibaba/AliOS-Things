/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_RINGBUF_H
#define K_MM_RINGBUF_H

#define RINGBUF_TYPE_FIX 0
#define RINGBUF_TYPE_DYN 1

typedef struct {
    uint8_t *buf;
    uint8_t *end;
    uint8_t *head;
    uint8_t *tail;
    size_t   freesize;
    size_t   type;
    size_t   blk_size;
} k_ringbuf_t;

RHINO_INLINE kstat_t ringbuf_queue_push(k_ringbuf_t *p_ringbuf, void *data, size_t len)
{
    if (p_ringbuf->tail == p_ringbuf->end) {
        p_ringbuf->tail = p_ringbuf->buf;
    }

    memcpy(p_ringbuf->tail, data, p_ringbuf->blk_size);
    p_ringbuf->tail += p_ringbuf->blk_size;

    return RHINO_SUCCESS;
}

RHINO_INLINE kstat_t ringbuf_queue_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen)
{
    if (p_ringbuf->head == p_ringbuf->end) {
        p_ringbuf->head = p_ringbuf->buf;
    }

    memcpy(pdata, p_ringbuf->head, p_ringbuf->blk_size);
    p_ringbuf->head += p_ringbuf->blk_size;

    if (plen != NULL) {
        *plen = p_ringbuf->blk_size;
    }

    return RHINO_SUCCESS;
}

#endif /* K_RINGBUF_H */

