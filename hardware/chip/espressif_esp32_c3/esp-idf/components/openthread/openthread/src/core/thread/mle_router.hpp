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
 *   This file includes definitions for MLE functionality required by the Thread Router and Leader roles.
 */

#ifndef MLE_ROUTER_HPP_
#define MLE_ROUTER_HPP_

#include "openthread-core-config.h"

#include <openthread/thread_ftd.h>

#include "coap/coap.hpp"
#include "coap/coap_message.hpp"
#include "common/time_ticker.hpp"
#include "common/timer.hpp"
#include "common/trickle_timer.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "net/icmp6.hpp"
#include "net/udp6.hpp"
#include "thread/child_table.hpp"
#include "thread/mle.hpp"
#include "thread/mle_tlvs.hpp"
#include "thread/router_table.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/topology.hpp"

namespace ot {
namespace Mle {

/**
 * @addtogroup core-mle-router
 *
 * @brief
 *   This module includes definitions for MLE functionality required by the Thread Router and Leader roles.
 *
 * @{
 */

#if OPENTHREAD_FTD

/**
 * This class implements MLE functionality required by the Thread Router and Leader roles.
 *
 */
class MleRouter : public Mle
{
    friend class Mle;
    friend class ot::Instance;
    friend class ot::TimeTicker;

public:
    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit MleRouter(Instance &aInstance);

    /**
     * This method indicates whether or not the device is router-eligible.
     *
     * @retval true   If device is router-eligible.
     * @retval false  If device is not router-eligible.
     *
     */
    bool IsRouterEligible(void) const;

    /**
     * This method sets whether or not the device is router-eligible.
     *
     * If @p aEligible is false and the device is currently operating as a router, this call will cause the device to
     * detach and attempt to reattach as a child.
     *
     * @param[in]  aEligible  TRUE to configure device router-eligible, FALSE otherwise.
     *
     * @retval kErrorNone         Successfully set the router-eligible configuration.
     * @retval kErrorNotCapable   The device is not capable of becoming a router.
     *
     */
    Error SetRouterEligible(bool aEligible);

    /**
     * This method indicates whether a node is the only router on the network.
     *
     * @retval TRUE   It is the only router in the network.
     * @retval FALSE  It is a child or is not a single router in the network.
     *
     */
    bool IsSingleton(void);

    /**
     * This method generates an Address Solicit request for a Router ID.
     *
     * @param[in]  aStatus  The reason for requesting a Router ID.
     *
     * @retval kErrorNone           Successfully generated an Address Solicit message.
     * @retval kErrorNotCapable     Device is not capable of becoming a router
     * @retval kErrorInvalidState   Thread is not enabled
     *
     */
    Error BecomeRouter(ThreadStatusTlv::Status aStatus);

    /**
     * This method causes the Thread interface to become a Leader and start a new partition.
     *
     * @retval kErrorNone           Successfully become a Leader and started a new partition.
     * @retval kErrorNotCapable     Device is not capable of becoming a leader
     * @retval kErrorInvalidState   Thread is not enabled
     *
     */
    Error BecomeLeader(void);

    /**
     * This method returns the Leader Weighting value for this Thread interface.
     *
     * @returns The Leader Weighting value for this Thread interface.
     *
     */
    uint8_t GetLeaderWeight(void) const { return mLeaderWeight; }

    /**
     * This method sets the Leader Weighting value for this Thread interface.
     *
     * @param[in]  aWeight  The Leader Weighting value.
     *
     */
    void SetLeaderWeight(uint8_t aWeight) { mLeaderWeight = aWeight; }

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE

    /**
     * This method returns the preferred Partition Id when operating in the Leader role for certification testing.
     *
     * @returns The preferred Partition Id value.
     *
     */
    uint32_t GetPreferredLeaderPartitionId(void) const { return mPreferredLeaderPartitionId; }

    /**
     * This method sets the preferred Partition Id when operating in the Leader role for certification testing.
     *
     * @param[in]  aPartitionId  The preferred Leader Partition Id.
     *
     */
    void SetPreferredLeaderPartitionId(uint32_t aPartitionId) { mPreferredLeaderPartitionId = aPartitionId; }
#endif

