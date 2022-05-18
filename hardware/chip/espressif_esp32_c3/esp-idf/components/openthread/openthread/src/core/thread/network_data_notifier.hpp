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
 *   This file includes definitions for transmitting SVR_DATA.ntf messages.
 */

#ifndef NETWORK_DATA_NOTIFIER_HPP_
#define NETWORK_DATA_NOTIFIER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#include "coap/coap.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"

namespace ot {
namespace NetworkData {

/**
 * This class implements the SVR_DATA.ntf transmission logic.
 *
 */
class Notifier : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * Constructor.
     *
     * @param[in] aInstance  The OpenThread instance.
     *
     */
    explicit Notifier(Instance &aInstance);

    /**
     * Call this method to inform the notifier that new server data is available.
     *
     */
    void HandleServerDataUpdated(void);

private:
    static constexpr uint32_t kDelayNoBufs                = 1000;   // in msec
    static constexpr uint32_t kDelayRemoveStaleChildren   = 5000;   // in msec
    static constexpr uint32_t kDelaySynchronizeServerData = 300000; // in msec

    void HandleNotifierEvents(Events aEvents);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static void HandleCoapResponse(void *               aContext,
                                   otMessage *          aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   Error                aResult);
    void        HandleCoapResponse(Error aResult);

    void SynchronizeServerData(void);

    TimerMilli mTimer;
    uint32_t   mNextDelay;
    bool       mWaitingForResponse;
};

} // namespace NetworkData
} // namespace ot

#endif // OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#endif // NETWORK_DATA_NOTIFIER_HPP_
