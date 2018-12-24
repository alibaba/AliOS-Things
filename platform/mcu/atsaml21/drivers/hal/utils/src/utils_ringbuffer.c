/**
 * \file
 *
 * \brief Ringbuffer functionality implementation.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include "utils_ringbuffer.h"

/**
 * \brief Ringbuffer init
 */
int32_t ringbuffer_init(struct ringbuffer *const rb, void *buf, uint32_t size)
{
	ASSERT(rb && buf && size);

	/*
	 * buf size must be aligned to power of 2
	 */
	if ((size & (size - 1)) != 0) {
		return ERR_INVALID_ARG;
	}

	/* size - 1 is faster in calculation */
	rb->size        = size - 1;
	rb->read_index  = 0;
	rb->write_index = rb->read_index;
	rb->buf         = (uint8_t *)buf;

	return ERR_NONE;
}

/**
 * \brief Get one byte from ringbuffer
 *
 */
int32_t ringbuffer_get(struct ringbuffer *const rb, uint8_t *data)
{
	ASSERT(rb && data);

	if (rb->write_index != rb->read_index) {
		*data = rb->buf[rb->read_index & rb->size];
		rb->read_index++;
		return ERR_NONE;
	}

	return ERR_NOT_FOUND;
}

/**
 * \brief Put one byte to ringbuffer
 *
 */
int32_t ringbuffer_put(struct ringbuffer *const rb, uint8_t data)
{
	ASSERT(rb);

	rb->buf[rb->write_index & rb->size] = data;

	/*
	 * buffer full strategy: new data will overwrite the oldest data in
	 * the buffer
	 */
	if ((rb->write_index - rb->read_index) > rb->size) {
		rb->read_index = rb->write_index - rb->size;
	}

	rb->write_index++;

	return ERR_NONE;
}

/**
 * \brief Return the element number of ringbuffer
 */
uint32_t ringbuffer_num(const struct ringbuffer *const rb)
{
	ASSERT(rb);

	return rb->write_index - rb->read_index;
}

/**
 * \brief Flush ringbuffer
 */
uint32_t ringbuffer_flush(struct ringbuffer *const rb)
{
	ASSERT(rb);

	rb->read_index = rb->write_index;

	return ERR_NONE;
}
