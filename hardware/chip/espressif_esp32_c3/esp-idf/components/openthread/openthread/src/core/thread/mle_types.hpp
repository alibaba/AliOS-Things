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
 *   This file includes definitions for MLE types and constants.
 */

#ifndef MLE_TYPES_HPP_
#define MLE_TYPES_HPP_

#include "openthread-core-config.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include <openthread/thread.h>

#include "common/clearable.hpp"
#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/equatable.hpp"
#include "common/string.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6_address.hpp"

namespace ot {
namespace Mle {

/**
 * @addtogroup core-mle-core
 *
 * @brief
 *   This module includes definition for MLE types and constants.
 *
 * @{
 *
 */

constexpr uint16_t kMaxChildren               = OPENTHREAD_CONFIG_MLE_MAX_CHILDREN;
constexpr uint8_t  kMaxChildKeepAliveAttempts = 4; ///< Max keep alive attempts before reattach to a new Parent.
constexpr uint8_t  kFailedChildTransmissions  = OPENTHREAD_CONFIG_FAILED_CHILD_TRANSMISSIONS;

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
// Extra one for core Backbone Router Service.
constexpr uint8_t kMaxServiceAlocs = OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_MAX_ALOCS + 1;
#else
constexpr uint8_t  kMaxServiceAlocs      = OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_MAX_ALOCS;
#endif

constexpr uint8_t  kThreadVersion = OPENTHREAD_CONFIG_THREAD_VERSION; ///< Thread Version
constexpr uint16_t kUdpPort       = 19788;                            ///< MLE UDP Port

/*
 * MLE Protocol delays and timeouts.
 *
 */
constexpr uint32_t kParentRequestRouterTimeout     = 750;  ///< Router Parent Request timeout (in msec)
constexpr uint32_t kParentRequestDuplicateMargin   = 50;   ///< Margin for duplicate parent request
constexpr uint32_t kParentRequestReedTimeout       = 1250; ///< Router and REEDs Parent Request timeout (in msec)
constexpr uint32_t kAttachStartJitter              = 50;   ///< Max jitter time added to start of attach (in msec)
constexpr uint32_t kAnnounceProcessTimeout         = 250;  ///< Delay after Announce rx before channel/pan-id change
constexpr uint32_t kAnnounceTimeout                = 1400; ///< Total timeout for sending Announce messages (in msec)
constexpr uint32_t kMinAnnounceDelay               = 80;   ///< Min delay between Announcement messages (in msec)
constexpr uint32_t kParentResponseMaxDelayRouters  = 500;  ///< Max response delay for Parent Req to routers (in msec)
constexpr uint32_t kParentResponseMaxDelayAll      = 1000; ///< Max response delay for Parent Req to all (in msec)
constexpr uint32_t kUnicastRetransmissionDelay     = 1000; ///< Base delay before an MLE unicast retx (in msec)
constexpr uint32_t kChildUpdateRequestPendingDelay = 100;  ///< Delay for aggregating Child Update Req (in msec)
constexpr uint8_t  kMaxTransmissionCount           = 3;    ///< Max number of times an MLE message may be transmitted
constexpr uint32_t kMaxResponseDelay               = 1000; ///< Max response delay for a multicast request (in msec)
constexpr uint32_t kMaxChildIdRequestTimeout       = 5000; ///< Max delay to rx a Child ID Request (in msec)
constexpr uint32_t kMaxChildUpdateResponseTimeout  = 2000; ///< Max delay to rx a Child Update Response (in msec)
constexpr uint32_t kMaxLinkRequestTimeout          = 2000; ///< Max delay to rx a Link Accept

constexpr uint32_t kMinTimeoutKeepAlive = (((kMaxChildKeepAliveAttempts + 1) * kUnicastRetransmissionDelay) / 1000);
constexpr uint32_t kMinPollPeriod       = OPENTHREAD_CONFIG_MAC_MINIMUM_POLL_PERIOD;
constexpr uint32_t kRetxPollPeriod      = OPENTHREAD_CONFIG_MAC_RETX_POLL_PERIOD;
constexpr uint32_t kMinTimeoutDataPoll  = (kMinPollPeriod + kFailedChildTransmissions * kRetxPollPeriod) / 1000;
constexpr uint32_t kMinTimeout          = OT_MAX(kMinTimeoutKeepAlive, kMinTimeoutDataPoll); ///< Min timeout (in sec)

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
constexpr uint8_t kLinkAcceptMaxRouters = 3; ///< Max Route TLV entries in a Link Accept message
#else
constexpr uint8_t  kLinkAcceptMaxRouters = 20; ///< Max Route TLV entries in a Link Accept message
#endif
constexpr uint8_t kLinkAcceptSequenceRollback = 64; ///< Route Sequence value rollback in a Link Accept message.

constexpr uint16_t kMinChildId = 1;   ///< Minimum Child ID
constexpr uint16_t kMaxChildId = 511; ///< Maximum Child ID

constexpr uint8_t kRouterIdOffset   = 10; ///< Bit offset of Router ID in RLOC16
constexpr uint8_t kRlocPrefixLength = 14; ///< Prefix length of RLOC in bytes

constexpr uint8_t kMinChallengeSize = 4; ///< Minimum Challenge size in bytes.
constexpr uint8_t kMaxChallengeSize = 8; ///< Maximum Challenge size in bytes.

/*
 * Routing Protocol Constants
 *
 */
constexpr uint32_t kAdvertiseIntervalMin = 1; ///< Min Advertise interval (in sec)
#if OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
constexpr uint32_t kAdvertiseIntervalMax = 5; ///< Max Advertise interval (in sec)
#else
constexpr uint32_t kAdvertiseIntervalMax = 32; ///< Max Advertise interval (in sec)
#endif

constexpr uint8_t kFailedRouterTransmissions = 4;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
constexpr uint8_t kFailedCslDataPollTransmissions = 15;
#endif

constexpr uint8_t  kRouterIdReuseDelay     = 100; ///< (in sec)
constexpr uint32_t kRouterIdSequencePeriod = 10;  ///< (in sec)
constexpr uint32_t kMaxNeighborAge         = 100; ///< (in sec)

#if OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
constexpr uint8_t kMaxRouteCost = 127;
#else
constexpr uint8_t  kMaxRouteCost         = 16;
#endif

constexpr uint8_t kMaxRouterId           = OT_NETWORK_MAX_ROUTER_ID; ///< Max Router ID
constexpr uint8_t kInvalidRouterId       = kMaxRouterId + 1;         ///< Value indicating incorrect Router ID
constexpr uint8_t kMaxRouters            = OPENTHREAD_CONFIG_MLE_MAX_ROUTERS;
constexpr uint8_t kMinDowngradeNeighbors = 7;

constexpr uint8_t kNetworkIdTimeout           = 120; ///< (in sec)
constexpr uint8_t kParentRouteToLeaderTimeout = 20;  ///< (in sec)
constexpr uint8_t kRouterSelectionJitter      = 120; ///< (in sec)

constexpr uint8_t kRouterDowngradeThreshold = 23;
constexpr uint8_t kRouterUpgradeThreshold   = 16;

constexpr uint32_t kMaxLeaderToRouterTimeout = 90;  ///< (in sec)
constexpr uint32_t kReedAdvertiseInterval    = 570; ///< (in sec)
constexpr uint32_t kReedAdvertiseJitter      = 60;  ///< (in sec)

constexpr uint8_t  kLeaderWeight             = 64;                                          ///< Default leader weight
constexpr uint32_t kMleEndDeviceTimeout      = OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT; ///< (in sec)
constexpr uint8_t  kMeshLocalPrefixContextId = 0; ///< 0 is reserved for Mesh Local Prefix

constexpr int8_t kParentPriorityHigh        = 1;  ///< Parent Priority High
constexpr int8_t kParentPriorityMedium      = 0;  ///< Parent Priority Medium (default)
constexpr int8_t kParentPriorityLow         = -1; ///< Parent Priority Low
constexpr int8_t kParentPriorityUnspecified = -2; ///< Parent Priority Unspecified

constexpr uint8_t kLinkQuality3LinkCost = 1;             ///< Link Cost for Link Quality 3
constexpr uint8_t kLinkQuality2LinkCost = 2;             ///< Link Cost for Link Quality 2
constexpr uint8_t kLinkQuality1LinkCost = 4;             ///< Link Cost for Link Quality 1
constexpr uint8_t kLinkQuality0LinkCost = kMaxRouteCost; ///< Link Cost for Link Quality 0

constexpr uint8_t kMplChildDataMessageTimerExpirations  = 0; ///< Number of MPL retransmissions for Children.
constexpr uint8_t kMplRouterDataMessageTimerExpirations = 2; ///< Number of MPL retransmissions for Routers.

/**
 * This type represents a Thread device role.
 *
 */
enum DeviceRole : uint8_t
{
    kRoleDisabled = OT_DEVICE_ROLE_DISABLED, ///< The Thread stack is disabled.
    kRoleDetached = OT_DEVICE_ROLE_DETACHED, ///< Not currently participating in a Thread network/partition.
    kRoleChild    = OT_DEVICE_ROLE_CHILD,    ///< The Thread Child role.
    kRoleRouter   = OT_DEVICE_ROLE_ROUTER,   ///< The Thread Router role.
    kRoleLeader   = OT_DEVICE_ROLE_LEADER,   ///< The Thread Leader role.
};

/**
 * MLE Attach modes
 *
 */
enum AttachMode : uint8_t
{
    kAttachAny           = 0, ///< Attach to any Thread partition.
    kAttachSame1         = 1, ///< Attach to the same Thread partition (attempt 1 when losing connectivity).
    kAttachSame2         = 2, ///< Attach to the same Thread partition (attempt 2 when losing connectivity).
    kAttachBetter        = 3, ///< Attach to a better (i.e. higher weight/partition id) Thread partition.
    kAttachSameDowngrade = 4, ///< Attach to the same Thread partition during downgrade process.
};

constexpr uint16_t kAloc16Leader                      = 0xfc00;
constexpr uint16_t kAloc16DhcpAgentStart              = 0xfc01;
constexpr uint16_t kAloc16DhcpAgentEnd                = 0xfc0f;
constexpr uint16_t kAloc16ServiceStart                = 0xfc10;
constexpr uint16_t kAloc16ServiceEnd                  = 0xfc2f;
constexpr uint16_t kAloc16CommissionerStart           = 0xfc30;
constexpr uint16_t kAloc16CommissionerEnd             = 0xfc37;
constexpr uint16_t kAloc16BackboneRouterPrimary       = 0xfc38;
constexpr uint16_t kAloc16CommissionerMask            = 0x0007;
constexpr uint16_t kAloc16NeighborDiscoveryAgentStart = 0xfc40;
constexpr uint16_t kAloc16NeighborDiscoveryAgentEnd   = 0xfc4e;

constexpr uint8_t kServiceMinId = 0x00; ///< Minimal Service ID.
constexpr uint8_t kServiceMaxId = 0x0f; ///< Maximal Service ID.

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/*
 * Backbone Router / DUA / MLR constants
 *
 */
constexpr uint16_t kRegistrationDelayDefault         = 1200;              ///< In seconds.
constexpr uint32_t kMlrTimeoutDefault                = 3600;              ///< In seconds.
constexpr uint32_t kMlrTimeoutMin                    = 300;               ///< In seconds.
constexpr uint32_t kMlrTimeoutMax                    = 0x7fffffff / 1000; ///< In seconds (about 24 days).
constexpr uint8_t  kBackboneRouterRegistrationJitter = 5;                 ///< In seconds.
constexpr uint8_t  kParentAggregateDelay             = 5;                 ///< In seconds.
constexpr uint8_t  kNoBufDelay                       = 5;                 ///< In seconds.
constexpr uint8_t  kImmediateReRegisterDelay         = 1;                 ///< In seconds.
constexpr uint8_t  KResponseTimeoutDelay             = 30;                ///< In seconds.

/**
 * Time period after which the address becomes "Preferred" if no duplicate address error (in seconds).
 *
 */
constexpr uint32_t kDuaDadPeriod = 100;

/**
 * Maximum number of times the multicast DAD query and wait time DUA_DAD_QUERY_TIMEOUT are repeated by the BBR, as
 * part of the DAD process.
 *
 */
constexpr uint8_t kDuaDadRepeats = 3;

/**
 * Time period (in seconds) during which a DUA registration is considered 'recent' at a BBR.
 *
 */
constexpr uint32_t kDuaRecentTime               = 20;
constexpr uint32_t kTimeSinceLastTransactionMax = 10 * 86400; ///< In seconds (10 days).
constexpr uint8_t  kDefaultBackboneHoplimit     = 1;          ///< default hoplimit for Backbone Link Protocol messages

static_assert(kMlrTimeoutDefault >= kMlrTimeoutMin && kMlrTimeoutDefault <= kMlrTimeoutMax,
              "kMlrTimeoutDefault must be larger than or equal to kMlrTimeoutMin");

static_assert(Mle::kParentAggregateDelay > 1, "kParentAggregateDelay should be larger than 1 second");
static_assert(kMlrTimeoutMax * 1000 > kMlrTimeoutMax, "SecToMsec(kMlrTimeoutMax) will overflow");

static_assert(kTimeSinceLastTransactionMax * 1000 > kTimeSinceLastTransactionMax,
              "SecToMsec(kTimeSinceLastTransactionMax) will overflow");

/**
 * State change of Child's DUA
 *
 */
enum class ChildDuaState : uint8_t
{
    kAdded,     ///< A new DUA registered by the Child via Address Registration.
    kChanged,   ///< A different DUA registered by the Child via Address Registration.
    kRemoved,   ///< DUA registered by the Child is removed and not in Address Registration.
    kUnchanged, ///< The Child registers the same DUA again.
};

#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/**
 * This type represents a MLE device mode.
 *
 */
class DeviceMode : public Equatable<DeviceMode>
{
public:
    static constexpr uint8_t kModeRxOnWhenIdle     = 1 << 3; ///< If to keep receiver on when not transmitting.
    static constexpr uint8_t kModeReserved         = 1 << 2; ///< Set on transmission, ignore on reception.
    static constexpr uint8_t kModeFullThreadDevice = 1 << 1; ///< If the device is an FTD.
    static constexpr uint8_t kModeFullNetworkData  = 1 << 0; ///< If the device requires the full Network Data.

