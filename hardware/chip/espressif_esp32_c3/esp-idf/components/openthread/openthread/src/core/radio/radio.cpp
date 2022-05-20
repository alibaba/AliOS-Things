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

#include "radio.hpp"

#include "common/locator_getters.hpp"
#include "utils/otns.hpp"

namespace ot {

#if OPENTHREAD_RADIO
void Radio::Init(void)
{
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    Error error = OT_ERROR_NONE;

    OT_UNUSED_VARIABLE(error);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    error = EnableCsl(0, Mac::kShortAddrInvalid, nullptr);
    OT_ASSERT(error == OT_ERROR_NONE);
#endif

    EnableSrcMatch(false);
    ClearSrcMatchShortEntries();
    ClearSrcMatchExtEntries();

    if (IsEnabled())
    {
        error = Sleep();
        OT_ASSERT(error == OT_ERROR_NONE);

        error = Disable();
        OT_ASSERT(error == OT_ERROR_NONE);
    }

    SetPanId(Mac::kPanIdBroadcast);
    SetExtendedAddress(Mac::ExtAddress{});
    SetShortAddress(Mac::kShortAddrInvalid);
    SetMacKey(0, 0, Mac::KeyMaterial{}, Mac::KeyMaterial{}, Mac::KeyMaterial{});
    SetMacFrameCounter(0);

    SetPromiscuous(false);
#endif // OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
}
#endif // OPENTHREAD_RADIO

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

void Radio::SetExtendedAddress(const Mac::ExtAddress &aExtAddress)
{
    otPlatRadioSetExtendedAddress(GetInstancePtr(), &aExtAddress);

#if (OPENTHREAD_MTD || OPENTHREAD_FTD) && OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitExtendedAddress(aExtAddress);
#endif
}

void Radio::SetShortAddress(Mac::ShortAddress aShortAddress)
{
    otPlatRadioSetShortAddress(GetInstancePtr(), aShortAddress);

#if (OPENTHREAD_MTD || OPENTHREAD_FTD) && OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitShortAddress(aShortAddress);
#endif
}

Error Radio::Transmit(Mac::TxFrame &aFrame)
{
#if (OPENTHREAD_MTD || OPENTHREAD_FTD) && OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitTransmit(aFrame);
#endif

    return otPlatRadioTransmit(GetInstancePtr(), &aFrame);
}
#endif // OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

} // namespace ot
