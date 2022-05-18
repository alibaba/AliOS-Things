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
 *   This file includes definitions for MLE functionality required by the Thread Child, Router, and Leader roles.
 */

#ifndef MLE_HPP_
#define MLE_HPP_

#include "openthread-core-config.h"

#include "common/encoding.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"
#include "mac/mac.hpp"
#include "meshcop/joiner_router.hpp"
#include "meshcop/meshcop.hpp"
#include "net/udp6.hpp"
#include "thread/link_metrics.hpp"
#include "thread/link_metrics_tlvs.hpp"
#include "thread/mle_tlvs.hpp"
#include "thread/mle_types.hpp"
#include "thread/neighbor_table.hpp"
#include "thread/topology.hpp"

namespace ot {

/**
 * @addtogroup core-mle MLE
 *
 * @brief
 *   This module includes definitions for the MLE protocol.
 *
 * @{
 *
 * @defgroup core-mle-core Core
 * @defgroup core-mle-router Router
 * @defgroup core-mle-tlvs TLVs
 *
 * @}
 */

/**
 * @namespace ot::Mle
 *
 * @brief
 *   This namespace includes definitions for the MLE protocol.
 */

namespace Mle {

/**
 * @addtogroup core-mle-core
 *
 * @brief
 *   This module includes definitions for MLE functionality required by the Thread Child, Router, and Leader roles.
 *
 * @{
 *
 */

/**
 * This class implements MLE functionality required by the Thread EndDevices, Router, and Leader roles.
 *
 */
class Mle : public InstanceLocator, private NonCopyable
{
    friend class DiscoverScanner;
    friend class ot::Notifier;
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    friend class ot::LinkMetrics::LinkMetrics;
#endif

public:
    /**
     * This constructor initializes the MLE object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Mle(Instance &aInstance);

    /**
     * This method enables MLE.
     *
     * @retval kErrorNone     Successfully enabled MLE.
     * @retval kErrorAlready  MLE was already enabled.
     *
     */
    Error Enable(void);

    /**
     * This method disables MLE.
     *
     * @retval kErrorNone     Successfully disabled MLE.
     *
     */
    Error Disable(void);

    /**
     * This method starts the MLE protocol operation.
     *
     * @retval kErrorNone           Successfully started the protocol operation.
     * @retval kErrorInvalidState   IPv6 interface is down or device is in raw-link mode.
     *
     */
    Error Start(void) { return Start(kNormalAttach); }

    /**
     * This method stops the MLE protocol operation.
     *
     */
    void Stop(void) { Stop(kUpdateNetworkDatasets); }

    /**
     * This method restores network information from non-volatile memory.
     *
     * @retval kErrorNone      Successfully restore the network information.
     * @retval kErrorNotFound  There is no valid network information stored in non-volatile memory.
     *
     */
    Error Restore(void);

    /**
     * This method stores network information into non-volatile memory.
     *
     * @retval kErrorNone      Successfully store the network information.
     * @retval kErrorNoBufs    Could not store the network information due to insufficient memory space.
     *
     */
    Error Store(void);

    /**
     * This method generates an MLE Announce message.
     *
     * @param[in]  aChannel        The channel to use when transmitting.
     *
     */
    void SendAnnounce(uint8_t aChannel) { SendAnnounce(aChannel, kNormalAnnounce); }

    /**
     * This method causes the Thread interface to detach from the Thread network.
     *
     * @retval kErrorNone          Successfully detached from the Thread network.
     * @retval kErrorInvalidState  MLE is Disabled.
     *
     */
    Error BecomeDetached(void);

    /**
     * This method causes the Thread interface to attempt an MLE attach.
     *
     * @param[in]  aMode  Indicates what partitions to attach to.
     *
     * @retval kErrorNone          Successfully began the attach process.
     * @retval kErrorInvalidState  MLE is Disabled.
     * @retval kErrorBusy          An attach process is in progress.
     *
     */
    Error BecomeChild(AttachMode aMode);

    /**
     * This method indicates whether or not the Thread device is attached to a Thread network.
     *
     * @retval TRUE   Attached to a Thread network.
     * @retval FALSE  Not attached to a Thread network.
     *
     */
    bool IsAttached(void) const;

    /**
     * This method indicates whether device is currently attaching or not.
     *
     * Note that an already attached device may also be in attaching state. Examples of this include a leader/router
     * trying to attach to a better partition, or a child trying to find a better parent (when feature
     * `OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE` is enabled).
     *
     * @retval TRUE   Device is currently trying to attach.
     * @retval FALSE  Device is not in middle of attach process.
     *
     */
    bool IsAttaching(void) const { return (mAttachState != kAttachStateIdle); }

    /**
     * This method returns the current Thread device role.
     *
     * @returns The current Thread device role.
     *
     */
    DeviceRole GetRole(void) const { return mRole; }

    /**
     * This method indicates whether device role is disabled.
     *
     * @retval TRUE   Device role is disabled.
     * @retval FALSE  Device role is not disabled.
     *
     */
    bool IsDisabled(void) const { return (mRole == kRoleDisabled); }

    /**
     * This method indicates whether device role is detached.
     *
     * @retval TRUE   Device role is detached.
     * @retval FALSE  Device role is not detached.
     *
     */
    bool IsDetached(void) const { return (mRole == kRoleDetached); }

    /**
     * This method indicates whether device role is child.
     *
     * @retval TRUE   Device role is child.
     * @retval FALSE  Device role is not child.
     *
     */
    bool IsChild(void) const { return (mRole == kRoleChild); }

    /**
     * This method indicates whether device role is router.
     *
     * @retval TRUE   Device role is router.
     * @retval FALSE  Device role is not router.
     *
     */
    bool IsRouter(void) const { return (mRole == kRoleRouter); }

    /**
     * This method indicates whether device role is leader.
     *
     * @retval TRUE   Device role is leader.
     * @retval FALSE  Device role is not leader.
     *
     */
    bool IsLeader(void) const { return (mRole == kRoleLeader); }

    /**
     * This method indicates whether device role is either router or leader.
     *
     * @retval TRUE   Device role is either router or leader.
     * @retval FALSE  Device role is neither router nor leader.
     *
     */
    bool IsRouterOrLeader(void) const;

    /**
     * This method returns the Device Mode as reported in the Mode TLV.
     *
     * @returns The Device Mode as reported in the Mode TLV.
     *
     */
    DeviceMode GetDeviceMode(void) const { return mDeviceMode; }

    /**
     * This method sets the Device Mode as reported in the Mode TLV.
     *
     * @param[in]  aDeviceMode  The device mode to set.
     *
     * @retval kErrorNone         Successfully set the Mode TLV.
     * @retval kErrorInvalidArgs  The mode combination specified in @p aMode is invalid.
     *
     */
    Error SetDeviceMode(DeviceMode aDeviceMode);

    /**
     * This method indicates whether or not the device is rx-on-when-idle.
     *
     * @returns TRUE if rx-on-when-idle, FALSE otherwise.
     *
     */
    bool IsRxOnWhenIdle(void) const { return mDeviceMode.IsRxOnWhenIdle(); }

    /**
     * This method indicates whether or not the device is a Full Thread Device.
     *
     * @returns TRUE if a Full Thread Device, FALSE otherwise.
     *
     */
    bool IsFullThreadDevice(void) const { return mDeviceMode.IsFullThreadDevice(); }

    /**
     * This method indicates whether or not the device requests Full Network Data.
     *
     * @returns TRUE if requests Full Network Data, FALSE otherwise.
     *
     */
    bool IsFullNetworkData(void) const { return mDeviceMode.IsFullNetworkData(); }