    /**
     * This method sets the preferred Router Id. Upon becoming a router/leader the node
     * attempts to use this Router Id. If the preferred Router Id is not set or if it
     * can not be used, a randomly generated router Id is picked.
     * This property can be set when he device role is detached or disabled.
     *
     * @param[in]  aRouterId             The preferred Router Id.
     *
     * @retval kErrorNone          Successfully set the preferred Router Id.
     * @retval kErrorInvalidState  Could not set (role is other than detached and disabled)
     *
     */
    Error SetPreferredRouterId(uint8_t aRouterId);

    /**
     * This method gets the Partition Id which the device joined successfully once.
     *
     */
    uint32_t GetPreviousPartitionId(void) const { return mPreviousPartitionId; }

    /**
     * This method sets the Partition Id which the device joins successfully.
     *
     * @param[in]  aPartitionId   The Partition Id.
     *
     */
    void SetPreviousPartitionId(uint32_t aPartitionId) { mPreviousPartitionId = aPartitionId; }

    /**
     * This method sets the Router Id.
     *
     * @param[in]  aRouterId   The Router Id.
     *
     */
    void SetRouterId(uint8_t aRouterId);

    /**
     * This method returns the next hop towards an RLOC16 destination.
     *
     * @param[in]  aDestination  The RLOC16 of the destination.
     *
     * @returns A RLOC16 of the next hop if a route is known, kInvalidRloc16 otherwise.
     *
     */
    uint16_t GetNextHop(uint16_t aDestination);

    /**
     * This method returns the NETWORK_ID_TIMEOUT value.
     *
     * @returns The NETWORK_ID_TIMEOUT value.
     *
     */
    uint8_t GetNetworkIdTimeout(void) const { return mNetworkIdTimeout; }

    /**
     * This method sets the NETWORK_ID_TIMEOUT value.
     *
     * @param[in]  aTimeout  The NETWORK_ID_TIMEOUT value.
     *
     */
    void SetNetworkIdTimeout(uint8_t aTimeout) { mNetworkIdTimeout = aTimeout; }

    /**
     * This method returns the route cost to a RLOC16.
     *
     * @param[in]  aRloc16  The RLOC16 of the destination.
     *
     * @returns The route cost to a RLOC16.
     *
     */
    uint8_t GetRouteCost(uint16_t aRloc16) const;

    /**
     * This method returns the link cost to the given Router.
     *
     * @param[in]  aRouterId  The Router ID.
     *
     * @returns The link cost to the Router.
     *
     */
    uint8_t GetLinkCost(uint8_t aRouterId);

    /**
     * This method returns the minimum cost to the given router.
     *
     * @param[in]  aRloc16  The short address of the given router.
     *
     * @returns The minimum cost to the given router (via direct link or forwarding).
     *
     */
    uint8_t GetCost(uint16_t aRloc16);

    /**
     * This method returns the ROUTER_SELECTION_JITTER value.
     *
     * @returns The ROUTER_SELECTION_JITTER value.
     *
     */
    uint8_t GetRouterSelectionJitter(void) const { return mRouterSelectionJitter; }

    /**
     * This method sets the ROUTER_SELECTION_JITTER value.
     *
     * @returns The ROUTER_SELECTION_JITTER value.
     *
     */
    Error SetRouterSelectionJitter(uint8_t aRouterJitter);

    /**
     * This method returns the current router selection jitter timeout value.
     *
     * @returns The current router selection jitter timeout value.
     *
     */
    uint8_t GetRouterSelectionJitterTimeout(void) const { return mRouterSelectionJitterTimeout; }

    /**
     * This method returns the ROUTER_UPGRADE_THRESHOLD value.
     *
     * @returns The ROUTER_UPGRADE_THRESHOLD value.
     *
     */
    uint8_t GetRouterUpgradeThreshold(void) const { return mRouterUpgradeThreshold; }

    /**
     * This method sets the ROUTER_UPGRADE_THRESHOLD value.
     *
     * @returns The ROUTER_UPGRADE_THRESHOLD value.
     *
     */
    void SetRouterUpgradeThreshold(uint8_t aThreshold) { mRouterUpgradeThreshold = aThreshold; }

