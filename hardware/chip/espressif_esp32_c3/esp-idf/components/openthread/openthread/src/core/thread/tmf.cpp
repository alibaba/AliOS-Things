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
 *   This file implements Thread Management Framework (TMF) functionalities.
 */

#include "thread/tmf.hpp"

#include "common/locator_getters.hpp"

namespace ot {
namespace Tmf {

Error Agent::Start(void)
{
    return Coap::Start(kUdpPort, OT_NETIF_THREAD);
}

Error Agent::Filter(const ot::Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo, void *aContext)
{
    OT_UNUSED_VARIABLE(aMessage);

    return static_cast<Agent *>(aContext)->IsTmfMessage(aMessageInfo) ? kErrorNone : kErrorNotTmf;
}

bool Agent::IsTmfMessage(const Ip6::MessageInfo &aMessageInfo) const
{
    bool rval = true;

    // A TMF message must comply with following rules:
    // 1. The destination is a Mesh Local Address or a Link-Local Multicast Address or a Realm-Local Multicast Address,
    //    and the source is a Mesh Local Address. Or
    // 2. Both the destination and the source are Link-Local Addresses.
    VerifyOrExit(
        ((Get<Mle::MleRouter>().IsMeshLocalAddress(aMessageInfo.GetSockAddr()) ||
          aMessageInfo.GetSockAddr().IsLinkLocalMulticast() || aMessageInfo.GetSockAddr().IsRealmLocalMulticast()) &&
         Get<Mle::MleRouter>().IsMeshLocalAddress(aMessageInfo.GetPeerAddr())) ||
            ((aMessageInfo.GetSockAddr().IsLinkLocal() || aMessageInfo.GetSockAddr().IsLinkLocalMulticast()) &&
             aMessageInfo.GetPeerAddr().IsLinkLocal()),
        rval = false);
exit:
    return rval;
}

} // namespace Tmf
} // namespace ot
