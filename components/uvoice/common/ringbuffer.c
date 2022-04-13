/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"

#include "uvoice_common.h"


int32_t uvoice_ringbuff_reset(uvoice_ringbuff_t *rb)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	rb->rd_ptr = rb->buffer;
	rb->wr_ptr = rb->buffer;
	rb->dirty_size = 0;
	rb->free_size = rb->buffer_end - rb->buffer;
	return 0;
}

int32_t uvoice_ringbuff_init(uvoice_ringbuff_t *rb,
	uint8_t *buffer, int32_t size)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	if (!buffer) {
		M_LOGE("buffer null !\n");
		return -1;
	}

	if (size <= 4) {
		M_LOGE("size %d invalid !\n", size);
		return -1;
	}

	rb->buffer = buffer;
	rb->buffer_end = rb->buffer + size;
	uvoice_ringbuff_reset(rb);
	return 0;
}

int32_t uvoice_ringbuff_freesize(uvoice_ringbuff_t *rb)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}
	return rb->free_size;
}

int32_t uvoice_ringbuff_dirtysize(uvoice_ringbuff_t *rb)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}
	return rb->dirty_size;
}

int32_t uvoice_ringbuff_fill(uvoice_ringbuff_t *rb,
	uint8_t *buffer, int32_t size)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	if (size > rb->free_size || size <= 0) {
		M_LOGE("size %d invalid !\n", size);
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
		M_LOGE("error ! buffer %08x buffer_end %08x rd_ptr %08x wr_ptr %08x dirty %d free %d\n",
			(unsigned int)rb->buffer,
			(unsigned int)rb->buffer_end,
			(unsigned int)rb->rd_ptr,
			(unsigned int)rb->wr_ptr, rb->dirty_size, rb->free_size);
		return -1;
	}

	return size;
}

int32_t uvoice_ringbuff_read(uvoice_ringbuff_t *rb,
	uint8_t *buffer, int32_t size)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	if (size > rb->dirty_size || size <= 0) {
		M_LOGE("size %d invalid !\n", size);
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
		M_LOGE("error ! buffer %08x buffer_end %08x rd_ptr %08x wr_ptr %08x dirty %d free %d\n",
			(unsigned int)rb->buffer,
			(unsigned int)rb->buffer_end,
			(unsigned int)rb->rd_ptr,
			(unsigned int)rb->wr_ptr, rb->dirty_size, rb->free_size);
		return -1;
	}

	return size;
}

int32_t uvoice_ringbuff_drop(uvoice_ringbuff_t *rb, int32_t size)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	if (size > rb->dirty_size || size <= 0) {
		M_LOGE("size %d invalid !\n", size);
		return -1;
	}

	if (rb->wr_ptr - rb->rd_ptr >= size) {
		rb->dirty_size -= size;
		rb->free_size += size;
		rb->rd_ptr += size;
		if (rb->rd_ptr >= rb->buffer_end)
			rb->rd_ptr = rb->buffer;
	} else if (rb->rd_ptr >= rb->wr_ptr) {
		if (rb->buffer_end - rb->rd_ptr >= size) {
			rb->dirty_size -= size;
			rb->free_size += size;
			rb->rd_ptr += size;
			if (rb->rd_ptr >= rb->buffer_end)
				rb->rd_ptr = rb->buffer;
		} else {
			rb->rd_ptr = rb->buffer +
				(size - (rb->buffer_end - rb->rd_ptr));
			rb->dirty_size -= size;
			rb->free_size += size;
		}
	} else {
		M_LOGE("error ! buffer %08x buffer_end %08x rd_ptr %08x wr_ptr %08x dirty %d free %d\n",
			(unsigned int)rb->buffer,
			(unsigned int)rb->buffer_end,
			(unsigned int)rb->rd_ptr,
			(unsigned int)rb->wr_ptr, rb->dirty_size, rb->free_size);
		return -1;
	}

	return 0;
}

int32_t uvoice_ringbuff_back(uvoice_ringbuff_t *rb, int32_t size)
{
	if (!rb) {
		M_LOGE("rb null !\n");
		return -1;
	}

	if (size > rb->free_size || size <= 0) {
		M_LOGE("size %d invaid !\n", size);
		return -1;
	}

	if (rb->wr_ptr >= rb->rd_ptr) {
		if (rb->rd_ptr - rb->buffer >= size) {
			rb->rd_ptr -= size;
			rb->free_size -= size;
			rb->dirty_size += size;
		} else {
			rb->rd_ptr = rb->buffer_end -
				(size - (rb->rd_ptr - rb->buffer));
			rb->free_size -= size;
			rb->dirty_size += size;
		}
	} else {
		if (rb->rd_ptr - rb->wr_ptr >= size) {
			rb->rd_ptr -= size;
			rb->free_size -= size;
			rb->dirty_size += size;
		} else {
			M_LOGE("error ! buffer %08x buffer_end %08x rd_ptr %08x wr_ptr %08x dirty %d free %d\n",
				(unsigned int)rb->buffer,
				(unsigned int)rb->buffer_end,
				(unsigned int)rb->rd_ptr,
				(unsigned int)rb->wr_ptr, rb->dirty_size, rb->free_size);
			return -1;
		}
	}

	return 0;
}

