/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
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
 *   This file implements the OpenThread Link Raw API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE

#include <string.h>
#include <openthread/diag.h>
#include <openthread/platform/diag.h>

#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "mac/mac_frame.hpp"

namespace ot {
namespace Mac {

LinkRaw::LinkRaw(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mReceiveChannel(OPENTHREAD_CONFIG_DEFAULT_CHANNEL)
    , mPanId(kPanIdBroadcast)
    , mReceiveDoneCallback(nullptr)
    , mTransmitDoneCallback(nullptr)
    , mEnergyScanDoneCallback(nullptr)
#if OPENTHREAD_RADIO
    , mSubMac(aInstance)
#elif OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    , mSubMac(aInstance.Get<SubMac>())
#endif
{
    Init();
}

void LinkRaw::Init(void)
{
    mEnergyScanDoneCallback = nullptr;
    mTransmitDoneCallback   = nullptr;
    mReceiveDoneCallback    = nullptr;

    mReceiveChannel      = OPENTHREAD_CONFIG_DEFAULT_CHANNEL;
    mPanId               = kPanIdBroadcast;
    mReceiveDoneCallback = nullptr;
#if OPENTHREAD_RADIO
    mSubMac.Init();
#endif
}

Error LinkRaw::SetReceiveDone(otLinkRawReceiveDone aCallback)
{
    Error error  = kErrorNone;
    bool  enable = aCallback != nullptr;

    otLogDebgMac("LinkRaw::Enabled(%s)", (enable ? "true" : "false"));

#if OPENTHREAD_MTD || OPENTHREAD_FTD
    VerifyOrExit(!Get<ThreadNetif>().IsUp(), error = kErrorInvalidState);

    // In MTD/FTD build, `Mac` has already enabled sub-mac. We ensure to
    // disable/enable MAC layer when link-raw is being enabled/disabled to
    // avoid any conflict in control of radio and sub-mac between `Mac` and
    // `LinkRaw`. in RADIO build, we directly enable/disable sub-mac.

    if (!enable)
    {
        // When disabling link-raw, make sure there is no ongoing
        // transmit or scan operation. Otherwise Mac will attempt to
        // handle an unexpected "done" callback.
        VerifyOrExit(!mSubMac.IsTransmittingOrScanning(), error = kErrorBusy);
    }

    Get<Mac>().SetEnabled(!enable);
#else
    if (enable)
    {
        SuccessOrExit(error = mSubMac.Enable());
    }
    else
    {
        IgnoreError(mSubMac.Disable());
    }
#endif

    mReceiveDoneCallback = aCallback;

exit:
    return error;
}

Error LinkRaw::SetPanId(uint16_t aPanId)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    mSubMac.SetPanId(aPanId);
    mPanId = aPanId;

exit:
    return error;
}

Error LinkRaw::SetChannel(uint8_t aChannel)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    mReceiveChannel = aChannel;

exit:
    return error;
}

Error LinkRaw::SetExtAddress(const ExtAddress &aExtAddress)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    mSubMac.SetExtAddress(aExtAddress);

exit:
    return error;
}

Error LinkRaw::SetShortAddress(ShortAddress aShortAddress)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    mSubMac.SetShortAddress(aShortAddress);

exit:
    return error;
}

Error LinkRaw::Receive(void)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);

    SuccessOrExit(error = mSubMac.Receive(mReceiveChannel));

exit:
    return error;
}

void LinkRaw::InvokeReceiveDone(RxFrame *aFrame, Error aError)
{
    otLogDebgMac("LinkRaw::ReceiveDone(%d bytes), error:%s", (aFrame != nullptr) ? aFrame->mLength : 0,
                 ErrorToString(aError));

    if (mReceiveDoneCallback && (aError == kErrorNone))
    {
        mReceiveDoneCallback(&GetInstance(), aFrame, aError);
    }
}

Error LinkRaw::Transmit(otLinkRawTransmitDone aCallback)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);

    SuccessOrExit(error = mSubMac.Send());
    mTransmitDoneCallback = aCallback;

exit:
    return error;
}

void LinkRaw::InvokeTransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError)
{
    otLogDebgMac("LinkRaw::TransmitDone(%d bytes), error:%s", aFrame.mLength, ErrorToString(aError));

    if (mTransmitDoneCallback)
    {
        mTransmitDoneCallback(&GetInstance(), &aFrame, aAckFrame, aError);
        mTransmitDoneCallback = nullptr;
    }
}

Error LinkRaw::EnergyScan(uint8_t aScanChannel, uint16_t aScanDuration, otLinkRawEnergyScanDone aCallback)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);

    SuccessOrExit(error = mSubMac.EnergyScan(aScanChannel, aScanDuration));
    mEnergyScanDoneCallback = aCallback;

exit:
    return error;
}

void LinkRaw::InvokeEnergyScanDone(int8_t aEnergyScanMaxRssi)
{
    if (IsEnabled() && mEnergyScanDoneCallback != nullptr)
    {
        mEnergyScanDoneCallback(&GetInstance(), aEnergyScanMaxRssi);
        mEnergyScanDoneCallback = nullptr;
    }
}

Error LinkRaw::SetMacKey(uint8_t    aKeyIdMode,
                         uint8_t    aKeyId,
                         const Key &aPrevKey,
                         const Key &aCurrKey,
                         const Key &aNextKey)
{
    Error       error = kErrorNone;
    KeyMaterial prevKey;
    KeyMaterial currKey;
    KeyMaterial nextKey;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);

    prevKey.SetFrom(aPrevKey);
    currKey.SetFrom(aCurrKey);
    nextKey.SetFrom(aNextKey);

    mSubMac.SetMacKey(aKeyIdMode, aKeyId, prevKey, currKey, nextKey);

exit:
    return error;
}

Error LinkRaw::SetMacFrameCounter(uint32_t aMacFrameCounter)
{
    Error error = kErrorNone;

    VerifyOrExit(IsEnabled(), error = kErrorInvalidState);
    mSubMac.SetFrameCounter(aMacFrameCounter);

exit:
    return error;
}

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

void LinkRaw::RecordFrameTransmitStatus(const TxFrame &aFrame,
                                        const RxFrame *aAckFrame,
                                        Error          aError,
                                        uint8_t        aRetryCount,
                                        bool           aWillRetx)
{
    OT_UNUSED_VARIABLE(aAckFrame);
    OT_UNUSED_VARIABLE(aWillRetx);

    if (aError != kErrorNone)
    {
        otLogInfoMac("Frame tx failed, error:%s, retries:%d/%d, %s", ErrorToString(aError), aRetryCount,
                     aFrame.GetMaxFrameRetries(), aFrame.ToInfoString().AsCString());
    }
}

#endif

// LCOV_EXCL_STOP

} // namespace Mac
} // namespace ot

#endif // OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
