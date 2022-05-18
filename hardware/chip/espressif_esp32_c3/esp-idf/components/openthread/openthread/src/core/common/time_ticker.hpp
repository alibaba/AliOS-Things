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
 *   This file includes definitions for a time ticker.
 */

#ifndef TIME_TICKER_HPP_
#define TIME_TICKER_HPP_

#include "openthread-core-config.h"

#include <limits.h>

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/time.hpp"
#include "common/timer.hpp"

namespace ot {

/**
 * This class represents a time ticker.
 *
 * The time ticker emits periodic ticks (with 1 second period interval) to a set of registered tick receiver modules.
 * The tick receivers (OpenThread objects) are identified by the `Receiver` enumeration. The receiver objects
 * must provide `HandleTimeTick()` method which would be invoked by `TimeTicker` periodically.
 *
 */
class TimeTicker : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This enumeration represents time tick receivers.
     *
     * This enumeration contains the list of all OpenThread modules that can be registered as time tick receivers.
     *
     */
    enum Receiver : uint8_t
    {
        kMeshForwarder,          ///< `MeshForwarder`
        kMleRouter,              ///< `Mle::MleRouter`
        kAddressResolver,        ///< `AddressResolver`
        kChildSupervisor,        ///< `Utils::ChildSupervisor`
        kIp6FragmentReassembler, ///< `Ip6::Ip6` (handling of fragmented messages)
        kDuaManager,             ///< `DuaManager`
        kMlrManager,             ///< `MlrManager`

        kNumReceivers, ///< Number of receivers.
    };

    /**
     * This constructor initializes the `TimeTicker` instance.
     *
     */
    explicit TimeTicker(Instance &aInstance);

    /**
     * This method registers a receiver with `TimeTicker` to receive periodic ticks.
     *
     * @param[in] aReceiver   A tick receiver identifier.
     *
     */
    void RegisterReceiver(Receiver aReceiver);

    /**
     * This method unregisters a receiver with `TimeTicker` to receive periodic ticks.
     *
     * @param[in] aReceiver   A tick receiver identifier.
     *
     */
    void UnregisterReceiver(Receiver aReceiver);

    /**
     * This method indicates whether a receiver is registered with `TimeTicker` to receive periodic ticks.
     *
     * @param[in] aReceiver   A tick receiver identifier.
     *
     * @retval TRUE   If @p aReceiver is registered with `TimeTicker`.
     * @retval FALSE  If @p aReceiver is not registered with `TimeTicker`.
     *
     */
    bool IsReceiverRegistered(Receiver aReceiver) const { return (mReceivers & Mask(aReceiver)) != 0; }

private:
    static constexpr uint32_t kTickInterval  = 1000; // in msec.
    static constexpr uint32_t kRestartJitter = 4;    // in msec, jitter added when restarting the timer [-4,+4] ms.

    constexpr static uint32_t Mask(Receiver aReceiver) { return static_cast<uint32_t>(1U) << aReceiver; }

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    uint32_t   mReceivers;
    TimerMilli mTimer;

    static_assert(kNumReceivers < sizeof(mReceivers) * CHAR_BIT, "Too many `Receiver`s - does not fit in a bit mask");
};

} // namespace ot

#endif // TIMER_HPP_