    /**
     * This method returns the ROUTER_DOWNGRADE_THRESHOLD value.
     *
     * @returns The ROUTER_DOWNGRADE_THRESHOLD value.
     *
     */
    uint8_t GetRouterDowngradeThreshold(void) const { return mRouterDowngradeThreshold; }

    /**
     * This method sets the ROUTER_DOWNGRADE_THRESHOLD value.
     *
     * @returns The ROUTER_DOWNGRADE_THRESHOLD value.
     *
     */
    void SetRouterDowngradeThreshold(uint8_t aThreshold) { mRouterDowngradeThreshold = aThreshold; }

    /**
     * This method returns if the REED is expected to become Router soon.
     *
     * @retval TRUE   If the REED is going to become a Router soon.
     * @retval FALSE  If the REED is not going to become a Router soon.
     *
     */
    bool IsExpectedToBecomeRouterSoon(void) const;

    /**
     * This method removes a link to a neighbor.
     *
     * @param[in]  aNeighbor  A reference to the neighbor object.
     *
     */
    void RemoveNeighbor(Neighbor &aNeighbor);

    /**
     * This method invalidates a direct link to a neighboring router (due to failed link-layer acks).
     *
     * @param[in]  aRouter  A reference to the router object.
     *
     */
    void RemoveRouterLink(Router &aRouter);

    /**
     * This method indicates whether or not the RLOC16 is an MTD child of this device.
     *
     * @param[in]  aRloc16  The RLOC16.
     *
     * @retval TRUE if @p aRloc16 is an MTD child of this device.
     * @retval FALSE if @p aRloc16 is not an MTD child of this device.
     *
     */
    bool IsMinimalChild(uint16_t aRloc16);

    /**
     * This method indicates whether or not the given Thread partition attributes are preferred.
     *
     * @param[in]  aSingletonA   Whether or not the Thread Partition A has a single router.
     * @param[in]  aLeaderDataA  A reference to Thread Partition A's Leader Data.
     * @param[in]  aSingletonB   Whether or not the Thread Partition B has a single router.
     * @param[in]  aLeaderDataB  A reference to Thread Partition B's Leader Data.
     *
     * @retval 1   If partition A is preferred.
     * @retval 0   If partition A and B have equal preference.
     * @retval -1  If partition B is preferred.
     *
     */
    static int ComparePartitions(bool              aSingletonA,
                                 const LeaderData &aLeaderDataA,
                                 bool              aSingletonB,
                                 const LeaderData &aLeaderDataB);

    /**
     * This method checks if the destination is reachable.
     *
     * @param[in]  aMeshDest   The RLOC16 of the destination.
     * @param[in]  aIp6Header  A reference to the IPv6 header of the message.
     *
     * @retval kErrorNone      The destination is reachable.
     * @retval kErrorNoRoute   The destination is not reachable and the message should be dropped.
     *
     */
    Error CheckReachability(uint16_t aMeshDest, Ip6::Header &aIp6Header);

    /**
     * This method resolves 2-hop routing loops.
     *
     * @param[in]  aSourceMac   The RLOC16 of the previous hop.
     * @param[in]  aDestRloc16  The RLOC16 of the final destination.
     *
     */
    void ResolveRoutingLoops(uint16_t aSourceMac, uint16_t aDestRloc16);

    /**
     * This method checks if a given Router ID has correct value.
     *
     * @param[in]  aRouterId  The Router ID value.
     *
     * @retval TRUE   If @p aRouterId is in correct range [0..62].
     * @retval FALSE  If @p aRouterId is not a valid Router ID.
     *
     */
    static bool IsRouterIdValid(uint8_t aRouterId) { return aRouterId <= kMaxRouterId; }

    /**
     * This method fills an ConnectivityTlv.
     *
     * @param[out]  aTlv  A reference to the tlv to be filled.
     *
     */
    void FillConnectivityTlv(ConnectivityTlv &aTlv);

    /**
     * This method fills an RouteTlv.
     *
     * @param[out]  aTlv  A reference to the tlv to be filled.
     *
     */
    void FillRouteTlv(RouteTlv &aTlv, Neighbor *aNeighbor = nullptr);

