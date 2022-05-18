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
 *  This file defines the OpenThread Multi Radio Link APIs.
 */

#ifndef OPENTHREAD_MULTI_RADIO_H_
#define OPENTHREAD_MULTI_RADIO_H_

#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-multi-radio
 *
 * @brief
 *   This module includes definitions and functions for multi radio link.
 *
 * @{
 *
 */

/**
 * This type represents information associated with a radio link.
 *
 */
typedef struct otRadioLinkInfo
{
    uint8_t mPreference; ///< Preference level of radio link
} otRadioLinkInfo;

/**
 * This type represents multi radio link information associated with a neighbor.
 *
 */
typedef struct otMultiRadioNeighborInfo
{
    bool            mSupportsIeee802154 : 1; ///< Neighbor supports IEEE 802.15.4 radio link
    bool            mSupportsTrelUdp6 : 1;   ///< Neighbor supports Thread Radio Encapsulation Link (TREL) radio link.
    otRadioLinkInfo mIeee802154Info;         ///< Additional info for 15.4 radio link (applicable when supported).
    otRadioLinkInfo mTrelUdp6Info;           ///< Additional info for TREL radio link (applicable when supported).
} otMultiRadioNeighborInfo;

/**
 * This function gets the multi radio link information associated with a neighbor with a given Extended Address.
 *
 * This function requires the multi radio link feature to be enabled (please see `config/radio_link.h`).
 *
 * @param[in]  aInstance      A pointer to an OpenThread instance.
 * @param[in]  aExtAddress    The Extended Address of neighbor.
 * @param[out] aNeighborInfo  A pointer to `otMultiRadioNeighborInfo` to output the neighbor info (on success).
 *
 * @retval OT_ERROR_NONE        Neighbor was found and @p aNeighborInfo was updated successfully.
 * @retval OT_ERROR_NOT_FOUND   Could not find a neighbor with @p aExtAddress.
 *
 */
otError otMultiRadioGetNeighborInfo(otInstance *              aInstance,
                                    const otExtAddress *      aExtAddress,
                                    otMultiRadioNeighborInfo *aNeighborInfo);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_MULTI_RADIO_H_
