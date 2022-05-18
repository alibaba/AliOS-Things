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
 *   This file implements the OpenThread Link API.
 */

#include "openthread-core-config.h"

#include <openthread/link.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"
#include "mac/mac.hpp"
#include "radio/radio.hpp"

using namespace ot;

uint8_t otLinkGetChannel(otInstance *aInstance)
{
    Instance &instance = AsCoreType(aInstance);
    uint8_t   channel;

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    if (instance.Get<Mac::LinkRaw>().IsEnabled())
    {
        channel = instance.Get<Mac::LinkRaw>().GetChannel();
    }
    else
#endif
    {
        channel = instance.Get<Mac::Mac>().GetPanChannel();
    }

    return channel;
}

otError otLinkSetChannel(otInstance *aInstance, uint8_t aChannel)
{
    Error     error;
    Instance &instance = AsCoreType(aInstance);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    if (instance.Get<Mac::LinkRaw>().IsEnabled())
    {
        error = instance.Get<Mac::LinkRaw>().SetChannel(aChannel);
        ExitNow();
    }
#endif

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    SuccessOrExit(error = instance.Get<Mac::Mac>().SetPanChannel(aChannel));
    instance.Get<MeshCoP::ActiveDataset>().Clear();
    instance.Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

uint32_t otLinkGetSupportedChannelMask(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetSupportedChannelMask().GetMask();
}

otError otLinkSetSupportedChannelMask(otInstance *aInstance, uint32_t aChannelMask)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetSupportedChannelMask(Mac::ChannelMask(aChannelMask));

exit:
    return error;
}

const otExtAddress *otLinkGetExtendedAddress(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mac::Mac>().GetExtAddress();
}

otError otLinkSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    OT_ASSERT(aExtAddress != nullptr);
    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetExtAddress(AsCoreType(aExtAddress));

    instance.Get<Mle::MleRouter>().UpdateLinkLocalAddress();

exit:
    return error;
}

void otLinkGetFactoryAssignedIeeeEui64(otInstance *aInstance, otExtAddress *aEui64)
{
    AsCoreType(aInstance).Get<Radio>().GetIeeeEui64(AsCoreType(aEui64));
}

otPanId otLinkGetPanId(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetPanId();
}

otError otLinkSetPanId(otInstance *aInstance, otPanId aPanId)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetPanId(aPanId);
    instance.Get<MeshCoP::ActiveDataset>().Clear();
    instance.Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

uint32_t otLinkGetPollPeriod(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<DataPollSender>().GetKeepAlivePollPeriod();
}

otError otLinkSetPollPeriod(otInstance *aInstance, uint32_t aPollPeriod)
{
    return AsCoreType(aInstance).Get<DataPollSender>().SetExternalPollPeriod(aPollPeriod);
}

otError otLinkSendDataRequest(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<DataPollSender>().SendDataPoll();
}

otShortAddress otLinkGetShortAddress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetShortAddress();
}

uint8_t otLinkGetMaxFrameRetriesDirect(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetMaxFrameRetriesDirect();
}

void otLinkSetMaxFrameRetriesDirect(otInstance *aInstance, uint8_t aMaxFrameRetriesDirect)
{
    AsCoreType(aInstance).Get<Mac::Mac>().SetMaxFrameRetriesDirect(aMaxFrameRetriesDirect);
}

#if OPENTHREAD_FTD

uint8_t otLinkGetMaxFrameRetriesIndirect(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetMaxFrameRetriesIndirect();
}

void otLinkSetMaxFrameRetriesIndirect(otInstance *aInstance, uint8_t aMaxFrameRetriesIndirect)
{
    AsCoreType(aInstance).Get<Mac::Mac>().SetMaxFrameRetriesIndirect(aMaxFrameRetriesIndirect);
}

#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

otMacFilterAddressMode otLinkFilterGetAddressMode(otInstance *aInstance)
{
    return MapEnum(AsCoreType(aInstance).Get<Mac::Filter>().GetMode());
}

void otLinkFilterSetAddressMode(otInstance *aInstance, otMacFilterAddressMode aMode)
{
    AsCoreType(aInstance).Get<Mac::Filter>().SetMode(MapEnum(aMode));
}

otError otLinkFilterAddAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_ASSERT(aExtAddress != nullptr);

    return AsCoreType(aInstance).Get<Mac::Filter>().AddAddress(AsCoreType(aExtAddress));
}

void otLinkFilterRemoveAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_ASSERT(aExtAddress != nullptr);

    AsCoreType(aInstance).Get<Mac::Filter>().RemoveAddress(AsCoreType(aExtAddress));
}

void otLinkFilterClearAddresses(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Filter>().ClearAddresses();
}

otError otLinkFilterGetNextAddress(otInstance *aInstance, otMacFilterIterator *aIterator, otMacFilterEntry *aEntry)
{
    OT_ASSERT(aIterator != nullptr && aEntry != nullptr);

    return AsCoreType(aInstance).Get<Mac::Filter>().GetNextAddress(*aIterator, *aEntry);
}

otError otLinkFilterAddRssIn(otInstance *aInstance, const otExtAddress *aExtAddress, int8_t aRss)
{
    OT_ASSERT(aExtAddress != nullptr);

    return AsCoreType(aInstance).Get<Mac::Filter>().AddRssIn(AsCoreType(aExtAddress), aRss);
}

void otLinkFilterRemoveRssIn(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_ASSERT(aExtAddress != nullptr);

    AsCoreType(aInstance).Get<Mac::Filter>().RemoveRssIn(AsCoreType(aExtAddress));
}

void otLinkFilterSetDefaultRssIn(otInstance *aInstance, int8_t aRss)
{
    AsCoreType(aInstance).Get<Mac::Filter>().SetDefaultRssIn(aRss);
}