    /**
     * This method indicates whether or not the device is a Minimal End Device.
     *
     * @returns TRUE if the device is a Minimal End Device, FALSE otherwise.
     *
     */
    bool IsMinimalEndDevice(void) const { return mDeviceMode.IsMinimalEndDevice(); }

    /**
     * This method returns a pointer to the Mesh Local Prefix.
     *
     * @returns A reference to the Mesh Local Prefix.
     *
     */
    const MeshLocalPrefix &GetMeshLocalPrefix(void) const
    {
        return static_cast<const MeshLocalPrefix &>(mMeshLocal16.GetAddress().GetPrefix());
    }

    /**
     * This method sets the Mesh Local Prefix.
     *
     * @param[in]  aMeshLocalPrefix  A reference to the Mesh Local Prefix.
     *
     */
    void SetMeshLocalPrefix(const MeshLocalPrefix &aMeshLocalPrefix);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    /**
     * This method sets the Mesh Local IID.
     *
     * Available only when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` is enabled.
     *
     * @param[in] aMlIid  The Mesh Local IID.
     *
     * @retval kErrorNone           Successfully configured Mesh Local IID.
     * @retval kErrorInvalidState   If the Thread stack is already enabled.
     *
     */
    Error SetMeshLocalIid(const Ip6::InterfaceIdentifier &aMlIid);
#endif

    /**
     * This method applies the Mesh Local Prefix.
     *
     */
    void ApplyMeshLocalPrefix(void);

    /**
     * This method returns a reference to the Thread link-local address.
     *
     * The Thread link local address is derived using IEEE802.15.4 Extended Address as Interface Identifier.
     *
     * @returns A reference to the Thread link local address.
     *
     */
    const Ip6::Address &GetLinkLocalAddress(void) const { return mLinkLocal64.GetAddress(); }

    /**
     * This method updates the link local address.
     *
     * Call this method when the IEEE 802.15.4 Extended Address has changed.
     *
     */
    void UpdateLinkLocalAddress(void);

    /**
     * This method returns a reference to the link-local all Thread nodes multicast address.
     *
     * @returns A reference to the link-local all Thread nodes multicast address.
     *
     */
    const Ip6::Address &GetLinkLocalAllThreadNodesAddress(void) const { return mLinkLocalAllThreadNodes.GetAddress(); }

    /**
     * This method returns a reference to the realm-local all Thread nodes multicast address.
     *
     * @returns A reference to the realm-local all Thread nodes multicast address.
     *
     */
    const Ip6::Address &GetRealmLocalAllThreadNodesAddress(void) const
    {
        return mRealmLocalAllThreadNodes.GetAddress();
    }

    /**
     * This method gets the parent when operating in End Device mode.
     *
     * @returns A reference to the parent.
     *
     */
    Router &GetParent(void) { return mParent; }

    /**
     * This method get the parent candidate.
     *
     * The parent candidate is valid when attempting to attach to a new parent.
     *
     */
    Router &GetParentCandidate(void) { return mParentCandidate; }

    /**
     * This method indicates whether or not an IPv6 address is an RLOC.
     *
     * @retval TRUE   If @p aAddress is an RLOC.
     * @retval FALSE  If @p aAddress is not an RLOC.
     *
     */
    bool IsRoutingLocator(const Ip6::Address &aAddress) const;

    /**
     * This method indicates whether or not an IPv6 address is an ALOC.
     *
     * @retval TRUE   If @p aAddress is an ALOC.
     * @retval FALSE  If @p aAddress is not an ALOC.
     *
     */
    bool IsAnycastLocator(const Ip6::Address &aAddress) const;

    /**
     * This method indicates whether or not an IPv6 address is a Mesh Local Address.
     *
     * @retval TRUE   If @p aAddress is a Mesh Local Address.
     * @retval FALSE  If @p aAddress is not a Mesh Local Address.
     *
     */
    bool IsMeshLocalAddress(const Ip6::Address &aAddress) const;

    /**
     * This method returns the MLE Timeout value.
     *
     * @returns The MLE Timeout value in seconds.
     *
     */
    uint32_t GetTimeout(void) const { return mTimeout; }

    /**
     * This method sets the MLE Timeout value.
     *
     * @param[in]  aTimeout  The Timeout value in seconds.
     *
     */
    void SetTimeout(uint32_t aTimeout);

    /**
     * This method returns the RLOC16 assigned to the Thread interface.
     *
     * @returns The RLOC16 assigned to the Thread interface.
     *
     */
    uint16_t GetRloc16(void) const;

    /**
     * This method returns a reference to the RLOC assigned to the Thread interface.
     *
     * @returns A reference to the RLOC assigned to the Thread interface.
     *
     */
    const Ip6::Address &GetMeshLocal16(void) const { return mMeshLocal16.GetAddress(); }

    /**
     * This method returns a reference to the ML-EID assigned to the Thread interface.
     *
     * @returns A reference to the ML-EID assigned to the Thread interface.
     *
     */
    const Ip6::Address &GetMeshLocal64(void) const { return mMeshLocal64.GetAddress(); }

    /**
     * This method returns the Router ID of the Leader.
     *
     * @returns The Router ID of the Leader.
     *
     */
    uint8_t GetLeaderId(void) const { return mLeaderData.GetLeaderRouterId(); }

    /**
     * This method retrieves the Leader's RLOC.
     *
     * @param[out]  aAddress  A reference to the Leader's RLOC.
     *
     * @retval kErrorNone      Successfully retrieved the Leader's RLOC.
     * @retval kErrorDetached  The Thread interface is not currently attached to a Thread Partition.
     *
     */
    Error GetLeaderAddress(Ip6::Address &aAddress) const;

    /**
     * This method retrieves the Leader's ALOC.
     *
     * @param[out]  aAddress  A reference to the Leader's ALOC.
     *
     * @retval kErrorNone      Successfully retrieved the Leader's ALOC.
     * @retval kErrorDetached  The Thread interface is not currently attached to a Thread Partition.
     *
     */
    Error GetLeaderAloc(Ip6::Address &aAddress) const { return GetLocatorAddress(aAddress, kAloc16Leader); }

    /**
     * This method computes the Commissioner's ALOC.
     *
     * @param[out]  aAddress        A reference to the Commissioner's ALOC.
     * @param[in]   aSessionId      Commissioner session id.
     *
     * @retval kErrorNone      Successfully retrieved the Commissioner's ALOC.
     * @retval kErrorDetached  The Thread interface is not currently attached to a Thread Partition.
     *
     */
    Error GetCommissionerAloc(Ip6::Address &aAddress, uint16_t aSessionId) const
    {
        return GetLocatorAddress(aAddress, CommissionerAloc16FromId(aSessionId));
    }

    /**
     * This method retrieves the Service ALOC for given Service ID.
     *
     * @param[in]   aServiceId Service ID to get ALOC for.
     * @param[out]  aAddress   A reference to the Service ALOC.
     *
     * @retval kErrorNone      Successfully retrieved the Service ALOC.
     * @retval kErrorDetached  The Thread interface is not currently attached to a Thread Partition.
     *
     */
    Error GetServiceAloc(uint8_t aServiceId, Ip6::Address &aAddress) const;

    /**
     * This method returns the most recently received Leader Data.
     *
     * @returns  A reference to the most recently received Leader Data.
     *
     */
    const LeaderData &GetLeaderData(void);

    /**
     * This method derives the Child ID from a given RLOC16.
     *
     * @param[in]  aRloc16  The RLOC16 value.
     *
     * @returns The Child ID portion of an RLOC16.
     *
     */
    static uint16_t ChildIdFromRloc16(uint16_t aRloc16) { return aRloc16 & kMaxChildId; }

