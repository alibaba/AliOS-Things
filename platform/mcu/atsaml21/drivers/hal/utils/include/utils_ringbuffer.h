/**
 * \file
 *
 * \brief Ringbuffer declaration.
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
#ifndef _UTILS_RINGBUFFER_H_INCLUDED
#define _UTILS_RINGBUFFER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_utils_ringbuffer
 *
 * @{
 */

#include "compiler.h"
#include "utils_assert.h"

/**
 * \brief Ring buffer element type
 */
struct ringbuffer {
	uint8_t *buf;         /** Buffer base address */
	uint32_t size;        /** Buffer size */
	uint32_t read_index;  /** Buffer read index */
	uint32_t write_index; /** Buffer write index */
};

/**
 * \brief Ring buffer init
 *
 * \param[in] rb The pointer to a ring buffer structure instance
 * \param[in] buf Space to store the data
 * \param[in] size The buffer length, must be aligned with power of 2
 *
 * \return ERR_NONE on success, or an error code on failure.
 */
int32_t ringbuffer_init(struct ringbuffer *const rb, void *buf, uint32_t size);

/**
 * \brief Get one byte from ring buffer, the user needs to handle the concurrent
 * access on buffer via put/get/flush
 *
 * \param[in] rb The pointer to a ring buffer structure instance
 * \param[in] data One byte space to store the read data
 *
 * \return ERR_NONE on success, or an error code on failure.
 */
int32_t ringbuffer_get(struct ringbuffer *const rb, uint8_t *data);

/**
 * \brief Put one byte to ring buffer, the user needs to handle the concurrent access
 * on buffer via put/get/flush
 *
 * \param[in] rb The pointer to a ring buffer structure instance
 * \param[in] data One byte data to be put into ring buffer
 *
 * \return ERR_NONE on success, or an error code on failure.
 */
int32_t ringbuffer_put(struct ringbuffer *const rb, uint8_t data);

/**
 * \brief Return the element number of ring buffer
 *
 * \param[in] rb The pointer to a ring buffer structure instance
 *
 * \return The number of elements in ring buffer [0, rb->size]
 */
uint32_t ringbuffer_num(const struct ringbuffer *const rb);

/**
 * \brief Flush ring buffer, the user needs to handle the concurrent access on buffer
 * via put/get/flush
 *
 * \param[in] rb The pointer to a ring buffer structure instance
 *
 * \return ERR_NONE on success, or an error code on failure.
 */
uint32_t ringbuffer_flush(struct ringbuffer *const rb);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* _UTILS_RINGBUFFER_H_INCLUDED */