void otLinkFilterClearDefaultRssIn(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Mac::Filter>().ClearDefaultRssIn();
}

void otLinkFilterClearAllRssIn(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Mac::Filter>().ClearAllRssIn();
}

otError otLinkFilterGetNextRssIn(otInstance *aInstance, otMacFilterIterator *aIterator, otMacFilterEntry *aEntry)
{
    OT_ASSERT(aIterator != nullptr && aEntry != nullptr);

    return AsCoreType(aInstance).Get<Mac::Filter>().GetNextRssIn(*aIterator, *aEntry);
}

uint8_t otLinkConvertRssToLinkQuality(otInstance *aInstance, int8_t aRss)
{
    return LinkQualityInfo::ConvertRssToLinkQuality(AsCoreType(aInstance).Get<Mac::Mac>().GetNoiseFloor(), aRss);
}

int8_t otLinkConvertLinkQualityToRss(otInstance *aInstance, uint8_t aLinkQuality)
{
    return LinkQualityInfo::ConvertLinkQualityToRss(AsCoreType(aInstance).Get<Mac::Mac>().GetNoiseFloor(),
                                                    aLinkQuality);
}

#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
const uint32_t *otLinkGetTxDirectRetrySuccessHistogram(otInstance *aInstance, uint8_t *aNumberOfEntries)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetDirectRetrySuccessHistogram(*aNumberOfEntries);
}

const uint32_t *otLinkGetTxIndirectRetrySuccessHistogram(otInstance *aInstance, uint8_t *aNumberOfEntries)
{
    const uint32_t *histogram = nullptr;

#if OPENTHREAD_FTD
    histogram = AsCoreType(aInstance).Get<Mac::Mac>().GetIndirectRetrySuccessHistogram(*aNumberOfEntries);
#else
    OT_UNUSED_VARIABLE(aInstance);
    *aNumberOfEntries = 0;
#endif

    return histogram;
}

void otLinkResetTxRetrySuccessHistogram(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Mac::Mac>().ResetRetrySuccessHistogram();
}
#endif // OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE

void otLinkSetPcapCallback(otInstance *aInstance, otLinkPcapCallback aPcapCallback, void *aCallbackContext)
{
    AsCoreType(aInstance).Get<Mac::Mac>().SetPcapCallback(aPcapCallback, aCallbackContext);
}

bool otLinkIsPromiscuous(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().IsPromiscuous();
}

otError otLinkSetPromiscuous(otInstance *aInstance, bool aPromiscuous)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    // cannot enable IEEE 802.15.4 promiscuous mode if the Thread interface is enabled
    VerifyOrExit(!instance.Get<ThreadNetif>().IsUp(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetPromiscuous(aPromiscuous);

exit:
    return error;
}

otError otLinkSetEnabled(otInstance *aInstance, bool aEnable)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    // cannot disable the link layer if the Thread interface is enabled
    VerifyOrExit(!instance.Get<ThreadNetif>().IsUp(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetEnabled(aEnable);

exit:
    return error;
}

bool otLinkIsEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().IsEnabled();
}

const otMacCounters *otLinkGetCounters(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mac::Mac>().GetCounters();
}

void otLinkResetCounters(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Mac::Mac>().ResetCounters();
}

otError otLinkActiveScan(otInstance *             aInstance,
                         uint32_t                 aScanChannels,
                         uint16_t                 aScanDuration,
                         otHandleActiveScanResult aCallback,
                         void *                   aCallbackContext)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().ActiveScan(aScanChannels, aScanDuration, aCallback, aCallbackContext);
}

bool otLinkIsActiveScanInProgress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().IsActiveScanInProgress();
}

otError otLinkEnergyScan(otInstance *             aInstance,
                         uint32_t                 aScanChannels,
                         uint16_t                 aScanDuration,
                         otHandleEnergyScanResult aCallback,
                         void *                   aCallbackContext)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().EnergyScan(aScanChannels, aScanDuration, aCallback, aCallbackContext);
}

bool otLinkIsEnergyScanInProgress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().IsEnergyScanInProgress();
}

bool otLinkIsInTransmitState(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().IsInTransmitState();
}

uint16_t otLinkGetCcaFailureRate(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetCcaFailureRate();
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
uint8_t otLinkCslGetChannel(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetCslChannel();
}

otError otLinkCslSetChannel(otInstance *aInstance, uint8_t aChannel)
{
    Error error = kErrorNone;

    VerifyOrExit(Radio::IsCslChannelValid(aChannel), error = kErrorInvalidArgs);

    AsCoreType(aInstance).Get<Mac::Mac>().SetCslChannel(aChannel);

exit:
    return error;
}

uint16_t otLinkCslGetPeriod(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetCslPeriod();
}

otError otLinkCslSetPeriod(otInstance *aInstance, uint16_t aPeriod)
{
    Error error = kErrorNone;

    VerifyOrExit((aPeriod == 0 || kMinCslPeriod <= aPeriod), error = kErrorInvalidArgs);
    AsCoreType(aInstance).Get<Mac::Mac>().SetCslPeriod(aPeriod);

exit:
    return error;
}

uint32_t otLinkCslGetTimeout(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetCslTimeout();
}

otError otLinkCslSetTimeout(otInstance *aInstance, uint32_t aTimeout)
{
    Error error = kErrorNone;

    VerifyOrExit(kMaxCslTimeout >= aTimeout, error = kErrorInvalidArgs);
    AsCoreType(aInstance).Get<Mle::MleRouter>().SetCslTimeout(aTimeout);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
otError otLinkSendEmptyData(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<MeshForwarder>().SendEmptyMessage();
}
#endif