    /**
     * This method derives the Router ID portion from a given RLOC16.
     *
     * @param[in]  aRloc16  The RLOC16 value.
     *
     * @returns The Router ID portion of an RLOC16.
     *
     */
    static uint8_t RouterIdFromRloc16(uint16_t aRloc16) { return aRloc16 >> kRouterIdOffset; }

    /**
     * This method returns whether the two RLOC16 have the same Router ID.
     *
     * @param[in]  aRloc16A  The first RLOC16 value.
     * @param[in]  aRloc16B  The second RLOC16 value.
     *
     * @returns true if the two RLOC16 have the same Router ID, false otherwise.
     *
     */
    static bool RouterIdMatch(uint16_t aRloc16A, uint16_t aRloc16B)
    {
        return RouterIdFromRloc16(aRloc16A) == RouterIdFromRloc16(aRloc16B);
    }

    /**
     * This method returns the Service ID corresponding to a Service ALOC16.
     *
     * @param[in]  aAloc16  The Servicer ALOC16 value.
     *
     * @returns The Service ID corresponding to given ALOC16.
     *
     */
    static uint8_t ServiceIdFromAloc(uint16_t aAloc16) { return static_cast<uint8_t>(aAloc16 - kAloc16ServiceStart); }

    /**
     * This method returns the Service Aloc corresponding to a Service ID.
     *
     * @param[in]  aServiceId  The Service ID value.
     *
     * @returns The Service ALOC16 corresponding to given ID.
     *
     */
    static uint16_t ServiceAlocFromId(uint8_t aServiceId)
    {
        return static_cast<uint16_t>(aServiceId + kAloc16ServiceStart);
    }

    /**
     * This method returns the Commissioner Aloc corresponding to a Commissioner Session ID.
     *
     * @param[in]  aSessionId   The Commissioner Session ID value.
     *
     * @returns The Commissioner ALOC16 corresponding to given ID.
     *
     */
    static uint16_t CommissionerAloc16FromId(uint16_t aSessionId)
    {
        return static_cast<uint16_t>((aSessionId & kAloc16CommissionerMask) + kAloc16CommissionerStart);
    }

    /**
     * This method derives RLOC16 from a given Router ID.
     *
     * @param[in]  aRouterId  The Router ID value.
     *
     * @returns The RLOC16 corresponding to the given Router ID.
     *
     */
    static uint16_t Rloc16FromRouterId(uint8_t aRouterId)
    {
        return static_cast<uint16_t>(aRouterId << kRouterIdOffset);
    }

    /**
     * This method indicates whether or not @p aRloc16 refers to an active router.
     *
     * @param[in]  aRloc16  The RLOC16 value.
     *
     * @retval TRUE   If @p aRloc16 refers to an active router.
     * @retval FALSE  If @p aRloc16 does not refer to an active router.
     *
     */
    static bool IsActiveRouter(uint16_t aRloc16) { return ChildIdFromRloc16(aRloc16) == 0; }

    /**
     * This method returns a reference to the send queue.
     *
     * @returns A reference to the send queue.
     *
     */
    const MessageQueue &GetMessageQueue(void) const { return mDelayedResponses; }

    /**
     * This method frees multicast MLE Data Response from Delayed Message Queue if any.
     *
     */
    void RemoveDelayedDataResponseMessage(void);

    /**
     * This method converts a device role into a human-readable string.
     *
     */
    static const char *RoleToString(DeviceRole aRole);

    /**
     * This method gets the MLE counters.
     *
     * @returns A reference to the MLE counters.
     *
     */
    const otMleCounters &GetCounters(void) const { return mCounters; }

    /**
     * This method resets the MLE counters.
     *
     */
    void ResetCounters(void) { memset(&mCounters, 0, sizeof(mCounters)); }

    /**
     * This function registers the client callback that is called when processing an MLE Parent Response message.
     *
     * @param[in]  aCallback A pointer to a function that is called to deliver MLE Parent Response data.
     * @param[in]  aContext  A pointer to application-specific context.
     *
     */
    void RegisterParentResponseStatsCallback(otThreadParentResponseCallback aCallback, void *aContext);

    /**
     * This method requests MLE layer to prepare and send a shorter version of Child ID Request message by only
     * including the mesh-local IPv6 address in the Address Registration TLV.
     *
     * This method should be called when a previous MLE Child ID Request message would require fragmentation at 6LoWPAN
     * layer.
     *
     */
    void RequestShorterChildIdRequest(void);

    /**
     * This method gets the RLOC or ALOC of a given RLOC16 or ALOC16.
     *
     * @param[out]  aAddress  A reference to the RLOC or ALOC.
     * @param[in]   aLocator  RLOC16 or ALOC16.
     *
     * @retval kErrorNone      If got the RLOC or ALOC successfully.
     * @retval kErrorDetached  If device is detached.
     *
     */
    Error GetLocatorAddress(Ip6::Address &aAddress, uint16_t aLocator) const;

    /**
     * This method schedules a Child Update Request.
     *
     */
    void ScheduleChildUpdateRequest(void);

    /*
     * This method indicates whether or not the device has restored the network information from
     * non-volatile settings after boot.
     *
     * @retval true  Successfully restored the network information.
     * @retval false No valid network information was found.
     *
     */
    bool HasRestored(void) const { return mHasRestored; }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /**
     * This method gets the CSL timeout.
     *
     * @returns CSL timeout
     *
     */
    uint32_t GetCslTimeout(void) const { return mCslTimeout; }

    /**
     * This method sets the CSL timeout.
     *
     * @param[in]  aTimeout  The CSL timeout in seconds.
     *
     */
    void SetCslTimeout(uint32_t aTimeout);

    /**
     * This method calculates CSL metric of parent.
     *
     * @param[in] aCslClockAccuracy The CSL Clock Accuracy.
     * @param[in] aCslUncertainty The CSL Uncertainty.
     *
     * @returns CSL metric.
     */
    uint64_t CalcParentCslMetric(uint8_t aCslClockAccuracy, uint8_t aCslUncertainty);

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

protected:
    /**
     * MLE Command Types.
     *
     */
    enum Command : uint8_t
    {
        kCommandLinkRequest                   = 0,  ///< Link Request
        kCommandLinkAccept                    = 1,  ///< Link Accept
        kCommandLinkAcceptAndRequest          = 2,  ///< Link Accept and Reject
        kCommandLinkReject                    = 3,  ///< Link Reject
        kCommandAdvertisement                 = 4,  ///< Advertisement
        kCommandUpdate                        = 5,  ///< Update
        kCommandUpdateRequest                 = 6,  ///< Update Request
        kCommandDataRequest                   = 7,  ///< Data Request
        kCommandDataResponse                  = 8,  ///< Data Response
        kCommandParentRequest                 = 9,  ///< Parent Request
        kCommandParentResponse                = 10, ///< Parent Response
        kCommandChildIdRequest                = 11, ///< Child ID Request
        kCommandChildIdResponse               = 12, ///< Child ID Response
        kCommandChildUpdateRequest            = 13, ///< Child Update Request
        kCommandChildUpdateResponse           = 14, ///< Child Update Response
        kCommandAnnounce                      = 15, ///< Announce
        kCommandDiscoveryRequest              = 16, ///< Discovery Request
        kCommandDiscoveryResponse             = 17, ///< Discovery Response
        kCommandLinkMetricsManagementRequest  = 18, ///< Link Metrics Management Request
        kCommandLinkMetricsManagementResponse = 19, ///< Link Metrics Management Response
        kCommandLinkProbe                     = 20, ///< Link Probe
        kCommandTimeSync                      = 99, ///< Time Sync (when OPENTHREAD_CONFIG_TIME_SYNC_ENABLE enabled)
    };

