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
 *   This file includes definitions for maintaining Thread network topologies.
 */

#ifndef TOPOLOGY_HPP_
#define TOPOLOGY_HPP_

#include "openthread-core-config.h"

#include <openthread/thread_ftd.h>

#include "common/clearable.hpp"
#include "common/equatable.hpp"
#include "common/linked_list.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "common/timer.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6.hpp"
#include "radio/radio.hpp"
#include "radio/trel_link.hpp"
#include "thread/csl_tx_scheduler.hpp"
#include "thread/indirect_sender.hpp"
#include "thread/link_metrics.hpp"
#include "thread/link_quality.hpp"
#include "thread/mle_tlvs.hpp"
#include "thread/mle_types.hpp"
#include "thread/radio_selector.hpp"

namespace ot {

/**
 * This class represents a Thread neighbor.
 *
 */
class Neighbor : public InstanceLocatorInit
#if OPENTHREAD_CONFIG_MULTI_RADIO
    ,
                 public RadioSelector::NeighborInfo
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    ,
                 public Trel::NeighborInfo
#endif
{
public:
    /**
     * Neighbor link states.
     *
     */
    enum State : uint8_t
    {
        kStateInvalid,            ///< Neighbor link is invalid
        kStateRestored,           ///< Neighbor is restored from non-volatile memory
        kStateParentRequest,      ///< Received an MLE Parent Request message
        kStateParentResponse,     ///< Received an MLE Parent Response message
        kStateChildIdRequest,     ///< Received an MLE Child ID Request message
        kStateLinkRequest,        ///< Sent an MLE Link Request message
        kStateChildUpdateRequest, ///< Sent an MLE Child Update Request message (trying to restore the child)
        kStateValid,              ///< Link is valid
    };

    /**
     * This enumeration defines state filters used for finding a neighbor or iterating through the child/neighbor table.
     *
     * Each filter definition accepts a subset of `State` values.
     *
     */
    enum StateFilter : uint8_t
    {
        kInStateValid,                     ///< Accept neighbor only in `kStateValid`.
        kInStateValidOrRestoring,          ///< Accept neighbor with `IsStateValidOrRestoring()` being `true`.
        kInStateChildIdRequest,            ///< Accept neighbor only in `Child:kStateChildIdRequest`.
        kInStateValidOrAttaching,          ///< Accept neighbor with `IsStateValidOrAttaching()` being `true`.
        kInStateInvalid,                   ///< Accept neighbor only in `kStateInvalid`.
        kInStateAnyExceptInvalid,          ///< Accept neighbor in any state except `kStateInvalid`.
        kInStateAnyExceptValidOrRestoring, ///< Accept neighbor in any state except `IsStateValidOrRestoring()`.
        kInStateAny,                       ///< Accept neighbor in any state.
    };

    /**
     * This class represents an Address Matcher used to find a neighbor (child/router) with a given MAC address also
     * matching a given state filter.
     *
     */
    class AddressMatcher
    {
    public:
        /**
         * This constructor initializes the `AddressMatcher` with a given MAC short address (RCOC16) and state filter.
         *
         * @param[in]  aShortAddress   A MAC short address (RLOC16).
         * @param[in]  aStateFilter    A state filter.
         *
         */
        AddressMatcher(Mac::ShortAddress aShortAddress, StateFilter aStateFilter)
            : AddressMatcher(aStateFilter, aShortAddress, nullptr)
        {
        }

        /**
         * This constructor initializes the `AddressMatcher` with a given MAC extended address and state filter.
         *
         * @param[in]  aExtAddress     A MAC extended address.
         * @param[in]  aStateFilter    A state filter.
         *
         */
        AddressMatcher(const Mac::ExtAddress &aExtAddress, StateFilter aStateFilter)
            : AddressMatcher(aStateFilter, Mac::kShortAddrInvalid, &aExtAddress)
        {
        }

        /**
         * This constructor initializes the `AddressMatcher` with a given MAC address and state filter.
         *
         * @param[in]  aMacAddress     A MAC address.
         * @param[in]  aStateFilter    A state filter.
         *
         */
        AddressMatcher(const Mac::Address &aMacAddress, StateFilter aStateFilter)
            : AddressMatcher(aStateFilter,
                             aMacAddress.IsShort() ? aMacAddress.GetShort()
                                                   : static_cast<Mac::ShortAddress>(Mac::kShortAddrInvalid),
                             aMacAddress.IsExtended() ? &aMacAddress.GetExtended() : nullptr)
        {
        }

        /**
         * This constructor initializes the `AddressMatcher` with a given state filter (it accepts any address).
         *
         * @param[in]  aStateFilter    A state filter.
         *
         */
        explicit AddressMatcher(StateFilter aStateFilter)
            : AddressMatcher(aStateFilter, Mac::kShortAddrInvalid, nullptr)
        {
        }

        /**
         * This method indicates if a given neighbor matches the address and state filter of `AddressMatcher`.
         *
         * @param[in] aNeighbor   A neighbor.
         *
         * @retval TRUE   Neighbor @p aNeighbor matches the address and state filter.
         * @retval FALSE  Neighbor @p aNeighbor does not match the address or state filter.
         *
         */
        bool Matches(const Neighbor &aNeighbor) const;

    private:
        AddressMatcher(StateFilter aStateFilter, Mac::ShortAddress aShortAddress, const Mac::ExtAddress *aExtAddress)
            : mStateFilter(aStateFilter)
            , mShortAddress(aShortAddress)
            , mExtAddress(aExtAddress)
        {
        }

        StateFilter            mStateFilter;
        Mac::ShortAddress      mShortAddress;
        const Mac::ExtAddress *mExtAddress;
    };

    /**
     * This type represents diagnostic information for a neighboring node.
     *
     */
    class Info : public otNeighborInfo, public Clearable<Info>
    {
    public:
        /**
         * This method sets the `Info` instance from a given `Neighbor`.
         *
         * @param[in] aNeighbor   A neighbor.
         *
         */
        void SetFrom(const Neighbor &aNeighbor);
    };

    /**
     * This method returns the current state.
     *
     * @returns The current state.
     *
     */
    State GetState(void) const { return static_cast<State>(mState); }

    /**
     * This method sets the current state.
     *
     * @param[in]  aState  The state value.
     *
     */
    void SetState(State aState) { mState = static_cast<uint8_t>(aState); }

    /**
     * This method indicates whether the neighbor is in the Invalid state.
     *
     * @returns TRUE if the neighbor is in the Invalid state, FALSE otherwise.
     *
     */
    bool IsStateInvalid(void) const { return (mState == kStateInvalid); }

    /**
     * This method indicates whether the neighbor is in the Child ID Request state.
     *
     * @returns TRUE if the neighbor is in the Child ID Request state, FALSE otherwise.
     *
     */
    bool IsStateChildIdRequest(void) const { return (mState == kStateChildIdRequest); }

    /**
     * This method indicates whether the neighbor is in the Link Request state.
     *
     * @returns TRUE if the neighbor is in the Link Request state, FALSE otherwise.
     *
     */
    bool IsStateLinkRequest(void) const { return (mState == kStateLinkRequest); }

    /**
     * This method indicates whether the neighbor is in the Parent Response state.
     *
     * @returns TRUE if the neighbor is in the Parent Response state, FALSE otherwise.
     *
     */
    bool IsStateParentResponse(void) const { return (mState == kStateParentResponse); }

    /**
     * This method indicates whether the neighbor is being restored.
     *
     * @returns TRUE if the neighbor is being restored, FALSE otherwise.
     *
     */
    bool IsStateRestoring(void) const { return (mState == kStateRestored) || (mState == kStateChildUpdateRequest); }

    /**
     * This method indicates whether the neighbor is in the Restored state.
     *
     * @returns TRUE if the neighbor is in the Restored state, FALSE otherwise.
     *
     */
    bool IsStateRestored(void) const { return (mState == kStateRestored); }

    /**
     * This method indicates whether the neighbor is valid (frame counters are synchronized).
     *
     * @returns TRUE if the neighbor is valid, FALSE otherwise.
     *
     */
    bool IsStateValid(void) const { return (mState == kStateValid); }

    /**
     * This method indicates whether the neighbor is in valid state or if it is being restored.
     *
     * When in these states messages can be sent to and/or received from the neighbor.
     *
     * @returns TRUE if the neighbor is in valid, restored, or being restored states, FALSE otherwise.
     *
     */
    bool IsStateValidOrRestoring(void) const { return (mState == kStateValid) || IsStateRestoring(); }

    /**
     * This method indicates if the neighbor state is valid, attaching, or restored.
     *
     * The states `kStateRestored`, `kStateChildIdRequest`, `kStateChildUpdateRequest`, `kStateValid`, and
     * `kStateLinkRequest` are considered as valid, attaching, or restored.
     *
     * @returns TRUE if the neighbor state is valid, attaching, or restored, FALSE otherwise.
     *
     */
    bool IsStateValidOrAttaching(void) const;

    /**
     * This method indicates whether neighbor state matches a given state filter.
     *
     * @param[in] aFilter   A state filter (`StateFilter` enumeration) to match against.
     *
     * @returns TRUE if the neighbor state matches the filter, FALSE otherwise.
     *
     */
    bool MatchesFilter(StateFilter aFilter) const;

    /**
     * This method indicates whether neighbor matches a given `AddressMatcher`.
     *
     * @param[in]  aMatcher   An `AddressMatcher` to match against.
     *
     * @returns TRUE if the neighbor matches the address and state filter of @p aMatcher, FALSE otherwise.
     *
     */
    bool Matches(const AddressMatcher &aMatcher) const { return aMatcher.Matches(*this); }

    /**
     * This method gets the device mode flags.
     *
     * @returns The device mode flags.
     *
     */
    Mle::DeviceMode GetDeviceMode(void) const { return Mle::DeviceMode(mMode); }

    /**
     * This method sets the device mode flags.
     *
     * @param[in]  aMode  The device mode flags.
     *
     */
    void SetDeviceMode(Mle::DeviceMode aMode) { mMode = aMode.Get(); }

    /**
     * This method indicates whether or not the device is rx-on-when-idle.
     *
     * @returns TRUE if rx-on-when-idle, FALSE otherwise.
     *
     */
    bool IsRxOnWhenIdle(void) const { return GetDeviceMode().IsRxOnWhenIdle(); }

    /**
     * This method indicates whether or not the device is a Full Thread Device.
     *
     * @returns TRUE if a Full Thread Device, FALSE otherwise.
     *
     */
    bool IsFullThreadDevice(void) const { return GetDeviceMode().IsFullThreadDevice(); }

    /**
     * This method indicates whether or not the device requests Full Network Data.
     *
     * @returns TRUE if requests Full Network Data, FALSE otherwise.
     *
     */
    bool IsFullNetworkData(void) const { return GetDeviceMode().IsFullNetworkData(); }

    /**
     * This method sets all bytes of the Extended Address to zero.
     *
     */
    void ClearExtAddress(void) { memset(&mMacAddr, 0, sizeof(mMacAddr)); }

    /**
     * This method returns the Extended Address.
     *
     * @returns A reference to the Extended Address.
     *
     */
    const Mac::ExtAddress &GetExtAddress(void) const { return mMacAddr; }

    /**
     * This method sets the Extended Address.
     *
     * @param[in]  aAddress  The Extended Address value to set.
     *
     */
    void SetExtAddress(const Mac::ExtAddress &aAddress) { mMacAddr = aAddress; }

    /**
     * This method gets the key sequence value.
     *
     * @returns The key sequence value.
     *
     */
    uint32_t GetKeySequence(void) const { return mKeySequence; }

    /**
     * This method sets the key sequence value.
     *
     * @param[in]  aKeySequence  The key sequence value.
     *
     */
    void SetKeySequence(uint32_t aKeySequence) { mKeySequence = aKeySequence; }

    /**
     * This method returns the last heard time.
     *
     * @returns The last heard time.
     *
     */
    TimeMilli GetLastHeard(void) const { return mLastHeard; }

    /**
     * This method sets the last heard time.
     *
     * @param[in]  aLastHeard  The last heard time.
     *
     */
    void SetLastHeard(TimeMilli aLastHeard) { mLastHeard = aLastHeard; }

    /**
     * This method gets the link frame counters.
     *
     * @returns A reference to `Mac::LinkFrameCounters` containing link frame counter for all supported radio links.
     *
     */
    Mac::LinkFrameCounters &GetLinkFrameCounters(void) { return mValidPending.mValid.mLinkFrameCounters; }

    /**
     * This method gets the link frame counters.
     *
     * @returns A reference to `Mac::LinkFrameCounters` containing link frame counter for all supported radio links.
     *
     */
    const Mac::LinkFrameCounters &GetLinkFrameCounters(void) const { return mValidPending.mValid.mLinkFrameCounters; }

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    /**
     * This method gets the link ACK frame counter value.
     *
     * @returns The link ACK frame counter value.
     *
     */
    uint32_t GetLinkAckFrameCounter(void) const { return mValidPending.mValid.mLinkAckFrameCounter; }
#endif

    /**
     * This method sets the link ACK frame counter value.
     *
     * @param[in]  aAckFrameCounter  The link ACK frame counter value.
     *
     */
    void SetLinkAckFrameCounter(uint32_t aAckFrameCounter)
    {
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        mValidPending.mValid.mLinkAckFrameCounter = aAckFrameCounter;
#else
        OT_UNUSED_VARIABLE(aAckFrameCounter);
#endif
    }

    /**
     * This method gets the MLE frame counter value.
     *
     * @returns The MLE frame counter value.
     *
     */
    uint32_t GetMleFrameCounter(void) const { return mValidPending.mValid.mMleFrameCounter; }

    /**
     * This method sets the MLE frame counter value.
     *
     * @param[in]  aFrameCounter  The MLE frame counter value.
     *
     */
    void SetMleFrameCounter(uint32_t aFrameCounter) { mValidPending.mValid.mMleFrameCounter = aFrameCounter; }

    /**
     * This method gets the RLOC16 value.
     *
     * @returns The RLOC16 value.
     *
     */
    uint16_t GetRloc16(void) const { return mRloc16; }

    /**
     * This method gets the Router ID value.
     *
     * @returns The Router ID value.
     *
     */
    uint8_t GetRouterId(void) const { return mRloc16 >> Mle::kRouterIdOffset; }

    /**
     * This method sets the RLOC16 value.
     *
     * @param[in]  aRloc16  The RLOC16 value.
     *
     */
    void SetRloc16(uint16_t aRloc16) { mRloc16 = aRloc16; }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    /**
     * This method clears the last received fragment tag.
     *
     * The last received fragment tag is used for detect duplicate frames (receievd over different radios) when
     * multi-radio feature is enabled.
     *
     */
    void ClearLastRxFragmentTag(void) { mLastRxFragmentTag = 0; }

    /**
     * This method gets the last received fragment tag.
     *
     * This method MUST be used only when the tag is set (and not cleared). Otherwise its behavior is undefined.
     *
     * @returns The last received fragment tag.
     *
     */
    uint16_t GetLastRxFragmentTag(void) const { return mLastRxFragmentTag; }

    /**
     * This method set the last received fragment tag.
     *
     * @param[in] aTag   The new tag value.
     *
     */
    void SetLastRxFragmentTag(uint16_t aTag) { mLastRxFragmentTag = (aTag == 0) ? 0xffff : aTag; }

    /**
     * This method indicates whether the last received fragment tag is set or not.
     *
     * @returns TRUE if the last received fragment tag is set, FALSE otherwise.
     *
     */
    bool IsLastRxFragmentTagSet(void) const { return (mLastRxFragmentTag != 0); }

    /**
     * This method indicates whether the last received fragment tag is strictly after a given tag value.
     *
     * This method MUST be used only when the tag is set (and not cleared). Otherwise its behavior is undefined.
     *
     * The tag value compassion follows the Serial Number Arithmetic logic from RFC-1982. It is semantically equivalent
     * to `LastRxFragementTag > aTag`.
     *
     * @param[in] aTag   A tag value to compare against.
     *
     * @returns TRUE if the current last rx fragment tag is strictly after @p aTag, FALSE if they are equal or it is
     * before @p aTag.
     *
     */
    bool IsLastRxFragmentTagAfter(uint16_t aTag) const { return ((aTag - mLastRxFragmentTag) & (1U << 15)) != 0; }

#endif // OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method indicates whether or not it is a valid Thread 1.1 neighbor.
     *
     * @returns TRUE if it is a valid Thread 1.1 neighbor, FALSE otherwise.
     *
     */
    bool IsThreadVersion1p1(void) const { return mState != kStateInvalid && mVersion == OT_THREAD_VERSION_1_1; }

    /**
     * This method indicates whether or not it is a valid Thread 1.2 neighbor.
     *
     * @returns TRUE if it is a valid Thread 1.2 neighbor, FALSE otherwise.
     *
     */
    bool IsThreadVersion1p2(void) const { return mState != kStateInvalid && mVersion == OT_THREAD_VERSION_1_2; }

    /**
     * This method indicates whether Enhanced Keep-Alive is supported or not.
     *
     * @returns TRUE if Enhanced Keep-Alive is supported, FALSE otherwise.
     *
     */
    bool IsEnhancedKeepAliveSupported(void) const
    {
        return mState != kStateInvalid && mVersion >= OT_THREAD_VERSION_1_2;
    }

    /**
     * This method gets the device MLE version.
     *
     */
    uint8_t GetVersion(void) const { return mVersion; }

    /**
     * This method sets the device MLE version.
     *
     * @param[in]  aVersion  The device MLE version.
     *
     */
    void SetVersion(uint8_t aVersion) { mVersion = aVersion; }

    /**
     * This method gets the number of consecutive link failures.
     *
     * @returns The number of consecutive link failures.
     *
     */
    uint8_t GetLinkFailures(void) const { return mLinkFailures; }

    /**
     * This method increments the number of consecutive link failures.
     *
     */
    void IncrementLinkFailures(void) { mLinkFailures++; }

    /**
     * This method resets the number of consecutive link failures to zero.
     *
     */
    void ResetLinkFailures(void) { mLinkFailures = 0; }

    /**
     * This method returns the LinkQualityInfo object.
     *
     * @returns The LinkQualityInfo object.
     *
     */
    LinkQualityInfo &GetLinkInfo(void) { return mLinkInfo; }

    /**
     * This method returns the LinkQualityInfo object.
     *
     * @returns The LinkQualityInfo object.
     *
     */
    const LinkQualityInfo &GetLinkInfo(void) const { return mLinkInfo; }

    /**
     * This method generates a new challenge value for MLE Link Request/Response exchanges.
     *
     */
    void GenerateChallenge(void);

    /**
     * This method returns the current challenge value for MLE Link Request/Response exchanges.
     *
     * @returns The current challenge value.
     *
     */
    const uint8_t *GetChallenge(void) const { return mValidPending.mPending.mChallenge; }

    /**
     * This method returns the size (bytes) of the challenge value for MLE Link Request/Response exchanges.
     *
     * @returns The size (bytes) of the challenge value for MLE Link Request/Response exchanges.
     *
     */
    uint8_t GetChallengeSize(void) const { return sizeof(mValidPending.mPending.mChallenge); }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    /**
     * This method indicates whether or not time sync feature is enabled.
     *
     * @returns TRUE if time sync feature is enabled, FALSE otherwise.
     *
     */
    bool IsTimeSyncEnabled(void) const { return mTimeSyncEnabled; }

    /**
     * This method sets whether or not time sync feature is enabled.
     *
     * @param[in]  aEnable    TRUE if time sync feature is enabled, FALSE otherwise.
     *
     */
    void SetTimeSyncEnabled(bool aEnabled) { mTimeSyncEnabled = aEnabled; }
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    /**
     * This method aggregates the Link Metrics data into all the series that is running for this neighbor.
     *
     * If a series wants to account frames of @p aFrameType, it would add count by 1 and aggregate @p aLqi and
     * @p aRss into its averagers.
     *
     * @param[in] aSeriesId     Series ID for Link Probe. Should be `0` if this method is not called by Link Probe.
     * @param[in] aFrameType    Type of the frame that carries Link Metrics data.
     * @param[in] aLqi          The LQI value.
     * @param[in] aRss          The Rss value.
     *
     */
    void AggregateLinkMetrics(uint8_t aSeriesId, uint8_t aFrameType, uint8_t aLqi, int8_t aRss);

    /**
     * This method adds a new LinkMetrics::SeriesInfo to the neighbor's list.
     *
     * @param[in]    A reference to the new SeriesInfo.
     *
     */
    void AddForwardTrackingSeriesInfo(LinkMetrics::SeriesInfo &aSeriesInfo);

    /**
     * This method finds a specific LinkMetrics::SeriesInfo by Series ID.
     *
     * @param[in] aSeriesId    A reference to the Series ID.
     *
     * @returns The pointer to the LinkMetrics::SeriesInfo. `nullptr` if not found.
     *
     */
    LinkMetrics::SeriesInfo *GetForwardTrackingSeriesInfo(const uint8_t &aSeriesId);

    /**
     * This method removes a specific LinkMetrics::SeriesInfo by Series ID.
     *
     * @param[in] aSeriesId    A reference to the Series ID to remove.
     *
     * @returns The pointer to the LinkMetrics::SeriesInfo. `nullptr` if not found.
     *
     */
    LinkMetrics::SeriesInfo *RemoveForwardTrackingSeriesInfo(const uint8_t &aSeriesId);

    /**
     * This method removes all the Series and return the data structures to the Pool.
     *
     */
    void RemoveAllForwardTrackingSeriesInfo(void);

    /**
     * This method gets the Enh-ACK Probing metrics (this `Neighbor` object is the Probing Subject).
     *
     * @returns Enh-ACK Probing metrics configured.
     *
     */
    const LinkMetrics::Metrics &GetEnhAckProbingMetrics(void) const { return mEnhAckProbingMetrics; }

    /**
     * This method sets the Enh-ACK Probing metrics (this `Neighbor` object is the Probing Subject).
     *
     * @param[in]  aEnhAckProbingMetrics  The metrics value to set.
     *
     */
    void SetEnhAckProbingMetrics(const LinkMetrics::Metrics &aEnhAckProbingMetrics)
    {
        mEnhAckProbingMetrics = aEnhAckProbingMetrics;
    }

    /**
     * This method indicates if Enh-ACK Probing is configured and active for this `Neighbor` object.
     *
     * @retval TRUE   Enh-ACK Probing is configured and active for this `Neighbor`.
     * @retval FALSE  Otherwise.
     *
     */
    bool IsEnhAckProbingActive(void) const
    {
        return (mEnhAckProbingMetrics.mLqi != 0) || (mEnhAckProbingMetrics.mLinkMargin != 0) ||
               (mEnhAckProbingMetrics.mRssi != 0);
    }
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

    /**
     * This method converts a given `State` to a human-readable string.
     *
     * @param[in] aState   A neighbor state.
     *
     * @returns A string representation of given state.
     *
     */
    static const char *StateToString(State aState);

protected:
    /**
     * This method initializes the `Neighbor` object.
     *
     * @param[in] aInstance  A reference to OpenThread instance.
     *
     */
    void Init(Instance &aInstance);

private:
    Mac::ExtAddress mMacAddr;   ///< The IEEE 802.15.4 Extended Address
    TimeMilli       mLastHeard; ///< Time when last heard.
    union
    {
        struct
        {
            Mac::LinkFrameCounters mLinkFrameCounters; ///< The Link Frame Counters
            uint32_t               mMleFrameCounter;   ///< The MLE Frame Counter
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
            uint32_t mLinkAckFrameCounter; ///< The Link Ack Frame Counter
#endif
        } mValid;
        struct
        {
            uint8_t mChallenge[Mle::kMaxChallengeSize]; ///< The challenge value
        } mPending;
    } mValidPending;

#if OPENTHREAD_CONFIG_MULTI_RADIO
    uint16_t mLastRxFragmentTag; ///< Last received fragment tag
#endif

    uint32_t mKeySequence; ///< Current key sequence
    uint16_t mRloc16;      ///< The RLOC16
    uint8_t  mState : 4;   ///< The link state
    uint8_t  mMode : 4;    ///< The MLE device mode
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    uint8_t mLinkFailures : 7;    ///< Consecutive link failure count
    bool    mTimeSyncEnabled : 1; ///< Indicates whether or not time sync feature is enabled.
#else
    uint8_t mLinkFailures; ///< Consecutive link failure count
#endif
    uint8_t         mVersion;  ///< The MLE version
    LinkQualityInfo mLinkInfo; ///< Link quality info (contains average RSS, link margin and link quality)
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    // A list of Link Metrics Forward Tracking Series that is being
    // tracked for this neighbor. Note that this device is the
    // Subject and this neighbor is the Initiator.
    LinkedList<LinkMetrics::SeriesInfo> mLinkMetricsSeriesInfoList;

    // Metrics configured for Enh-ACK Based Probing at the Probing
    // Subject (this neighbor). Note that this device is the Initiator
    // and this neighbor is the Subject.
    LinkMetrics::Metrics mEnhAckProbingMetrics;
#endif
};

#if OPENTHREAD_FTD

/**
 * This class represents a Thread Child.
 *
 */
class Child : public Neighbor,
              public IndirectSender::ChildInfo,
              public DataPollHandler::ChildInfo
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    ,
              public CslTxScheduler::ChildInfo
#endif
{
    class AddressIteratorBuilder;

public:
    static constexpr uint8_t kMaxRequestTlvs = 5;

    /**
     * This class represents diagnostic information for a Thread Child.
     *
     */
    class Info : public otChildInfo, public Clearable<Info>
    {
    public:
        /**
         * This method sets the `Info` instance from a given `Child`.
         *
         * @param[in] aChild   A neighbor.
         *
         */
        void SetFrom(const Child &aChild);
    };

    /**
     * This class defines an iterator used to go through IPv6 address entries of a child.
     *
     */
    class AddressIterator : public Unequatable<AddressIterator>
    {
        friend class AddressIteratorBuilder;

    public:
        /**
         * This type represents an index indicating the current IPv6 address entry to which the iterator is pointing.
         *
         */
        typedef otChildIp6AddressIterator Index;

        /**
         * This constructor initializes the iterator associated with a given `Child` starting from beginning of the
         * IPv6 address list.
         *
         * @param[in] aChild    A reference to a child entry.
         * @param[in] aFilter   An IPv6 address type filter restricting iterator to certain type of addresses.
         *
         */
        explicit AddressIterator(const Child &aChild, Ip6::Address::TypeFilter aFilter = Ip6::Address::kTypeAny)
            : AddressIterator(aChild, 0, aFilter)
        {
        }

        /**
         * This constructor initializes the iterator associated with a given `Child` starting from a given index
         *
         * @param[in]  aChild   A reference to the child entry.
         * @param[in]  aIndex   An index (`Index`) with which to initialize the iterator.
         * @param[in]  aFilter  An IPv6 address type filter restricting iterator to certain type of addresses.
         *
         */
        AddressIterator(const Child &aChild, Index aIndex, Ip6::Address::TypeFilter aFilter = Ip6::Address::kTypeAny)
            : mChild(aChild)
            , mFilter(aFilter)
            , mIndex(aIndex)
        {
            Update();
        }

        /**
         * This method converts the iterator into an index.
         *
         * @returns An index corresponding to the iterator.
         *
         */
        Index GetAsIndex(void) const { return mIndex; }

        /**
         * This method gets the iterator's associated `Child` entry.
         *
         * @returns The associated child entry.
         *
         */
        const Child &GetChild(void) const { return mChild; }

        /**
         * This method gets the current `Child` IPv6 Address to which the iterator is pointing.
         *
         * @returns  A pointer to the associated IPv6 Address, or nullptr if iterator is done.
         *
         */
        const Ip6::Address *GetAddress(void) const;

        /**
         * This method indicates whether the iterator has reached end of the list.
         *
         * @retval TRUE   There are no more entries in the list (reached end of the list).
         * @retval FALSE  The current entry is valid.
         *
         */
        bool IsDone(void) const { return (mIndex >= kMaxIndex); }

        /**
         * This method overloads `++` operator (pre-increment) to advance the iterator.
         *
         * The iterator is moved to point to the next `Address` entry.  If there are no more `Ip6::Address` entries
         * `IsDone()` returns `true`.
         *
         */
        void operator++(void) { mIndex++, Update(); }

        /**
         * This method overloads `++` operator (post-increment) to advance the iterator.
         *
         * The iterator is moved to point to the next `Address` entry.  If there are no more `Ip6::Address` entries
         *  `IsDone()` returns `true`.
         *
         */
        void operator++(int) { mIndex++, Update(); }

        /**
         * This method overloads the `*` dereference operator and gets a reference to `Ip6::Address` to which the
         * iterator is currently pointing.
         *
         * This method MUST be used when the iterator is not done (i.e., `IsDone()` returns `false`).
         *
         * @returns A reference to the `Ip6::Address` entry currently pointed by the iterator.
         *
         */
        const Ip6::Address &operator*(void)const { return *GetAddress(); }

        /**
         * This method overloads operator `==` to evaluate whether or not two `Iterator` instances are equal.
         *
         * This method MUST be used when the two iterators are associated with the same `Child` entry.
         *
         * @param[in]  aOther  The other `Iterator` to compare with.
         *
         * @retval TRUE   If the two `Iterator` objects are equal.
         * @retval FALSE  If the two `Iterator` objects are not equal.
         *
         */
        bool operator==(const AddressIterator &aOther) const { return (mIndex == aOther.mIndex); }

    private:
        enum IteratorType : uint8_t
        {
            kEndIterator,
        };

        static constexpr uint16_t kMaxIndex = OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD;

        AddressIterator(const Child &aChild, IteratorType)
            : mChild(aChild)
            , mIndex(kMaxIndex)
        {
        }

        void Update(void);

        const Child &            mChild;
        Ip6::Address::TypeFilter mFilter;
        Index                    mIndex;
        Ip6::Address             mMeshLocalAddress;
    };

    /**
     * This method initializes the `Child` object.
     *
     * @param[in] aInstance  A reference to OpenThread instance.
     *
     */
    void Init(Instance &aInstance) { Neighbor::Init(aInstance); }

    /**
     * This method clears the child entry.
     *
     */
    void Clear(void);

    /**
     * This method clears the IPv6 address list for the child.
     *
     */
    void ClearIp6Addresses(void);

    /**
     * This method sets the device mode flags.
     *
     * @param[in]  aMode  The device mode flags.
     *
     */
    void SetDeviceMode(Mle::DeviceMode aMode);

    /**
     * This method gets the mesh-local IPv6 address.
     *
     * @param[out]   aAddress            A reference to an IPv6 address to provide address (if any).
     *
     * @retval kErrorNone      Successfully found the mesh-local address and updated @p aAddress.
     * @retval kErrorNotFound  No mesh-local IPv6 address in the IPv6 address list.
     *
     */
    Error GetMeshLocalIp6Address(Ip6::Address &aAddress) const;

    /**
     * This method returns the Mesh Local Interface Identifier.
     *
     * @returns The Mesh Local Interface Identifier.
     *
     */
    const Ip6::InterfaceIdentifier &GetMeshLocalIid(void) const { return mMeshLocalIid; }

    /**
     * This method enables range-based `for` loop iteration over all (or a subset of) IPv6 addresses.
     *
     * This method should be used as follows: to iterate over all addresses
     *
     *     for (const Ip6::Address &address : child.IterateIp6Addresses()) { ... }
     *
     * or to iterate over a subset of IPv6 addresses determined by a given address type filter
     *
     *     for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticast)) { ... }
     *
     * @param[in] aFilter  An IPv6 address type filter restricting iteration to certain type of addresses (default is
     *                     to accept any address type).
     *
     * @returns An IteratorBuilder instance.
     *
     */
    AddressIteratorBuilder IterateIp6Addresses(Ip6::Address::TypeFilter aFilter = Ip6::Address::kTypeAny) const
    {
        return AddressIteratorBuilder(*this, aFilter);
    }

    /**
     * This method adds an IPv6 address to the list.
     *
     * @param[in]  aAddress           A reference to IPv6 address to be added.
     *
     * @retval kErrorNone          Successfully added the new address.
     * @retval kErrorAlready       Address is already in the list.
     * @retval kErrorNoBufs        Already at maximum number of addresses. No entry available to add the new address.
     * @retval kErrorInvalidArgs   Address is invalid (it is the Unspecified Address).
     *
     */
    Error AddIp6Address(const Ip6::Address &aAddress);

    /**
     * This method removes an IPv6 address from the list.
     *
     * @param[in]  aAddress               A reference to IPv6 address to be removed.
     *
     * @retval kErrorNone             Successfully removed the address.
     * @retval kErrorNotFound         Address was not found in the list.
     * @retval kErrorInvalidArgs      Address is invalid (it is the Unspecified Address).
     *
     */
    Error RemoveIp6Address(const Ip6::Address &aAddress);

    /**
     * This method indicates whether an IPv6 address is in the list of IPv6 addresses of the child.
     *
     * @param[in]  aAddress   A reference to IPv6 address.
     *
     * @retval TRUE           The address exists on the list.
     * @retval FALSE          Address was not found in the list.
     *
     */
    bool HasIp6Address(const Ip6::Address &aAddress) const;

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE
    /**
     * This method retrieves the Domain Unicast Address registered by the child.
     *
     * @returns A pointer to Domain Unicast Address registered by the child if there is.
     *
     */
    const Ip6::Address *GetDomainUnicastAddress(void) const;
#endif

    /**
     * This method gets the child timeout.
     *
     * @returns The child timeout.
     *
     */
    uint32_t GetTimeout(void) const { return mTimeout; }

    /**
     * This method sets the child timeout.
     *
     * @param[in]  aTimeout  The child timeout.
     *
     */
    void SetTimeout(uint32_t aTimeout) { mTimeout = aTimeout; }

    /**
     * This method gets the network data version.
     *
     * @returns The network data version.
     *
     */
    uint8_t GetNetworkDataVersion(void) const { return mNetworkDataVersion; }

    /**
     * This method sets the network data version.
     *
     * @param[in]  aVersion  The network data version.
     *
     */
    void SetNetworkDataVersion(uint8_t aVersion) { mNetworkDataVersion = aVersion; }

    /**
     * This method generates a new challenge value to use during a child attach.
     *
     */
    void GenerateChallenge(void);

    /**
     * This method gets the current challenge value used during attach.
     *
     * @returns The current challenge value.
     *
     */
    const uint8_t *GetChallenge(void) const { return mAttachChallenge; }

    /**
     * This method gets the challenge size (bytes) used during attach.
     *
     * @returns The challenge size (bytes).
     *
     */
    uint8_t GetChallengeSize(void) const { return sizeof(mAttachChallenge); }

    /**
     * This method clears the requested TLV list.
     *
     */
    void ClearRequestTlvs(void) { memset(mRequestTlvs, Mle::Tlv::kInvalid, sizeof(mRequestTlvs)); }

    /**
     * This method returns the requested TLV at index @p aIndex.
     *
     * @param[in]  aIndex  The index into the requested TLV list.
     *
     * @returns The requested TLV at index @p aIndex.
     *
     */
    uint8_t GetRequestTlv(uint8_t aIndex) const { return mRequestTlvs[aIndex]; }

    /**
     * This method sets the requested TLV at index @p aIndex.
     *
     * @param[in]  aIndex  The index into the requested TLV list.
     * @param[in]  aType   The TLV type.
     *
     */
    void SetRequestTlv(uint8_t aIndex, uint8_t aType) { mRequestTlvs[aIndex] = aType; }

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

    /**
     * This method increments the number of seconds since last supervision of the child.
     *
     */
    void IncrementSecondsSinceLastSupervision(void) { mSecondsSinceSupervision++; }

    /**
     * This method returns the number of seconds since last supervision of the child (last message to the child)
     *
     * @returns Number of seconds since last supervision of the child.
     *
     */
    uint16_t GetSecondsSinceLastSupervision(void) const { return mSecondsSinceSupervision; }

    /**
     * This method resets the number of seconds since last supervision of the child to zero.
     *
     */
    void ResetSecondsSinceLastSupervision(void) { mSecondsSinceSupervision = 0; }

#endif // #if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    /**
     * This method returns MLR state of an IPv6 multicast address.
     *
     * @note The @p aAdddress reference MUST be from `IterateIp6Addresses()` or `AddressIterator`.
     *
     * @param[in] aAddress  The IPv6 multicast address.
     *
     * @returns MLR state of the IPv6 multicast address.
     *
     */
    MlrState GetAddressMlrState(const Ip6::Address &aAddress) const;

    /**
     * This method sets MLR state of an IPv6 multicast address.
     *
     * @note The @p aAdddress reference MUST be from `IterateIp6Addresses()` or `AddressIterator`.
     *
     * @param[in] aAddress  The IPv6 multicast address.
     * @param[in] aState    The target MLR state.
     *
     */
    void SetAddressMlrState(const Ip6::Address &aAddress, MlrState aState);

    /**
     * This method returns if the Child has IPv6 address @p aAddress of MLR state `kMlrStateRegistered`.
     *
     * @param[in] aAddress  The IPv6 address.
     *
     * @retval true   If the Child has IPv6 address @p aAddress of MLR state `kMlrStateRegistered`.
     * @retval false  If the Child does not have IPv6 address @p aAddress of MLR state `kMlrStateRegistered`.
     *
     */
    bool HasMlrRegisteredAddress(const Ip6::Address &aAddress) const;

    /**
     * This method returns if the Child has any IPv6 address of MLR state `kMlrStateRegistered`.
     *
     * @retval true   If the Child has any IPv6 address of MLR state `kMlrStateRegistered`.
     * @retval false  If the Child does not have any IPv6 address of MLR state `kMlrStateRegistered`.
     *
     */
    bool HasAnyMlrRegisteredAddress(void) const { return mMlrRegisteredMask.HasAny(); }

    /**
     * This method returns if the Child has any IPv6 address of MLR state `kMlrStateToRegister`.
     *
     * @retval true   If the Child has any IPv6 address of MLR state `kMlrStateToRegister`.
     * @retval false  If the Child does not have any IPv6 address of MLR state `kMlrStateToRegister`.
     *
     */
    bool HasAnyMlrToRegisterAddress(void) const { return mMlrToRegisterMask.HasAny(); }
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE

private:
#if OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD < 2
#error OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD should be at least set to 2.
#endif

    static constexpr uint16_t kNumIp6Addresses = OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD - 1;

    typedef BitVector<kNumIp6Addresses> ChildIp6AddressMask;

    class AddressIteratorBuilder
    {
    public:
        AddressIteratorBuilder(const Child &aChild, Ip6::Address::TypeFilter aFilter)
            : mChild(aChild)
            , mFilter(aFilter)
        {
        }

        AddressIterator begin(void) { return AddressIterator(mChild, mFilter); }
        AddressIterator end(void) { return AddressIterator(mChild, AddressIterator::kEndIterator); }

    private:
        const Child &            mChild;
        Ip6::Address::TypeFilter mFilter;
    };

    Ip6::InterfaceIdentifier mMeshLocalIid;                 ///< IPv6 address IID for mesh-local address
    Ip6::Address             mIp6Address[kNumIp6Addresses]; ///< Registered IPv6 addresses
    uint32_t                 mTimeout;                      ///< Child timeout

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    ChildIp6AddressMask mMlrToRegisterMask;
    ChildIp6AddressMask mMlrRegisteredMask;
#endif

    uint8_t mNetworkDataVersion; ///< Current Network Data version

    union
    {
        uint8_t mRequestTlvs[kMaxRequestTlvs];            ///< Requested MLE TLVs
        uint8_t mAttachChallenge[Mle::kMaxChallengeSize]; ///< The challenge value
    };

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
    uint16_t mSecondsSinceSupervision; ///< Number of seconds since last supervision of the child.
#endif

    static_assert(OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS < 8192, "mQueuedMessageCount cannot fit max required!");
};

#endif // OPENTHREAD_FTD

/**
 * This class represents a Thread Router
 *
 */
class Router : public Neighbor
{
public:
    /**
     * This class represents diagnostic information for a Thread Router.
     *
     */
    class Info : public otRouterInfo, public Clearable<Info>
    {
    public:
        /**
         * This method sets the `Info` instance from a given `Router`.
         *
         * @param[in] aRouter   A router.
         *
         */
        void SetFrom(const Router &aRouter);
    };

