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

/**
 * @file
 * @brief
 *   This file defines the link metrics interface for OpenThread platform radio drivers.
 *
 *   APIs defined in this module could be used by a platform to implement Enhanced-ACK Based Probing feature
 *   (Probing Subject side) in its radio driver.
 *
 */

#ifndef OPENTHREAD_UTILS_LINK_METRICS_H
#define OPENTHREAD_UTILS_LINK_METRICS_H

#include <openthread/link_metrics.h>

#include "mac_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This method initializes the Link Metrics util module.
 *
 * @param[in]  aNoiseFloor    The noise floor used by Link Metrics. It should be set to the platform's
 *                            noise floor (measured noise floor, receiver sensitivity or a constant).
 *
 */
void otLinkMetricsInit(int8_t aNoiseFloor);

/**
 * This method sets/clears Enhanced-ACK Based Probing for a specific Initiator.
 *
 * This method can start/stop Enhanced-ACK Based Probing for a neighbor that has the address @p aShortAddress and
 * @p aExtAddress. Once the Probing is started, the device would record the Link Metrics data of link layer frames
 * sent from that neighbor and include the data into header IE in Enhanced-ACK sent to that neighbor.
 *
 * @param[in]  aShortAddress    The short address of the Initiator.
 * @param[in]  aExtAddress      A pointer to the extended address of the Initiator.
 * @param[in]  aLinkMetrics     Flags specifying what metrics to query (Pdu Count would be omitted). When
 *                              @p aLinkMetrics is eqaul to `0`, this method clears the Initiator.
 *
 * @retval  OT_ERROR_NONE            Successfully configured the Enhanced-ACK Based Probing.
 * @retval  OT_ERROR_INVALID_ARGS    @p aExtAddress is `nullptr`.
 * @retval  OT_ERROR_NOT_FOUND       The Initiator indicated by @p aShortAddress is not found when trying to clear.
 * @retval  OT_ERROR_NO_BUFS         No more Initiator can be supported.
 *
 */
otError otLinkMetricsConfigureEnhAckProbing(otShortAddress      aShortAddress,
                                            const otExtAddress *aExtAddress,
                                            otLinkMetrics       aLinkMetrics);

/**
 * This method generates the Link Metrics data (assessed for the acknowledged frame) bytes that would be included in
 * Vendor-Specific IE.
 *
 * This method first checks what Link Metrics are specified by the Initiator indicated by @p aMacAddress. And then
 * write the values to @p aData.
 *
 * @param[in]  aMacAddress    The Mac address of the Initiator.
 * @param[in]  aLqi           LQI value of the acknowledged frame.
 * @param[in]  aRssi          RSSI value of the acknowledged frame.
 * @param[out] aData          A pointer to the buffer where the data would be written to. The caller should make
 *                            sure that the size of the buffer is not less than the size of Link Metrics data
 *                            configured before.
 *
 * @returns  The size of data read. Would be `0` if the Initiator is not found or @p aData is invalid.
 *
 */
uint8_t otLinkMetricsEnhAckGenData(const otMacAddress *aMacAddress, uint8_t aLqi, int8_t aRssi, uint8_t *aData);

/**
 * This method returns the data length of Enhanced-ACK Based Probing for a specific Initiator.
 *
 * @param[in]  aMacAddress    The Mac address of the Initiator.
 *
 * @returns  The size of data. `0` if it's not configured for the Initiator.
 *
 */
uint8_t otLinkMetricsEnhAckGetDataLen(const otMacAddress *aMacAddress);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_UTILS_LINK_METRICS_H