    /**
     * States during attach (when searching for a parent).
     *
     */
    enum AttachState : uint8_t
    {
        kAttachStateIdle,                ///< Not currently searching for a parent.
        kAttachStateProcessAnnounce,     ///< Waiting to process a received Announce (to switch channel/pan-id).
        kAttachStateStart,               ///< Starting to look for a parent.
        kAttachStateParentRequestRouter, ///< Searching for a Router to attach to.
        kAttachStateParentRequestReed,   ///< Searching for Routers or REEDs to attach to.
        kAttachStateAnnounce,            ///< Send Announce messages
        kAttachStateChildIdRequest,      ///< Sending a Child ID Request message.
    };

    /**
     * States when reattaching network using stored dataset
     *
     */
    enum ReattachState : uint8_t
    {
        kReattachStop    = 0, ///< Reattach process is disabled or finished
        kReattachStart   = 1, ///< Start reattach process
        kReattachActive  = 2, ///< Reattach using stored Active Dataset
        kReattachPending = 3, ///< Reattach using stored Pending Dataset
    };

    static constexpr uint16_t kMleMaxResponseDelay = 1000u; ///< Max delay before responding to a multicast request.

    /**
     * This enumeration type is used in `AppendAddressRegistration()` to determine which addresses to include in the
     * appended Address Registration TLV.
     *
     */
    enum AddressRegistrationMode : uint8_t
    {
        kAppendAllAddresses,  ///< Append all addresses (unicast/multicast) in Address Registration TLV.
        kAppendMeshLocalOnly, ///< Only append the Mesh Local (ML-EID) address in Address Registration TLV.
    };

    /**
     * This enumeration represents the message actions used in `Log()` methods.
     *
     */
    enum MessageAction : uint8_t
    {
        kMessageSend,
        kMessageReceive,
        kMessageDelay,
        kMessageRemoveDelayed,
    };

    /**
     * This enumeration represents message types used in `Log()` methods.
     *
     */
    enum MessageType : uint8_t
    {
        kTypeAdvertisement,
        kTypeAnnounce,
        kTypeChildIdRequest,
        kTypeChildIdRequestShort,
        kTypeChildIdResponse,
        kTypeChildUpdateRequestOfParent,
        kTypeChildUpdateResponseOfParent,
        kTypeDataRequest,
        kTypeDataResponse,
        kTypeDiscoveryRequest,
        kTypeDiscoveryResponse,
        kTypeGenericDelayed,
        kTypeGenericUdp,
        kTypeParentRequestToRouters,
        kTypeParentRequestToRoutersReeds,
        kTypeParentResponse,
#if OPENTHREAD_FTD
        kTypeAddressRelease,
        kTypeAddressReleaseReply,
        kTypeAddressReply,
        kTypeAddressSolicit,
        kTypeChildUpdateRequestOfChild,
        kTypeChildUpdateResponseOfChild,
        kTypeChildUpdateResponseOfUnknownChild,
        kTypeLinkAccept,
        kTypeLinkAcceptAndRequest,
        kTypeLinkReject,
        kTypeLinkRequest,
        kTypeParentRequest,
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        kTypeTimeSync,
#endif
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        kTypeLinkMetricsManagementRequest,
        kTypeLinkMetricsManagementResponse,
        kTypeLinkProbe,
#endif
    };

    /**
     * This type represents a Challenge (or Response) data.
     *
     */
    struct Challenge
    {
        uint8_t mBuffer[kMaxChallengeSize]; ///< Buffer containing the challenge/response byte sequence.
        uint8_t mLength;                    ///< Challenge length (in bytes).

        /**
         * This method generates a cryptographically secure random sequence to populate the challenge data.
         *
         */
        void GenerateRandom(void);

        /**
         * This method indicates whether the Challenge matches a given buffer.
         *
         * @param[in] aBuffer   A pointer to a buffer to compare with the Challenge.
         * @param[in] aLength   Length of @p aBuffer (in bytes).
         *
         * @retval TRUE  If the Challenge matches the given buffer.
         * @retval FALSE If the Challenge does not match the given buffer.
         *
         */
        bool Matches(const uint8_t *aBuffer, uint8_t aLength) const;

        /**
         * This method indicates whether two Challenge data byte sequences are equal or not.
         *
         * @param[in] aOther   Another Challenge data to compare.
         *
         * @retval TRUE  If the two Challenges match.
         * @retval FALSE If the two Challenges do not match.
         *
         */
        bool operator==(const Challenge &aOther) const { return Matches(aOther.mBuffer, aOther.mLength); }
    };

    /**
     * This type represents list of requested TLVs in a TLV Request TLV.
     *
     */
    struct RequestedTlvs
    {
        static constexpr uint8_t kMaxNumTlvs = 16; ///< Maximum number of TLVs in request array.

        uint8_t mTlvs[kMaxNumTlvs]; ///< Array of requested TLVs.
        uint8_t mNumTlvs;           ///< Number of TLVs in the array.
    };

    /**
     * This method allocates a new message buffer for preparing an MLE message.
     *
     * @returns A pointer to the message or nullptr if insufficient message buffers are available.
     *
     */
    Message *NewMleMessage(void);

    /**
     * This method sets the device role.
     *
     * @param[in] aRole A device role.
     *
     */
    void SetRole(DeviceRole aRole);

    /**
     * This method sets the attach state
     *
     * @param[in] aState An attach state
     *
     */
    void SetAttachState(AttachState aState);

    /**
     * This method appends an MLE header to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aCommand  The MLE Command Type.
     *
     * @retval kErrorNone    Successfully appended the header.
     * @retval kErrorNoBufs  Insufficient buffers available to append the header.
     *
     */
    Error AppendHeader(Message &aMessage, Command aCommand);

    /**
     * This method appends a Source Address TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone    Successfully appended the Source Address TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Source Address TLV.
     *
     */
    Error AppendSourceAddress(Message &aMessage) const;

    /**
     * This method appends a Mode TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aMode     The Device Mode.
     *
     * @retval kErrorNone    Successfully appended the Mode TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Mode TLV.
     *
     */
    Error AppendMode(Message &aMessage, DeviceMode aMode);

    /**
     * This method appends a Timeout TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aTimeout  The Timeout value.
     *
     * @retval kErrorNone    Successfully appended the Timeout TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Timeout TLV.
     *
     */
    Error AppendTimeout(Message &aMessage, uint32_t aTimeout);

    /**
     * This method appends a Challenge TLV to a message.
     *
     * @param[in]  aMessage          A reference to the message.
     * @param[in]  aChallenge        A pointer to the Challenge value.
     * @param[in]  aChallengeLength  The length of the Challenge value in bytes.
     *
     * @retval kErrorNone    Successfully appended the Challenge TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Challenge TLV.
     *
     */
    Error AppendChallenge(Message &aMessage, const uint8_t *aChallenge, uint8_t aChallengeLength);

    /**
     * This method appends a Challenge TLV to a message.
     *
     * @param[in]  aMessage          A reference to the message.
     * @param[in]  aChallenge        A reference to the Challenge data.
     *
     * @retval kErrorNone    Successfully appended the Challenge TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Challenge TLV.
     *
     */
    Error AppendChallenge(Message &aMessage, const Challenge &aChallenge);

    /**
     * This method reads Challenge TLV from a message.
     *
     * @param[in]  aMessage          A reference to the message.
     * @param[out] aChallenge        A reference to the Challenge data where to output the read value.
     *
     * @retval kErrorNone       Successfully read the Challenge TLV.
     * @retval kErrorNotFound   Challenge TLV was not found in the message.
     * @retval kErrorParse      Challenge TLV was found but could not be parsed.
     *
     */
    Error ReadChallenge(const Message &aMessage, Challenge &aChallenge);

