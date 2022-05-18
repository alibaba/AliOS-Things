/*
 *    Copyright (c) 2016-2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements minimal thread device required Spinel interface to the OpenThread stack.
 */

#include "openthread-core-config.h"

#include "ncp_base.hpp"

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#include <openthread/border_router.h>
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#include <openthread/channel_monitor.h>
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#include <openthread/child_supervision.h>
#endif
#include <openthread/diag.h>
#include <openthread/icmp6.h>
#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
#include <openthread/jam_detection.h>
#endif
#include <openthread/ncp.h>
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#include <openthread/network_time.h>
#endif
#include <openthread/platform/misc.h>
#include <openthread/platform/radio.h>
#if OPENTHREAD_FTD
#include <openthread/thread_ftd.h>
#endif
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#include <openthread/server.h>
#endif
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#include "openthread/backbone_router.h"
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
#include <openthread/srp_client_buffers.h>
#endif

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/string.hpp"
#include "net/ip6.hpp"

#if OPENTHREAD_MTD || OPENTHREAD_FTD

namespace ot {
namespace Ncp {

static uint8_t BorderRouterConfigToFlagByte(const otBorderRouterConfig &aConfig)
{
    uint8_t flags = 0;

    if (aConfig.mPreferred)
    {
        flags |= SPINEL_NET_FLAG_PREFERRED;
    }

    if (aConfig.mSlaac)
    {
        flags |= SPINEL_NET_FLAG_SLAAC;
    }

    if (aConfig.mDhcp)
    {
        flags |= SPINEL_NET_FLAG_DHCP;
    }

    if (aConfig.mDefaultRoute)
    {
        flags |= SPINEL_NET_FLAG_DEFAULT_ROUTE;
    }

    if (aConfig.mConfigure)
    {
        flags |= SPINEL_NET_FLAG_CONFIGURE;
    }

    if (aConfig.mOnMesh)
    {
        flags |= SPINEL_NET_FLAG_ON_MESH;
    }

    flags |= (static_cast<uint8_t>(aConfig.mPreference) << SPINEL_NET_FLAG_PREFERENCE_OFFSET);

    return flags;
}

static uint8_t BorderRouterConfigToFlagByteExtended(const otBorderRouterConfig &aConfig)
{
    uint8_t flags = 0;

    if (aConfig.mNdDns)
    {
        flags |= SPINEL_NET_FLAG_EXT_DNS;
    }

    if (aConfig.mDp)
    {
        flags |= SPINEL_NET_FLAG_EXT_DP;
    }

    return flags;
}

static uint8_t ExternalRouteConfigToFlagByte(const otExternalRouteConfig &aConfig)
{
    uint8_t flags = 0;

    switch (aConfig.mPreference)
    {
    case OT_ROUTE_PREFERENCE_LOW:
        flags |= SPINEL_ROUTE_PREFERENCE_LOW;
        break;

    case OT_ROUTE_PREFERENCE_HIGH:
        flags |= SPINEL_ROUTE_PREFERENCE_HIGH;
        break;

    case OT_ROUTE_PREFERENCE_MED:
    default:
        flags |= SPINEL_ROUTE_PREFERENCE_MEDIUM;
        break;
    }

    if (aConfig.mNat64)
    {
        flags |= SPINEL_ROUTE_FLAG_NAT64;
    }

    return flags;
}

uint8_t NcpBase::LinkFlagsToFlagByte(bool aRxOnWhenIdle, bool aDeviceType, bool aNetworkData)
{
    uint8_t flags(0);

    if (aRxOnWhenIdle)
    {
        flags |= SPINEL_THREAD_MODE_RX_ON_WHEN_IDLE;
    }

    if (aDeviceType)
    {
        flags |= SPINEL_THREAD_MODE_FULL_THREAD_DEV;
    }

    if (aNetworkData)
    {
        flags |= SPINEL_THREAD_MODE_FULL_NETWORK_DATA;
    }

    return flags;
}

otError NcpBase::EncodeNeighborInfo(const otNeighborInfo &aNeighborInfo)
{
    otError error;
    uint8_t modeFlags;

    modeFlags = LinkFlagsToFlagByte(aNeighborInfo.mRxOnWhenIdle, aNeighborInfo.mFullThreadDevice,
                                    aNeighborInfo.mFullNetworkData);

    SuccessOrExit(error = mEncoder.OpenStruct());

    SuccessOrExit(error = mEncoder.WriteEui64(aNeighborInfo.mExtAddress));
    SuccessOrExit(error = mEncoder.WriteUint16(aNeighborInfo.mRloc16));
    SuccessOrExit(error = mEncoder.WriteUint32(aNeighborInfo.mAge));
    SuccessOrExit(error = mEncoder.WriteUint8(aNeighborInfo.mLinkQualityIn));
    SuccessOrExit(error = mEncoder.WriteInt8(aNeighborInfo.mAverageRssi));
    SuccessOrExit(error = mEncoder.WriteUint8(modeFlags));
    SuccessOrExit(error = mEncoder.WriteBool(aNeighborInfo.mIsChild));
    SuccessOrExit(error = mEncoder.WriteUint32(aNeighborInfo.mLinkFrameCounter));
    SuccessOrExit(error = mEncoder.WriteUint32(aNeighborInfo.mMleFrameCounter));
    SuccessOrExit(error = mEncoder.WriteInt8(aNeighborInfo.mLastRssi));

    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
otError NcpBase::DecodeLinkMetrics(otLinkMetrics *aMetrics, bool aAllowPduCount)
{
    otError error   = OT_ERROR_NONE;
    uint8_t metrics = 0;

    SuccessOrExit(error = mDecoder.ReadUint8(metrics));

    if (metrics & SPINEL_THREAD_LINK_METRIC_PDU_COUNT)
    {
        VerifyOrExit(aAllowPduCount, error = OT_ERROR_INVALID_ARGS);
        aMetrics->mPduCount = true;
    }

    if (metrics & SPINEL_THREAD_LINK_METRIC_LQI)
    {
        aMetrics->mLqi = true;
    }

    if (metrics & SPINEL_THREAD_LINK_METRIC_LINK_MARGIN)
    {
        aMetrics->mLinkMargin = true;
    }

    if (metrics & SPINEL_THREAD_LINK_METRIC_RSSI)
    {
        aMetrics->mRssi = true;
    }

exit:
    return error;
}

otError NcpBase::EncodeLinkMetricsValues(const otLinkMetricsValues *aMetricsValues)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mEncoder.OpenStruct());