    static constexpr uint16_t kInfoStringSize = 45; ///< String buffer size used for `ToString()`.

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     *  This structure represents an MLE Mode configuration.
     *
     */
    typedef otLinkModeConfig ModeConfig;

    /**
     * This is the default constructor for `DeviceMode` object.
     *
     */
    DeviceMode(void) = default;

    /**
     * This constructor initializes a `DeviceMode` object from a given mode TLV bitmask.
     *
     * @param[in] aMode   A mode TLV bitmask to initialize the `DeviceMode` object.
     *
     */
    explicit DeviceMode(uint8_t aMode) { Set(aMode); }

    /**
     * This constructor initializes a `DeviceMode` object from a given mode configuration structure.
     *
     * @param[in] aModeConfig   A mode configuration to initialize the `DeviceMode` object.
     *
     */
    explicit DeviceMode(ModeConfig aModeConfig) { Set(aModeConfig); }

    /**
     * This method gets the device mode as a mode TLV bitmask.
     *
     * @returns The device mode as a mode TLV bitmask.
     *
     */
    uint8_t Get(void) const { return mMode; }

    /**
     * This method sets the device mode from a given mode TLV bitmask.
     *
     * @param[in] aMode   A mode TLV bitmask.
     *
     */
    void Set(uint8_t aMode) { mMode = aMode | kModeReserved; }

