/**
 * @file k_ringbuf.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_MM_RINGBUF_H
#define K_MM_RINGBUF_H

/** @addtogroup aos_rhino ringbuf
 *  Ringbuf is a FIFO communication mechanism.
 *
 *  @{
 */

/**
 * The msg size of each push and pop operation is fixed, and it is determined at initialization
 */
#define RINGBUF_TYPE_FIX 0
/**
 * The msg size of each push and pop operation is not fixed
 */
#define RINGBUF_TYPE_DYN 1

/**
 * ringbuf object
 */
typedef struct {
    uint8_t *buf;
    uint8_t *end;
    uint8_t *head;
    uint8_t *tail;
    size_t   freesize;
    size_t   type;      /**< RINGBUF_TYPE_FIX or RINGBUF_TYPE_DYN */
    size_t   blk_size;
} k_ringbuf_t;

/**
 * Push a msg to the ring.
 *
 * @param[in] p_ringbuf pointer to the ringbuf
 * @param[in] data      data address
 * @param[in] len       data length
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
RHINO_INLINE kstat_t ringbuf_queue_push(k_ringbuf_t *p_ringbuf, void *data, size_t len)
{
    if (p_ringbuf->tail == p_ringbuf->end) {
        p_ringbuf->tail = p_ringbuf->buf;
    }

    memcpy(p_ringbuf->tail, data, p_ringbuf->blk_size);
    p_ringbuf->tail += p_ringbuf->blk_size;

    return RHINO_SUCCESS;
}

/**
 * Pop a msg from the ring.
 *
 * @param[in] p_ringbuf pointer to the ringbuf
 * @param[in] pdata     data address
 * @param[out] plen     data length
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
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

/** @} */

#endif /* K_RINGBUF_H */