    /**
     * This method initializes the `Router` object.
     *
     * @param[in] aInstance  A reference to OpenThread instance.
     *
     */
    void Init(Instance &aInstance)
    {
        Neighbor::Init(aInstance);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        SetCslClockAccuracy(kCslWorstCrystalPpm);
        SetCslClockUncertainty(kCslWorstUncertainty);
#endif
    }

    /**
     * This method clears the router entry.
     *
     */
    void Clear(void);

    /**
     * This method gets the router ID of the next hop to this router.
     *
     * @returns The router ID of the next hop to this router.
     *
     */
    uint8_t GetNextHop(void) const { return mNextHop; }

    /**
     * This method sets the router ID of the next hop to this router.
     *
     * @param[in]  aRouterId  The router ID of the next hop to this router.
     *
     */
    void SetNextHop(uint8_t aRouterId) { mNextHop = aRouterId; }

    /**
     * This method gets the link quality out value for this router.
     *
     * @returns The link quality out value for this router.
     *
     */
    uint8_t GetLinkQualityOut(void) const { return mLinkQualityOut; }

    /**
     * This method sets the link quality out value for this router.
     *
     * @param[in]  aLinkQuality  The link quality out value for this router.
     *
     */
    void SetLinkQualityOut(uint8_t aLinkQuality) { mLinkQualityOut = aLinkQuality; }

