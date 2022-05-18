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
 *   This file implements full thread device specified Spinel interface to the OpenThread stack.
 */

#include "ncp_base.hpp"
#include <openthread/config.h>

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#include <openthread/backbone_router_ftd.h>
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
#include <openthread/channel_manager.h>
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#include <openthread/child_supervision.h>
#endif
#include <openthread/dataset.h>
#include <openthread/dataset_ftd.h>
#include <openthread/diag.h>
#include <openthread/icmp6.h>
#include <openthread/ncp.h>
#include <openthread/thread_ftd.h>
#include <openthread/platform/misc.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
#include "meshcop/commissioner.hpp"
#endif

#if OPENTHREAD_FTD
namespace ot {
namespace Ncp {

otError NcpBase::EncodeChildInfo(const otChildInfo &aChildInfo)
{
    otError error = OT_ERROR_NONE;
    uint8_t modeFlags;

    modeFlags =
        LinkFlagsToFlagByte(aChildInfo.mRxOnWhenIdle, aChildInfo.mFullThreadDevice, aChildInfo.mFullNetworkData);

    SuccessOrExit(error = mEncoder.WriteEui64(aChildInfo.mExtAddress));
    SuccessOrExit(error = mEncoder.WriteUint16(aChildInfo.mRloc16));
    SuccessOrExit(error = mEncoder.WriteUint32(aChildInfo.mTimeout));
    SuccessOrExit(error = mEncoder.WriteUint32(aChildInfo.mAge));
    SuccessOrExit(error = mEncoder.WriteUint8(aChildInfo.mNetworkDataVersion));
    SuccessOrExit(error = mEncoder.WriteUint8(aChildInfo.mLinkQualityIn));
    SuccessOrExit(error = mEncoder.WriteInt8(aChildInfo.mAverageRssi));
    SuccessOrExit(error = mEncoder.WriteUint8(modeFlags));
    SuccessOrExit(error = mEncoder.WriteInt8(aChildInfo.mLastRssi));

exit:
    return error;
}

// ----------------------------------------------------------------------------
// MARK: Property/Status Changed
// ----------------------------------------------------------------------------

void NcpBase::HandleParentResponseInfo(otThreadParentResponseInfo *aInfo, void *aContext)
{
    VerifyOrExit(aInfo && aContext);

    static_cast<NcpBase *>(aContext)->HandleParentResponseInfo(*aInfo);

exit:
    return;
}

void NcpBase::HandleParentResponseInfo(const otThreadParentResponseInfo &aInfo)
{
    VerifyOrExit(!mChangedPropsSet.IsPropertyFiltered(SPINEL_PROP_PARENT_RESPONSE_INFO));

    SuccessOrExit(mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_IS,
                                      SPINEL_PROP_PARENT_RESPONSE_INFO));

    SuccessOrExit(mEncoder.WriteEui64(aInfo.mExtAddr));
    SuccessOrExit(mEncoder.WriteUint16(aInfo.mRloc16));
    SuccessOrExit(mEncoder.WriteInt8(aInfo.mRssi));
    SuccessOrExit(mEncoder.WriteInt8(aInfo.mPriority));
    SuccessOrExit(mEncoder.WriteUint8(aInfo.mLinkQuality3));
    SuccessOrExit(mEncoder.WriteUint8(aInfo.mLinkQuality2));
    SuccessOrExit(mEncoder.WriteUint8(aInfo.mLinkQuality1));
    SuccessOrExit(mEncoder.WriteBool(aInfo.mIsAttached));

    SuccessOrExit(mEncoder.EndFrame());

exit:
    return;
}

void NcpBase::HandleNeighborTableChanged(otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntry)
{
    GetNcpInstance()->HandleNeighborTableChanged(aEvent, *aEntry);
}

void NcpBase::HandleNeighborTableChanged(otNeighborTableEvent aEvent, const otNeighborTableEntryInfo &aEntry)
{
    otError           error   = OT_ERROR_NONE;
    unsigned int      command = SPINEL_CMD_PROP_VALUE_REMOVED;
    spinel_prop_key_t property;

    switch (aEvent)
    {
    case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
        command = SPINEL_CMD_PROP_VALUE_INSERTED;

        OT_FALL_THROUGH;

    case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
        property = SPINEL_PROP_THREAD_CHILD_TABLE;
        VerifyOrExit(!aEntry.mInfo.mChild.mIsStateRestoring);
        break;

    case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
        command = SPINEL_CMD_PROP_VALUE_INSERTED;

        OT_FALL_THROUGH;

    case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
        property = SPINEL_PROP_THREAD_NEIGHBOR_TABLE;
        break;

    default:
        ExitNow();
    }

    VerifyOrExit(!mChangedPropsSet.IsPropertyFiltered(property));

    SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, command, property));

    if (property == SPINEL_PROP_THREAD_CHILD_TABLE)
    {
        SuccessOrExit(error = EncodeChildInfo(aEntry.mInfo.mChild));
    }
    else
    {
        SuccessOrExit(error = EncodeNeighborInfo(aEntry.mInfo.mRouter));
    }

    SuccessOrExit(error = mEncoder.EndFrame());

