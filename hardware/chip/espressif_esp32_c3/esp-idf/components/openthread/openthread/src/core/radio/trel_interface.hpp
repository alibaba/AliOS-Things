/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes definitions for Thread Radio Encapsulation Link (TREL) interface.
 */

#ifndef TREL_INTERFACE_HPP_
#define TREL_INTERFACE_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include "common/locator.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6_address.hpp"
#include "radio/trel_packet.hpp"

namespace ot {
namespace Trel {

/**
 * This class represents a TREL link interface.
 *
 */
class Interface : public InstanceLocator
{
public:
    /**
     * This method initializes an `Interface` object
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Interface(Instance &aInstance);

    /**
     * This method initializes the interface.
     *
     * This method should be called after OpenThread instance itself is fully initialized, allowing the `Init()` method
     * to use method from `Instance` and any of its containing objects.
     *
     */
    void Init(void);

    /**
     * This method indicates whether the interface is initialized or not.
     *
     * @returns TRUE if the interface is initialized, FALSE otherwise.
     *
     */
    bool IsInitialized(void) const { return mInitialized; }

    /**
     * This method notifies the interface that device's extended MAC address has changed for it to update any
     * internal address/state.
     *
     */
    void HandleExtAddressChange(void);

    /**
     * This method sends a packet over the interface.
     *
     * @note There is no expected callback from the interface to notify completion of send.
     *
     * @param[in] aPacket   A packet to send.
     *
     * @retval kErrorNone   The frame was sent successfully.
     * @retval kErrorAbort  The interface is not ready and send was aborted.
     *
     */
    Error Send(const Packet &aPacket);

    /**
     * This method is a callback from platform layer to handle a received packet over the interface.
     *
     * @param[in] aBuffer  A pointer to buffer containing the received packet.
     * @param[in] aLength  The length (number of bytes) in the received packet.
     *
     */
    void HandleReceived(uint8_t *aBuffer, uint16_t aLength);

private:
    static constexpr uint16_t kUdpPort = 19788; // UDP port (same as MLE port).

    void CreateMulticastIp6Address(Ip6::Address &aIp6Address);

    bool   mInitialized;
    Packet mRxPacket;
};

} // namespace Trel
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#endif // TREL_INTERFACE_HPP_