    /**
     * This method get the route cost to this router.
     *
     * @returns The route cost to this router.
     *
     */
    uint8_t GetCost(void) const { return mCost; }

    /**
     * This method sets the router cost to this router.
     *
     * @param[in]  aCost  The router cost to this router.
     *
     */
    void SetCost(uint8_t aCost) { mCost = aCost; }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /**
     * This method get the CSL clock accuracy of this router.
     *
     * @returns The CSL clock accuracy of this router.
     *
     */
    uint8_t GetCslClockAccuracy(void) const { return mCslClockAccuracy; }

    /**
     * This method sets the CSL clock accuracy of this router.
     *
     * @param[in]  aCost  The CSL clock accuracy of this router.
     *
     */
    void SetCslClockAccuracy(uint8_t aCslClockAccuracy) { mCslClockAccuracy = aCslClockAccuracy; }

    /**
     * This method get the CSL clock uncertainty of this router.
     *
     * @returns The CSL clock uncertainty of this router.
     *
     */
    uint8_t GetCslClockUncertainty(void) const { return mCslClockUncertainty; }

    /**
     * This method sets the CSL clock uncertainty of this router.
     *
     * @param[in]  aCost  The CSL clock uncertainty of this router.
     *
     */
    void SetCslClockUncertainty(uint8_t aCslClockUncertainty) { mCslClockUncertainty = aCslClockUncertainty; }
#endif

private:
    uint8_t mNextHop;            ///< The next hop towards this router
    uint8_t mLinkQualityOut : 2; ///< The link quality out for this router

#if OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
    uint8_t mCost; ///< The cost to this router via neighbor router
#else
    uint8_t mCost : 4;     ///< The cost to this router via neighbor router
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint8_t mCslClockAccuracy;    ///< Crystal accuracy, in units of ± ppm.
    uint8_t mCslClockUncertainty; ///< Scheduling uncertainty, in units of 10 us.
#endif
};

} // namespace ot

#endif // TOPOLOGY_HPP_