    /**
     * This method generates an MLE Child Update Request message to be sent to the parent.
     *
     * @retval kErrorNone     Successfully generated an MLE Child Update Request message.
     * @retval kErrorNoBufs   Insufficient buffers to generate the MLE Child Update Request message.
     *
     */
    Error SendChildUpdateRequest(void) { return Mle::SendChildUpdateRequest(); }

    Error SendLinkRequest(Neighbor *aNeighbor);

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
    /**
     * This method sets steering data out of band
     *
     * @param[in]  aExtAddress  Value used to set steering data
     *                          All zeros clears steering data
     *                          All 0xFFs sets steering data to 0xFF
     *                          Anything else is used to compute the bloom filter
     *
     */
    void SetSteeringData(const Mac::ExtAddress *aExtAddress);
#endif

    /**
     * This method gets the assigned parent priority.
     *
     * @returns The assigned parent priority value, -2 means not assigned.
     *
     */
    int8_t GetAssignParentPriority(void) const { return mParentPriority; }

    /**
     * This method sets the parent priority.
     *
     * @param[in]  aParentPriority  The parent priority value.
     *
     * @retval kErrorNone           Successfully set the parent priority.
     * @retval kErrorInvalidArgs    If the parent priority value is not among 1, 0, -1 and -2.
     *
     */
    Error SetAssignParentPriority(int8_t aParentPriority);

    /**
     * This method gets the longest MLE Timeout TLV for all active MTD children.
     *
     * @param[out]  aTimeout  A reference to where the information is placed.
     *
     * @retval kErrorNone           Successfully get the max child timeout
     * @retval kErrorInvalidState   Not an active router
     * @retval kErrorNotFound       NO MTD child
     *
     */
    Error GetMaxChildTimeout(uint32_t &aTimeout) const;

    /**
     * This function sets the callback that is called when processing an MLE Discovery Request message.
     *
     * @param[in]  aCallback A pointer to a function that is called to deliver MLE Discovery Request data.
     * @param[in]  aContext  A pointer to application-specific context.
     *
     */
    void SetDiscoveryRequestCallback(otThreadDiscoveryRequestCallback aCallback, void *aContext)
    {
        mDiscoveryRequestCallback        = aCallback;
        mDiscoveryRequestCallbackContext = aContext;
    }

    /**
     * This method resets the MLE Advertisement Trickle timer interval.
     *
     */
    void ResetAdvertiseInterval(void);

    /**
     * This static method converts link quality to route cost.
     *
     * @param[in]  aLinkQuality  The link quality.
     *
     * @returns The link cost corresponding to @p aLinkQuality.
     *
     */
    static uint8_t LinkQualityToCost(uint8_t aLinkQuality);

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    /**
     * This method generates an MLE Time Synchronization message.
     *
     * @retval kErrorNone     Successfully sent an MLE Time Synchronization message.
     * @retval kErrorNoBufs   Insufficient buffers to generate the MLE Time Synchronization message.
     *
     */
    Error SendTimeSync(void);
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    /**
     * This method sets the delay before registering Backbone Router service.
     *
     * @param[in]  aDelay  The delay before registering Backbone Router service.
     *
     */
    void SetBackboneRouterRegistrationDelay(uint8_t aDelay) { mBackboneRouterRegistrationDelay = aDelay; }
#endif

    /**
     * This method gets the maximum number of IP addresses that each MTD child may register with this device as parent.
     *
     * @returns The maximum number of IP addresses that each MTD child may register with this device as parent.
     *
     */
    uint8_t GetMaxChildIpAddresses(void) const;

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    /**
     * This method sets/restores the maximum number of IP addresses that each MTD child may register with this
     * device as parent.
     *
     * @param[in]  aMaxIpAddresses  The maximum number of IP addresses that each MTD child may register with this
     *                              device as parent. 0 to clear the setting and restore the default.
     *
     * @retval kErrorNone           Successfully set/cleared the number.
     * @retval kErrorInvalidArgs    If exceeds the allowed maximum number.
     *
     */
    Error SetMaxChildIpAddresses(uint8_t aMaxIpAddresses);
#endif

private:
    static constexpr uint16_t kDiscoveryMaxJitter            = 250;  // Max jitter delay Discovery Responses (in msec).
    static constexpr uint32_t kStateUpdatePeriod             = 1000; // State update period (in msec).
    static constexpr uint16_t kUnsolicitedDataResponseJitter = 500;  // Max delay for unsol Data Response (in msec).