exit:

    // If the frame can not be added (out of NCP buffer space), we remember
    // to send an async `LAST_STATUS(NOMEM)` when buffer space becomes
    // available. Also `mShouldEmitChildTableUpdate` flag is set to `true` so
    // that the entire child table is later emitted as `VALUE_IS` spinel frame
    // update from `ProcessThreadChangedFlags()`.

    if (error != OT_ERROR_NONE)
    {
        if (property == SPINEL_PROP_THREAD_CHILD_TABLE)
        {
            mShouldEmitChildTableUpdate = true;
        }

        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }
}

// ----------------------------------------------------------------------------
// MARK: Individual Property Handlers
// ----------------------------------------------------------------------------

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_LOCAL_LEADER_WEIGHT>(void)
{
    return mEncoder.WriteUint8(otThreadGetLocalLeaderWeight(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_LEADER_WEIGHT>(void)
{
    return mEncoder.WriteUint8(otThreadGetLeaderWeight(mInstance));
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CHILD_TABLE>(void)
{
    otError     error = OT_ERROR_NONE;
    otChildInfo childInfo;
    uint16_t    maxChildren;

    maxChildren = otThreadGetMaxAllowedChildren(mInstance);

    for (uint16_t index = 0; index < maxChildren; index++)
    {
        if ((otThreadGetChildInfoByIndex(mInstance, index, &childInfo) != OT_ERROR_NONE) || childInfo.mIsStateRestoring)
        {
            continue;
        }

        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = EncodeChildInfo(childInfo));
        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ROUTER_TABLE>(void)
{
    otError      error = OT_ERROR_NONE;
    otRouterInfo routerInfo;
    uint8_t      maxRouterId;

    maxRouterId = otThreadGetMaxRouterId(mInstance);

    for (uint8_t routerId = 0; routerId <= maxRouterId; routerId++)
    {
        if ((otThreadGetRouterInfo(mInstance, routerId, &routerInfo) != OT_ERROR_NONE) || !routerInfo.mAllocated)
        {
            continue;
        }

        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(routerInfo.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteUint16(routerInfo.mRloc16));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mRouterId));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mNextHop));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mPathCost));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mLinkQualityIn));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mLinkQualityOut));
        SuccessOrExit(error = mEncoder.WriteUint8(routerInfo.mAge));
        SuccessOrExit(error = mEncoder.WriteBool(routerInfo.mLinkEstablished));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CHILD_TABLE_ADDRESSES>(void)
{
    otError                   error = OT_ERROR_NONE;
    otChildInfo               childInfo;
    uint16_t                  maxChildren;
    otIp6Address              ip6Address;
    otChildIp6AddressIterator iterator = OT_CHILD_IP6_ADDRESS_ITERATOR_INIT;

    maxChildren = otThreadGetMaxAllowedChildren(mInstance);

    for (uint16_t childIndex = 0; childIndex < maxChildren; childIndex++)
    {
        if ((otThreadGetChildInfoByIndex(mInstance, childIndex, &childInfo) != OT_ERROR_NONE) ||
            childInfo.mIsStateRestoring)
        {
            continue;
        }

        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.WriteEui64(childInfo.mExtAddress));
        SuccessOrExit(error = mEncoder.WriteUint16(childInfo.mRloc16));

        iterator = OT_CHILD_IP6_ADDRESS_ITERATOR_INIT;

        while (otThreadGetChildNextIp6Address(mInstance, childIndex, &iterator, &ip6Address) == OT_ERROR_NONE)
        {
            SuccessOrExit(error = mEncoder.WriteIp6Address(ip6Address));
        }

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ROUTER_ROLE_ENABLED>(void)
{
    return mEncoder.WriteBool(otThreadIsRouterEligible(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ROUTER_ROLE_ENABLED>(void)
{
    bool    eligible;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(eligible));

    error = otThreadSetRouterEligible(mInstance, eligible);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MAC_MAX_RETRY_NUMBER_INDIRECT>(void)
{
    return mEncoder.WriteUint8(otLinkGetMaxFrameRetriesIndirect(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MAC_MAX_RETRY_NUMBER_INDIRECT>(void)
{
    uint8_t maxFrameRetriesIndirect;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(maxFrameRetriesIndirect));
    otLinkSetMaxFrameRetriesIndirect(mInstance, maxFrameRetriesIndirect);

exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_DOMAIN_NAME>(void)
{
    return mEncoder.WriteUtf8(otThreadGetDomainName(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_DOMAIN_NAME>(void)
{
    otError     error = OT_ERROR_NONE;
    const char *domainName;

    SuccessOrExit(error = mDecoder.ReadUtf8(domainName));

    error = otThreadSetDomainName(mInstance, domainName);

exit:
    return error;
}
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_DUA_ID>(void)
{
    const otIp6InterfaceIdentifier *iid   = otThreadGetFixedDuaInterfaceIdentifier(mInstance);
    otError                         error = OT_ERROR_NONE;

    if (iid == nullptr)
    {
        // send empty response
    }
    else
    {
        for (size_t i = 0; i < sizeof(otIp6InterfaceIdentifier); i++)
        {
            SuccessOrExit(error = mEncoder.WriteUint8(iid->mFields.m8[i]));
        }
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_DUA_ID>(void)
{
    otError error = OT_ERROR_NONE;

    if (mDecoder.GetRemainingLength() == 0)
    {
        SuccessOrExit(error = otThreadSetFixedDuaInterfaceIdentifier(mInstance, nullptr));
    }
    else
    {
        otIp6InterfaceIdentifier iid;

        for (size_t i = 0; i < sizeof(otIp6InterfaceIdentifier); i++)
        {
            SuccessOrExit(error = mDecoder.ReadUint8(iid.mFields.m8[i]));
        }

        SuccessOrExit(error = otThreadSetFixedDuaInterfaceIdentifier(mInstance, &iid));
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_DUA_ENABLE

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_STATE>(void)
{
    uint8_t state = SPINEL_THREAD_BACKBONE_ROUTER_STATE_DISABLED;

    switch (otBackboneRouterGetState(mInstance))
    {
    case OT_BACKBONE_ROUTER_STATE_DISABLED:
        state = SPINEL_THREAD_BACKBONE_ROUTER_STATE_DISABLED;
        break;

    case OT_BACKBONE_ROUTER_STATE_SECONDARY:
        state = SPINEL_THREAD_BACKBONE_ROUTER_STATE_SECONDARY;
        break;

    case OT_BACKBONE_ROUTER_STATE_PRIMARY:
        state = SPINEL_THREAD_BACKBONE_ROUTER_STATE_PRIMARY;
        break;
    }

    return mEncoder.WriteUint8(state);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_STATE>(void)
{
    uint8_t state;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(state));

    if (state)
    {
        otBackboneRouterSetEnabled(mInstance, true);
    }
    else
    {
        otBackboneRouterSetEnabled(mInstance, false);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_CONFIG>(void)
{
    otError                error = OT_ERROR_NONE;
    otBackboneRouterConfig bbrConfig;

    otBackboneRouterGetConfig(mInstance, &bbrConfig);

    SuccessOrExit(error = mEncoder.WriteUint16(bbrConfig.mReregistrationDelay));
    SuccessOrExit(error = mEncoder.WriteUint32(bbrConfig.mMlrTimeout));
    SuccessOrExit(error = mEncoder.WriteUint8(bbrConfig.mSequenceNumber));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_CONFIG>(void)
{
    otError                error = OT_ERROR_NONE;
    otBackboneRouterConfig bbrConfig;

    SuccessOrExit(error = mDecoder.ReadUint16(bbrConfig.mReregistrationDelay));
    SuccessOrExit(error = mDecoder.ReadUint32(bbrConfig.mMlrTimeout));
    SuccessOrExit(error = mDecoder.ReadUint8(bbrConfig.mSequenceNumber));

    SuccessOrExit(error = otBackboneRouterSetConfig(mInstance, &bbrConfig));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTER>(void)
{
    return otBackboneRouterRegister(mInstance);
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTRATION_JITTER>(void)
{
    uint8_t jitter = otBackboneRouterGetRegistrationJitter(mInstance);

    return mEncoder.WriteUint8(jitter);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_BACKBONE_ROUTER_LOCAL_REGISTRATION_JITTER>(void)
{
    otError error = OT_ERROR_NONE;
    uint8_t jitter;

    SuccessOrExit(error = mDecoder.ReadUint8(jitter));

    otBackboneRouterSetRegistrationJitter(mInstance, jitter);

exit:
    return error;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_NET_PSKC>(void)
{
    Pskc pskc;

    otThreadGetPskc(mInstance, &pskc);

    return mEncoder.WriteData(pskc.m8, sizeof(spinel_net_pskc_t));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_PSKC>(void)
{
    const uint8_t *ptr = nullptr;
    uint16_t       len;
    otError        error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadData(ptr, len));

    VerifyOrExit(len == sizeof(spinel_net_pskc_t), error = OT_ERROR_PARSE);

    error = otThreadSetPskc(mInstance, reinterpret_cast<const otPskc *>(ptr));

exit:
    return error;
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_NET_PARTITION_ID>(void)
{
    uint32_t partitionId = 0;
    otError  error       = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(partitionId));

    otThreadSetPreferredLeaderPartitionId(mInstance, partitionId);

exit:
    return error;
}
#endif

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CHILD_COUNT_MAX>(void)
{
    return mEncoder.WriteUint8(static_cast<uint8_t>(otThreadGetMaxAllowedChildren(mInstance)));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CHILD_COUNT_MAX>(void)
{
    uint8_t maxChildren = 0;
    otError error       = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(maxChildren));

    error = otThreadSetMaxAllowedChildren(mInstance, maxChildren);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ROUTER_UPGRADE_THRESHOLD>(void)
{
    return mEncoder.WriteUint8(otThreadGetRouterUpgradeThreshold(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ROUTER_UPGRADE_THRESHOLD>(void)
{
    uint8_t threshold = 0;
    otError error     = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(threshold));

    otThreadSetRouterUpgradeThreshold(mInstance, threshold);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ROUTER_DOWNGRADE_THRESHOLD>(void)
{
    return mEncoder.WriteUint8(otThreadGetRouterDowngradeThreshold(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ROUTER_DOWNGRADE_THRESHOLD>(void)
{
    uint8_t threshold = 0;
    otError error     = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(threshold));

    otThreadSetRouterDowngradeThreshold(mInstance, threshold);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ROUTER_SELECTION_JITTER>(void)
{
    return mEncoder.WriteUint8(otThreadGetRouterSelectionJitter(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_ROUTER_SELECTION_JITTER>(void)
{
    uint8_t jitter = 0;
    otError error  = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(jitter));

    otThreadSetRouterSelectionJitter(mInstance, jitter);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_CONTEXT_REUSE_DELAY>(void)
{
    return mEncoder.WriteUint32(otThreadGetContextIdReuseDelay(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_CONTEXT_REUSE_DELAY>(void)
{
    uint32_t delay = 0;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(delay));

    otThreadSetContextIdReuseDelay(mInstance, delay);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NETWORK_ID_TIMEOUT>(void)
{
    return mEncoder.WriteUint8(otThreadGetNetworkIdTimeout(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_NETWORK_ID_TIMEOUT>(void)
{
    uint8_t timeout = 0;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(timeout));

    otThreadSetNetworkIdTimeout(mInstance, timeout);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_NEW_DATASET>(void)
{
    otError              error;
    otOperationalDataset dataset;

    error = otDatasetCreateNewNetwork(mInstance, &dataset);

    if (error == OT_ERROR_NONE)
    {
        error = EncodeOperationalDataset(dataset);
    }
    else
    {
        error = mEncoder.OverwriteWithLastStatusError(ThreadErrorToSpinelStatus(error));
    }

    return error;
}

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_COMMISSIONER_STATE>(void)
{
    uint8_t state = SPINEL_MESHCOP_COMMISSIONER_STATE_DISABLED;

    switch (otCommissionerGetState(mInstance))
    {
    case OT_COMMISSIONER_STATE_DISABLED:
        state = SPINEL_MESHCOP_COMMISSIONER_STATE_DISABLED;
        break;

    case OT_COMMISSIONER_STATE_PETITION:
        state = SPINEL_MESHCOP_COMMISSIONER_STATE_PETITION;
        break;

    case OT_COMMISSIONER_STATE_ACTIVE:
        state = SPINEL_MESHCOP_COMMISSIONER_STATE_ACTIVE;
        break;
    }

    return mEncoder.WriteUint8(state);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_STATE>(void)
{
    uint8_t state;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(state));

    switch (state)
    {
    case SPINEL_MESHCOP_COMMISSIONER_STATE_DISABLED:
        error = otCommissionerStop(mInstance);
        break;

    case SPINEL_MESHCOP_COMMISSIONER_STATE_ACTIVE:
        error = otCommissionerStart(mInstance, nullptr, nullptr, nullptr);
        break;

    default:
        error = OT_ERROR_INVALID_ARGS;
        break;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS>(void)
{
    otError      error = OT_ERROR_NONE;
    uint16_t     iter  = 0;
    otJoinerInfo joinerInfo;

    while (otCommissionerGetNextJoinerInfo(mInstance, &iter, &joinerInfo) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = mEncoder.OpenStruct());

        SuccessOrExit(error = mEncoder.OpenStruct()); // Joiner Id (any, EUI64 or a Joiner Discerner) struct

        switch (joinerInfo.mType)
        {
        case OT_JOINER_INFO_TYPE_ANY:
            break;

        case OT_JOINER_INFO_TYPE_EUI64:
            SuccessOrExit(error = mEncoder.WriteEui64(joinerInfo.mSharedId.mEui64));
            break;

        case OT_JOINER_INFO_TYPE_DISCERNER:
            SuccessOrExit(error = mEncoder.WriteUint8(joinerInfo.mSharedId.mDiscerner.mLength));
            SuccessOrExit(error = mEncoder.WriteUint64(joinerInfo.mSharedId.mDiscerner.mValue));
            break;
        }

        SuccessOrExit(error = mEncoder.CloseStruct());

        SuccessOrExit(error = mEncoder.WriteUint32(joinerInfo.mExpirationTime));
        SuccessOrExit(error = mEncoder.WriteUtf8(joinerInfo.mPskd.m8));

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS>(void)
{
    otError             error = OT_ERROR_NONE;
    otJoinerDiscerner   discerner;
    bool                withDiscerner = false;
    const otExtAddress *eui64;
    uint32_t            timeout;
    const char *        psk;

    SuccessOrExit(error = mDecoder.OpenStruct());

    switch (mDecoder.GetRemainingLengthInStruct())
    {
    case 0:
        // Empty struct indicates any joiner
        eui64 = nullptr;
        break;

    case sizeof(spinel_eui64_t):
        SuccessOrExit(error = mDecoder.ReadEui64(eui64));
        break;

    default:
        SuccessOrExit(error = mDecoder.ReadUint8(discerner.mLength));
        SuccessOrExit(error = mDecoder.ReadUint64(discerner.mValue));
        withDiscerner = true;
        break;
    }

    SuccessOrExit(error = mDecoder.CloseStruct());

    SuccessOrExit(error = mDecoder.ReadUint32(timeout));
    SuccessOrExit(error = mDecoder.ReadUtf8(psk));

    if (withDiscerner)
    {
        error = otCommissionerAddJoinerWithDiscerner(mInstance, &discerner, psk, timeout);
    }
    else
    {
        error = otCommissionerAddJoiner(mInstance, eui64, psk, timeout);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS>(void)
{
    otError             error = OT_ERROR_NONE;
    otJoinerDiscerner   discerner;
    bool                withDiscerner = false;
    const otExtAddress *eui64;

    SuccessOrExit(error = mDecoder.OpenStruct());

    switch (mDecoder.GetRemainingLengthInStruct())
    {
    case 0:
        // Empty struct indicates any joiner
        eui64 = nullptr;
        break;

    case sizeof(spinel_eui64_t):
        SuccessOrExit(error = mDecoder.ReadEui64(eui64));
        break;

    default:
        SuccessOrExit(error = mDecoder.ReadUint8(discerner.mLength));
        SuccessOrExit(error = mDecoder.ReadUint64(discerner.mValue));
        withDiscerner = true;
        break;
    }

    SuccessOrExit(error = mDecoder.CloseStruct());

    if (withDiscerner)
    {
        error = otCommissionerRemoveJoinerWithDiscerner(mInstance, &discerner);
    }
    else
    {
        error = otCommissionerRemoveJoiner(mInstance, eui64);
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_COMMISSIONER_PROVISIONING_URL>(void)
{
    return mEncoder.WriteUtf8(otCommissionerGetProvisioningUrl(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_PROVISIONING_URL>(void)
{
    otError     error = OT_ERROR_NONE;
    const char *url;

    SuccessOrExit(error = mDecoder.ReadUtf8(url));

    error = otCommissionerSetProvisioningUrl(mInstance, url);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_MESHCOP_COMMISSIONER_SESSION_ID>(void)
{
    return mEncoder.WriteUint16(otCommissionerGetSessionId(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_ANNOUNCE_BEGIN>(void)
{
    otError             error = OT_ERROR_NONE;
    uint32_t            channelMask;
    uint8_t             count;
    uint16_t            period;
    const otIp6Address *address;

    SuccessOrExit(error = mDecoder.ReadUint32(channelMask));
    SuccessOrExit(error = mDecoder.ReadUint8(count));
    SuccessOrExit(error = mDecoder.ReadUint16(period));
    SuccessOrExit(error = mDecoder.ReadIp6Address(address));

    error = otCommissionerAnnounceBegin(mInstance, channelMask, count, period, address);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN>(void)
{
    otError             error = OT_ERROR_NONE;
    uint32_t            channelMask;
    uint8_t             count;
    uint16_t            period;
    uint16_t            scanDuration;
    const otIp6Address *address;

    SuccessOrExit(error = mDecoder.ReadUint32(channelMask));
    SuccessOrExit(error = mDecoder.ReadUint8(count));
    SuccessOrExit(error = mDecoder.ReadUint16(period));
    SuccessOrExit(error = mDecoder.ReadUint16(scanDuration));
    SuccessOrExit(error = mDecoder.ReadIp6Address(address));

    error = otCommissionerEnergyScan(mInstance, channelMask, count, period, scanDuration, address,
                                     &NcpBase::HandleCommissionerEnergyReport_Jump, this);

exit:
    return error;
}

void NcpBase::HandleCommissionerEnergyReport_Jump(uint32_t       aChannelMask,
                                                  const uint8_t *aEnergyData,
                                                  uint8_t        aLength,
                                                  void *         aContext)
{
    static_cast<NcpBase *>(aContext)->HandleCommissionerEnergyReport(aChannelMask, aEnergyData, aLength);
}

void NcpBase::HandleCommissionerEnergyReport(uint32_t aChannelMask, const uint8_t *aEnergyData, uint8_t aLength)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_INSERTED,
                                              SPINEL_PROP_MESHCOP_COMMISSIONER_ENERGY_SCAN_RESULT));
    SuccessOrExit(error = mEncoder.WriteUint32(aChannelMask));
    SuccessOrExit(error = mEncoder.WriteDataWithLen(aEnergyData, aLength));
    SuccessOrExit(error = mEncoder.EndFrame());

exit:

    if (error != OT_ERROR_NONE)
    {
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_QUERY>(void)
{
    otError             error = OT_ERROR_NONE;
    uint16_t            panId;
    uint32_t            channelMask;
    const otIp6Address *address;

    SuccessOrExit(error = mDecoder.ReadUint16(panId));
    SuccessOrExit(error = mDecoder.ReadUint32(channelMask));
    SuccessOrExit(error = mDecoder.ReadIp6Address(address));

    error = otCommissionerPanIdQuery(mInstance, panId, channelMask, address,
                                     &NcpBase::HandleCommissionerPanIdConflict_Jump, this);

exit:
    return error;
}

void NcpBase::HandleCommissionerPanIdConflict_Jump(uint16_t aPanId, uint32_t aChannelMask, void *aContext)
{
    static_cast<NcpBase *>(aContext)->HandleCommissionerPanIdConflict(aPanId, aChannelMask);
}

void NcpBase::HandleCommissionerPanIdConflict(uint16_t aPanId, uint32_t aChannelMask)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mEncoder.BeginFrame(SPINEL_HEADER_FLAG | SPINEL_HEADER_IID_0, SPINEL_CMD_PROP_VALUE_INSERTED,
                                              SPINEL_PROP_MESHCOP_COMMISSIONER_PAN_ID_CONFLICT_RESULT));

    SuccessOrExit(error = mEncoder.WriteUint16(aPanId));
    SuccessOrExit(error = mEncoder.WriteUint32(aChannelMask));
    SuccessOrExit(error = mEncoder.EndFrame());

exit:

    if (error != OT_ERROR_NONE)
    {
        mChangedPropsSet.AddLastStatus(SPINEL_STATUS_NOMEM);
        mUpdateChangedPropsTask.Post();
    }
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_GET>(void)
{
    otError        error = OT_ERROR_NONE;
    const uint8_t *tlvs;
    uint16_t       length;

    SuccessOrExit(error = mDecoder.ReadDataWithLen(tlvs, length));
    VerifyOrExit(length <= 255, error = OT_ERROR_INVALID_ARGS);

    error = otCommissionerSendMgmtGet(mInstance, tlvs, static_cast<uint8_t>(length));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_MESHCOP_COMMISSIONER_MGMT_SET>(void)
{
    otError                error = OT_ERROR_NONE;
    const uint8_t *        tlvs;
    uint16_t               length;
    otCommissioningDataset dataset;

    SuccessOrExit(error = mDecoder.ReadDataWithLen(tlvs, length));
    VerifyOrExit(length <= 255, error = OT_ERROR_INVALID_ARGS);

    memset(&dataset, 0, sizeof(otCommissioningDataset));
    error = otCommissionerSendMgmtSet(mInstance, &dataset, tlvs, static_cast<uint8_t>(length));

exit:
    return error;
}

otError NcpBase::HandlePropertySet_SPINEL_PROP_MESHCOP_COMMISSIONER_GENERATE_PSKC(uint8_t aHeader)
{
    otError        error = OT_ERROR_NONE;
    const char *   passPhrase;
    const char *   networkName;
    const uint8_t *extPanIdData;
    uint16_t       length;
    otPskc         pskc;

    SuccessOrExit(error = mDecoder.ReadUtf8(passPhrase));
    SuccessOrExit(error = mDecoder.ReadUtf8(networkName));
    SuccessOrExit(error = mDecoder.ReadDataWithLen(extPanIdData, length));
    VerifyOrExit(length == sizeof(spinel_net_xpanid_t), error = OT_ERROR_PARSE);

    SuccessOrExit(error = otDatasetGeneratePskc(passPhrase, reinterpret_cast<const otNetworkName *>(networkName),
                                                reinterpret_cast<const otExtendedPanId *>(extPanIdData), &pskc));

    SuccessOrExit(
        error = mEncoder.BeginFrame(aHeader, SPINEL_CMD_PROP_VALUE_IS, SPINEL_PROP_MESHCOP_COMMISSIONER_GENERATE_PSKC));
    SuccessOrExit(error = mEncoder.WriteData(pskc.m8, sizeof(pskc)));
    SuccessOrExit(error = mEncoder.EndFrame());

exit:
    return error;
}

// SPINEL_PROP_THREAD_COMMISSIONER_ENABLED is replaced by SPINEL_PROP_MESHCOP_COMMISSIONER_STATE. Please use the new
// property. The old property/implementation remains for backward compatibility.

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_COMMISSIONER_ENABLED>(void)
{
    return mEncoder.WriteBool(otCommissionerGetState(mInstance) == OT_COMMISSIONER_STATE_ACTIVE);
}

otError NcpBase::HandlePropertySet_SPINEL_PROP_THREAD_COMMISSIONER_ENABLED(uint8_t aHeader)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));

    if (!enabled)
    {
        error = otCommissionerStop(mInstance);
    }
    else
    {
        error = otCommissionerStart(mInstance, nullptr, nullptr, nullptr);
    }

exit:
    return PrepareLastStatusResponse(aHeader, ThreadErrorToSpinelStatus(error));
}

// SPINEL_PROP_THREAD_JOINERS is replaced by SPINEL_PROP_MESHCOP_COMMISSIONER_JOINERS. Please us the new property.
// The old property/implementation remains for backward compatibility.

template <> otError NcpBase::HandlePropertyInsert<SPINEL_PROP_THREAD_JOINERS>(void)
{
    otError             error         = OT_ERROR_NONE;
    const otExtAddress *eui64         = nullptr;
    const char *        pskd          = nullptr;
    uint32_t            joinerTimeout = 0;

    SuccessOrExit(error = mDecoder.ReadUtf8(pskd));
    SuccessOrExit(error = mDecoder.ReadUint32(joinerTimeout));

    if (mDecoder.ReadEui64(eui64) != OT_ERROR_NONE)
    {
        eui64 = nullptr;
    }

    error = otCommissionerAddJoiner(mInstance, eui64, pskd, joinerTimeout);

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_LOCAL_LEADER_WEIGHT>(void)
{
    uint8_t weight;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(weight));

    otThreadSetLocalLeaderWeight(mInstance, weight);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_STEERING_DATA>(void)
{
    return mEncoder.WriteEui64(mSteeringDataAddress);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_STEERING_DATA>(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadEui64(mSteeringDataAddress));

    otThreadSetSteeringData(mInstance, &mSteeringDataAddress);

exit:
    return error;
}
#endif // #if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_PREFERRED_ROUTER_ID>(void)
{
    return mEncoder.WriteUint8(mPreferredRouteId);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_THREAD_PREFERRED_ROUTER_ID>(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(mPreferredRouteId));

    SuccessOrExit(error = otThreadSetPreferredRouterId(mInstance, mPreferredRouteId));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyRemove<SPINEL_PROP_THREAD_ACTIVE_ROUTER_IDS>(void)
{
    otError error = OT_ERROR_NONE;
    uint8_t routerId;

    SuccessOrExit(error = mDecoder.ReadUint8(routerId));

    error = otThreadReleaseRouterId(mInstance, routerId);

    // `INVALID_STATE` is returned when router ID was not allocated (i.e. not in the list)
    // in such a case, the "remove" operation can be considered successful.

    if (error == OT_ERROR_INVALID_STATE)
    {
        error = OT_ERROR_NONE;
    }

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_THREAD_ADDRESS_CACHE_TABLE>(void)
{
    otError              error = OT_ERROR_NONE;
    otCacheEntryIterator iterator;
    otCacheEntryInfo     entry;

    memset(&iterator, 0, sizeof(iterator));

    for (uint8_t index = 0;; index++)
    {
        SuccessOrExit(otThreadGetNextCacheEntry(mInstance, &entry, &iterator));

        SuccessOrExit(error = mEncoder.OpenStruct());
        SuccessOrExit(error = mEncoder.WriteIp6Address(entry.mTarget));
        SuccessOrExit(error = mEncoder.WriteUint16(entry.mRloc16));
        SuccessOrExit(error = mEncoder.WriteUint8(index));

        switch (entry.mState)
        {
        case OT_CACHE_ENTRY_STATE_CACHED:
            SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_ADDRESS_CACHE_ENTRY_STATE_CACHED));
            break;
        case OT_CACHE_ENTRY_STATE_SNOOPED:
            SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_ADDRESS_CACHE_ENTRY_STATE_SNOOPED));
            break;
        case OT_CACHE_ENTRY_STATE_QUERY:
            SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_ADDRESS_CACHE_ENTRY_STATE_QUERY));
            break;
        case OT_CACHE_ENTRY_STATE_RETRY_QUERY:
            SuccessOrExit(error = mEncoder.WriteUint8(SPINEL_ADDRESS_CACHE_ENTRY_STATE_RETRY_QUERY));
            break;
        }

        SuccessOrExit(error = mEncoder.OpenStruct());

        if (entry.mState == OT_CACHE_ENTRY_STATE_CACHED)
        {
            SuccessOrExit(error = mEncoder.WriteBool(entry.mValidLastTrans));
            SuccessOrExit(error = mEncoder.WriteUint32(entry.mLastTransTime));
            SuccessOrExit(error = mEncoder.WriteIp6Address(entry.mMeshLocalEid));
        }

        SuccessOrExit(error = mEncoder.CloseStruct());

        SuccessOrExit(error = mEncoder.OpenStruct());

        if (entry.mState != OT_CACHE_ENTRY_STATE_CACHED)
        {
            SuccessOrExit(error = mEncoder.WriteBool(entry.mCanEvict));
            SuccessOrExit(error = mEncoder.WriteUint16(entry.mTimeout));
            SuccessOrExit(error = mEncoder.WriteUint16(entry.mRetryDelay));
        }

        SuccessOrExit(error = mEncoder.CloseStruct());

        SuccessOrExit(error = mEncoder.CloseStruct());
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHILD_SUPERVISION_INTERVAL>(void)
{
    return mEncoder.WriteUint16(otChildSupervisionGetInterval(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHILD_SUPERVISION_INTERVAL>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t interval;

    SuccessOrExit(error = mDecoder.ReadUint16(interval));
    otChildSupervisionSetInterval(mInstance, interval);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL>(void)
{
    return mEncoder.WriteUint8(otChannelManagerGetRequestedChannel(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_NEW_CHANNEL>(void)
{
    uint8_t channel;
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint8(channel));

    otChannelManagerRequestChannelChange(mInstance, channel);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_DELAY>(void)
{
    return mEncoder.WriteUint16(otChannelManagerGetDelay(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_DELAY>(void)
{
    uint16_t delay;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint16(delay));

    error = otChannelManagerSetDelay(mInstance, delay);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_SUPPORTED_CHANNELS>(void)
{
    return EncodeChannelMask(otChannelManagerGetSupportedChannels(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_SUPPORTED_CHANNELS>(void)
{
    uint32_t channelMask = 0;
    otError  error       = OT_ERROR_NONE;

    SuccessOrExit(error = DecodeChannelMask(channelMask));
    otChannelManagerSetSupportedChannels(mInstance, channelMask);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_FAVORED_CHANNELS>(void)
{
    return EncodeChannelMask(otChannelManagerGetFavoredChannels(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_FAVORED_CHANNELS>(void)
{
    uint32_t channelMask = 0;
    otError  error       = OT_ERROR_NONE;

    SuccessOrExit(error = DecodeChannelMask(channelMask));
    otChannelManagerSetFavoredChannels(mInstance, channelMask);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_CHANNEL_SELECT>(void)
{
    return mEncoder.WriteBool(false);
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_CHANNEL_SELECT>(void)
{
    bool    skipQualityCheck = false;
    otError error            = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(skipQualityCheck));
    error = otChannelManagerRequestChannelSelect(mInstance, skipQualityCheck);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_ENABLED>(void)
{
    return mEncoder.WriteBool(otChannelManagerGetAutoChannelSelectionEnabled(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_ENABLED>(void)
{
    bool    enabled = false;
    otError error   = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadBool(enabled));
    otChannelManagerSetAutoChannelSelectionEnabled(mInstance, enabled);

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL>(void)
{
    return mEncoder.WriteUint32(otChannelManagerGetAutoChannelSelectionInterval(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_CHANNEL_MANAGER_AUTO_SELECT_INTERVAL>(void)
{
    uint32_t interval;
    otError  error = OT_ERROR_NONE;

    SuccessOrExit(error = mDecoder.ReadUint32(interval));
    error = otChannelManagerSetAutoChannelSelectionInterval(mInstance, interval);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_TIME_SYNC_PERIOD>(void)
{
    return mEncoder.WriteUint16(otNetworkTimeGetSyncPeriod(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_TIME_SYNC_PERIOD>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t timeSyncPeriod;

    SuccessOrExit(error = mDecoder.ReadUint16(timeSyncPeriod));

    SuccessOrExit(error = otNetworkTimeSetSyncPeriod(mInstance, timeSyncPeriod));

exit:
    return error;
}

template <> otError NcpBase::HandlePropertyGet<SPINEL_PROP_TIME_SYNC_XTAL_THRESHOLD>(void)
{
    return mEncoder.WriteUint16(otNetworkTimeGetXtalThreshold(mInstance));
}

template <> otError NcpBase::HandlePropertySet<SPINEL_PROP_TIME_SYNC_XTAL_THRESHOLD>(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t xtalThreshold;

    SuccessOrExit(error = mDecoder.ReadUint16(xtalThreshold));

    SuccessOrExit(error = otNetworkTimeSetXtalThreshold(mInstance, xtalThreshold));

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

} // namespace Ncp
} // namespace ot

#endif // OPENTHREAD_FTD