    /**
     * This method gets the device mode as a mode configuration structure.
     *
     * @param[out] aModeConfig   A reference to a mode configuration structure to output the device mode.
     *
     */
    void Get(ModeConfig &aModeConfig) const;

    /**
     * this method sets the device mode from a given mode configuration structure.
     *
     * @param[in] aModeConfig   A mode configuration structure.
     *
     */
    void Set(const ModeConfig &aModeConfig);

    /**
     * This method indicates whether or not the device is rx-on-when-idle.
     *
     * @retval TRUE   If the device is rx-on-when-idle (non-sleepy).
     * @retval FALSE  If the device is not rx-on-when-idle (sleepy).
     *
     */
    bool IsRxOnWhenIdle(void) const { return (mMode & kModeRxOnWhenIdle) != 0; }

    /**
     * This method indicates whether or not the device is a Full Thread Device.
     *
     * @retval TRUE   If the device is Full Thread Device.
     * @retval FALSE  If the device if not Full Thread Device.
     *
     */
    bool IsFullThreadDevice(void) const { return (mMode & kModeFullThreadDevice) != 0; }

    /**
     * This method indicates whether or not the device requests Full Network Data.
     *
     * @retval TRUE   If the device requests Full Network Data.
     * @retval FALSE  If the device does not request Full Network Data (only stable Network Data).
     *
     */
    bool IsFullNetworkData(void) const { return (mMode & kModeFullNetworkData) != 0; }