    Error AppendConnectivity(Message &aMessage);
    Error AppendChildAddresses(Message &aMessage, Child &aChild);
    Error AppendRoute(Message &aMessage, Neighbor *aNeighbor = nullptr);
    Error AppendActiveDataset(Message &aMessage);
    Error AppendPendingDataset(Message &aMessage);
    void  HandleDetachStart(void);
    void  HandleChildStart(AttachMode aMode);
    void  HandleLinkRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor);
    void  HandleLinkAccept(const Message &         aMessage,
                           const Ip6::MessageInfo &aMessageInfo,
                           uint32_t                aKeySequence,
                           Neighbor *              aNeighbor);
    Error HandleLinkAccept(const Message &         aMessage,
                           const Ip6::MessageInfo &aMessageInfo,
                           uint32_t                aKeySequence,
                           Neighbor *              aNeighbor,
                           bool                    aRequest);
    void  HandleLinkAcceptAndRequest(const Message &         aMessage,
                                     const Ip6::MessageInfo &aMessageInfo,
                                     uint32_t                aKeySequence,
                                     Neighbor *              aNeighbor);
    Error HandleAdvertisement(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *);
    void  HandleParentRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void  HandleChildIdRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, uint32_t aKeySequence);
    void  HandleChildUpdateRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void  HandleChildUpdateResponse(const Message &         aMessage,
                                    const Ip6::MessageInfo &aMessageInfo,
                                    uint32_t                aKeySequence,
                                    Neighbor *              aNeighbor);
    void  HandleDataRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, const Neighbor *aNeighbor);
    void  HandleNetworkDataUpdateRouter(void);
    void  HandleDiscoveryRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    void HandleTimeSync(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, const Neighbor *aNeighbor);
