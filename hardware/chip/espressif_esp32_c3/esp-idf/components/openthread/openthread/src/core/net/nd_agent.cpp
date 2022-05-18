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
 *   This file implements IPv6 Neighbor Discovery Agent.
 */

#include "nd_agent.hpp"

#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE

#include "common/locator_getters.hpp"
#include "thread/lowpan.hpp"
#include "thread/mle_router.hpp"
#include "thread/network_data_leader.hpp"
#include "thread/thread_netif.hpp"

namespace ot {
namespace NeighborDiscovery {

void Agent::UpdateService(void)
{
    Error                           error;
    uint16_t                        rloc16 = Get<Mle::MleRouter>().GetRloc16();
    NetworkData::Iterator           iterator;
    NetworkData::OnMeshPrefixConfig config;
    Lowpan::Context                 lowpanContext;

    if (IsAlocInUse())
    {
        uint8_t contextId = static_cast<uint8_t>(mAloc.GetAddress().GetIid().GetLocator() -
                                                 Mle::kAloc16NeighborDiscoveryAgentStart + 1);
        bool    found     = false;

        iterator = NetworkData::kIteratorInit;

        while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, rloc16, config) == kErrorNone)
        {
            if (!config.mNdDns)
            {
                continue;
            }

            error = Get<NetworkData::Leader>().GetContext(static_cast<const Ip6::Address &>(config.mPrefix.mPrefix),
                                                          lowpanContext);

            if ((error != kErrorNone) || (lowpanContext.mContextId != contextId))
            {
                continue;
            }

            found = true;
            break;
        }

        if (!found)
        {
            Get<ThreadNetif>().RemoveUnicastAddress(mAloc);
            FreeAloc();
        }
    }

    VerifyOrExit(!IsAlocInUse());

    iterator = NetworkData::kIteratorInit;

    while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, rloc16, config) == kErrorNone)
    {
        if (!config.mNdDns)
        {
            continue;
        }

        error = Get<NetworkData::Leader>().GetContext(static_cast<const Ip6::Address &>(config.mPrefix.mPrefix),
                                                      lowpanContext);

        if (error == kErrorNone)
        {
            uint16_t rloc = Mle::kAloc16NeighborDiscoveryAgentStart + lowpanContext.mContextId - 1;

            mAloc.InitAsThreadOrigin();
            mAloc.GetAddress().SetToAnycastLocator(Get<Mle::MleRouter>().GetMeshLocalPrefix(), rloc);
            Get<ThreadNetif>().AddUnicastAddress(mAloc);
            ExitNow();
        }
    }

exit:
    return;
}

void Agent::ApplyMeshLocalPrefix(void)
{
    VerifyOrExit(IsAlocInUse());

    Get<ThreadNetif>().RemoveUnicastAddress(mAloc);
    mAloc.GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
    Get<ThreadNetif>().AddUnicastAddress(mAloc);

exit:
    return;
}

} // namespace NeighborDiscovery
} // namespace ot

#endif // OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
