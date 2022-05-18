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
 *   This file includes definitions for responding to PANID Query Requests.
 */

#ifndef ENERGY_SCAN_CLIENT_HPP_
#define ENERGY_SCAN_CLIENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

#include <openthread/commissioner.h>

#include "coap/coap.hpp"
#include "common/locator.hpp"
#include "net/ip6_address.hpp"
#include "net/udp6.hpp"

namespace ot {

/**
 * This class implements handling PANID Query Requests.
 *
 */
class EnergyScanClient : public InstanceLocator
{
public:
    /**
     * This constructor initializes the object.
     *
     */
    explicit EnergyScanClient(Instance &aInstance);

    /**
     * This method sends an Energy Scan Query message.
     *
     * @param[in]  aChannelMask   The channel mask value.
     * @param[in]  aCount         The number of energy measurements per channel.
     * @param[in]  aPeriod        The time between energy measurements (milliseconds).
     * @param[in]  aScanDuration  The scan duration for each energy measurement (milliseconds).
     * @param[in]  aAddress       A pointer to the IPv6 destination.
     * @param[in]  aCallback      A pointer to a function called on receiving an Energy Report message.
     * @param[in]  aContext       A pointer to application-specific context.
     *
     * @retval kErrorNone     Successfully enqueued the Energy Scan Query message.
     * @retval kErrorNoBufs   Insufficient buffers to generate an Energy Scan Query message.
     *
     */
    Error SendQuery(uint32_t                           aChannelMask,
                    uint8_t                            aCount,
                    uint16_t                           aPeriod,
                    uint16_t                           aScanDuration,
                    const Ip6::Address &               aAddress,
                    otCommissionerEnergyReportCallback aCallback,
                    void *                             aContext);

private:
    static void HandleReport(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleReport(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    otCommissionerEnergyReportCallback mCallback;
    void *                             mContext;

    Coap::Resource mEnergyScan;
};

/**
 * @}
 */

} // namespace ot

#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

#endif // ENERGY_SCAN_CLIENT_HPP_
