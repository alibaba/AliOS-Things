/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_RINGBUFFER_H__
#define __UVOICE_RINGBUFFER_H__


int32_t uvoice_ringbuff_reset(uvoice_ringbuff_t *rb);
int32_t uvoice_ringbuff_init(uvoice_ringbuff_t *rb, uint8_t *buffer, int32_t size);
int32_t uvoice_ringbuff_freesize(uvoice_ringbuff_t *rb);
int32_t uvoice_ringbuff_dirtysize(uvoice_ringbuff_t *rb);
int32_t uvoice_ringbuff_fill(uvoice_ringbuff_t *rb, uint8_t *buffer, int32_t size);
int32_t uvoice_ringbuff_read(uvoice_ringbuff_t *rb, uint8_t *buffer, int32_t size);
int32_t uvoice_ringbuff_drop(uvoice_ringbuff_t *rb, int32_t size);
int32_t uvoice_ringbuff_back(uvoice_ringbuff_t *rb, int32_t size);


#endif /* __UVOICE_RINGBUFFER_H__ */