    /**
     * This method appends a Response TLV to a message.
     *
     * @param[in]  aMessage         A reference to the message.
     * @param[in]  aResponse        A reference to the Response data.
     *
     * @retval kErrorNone    Successfully appended the Response TLV.
     * @retval kErrorNoBufs  Insufficient buffers available to append the Response TLV.
     *
     */
    Error AppendResponse(Message &aMessage, const Challenge &aResponse);

    /**
     * This method reads Response TLV from a message.
     *
     * @param[in]  aMessage         A reference to the message.
     * @param[out] aResponse        A reference to the Response data where to output the read value.
     *
     * @retval kErrorNone       Successfully read the Response TLV.
     * @retval kErrorNotFound   Response TLV was not found in the message.
     * @retval kErrorParse      Response TLV was found but could not be parsed.
     *
     */
    Error ReadResponse(const Message &aMessage, Challenge &aResponse);

    /**
     * This method appends a Link Frame Counter TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Link Frame Counter TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Link Frame Counter TLV.
     *
     */
    Error AppendLinkFrameCounter(Message &aMessage);

    /**
     * This method reads Link and MLE Frame Counters from a message.
     *
     * Link Frame Counter TLV must be present in the message and its value is read into @p aLinkFrameCounter. If MLE
     * Frame Counter TLV is present in the message, its value is read into @p aMleFrameCounter. If the MLE Frame
     * Counter TLV is not present in the message, then @p aMleFrameCounter is set to same value as @p aLinkFrameCounter.
     *
     * @param[in]  aMesssage           A reference to the message to read from.
     * @param[out] aLinkFrameCounter   A reference to an `uint32_t` to output the Link Frame Counter.
     * @param[out] aMleFrameCounter    A reference to an `uint32_t` to output the MLE Frame Counter.
     *
     * @retval kErrorNone       Successfully read the counters.
     * @retval kErrorNotFound   Link Frame Counter TLV was not found in the message.
     * @retval kErrorParse      TLVs are not well-formed.
     *
     */
    Error ReadFrameCounters(const Message &aMessage, uint32_t &aLinkFrameCounter, uint32_t &aMleFrameCounter) const;

    /**
     * This method appends an MLE Frame Counter TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Frame Counter TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the MLE Frame Counter TLV.
     *
     */
    Error AppendMleFrameCounter(Message &aMessage);

    /**
     * This method appends an Address16 TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aRloc16   The RLOC16 value.
     *
     * @retval kErrorNone     Successfully appended the Address16 TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Address16 TLV.
     *
     */
    Error AppendAddress16(Message &aMessage, uint16_t aRloc16);

    /**
     * This method appends a Network Data TLV to the message.
     *
     * @param[in]  aMessage     A reference to the message.
     * @param[in]  aStableOnly  TRUE to append stable data, FALSE otherwise.
     *
     * @retval kErrorNone     Successfully appended the Network Data TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Network Data TLV.
     *
     */
    Error AppendNetworkData(Message &aMessage, bool aStableOnly);

    /**
     * This method appends a TLV Request TLV to a message.
     *
     * @param[in]  aMessage     A reference to the message.
     * @param[in]  aTlvs        A pointer to the list of TLV types.
     * @param[in]  aTlvsLength  The number of TLV types in @p aTlvs
     *
     * @retval kErrorNone     Successfully appended the TLV Request TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the TLV Request TLV.
     *
     */
    Error AppendTlvRequest(Message &aMessage, const uint8_t *aTlvs, uint8_t aTlvsLength);

    /**
     * This method reads TLV Request TLV from a message.
     *
     * @param[in]  aMessage         A reference to the message.
     * @param[out] aRequestedTlvs   A reference to output the read list of requested TLVs.
     *
     * @retval kErrorNone       Successfully read the TLV.
     * @retval kErrorNotFound   TLV was not found in the message.
     * @retval kErrorParse      TLV was found but could not be parsed.
     *
     */
    Error FindTlvRequest(const Message &aMessage, RequestedTlvs &aRequestedTlvs);

    /**
     * This method appends a Leader Data TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Leader Data TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Leader Data TLV.
     *
     */
    Error AppendLeaderData(Message &aMessage);

    /**
     * This method reads Leader Data TLV from a message.
     *
     * @param[in]  aMessage        A reference to the message.
     * @param[out] aLeaderData     A reference to output the Leader Data.
     *
     * @retval kErrorNone       Successfully read the TLV.
     * @retval kErrorNotFound   TLV was not found in the message.
     * @retval kErrorParse      TLV was found but could not be parsed.
     *
     */
    Error ReadLeaderData(const Message &aMessage, LeaderData &aLeaderData);

    /**
     * This method appends a Scan Mask TLV to a message.
     *
     * @param[in]  aMessage   A reference to the message.
     * @param[in]  aScanMask  The Scan Mask value.
     *
     * @retval kErrorNone     Successfully appended the Scan Mask TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Scan Mask TLV.
     *
     */
    Error AppendScanMask(Message &aMessage, uint8_t aScanMask);

    /**
     * This method appends a Status TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aStatus   The Status value.
     *
     * @retval kErrorNone     Successfully appended the Status TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Status TLV.
     *
     */
    Error AppendStatus(Message &aMessage, StatusTlv::Status aStatus);

    /**
     * This method appends a Link Margin TLV to a message.
     *
     * @param[in]  aMessage     A reference to the message.
     * @param[in]  aLinkMargin  The Link Margin value.
     *
     * @retval kErrorNone     Successfully appended the Link Margin TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Link Margin TLV.
     *
     */
    Error AppendLinkMargin(Message &aMessage, uint8_t aLinkMargin);

    /**
     * This method appends a Version TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Version TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Version TLV.
     *
     */
    Error AppendVersion(Message &aMessage);

    /**
     * This method appends an Address Registration TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     * @param[in]  aMode     Determines which addresses to include in the TLV (see `AddressRegistrationMode`).
     *
     * @retval kErrorNone     Successfully appended the Address Registration TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Address Registration TLV.
     *
     */
    Error AppendAddressRegistration(Message &aMessage, AddressRegistrationMode aMode = kAppendAllAddresses);

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    /**
     * This method appends a Time Request TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Time Request TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Time Request TLV.
     *
     */
    Error AppendTimeRequest(Message &aMessage);

    /**
     * This method appends a Time Parameter TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Time Parameter TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Time Parameter TLV.
     *
     */
    Error AppendTimeParameter(Message &aMessage);

    /**
     * This method appends a XTAL Accuracy TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the XTAL Accuracy TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the XTAl Accuracy TLV.
     *
     */
    Error AppendXtalAccuracy(Message &aMessage);
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE) || OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /**
     * This method appends a CSL Channel TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the CSL Channel TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the CSL Channel TLV.
     *
     */
    Error AppendCslChannel(Message &aMessage);

    /**
     * This method appends a CSL Sync Timeout TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the CSL Timeout TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the CSL Timeout TLV.
     *
     */
    Error AppendCslTimeout(Message &aMessage);
#endif // (OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE) || OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    /**
     * This method appends a CSL Clock Accuracy TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the CSL Accuracy TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the CSL Accuracy TLV.
     */
    Error AppendCslClockAccuracy(Message &aMessage);
#endif

    /**
     * This method appends a Active Timestamp TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Active Timestamp TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Active Timestamp TLV.
     *
     */
    Error AppendActiveTimestamp(Message &aMessage);

    /**
     * This method appends a Pending Timestamp TLV to a message.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone     Successfully appended the Pending Timestamp TLV.
     * @retval kErrorNoBufs   Insufficient buffers available to append the Pending Timestamp TLV.
     *
     */
    Error AppendPendingTimestamp(Message &aMessage);

