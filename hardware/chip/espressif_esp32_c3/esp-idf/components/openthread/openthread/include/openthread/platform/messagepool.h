/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *   This file includes the platform abstraction for the message pool.
 */

#ifndef OPENTHREAD_PLATFORM_MESSAGEPOOL_H_
#define OPENTHREAD_PLATFORM_MESSAGEPOOL_H_

#include <stdint.h>

#include <openthread/message.h>

/**
 * @addtogroup plat-messagepool
 *
 * @brief
 *   This module includes the platform abstraction for the message pool.
 *
 * @{
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This struct represents an OpenThread message buffer.
 *
 */
typedef struct otMessageBuffer
{
    struct otMessageBuffer *mNext; ///< Pointer to the next buffer.
} otMessageBuffer;

/**
 * Initialize the platform implemented message pool.
 *
 * This function is used when `OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT` is enabled.
 *
 * @param[in] aInstance            A pointer to the OpenThread instance.
 * @param[in] aMinNumFreeBuffers   An uint16 containing the minimum number of free buffers desired by OpenThread.
 * @param[in] aBufferSize          The size in bytes of a buffer object.
 *
 */
void otPlatMessagePoolInit(otInstance *aInstance, uint16_t aMinNumFreeBuffers, size_t aBufferSize);

/**
 * Allocate a buffer from the platform managed buffer pool.
 *
 * This function is used when `OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT` is enabled.
 *
 * The returned buffer instance MUST have at least `aBufferSize` bytes (as specified in `otPlatMessagePoolInit()`).
 *
 * @param[in] aInstance            A pointer to the OpenThread instance.
 *
 * @returns A pointer to the buffer or NULL if no buffers are available.
 *
 */
otMessageBuffer *otPlatMessagePoolNew(otInstance *aInstance);

/**
 * This function is used to free a buffer back to the platform managed buffer pool.
 *
 * This function is used when `OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT` is enabled.
 *
 * @param[in]  aInstance  A pointer to the OpenThread instance.
 * @param[in]  aBuffer    The buffer to free.
 *
 */
void otPlatMessagePoolFree(otInstance *aInstance, otMessageBuffer *aBuffer);

/**
 * Get the number of free buffers.
 *
 * This function is used when `OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT` is enabled.
 *
 * @param[in]  aInstance  A pointer to the OpenThread instance.
 *
 * @returns The number of buffers currently free and available to OpenThread.
 *
 */
uint16_t otPlatMessagePoolNumFreeBuffers(otInstance *aInstance);

#ifdef __cplusplus
} // extern "C"
#endif

/**
 * @}
 *
 */

#endif // OPENTHREAD_PLATFORM_MESSAGEPOOL_H_
