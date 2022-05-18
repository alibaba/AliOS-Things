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
 *   This file implements a time ticker.
 */

#include "time_ticker.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/random.hpp"
#include "thread/mle_router.hpp"

namespace ot {

TimeTicker::TimeTicker(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mReceivers(0)
    , mTimer(aInstance, HandleTimer)
{
}

void TimeTicker::RegisterReceiver(Receiver aReceiver)
{
    mReceivers |= Mask(aReceiver);

    if (!mTimer.IsRunning())
    {
        mTimer.Start(Random::NonCrypto::GetUint32InRange(0, kTickInterval + 1));
    }
}

void TimeTicker::UnregisterReceiver(Receiver aReceiver)
{
    mReceivers &= ~Mask(aReceiver);

    if (mReceivers == 0)
    {
        mTimer.Stop();
    }
}

void TimeTicker::HandleTimer(Timer &aTimer)
{
    aTimer.Get<TimeTicker>().HandleTimer();
}

void TimeTicker::HandleTimer(void)
{
    mTimer.FireAt(mTimer.GetFireTime() + Random::NonCrypto::AddJitter(kTickInterval, kRestartJitter));

    if (mReceivers & Mask(kMeshForwarder))
    {
        Get<MeshForwarder>().HandleTimeTick();
    }

#if OPENTHREAD_FTD
    if (mReceivers & Mask(kMleRouter))
    {
        Get<Mle::MleRouter>().HandleTimeTick();
    }

    if (mReceivers & Mask(kAddressResolver))
    {
        Get<AddressResolver>().HandleTimeTick();
    }

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
    if (mReceivers & Mask(kChildSupervisor))
    {
        Get<Utils::ChildSupervisor>().HandleTimeTick();
    }
#endif
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_IP6_FRAGMENTATION_ENABLE
    if (mReceivers & Mask(kIp6FragmentReassembler))
    {
        Get<Ip6::Ip6>().HandleTimeTick();
    }
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
    if (mReceivers & Mask(kDuaManager))
    {
        Get<DuaManager>().HandleTimeTick();
    }
#endif

#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
    if (mReceivers & Mask(kMlrManager))
    {
        Get<MlrManager>().HandleTimeTick();
    }
#endif
}

} // namespace ot