    if (aMetricsValues->mMetrics.mPduCount)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_THREAD_LINK_METRIC_PDU_COUNT));
        SuccessOrExit(error = mEncoder.WriteUint32(aMetricsValues->mPduCountValue));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aMetricsValues->mMetrics.mLqi)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_THREAD_LINK_METRIC_LQI));
        SuccessOrExit(error = mEncoder.WriteUint8(aMetricsValues->mLqiValue));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aMetricsValues->mMetrics.mLinkMargin)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_THREAD_LINK_METRIC_LINK_MARGIN));
        SuccessOrExit(error = mEncoder.WriteUint8(aMetricsValues->mLinkMarginValue));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aMetricsValues->mMetrics.mRssi)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_THREAD_LINK_METRIC_RSSI));
        SuccessOrExit(error = mEncoder.WriteInt8(aMetricsValues->mRssiValue));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CSL_PERIOD>(void)
{
    uint16_t cslPeriod;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint16(cslPeriod));

    error = otLinkCslSetPeriod(mInstance, cslPeriod);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CSL_PERIOD>(void)
{
    return mEncoder.WriteUint16(otLinkCslGetPeriod(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CSL_TIMEOUT>(void)
{
    uint32_t cslTimeout;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(cslTimeout));

    error = otLinkCslSetTimeout(mInstance, cslTimeout);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CSL_TIMEOUT>(void)
{
    return mEncoder.WriteUint32(otLinkCslGetTimeout(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CSL_CHANNEL>(void)
{
    uint8_t cslChannel;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(cslChannel));

    error = otLinkCslSetChannel(mInstance, cslChannel);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CSL_CHANNEL>(void)
{
    return mEncoder.WriteUint8(otLinkCslGetChannel(mInstance));
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MLR_REQUEST>(void)
{
    otError      error = OT_ERROR_NONE;
    otIp6Address addresses[kIp6AddressesNumMax];
    uint8_t      addressesCount = 0U;
    bool         timeoutPresent = false;
    uint32_t     timeout;

    SuccessOrExit(error = mDecoder.OpenStruct());

    while (mDecoder.GetRemainingLengthInStruct())
    {
        VerifyOrExit(addressesCount < kIp6AddressesNumMax, error = OT_ERROR_NO_BUFS);
        SuccessOrExit(error = mDecoder.ReadIp6Address(addresses[addressesCount]));
        ++addressesCount;
    }

    SuccessOrExit(error = mDecoder.CloseStruct());

    while (mDecoder.GetRemainingLengthInStruct())
    {
        uint8_t paramId;

        SuccessOrExit(error = mDecoder.OpenStruct());

        SuccessOrExit(error = mDecoder.ReadUint8(paramId));

        switch (paramId)
        {
        case SPINEL_THREAD_MLR_PARAMID_TIMEOUT:
            SuccessOrExit(error = mDecoder.ReadUint32(timeout));
            timeoutPresent = true;
            break;

        default:
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }

        SuccessOrExit(error = mDecoder.CloseStruct());
    }

    SuccessOrExit(error = otIp6RegisterMulticastListeners(mInstance, addresses, addressesCount,
                                                          timeoutPresent ? &timeout : nullptr,
                                                          &NcpBase::HandleMlrRegResult_Jump, this));
exit:
    return error;
}

void NcpBase::HandleMlrRegResult_Jump(void *              aContext,
                                      otError             aError,
                                      uint8_t             aMlrStatus,
                                      const otIp6Address *aFailedAddresses,
                                      uint8_t             aFailedAddressNum)
{
    static_cast<NcpBase *>(aContext)->HandleMlrRegResult(aError, aMlrStatus, aFailedAddresses, aFailedAddressNum);
}

void NcpBase::HandleMlrRegResult(otError             aError,
                                 uint8_t             aMlrStatus,
                                 const otIp6Address *aFailedAddresses,
                                 uint8_t             aFailedAddressNum)
{
    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_THREAD_MLR_RESPONSE));

    SuccessOrExit(mEncoder.WriteUint8(static_cast<uint8_t>(ThreadErrorToSpinelStatus(aError))));
    SuccessOrExit(mEncoder.WriteUint8(aMlrStatus));

    SuccessOrExit(mEncoder.OpenStruct());

    if (aError == OT_ERROR_NONE)
    {
        for (size_t i = 0U; i < aFailedAddressNum; ++i)
        {
            SuccessOrExit(mEncoder.WriteIp6Address(aFailedAddresses[i]));
        }
    }

    SuccessOrExit(mEncoder.CloseStruct());

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_PRIMARY>(void)
{
    otError                error = OT_ERROR_NONE;
    otBackboneRouterConfig bbrConfig;

    SuccessOrExit(error = otBackboneRouterGetPrimary(mInstance, &bbrConfig));

    SuccessOrExit(error = mEncoder.WriteUint16(bbrConfig.mServer16));
    SuccessOrExit(error = mEncoder.WriteUint16(bbrConfig.mReregistrationDelay));
    SuccessOrExit(error = mEncoder.WriteUint32(bbrConfig.mMlrTimeout));
    SuccessOrExit(error = mEncoder.WriteUint8(bbrConfig.mSequenceNumber));

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_DATA_POLL_PERIOD>(void)
{
    return mEncoder.WriteUint32(otLinkGetPollPeriod(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_DATA_POLL_PERIOD>(void)
{
    uint32_t pollPeriod;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(pollPeriod));

    error = otLinkSetPollPeriod(mInstance, pollPeriod);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_EXTENDED_ADDR>(void)
{
    return mEncoder.WriteEui64(*otLinkGetExtendedAddress(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_MAX_RETRY_NUMBER_DIRECT>(void)
{
    return mEncoder.WriteUint8(otLinkGetMaxFrameRetriesDirect(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_MAX_RETRY_NUMBER_DIRECT>(void)
{
    uint8_t maxFrameRetriesDirect;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(maxFrameRetriesDirect));
    otLinkSetMaxFrameRetriesDirect(mInstance, maxFrameRetriesDirect);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_PHY_CHAN_SUPPORTED>(void)
{
    uint32_t newMask = 0;
    otError  error   = OT_ERROR_NONE;

    SuccessOrExit(error = DecodeChannelMask(newMask));
    error = otLinkSetSupportedChannelMask(mInstance, newMask);

exit:
    return error;
}

otError NcpBase::CommandHandler_NET_CLEAR(uint8_t aHeader)
{
    return PrepareLastStatusResponse(aHeader, ThreadErrorToSpinelStatus(otInstanceErasePersistentInfo(mInstance)));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_SAVED>(void)
{
    return mEncoder.WriteBool(otDatasetIsCommissioned(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_IF_UP>(void)
{
    return mEncoder.WriteBool(otIp6IsEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_IF_UP>(void)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    error = otIp6SetEnabled(mInstance, enabled);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_STACK_UP>(void)
{
    return mEncoder.WriteBool(otThreadGetDeviceRole(mInstance) != OT_DEVICE_ROLE_DISABLED);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_STACK_UP>(void)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    // If the value has changed...
    if (enabled != (otThreadGetDeviceRole(mInstance) != OT_DEVICE_ROLE_DISABLED))
    {
        if (enabled)
        {
            error = otThreadSetEnabled(mInstance, true);
            StartLegacy();
        }
        else
        {
            error = otThreadSetEnabled(mInstance, false);
            StopLegacy();
        }
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_ROLE>(void)
{
    spinel_net_role_t role(SPINEL_NET_ROLE_DETACHED);

    switch (otThreadGetDeviceRole(mInstance))
    {
    case OT_DEVICE_ROLE_DISABLED:
    case OT_DEVICE_ROLE_DETACHED:
        role = SPINEL_NET_ROLE_DETACHED;
        break;

    case OT_DEVICE_ROLE_CHILD:
        role = SPINEL_NET_ROLE_CHILD;
        break;

    case OT_DEVICE_ROLE_ROUTER:
        role = SPINEL_NET_ROLE_ROUTER;
        break;

    case OT_DEVICE_ROLE_LEADER:
        role = SPINEL_NET_ROLE_LEADER;
        break;
    }

    return mEncoder.WriteUint8(role);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_ROLE>(void)
{
    unsigned int role  = 0;
    otError      error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUintPacked(role));

    switch (role)
    {
    case SPINEL_NET_ROLE_DETACHED:
        error = otThreadBecomeDetached(mInstance);
        break;

#if OPENTHREAD_FTD
    case SPINEL_NET_ROLE_ROUTER:
        error = otThreadBecomeRouter(mInstance);
        break;

    case SPINEL_NET_ROLE_LEADER:
        error = otThreadBecomeLeader(mInstance);
        break;
#endif // OPENTHREAD_FTD

    case SPINEL_NET_ROLE_CHILD:
        error = otThreadBecomeChild(mInstance);
        break;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_NETWORK_NAME>(void)
{
    return mEncoder.WriteUtf8(otThreadGetNetworkName(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_NETWORK_NAME>(void)
{
    const char *string = nullptr;
    otError     error  = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUtf8(string));

    error = otThreadSetNetworkName(mInstance, string);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_XPANID>(void)
{
    return mEncoder.WriteData(otThreadGetExtendedPanId(mInstance)->m8, sizeof(spinel_net_xpanid_t));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_XPANID>(void)
{
    const uint8_t *ptr = nullptr;
    uint16_t       len;
    otError        error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadData(ptr, len));

    VerifyOrExit(len == sizeof(spinel_net_xpanid_t), error = OT_ERROR_PARSE);

    error = otThreadSetExtendedPanId(mInstance, reinterpret_cast<const otExtendedPanId *>(ptr));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_NETWORK_KEY>(void)
{
    otNetworkKey networkKey;

    otThreadGetNetworkKey(mInstance, &networkKey);

    return mEncoder.WriteData(networkKey.m8, OT_NETWORK_KEY_SIZE);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_NETWORK_KEY>(void)
{
    const uint8_t *ptr = nullptr;
    uint16_t       len;
    otError        error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadData(ptr, len));

    VerifyOrExit(len == OT_NETWORK_KEY_SIZE, error = OT_ERROR_PARSE);

    error = otThreadSetNetworkKey(mInstance, reinterpret_cast<const otNetworkKey *>(ptr));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER>(void)
{
    return mEncoder.WriteUint32(otThreadGetKeySequenceCounter(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER>(void)
{
    uint32_t keySeqCounter;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(keySeqCounter));

    otThreadSetKeySequenceCounter(mInstance, keySeqCounter);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_PARTITION_ID>(void)
{
    return mEncoder.WriteUint32(otThreadGetPartitionId(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_KEY_SWITCH_GUARDTIME>(void)
{
    return mEncoder.WriteUint32(otThreadGetKeySwitchGuardTime(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_KEY_SWITCH_GUARDTIME>(void)
{
    uint32_t keyGuardTime;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(keyGuardTime));

    otThreadSetKeySwitchGuardTime(mInstance, keyGuardTime);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NETWORK_DATA_VERSION>(void)
{
    return mEncoder.WriteUint8(otNetDataGetVersion(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_STABLE_NETWORK_DATA_VERSION>(void)
{
    return mEncoder.WriteUint8(otNetDataGetStableVersion(mInstance));
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NETWORK_DATA>(void)
{
    uint8_t networkData[255];
    uint8_t networkDataLen = 255;

    IgnoreError(otBorderRouterGetNetData(mInstance,
                                         false, // Stable?
                                         networkData, &networkDataLen));

    return mEncoder.WriteData(networkData, networkDataLen);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_STABLE_NETWORK_DATA>(void)
{
    uint8_t networkData[255];
    uint8_t networkDataLen = 255;

    IgnoreError(otBorderRouterGetNetData(mInstance,
                                         true, // Stable?
                                         networkData, &networkDataLen));

    return mEncoder.WriteData(networkData, networkDataLen);
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_LEADER_NETWORK_DATA>(void)
{
    uint8_t networkData[255];
    uint8_t networkDataLen = 255;

    IgnoreError(otNetDataGet(mInstance,
                             false, // Stable?
                             networkData, &networkDataLen));

    return mEncoder.WriteData(networkData, networkDataLen);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_STABLE_LEADER_NETWORK_DATA>(void)
{
    uint8_t networkData[255];
    uint8_t networkDataLen = 255;

    IgnoreError(otNetDataGet(mInstance,
                             true, // Stable?
                             networkData, &networkDataLen));

    return mEncoder.WriteData(networkData, networkDataLen);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_LEADER_RID>(void)
{
    return mEncoder.WriteUint8(otThreadGetLeaderRouterId(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_LEADER_ADDR>(void)
{
    otError      error = OT_ERROR_NONE;
    otIp6Address address;

    error = otThreadGetLeaderRloc(mInstance, &address);

    if (error == OT_ERROR_NONE)
    {
        error = mEncoder.WriteIp6Address(address);
    }
    else
    {
        error = mEncoder.OverwriteWithLastStatusError(ThreadErrorToSpinelStatus(error));
    }

    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_PARENT>(void)
{
    otError error = OT_ERROR_NONE;

    otRouterInfo parentInfo;

    error = otThreadGetParentInfo(mInstance, &parentInfo);

    if (error == OT_ERROR_NONE)
    {
        if (parentInfo.mLinkEstablished)
        {
            int8_t averageRssi;
            int8_t lastRssi;

            IgnoreError(otThreadGetParentAverageRssi(mInstance, &averageRssi));
            IgnoreError(otThreadGetParentLastRssi(mInstance, &lastRssi));

            SuccessOrExit(error = mEncoder.WriteEui64(parentInfo.mExtAddress));
            SuccessOrExit(error = mEncoder.WriteUint16(parentInfo.mRloc16));
            SuccessOrExit(error = mEncoder.WriteUint32(parentInfo.mAge));
            SuccessOrExit(error = mEncoder.WriteInt8(averageRssi));
            SuccessOrExit(error = mEncoder.WriteInt8(lastRssi));
            SuccessOrExit(error = mEncoder.WriteUint8(parentInfo.mLinkQualityIn));
            SuccessOrExit(error = mEncoder.WriteUint8(parentInfo.mLinkQualityOut));
        }
        else
        {
            SuccessOrExit(error = mEncoder.OverwriteWithLastStatusError(SPINEL_STATUS_ITEM_NOT_FOUND));
        }
    }
    else
    {
        error = mEncoder.OverwriteWithLastStatusError(ThreadErrorToSpinelStatus(error));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NEIGHBOR_TABLE>(void)
{
    otError                error = OT_ERROR_NONE;
    otNeighborInfoIterator iter  = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otNeighborInfo         neighInfo;

    while (otThreadGetNextNeighborInfo(mInstance, &iter, &neighInfo) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = EncodeNeighborInfo(neighInfo));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NEIGHBOR_TABLE_ERROR_RATES>(void)
{
    otError                error = OT_ERROR_NONE;
    otNeighborInfoIterator iter  = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otNeighborInfo         neighInfo;

    while (otThreadGetNextNeighborInfo(mInstance, &iter, &neighInfo) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(neighInfo.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteUint16(neighInfo.mRloc16));
        SuccessOrExit(error = mEncoder.WriteUint16(neighInfo.mFrameErrorRate));
        SuccessOrExit(error = mEncoder.WriteUint16(neighInfo.mMessageErrorRate));
        SuccessOrExit(error = mEncoder.WriteInt8(neighInfo.mAverageRssi));
        SuccessOrExit(error = mEncoder.WriteInt8(neighInfo.mLastRssi));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ASSISTING_PORTS>(void)
{
    otError         error      = OT_ERROR_NONE;
    uint8_t         numEntries = 0;
    const uint16_t *ports      = otIp6GetUnsecurePorts(mInstance, &numEntries);

    for (; numEntries != 0; ports++, numEntries--)
    {
        SuccessOrExit(error = mEncoder.WriteUint16(*ports));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ASSISTING_PORTS>(void)
{
    otError error = OT_ERROR_NONE;

    // First, we need to remove all of the current assisting ports.
    otIp6RemoveAllUnsecurePorts(mInstance);

    while (mDecoder.GetRemainingLengthInStruct() >= sizeof(uint16_t))
    {
        uint16_t port;

        SuccessOrExit(error = mDecoder.ReadUint16(port));
        SuccessOrExit(error = otIp6AddUnsecurePort(mInstance, port));
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        // We had an error, but we've actually changed
        // the state of these ports, so we need to report
        // those incomplete changes via an asynchronous
        // change event.
        IgnoreError(
            WritePropertyValueIsFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_PROP_THREAD_ASSISTING_PORTS));
    }

    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ALLOW_LOCAL_NET_DATA_CHANGE>(void)
{
    return mEncoder.WriteBool(mAllowLocalNetworkDataChange);
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ALLOW_LOCAL_NET_DATA_CHANGE>(void)
{
    bool    value                    = false;
    otError error                    = OT_ERROR_NONE;
    bool    shouldRegisterWithLeader = false;

    SuccessOrExit(error = mDecoder.ReadBool(value));

    // Register any net data changes on transition from `true` to `false`.
    shouldRegisterWithLeader = mAllowLocalNetworkDataChange && !value;

    mAllowLocalNetworkDataChange = value;

exit:

    if (shouldRegisterWithLeader)
    {
        IgnoreError(otBorderRouterRegister(mInstance));
    }

    return error;
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ON_MESH_NETS>(void)
{
    otError               error = OT_ERROR_NONE;
    otBorderRouterConfig  borderRouterConfig;
    otNetworkDataIterator iter = OT_NETWORK_DATA_ITERATOR_INIT;

    // Fill from non-local network data first
    while (otNetDataGetNextOnMeshPrefix(mInstance, &iter, &borderRouterConfig) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteIp6Address(borderRouterConfig.mPrefix.mPrefix));
        SuccessOrExit(error = mEncoder.WriteUint8(borderRouterConfig.mPrefix.mLength));
        SuccessOrExit(error = mEncoder.WriteBool(borderRouterConfig.mStable));
        SuccessOrExit(error = mEncoder.WriteUint8(BorderRouterConfigToFlagByte(borderRouterConfig)));
        SuccessOrExit(error = mEncoder.WriteBool(false)); // isLocal
        SuccessOrExit(error = mEncoder.WriteUint16(borderRouterConfig.mRloc16));
        SuccessOrExit(error = mEncoder.WriteUint8(BorderRouterConfigToFlagByteExtended(borderRouterConfig)));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

    iter = OT_NETWORK_DATA_ITERATOR_INIT;

    // Fill from local network data last
    while (otBorderRouterGetNextOnMeshPrefix(mInstance, &iter, &borderRouterConfig) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteIp6Address(borderRouterConfig.mPrefix.mPrefix));
        SuccessOrExit(error = mEncoder.WriteUint8(borderRouterConfig.mPrefix.mLength));
        SuccessOrExit(error = mEncoder.WriteBool(borderRouterConfig.mStable));
        SuccessOrExit(error = mEncoder.WriteUint8(BorderRouterConfigToFlagByte(borderRouterConfig)));
        SuccessOrExit(error = mEncoder.WriteBool(true)); // isLocal
        SuccessOrExit(error = mEncoder.WriteUint16(borderRouterConfig.mRloc16));
        SuccessOrExit(error = mEncoder.WriteUint8(BorderRouterConfigToFlagByteExtended(borderRouterConfig)));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

exit:
    return error;
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_THREAD_ON_MESH_NETS>(void)
{
    otError              error = OT_ERROR_NONE;
    otBorderRouterConfig borderRouterConfig;
    bool                 stable = false;
    bool                 isLocal;
    uint8_t              flags         = 0;
    uint8_t              flagsExtended = 0;
    uint8_t              prefixLength;
    uint16_t             rloc16;

    memset(&borderRouterConfig, 0, sizeof(otBorderRouterConfig));

    VerifyOrExit(mAllowLocalNetworkDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadIp6Address(borderRouterConfig.mPrefix.mPrefix));
    SuccessOrExit(error = mDecoder.ReadUint8(prefixLength));
    SuccessOrExit(error = mDecoder.ReadBool(stable));
    SuccessOrExit(error = mDecoder.ReadUint8(flags));

    borderRouterConfig.mPrefix.mLength = prefixLength;
    borderRouterConfig.mStable         = stable;
    borderRouterConfig.mPreference   = ((flags & SPINEL_NET_FLAG_PREFERENCE_MASK) >> SPINEL_NET_FLAG_PREFERENCE_OFFSET);
    borderRouterConfig.mPreferred    = ((flags & SPINEL_NET_FLAG_PREFERRED) != 0);
    borderRouterConfig.mSlaac        = ((flags & SPINEL_NET_FLAG_SLAAC) != 0);
    borderRouterConfig.mDhcp         = ((flags & SPINEL_NET_FLAG_DHCP) != 0);
    borderRouterConfig.mConfigure    = ((flags & SPINEL_NET_FLAG_CONFIGURE) != 0);
    borderRouterConfig.mDefaultRoute = ((flags & SPINEL_NET_FLAG_DEFAULT_ROUTE) != 0);
    borderRouterConfig.mOnMesh       = ((flags & SPINEL_NET_FLAG_ON_MESH) != 0);

    // A new field 'TLV flags extended' has been added to the SPINEL_PROP_THREAD_ON_MESH_NETS property.
    // To correctly handle a new field for INSERT command, the additional fields 'isLocal' and 'rloc16' are read and
    // ignored.
    if ((mDecoder.ReadBool(isLocal) == OT_ERROR_NONE) && (mDecoder.ReadUint16(rloc16) == OT_ERROR_NONE) &&
        (mDecoder.ReadUint8(flagsExtended) == OT_ERROR_NONE))
    {
        borderRouterConfig.mNdDns = ((flagsExtended & SPINEL_NET_FLAG_EXT_DNS) != 0);
        borderRouterConfig.mDp    = ((flagsExtended & SPINEL_NET_FLAG_EXT_DP) != 0);
    }

    error = otBorderRouterAddOnMeshPrefix(mInstance, &borderRouterConfig);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_THREAD_ON_MESH_NETS>(void)
{
    otError     error = OT_ERROR_NONE;
    otIp6Prefix ip6Prefix;
    uint8_t     prefixLength;

    memset(&ip6Prefix, 0, sizeof(otIp6Prefix));

    VerifyOrExit(mAllowLocalNetworkDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadIp6Address(ip6Prefix.mPrefix));
    SuccessOrExit(error = mDecoder.ReadUint8(prefixLength));

    ip6Prefix.mLength = prefixLength;

    error = otBorderRouterRemoveOnMeshPrefix(mInstance, &ip6Prefix);

    // If prefix was not on the list, "remove" command can be considred
    // successful.

    if (error == OT_ERROR_NOT_FOUND)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SERVER_ALLOW_LOCAL_DATA_CHANGE>(void)
{
    return mEncoder.WriteBool(mAllowLocalServerDataChange);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SERVER_ALLOW_LOCAL_DATA_CHANGE>(void)
{
    bool    value                    = false;
    otError error                    = OT_ERROR_NONE;
    bool    shouldRegisterWithLeader = false;

    SuccessOrExit(error = mDecoder.ReadBool(value));

    // Register any server data changes on transition from `true` to `false`.
    shouldRegisterWithLeader = mAllowLocalServerDataChange && !value;

    mAllowLocalServerDataChange = value;

exit:

    if (shouldRegisterWithLeader)
    {
        IgnoreError(otServerRegister(mInstance));
    }

    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_SERVER_SERVICES>(void)
{
    otError         error = OT_ERROR_NONE;
    otServiceConfig cfg;
    bool            stable;
    const uint8_t * data;
    uint16_t        dataLen;

    VerifyOrExit(mAllowLocalServerDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadUint32(cfg.mEnterpriseNumber));
    SuccessOrExit(error = mDecoder.ReadDataWithLen(data, dataLen));

    VerifyOrExit((dataLen <= sizeof(cfg.mServiceData)), error = OT_ERROR_INVALID_ARGS);
    memcpy(cfg.mServiceData, data, dataLen);

    static_assert((sizeof(cfg.mServiceData) <= UINT8_MAX), "Cannot handle full range of buffer length");
    cfg.mServiceDataLength = static_cast<uint8_t>(dataLen);

    SuccessOrExit(error = mDecoder.ReadBool(stable));
    cfg.mServerConfig.mStable = stable;
    SuccessOrExit(error = mDecoder.ReadDataWithLen(data, dataLen));

    VerifyOrExit((dataLen <= sizeof(cfg.mServerConfig.mServerData)), error = OT_ERROR_INVALID_ARGS);
    memcpy(cfg.mServerConfig.mServerData, data, dataLen);

    static_assert((sizeof(cfg.mServerConfig.mServerData) <= UINT8_MAX), "Cannot handle full range of buffer length");
    cfg.mServerConfig.mServerDataLength = static_cast<uint8_t>(dataLen);

    SuccessOrExit(error = otServerAddService(mInstance, &cfg));
exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_SERVER_SERVICES>(void)
{
    otError error = OT_ERROR_NONE;

    uint32_t       enterpriseNumber;
    const uint8_t *serviceData;
    uint16_t       serviceDataLength;

    VerifyOrExit(mAllowLocalServerDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadUint32(enterpriseNumber));
    SuccessOrExit(error = mDecoder.ReadDataWithLen(serviceData, serviceDataLength));

    VerifyOrExit(serviceDataLength <= UINT8_MAX, error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = otServerRemoveService(mInstance, enterpriseNumber, serviceData,
                                                static_cast<uint8_t>(serviceDataLength)));
exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SERVER_SERVICES>(void)
{
    otError               error    = OT_ERROR_NONE;
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otServiceConfig       cfg;

    while (otServerGetNextService(mInstance, &iterator, &cfg) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteUint32(cfg.mEnterpriseNumber));
        SuccessOrExit(error = mEncoder.WriteDataWithLen(cfg.mServiceData, cfg.mServiceDataLength));
        SuccessOrExit(error = mEncoder.WriteBool(cfg.mServerConfig.mStable));
        SuccessOrExit(
            error = mEncoder.WriteDataWithLen(cfg.mServerConfig.mServerData, cfg.mServerConfig.mServerDataLength));
        SuccessOrExit(error = mEncoder.WriteUint16(cfg.mServerConfig.mRloc16));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }
exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SERVER_LEADER_SERVICES>(void)
{
    otError               error    = OT_ERROR_NONE;
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otServiceConfig       cfg;

    while (otNetDataGetNextService(mInstance, &iterator, &cfg) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteUint8(cfg.mServiceId));
        SuccessOrExit(error = mEncoder.WriteUint32(cfg.mEnterpriseNumber));
        SuccessOrExit(error = mEncoder.WriteDataWithLen(cfg.mServiceData, cfg.mServiceDataLength));
        SuccessOrExit(error = mEncoder.WriteBool(cfg.mServerConfig.mStable));
        SuccessOrExit(
            error = mEncoder.WriteDataWithLen(cfg.mServerConfig.mServerData, cfg.mServerConfig.mServerDataLength));
        SuccessOrExit(error = mEncoder.WriteUint16(cfg.mServerConfig.mRloc16));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }
exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_JOINER_FLAG>(void)
{
    return mEncoder.WriteBool(mDiscoveryScanJoinerFlag);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_JOINER_FLAG>(void)
{
    return mDecoder.ReadBool(mDiscoveryScanJoinerFlag);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_ENABLE_FILTERING>(void)
{
    return mEncoder.WriteBool(mDiscoveryScanEnableFiltering);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_ENABLE_FILTERING>(void)
{
    return mDecoder.ReadBool(mDiscoveryScanEnableFiltering);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_PANID>(void)
{
    return mEncoder.WriteUint16(mDiscoveryScanPanId);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_DISCOVERY_SCAN_PANID>(void)
{
    return mDecoder.ReadUint16(mDiscoveryScanPanId);
}

otError NcpBase::EncodeOperationalDataset(const otOperationalDataset &aDataset)
{
    otError error = OT_ERROR_NONE;

    if (aDataset.mComponents.mIsActiveTimestampPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP));
        SuccessOrExit(error = mEncoder.WriteUint64(aDataset.mActiveTimestamp));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsPendingTimestampPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_DATASET_PENDING_TIMESTAMP));
        SuccessOrExit(error = mEncoder.WriteUint64(aDataset.mPendingTimestamp));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsNetworkKeyPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_NET_NETWORK_KEY));
        SuccessOrExit(error = mEncoder.WriteData(aDataset.mNetworkKey.m8, OT_NETWORK_KEY_SIZE));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsNetworkNamePresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_NET_NETWORK_NAME));
        SuccessOrExit(error = mEncoder.WriteUtf8(aDataset.mNetworkName.m8));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsExtendedPanIdPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_NET_XPANID));
        SuccessOrExit(error = mEncoder.WriteData(aDataset.mExtendedPanId.m8, OT_EXT_PAN_ID_SIZE));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsMeshLocalPrefixPresent)
    {
        otIp6Address addr;

        memcpy(addr.mFields.m8, aDataset.mMeshLocalPrefix.m8, 8);
        memset(addr.mFields.m8 + 8, 0, 8); // Zero out the last 8 bytes.

        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_IPV6_ML_PREFIX));
        SuccessOrExit(error = mEncoder.WriteIp6Address(addr));             // Mesh local prefix
        SuccessOrExit(error = mEncoder.WriteUint8(OT_IP6_PREFIX_BITSIZE)); // Prefix length (in bits)
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsDelayPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_DATASET_DELAY_TIMER));
        SuccessOrExit(error = mEncoder.WriteUint32(aDataset.mDelay));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsPanIdPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_MAC_15_4_PANID));
        SuccessOrExit(error = mEncoder.WriteUint16(aDataset.mPanId));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsChannelPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_PHY_CHAN));

        // The channel is stored in Dataset as `uint16_t` (to accommodate
        // larger number of channels in sub-GHz band),  however the current
        // definition of `SPINEL_PROP_PHY_CHAN` property limits the channel
        // to a `uint8_t`.

        SuccessOrExit(error = mEncoder.WriteUint8(static_cast<uint8_t>(aDataset.mChannel)));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsPskcPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_NET_PSKC));
        SuccessOrExit(error = mEncoder.WriteData(aDataset.mPskc.m8, sizeof(spinel_net_pskc_t)));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsSecurityPolicyPresent)
    {
        uint8_t flags[2];

        static_cast<const SecurityPolicy &>(aDataset.mSecurityPolicy).GetFlags(flags, sizeof(flags));
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_DATASET_SECURITY_POLICY));
        SuccessOrExit(error = mEncoder.WriteUint16(aDataset.mSecurityPolicy.mRotationTime));
        SuccessOrExit(error = mEncoder.WriteUint8(flags[0]));
        if (otThreadGetVersion() >= OT_THREAD_VERSION_1_2)
        {
            SuccessOrExit(error = mEncoder.WriteUint8(flags[1]));
        }
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

    if (aDataset.mComponents.mIsChannelMaskPresent)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROP_PHY_CHAN_SUPPORTED));
        SuccessOrExit(error = EncodeChannelMask(aDataset.mChannelMask));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ACTIVE_DATASET>(void)
{
    otOperationalDataset dataset;

    IgnoreError(otDatasetGetActive(mInstance, &dataset));
    return EncodeOperationalDataset(dataset);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_PENDING_DATASET>(void)
{
    otOperationalDataset dataset;

    IgnoreError(otDatasetGetPending(mInstance, &dataset));
    return EncodeOperationalDataset(dataset);
}

otError NcpBase::DecodeOperationalDataset(otOperationalDataset &aDataset,
                                          const uint8_t **      aTlvs,
                                          uint8_t *             aTlvsLength,
                                          const otIp6Address ** aDestIpAddress,
                                          bool                  aAllowEmptyValues)
{
    otError error = OT_ERROR_NONE;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    if (aTlvs != nullptr)
    {
        *aTlvs = nullptr;
    }

    if (aTlvsLength != nullptr)
    {
        *aTlvsLength = 0;
    }

    if (aDestIpAddress != nullptr)
    {
        *aDestIpAddress = nullptr;
    }

    while (!mDecoder.IsAllReadInStruct())
    {
        unsigned int propKey;

        SuccessOrExit(error = mDecoder.OpenStruct());
        SuccessOrExit(error = mDecoder.ReadUintPacked(propKey));

        switch (static_cast<spinel_prop_key_t>(propKey))
        {
        case SPINEL_PROP_DATASET_ACTIVE_TIMESTAMP:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                SuccessOrExit(error = mDecoder.ReadUint64(aDataset.mActiveTimestamp));
            }

            aDataset.mComponents.mIsActiveTimestampPresent = true;
            break;

        case SPINEL_PROP_DATASET_PENDING_TIMESTAMP:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                SuccessOrExit(error = mDecoder.ReadUint64(aDataset.mPendingTimestamp));
            }

            aDataset.mComponents.mIsPendingTimestampPresent = true;
            break;

        case SPINEL_PROP_NET_NETWORK_KEY:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const uint8_t *key;
                uint16_t       len;

                SuccessOrExit(error = mDecoder.ReadData(key, len));
                VerifyOrExit(len == OT_NETWORK_KEY_SIZE, error = OT_ERROR_INVALID_ARGS);
                memcpy(aDataset.mNetworkKey.m8, key, len);
            }

            aDataset.mComponents.mIsNetworkKeyPresent = true;
            break;

        case SPINEL_PROP_NET_NETWORK_NAME:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const char *name;
                size_t      len;

                SuccessOrExit(error = mDecoder.ReadUtf8(name));
                len = strlen(name);
                VerifyOrExit(len <= OT_NETWORK_NAME_MAX_SIZE, error = OT_ERROR_INVALID_ARGS);
                memcpy(aDataset.mNetworkName.m8, name, len + 1);
            }

            aDataset.mComponents.mIsNetworkNamePresent = true;
            break;

        case SPINEL_PROP_NET_XPANID:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const uint8_t *xpanid;
                uint16_t       len;

                SuccessOrExit(error = mDecoder.ReadData(xpanid, len));
                VerifyOrExit(len == OT_EXT_PAN_ID_SIZE, error = OT_ERROR_INVALID_ARGS);
                memcpy(aDataset.mExtendedPanId.m8, xpanid, len);
            }

            aDataset.mComponents.mIsExtendedPanIdPresent = true;
            break;

        case SPINEL_PROP_IPV6_ML_PREFIX:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const otIp6Address *addr;
                uint8_t             prefixLen;

                SuccessOrExit(error = mDecoder.ReadIp6Address(addr));
                SuccessOrExit(error = mDecoder.ReadUint8(prefixLen));
                VerifyOrExit(prefixLen == OT_IP6_PREFIX_BITSIZE, error = OT_ERROR_INVALID_ARGS);
                memcpy(aDataset.mMeshLocalPrefix.m8, addr, OT_MESH_LOCAL_PREFIX_SIZE);
            }

            aDataset.mComponents.mIsMeshLocalPrefixPresent = true;
            break;

        case SPINEL_PROP_DATASET_DELAY_TIMER:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                SuccessOrExit(error = mDecoder.ReadUint32(aDataset.mDelay));
            }

            aDataset.mComponents.mIsDelayPresent = true;
            break;

        case SPINEL_PROP_MAC_15_4_PANID:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                SuccessOrExit(error = mDecoder.ReadUint16(aDataset.mPanId));
            }

            aDataset.mComponents.mIsPanIdPresent = true;
            break;

        case SPINEL_PROP_PHY_CHAN:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                uint8_t channel;

                SuccessOrExit(error = mDecoder.ReadUint8(channel));
                aDataset.mChannel = channel;
            }

            aDataset.mComponents.mIsChannelPresent = true;
            break;

        case SPINEL_PROP_NET_PSKC:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const uint8_t *psk;
                uint16_t       len;

                SuccessOrExit(error = mDecoder.ReadData(psk, len));
                VerifyOrExit(len == OT_PSKC_MAX_SIZE, error = OT_ERROR_INVALID_ARGS);
                memcpy(aDataset.mPskc.m8, psk, OT_PSKC_MAX_SIZE);
            }

            aDataset.mComponents.mIsPskcPresent = true;
            break;

        case SPINEL_PROP_DATASET_SECURITY_POLICY:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                uint8_t flags[2];
                uint8_t flagsLength = 1;

                SuccessOrExit(error = mDecoder.ReadUint16(aDataset.mSecurityPolicy.mRotationTime));
                SuccessOrExit(error = mDecoder.ReadUint8(flags[0]));
                if (otThreadGetVersion() >= OT_THREAD_VERSION_1_2 && mDecoder.GetRemainingLengthInStruct() > 0)
                {
                    SuccessOrExit(error = mDecoder.ReadUint8(flags[1]));
                    ++flagsLength;
                }
                static_cast<SecurityPolicy &>(aDataset.mSecurityPolicy).SetFlags(flags, flagsLength);
            }

            aDataset.mComponents.mIsSecurityPolicyPresent = true;
            break;

        case SPINEL_PROP_PHY_CHAN_SUPPORTED:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                uint8_t channel;

                aDataset.mChannelMask = 0;

                while (!mDecoder.IsAllReadInStruct())
                {
                    SuccessOrExit(error = mDecoder.ReadUint8(channel));
                    VerifyOrExit(channel <= 31, error = OT_ERROR_INVALID_ARGS);
                    aDataset.mChannelMask |= (1UL << channel);
                }
            }

            aDataset.mComponents.mIsChannelMaskPresent = true;
            break;

        case SPINEL_PROP_DATASET_RAW_TLVS:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const uint8_t *tlvs;
                uint16_t       len;

                SuccessOrExit(error = mDecoder.ReadData(tlvs, len));
                VerifyOrExit(len <= 255, error = OT_ERROR_INVALID_ARGS);

                if (aTlvs != nullptr)
                {
                    *aTlvs = tlvs;
                }

                if (aTlvsLength != nullptr)
                {
                    *aTlvsLength = static_cast<uint8_t>(len);
                }
            }

            break;

        case SPINEL_PROP_DATASET_DEST_ADDRESS:

            if (!aAllowEmptyValues || !mDecoder.IsAllReadInStruct())
            {
                const otIp6Address *addr;

                SuccessOrExit(error = mDecoder.ReadIp6Address(addr));

                if (aDestIpAddress != nullptr)
                {
                    *aDestIpAddress = addr;
                }
            }

            break;

        default:
            break;
        }

        SuccessOrExit(error = mDecoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ACTIVE_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;

    SuccessOrExit(error = DecodeOperationalDataset(dataset));
    error = otDatasetSetActive(mInstance, &dataset);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_PENDING_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;

    SuccessOrExit(error = DecodeOperationalDataset(dataset));
    error = otDatasetSetPending(mInstance, &dataset);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MGMT_SET_ACTIVE_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;
    const uint8_t *      extraTlvs;
    uint8_t              extraTlvsLength;

    SuccessOrExit(error = DecodeOperationalDataset(dataset, &extraTlvs, &extraTlvsLength));
    error = otDatasetSendMgmtActiveSet(mInstance, &dataset, extraTlvs, extraTlvsLength, /* aCallback */ nullptr,
                                       /* aContext */ nullptr);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MGMT_SET_PENDING_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;
    const uint8_t *      extraTlvs;
    uint8_t              extraTlvsLength;

    SuccessOrExit(error = DecodeOperationalDataset(dataset, &extraTlvs, &extraTlvsLength));
    error = otDatasetSendMgmtPendingSet(mInstance, &dataset, extraTlvs, extraTlvsLength, /* aCallback */ nullptr,
                                        /* aContext */ nullptr);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MGMT_GET_ACTIVE_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;
    const uint8_t *      extraTlvs;
    uint8_t              extraTlvsLength;
    const otIp6Address * destIpAddress;

    SuccessOrExit(error = DecodeOperationalDataset(dataset, &extraTlvs, &extraTlvsLength, &destIpAddress, true));
    error = otDatasetSendMgmtActiveGet(mInstance, &dataset.mComponents, extraTlvs, extraTlvsLength, destIpAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MGMT_GET_PENDING_DATASET>(void)
{
    otError              error = OT_ERROR_NONE;
    otOperationalDataset dataset;
    const uint8_t *      extraTlvs;
    uint8_t              extraTlvsLength;
    const otIp6Address * destIpAddress;

    SuccessOrExit(error = DecodeOperationalDataset(dataset, &extraTlvs, &extraTlvsLength, &destIpAddress, true));
    error = otDatasetSendMgmtPendingGet(mInstance, &dataset.mComponents, extraTlvs, extraTlvsLength, destIpAddress);

exit:
    return error;
}
#if OPENTHREAD_CONFIG_JOINER_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_JOINER_STATE>(void)
{
    spinel_meshcop_joiner_state_t state = SPINEL_MESHCOP_JOINER_STATE_IDLE;

    switch (otJoinerGetState(mInstance))
    {
    case OT_JOINER_STATE_IDLE:
        state = SPINEL_MESHCOP_JOINER_STATE_IDLE;
        break;
    case OT_JOINER_STATE_DISCOVER:
        state = SPINEL_MESHCOP_JOINER_STATE_DISCOVER;
        break;
    case OT_JOINER_STATE_CONNECT:
        state = SPINEL_MESHCOP_JOINER_STATE_CONNECTING;
        break;
    case OT_JOINER_STATE_CONNECTED:
        state = SPINEL_MESHCOP_JOINER_STATE_CONNECTED;
        break;
    case OT_JOINER_STATE_ENTRUST:
        state = SPINEL_MESHCOP_JOINER_STATE_ENTRUST;
        break;
    case OT_JOINER_STATE_JOINED:
        state = SPINEL_MESHCOP_JOINER_STATE_JOINED;
        break;
    }

    return mEncoder.WriteUint8(state);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_JOINER_COMMISSIONING>(void)
{
    otError     error           = OT_ERROR_NONE;
    bool        action          = false;
    const char *psk             = nullptr;
    const char *provisioningUrl = nullptr;
    const char *vendorName      = nullptr;
    const char *vendorModel     = nullptr;
    const char *vendorSwVersion = nullptr;
    const char *vendorData      = nullptr;

    SuccessOrExit(error = mDecoder.ReadBool(action));

    if (!action)
    {
        otJoinerStop(mInstance);
        ExitNow();
    }

    SuccessOrExit(error = mDecoder.ReadUtf8(psk));

    // Parse optional fields

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadUtf8(provisioningUrl));
    }

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadUtf8(vendorName));
    }

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadUtf8(vendorModel));
    }

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadUtf8(vendorSwVersion));
    }

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadUtf8(vendorData));
    }

    // Use OpenThread default values for vendor name, mode, sw version if
    // not specified or an empty string is given.

    if ((vendorName == nullptr) || (vendorName[0] == 0))
    {
        vendorName = PACKAGE_NAME;
    }

    if ((vendorModel == nullptr) || (vendorModel[0] == 0))
    {
        vendorModel = OPENTHREAD_CONFIG_PLATFORM_INFO;
    }

    if ((vendorSwVersion == nullptr) || (vendorSwVersion[0] == 0))
    {
        vendorSwVersion = PACKAGE_VERSION;
    }

    error = otJoinerStart(mInstance, psk, provisioningUrl, vendorName, vendorModel, vendorSwVersion, vendorData,
                          &NcpBase::HandleJoinerCallback_Jump, this);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_JOINER_DISCERNER>(void)
{
    otError                  error;
    const otJoinerDiscerner *discerner = otJoinerGetDiscerner(mInstance);

    if (discerner == nullptr)
    {
        SuccessOrExit(error = mEncoder.WriteUint8(0));
    }
    else
    {
        SuccessOrExit(error = mEncoder.WriteUint8(discerner->mLength));
        SuccessOrExit(error = mEncoder.WriteUint64(discerner->mValue));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_JOINER_DISCERNER>(void)
{
    otError           error = OT_ERROR_NONE;
    otJoinerDiscerner discerner;

    SuccessOrExit(error = mDecoder.ReadUint8(discerner.mLength));

    if (discerner.mLength == 0)
    {
        // Clearing any previously set Joiner Discerner
        error = otJoinerSetDiscerner(mInstance, nullptr);
        ExitNow();
    }

    SuccessOrExit(error = mDecoder.ReadUint64(discerner.mValue));
    error = otJoinerSetDiscerner(mInstance, &discerner);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_JOINER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ML_PREFIX>(void)
{
    otError                  error    = OT_ERROR_NONE;
    const otMeshLocalPrefix *mlPrefix = otThreadGetMeshLocalPrefix(mInstance);
    otIp6Address             addr;

    VerifyOrExit(mlPrefix != nullptr); // If `mlPrefix` is nullptr send empty response.

    memcpy(addr.mFields.m8, mlPrefix->m8, 8);

    // Zero out the last 8 bytes.
    memset(addr.mFields.m8 + 8, 0, 8);

    SuccessOrExit(error = mEncoder.WriteIp6Address(addr));             // Mesh local prefix
    SuccessOrExit(error = mEncoder.WriteUint8(OT_IP6_PREFIX_BITSIZE)); // Prefix length (in bits)

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_IPV6_ML_PREFIX>(void)
{
    otError             error = OT_ERROR_NONE;
    const otIp6Address *meshLocalPrefix;
    uint8_t             prefixLength;

    SuccessOrExit(error = mDecoder.ReadIp6Address(meshLocalPrefix));
    SuccessOrExit(error = mDecoder.ReadUint8(prefixLength));
    VerifyOrExit(prefixLength == OT_IP6_PREFIX_BITSIZE, error = OT_ERROR_INVALID_ARGS);

    error = otThreadSetMeshLocalPrefix(mInstance, reinterpret_cast<const otMeshLocalPrefix *>(meshLocalPrefix));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ML_ADDR>(void)
{
    otError             error = OT_ERROR_NONE;
    const otIp6Address *ml64  = otThreadGetMeshLocalEid(mInstance);

    VerifyOrExit(ml64 != nullptr);
    SuccessOrExit(error = mEncoder.WriteIp6Address(*ml64));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_LL_ADDR>(void)
{
    otError             error   = OT_ERROR_NONE;
    const otIp6Address *address = otThreadGetLinkLocalIp6Address(mInstance);

    VerifyOrExit(address != nullptr);
    SuccessOrExit(error = mEncoder.WriteIp6Address(*address));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ADDRESS_TABLE>(void)
{
    otError error = OT_ERROR_NONE;

    for (const otNetifAddress *address = otIp6GetUnicastAddresses(mInstance); address; address = address->mNext)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteIp6Address(address->mAddress));
        SuccessOrExit(error = mEncoder.WriteUint8(address->mPrefixLength));
        SuccessOrExit(error = mEncoder.WriteUint32(address->mPreferred ? 0xffffffff : 0));
        SuccessOrExit(error = mEncoder.WriteUint32(address->mValid ? 0xffffffff : 0));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_IPV6_ADDRESS_TABLE>(void)
{
    otError        error = OT_ERROR_NONE;
    otNetifAddress netifAddr;
    uint32_t       preferredLifetime;
    uint32_t       validLifetime;

    SuccessOrExit(error = mDecoder.ReadIp6Address(netifAddr.mAddress));
    SuccessOrExit(error = mDecoder.ReadUint8(netifAddr.mPrefixLength));
    SuccessOrExit(error = mDecoder.ReadUint32(preferredLifetime));
    SuccessOrExit(error = mDecoder.ReadUint32(validLifetime));

    netifAddr.mAddressOrigin = OT_ADDRESS_ORIGIN_MANUAL;
    netifAddr.mPreferred     = (preferredLifetime != 0);
    netifAddr.mValid         = (validLifetime != 0);

    error = otIp6AddUnicastAddress(mInstance, &netifAddr);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_IPV6_ADDRESS_TABLE>(void)
{
    otError             error = OT_ERROR_NONE;
    const otIp6Address *addrPtr;

    SuccessOrExit(error = mDecoder.ReadIp6Address(addrPtr));

    error = otIp6RemoveUnicastAddress(mInstance, addrPtr);

    // If address was not on the list, "remove" command is successful.
    if (error == OT_ERROR_NOT_FOUND)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ROUTE_TABLE>(void)
{
    // TODO: Implement get route table
    return mEncoder.OverwriteWithLastStatusError(SPINEL_STATUS_UNIMPLEMENTED);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD>(void)
{
    return mEncoder.WriteBool(otIcmp6GetEchoMode(mInstance) != OT_ICMP6_ECHO_HANDLER_DISABLED);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD>(void)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    otIcmp6SetEchoMode(mInstance, enabled ? OT_ICMP6_ECHO_HANDLER_ALL : OT_ICMP6_ECHO_HANDLER_DISABLED);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE>(void)
{
    otError                        error = OT_ERROR_NONE;
    const otNetifMulticastAddress *address;

    for (address = otIp6GetMulticastAddresses(mInstance); address; address = address->mNext)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteIp6Address(address->mAddress));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE>(void)
{
    otError             error = OT_ERROR_NONE;
    const otIp6Address *addrPtr;

    SuccessOrExit(error = mDecoder.ReadIp6Address(addrPtr));

    error = otIp6SubscribeMulticastAddress(mInstance, addrPtr);

    if (error == OT_ERROR_ALREADY)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE>(void)
{
    otError             error = OT_ERROR_NONE;
    const otIp6Address *addrPtr;

    SuccessOrExit(error = mDecoder.ReadIp6Address(addrPtr));

    error = otIp6UnsubscribeMulticastAddress(mInstance, addrPtr);

    // If the address was not on the list, "remove" command is successful,
    // and we respond with a `SPINEL_STATUS_OK` status.
    if (error == OT_ERROR_NOT_FOUND)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD_MODE>(void)
{
    spinel_ipv6_icmp_ping_offload_mode_t mode = SPINEL_IPV6_ICMP_PING_OFFLOAD_DISABLED;

    switch (otIcmp6GetEchoMode(mInstance))
    {
    case OT_ICMP6_ECHO_HANDLER_DISABLED:
        mode = SPINEL_IPV6_ICMP_PING_OFFLOAD_DISABLED;
        break;
    case OT_ICMP6_ECHO_HANDLER_UNICAST_ONLY:
        mode = SPINEL_IPV6_ICMP_PING_OFFLOAD_UNICAST_ONLY;
        break;
    case OT_ICMP6_ECHO_HANDLER_MULTICAST_ONLY:
        mode = SPINEL_IPV6_ICMP_PING_OFFLOAD_MULTICAST_ONLY;
        break;
    case OT_ICMP6_ECHO_HANDLER_ALL:
        mode = SPINEL_IPV6_ICMP_PING_OFFLOAD_ALL;
        break;
    };

    return mEncoder.WriteUint8(mode);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_IPV6_ICMP_PING_OFFLOAD_MODE>(void)
{
    otError         error = OT_ERROR_NONE;
    otIcmp6EchoMode mode  = OT_ICMP6_ECHO_HANDLER_DISABLED;
    uint8_t         spinelMode;

    SuccessOrExit(error = mDecoder.ReadUint8(spinelMode));

    switch (spinelMode)
    {
    case SPINEL_IPV6_ICMP_PING_OFFLOAD_DISABLED:
        mode = OT_ICMP6_ECHO_HANDLER_DISABLED;
        break;
    case SPINEL_IPV6_ICMP_PING_OFFLOAD_UNICAST_ONLY:
        mode = OT_ICMP6_ECHO_HANDLER_UNICAST_ONLY;
        break;
    case SPINEL_IPV6_ICMP_PING_OFFLOAD_MULTICAST_ONLY:
        mode = OT_ICMP6_ECHO_HANDLER_MULTICAST_ONLY;
        break;
    case SPINEL_IPV6_ICMP_PING_OFFLOAD_ALL:
        mode = OT_ICMP6_ECHO_HANDLER_ALL;
        break;
    };

    otIcmp6SetEchoMode(mInstance, mode);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_RLOC16_DEBUG_PASSTHRU>(void)
{
    // Note reverse logic: passthru enabled = filter disabled
    return mEncoder.WriteBool(!otIp6IsReceiveFilterEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_RLOC16_DEBUG_PASSTHRU>(void)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    // Note reverse logic: passthru enabled = filter disabled
    otIp6SetReceiveFilterEnabled(mInstance, !enabled);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_OFF_MESH_ROUTES>(void)
{
    otError               error = OT_ERROR_NONE;
    otExternalRouteConfig routeConfig;
    otNetworkDataIterator iter = OT_NETWORK_DATA_ITERATOR_INIT;

    while (otNetDataGetNextRoute(mInstance, &iter, &routeConfig) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteIp6Address(routeConfig.mPrefix.mPrefix));
        SuccessOrExit(error = mEncoder.WriteUint8(routeConfig.mPrefix.mLength));
        SuccessOrExit(error = mEncoder.WriteBool(routeConfig.mStable));
        SuccessOrExit(error = mEncoder.WriteUint8(ExternalRouteConfigToFlagByte(routeConfig)));
        SuccessOrExit(error = mEncoder.WriteBool(false)); // IsLocal
        SuccessOrExit(error = mEncoder.WriteBool(routeConfig.mNextHopIsThisDevice));
        SuccessOrExit(error = mEncoder.WriteUint16(routeConfig.mRloc16));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

    iter = OT_NETWORK_DATA_ITERATOR_INIT;

    while (otBorderRouterGetNextRoute(mInstance, &iter, &routeConfig) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteIp6Address(routeConfig.mPrefix.mPrefix));
        SuccessOrExit(error = mEncoder.WriteUint8(routeConfig.mPrefix.mLength));
        SuccessOrExit(error = mEncoder.WriteBool(routeConfig.mStable));
        SuccessOrExit(error = mEncoder.WriteUint8(ExternalRouteConfigToFlagByte(routeConfig)));
        SuccessOrExit(error = mEncoder.WriteBool(true)); // IsLocal
        SuccessOrExit(error = mEncoder.WriteBool(routeConfig.mNextHopIsThisDevice));
        SuccessOrExit(error = mEncoder.WriteUint16(routeConfig.mRloc16));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

exit:
    return error;
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
static int FlagByteToExternalRoutePreference(uint8_t aFlags)
{
    int route_preference = 0;

    switch (aFlags & SPINEL_NET_FLAG_PREFERENCE_MASK)
    {
    case SPINEL_ROUTE_PREFERENCE_HIGH:
        route_preference = OT_ROUTE_PREFERENCE_HIGH;
        break;

    case SPINEL_ROUTE_PREFERENCE_MEDIUM:
        route_preference = OT_ROUTE_PREFERENCE_MED;
        break;

    case SPINEL_ROUTE_PREFERENCE_LOW:
        route_preference = OT_ROUTE_PREFERENCE_LOW;
        break;
    }

    return route_preference;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_THREAD_OFF_MESH_ROUTES>(void)
{
    otError               error = OT_ERROR_NONE;
    otExternalRouteConfig routeConfig;
    bool                  stable = false;
    uint8_t               flags  = 0;
    uint8_t               prefixLength;

    memset(&routeConfig, 0, sizeof(otExternalRouteConfig));

    VerifyOrExit(mAllowLocalNetworkDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadIp6Address(routeConfig.mPrefix.mPrefix));
    SuccessOrExit(error = mDecoder.ReadUint8(prefixLength));
    SuccessOrExit(error = mDecoder.ReadBool(stable));
    SuccessOrExit(error = mDecoder.ReadUint8(flags));

    routeConfig.mPrefix.mLength = prefixLength;
    routeConfig.mStable         = stable;
    routeConfig.mPreference     = FlagByteToExternalRoutePreference(flags);
    routeConfig.mNat64          = ((flags & SPINEL_ROUTE_FLAG_NAT64) != 0);

    error = otBorderRouterAddRoute(mInstance, &routeConfig);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_THREAD_OFF_MESH_ROUTES>(void)
{
    otError     error = OT_ERROR_NONE;
    otIp6Prefix ip6Prefix;
    uint8_t     prefixLength;

    memset(&ip6Prefix, 0, sizeof(otIp6Prefix));

    VerifyOrExit(mAllowLocalNetworkDataChange, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = mDecoder.ReadIp6Address(ip6Prefix.mPrefix));
    SuccessOrExit(error = mDecoder.ReadUint8(prefixLength));

    ip6Prefix.mLength = prefixLength;

    error = otBorderRouterRemoveRoute(mInstance, &ip6Prefix);

    // If the route prefix was not on the list, "remove" command is successful.
    if (error == OT_ERROR_NOT_FOUND)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_STREAM_NET>(void)
{
    const uint8_t *framePtr = nullptr;
    uint16_t       frameLen = 0;
    const uint8_t *metaPtr  = nullptr;
    uint16_t       metaLen  = 0;
    otMessage *    message  = nullptr;
    otError        error    = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadDataWithLen(framePtr, frameLen));
    SuccessOrExit(error = mDecoder.ReadData(metaPtr, metaLen));

    // We ignore metadata for now.
    // May later include TX power, allow retransmits, etc...

    // STREAM_NET requires layer 2 security.
    message = otIp6NewMessageFromBuffer(mInstance, framePtr, frameLen, nullptr);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    error = otIp6Send(mInstance, message);

exit:

    if (error == OT_ERROR_NONE)
    {
        mInboundSecureIpFrameCounter++;
    }
    else
    {
        mDroppedInboundIpFrameCounter++;
    }

    return error;
}

#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECT_ENABLE>(void)
{
    return mEncoder.WriteBool(otJamDetectionIsEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECTED>(void)
{
    return mEncoder.WriteBool(otJamDetectionGetState(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECT_RSSI_THRESHOLD>(void)
{
    return mEncoder.WriteInt8(otJamDetectionGetRssiThreshold(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECT_WINDOW>(void)
{
    return mEncoder.WriteUint8(otJamDetectionGetWindow(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECT_BUSY>(void)
{
    return mEncoder.WriteUint8(otJamDetectionGetBusyPeriod(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_JAM_DETECT_HISTORY_BITMAP>(void)
{
    return mEncoder.WriteUint64(otJamDetectionGetHistoryBitmap(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_JAM_DETECT_ENABLE>(void)
{
    bool    enabled;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    if (enabled)
    {
        IgnoreError(otJamDetectionStart(mInstance, &NcpBase::HandleJamStateChange_Jump, this));
    }
    else
    {
        IgnoreError(otJamDetectionStop(mInstance));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_JAM_DETECT_RSSI_THRESHOLD>(void)
{
    int8_t  threshold = 0;
    otError error     = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadInt8(threshold));

    error = otJamDetectionSetRssiThreshold(mInstance, threshold);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_JAM_DETECT_WINDOW>(void)
{
    uint8_t window = 0;
    otError error  = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(window));

    error = otJamDetectionSetWindow(mInstance, window);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_JAM_DETECT_BUSY>(void)
{
    uint8_t busy  = 0;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(busy));

    error = otJamDetectionSetBusyPeriod(mInstance, busy);

exit:
    return error;
}

void NcpBase::HandleJamStateChange_Jump(bool aJamState, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleJamStateChange(aJamState);
}

void NcpBase::HandleJamStateChange(bool aJamState)
{
    OT_UNUSED_VARIABLE(aJamState);

    mChangedPropsSet.AddProperty(SPINEL_PROP_JAM_DETECTED);
    mUpdateChangedPropsTask.Post();
}

#endif // OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHILD_SUPERVISION_CHECK_TIMEOUT>(void)
{
    return mEncoder.WriteUint16(otChildSupervisionGetCheckTimeout(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHILD_SUPERVISION_CHECK_TIMEOUT>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t timeout;

    SuccessOrExit(error = mDecoder.ReadUint16(timeout));
    otChildSupervisionSetCheckTimeout(mInstance, timeout);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_INTERVAL>(void)
{
    return mEncoder.WriteUint32(otChannelMonitorGetSampleInterval(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MONITOR_RSSI_THRESHOLD>(void)
{
    return mEncoder.WriteInt8(otChannelMonitorGetRssiThreshold(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_WINDOW>(void)
{
    return mEncoder.WriteUint32(otChannelMonitorGetSampleWindow(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MONITOR_SAMPLE_COUNT>(void)
{
    return mEncoder.WriteUint32(otChannelMonitorGetSampleCount(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MONITOR_CHANNEL_OCCUPANCY>(void)
{
    otError  error       = OT_ERROR_NONE;
    uint32_t channelMask = otLinkGetSupportedChannelMask(mInstance);
    uint8_t  channelNum  = sizeof(channelMask) * CHAR_BIT;

    for (uint8_t channel = 0; channel < channelNum; channel++)
    {
        if (!((1UL << channel) & channelMask))
        {
            continue;
        }

        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteUint8(channel));
        SuccessOrExit(error = mEncoder.WriteUint16(otChannelMonitorGetChannelOccupancy(mInstance, channel)));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_CCA_FAILURE_RATE>(void)
{
    return mEncoder.WriteUint16(otLinkGetCcaFailureRate(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_TOTAL>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxTotal);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_ACK_REQ>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxAckRequested);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_ACKED>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxAcked);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_NO_ACK_REQ>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxNoAckRequested);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_DATA>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxData);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_DATA_POLL>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxDataPoll);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_BEACON>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxBeacon);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_BEACON_REQ>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxBeaconRequest);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_OTHER>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxOther);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_RETRY>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxRetry);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_ERR_CCA>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxErrCca);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_UNICAST>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxUnicast);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_PKT_BROADCAST>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxBroadcast);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_ERR_ABORT>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mTxErrAbort);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_TOTAL>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxTotal);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_DATA>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxData);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_DATA_POLL>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxDataPoll);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_BEACON>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxBeacon);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_BEACON_REQ>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxBeaconRequest);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_OTHER>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxOther);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_FILT_WL>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxAddressFiltered);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_FILT_DA>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxDestAddrFiltered);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_DUP>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxDuplicated);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_UNICAST>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxUnicast);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_PKT_BROADCAST>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxBroadcast);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_EMPTY>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrNoFrame);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_UKWN_NBR>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrUnknownNeighbor);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_NVLD_SADDR>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrInvalidSrcAddr);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_SECURITY>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrSec);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_BAD_FCS>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrFcs);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_ERR_OTHER>(void)
{
    return mEncoder.WriteUint32(otLinkGetCounters(mInstance)->mRxErrOther);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_IP_SEC_TOTAL>(void)
{
    return mEncoder.WriteUint32(mInboundSecureIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_IP_INSEC_TOTAL>(void)
{
    return mEncoder.WriteUint32(mInboundInsecureIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_IP_DROPPED>(void)
{
    return mEncoder.WriteUint32(mDroppedInboundIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_IP_SEC_TOTAL>(void)
{
    return mEncoder.WriteUint32(mOutboundSecureIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_IP_INSEC_TOTAL>(void)
{
    return mEncoder.WriteUint32(mOutboundInsecureIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_IP_DROPPED>(void)
{
    return mEncoder.WriteUint32(mDroppedOutboundIpFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_TX_SPINEL_TOTAL>(void)
{
    return mEncoder.WriteUint32(mTxSpinelFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_SPINEL_TOTAL>(void)
{
    return mEncoder.WriteUint32(mRxSpinelFrameCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_SPINEL_OUT_OF_ORDER_TID>(void)
{
    return mEncoder.WriteUint32(mRxSpinelOutOfOrderTidCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_RX_SPINEL_ERR>(void)
{
    return mEncoder.WriteUint32(mFramingErrorCounter);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_IP_TX_SUCCESS>(void)
{
    return mEncoder.WriteUint32(otThreadGetIp6Counters(mInstance)->mTxSuccess);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_IP_RX_SUCCESS>(void)
{
    return mEncoder.WriteUint32(otThreadGetIp6Counters(mInstance)->mRxSuccess);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_IP_TX_FAILURE>(void)
{
    return mEncoder.WriteUint32(otThreadGetIp6Counters(mInstance)->mTxFailure);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_IP_RX_FAILURE>(void)
{
    return mEncoder.WriteUint32(otThreadGetIp6Counters(mInstance)->mRxFailure);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MSG_BUFFER_COUNTERS>(void)
{
    otError      error;
    otBufferInfo bufferInfo;

    otMessageGetBufferInfo(mInstance, &bufferInfo);

    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mTotalBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mFreeBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.m6loSendMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.m6loSendBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.m6loReassemblyMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.m6loReassemblyBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mIp6Messages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mIp6Buffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mMplMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mMplBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mMleMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mMleBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mArpMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mArpBuffers));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mCoapMessages));
    SuccessOrExit(error = mEncoder.WriteUint16(bufferInfo.mCoapBuffers));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_ALL_MAC_COUNTERS>(void)
{
    otError              error    = OT_ERROR_NONE;
    const otMacCounters *counters = otLinkGetCounters(mInstance);

    // Encode Tx related counters
    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxTotal));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxUnicast));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxBroadcast));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxAckRequested));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxAcked));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxNoAckRequested));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxData));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxDataPoll));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxBeacon));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxBeaconRequest));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxOther));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxRetry));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxErrCca));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxErrAbort));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxErrBusyChannel));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxDirectMaxRetryExpiry));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxIndirectMaxRetryExpiry));
    SuccessOrExit(error = mEncoder.CloseStruct());

    // Encode Rx related counters
    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxTotal));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxUnicast));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxBroadcast));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxData));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxDataPoll));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxBeacon));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxBeaconRequest));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxOther));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxAddressFiltered));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxDestAddrFiltered));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxDuplicated));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrNoFrame));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrUnknownNeighbor));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrInvalidSrcAddr));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrSec));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrFcs));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxErrOther));
    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CNTR_ALL_MAC_COUNTERS>(void)
{
    otLinkResetCounters(mInstance);

    return OT_ERROR_NONE;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_MLE_COUNTERS>(void)
{
    otError              error    = OT_ERROR_NONE;
    const otMleCounters *counters = otThreadGetMleCounters(mInstance);

    OT_ASSERT(counters != nullptr);

    SuccessOrExit(error = mEncoder.WriteUint16(counters->mDisabledRole));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mDetachedRole));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mChildRole));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mRouterRole));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mLeaderRole));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mAttachAttempts));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mPartitionIdChanges));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mBetterPartitionAttachAttempts));
    SuccessOrExit(error = mEncoder.WriteUint16(counters->mParentChanges));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CNTR_MLE_COUNTERS>(void)
{
    otThreadResetMleCounters(mInstance);

    return OT_ERROR_NONE;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_ALL_IP_COUNTERS>(void)
{
    otError             error    = OT_ERROR_NONE;
    const otIpCounters *counters = otThreadGetIp6Counters(mInstance);

    OT_ASSERT(counters != nullptr);

    // Encode Tx related counters
    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxSuccess));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mTxFailure));
    SuccessOrExit(error = mEncoder.CloseStruct());

    // Encode Rx related counters
    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxSuccess));
    SuccessOrExit(error = mEncoder.WriteUint32(counters->mRxFailure));
    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CNTR_MAC_RETRY_HISTOGRAM>(void)
{
    otError         error = OT_ERROR_NONE;
    const uint32_t *histogramDirect;
    const uint32_t *histogramIndirect;
    uint8_t         histogramDirectEntries;
    uint8_t         histogramIndirectEntries;

    histogramDirect   = otLinkGetTxDirectRetrySuccessHistogram(mInstance, &histogramDirectEntries);
    histogramIndirect = otLinkGetTxIndirectRetrySuccessHistogram(mInstance, &histogramIndirectEntries);

    OT_ASSERT((histogramDirectEntries == 0) || (histogramDirect != nullptr));
    OT_ASSERT((histogramIndirectEntries == 0) || (histogramIndirect != nullptr));

    // Encode direct message retries histogram
    SuccessOrExit(error = mEncoder.OpenStruct());
    for (uint8_t i = 0; i < histogramDirectEntries; i++)
    {
        SuccessOrExit(error = mEncoder.WriteUint32(histogramDirect[i]));
    }
    SuccessOrExit(error = mEncoder.CloseStruct());

    // Encode indirect message retries histogram
    SuccessOrExit(error = mEncoder.OpenStruct());
    for (uint8_t i = 0; i < histogramIndirectEntries; i++)
    {
        SuccessOrExit(error = mEncoder.WriteUint32(histogramIndirect[i]));
    }
    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CNTR_MAC_RETRY_HISTOGRAM>(void)
{
    otLinkResetTxRetrySuccessHistogram(mInstance);

    return OT_ERROR_NONE;
}
#endif // OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CNTR_ALL_IP_COUNTERS>(void)
{
    otThreadResetIp6Counters(mInstance);

    return OT_ERROR_NONE;
}

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_ALLOWLIST>(void)
{
    otMacFilterEntry    entry;
    otMacFilterIterator iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otError             error    = OT_ERROR_NONE;

    while (otLinkFilterGetNextAddress(mInstance, &iterator, &entry) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(entry.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteInt8(entry.mRssIn));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_ALLOWLIST_ENABLED>(void)
{
    return mEncoder.WriteBool(otLinkFilterGetAddressMode(mInstance) == OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_DENYLIST>(void)
{
    otMacFilterEntry    entry;
    otMacFilterIterator iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otError             error    = OT_ERROR_NONE;

    while (otLinkFilterGetNextAddress(mInstance, &iterator, &entry) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteEui64(entry.mExtAddress));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_DENYLIST_ENABLED>(void)
{
    return mEncoder.WriteBool(otLinkFilterGetAddressMode(mInstance) == OT_MAC_FILTER_ADDRESS_MODE_DENYLIST);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_FIXED_RSS>(void)
{
    otMacFilterEntry    entry;
    otMacFilterIterator iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otError             error    = OT_ERROR_NONE;

    while (otLinkFilterGetNextRssIn(mInstance, &iterator, &entry) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(entry.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteInt8(entry.mRssIn));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_ALLOWLIST>(void)
{
    otError error = OT_ERROR_NONE;

    // First, clear the address filter entries.
    otLinkFilterClearAddresses(mInstance);

    while (mDecoder.GetRemainingLengthInStruct() > 0)
    {
        const otExtAddress *extAddress = nullptr;
        int8_t              rss;

        SuccessOrExit(error = mDecoder.OpenStruct());
        SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

        if (!mDecoder.IsAllReadInStruct())
        {
            SuccessOrExit(error = mDecoder.ReadInt8(rss));
        }
        else
        {
            rss = OT_MAC_FILTER_FIXED_RSS_DISABLED;
        }

        SuccessOrExit(error = mDecoder.CloseStruct());

        error = otLinkFilterAddAddress(mInstance, extAddress);

        if (error == OT_ERROR_ALREADY)
        {
            error = OT_ERROR_NONE;
        }

        SuccessOrExit(error);

        if (rss != OT_MAC_FILTER_FIXED_RSS_DISABLED)
        {
            SuccessOrExit(error = otLinkFilterAddRssIn(mInstance, extAddress, rss));
        }
    }

exit:
    // If we had an error, we may have actually changed
    // the state of the allowlist, so we need to report
    // those incomplete changes via an asynchronous
    // change event.

    if (error != OT_ERROR_NONE)
    {
        IgnoreError(WritePropertyValueIsFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_PROP_MAC_ALLOWLIST));
    }

    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_ALLOWLIST_ENABLED>(void)
{
    bool                   enabled;
    otError                error = OT_ERROR_NONE;
    otMacFilterAddressMode mode  = OT_MAC_FILTER_ADDRESS_MODE_DISABLED;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    if (enabled)
    {
        mode = OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST;
    }

    otLinkFilterSetAddressMode(mInstance, mode);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_DENYLIST>(void)
{
    otError error = OT_ERROR_NONE;

    // First, clear the address filter entries.
    otLinkFilterClearAddresses(mInstance);

    while (mDecoder.GetRemainingLengthInStruct() > 0)
    {
        const otExtAddress *extAddress = nullptr;

        SuccessOrExit(error = mDecoder.OpenStruct());
        SuccessOrExit(error = mDecoder.ReadEui64(extAddress));
        SuccessOrExit(error = mDecoder.CloseStruct());

        SuccessOrExit(error = otLinkFilterAddAddress(mInstance, extAddress));
    }

exit:
    // If we had an error, we may have actually changed
    // the state of the denylist, so we need to report
    // those incomplete changes via an asynchronous
    // change event.

    if (error != OT_ERROR_NONE)
    {
        IgnoreError(WritePropertyValueIsFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_PROP_MAC_DENYLIST));
    }

    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_DENYLIST_ENABLED>(void)
{
    bool                   enabled;
    otError                error = OT_ERROR_NONE;
    otMacFilterAddressMode mode  = OT_MAC_FILTER_ADDRESS_MODE_DISABLED;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    if (enabled)
    {
        mode = OT_MAC_FILTER_ADDRESS_MODE_DENYLIST;
    }

    otLinkFilterSetAddressMode(mInstance, mode);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_FIXED_RSS>(void)
{
    otError error = OT_ERROR_NONE;

    // First, clear the address filter entries.
    otLinkFilterClearAllRssIn(mInstance);

    while (mDecoder.GetRemainingLengthInStruct() > 0)
    {
        const otExtAddress *extAddress;
        int8_t              rss;

        SuccessOrExit(error = mDecoder.OpenStruct());

        if (mDecoder.GetRemainingLengthInStruct() > sizeof(otExtAddress))
        {
            SuccessOrExit(error = mDecoder.ReadEui64(extAddress));
        }
        else
        {
            extAddress = nullptr;
        }

        SuccessOrExit(error = mDecoder.ReadInt8(rss));

        SuccessOrExit(error = mDecoder.CloseStruct());

        if (extAddress != nullptr)
        {
            SuccessOrExit(error = otLinkFilterAddRssIn(mInstance, extAddress, rss));
        }
        else
        {
            otLinkFilterSetDefaultRssIn(mInstance, rss);
        }
    }

exit:
    // If we had an error, we may have actually changed
    // the state of the RssIn filter, so we need to report
    // those incomplete changes via an asynchronous
    // change event.

    if (error != OT_ERROR_NONE)
    {
        IgnoreError(WritePropertyValueIsFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_PROP_MAC_FIXED_RSS));
    }

    return error;
}
#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_LINK_METRICS_QUERY>(void)
{
    otError             error = OT_ERROR_NONE;
    struct otIp6Address address;
    uint8_t             seriesId;
    otLinkMetrics       linkMetrics = {};

    SuccessOrExit(error = mDecoder.ReadIp6Address(address));
    SuccessOrExit(error = mDecoder.ReadUint8(seriesId));
    SuccessOrExit(error = DecodeLinkMetrics(&linkMetrics, true));

    error =
        otLinkMetricsQuery(mInstance, &address, seriesId, &linkMetrics, &NcpBase::HandleLinkMetricsReport_Jump, this);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_LINK_METRICS_PROBE>(void)
{
    otError             error = OT_ERROR_NONE;
    struct otIp6Address address;
    uint8_t             seriesId;
    uint8_t             length;

    SuccessOrExit(error = mDecoder.ReadIp6Address(address));
    SuccessOrExit(error = mDecoder.ReadUint8(seriesId));
    SuccessOrExit(error = mDecoder.ReadUint8(length));

    error = otLinkMetricsSendLinkProbe(mInstance, &address, seriesId, length);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK>(void)
{
    otError             error = OT_ERROR_NONE;
    struct otIp6Address address;
    uint8_t             controlFlags;
    otLinkMetrics       linkMetrics = {};

    SuccessOrExit(error = mDecoder.ReadIp6Address(address));
    SuccessOrExit(error = mDecoder.ReadUint8(controlFlags));
    SuccessOrExit(error = DecodeLinkMetrics(&linkMetrics, false));

    error = otLinkMetricsConfigEnhAckProbing(mInstance, &address, static_cast<otLinkMetricsEnhAckFlags>(controlFlags),
                                             controlFlags ? &linkMetrics : nullptr,
                                             &NcpBase::HandleLinkMetricsMgmtResponse_Jump, this,
                                             &NcpBase::HandleLinkMetricsEnhAckProbingIeReport_Jump, this);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_LINK_METRICS_MGMT_FORWARD>(void)
{
    otError                  error = OT_ERROR_NONE;
    struct otIp6Address      address;
    uint8_t                  seriesId;
    uint8_t                  types;
    otLinkMetrics            linkMetrics = {};
    otLinkMetricsSeriesFlags seriesFlags = {};

    SuccessOrExit(error = mDecoder.ReadIp6Address(address));
    SuccessOrExit(error = mDecoder.ReadUint8(seriesId));
    SuccessOrExit(error = mDecoder.ReadUint8(types));

    SuccessOrExit(error = DecodeLinkMetrics(&linkMetrics, true));

    if (types & SPINEL_THREAD_FRAME_TYPE_MLE_LINK_PROBE)
    {
        seriesFlags.mLinkProbe = true;
    }

    if (types & SPINEL_THREAD_FRAME_TYPE_MAC_DATA)
    {
        seriesFlags.mMacData = true;
    }

    if (types & SPINEL_THREAD_FRAME_TYPE_MAC_DATA_REQUEST)
    {
        seriesFlags.mMacDataRequest = true;
    }

    if (types & SPINEL_THREAD_FRAME_TYPE_MAC_ACK)
    {
        seriesFlags.mMacAck = true;
    }

    error = otLinkMetricsConfigForwardTrackingSeries(mInstance, &address, seriesId, seriesFlags, &linkMetrics,
                                                     &NcpBase::HandleLinkMetricsMgmtResponse_Jump, this);

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_MODE>(void)
{
    uint8_t          numericMode;
    otLinkModeConfig modeConfig = otThreadGetLinkMode(mInstance);

    numericMode = LinkFlagsToFlagByte(modeConfig.mRxOnWhenIdle, modeConfig.mDeviceType, modeConfig.mNetworkData);

    return mEncoder.WriteUint8(numericMode);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_MODE>(void)
{
    uint8_t          numericMode = 0;
    otLinkModeConfig modeConfig;
    otError          error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(numericMode));

    modeConfig.mRxOnWhenIdle =
        ((numericMode & SPINEL_THREAD_MODE_RX_ON_WHEN_IDLE) == SPINEL_THREAD_MODE_RX_ON_WHEN_IDLE);
    modeConfig.mDeviceType = ((numericMode & SPINEL_THREAD_MODE_FULL_THREAD_DEV) == SPINEL_THREAD_MODE_FULL_THREAD_DEV);
    modeConfig.mNetworkData =
        ((numericMode & SPINEL_THREAD_MODE_FULL_NETWORK_DATA) == SPINEL_THREAD_MODE_FULL_NETWORK_DATA);

    error = otThreadSetLinkMode(mInstance, modeConfig);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CHILD_TIMEOUT>(void)
{
    return mEncoder.WriteUint32(otThreadGetChildTimeout(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CHILD_TIMEOUT>(void)
{
    uint32_t timeout = 0;
    otError  error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(timeout));

    otThreadSetChildTimeout(mInstance, timeout);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_RLOC16>(void)
{
    return mEncoder.WriteUint16(otThreadGetRloc16(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING>(void)
{
    return mEncoder.WriteBool(mRequireJoinExistingNetwork);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING>(void)
{
    return mDecoder.ReadBool(mRequireJoinExistingNetwork);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_STREAM_NET_INSECURE>(void)
{
    const uint8_t *   framePtr    = nullptr;
    uint16_t          frameLen    = 0;
    const uint8_t *   metaPtr     = nullptr;
    uint16_t          metaLen     = 0;
    otMessage *       message     = nullptr;
    otError           error       = OT_ERROR_NONE;
    otMessageSettings msgSettings = {false, OT_MESSAGE_PRIORITY_NORMAL};

    SuccessOrExit(error = mDecoder.ReadDataWithLen(framePtr, frameLen));
    SuccessOrExit(error = mDecoder.ReadData(metaPtr, metaLen));

    // We ignore metadata for now.
    // May later include TX power, allow retransmits, etc...

    // STREAM_NET_INSECURE packets are not secured at layer 2.
    message = otIp6NewMessageFromBuffer(mInstance, framePtr, frameLen, &msgSettings);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    // Ensure the insecure message is forwarded using direct transmission.
    otMessageSetDirectTransmission(message, true);

    error = otIp6Send(mInstance, message);

exit:
    if (error == OT_ERROR_NONE)
    {
        mInboundInsecureIpFrameCounter++;
    }
    else
    {
        mDroppedInboundIpFrameCounter++;
    }

    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CNTR_RESET>(void)
{
    otLinkResetCounters(mInstance);
#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
    otLinkResetTxRetrySuccessHistogram(mInstance);
#endif
    otThreadResetIp6Counters(mInstance);
    otThreadResetMleCounters(mInstance);
    ResetCounters();

    return OT_ERROR_NONE;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_THREAD_ASSISTING_PORTS>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t port;

    SuccessOrExit(error = mDecoder.ReadUint16(port));

    error = otIp6AddUnsecurePort(mInstance, port);
exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MAC_ALLOWLIST>(void)
{
    otError             error = OT_ERROR_NONE;
    const otExtAddress *extAddress;
    int8_t              rss = OT_MAC_FILTER_FIXED_RSS_DISABLED;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

    if (!mDecoder.IsAllRead())
    {
        SuccessOrExit(error = mDecoder.ReadInt8(rss));
    }

    error = otLinkFilterAddAddress(mInstance, extAddress);

    if (error == OT_ERROR_ALREADY)
    {
        error = OT_ERROR_NONE;
    }

    SuccessOrExit(error);

    if (rss != OT_MAC_FILTER_FIXED_RSS_DISABLED)
    {
        error = otLinkFilterAddRssIn(mInstance, extAddress, rss);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MAC_DENYLIST>(void)
{
    otError             error = OT_ERROR_NONE;
    const otExtAddress *extAddress;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

    error = otLinkFilterAddAddress(mInstance, extAddress);

    if (error == OT_ERROR_ALREADY)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MAC_FIXED_RSS>(void)
{
    otError             error      = OT_ERROR_NONE;
    const otExtAddress *extAddress = nullptr;
    int8_t              rss        = OT_MAC_FILTER_FIXED_RSS_DISABLED;

    if (mDecoder.GetRemainingLength() > sizeof(int8_t))
    {
        SuccessOrExit(error = mDecoder.ReadEui64(extAddress));
    }

    SuccessOrExit(error = mDecoder.ReadInt8(rss));

    if (extAddress != nullptr)
    {
        error = otLinkFilterAddRssIn(mInstance, extAddress, rss);
    }
    else
    {
        otLinkFilterSetDefaultRssIn(mInstance, rss);
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_THREAD_ASSISTING_PORTS>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t port;

    SuccessOrExit(error = mDecoder.ReadUint16(port));

    error = otIp6RemoveUnsecurePort(mInstance, port);

    // If unsecure port was not on the list, "remove" command is successful.
    if (error == OT_ERROR_NOT_FOUND)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MAC_ALLOWLIST>(void)
{
    otError             error      = OT_ERROR_NONE;
    const otExtAddress *extAddress = nullptr;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

    otLinkFilterRemoveAddress(mInstance, extAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MAC_DENYLIST>(void)
{
    otError             error      = OT_ERROR_NONE;
    const otExtAddress *extAddress = nullptr;

    SuccessOrExit(error = mDecoder.ReadEui64(extAddress));

    otLinkFilterRemoveAddress(mInstance, extAddress);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MAC_FIXED_RSS>(void)
{
    otError             error      = OT_ERROR_NONE;
    const otExtAddress *extAddress = nullptr;

    if (mDecoder.GetRemainingLength() > 0)
    {
        SuccessOrExit(error = mDecoder.ReadEui64(extAddress));
    }

    if (extAddress != nullptr)
    {
        otLinkFilterRemoveRssIn(mInstance, extAddress);
    }
    else
    {
        otLinkFilterClearDefaultRssIn(mInstance);
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

#if OPENTHREAD_PLATFORM_POSIX

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_RCP_VERSION>(void)
{
    return mEncoder.WriteUtf8(otGetRadioVersionString(mInstance));
}

#endif

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SLAAC_ENABLED>(void)
{
    return mEncoder.WriteBool(otIp6IsSlaacEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SLAAC_ENABLED>(void)
{
    otError error = OT_ERROR_NONE;
    bool    enabled;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));
    otIp6SetSlaacEnabled(mInstance, enabled);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SUPPORTED_RADIO_LINKS>(void)
{
    otError error;

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_RADIO_LINK_IEEE_802_15_4));
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_RADIO_LINK_TREL_UDP6));
#endif

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MULTI_RADIO

otError NcpBase::EncodeNeighborMultiRadioInfo(uint32_t aSpinelRadioLink, const otRadioLinkInfo &aInfo)
{
    otError error;

    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = mEncoder.WriteUintPacked(aSpinelRadioLink));
    SuccessOrExit(error = mEncoder.WriteUint8(aInfo.mPreference));
    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NEIGHBOR_TABLE_MULTI_RADIO_INFO>(void)
{
    otError                  error = OT_ERROR_NONE;
    otNeighborInfoIterator   iter  = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otNeighborInfo           neighInfo;
    otMultiRadioNeighborInfo multiRadioInfo;

    while (otThreadGetNextNeighborInfo(mInstance, &iter, &neighInfo) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(neighInfo.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteUint16(neighInfo.mRloc16));

        if (otMultiRadioGetNeighborInfo(mInstance, &neighInfo.mExtAddress, &multiRadioInfo) == OT_ERROR_NONE)
        {
            if (multiRadioInfo.mSupportsIeee802154)
            {
                SuccessOrExit(error = EncodeNeighborMultiRadioInfo(SPINEL_RADIO_LINK_IEEE_802_15_4,
                                                                   multiRadioInfo.mIeee802154Info));
            }

            if (multiRadioInfo.mSupportsTrelUdp6)
            {
                SuccessOrExit(
                    error = EncodeNeighborMultiRadioInfo(SPINEL_RADIO_LINK_TREL_UDP6, multiRadioInfo.mTrelUdp6Info));
            }
        }

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_MULTI_RADIO

// ----------------------------------------------------------------------------
// SRP Client
// ----------------------------------------------------------------------------

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_START>(void)
{
    otError    error = OT_ERROR_NONE;
    bool       start;
    bool       callbackEnabled;
    otSockAddr serverAddr;

    SuccessOrExit(error = mDecoder.ReadBool(start));

    if (!start)
    {
        otSrpClientStop(mInstance);
        ExitNow();
    }

    SuccessOrExit(error = mDecoder.ReadIp6Address(serverAddr.mAddress));
    SuccessOrExit(error = mDecoder.ReadUint16(serverAddr.mPort));
    SuccessOrExit(error = mDecoder.ReadBool(callbackEnabled));

    SuccessOrExit(error = otSrpClientStart(mInstance, &serverAddr));
    mSrpClientCallbackEnabled = callbackEnabled;

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_LEASE_INTERVAL>(void)
{
    return mEncoder.WriteUint32(otSrpClientGetLeaseInterval(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_LEASE_INTERVAL>(void)
{
    otError  error;
    uint32_t interval;

    SuccessOrExit(error = mDecoder.ReadUint32(interval));
    otSrpClientSetLeaseInterval(mInstance, interval);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_KEY_LEASE_INTERVAL>(void)
{
    return mEncoder.WriteUint32(otSrpClientGetKeyLeaseInterval(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_KEY_LEASE_INTERVAL>(void)
{
    otError  error;
    uint32_t interval;

    SuccessOrExit(error = mDecoder.ReadUint32(interval));
    otSrpClientSetKeyLeaseInterval(mInstance, interval);

exit:
    return error;
}

static spinel_srp_client_item_state_t SrpClientItemStatetoSpinel(otSrpClientItemState aItemState)
{
    spinel_srp_client_item_state_t state = SPINEL_SRP_CLIENT_ITEM_STATE_REMOVED;

    switch (aItemState)
    {
    case OT_SRP_CLIENT_ITEM_STATE_TO_ADD:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_TO_ADD;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_ADDING:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_ADDING;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_TO_REFRESH:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_TO_REFRESH;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_REFRESHING:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_REFRESHING;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_TO_REMOVE:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_TO_REMOVE;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_REMOVING:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_REMOVING;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_REGISTERED:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_REGISTERED;
        break;
    case OT_SRP_CLIENT_ITEM_STATE_REMOVED:
        state = SPINEL_SRP_CLIENT_ITEM_STATE_REMOVED;
        break;
    }

    return state;
}

otError NcpBase::EncodeSrpClientHostInfo(const otSrpClientHostInfo &aHostInfo)
{
    otError error;

    SuccessOrExit(error = mEncoder.WriteUtf8(aHostInfo.mName != nullptr ? aHostInfo.mName : ""));
    SuccessOrExit(error = mEncoder.WriteUint8(SrpClientItemStatetoSpinel(aHostInfo.mState)));

    SuccessOrExit(error = mEncoder.OpenStruct());

    for (uint8_t index = 0; index < aHostInfo.mNumAddresses; index++)
    {
        SuccessOrExit(error = mEncoder.WriteIp6Address(aHostInfo.mAddresses[index]));
    }

    SuccessOrExit(error = mEncoder.CloseStruct());

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_HOST_INFO>(void)
{
    return EncodeSrpClientHostInfo(*otSrpClientGetHostInfo(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_HOST_NAME>(void)
{
    const char *name = otSrpClientGetHostInfo(mInstance)->mName;

    return mEncoder.WriteUtf8(name != nullptr ? name : "");
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_HOST_NAME>(void)
{
    otError     error;
    const char *name;
    uint16_t    size;
    char *      hostNameBuffer;

    SuccessOrExit(error = mDecoder.ReadUtf8(name));

    hostNameBuffer = otSrpClientBuffersGetHostNameString(mInstance, &size);

    VerifyOrExit(StringLength(name, size) < size, error = OT_ERROR_INVALID_ARGS);

    // We first make sure we can set the name, and if so
    // we copy it to the persisted buffer and set
    // the host name again now with the persisted buffer.
    // This ensures that we do not overwrite a previous
    // buffer with a host name that cannot be set.

    SuccessOrExit(error = otSrpClientSetHostName(mInstance, name));

    strcpy(hostNameBuffer, name);
    error = otSrpClientSetHostName(mInstance, hostNameBuffer);
    OT_ASSERT(error == OT_ERROR_NONE);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_HOST_ADDRESSES>(void)
{
    otError                    error    = OT_ERROR_NONE;
    const otSrpClientHostInfo *hostInfo = otSrpClientGetHostInfo(mInstance);

    for (uint8_t index = 0; index < hostInfo->mNumAddresses; index++)
    {
        SuccessOrExit(error = mEncoder.WriteIp6Address(hostInfo->mAddresses[index]));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_HOST_ADDRESSES>(void)
{
    otError       error;
    otIp6Address  addresses[kSrpClientMaxHostAddresses];
    uint8_t       numAddresses = 0;
    otIp6Address *hostAddressArray;
    uint8_t       hostAddressArrayLength;

    hostAddressArray = otSrpClientBuffersGetHostAddressesArray(mInstance, &hostAddressArrayLength);
    OT_ASSERT(hostAddressArrayLength <= kSrpClientMaxHostAddresses);

    while (!mDecoder.IsAllReadInStruct())
    {
        VerifyOrExit(numAddresses < kSrpClientMaxHostAddresses, error = OT_ERROR_NO_BUFS);

        SuccessOrExit(error = mDecoder.ReadIp6Address(addresses[numAddresses]));
        numAddresses++;
    }

    // We first make sure we can set the addresses, and if so we copy
    // the address list into `hostAddressArray` and set it again. This
    // ensures that we do not overwrite a previous list before we know
    // it is safe to set/change the address list.

    SuccessOrExit(error = otSrpClientSetHostAddresses(mInstance, addresses, numAddresses));

    memcpy(hostAddressArray, addresses, sizeof(addresses));

    error = otSrpClientSetHostAddresses(mInstance, hostAddressArray, numAddresses);
    OT_ASSERT(error == OT_ERROR_NONE);

exit:
    return error;
}

otError NcpBase::EncodeSrpClientServices(const otSrpClientService *aServices)
{
    otError error = OT_ERROR_NONE;

    for (; aServices != nullptr; aServices = aServices->mNext)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteUtf8(aServices->mName));
        SuccessOrExit(error = mEncoder.WriteUtf8(aServices->mInstanceName));
        SuccessOrExit(error = mEncoder.WriteUint16(aServices->mPort));
        SuccessOrExit(error = mEncoder.WriteUint16(aServices->mPriority));
        SuccessOrExit(error = mEncoder.WriteUint16(aServices->mWeight));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_SERVICES>(void)
{
    return EncodeSrpClientServices(otSrpClientGetServices(mInstance));
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_SRP_CLIENT_SERVICES>(void)
{
    otError                         error = OT_ERROR_NONE;
    otSrpClientBuffersServiceEntry *entry = nullptr;
    const char *                    serviceName;
    const char *                    instanceName;
    char *                          stringBuffer;
    uint16_t                        size;

    entry = otSrpClientBuffersAllocateService(mInstance);
    VerifyOrExit(entry != nullptr, error = OT_ERROR_NO_BUFS);

    stringBuffer = otSrpClientBuffersGetServiceEntryServiceNameString(entry, &size);
    SuccessOrExit(error = mDecoder.ReadUtf8(serviceName));
    VerifyOrExit(StringLength(serviceName, size) < size, error = OT_ERROR_INVALID_ARGS);
    strcpy(stringBuffer, serviceName);

    stringBuffer = otSrpClientBuffersGetServiceEntryInstanceNameString(entry, &size);
    SuccessOrExit(error = mDecoder.ReadUtf8(instanceName));
    VerifyOrExit(StringLength(instanceName, size) < size, error = OT_ERROR_INVALID_ARGS);
    strcpy(stringBuffer, instanceName);

    SuccessOrExit(error = mDecoder.ReadUint16(entry->mService.mPort));
    SuccessOrExit(error = mDecoder.ReadUint16(entry->mService.mPriority));
    SuccessOrExit(error = mDecoder.ReadUint16(entry->mService.mWeight));

    SuccessOrExit(error = otSrpClientAddService(mInstance, &entry->mService));
    entry = nullptr;

exit:
    if (entry != nullptr)
    {
        otSrpClientBuffersFreeService(mInstance, entry);
    }

    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_SRP_CLIENT_SERVICES>(void)
{
    otError                   error = OT_ERROR_NONE;
    const char *              serviceName;
    const char *              instanceName;
    bool                      toClear = false;
    const otSrpClientService *service;

    SuccessOrExit(error = mDecoder.ReadUtf8(serviceName));
    SuccessOrExit(error = mDecoder.ReadUtf8(instanceName));

    if (!mDecoder.IsAllReadInStruct())
    {
        SuccessOrExit(error = mDecoder.ReadBool(toClear));
    }

    for (service = otSrpClientGetServices(mInstance); service != nullptr; service = service->mNext)
    {
        if ((strcmp(serviceName, service->mName) == 0) || (strcmp(instanceName, service->mInstanceName) == 0))
        {
            break;
        }
    }

    VerifyOrExit(service != nullptr, error = OT_ERROR_NOT_FOUND);

    if (toClear)
    {
        SuccessOrExit(error = otSrpClientClearService(mInstance, const_cast<otSrpClientService *>(service)));
        otSrpClientBuffersFreeService(
            mInstance, reinterpret_cast<otSrpClientBuffersServiceEntry *>(const_cast<otSrpClientService *>(service)));
    }
    else
    {
        error = otSrpClientRemoveService(mInstance, const_cast<otSrpClientService *>(service));
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_REMOVE>(void)
{
    otError error = OT_ERROR_NONE;
    bool    removeKeyLease;
    bool    sendUnregToServer;

    SuccessOrExit(error = mDecoder.ReadBool(removeKeyLease));
    SuccessOrExit(error = mDecoder.ReadBool(sendUnregToServer));

    error = otSrpClientRemoveHostAndServices(mInstance, removeKeyLease, sendUnregToServer);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_HOST_SERVICES_CLEAR>(void)
{
    otSrpClientClearHostAndServices(mInstance);

    return OT_ERROR_NONE;
}

static spinel_srp_client_error_t SrpClientErrorToSpinelError(otError aError)
{
    spinel_srp_client_error_t error = SPINEL_SRP_CLIENT_ERROR_FAILED;

    switch (aError)
    {
    case OT_ERROR_NONE:
        error = SPINEL_SRP_CLIENT_ERROR_NONE;
        break;
    case OT_ERROR_PARSE:
        error = SPINEL_SRP_CLIENT_ERROR_PARSE;
        break;
    case OT_ERROR_NOT_FOUND:
        error = SPINEL_SRP_CLIENT_ERROR_NOT_FOUND;
        break;
    case OT_ERROR_NOT_IMPLEMENTED:
        error = SPINEL_SRP_CLIENT_ERROR_NOT_IMPLEMENTED;
        break;
    case OT_ERROR_SECURITY:
        error = SPINEL_SRP_CLIENT_ERROR_SECURITY;
        break;
    case OT_ERROR_DUPLICATED:
        error = SPINEL_SRP_CLIENT_ERROR_DUPLICATED;
        break;
    case OT_ERROR_RESPONSE_TIMEOUT:
        error = SPINEL_SRP_CLIENT_ERROR_RESPONSE_TIMEOUT;
        break;
    case OT_ERROR_INVALID_ARGS:
        error = SPINEL_SRP_CLIENT_ERROR_INVALID_ARGS;
        break;
    case OT_ERROR_NO_BUFS:
        error = SPINEL_SRP_CLIENT_ERROR_NO_BUFS;
        break;
    case OT_ERROR_FAILED:
    default:
        error = SPINEL_SRP_CLIENT_ERROR_FAILED;
        break;
    }

    return error;
}

void NcpBase::HandleSrpClientCallback(otError                    aError,
                                      const otSrpClientHostInfo *aHostInfo,
                                      const otSrpClientService * aServices,
                                      const otSrpClientService * aRemovedServices,
                                      void *                     aContext)
{
    static_cast<NcpBase *>(aContext)->HandleSrpClientCallback(aError, aHostInfo, aServices, aRemovedServices);
}

void NcpBase::HandleSrpClientCallback(otError                    aError,
                                      const otSrpClientHostInfo *aHostInfo,
                                      const otSrpClientService * aServices,
                                      const otSrpClientService * aRemovedServices)
{
    otError                   error = OT_ERROR_NONE;
    const otSrpClientService *service;
    const otSrpClientService *next;

    VerifyOrExit(mSrpClientCallbackEnabled);

    SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                              SPINEL_PROP_SRP_CLIENT_EVENT));

    SuccessOrExit(error = mEncoder.WriteUint16(SrpClientErrorToSpinelError(aError)));

    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = EncodeSrpClientHostInfo(*aHostInfo));
    SuccessOrExit(error = mEncoder.CloseStruct());

    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = EncodeSrpClientServices(aServices));
    SuccessOrExit(error = mEncoder.CloseStruct());

    SuccessOrExit(error = mEncoder.OpenStruct());
    SuccessOrExit(error = EncodeSrpClientServices(aRemovedServices));
    SuccessOrExit(error = mEncoder.CloseStruct());

    SuccessOrExit(error = mEncoder.EndFrame());

exit:

    if (error != OT_ERROR_NONE)
    {
        // Emit a NONMEM status if we fail to send the event.
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }

    for (service = aRemovedServices; service != nullptr; service = next)
    {
        next = service->mNext;

        otSrpClientBuffersFreeService(
            mInstance, reinterpret_cast<otSrpClientBuffersServiceEntry *>(const_cast<otSrpClientService *>(service)));
    }
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_SRP_CLIENT_SERVICE_KEY_ENABLED>(void)
{
    return mEncoder.WriteBool(otSrpClientIsServiceKeyRecordEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_SRP_CLIENT_SERVICE_KEY_ENABLED>(void)
{
    otError error = OT_ERROR_NONE;
    bool    enabled;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));
    otSrpClientSetServiceKeyRecordEnabled(mInstance, enabled);

exit:
    return error;
}
#endif

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE

#if OPENTHREAD_CONFIG_LEGACY_ENABLE

void NcpBase::RegisterLegacyHandlers(const otNcpLegacyHandlers *aHandlers)
{
    mLegacyHandlers = aHandlers;
    bool isEnabled;

    VerifyOrExit(mLegacyHandlers != nullptr);

    isEnabled = (otThreadGetDeviceRole(mInstance) != OT_DEVICE_ROLE_DISABLED);

    if (isEnabled)
    {
        if (mLegacyHandlers->mStartLegacy)
        {
            mLegacyHandlers->mStartLegacy();
        }
    }
    else
    {
        if (mLegacyHandlers->mStopLegacy)
        {
            mLegacyHandlers->mStopLegacy();
        }
    }

    if (mLegacyHandlers->mSetLegacyUlaPrefix)
    {
        mLegacyHandlers->mSetLegacyUlaPrefix(mLegacyUlaPrefix);
    }

exit:
    return;
}

void NcpBase::HandleDidReceiveNewLegacyUlaPrefix(const uint8_t *aUlaPrefix)
{
    memcpy(mLegacyUlaPrefix, aUlaPrefix, OT_NCP_LEGACY_ULA_PREFIX_LENGTH);
    mChangedPropsSet.AddProperty(SPINEL_PROP_NEST_LEGACY_ULA_PREFIX);
    mUpdateChangedPropsTask.Post();
}

void NcpBase::HandleLegacyNodeDidJoin(const otExtAddress *aExtAddr)
{
    mLegacyNodeDidJoin    = true;
    mLegacyLastJoinedNode = *aExtAddr;
    mChangedPropsSet.AddProperty(SPINEL_PROP_NEST_LEGACY_LAST_NODE_JOINED);
    mUpdateChangedPropsTask.Post();
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NEST_LEGACY_ULA_PREFIX>(void)
{
    return mEncoder.WriteData(mLegacyUlaPrefix, sizeof(mLegacyUlaPrefix));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NEST_LEGACY_ULA_PREFIX>(void)
{
    const uint8_t *ptr = nullptr;
    uint16_t       len;
    otError        error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadData(ptr, len));

    VerifyOrExit(len <= sizeof(mLegacyUlaPrefix), error = OT_ERROR_PARSE);

    memset(mLegacyUlaPrefix, 0, sizeof(mLegacyUlaPrefix));
    memcpy(mLegacyUlaPrefix, ptr, len);

    if ((mLegacyHandlers != nullptr) && (mLegacyHandlers->mSetLegacyUlaPrefix != nullptr))
    {
        mLegacyHandlers->mSetLegacyUlaPrefix(mLegacyUlaPrefix);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NEST_LEGACY_LAST_NODE_JOINED>(void)
{
    if (!mLegacyNodeDidJoin)
    {
        memset(&mLegacyLastJoinedNode, 0, sizeof(mLegacyLastJoinedNode));
    }

    return mEncoder.WriteEui64(mLegacyLastJoinedNode);
}

void NcpBase::StartLegacy(void)
{
    mLegacyNodeDidJoin = false;

    if ((mLegacyHandlers != nullptr) && (mLegacyHandlers->mStartLegacy != nullptr))
    {
        mLegacyHandlers->mStartLegacy();
    }
}

void NcpBase::StopLegacy(void)
{
    mLegacyNodeDidJoin = false;

    if ((mLegacyHandlers != nullptr) && (mLegacyHandlers->mStopLegacy != nullptr))
    {
        mLegacyHandlers->mStopLegacy();
    }
}

#endif // OPENTHREAD_CONFIG_LEGACY_ENABLE

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NETWORK_TIME>(void)
{
    otError             error = OT_ERROR_NONE;
    otNetworkTimeStatus networkTimeStatus;
    uint64_t            time;

    networkTimeStatus = otNetworkTimeGet(mInstance, &time);

    SuccessOrExit(error = mEncoder.WriteUint64(time));
    SuccessOrExit(error = mEncoder.WriteInt8((int8_t)networkTimeStatus));

exit:
    return error;
}

void NcpBase::HandleTimeSyncUpdate(void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleTimeSyncUpdate();
}

void NcpBase::HandleTimeSyncUpdate(void)
{
    mChangedPropsSet.AddProperty(SPINEL_PROP_THREAD_NETWORK_TIME);
    mUpdateChangedPropsTask.Post();
}
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

void NcpBase::HandleActiveScanResult_Jump(otActiveScanResult *aResult, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleActiveScanResult(aResult);
}

// ----------------------------------------------------------------------------
// MARK: Scan Results Glue
// ----------------------------------------------------------------------------

void NcpBase::HandleActiveScanResult(otActiveScanResult *aResult)
{
    otError error = OT_ERROR_NONE;

    if (aResult)
    {
        uint8_t flags = static_cast<uint8_t>(aResult->mVersion << SPINEL_BEACON_THREAD_FLAG_VERSION_SHIFT);

        if (aResult->mIsJoinable)
        {
            flags |= SPINEL_BEACON_THREAD_FLAG_JOINABLE;
        }

        if (aResult->mIsNative)
        {
            flags |= SPINEL_BEACON_THREAD_FLAG_NATIVE;
        }

        SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0,
                                                  SPINEL_CMD_PROP_VALUE_INSERTED, SPINEL_PROP_MAC_SCAN_BEACON));
        SuccessOrExit(error = mEncoder.WriteUint8(aResult->mChannel));
        SuccessOrExit(error = mEncoder.WriteInt8(aResult->mRssi));

        SuccessOrExit(error = mEncoder.OpenStruct()); // "mac-layer data"
        SuccessOrExit(error = mEncoder.WriteEui64(aResult->mExtAddress));
        SuccessOrExit(error = mEncoder.WriteUint16(0xffff)); // short address, not given
        SuccessOrExit(error = mEncoder.WriteUint16(aResult->mPanId));
        SuccessOrExit(error = mEncoder.WriteUint8(aResult->mLqi));
        SuccessOrExit(error = mEncoder.CloseStruct());

        SuccessOrExit(error = mEncoder.OpenStruct());                                 // "net-layer data"
        SuccessOrExit(error = mEncoder.WriteUintPacked(SPINEL_PROTOCOL_TYPE_THREAD)); // type
        SuccessOrExit(error = mEncoder.WriteUint8(flags));
        SuccessOrExit(error = mEncoder.WriteUtf8(aResult->mNetworkName.m8));
        SuccessOrExit(error = mEncoder.WriteDataWithLen(aResult->mExtendedPanId.m8, OT_EXT_PAN_ID_SIZE));
        SuccessOrExit(error = mEncoder.WriteDataWithLen(aResult->mSteeringData.m8, aResult->mSteeringData.mLength));
        SuccessOrExit(error = mEncoder.CloseStruct());

        SuccessOrExit(error = mEncoder.EndFrame());
    }
    else
    {
        // We are finished with the scan, send an unsolicited
        // scan state update.
        mChangedPropsSet.AddProperty(SPINEL_PROP_MAC_SCAN_STATE);
        mUpdateChangedPropsTask.Post();
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        // We ran out of buffer adding a scan result so remember to send
        // an async `LAST_STATUS(NOMEM)` when buffer space becomes
        // available.
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }
}

void NcpBase::HandleEnergyScanResult_Jump(otEnergyScanResult *aResult, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleEnergyScanResult(aResult);
}

void NcpBase::HandleEnergyScanResult(otEnergyScanResult *aResult)
{
    otError error = OT_ERROR_NONE;

    if (aResult)
    {
        SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0,
                                                  SPINEL_CMD_PROP_VALUE_INSERTED, SPINEL_PROP_MAC_ENERGY_SCAN_RESULT));
        SuccessOrExit(error = mEncoder.WriteUint8(aResult->mChannel));
        SuccessOrExit(error = mEncoder.WriteInt8(aResult->mMaxRssi));
        SuccessOrExit(error = mEncoder.EndFrame());
    }
    else
    {
        // We are finished with the scan, send an unsolicited
        // scan state update.
        mChangedPropsSet.AddProperty(SPINEL_PROP_MAC_SCAN_STATE);
        mUpdateChangedPropsTask.Post();
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }
}

#if OPENTHREAD_CONFIG_JOINER_ENABLE
void NcpBase::HandleJoinerCallback_Jump(otError aError, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleJoinerCallback(aError);
}

void NcpBase::HandleJoinerCallback(otError aError)
{
    switch (aError)
    {
    case OT_ERROR_NONE:
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_SUCCESS);
        break;
    case OT_ERROR_SECURITY:
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_SECURITY);
        break;
    case OT_ERROR_NOT_FOUND:
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_NO_PEERS);
        break;
    case OT_ERROR_RESPONSE_TIMEOUT:
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_RSP_TIMEOUT);
        break;
    default:
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_FAILURE);
        break;
    }

    mUpdateChangedPropsTask.Post();
}
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
void NcpBase::HandleLinkMetricsReport_Jump(const otIp6Address *       aSource,
                                           const otLinkMetricsValues *aMetricsValues,
                                           uint8_t                    aStatus,
                                           void *                     aContext)
{
    static_cast<NcpBase *>(aContext)->HandleLinkMetricsReport(aSource, aMetricsValues, aStatus);
}

void NcpBase::HandleLinkMetricsReport(const otIp6Address *       aSource,
                                      const otLinkMetricsValues *aMetricsValues,
                                      uint8_t                    aStatus)
{
    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_THREAD_LINK_METRICS_QUERY_RESULT));

    SuccessOrExit(mEncoder.WriteIp6Address(*aSource));
    SuccessOrExit(mEncoder.WriteUint8(aStatus));
    SuccessOrExit(EncodeLinkMetricsValues(aMetricsValues));

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

void NcpBase::HandleLinkMetricsMgmtResponse_Jump(const otIp6Address *aSource, uint8_t aStatus, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleLinkMetricsMgmtResponse(aSource, aStatus);
}

void NcpBase::HandleLinkMetricsMgmtResponse(const otIp6Address *aSource, uint8_t aStatus)
{
    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_THREAD_LINK_METRICS_MGMT_RESPONSE));

    SuccessOrExit(mEncoder.WriteIp6Address(*aSource));
    SuccessOrExit(mEncoder.WriteUint8(aStatus));

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

void NcpBase::HandleLinkMetricsEnhAckProbingIeReport_Jump(otShortAddress             aShortAddress,
                                                          const otExtAddress *       aExtAddress,
                                                          const otLinkMetricsValues *aMetricsValues,
                                                          void *                     aContext)
{
    static_cast<NcpBase *>(aContext)->HandleLinkMetricsEnhAckProbingIeReport(aShortAddress, aExtAddress,
                                                                             aMetricsValues);
}

void NcpBase::HandleLinkMetricsEnhAckProbingIeReport(otShortAddress             aShortAddress,
                                                     const otExtAddress *       aExtAddress,
                                                     const otLinkMetricsValues *aMetricsValues)
{
    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_THREAD_LINK_METRICS_MGMT_ENH_ACK_IE));

    SuccessOrExit(mEncoder.WriteUint16(aShortAddress));
    SuccessOrExit(mEncoder.WriteEui64(*aExtAddress));
    SuccessOrExit(EncodeLinkMetricsValues(aMetricsValues));

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}
#endif

// ----------------------------------------------------------------------------
// MARK: Outbound Datagram Handling
// ----------------------------------------------------------------------------

void NcpBase::HandleDatagramFromStack(otMessage *aMessage, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleDatagramFromStack(aMessage);
}

void NcpBase::HandleDatagramFromStack(otMessage *aMessage)
{
    VerifyOrExit(aMessage != nullptr);

    // Do not forward frames larger than SPINEL payload size.
    VerifyOrExit(otMessageGetLength(aMessage) <= SPINEL_FRAME_MAX_COMMAND_PAYLOAD_SIZE, otMessageFree(aMessage));

    otMessageQueueEnqueue(&mMessageQueue, aMessage);

    // If there is no queued spinel command response, try to write/send
    // the datagram message immediately. If there is a queued response
    // or if currently out of buffer space, the IPv6 datagram message
    // will be sent from `HandleFrameRemovedFromNcpBuffer()` when buffer
    //  space becomes available and after any pending spinel command
    // response.

    if (IsResponseQueueEmpty())
    {
        IgnoreError(SendQueuedDatagramMessages());
    }

exit:
    return;
}

otError NcpBase::SendDatagramMessage(otMessage *aMessage)
{
    otError           error    = OT_ERROR_NONE;
    uint8_t           header   = SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0;
    bool              isSecure = otMessageIsLinkSecurityEnabled(aMessage);
    spinel_prop_key_t propKey  = isSecure ? SPINEL_PROP_STREAM_NET : SPINEL_PROP_STREAM_NET_INSECURE;

    SuccessOrExit(error = mEncoder.BeginFrame(header, SPINEL_CMD_PROP_VALUE_IS, propKey));
    SuccessOrExit(error = mEncoder.WriteUint16(otMessageGetLength(aMessage)));
    SuccessOrExit(error = mEncoder.WriteMessage(aMessage));

    // Append any metadata (rssi, lqi, channel, etc) here!

    SuccessOrExit(error = mEncoder.EndFrame());

    if (isSecure)
    {
        mOutboundSecureIpFrameCounter++;
    }
    else
    {
        mOutboundInsecureIpFrameCounter++;
    }

exit:
    return error;
}

otError NcpBase::SendQueuedDatagramMessages(void)
{
    otError    error = OT_ERROR_NONE;
    otMessage *message;

    while ((message = otMessageQueueGetHead(&mMessageQueue)) != nullptr)
    {
        // Since an `otMessage` instance can be in one queue at a time,
        // it is first dequeued from `mMessageQueue` before attempting
        // to include it in a spinel frame by calling `SendDatagramMessage()`
        // If forming of the spinel frame fails, the message is enqueued
        // back at the front of `mMessageQueue`.

        otMessageQueueDequeue(&mMessageQueue, message);

        error = SendDatagramMessage(message);

        if (error != OT_ERROR_NONE)
        {
            otMessageQueueEnqueueAtHead(&mMessageQueue, message);
        }

        SuccessOrExit(error);
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_UDP_FORWARD_STREAM>(void)
{
    const uint8_t *     framePtr = nullptr;
    uint16_t            frameLen = 0;
    const otIp6Address *peerAddr;
    uint16_t            peerPort;
    uint16_t            sockPort;
    otMessage *         message;
    otError             error       = OT_ERROR_NONE;
    otMessageSettings   msgSettings = {false, OT_MESSAGE_PRIORITY_NORMAL};

    message = otIp6NewMessage(mInstance, &msgSettings);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    SuccessOrExit(error = mDecoder.ReadDataWithLen(framePtr, frameLen));
    SuccessOrExit(error = mDecoder.ReadUint16(peerPort));
    SuccessOrExit(error = mDecoder.ReadIp6Address(peerAddr));
    SuccessOrExit(error = mDecoder.ReadUint16(sockPort));

    SuccessOrExit(error = otMessageAppend(message, framePtr, static_cast<uint16_t>(frameLen)));

    otUdpForwardReceive(mInstance, message, peerPort, peerAddr, sockPort);

    // `otUdpForwardReceive()` takes ownership of `message` (in both success
    // or failure cases). `message` is set to nullptr so it is not freed at
    // exit.
    message = nullptr;

exit:
    if (message != nullptr)
    {
        otMessageFree(message);
    }

    return error;
}

void NcpBase::HandleUdpForwardStream(otMessage *   aMessage,
                                     uint16_t      aPeerPort,
                                     otIp6Address *aPeerAddr,
                                     uint16_t      aSockPort,
                                     void *        aContext)
{
    static_cast<NcpBase *>(aContext)->HandleUdpForwardStream(aMessage, aPeerPort, *aPeerAddr, aSockPort);
}

void NcpBase::HandleUdpForwardStream(otMessage *aMessage, uint16_t aPeerPort, otIp6Address &aPeerAddr, uint16_t aPort)
{
    uint16_t length = otMessageGetLength(aMessage);
    uint8_t  header = SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0;

    SuccessOrExit(mEncoder.BeginFrame(header, SPINEL_CMD_PROP_VALUE_IS, SPINEL_PROP_THREAD_UDP_FORWARD_STREAM));
    SuccessOrExit(mEncoder.WriteUint16(length));
    SuccessOrExit(mEncoder.WriteMessage(aMessage));

    SuccessOrExit(mEncoder.WriteUint16(aPeerPort));
    SuccessOrExit(mEncoder.WriteIp6Address(aPeerAddr));
    SuccessOrExit(mEncoder.WriteUint16(aPort));
    SuccessOrExit(mEncoder.EndFrame());

    // The `aMessage` is owned by the outbound frame and NCP buffer
    // after frame was finished/ended successfully. It will be freed
    // when the frame is successfully sent and removed.

    aMessage = nullptr;

exit:

    if (aMessage != nullptr)
    {
        otMessageFree(aMessage);
    }
}
#endif // OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE

// ----------------------------------------------------------------------------
// MARK: Pcap frame handling
// ----------------------------------------------------------------------------

void NcpBase::HandlePcapFrame(const otRadioFrame *aFrame, bool aIsTx, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandlePcapFrame(aFrame, aIsTx);
}

void NcpBase::HandlePcapFrame(const otRadioFrame *aFrame, bool aIsTx)
{
    uint16_t flags  = 0;
    uint8_t  header = SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0;

    VerifyOrExit(mPcapEnabled);

    if (aIsTx)
    {
        flags |= SPINEL_MD_FLAG_TX;
    }

    SuccessOrExit(mEncoder.BeginFrame(header, SPINEL_CMD_PROP_VALUE_IS, SPINEL_PROP_STREAM_RAW));
    SuccessOrExit(mEncoder.WriteUint16(aFrame->mLength));

    SuccessOrExit(mEncoder.WriteData(aFrame->mPsdu, aFrame->mLength));

    // Append metadata (rssi, etc)
    SuccessOrExit(mEncoder.WriteInt8(aFrame->mInfo.mRxInfo.mRssi)); // RSSI
    SuccessOrExit(mEncoder.WriteInt8(-128));                        // Noise floor (Currently unused)
    SuccessOrExit(mEncoder.WriteUint16(flags));                     // Flags

    SuccessOrExit(mEncoder.OpenStruct()); // PHY-data
    // Empty for now
    SuccessOrExit(mEncoder.CloseStruct());

    SuccessOrExit(mEncoder.OpenStruct()); // Vendor-data
    // Empty for now
    SuccessOrExit(mEncoder.CloseStruct());

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_PHY_PCAP_ENABLED>(void)
{
    return mEncoder.WriteBool(mPcapEnabled);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_PHY_PCAP_ENABLED>(void)
{
    otError error = OT_ERROR_NONE;
    bool    enabled;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));
    VerifyOrExit(enabled != mPcapEnabled);

    mPcapEnabled = enabled;

    if (mPcapEnabled)
    {
        otLinkSetPcapCallback(mInstance, &NcpBase::HandlePcapFrame, static_cast<void *>(this));
    }
    else
    {
        otLinkSetPcapCallback(mInstance, nullptr, nullptr);
    }

exit:
    return error;
}

// ----------------------------------------------------------------------------
// MARK: Property/Status Changed
// ----------------------------------------------------------------------------

void NcpBase::HandleStateChanged(otChangedFlags aFlags, void *aContext)
{
    NcpBase *ncp = static_cast<NcpBase *>(aContext);

    ncp->mThreadChangedFlags |= aFlags;
    ncp->mUpdateChangedPropsTask.Post();
}

void NcpBase::ProcessThreadChangedFlags(void)
{
    static const struct
    {
        otChangedFlags    mThreadFlag;
        spinel_prop_key_t mPropKey;
    } kFlags[] = {
        {OT_CHANGED_IP6_ADDRESS_ADDED, SPINEL_PROP_IPV6_ADDRESS_TABLE},
        {OT_CHANGED_IP6_ADDRESS_REMOVED, SPINEL_PROP_IPV6_ADDRESS_TABLE},
        {OT_CHANGED_THREAD_ROLE, SPINEL_PROP_NET_ROLE},
        {OT_CHANGED_THREAD_LL_ADDR, SPINEL_PROP_IPV6_LL_ADDR},
        {OT_CHANGED_THREAD_ML_ADDR, SPINEL_PROP_IPV6_ML_ADDR},
        {OT_CHANGED_THREAD_PARTITION_ID, SPINEL_PROP_NET_PARTITION_ID},
        {OT_CHANGED_THREAD_KEY_SEQUENCE_COUNTER, SPINEL_PROP_NET_KEY_SEQUENCE_COUNTER},
        {OT_CHANGED_THREAD_NETDATA, SPINEL_PROP_THREAD_LEADER_NETWORK_DATA},
        {OT_CHANGED_THREAD_CHILD_ADDED, SPINEL_PROP_THREAD_CHILD_TABLE},
        {OT_CHANGED_THREAD_CHILD_REMOVED, SPINEL_PROP_THREAD_CHILD_TABLE},
        {OT_CHANGED_IP6_MULTICAST_SUBSCRIBED, SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE},
        {OT_CHANGED_IP6_MULTICAST_UNSUBSCRIBED, SPINEL_PROP_IPV6_MULTICAST_ADDRESS_TABLE},
        {OT_CHANGED_THREAD_CHANNEL, SPINEL_PROP_PHY_CHAN},
        {OT_CHANGED_THREAD_PANID, SPINEL_PROP_MAC_15_4_PANID},
        {OT_CHANGED_THREAD_NETWORK_NAME, SPINEL_PROP_NET_NETWORK_NAME},
        {OT_CHANGED_THREAD_EXT_PANID, SPINEL_PROP_NET_XPANID},
        {OT_CHANGED_THREAD_RLOC_ADDED, SPINEL_PROP_IPV6_ADDRESS_TABLE},
        {OT_CHANGED_THREAD_RLOC_REMOVED, SPINEL_PROP_IPV6_ADDRESS_TABLE},
        {OT_CHANGED_NETWORK_KEY, SPINEL_PROP_NET_NETWORK_KEY},
        {OT_CHANGED_PSKC, SPINEL_PROP_NET_PSKC},
        {OT_CHANGED_CHANNEL_MANAGER_NEW_CHANNEL, SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL},
        {OT_CHANGED_SUPPORTED_CHANNEL_MASK, SPINEL_PROP_PHY_CHAN_SUPPORTED},
    };

    VerifyOrExit(mThreadChangedFlags != 0);

    // If thread role has changed, check for possible "join" error.

    if ((mThreadChangedFlags & OT_CHANGED_THREAD_ROLE) != 0)
    {
        if (mRequireJoinExistingNetwork)
        {
            switch (otThreadGetDeviceRole(mInstance))
            {
            case OT_DEVICE_ROLE_DETACHED:
            case OT_DEVICE_ROLE_DISABLED:
                break;

            default:
                mRequireJoinExistingNetwork = false;
                mChangedPropsSet.AddProperty(SPINEL_PROP_NET_REQUIRE_JOIN_EXISTING);
                break;
            }

            if ((otThreadGetDeviceRole(mInstance) == OT_DEVICE_ROLE_LEADER) && otThreadIsSingleton(mInstance)
#if OPENTHREAD_CONFIG_LEGACY_ENABLE
                && !mLegacyNodeDidJoin
#endif
            )
            {
                mThreadChangedFlags &= ~static_cast<uint32_t>(OT_CHANGED_THREAD_PARTITION_ID);
                IgnoreError(otThreadSetEnabled(mInstance, false));

                mChangedPropsSet.AddProperty(SPINEL_PROP_NET_STACK_UP);
                mChangedPropsSet.AddLastStatus(SPINEL_STATUS_JOIN_FAILURE);
            }
        }
    }

    // Convert OT_CHANGED flags to corresponding NCP property update.

    for (auto &flag : kFlags)
    {
        uint32_t threadFlag = flag.mThreadFlag;

        if (mThreadChangedFlags & threadFlag)
        {
            spinel_prop_key_t propKey           = flag.mPropKey;
            bool              shouldAddProperty = true;

            // Child table changes are reported using the `HandleChildAdded()` and
            // `HandleChildRemoved()` callbacks emitting spinel `VALUE_INSERTED` and
            // `VALUE_REMOVED` async spinel frames. If the spinel frames could not be
            // added (e.g., out of NCP buffer) from the above callbacks, the flag
            // `mShouldEmitChildTableUpdate` is set to `true` so that the entire
            // child table is emitted as an unsolicited `VALUE_IS` update.

            if (propKey == SPINEL_PROP_THREAD_CHILD_TABLE)
            {
                shouldAddProperty           = mShouldEmitChildTableUpdate;
                mShouldEmitChildTableUpdate = false;
            }

            if (shouldAddProperty)
            {
                mChangedPropsSet.AddProperty(propKey);
            }

            if (threadFlag == OT_CHANGED_THREAD_NETDATA)
            {
                mChangedPropsSet.AddProperty(SPINEL_PROP_THREAD_ON_MESH_NETS);
                mChangedPropsSet.AddProperty(SPINEL_PROP_THREAD_OFF_MESH_ROUTES);
            }

            mThreadChangedFlags &= ~threadFlag;
            VerifyOrExit(mThreadChangedFlags != 0);
        }
    }

    // Clear any remaining ThreadFlag that has no matching
    // NCP property update (e.g., OT_CHANGED_SECURITY_POLICY)

    mThreadChangedFlags = 0;

exit:
    return;
}

} // namespace Ncp
} // namespace ot

// ----------------------------------------------------------------------------
// MARK: Legacy network APIs
// ----------------------------------------------------------------------------

void otNcpRegisterLegacyHandlers(const otNcpLegacyHandlers *aHandlers)
{
#if OPENTHREAD_CONFIG_LEGACY_ENABLE
    ot::Ncp::NcpBase *ncp = ot::Ncp::NcpBase::GetNcpInstance();

    if (ncp != nullptr)
    {
        ncp->RegisterLegacyHandlers(aHandlers);
    }

#else
    OT_UNUSED_VARIABLE(aHandlers);
#endif
}

void otNcpHandleDidReceiveNewLegacyUlaPrefix(const uint8_t *aUlaPrefix)
{
#if OPENTHREAD_CONFIG_LEGACY_ENABLE
    ot::Ncp::NcpBase *ncp = ot::Ncp::NcpBase::GetNcpInstance();

    if (ncp != nullptr)
    {
        ncp->HandleDidReceiveNewLegacyUlaPrefix(aUlaPrefix);
    }

#else
    OT_UNUSED_VARIABLE(aUlaPrefix);
#endif
}

void otNcpHandleLegacyNodeDidJoin(const otExtAddress *aExtAddr)
{
#if OPENTHREAD_CONFIG_LEGACY_ENABLE
    ot::Ncp::NcpBase *ncp = ot::Ncp::NcpBase::GetNcpInstance();

    if (ncp != nullptr)
    {
        ncp->HandleLegacyNodeDidJoin(aExtAddr);
    }

#else
    OT_UNUSED_VARIABLE(aExtAddr);
#endif
}

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD
