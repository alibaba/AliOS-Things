/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for IPv6 Neighbor Discovery Agent.
 */

#ifndef NEIGHBOR_DISCOVERY_AGENT_HPP_
#define NEIGHBOR_DISCOVERY_AGENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "net/netif.hpp"

namespace ot {
namespace NeighborDiscovery {

class Agent : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Agent(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
        FreeAloc();
    }

    /**
     * This method updates the Neighbor Discovery Agents using current Thread Network Data.
     *
     */
    void UpdateService(void);

    /**
     * This method updates the prefix of the Neighbor Discovery Agent Anycast Locator.
     *
     */
    void ApplyMeshLocalPrefix(void);

private:
    void FreeAloc(void) { mAloc.mNext = &mAloc; }
    bool IsAlocInUse(void) const { return mAloc.mNext != &mAloc; }

    Ip6::Netif::UnicastAddress mAloc;
};

} // namespace NeighborDiscovery
} // namespace ot

#endif // OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE

#endif // NEIGHBOR_DISCOVERY_AGENT_HPP_
