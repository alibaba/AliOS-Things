/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *  This file defines the OpenThread Network Time Synchronization Service API.
 */

#ifndef OPENTHREAD_NETWORK_TIME_H_
#define OPENTHREAD_NETWORK_TIME_H_

#include <openthread/error.h>
#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-network-time
 *
 * @brief
 *   This module includes functions that control network time synchronization service.
 *
 * @{
 *
 */

/**
 * This enumeration represents OpenThread time synchronization status.
 *
 */
typedef enum otNetworkTimeStatus
{
    OT_NETWORK_TIME_UNSYNCHRONIZED = -1, ///< The device hasn't attached to a network.
    OT_NETWORK_TIME_RESYNC_NEEDED  = 0,  ///< The device hasn’t received time sync for more than two periods time.
    OT_NETWORK_TIME_SYNCHRONIZED   = 1,  ///< The device network time is synchronized.
} otNetworkTimeStatus;

/**
 * This function pointer is called when a network time sync or status change occurs.
 *
 */
typedef void (*otNetworkTimeSyncCallbackFn)(void *aCallbackContext);

/**
 * zero is considered as invalid time synchronization sequence.
 *
 */
#define OT_TIME_SYNC_INVALID_SEQ 0

/**
 * Get the Thread network time.
 *
 * @param[in]    aInstance     The OpenThread instance structure.
 * @param[inout] aNetworkTime  The Thread network time in microseconds.
 *
 * @returns The time synchronization status.
 *
 */
otNetworkTimeStatus otNetworkTimeGet(otInstance *aInstance, uint64_t *aNetworkTime);

/**
 * Set the time synchronization period.
 *
 * This function can only be called while Thread protocols are disabled.
 *
 * @param[in] aInstance         The OpenThread instance structure.
 * @param[in] aTimeSyncPeriod   The time synchronization period, in seconds.
 *
 * @retval OT_ERROR_NONE           Successfully set the time sync period.
 * @retval OT_ERROR_INVALID_STATE  Thread protocols are enabled.
 *
 */
otError otNetworkTimeSetSyncPeriod(otInstance *aInstance, uint16_t aTimeSyncPeriod);

/**
 * Get the time synchronization period.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The time synchronization period.
 *
 */
uint16_t otNetworkTimeGetSyncPeriod(otInstance *aInstance);

/**
 * Set the time synchronization XTAL accuracy threshold for Router-Capable device.
 *
 * This function can only be called while Thread protocols are disabled.
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aXTALThreshold   The XTAL accuracy threshold for Router, in PPM.
 *
 * @retval OT_ERROR_NONE           Successfully set the time sync period.
 * @retval OT_ERROR_INVALID_STATE  Thread protocols are enabled.
 *
 */
otError otNetworkTimeSetXtalThreshold(otInstance *aInstance, uint16_t aXTALThreshold);

/**
 * Get the time synchronization XTAL accuracy threshold for Router.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 * @returns The XTAL accuracy threshold for Router, in PPM.
 *
 */
uint16_t otNetworkTimeGetXtalThreshold(otInstance *aInstance);

/**
 * Set a callback to be called when a network time sync or status change occurs
 *
 * This callback shall be called only when the network time offset jumps by
 * OPENTHREAD_CONFIG_TIME_SYNC_JUMP_NOTIF_MIN_US or when the status changes.
 *
 * @param[in] aInstance The OpenThread instance structure.
 * @param[in] aCallbackFn The callback function to be called
 * @param[in] aCallbackContext The context to be passed to the callback function upon invocation
 *
 */
void otNetworkTimeSyncSetCallback(otInstance *                aInstance,
                                  otNetworkTimeSyncCallbackFn aCallbackFn,
                                  void *                      aCallbackContext);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_NETWORK_TIME_H_