    /**
     * This method checks if the destination is reachable.
     *
     * @param[in]  aMeshDest   The RLOC16 of the destination.
     * @param[in]  aIp6Header  The IPv6 header of the message.
     *
     * @retval kErrorNone      The destination is reachable.
     * @retval kErrorNoRoute   The destination is not reachable and the message should be dropped.
     *
     */
    Error CheckReachability(uint16_t aMeshDest, Ip6::Header &aIp6Header);

    /**
     * This method returns the next hop towards an RLOC16 destination.
     *
     * @param[in]  aDestination  The RLOC16 of the destination.
     *
     * @returns A RLOC16 of the next hop if a route is known, kInvalidRloc16 otherwise.
     *
     */
    Mac::ShortAddress GetNextHop(uint16_t aDestination) const;

    /**
     * This method generates an MLE Data Request message.
     *
     * @param[in]  aDestination      A reference to the IPv6 address of the destination.
     * @param[in]  aTlvs             A pointer to requested TLV types.
     * @param[in]  aTlvsLength       The number of TLV types in @p aTlvs.
     * @param[in]  aDelay            Delay in milliseconds before the Data Request message is sent.
     * @param[in]  aExtraTlvs        A pointer to extra TLVs.
     * @param[in]  aExtraTlvsLength  Length of extra TLVs.
     *
     * @retval kErrorNone     Successfully generated an MLE Data Request message.
     * @retval kErrorNoBufs   Insufficient buffers to generate the MLE Data Request message.
     *
     */
    Error SendDataRequest(const Ip6::Address &aDestination,
                          const uint8_t *     aTlvs,
                          uint8_t             aTlvsLength,
                          uint16_t            aDelay,
                          const uint8_t *     aExtraTlvs       = nullptr,
                          uint8_t             aExtraTlvsLength = 0);

    /**
     * This method generates an MLE Child Update Request message.
     *
     * @retval kErrorNone    Successfully generated an MLE Child Update Request message.
     * @retval kErrorNoBufs  Insufficient buffers to generate the MLE Child Update Request message.
     *
     */
    Error SendChildUpdateRequest(void);

    /**
     * This method generates an MLE Child Update Response message.
     *
     * @param[in]  aTlvs         A pointer to requested TLV types.
     * @param[in]  aNumTlvs      The number of TLV types in @p aTlvs.
     * @param[in]  aChallenge    The Challenge for the response.
     *
     * @retval kErrorNone     Successfully generated an MLE Child Update Response message.
     * @retval kErrorNoBufs   Insufficient buffers to generate the MLE Child Update Response message.
     *
     */
    Error SendChildUpdateResponse(const uint8_t *aTlvs, uint8_t aNumTlvs, const Challenge &aChallenge);

    /**
     * This method submits an MLE message to the UDP socket.
     *
     * @param[in]  aMessage      A reference to the message.
     * @param[in]  aDestination  A reference to the IPv6 address of the destination.
     *
     * @retval kErrorNone     Successfully submitted the MLE message.
     * @retval kErrorNoBufs   Insufficient buffers to form the rest of the MLE message.
     *
     */
    Error SendMessage(Message &aMessage, const Ip6::Address &aDestination);

    /**
     * This method sets the RLOC16 assigned to the Thread interface.
     *
     * @param[in]  aRloc16  The RLOC16 to set.
     *
     */
    void SetRloc16(uint16_t aRloc16);

    /**
     * This method sets the Device State to Detached.
     *
     */
    void SetStateDetached(void);

    /**
     * This method sets the Device State to Child.
     *
     */
    void SetStateChild(uint16_t aRloc16);

    /**
     * This method sets the Leader's Partition ID, Weighting, and Router ID values.
     *
     * @param[in]  aPartitionId     The Leader's Partition ID value.
     * @param[in]  aWeighting       The Leader's Weighting value.
     * @param[in]  aLeaderRouterId  The Leader's Router ID value.
     *
     */
    void SetLeaderData(uint32_t aPartitionId, uint8_t aWeighting, uint8_t aLeaderRouterId);

    /**
     * This method adds a message to the message queue. The queued message will be transmitted after given delay.
     *
     * @param[in]  aMessage             The message to transmit after given delay.
     * @param[in]  aDestination         The IPv6 address of the recipient of the message.
     * @param[in]  aDelay               The delay in milliseconds before transmission of the message.
     *
     * @retval kErrorNone     Successfully queued the message to transmit after the delay.
     * @retval kErrorNoBufs   Insufficient buffers to queue the message.
     *
     */
    Error AddDelayedResponse(Message &aMessage, const Ip6::Address &aDestination, uint16_t aDelay);

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
    /**
     * This static method emits a log message with an IPv6 address.
     *
     * @param[in]  aAction     The message action (send/receive/delay, etc).
     * @param[in]  aType       The message type.
     * @param[in]  aAddress    The IPv6 address of the peer.
     *
     */
    static void Log(MessageAction aAction, MessageType aType, const Ip6::Address &aAddress);

    /**
     * This static method emits a log message with an IPv6 address and RLOC16.
     *
     * @param[in]  aAction     The message action (send/receive/delay, etc).
     * @param[in]  aType       The message type.
     * @param[in]  aAddress    The IPv6 address of the peer.
     * @param[in]  aRloc       The RLOC16.
     *
     */
    static void Log(MessageAction aAction, MessageType aType, const Ip6::Address &aAddress, uint16_t aRloc);
#else
    static void Log(MessageAction, MessageType, const Ip6::Address &) {}
    static void Log(MessageAction, MessageType, const Ip6::Address &, uint16_t) {}
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MLE == 1)

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
    /**
     * This static method emits a log message indicating an error in processing of a message.
     *
     * Note that log message is emitted only if there is an error, i.e., @p aError is not `kErrorNone`. The log
     * message will have the format "Failed to process {aMessageString} : {ErrorString}".
     *
     * @param[in]  aType      The message type.
     * @param[in]  aError     The error in processing of the message.
     *
     */
    static void LogProcessError(MessageType aType, Error aError);

    /**
     * This static method emits a log message indicating an error when sending a message.
     *
     * Note that log message is emitted only if there is an error, i.e. @p aError is not `kErrorNone`. The log
     * message will have the format "Failed to send {Message Type} : {ErrorString}".
     *
     * @param[in]  aType    The message type.
     * @param[in]  aError   The error in sending the message.
     *
     */
    static void LogSendError(MessageType aType, Error aError);
#else
    static void LogProcessError(MessageType, Error) {}
    static void LogSendError(MessageType, Error) {}
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLE == 1)

    /**
     * This method triggers MLE Announce on previous channel after the Thread device successfully
     * attaches and receives the new Active Commissioning Dataset if needed.
     *
     * MTD would send Announce immediately after attached.
     * FTD would delay to send Announce after tried to become Router or decided to stay in REED role.
     *
     */
    void InformPreviousChannel(void);

    /**
     * This method indicates whether or not in announce attach process.
     *
     * @retval true if attaching/attached on the announced parameters, false otherwise.
     *
     */
    bool IsAnnounceAttach(void) const { return mAlternatePanId != Mac::kPanIdBroadcast; }

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
    /**
     * This method converts an `AttachMode` enumeration value into a human-readable string.
     *
     * @param[in] aMode An attach mode
     *
     * @returns A human-readable string corresponding to the attach mode.
     *
     */
    static const char *AttachModeToString(AttachMode aMode);

    /**
     * This method converts an `AttachState` enumeration value into a human-readable string.
     *
     * @param[in] aState An attach state
     *
     * @returns A human-readable string corresponding to the attach state.
     *
     */
    static const char *AttachStateToString(AttachState aState);

    /**
     * This method converts a `ReattachState` enumeration value into a human-readable string.
     *
     * @param[in] aState A reattach state
     *
     * @returns A human-readable string corresponding to the reattach state.
     *
     */
    static const char *ReattachStateToString(ReattachState aState);
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    /**
     * This method sends a Link Metrics Management Request message.
     *
     * @param[in]  aDestination  A reference to the IPv6 address of the destination.
     * @param[in]  aSubTlvs      A pointer to the buffer of the sub-TLVs in the message.
     * @param[in]  aLength       The overall length of @p aSubTlvs.
     *
     * @retval kErrorNone     Successfully sent a Link Metrics Management Request.
     * @retval kErrorNoBufs   Insufficient buffers to generate the MLE Link Metrics Management Request message.
     *
     */
    Error SendLinkMetricsManagementRequest(const Ip6::Address &aDestination, const uint8_t *aSubTlvs, uint8_t aLength);

    /**
     * This method sends an MLE Link Probe message.
     *
     * @param[in]  aDestination  A reference to the IPv6 address of the destination.
     * @param[in]  aSeriesId     The Series ID [1, 254] which the Probe message targets at.
     * @param[in]  aBuf          A pointer to the data payload.
     * @param[in]  aLength       The length of the data payload in Link Probe TLV, [0, 64].
     *
     * @retval kErrorNone         Successfully sent a Link Metrics Management Request.
     * @retval kErrorNoBufs       Insufficient buffers to generate the MLE Link Metrics Management Request message.
     * @retval kErrorInvalidArgs  Series ID is not a valid value, not within range [1, 254].
     *
     */
    Error SendLinkProbe(const Ip6::Address &aDestination, uint8_t aSeriesId, uint8_t *aBuf, uint8_t aLength);

