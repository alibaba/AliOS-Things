/**
 ******************************************************************************
 * @file    mx_ringbuffer.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Apr-2018
 * @brief   Ringbuffer utils used for UART data receive
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef _MX_RINGBUFFER_H_
#define _MX_RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mx_common.h"
#include "mx_debug.h"

/** \addtogroup utils */
/** @{*/

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

/** @}*/

#ifdef __cplusplus
}
#endif
#endif /* _MX_RINGBUFFER_H_ */
