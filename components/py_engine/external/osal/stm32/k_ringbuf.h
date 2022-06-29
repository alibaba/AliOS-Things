/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_MM_RINGBUF_H
#define K_MM_RINGBUF_H

#ifdef __cplusplus
extern "C" {
#endif
#include "aos_errno.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
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


int ringbuf_init(k_ringbuf_t *p_ringbuf, void *buf, size_t len, size_t type, size_t block_size);
int ringbuf_reset(k_ringbuf_t *p_ringbuf);
int ringbuf_push(k_ringbuf_t *p_ringbuf, void *data, size_t len);
int ringbuf_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen);
int ringbuf_is_full(k_ringbuf_t *p_ringbuf);
int ringbuf_is_empty(k_ringbuf_t *p_ringbuf);


// /**
//  * Push a msg to the ring.
//  *
//  * @param[in] p_ringbuf pointer to the ringbuf
//  * @param[in] data      data address
//  * @param[in] len       data length
//  *
//  * @return the operation status, RHINO_SUCCESS is OK, others is error
//  */
// inline int ringbuf_queue_push(k_ringbuf_t *p_ringbuf, void *data, size_t len)
// {
//     if (p_ringbuf->tail == p_ringbuf->end) {
//         p_ringbuf->tail = p_ringbuf->buf;
//     }

//     memcpy(p_ringbuf->tail, data, p_ringbuf->blk_size);
//     p_ringbuf->tail += p_ringbuf->blk_size;

//     return 0;
// }

// /**
//  * Pop a msg from the ring.
//  *
//  * @param[in] p_ringbuf pointer to the ringbuf
//  * @param[in] pdata     data address
//  * @param[out] plen     data length
//  *
//  * @return the operation status, RHINO_SUCCESS is OK, others is error
//  */
// inline int ringbuf_queue_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen)
// {
//     if (p_ringbuf->head == p_ringbuf->end) {
//         p_ringbuf->head = p_ringbuf->buf;
//     }

//     memcpy(pdata, p_ringbuf->head, p_ringbuf->blk_size);
//     p_ringbuf->head += p_ringbuf->blk_size;

//     if (plen != NULL) {
//         *plen = p_ringbuf->blk_size;
//     }

//     return 0;
// }

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* K_RINGBUF_H */

