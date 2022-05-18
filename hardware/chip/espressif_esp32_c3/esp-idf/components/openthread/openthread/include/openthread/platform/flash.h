/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#ifndef OPENTHREAD_PLATFORM_FLASH_H_
#define OPENTHREAD_PLATFORM_FLASH_H_

#include <stdint.h>

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function initializes the flash driver.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 */
void otPlatFlashInit(otInstance *aInstance);

/**
 * This function gets the size of the swap space.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The size of the swap space in bytes.
 *
 */
uint32_t otPlatFlashGetSwapSize(otInstance *aInstance);

/**
 * This function erases the swap space indicated by @p aSwapIndex.
 *
 * @param[in] aInstance   The OpenThread instance structure.
 * @param[in] aSwapIndex  A value in [0, 1] that indicates the swap space.
 *
 */
void otPlatFlashErase(otInstance *aInstance, uint8_t aSwapIndex);

/**
 * This function reads @p aSize bytes into @p aData.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 * @param[in]  aSwapIndex  A value in [0, 1] that indicates the swap space.
 * @param[in]  aOffset     A byte offset within the swap space.
 * @param[out] aData       A pointer to the data buffer for reading.
 * @param[in]  aSize       Number of bytes to read.
 *
 */
void otPlatFlashRead(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, void *aData, uint32_t aSize);

/**
 * This function writes @p aSize bytes from @p aData.
 *
 * @param[in]  aInstance   The OpenThread instance structure.
 * @param[in]  aSwapIndex  A value in [0, 1] that indicates the swap space.
 * @param[in]  aOffset     A byte offset within the swap space.
 * @param[out] aData       A pointer to the data to write.
 * @param[in]  aSize       Number of bytes to write.
 *
 *
 */
void otPlatFlashWrite(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, const void *aData, uint32_t aSize);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_FLASH_H_