    /**
     * This method indicates whether or not the device is a Minimal End Device.
     *
     * @retval TRUE   If the device is a Minimal End Device.
     * @retval FALSE  If the device is not a Minimal End Device.
     *
     */
    bool IsMinimalEndDevice(void) const
    {
        return (mMode & (kModeFullThreadDevice | kModeRxOnWhenIdle)) != (kModeFullThreadDevice | kModeRxOnWhenIdle);
    }

    /**
     * This method indicates whether or not the device mode flags are valid.
     *
     * An FTD which is not rx-on-when-idle (is sleepy) is considered invalid.
     *
     * @returns TRUE if , FALSE otherwise.
     * @retval TRUE   If the device mode flags are valid.
     * @retval FALSE  If the device mode flags are not valid.
     *
     */
    bool IsValid(void) const { return !IsFullThreadDevice() || IsRxOnWhenIdle(); }

    /**
     * This method converts the device mode into a human-readable string.
     *
     * @returns An `InfoString` object representing the device mode.
     *
     */
    InfoString ToString(void) const;

private:
    uint8_t mMode;
};

/**
 * This class represents a Mesh Local Prefix.
 *
 */
OT_TOOL_PACKED_BEGIN
class MeshLocalPrefix : public Ip6::NetworkPrefix
{
public:
    /**
     * This method derives and sets the Mesh Local Prefix from an Extended PAN ID.
     *
     * @param[in] aExtendedPanId   An Extended PAN ID.
     *
     */
    void SetFromExtendedPanId(const Mac::ExtendedPanId &aExtendedPanId);

} OT_TOOL_PACKED_END;

/**
 * This class represents the Thread Leader Data.
 *
 */
class LeaderData : public otLeaderData, public Clearable<LeaderData>
{
public:
    /**
     * This method returns the Partition ID value.
     *
     * @returns The Partition ID value.
     *
     */
    uint32_t GetPartitionId(void) const { return mPartitionId; }

