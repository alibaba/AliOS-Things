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
 *   This file implements IPv6 datagram filtering.
 */

#include "ip6_filter.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/meshcop.hpp"
#include "net/ip6.hpp"
#include "net/tcp6.hpp"
#include "net/udp6.hpp"
#include "thread/mle.hpp"

namespace ot {
namespace Ip6 {

Filter::Filter(Instance &aInstance)
    : InstanceLocator(aInstance)
{
    memset(mUnsecurePorts, 0, sizeof(mUnsecurePorts));
}

bool Filter::Accept(Message &aMessage) const
{
    bool        rval = false;
    Header      ip6;
    Udp::Header udp;
    Tcp::Header tcp;
    uint16_t    dstport;

    // Allow all received IPv6 datagrams with link security enabled
    if (aMessage.IsLinkSecurityEnabled())
    {
        ExitNow(rval = true);
    }

    // Read IPv6 header
    SuccessOrExit(aMessage.Read(0, ip6));

    // Allow only link-local unicast or multicast
    VerifyOrExit(ip6.GetDestination().IsLinkLocal() || ip6.GetDestination().IsLinkLocalMulticast());

    // Allow all link-local IPv6 datagrams when Thread is not enabled
    if (Get<Mle::MleRouter>().GetRole() == Mle::kRoleDisabled)
    {
        ExitNow(rval = true);
    }

    switch (ip6.GetNextHeader())
    {
    case kProtoUdp:
        // Read the UDP header and get the dst port
        SuccessOrExit(aMessage.Read(sizeof(ip6), udp));

        dstport = udp.GetDestinationPort();

        // Allow MLE traffic
        if (dstport == Mle::kUdpPort)
        {
            ExitNow(rval = true);
        }

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
        // Allow native commissioner traffic
        if (Get<KeyManager>().GetSecurityPolicy().mNativeCommissioningEnabled &&
            dstport == Get<MeshCoP::BorderAgent>().GetUdpPort())
        {
            ExitNow(rval = true);
        }
#endif
        break;

    case kProtoTcp:
        // Read the TCP header and get the dst port
        SuccessOrExit(aMessage.Read(sizeof(ip6), tcp));

        dstport = tcp.GetDestinationPort();

        break;

    default:
        // Allow UDP or TCP traffic only
        ExitNow();
    }

    // Check against allowed unsecure port list
    for (uint16_t unsecurePort : mUnsecurePorts)
    {
        if (unsecurePort != 0 && unsecurePort == dstport)
        {
            ExitNow(rval = true);
        }
    }

exit:
    return rval;
}

Error Filter::AddUnsecurePort(uint16_t aPort)
{
    Error error = kErrorNone;

    VerifyOrExit(aPort != 0, error = kErrorInvalidArgs);

    for (uint16_t unsecurePort : mUnsecurePorts)
    {
        if (unsecurePort == aPort)
        {
            ExitNow();
        }
    }

    for (uint16_t &unsecurePort : mUnsecurePorts)
    {
        if (unsecurePort == 0)
        {
            unsecurePort = aPort;
            otLogInfoIp6("Added unsecure port %d", aPort);
            ExitNow();
        }
    }

    ExitNow(error = kErrorNoBufs);

exit:
    return error;
}

Error Filter::RemoveUnsecurePort(uint16_t aPort)
{
    Error error = kErrorNone;

    VerifyOrExit(aPort != 0, error = kErrorInvalidArgs);

    for (int i = 0; i < kMaxUnsecurePorts; i++)
    {
        if (mUnsecurePorts[i] == aPort)
        {
            // Shift all of the ports higher than this
            // port down.
            for (; i < kMaxUnsecurePorts - 1; i++)
            {
                mUnsecurePorts[i] = mUnsecurePorts[i + 1];
            }

            // Clear the last port entry.
            mUnsecurePorts[i] = 0;
            otLogInfoIp6("Removed unsecure port %d", aPort);
            ExitNow();
        }
    }

    ExitNow(error = kErrorNotFound);

exit:
    return error;
}

bool Filter::IsUnsecurePort(uint16_t aPort)
{
    bool found = false;

    for (uint16_t unsecurePort : mUnsecurePorts)
    {
        if (unsecurePort == aPort)
        {
            found = true;
            break;
        }
    }
    return found;
}

void Filter::RemoveAllUnsecurePorts(void)
{
    memset(mUnsecurePorts, 0, sizeof(mUnsecurePorts));
}

const uint16_t *Filter::GetUnsecurePorts(uint8_t &aNumEntries) const
{
    // Count the number of unsecure ports.
    for (aNumEntries = 0; aNumEntries < kMaxUnsecurePorts; aNumEntries++)
    {
        if (mUnsecurePorts[aNumEntries] == 0)
        {
            break;
        }
    }

    return mUnsecurePorts;
}

} // namespace Ip6
} // namespace ot
