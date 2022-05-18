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
 *   This file implements the radio callbacks.
 */

#include "radio.hpp"

#include "common/instance.hpp"
#include "common/locator_getters.hpp"

namespace ot {

void Radio::Callbacks::HandleReceiveDone(Mac::RxFrame *aFrame, Error aError)
{
    Get<Mac::SubMac>().HandleReceiveDone(aFrame, aError);
}

void Radio::Callbacks::HandleTransmitStarted(Mac::TxFrame &aFrame)
{
    Get<Mac::SubMac>().HandleTransmitStarted(aFrame);
}

void Radio::Callbacks::HandleTransmitDone(Mac::TxFrame &aFrame, Mac::RxFrame *aAckFrame, Error aError)
{
    Get<Mac::SubMac>().HandleTransmitDone(aFrame, aAckFrame, aError);
}

void Radio::Callbacks::HandleEnergyScanDone(int8_t aMaxRssi)
{
    Get<Mac::SubMac>().HandleEnergyScanDone(aMaxRssi);
}

#if OPENTHREAD_CONFIG_DIAG_ENABLE
void Radio::Callbacks::HandleDiagsReceiveDone(Mac::RxFrame *aFrame, Error aError)
{
#if OPENTHREAD_RADIO && !OPENTHREAD_RADIO_CLI
    // Pass it to notify OpenThread `Diags` module on host side.
    HandleReceiveDone(aFrame, aError);
#else // For OPENTHREAD_FTD, OPENTHREAD_MTD and OPENTHREAD_RADIO(CLI)
    Get<FactoryDiags::Diags>().ReceiveDone(aFrame, aError);
#endif
}

void Radio::Callbacks::HandleDiagsTransmitDone(Mac::TxFrame &aFrame, Error aError)
{
#if OPENTHREAD_RADIO && !OPENTHREAD_RADIO_CLI
    // Pass it to notify OpenThread `Diags` module on host side.
    HandleTransmitDone(aFrame, nullptr, aError);
#else // For OPENTHREAD_FTD, OPENTHREAD_MTD and OPENTHREAD_RADIO(CLI)
    OT_UNUSED_VARIABLE(aFrame);
    Get<FactoryDiags::Diags>().TransmitDone(aError);
#endif
}
#endif // OPENTHREAD_CONFIG_DIAG_ENABLE

} // namespace ot