    /**
     * This method sets the Partition ID value.
     *
     * @param[in]  aPartitionId  The Partition ID value.
     *
     */
    void SetPartitionId(uint32_t aPartitionId) { mPartitionId = aPartitionId; }

    /**
     * This method returns the Weighting value.
     *
     * @returns The Weighting value.
     *
     */
    uint8_t GetWeighting(void) const { return mWeighting; }

    /**
     * This method sets the Weighting value.
     *
     * @param[in]  aWeighting  The Weighting value.
     *
     */
    void SetWeighting(uint8_t aWeighting) { mWeighting = aWeighting; }

    /**
     * This method returns the Data Version value.
     *
     * @returns The Data Version value.
     *
     */
    uint8_t GetDataVersion(void) const { return mDataVersion; }

    /**
     * This method sets the Data Version value.
     *
     * @param[in]  aVersion  The Data Version value.
     *
     */
    void SetDataVersion(uint8_t aVersion) { mDataVersion = aVersion; }

    /**
     * This method returns the Stable Data Version value.
     *
     * @returns The Stable Data Version value.
     *
     */
    uint8_t GetStableDataVersion(void) const { return mStableDataVersion; }

    /**
     * This method sets the Stable Data Version value.
     *
     * @param[in]  aVersion  The Stable Data Version value.
     *
     */
    void SetStableDataVersion(uint8_t aVersion) { mStableDataVersion = aVersion; }

    /**
     * This method returns the Leader Router ID value.
     *
     * @returns The Leader Router ID value.
     *
     */
    uint8_t GetLeaderRouterId(void) const { return mLeaderRouterId; }

    /**
     * This method sets the Leader Router ID value.
     *
     * @param[in]  aRouterId  The Leader Router ID value.
     *
     */
    void SetLeaderRouterId(uint8_t aRouterId) { mLeaderRouterId = aRouterId; }
};

OT_TOOL_PACKED_BEGIN
class RouterIdSet : public Equatable<RouterIdSet>
{
public:
    /**
     * This method clears the Router Id Set.
     *
     */
    void Clear(void) { memset(mRouterIdSet, 0, sizeof(mRouterIdSet)); }

    /**
     * This method indicates whether or not a Router ID bit is set.
     *
     * @param[in]  aRouterId  The Router ID.
     *
     * @retval TRUE   If the Router ID bit is set.
     * @retval FALSE  If the Router ID bit is not set.
     *
     */
    bool Contains(uint8_t aRouterId) const { return (mRouterIdSet[aRouterId / 8] & (0x80 >> (aRouterId % 8))) != 0; }

    /**
     * This method sets a given Router ID.
     *
     * @param[in]  aRouterId  The Router ID to set.
     *
     */
    void Add(uint8_t aRouterId) { mRouterIdSet[aRouterId / 8] |= 0x80 >> (aRouterId % 8); }

    /**
     * This method removes a given Router ID.
     *
     * @param[in]  aRouterId  The Router ID to remove.
     *
     */
    void Remove(uint8_t aRouterId) { mRouterIdSet[aRouterId / 8] &= ~(0x80 >> (aRouterId % 8)); }

private:
    uint8_t mRouterIdSet[BitVectorBytes(Mle::kMaxRouterId + 1)];
} OT_TOOL_PACKED_END;

/**
 * This class represents a MLE Key Material
 *
 */
typedef Mac::KeyMaterial KeyMaterial;

/**
 * This class represents a MLE Key.
 *
 */
typedef Mac::Key Key;

/**
 * @}
 *
 */

} // namespace Mle
} // namespace ot

#endif // MLE_TYPES_HPP_