#endif

    Error ProcessRouteTlv(const RouteTlv &aRoute);
    void  StopAdvertiseTrickleTimer(void);
    Error SendAddressSolicit(ThreadStatusTlv::Status aStatus);
    void  SendAddressRelease(void);
    void  SendAddressSolicitResponse(const Coap::Message &   aRequest,
                                     const Router *          aRouter,
                                     const Ip6::MessageInfo &aMessageInfo);
    void  SendAdvertisement(void);
    Error SendLinkAccept(const Ip6::MessageInfo &aMessageInfo,
                         Neighbor *              aNeighbor,
                         const RequestedTlvs &   aRequestedTlvs,
                         const Challenge &       aChallenge);
    void  SendParentResponse(Child *aChild, const Challenge &aChallenge, bool aRoutersOnlyRequest);
    Error SendChildIdResponse(Child &aChild);
    Error SendChildUpdateRequest(Child &aChild);
    void  SendChildUpdateResponse(Child *                 aChild,
                                  const Ip6::MessageInfo &aMessageInfo,
                                  const uint8_t *         aTlvs,
                                  uint8_t                 aTlvsLength,
                                  const Challenge &       aChallenge);
    void  SendDataResponse(const Ip6::Address &aDestination,
                           const uint8_t *     aTlvs,
                           uint8_t             aTlvsLength,
                           uint16_t            aDelay,
                           const Message *     aRequestMessage = nullptr);
    Error SendDiscoveryResponse(const Ip6::Address &aDestination, const Message &aDiscoverRequestMessage);
    void  SetStateRouter(uint16_t aRloc16);
    void  SetStateLeader(uint16_t aRloc16);
    void  StopLeader(void);
    void  SynchronizeChildNetworkData(void);
    Error UpdateChildAddresses(const Message &aMessage, uint16_t aOffset, Child &aChild);
    void  UpdateRoutes(const RouteTlv &aRoute, uint8_t aRouterId);
    bool  UpdateLinkQualityOut(const RouteTlv &aRoute, Router &aNeighbor, bool &aResetAdvInterval);

    static void HandleAddressSolicitResponse(void *               aContext,
                                             otMessage *          aMessage,
                                             const otMessageInfo *aMessageInfo,
                                             Error                aResult);
    void HandleAddressSolicitResponse(Coap::Message *aMessage, const Ip6::MessageInfo *aMessageInfo, Error aResult);
    static void HandleAddressRelease(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleAddressRelease(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    static void HandleAddressSolicit(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleAddressSolicit(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static bool IsSingleton(const RouteTlv &aRouteTlv);

    void HandlePartitionChange(void);

    void SetChildStateToValid(Child &aChild);
    bool HasChildren(void);
    void RemoveChildren(void);
    bool HasMinDowngradeNeighborRouters(void);
    bool HasOneNeighborWithComparableConnectivity(const RouteTlv &aRoute, uint8_t aRouterId);
    bool HasSmallNumberOfChildren(void);

    static void HandleAdvertiseTrickleTimer(TrickleTimer &aTimer);
    void        HandleAdvertiseTrickleTimer(void);
    void        HandleTimeTick(void);

    TrickleTimer mAdvertiseTrickleTimer;

    Coap::Resource mAddressSolicit;
    Coap::Resource mAddressRelease;

    ChildTable  mChildTable;
    RouterTable mRouterTable;

    uint8_t   mChallengeTimeout;
    Challenge mChallenge;

    uint16_t mNextChildId;
    uint8_t  mNetworkIdTimeout;
    uint8_t  mRouterUpgradeThreshold;
    uint8_t  mRouterDowngradeThreshold;
    uint8_t  mLeaderWeight;
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    uint32_t mPreferredLeaderPartitionId; ///< only for certification testing
#endif
    bool mRouterEligible : 1;
    bool mAddressSolicitPending : 1;
    bool mAddressSolicitRejected : 1;

    uint8_t mRouterId;
    uint8_t mPreviousRouterId;

    uint32_t mPreviousPartitionIdRouter;         ///< The partition ID when last operating as a router
    uint32_t mPreviousPartitionId;               ///< The partition ID when last attached
    uint8_t  mPreviousPartitionRouterIdSequence; ///< The router ID sequence when last attached
    uint8_t  mPreviousPartitionIdTimeout;        ///< The partition ID timeout when last attached

    uint8_t mRouterSelectionJitter;        ///< The variable to save the assigned jitter value.
    uint8_t mRouterSelectionJitterTimeout; ///< The Timeout prior to request/release Router ID.

    int8_t mParentPriority; ///< The assigned parent priority value, -2 means not assigned.
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    uint8_t mBackboneRouterRegistrationDelay; ///< Delay before registering Backbone Router service.
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    uint8_t mMaxChildIpAddresses;
#endif

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
    MeshCoP::SteeringData mSteeringData;
#endif

    otThreadDiscoveryRequestCallback mDiscoveryRequestCallback;
    void *                           mDiscoveryRequestCallbackContext;
};

#endif // OPENTHREAD_FTD

#if OPENTHREAD_MTD

class MleRouter : public Mle
{
    friend class Mle;
    friend class ot::Instance;

public:
    explicit MleRouter(Instance &aInstance)
        : Mle(aInstance)
    {
    }

    bool IsSingleton(void) const { return false; }

    uint16_t GetNextHop(uint16_t aDestination) const { return Mle::GetNextHop(aDestination); }

    uint8_t GetCost(uint16_t) { return 0; }

    Error RemoveNeighbor(Neighbor &) { return BecomeDetached(); }
    void  RemoveRouterLink(Router &) { IgnoreError(BecomeDetached()); }

    static bool IsRouterIdValid(uint8_t aRouterId) { return aRouterId <= kMaxRouterId; }

    Error SendChildUpdateRequest(void) { return Mle::SendChildUpdateRequest(); }

    Error CheckReachability(uint16_t aMeshDest, Ip6::Header &aIp6Header)
    {
        return Mle::CheckReachability(aMeshDest, aIp6Header);
    }
};

#endif // OPENTHREAD_MTD

} // namespace Mle

/**
 * @}
 */

} // namespace ot

#endif // MLE_ROUTER_HPP_
