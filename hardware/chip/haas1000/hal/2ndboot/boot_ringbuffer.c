/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "boot_ringbuffer.h"

int32_t boot_ringbuff_reset(boot_ringbuff_t *rb)
{
	if (!rb) {
		return -1;
	}

	rb->rd_ptr = rb->buffer;
	rb->wr_ptr = rb->buffer;
	rb->dirty_size = 0;
	rb->free_size = rb->buffer_end - rb->buffer;
	return 0;
}

int32_t boot_ringbuff_init(boot_ringbuff_t *rb,	uint8_t *buffer, int32_t size)
{
	if (!rb) {
		return -1;
	}

	if (!buffer) {
		return -1;
	}

	if (size <= 4) {
		return -1;
	}

	rb->buffer = buffer;
	rb->buffer_end = rb->buffer + size;
	boot_ringbuff_reset(rb);
	return 0;
}

int32_t boot_ringbuff_freesize(boot_ringbuff_t *rb)
{
	if (!rb) {
		return -1;
	}
	return rb->free_size;
}

int32_t boot_ringbuff_dirtysize(boot_ringbuff_t *rb)
{
	if (!rb) {
		return -1;
	}
	return rb->dirty_size;
}

int32_t boot_ringbuff_fill(boot_ringbuff_t *rb, uint8_t *buffer, int32_t size)
{
	if (!rb) {
		return -1;
	}

	if (size > rb->free_size || size <= 0) {
		return -1;
	}

	if ((rb->buffer_end - rb->wr_ptr >= size &&
		rb->wr_ptr >= rb->rd_ptr) ||
		rb->wr_ptr < rb->rd_ptr) {
		memcpy(rb->wr_ptr, buffer, size);
		rb->wr_ptr += size;
		rb->dirty_size += size;
		rb->free_size -= size;
		if (rb->wr_ptr >= rb->buffer_end)
			rb->wr_ptr = rb->buffer;
	} else if (rb->buffer_end - rb->wr_ptr < size &&
		rb->wr_ptr >= rb->rd_ptr) {
		int temp = rb->buffer_end - rb->wr_ptr;
		memcpy(rb->wr_ptr, buffer, temp);
		rb->wr_ptr = rb->buffer;
		memcpy(rb->wr_ptr, buffer + temp, size - temp);
		rb->dirty_size += size;
		rb->free_size -= size;
		rb->wr_ptr += size - temp;
	} else {
		return -1;
	}

	return size;
}

int32_t boot_ringbuff_read(boot_ringbuff_t *rb,	uint8_t *buffer, int32_t size)
{
	if (!rb) {
		return -1;
	}

	if (size > rb->dirty_size || size <= 0) {
		return -1;
	}

	if (rb->rd_ptr < rb->wr_ptr &&
		rb->wr_ptr - rb->rd_ptr >= size) {
		memcpy(buffer, rb->rd_ptr, size);
		rb->dirty_size -= size;
		rb->free_size += size;
		rb->rd_ptr += size;
	} else if (rb->rd_ptr >= rb->wr_ptr) {
		if (rb->buffer_end - rb->rd_ptr >= size) {
			memcpy(buffer, rb->rd_ptr, size);
			rb->dirty_size -= size;
			rb->free_size += size;
			rb->rd_ptr += size;
			if (rb->rd_ptr >= rb->buffer_end)
				rb->rd_ptr = rb->buffer;
		} else {
			int temp = rb->buffer_end - rb->rd_ptr;
			memcpy(buffer, rb->rd_ptr, temp);
			rb->rd_ptr = rb->buffer;
			memcpy(buffer + temp, rb->rd_ptr, size - temp);
			rb->dirty_size -= size;
			rb->free_size += size;
			rb->rd_ptr += size - temp;
		}
	} else {
		return -1;
	}

	return size;
}



