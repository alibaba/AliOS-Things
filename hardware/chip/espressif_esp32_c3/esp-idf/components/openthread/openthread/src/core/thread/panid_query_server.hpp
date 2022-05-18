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

#ifndef PANID_QUERY_SERVER_HPP_
#define PANID_QUERY_SERVER_HPP_

#include "openthread-core-config.h"

#include "coap/coap.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "mac/mac.hpp"
#include "net/ip6_address.hpp"
#include "net/udp6.hpp"

namespace ot {

/**
 * This class implements handling PANID Query Requests.
 *
 */
class PanIdQueryServer : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     */
    explicit PanIdQueryServer(Instance &aInstance);

private:
    static constexpr uint32_t kScanDelay = 1000; ///< SCAN_DELAY (in msec)

    static void HandleQuery(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleQuery(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleScanResult(Mac::ActiveScanResult *aScanResult, void *aContext);
    void        HandleScanResult(Mac::ActiveScanResult *aScanResult);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

    void SendConflict(void);

    Ip6::Address mCommissioner;
    uint32_t     mChannelMask;
    uint16_t     mPanId;

    TimerMilli mTimer;

    Coap::Resource mPanIdQuery;
};

/**
 * @}
 */

} // namespace ot

#endif // PANID_QUERY_SERVER_HPP_