#endif

    Ip6::Netif::UnicastAddress mLeaderAloc; ///< Leader anycast locator

    LeaderData    mLeaderData;               ///< Last received Leader Data TLV.
    bool          mRetrieveNewNetworkData;   ///< Indicating new Network Data is needed if set.
    DeviceRole    mRole;                     ///< Current Thread role.
    Router        mParent;                   ///< Parent information.
    Router        mParentCandidate;          ///< Parent candidate information.
    NeighborTable mNeighborTable;            ///< The neighbor table.
    DeviceMode    mDeviceMode;               ///< Device mode setting.
    AttachState   mAttachState;              ///< The parent request state.
    ReattachState mReattachState;            ///< Reattach state
    uint16_t      mAttachCounter;            ///< Attach attempt counter.
    uint16_t      mAnnounceDelay;            ///< Delay in between sending Announce messages during attach.
    TimerMilli    mAttachTimer;              ///< The timer for driving the attach process.
    TimerMilli    mDelayedResponseTimer;     ///< The timer to delay MLE responses.
    TimerMilli    mMessageTransmissionTimer; ///< The timer for (re-)sending of MLE messages (e.g. Child Update).
    uint8_t       mParentLeaderCost;

private:
    static constexpr uint8_t kMleHopLimit        = 255;
    static constexpr uint8_t kMleSecurityTagSize = 4; // Security tag size in bytes.

    // Parameters related to "periodic parent search" feature (CONFIG_ENABLE_PERIODIC_PARENT_SEARCH).
    // All timer intervals are converted to milliseconds.
    static constexpr uint32_t kParentSearchCheckInterval   = (OPENTHREAD_CONFIG_PARENT_SEARCH_CHECK_INTERVAL * 1000u);
    static constexpr uint32_t kParentSearchBackoffInterval = (OPENTHREAD_CONFIG_PARENT_SEARCH_BACKOFF_INTERVAL * 1000u);
    static constexpr uint32_t kParentSearchJitterInterval  = (15 * 1000u);
    static constexpr int8_t   kParentSearchRssThreadhold   = OPENTHREAD_CONFIG_PARENT_SEARCH_RSS_THRESHOLD;

    // Parameters for "attach backoff" feature (CONFIG_ENABLE_ATTACH_BACKOFF) - Intervals are in milliseconds.
    static constexpr uint32_t kAttachBackoffMinInterval = OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MINIMUM_INTERVAL;
    static constexpr uint32_t kAttachBackoffMaxInterval = OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL;
    static constexpr uint32_t kAttachBackoffJitter      = OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL;
    static constexpr uint32_t kAttachBackoffDelayToResetCounter =
        OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_DELAY_TO_RESET_BACKOFF_INTERVAL;

    enum StartMode : uint8_t // Used in `Start()`.
    {
        kNormalAttach,
        kAnnounceAttach, // Try to attach on the announced thread network with newer active timestamp.
    };

    enum StopMode : uint8_t // Used in `Stop()`.
    {
        kKeepNetworkDatasets,
        kUpdateNetworkDatasets,
    };

    enum AnnounceMode : uint8_t // Used in `SendAnnounce()`
    {
        kNormalAnnounce,
        kOrphanAnnounce,
    };

    enum ParentRequestType : uint8_t
    {
        kParentRequestTypeRouters,         // Parent Request to all routers.
        kParentRequestTypeRoutersAndReeds, // Parent Request to all routers and REEDs.
    };

    enum ChildUpdateRequestState : uint8_t
    {
        kChildUpdateRequestNone,    // No pending or active Child Update Request.
        kChildUpdateRequestPending, // Pending Child Update Request due to relative OT_CHANGED event.
        kChildUpdateRequestActive,  // Child Update Request has been sent and Child Update Response is expected.
    };

    enum DataRequestState : uint8_t
    {
        kDataRequestNone,   // Not waiting for a Data Response.
        kDataRequestActive, // Data Request has been sent, Data Response is expected.
    };

    struct DelayedResponseMetadata
    {
        Error AppendTo(Message &aMessage) const { return aMessage.Append(*this); }
        void  ReadFrom(const Message &aMessage);
        void  RemoveFrom(Message &aMessage) const;

        Ip6::Address mDestination; // IPv6 address of the message destination.
        TimeMilli    mSendTime;    // Time when the message shall be sent.
    };

    OT_TOOL_PACKED_BEGIN
    class Header
    {
    public:
        enum SecuritySuite : uint8_t
        {
            k154Security = 0,
            kNoSecurity  = 255,
        };

        void Init(void)
        {
            mSecuritySuite   = k154Security;
            mSecurityControl = Mac::Frame::kSecEncMic32;
        }

        bool IsValid(void) const
        {
            return (mSecuritySuite == kNoSecurity) ||
                   (mSecuritySuite == k154Security &&
                    mSecurityControl == (Mac::Frame::kKeyIdMode2 | Mac::Frame::kSecEncMic32));
        }

        uint8_t GetLength(void) const
        {
            return sizeof(mSecuritySuite) + sizeof(mCommand) +
                   ((mSecuritySuite == k154Security)
                        ? sizeof(mSecurityControl) + sizeof(mFrameCounter) + sizeof(mKeySource) + sizeof(mKeyIndex)
                        : 0);
        }

        SecuritySuite GetSecuritySuite(void) const { return static_cast<SecuritySuite>(mSecuritySuite); }
        void SetSecuritySuite(SecuritySuite aSecuritySuite) { mSecuritySuite = static_cast<uint8_t>(aSecuritySuite); }

        uint8_t GetHeaderLength(void) const
        {
            return sizeof(mSecurityControl) + sizeof(mFrameCounter) + sizeof(mKeySource) + sizeof(mKeyIndex);
        }

        const uint8_t *GetBytes(void) const { return reinterpret_cast<const uint8_t *>(&mSecuritySuite); }
        uint8_t        GetSecurityControl(void) const { return mSecurityControl; }

        bool IsKeyIdMode2(void) const
        {
            return (mSecurityControl & Mac::Frame::kKeyIdModeMask) == Mac::Frame::kKeyIdMode2;
        }

        void SetKeyIdMode2(void)
        {
            mSecurityControl = (mSecurityControl & ~Mac::Frame::kKeyIdModeMask) | Mac::Frame::kKeyIdMode2;
        }

        uint32_t GetKeyId(void) const { return Encoding::BigEndian::HostSwap32(mKeySource); }

        void SetKeyId(uint32_t aKeySequence)
        {
            mKeySource = Encoding::BigEndian::HostSwap32(aKeySequence);
            mKeyIndex  = (aKeySequence & 0x7f) + 1;
        }

        uint32_t GetFrameCounter(void) const { return Encoding::LittleEndian::HostSwap32(mFrameCounter); }
        void     SetFrameCounter(uint32_t aFrameCounter)
        {
            mFrameCounter = Encoding::LittleEndian::HostSwap32(aFrameCounter);
        }

        Command GetCommand(void) const
        {
            return static_cast<Command>((mSecuritySuite == kNoSecurity) ? mSecurityControl : mCommand);
        }

        void SetCommand(Command aCommand)
        {
            if (mSecuritySuite == kNoSecurity)
            {
                mSecurityControl = static_cast<uint8_t>(aCommand);
            }
            else
            {
                mCommand = static_cast<uint8_t>(aCommand);
            }
        }

    private:
        uint8_t  mSecuritySuite;
        uint8_t  mSecurityControl;
        uint32_t mFrameCounter;
        uint32_t mKeySource;
        uint8_t  mKeyIndex;
        uint8_t  mCommand;
    } OT_TOOL_PACKED_END;

    Error       Start(StartMode aMode);
    void        Stop(StopMode aMode);
    void        HandleNotifierEvents(Events aEvents);
    static void HandleAttachTimer(Timer &aTimer);
    void        HandleAttachTimer(void);
    static void HandleDelayedResponseTimer(Timer &aTimer);
    void        HandleDelayedResponseTimer(void);
    static void HandleMessageTransmissionTimer(Timer &aTimer);
    void        HandleMessageTransmissionTimer(void);
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void        ScheduleMessageTransmissionTimer(void);
    Error       ReadChallengeOrResponse(const Message &aMessage, uint8_t aTlvType, Challenge &aBuffer);

    void HandleAdvertisement(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor);
    void HandleChildIdResponse(const Message &         aMessage,
                               const Ip6::MessageInfo &aMessageInfo,
                               const Neighbor *        aNeighbor);
    void HandleChildUpdateRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor);
    void HandleChildUpdateResponse(const Message &         aMessage,
                                   const Ip6::MessageInfo &aMessageInfo,
                                   const Neighbor *        aNeighbor);
    void HandleDataResponse(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, const Neighbor *aNeighbor);
    void HandleParentResponse(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, uint32_t aKeySequence);
    void HandleAnnounce(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    void HandleLinkMetricsManagementRequest(const Message &         aMessage,
                                            const Ip6::MessageInfo &aMessageInfo,
                                            Neighbor *              aNeighbor);
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    void HandleLinkMetricsManagementResponse(const Message &         aMessage,
                                             const Ip6::MessageInfo &aMessageInfo,
                                             Neighbor *              aNeighbor);
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    void HandleLinkProbe(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor);
#endif
    Error HandleLeaderData(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void  ProcessAnnounce(void);
    bool  HasUnregisteredAddress(void);

    uint32_t GetAttachStartDelay(void) const;
    Error    SendParentRequest(ParentRequestType aType);
    Error    SendChildIdRequest(void);
    Error    GetNextAnnouceChannel(uint8_t &aChannel) const;
    bool     HasMoreChannelsToAnnouce(void) const;
    bool     PrepareAnnounceState(void);
    void     SendAnnounce(uint8_t aChannel, AnnounceMode aMode);
    void     SendAnnounce(uint8_t aChannel, const Ip6::Address &aDestination, AnnounceMode aMode = kNormalAnnounce);
    void     RemoveDelayedDataRequestMessage(const Ip6::Address &aDestination);
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    Error SendLinkMetricsManagementResponse(const Ip6::Address &aDestination, LinkMetrics::Status aStatus);
#endif
    uint32_t Reattach(void);

    bool IsBetterParent(uint16_t               aRloc16,
                        uint8_t                aLinkQuality,
                        uint8_t                aLinkMargin,
                        const ConnectivityTlv &aConnectivityTlv,
                        uint8_t                aVersion,
                        uint8_t                aCslClockAccuracy,
                        uint8_t                aCslUncertainty);
    bool IsNetworkDataNewer(const LeaderData &aLeaderData);

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    /**
     * This method scans for network data from the leader and updates IP addresses assigned to this
     * interface to make sure that all Service ALOCs (0xfc10-0xfc1f) are properly set.
     */
    void UpdateServiceAlocs(void);
#endif

#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
    void InformPreviousParent(void);
#endif

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    static void HandleParentSearchTimer(Timer &aTimer);
    void        HandleParentSearchTimer(void);
    void        StartParentSearchTimer(void);
    void        UpdateParentSearchState(void);
#endif

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
    static void        LogError(MessageAction aAction, MessageType aType, Error aError);
    static const char *MessageActionToString(MessageAction aAction);
    static const char *MessageTypeToString(MessageType aType);
    static const char *MessageTypeActionToSuffixString(MessageType aType, MessageAction aAction);
#endif

    MessageQueue mDelayedResponses;

    Challenge mParentRequestChallenge;

    AttachMode mParentRequestMode;
    int8_t     mParentPriority;
    uint8_t    mParentLinkQuality3;
    uint8_t    mParentLinkQuality2;
    uint8_t    mParentLinkQuality1;
    uint16_t   mParentSedBufferSize;
    uint8_t    mParentSedDatagramCount;

    uint8_t                 mChildUpdateAttempts;
    ChildUpdateRequestState mChildUpdateRequestState;
    uint8_t                 mDataRequestAttempts;
    DataRequestState        mDataRequestState;

    AddressRegistrationMode mAddressRegistrationMode;

    bool       mHasRestored;
    uint8_t    mParentLinkMargin;
    bool       mParentIsSingleton;
    bool       mReceivedResponseFromParent;
    LeaderData mParentLeaderData;

    Challenge mParentCandidateChallenge;

    Ip6::Udp::Socket mSocket;
    uint32_t         mTimeout;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint32_t mCslTimeout;
#endif

#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
    uint16_t mPreviousParentRloc;
#endif

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    bool       mParentSearchIsInBackoff : 1;
    bool       mParentSearchBackoffWasCanceled : 1;
    bool       mParentSearchRecentlyDetached : 1;
    TimeMilli  mParentSearchBackoffCancelTime;
    TimerMilli mParentSearchTimer;
#endif

    uint8_t  mAnnounceChannel;
    uint8_t  mAlternateChannel;
    uint16_t mAlternatePanId;
    uint64_t mAlternateTimestamp;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    Ip6::Netif::UnicastAddress mServiceAlocs[kMaxServiceAlocs];
#endif

    otMleCounters mCounters;

    Ip6::Netif::UnicastAddress   mLinkLocal64;
    Ip6::Netif::UnicastAddress   mMeshLocal64;
    Ip6::Netif::UnicastAddress   mMeshLocal16;
    Ip6::Netif::MulticastAddress mLinkLocalAllThreadNodes;
    Ip6::Netif::MulticastAddress mRealmLocalAllThreadNodes;

    otThreadParentResponseCallback mParentResponseCb;
    void *                         mParentResponseCbContext;
};

} // namespace Mle

/**
 * @}
 *
 */

} // namespace ot

#endif // MLE_HPP_
