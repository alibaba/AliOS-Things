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
 *   This file includes definitions for manipulating Thread Network Data managed by the Thread Leader.
 */

#ifndef NETWORK_DATA_LEADER_FTD_HPP_
#define NETWORK_DATA_LEADER_FTD_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include <stdint.h>

#include "coap/coap.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "net/ip6_address.hpp"
#include "thread/mle_router.hpp"
#include "thread/network_data.hpp"

namespace ot {

namespace NetworkData {

/**
 * @addtogroup core-netdata-leader
 *
 * @brief
 *   This module includes definitions for manipulating Thread Network Data managed by the Thread Leader.
 *
 * @{
 *
 */

/**
 * This class implements the Thread Network Data maintained by the Leader.
 *
 */
class Leader : public LeaderBase, private NonCopyable
{
public:
    /**
     * This enumeration defines the match mode constants to compare two RLOC16 values.
     *
     */
    enum MatchMode : uint8_t
    {
        kMatchModeRloc16,   ///< Perform exact RLOC16 match.
        kMatchModeRouterId, ///< Perform Router ID match (match the router and any of its children).
    };

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Leader(Instance &aInstance);

    /**
     * This method reset the Thread Network Data.
     *
     */
    void Reset(void);

    /**
     * This method starts the Leader services.
     *
     */
    void Start(void);

    /**
     * This method stops the Leader services.
     *
     */
    void Stop(void);

    /**
     * This method increments the Thread Network Data version.
     *
     */
    void IncrementVersion(void);

    /**
     * This method increments both the Thread Network Data version and stable version.
     *
     */
    void IncrementVersionAndStableVersion(void);

    /**
     * This method returns CONTEXT_ID_RESUSE_DELAY value.
     *
     * @returns The CONTEXT_ID_REUSE_DELAY value.
     *
     */
    uint32_t GetContextIdReuseDelay(void) const { return mContextIdReuseDelay; }

    /**
     * This method sets CONTEXT_ID_RESUSE_DELAY value.
     *
     * @warning This method should only be used for testing.
     *
     * @param[in]  aDelay  The CONTEXT_ID_REUSE_DELAY value.
     *
     */
    void SetContextIdReuseDelay(uint32_t aDelay) { mContextIdReuseDelay = aDelay; }

    /**
     * This method removes Network Data entries matching with a given RLOC16.
     *
     * @param[in]  aRloc16    A RLOC16 value.
     * @param[in]  aMatchMode A match mode (@sa MatchMode).
     *
     */
    void RemoveBorderRouter(uint16_t aRloc16, MatchMode aMatchMode);

    /**
     * This method synchronizes internal 6LoWPAN Context ID Set with recently obtained Thread Network Data.
     *
     * Note that this method should be called only by the Leader once after reset.
     *
     */
    void UpdateContextsAfterReset(void);

    /**
     * This method scans network data for given Service ID and returns pointer to the respective TLV, if present.
     *
     * @param aServiceId Service ID to look for.
     * @return Pointer to the Service TLV for given Service ID, or nullptr if not present.
     *
     */
    const ServiceTlv *FindServiceById(uint8_t aServiceId) const;

    /**
     * This method sends SVR_DATA.ntf message for any stale child entries that exist in the network data.
     *
     * @param[in]  aHandler  A function pointer that is called when the transaction ends.
     * @param[in]  aContext  A pointer to arbitrary context information.
     *
     * @retval kErrorNone      A stale child entry was found and successfully enqueued a SVR_DATA.ntf message.
     * @retval kErrorNoBufs    A stale child entry was found, but insufficient message buffers were available.
     * @retval kErrorNotFound  No stale child entries were found.
     *
     */
    Error RemoveStaleChildEntries(Coap::ResponseHandler aHandler, void *aContext);

private:
    class ChangedFlags
    {
    public:
        ChangedFlags(void)
            : mChanged(false)
            , mStableChanged(false)
        {
        }

        void Update(const NetworkDataTlv &aTlv)
        {
            mChanged       = true;
            mStableChanged = (mStableChanged || aTlv.IsStable());
        }

        bool DidChange(void) const { return mChanged; }
        bool DidStableChange(void) const { return mStableChanged; }

    private:
        bool mChanged;       // Any (stable or not) network data change (add/remove).
        bool mStableChanged; // Stable network data change (add/remove).
    };

    enum UpdateStatus : uint8_t
    {
        kTlvRemoved, // TLV contained no sub TLVs and therefore is removed.
        kTlvUpdated, // TLV stable flag is updated based on its sub TLVs.
    };

    static void HandleServerData(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleServerData(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    void RegisterNetworkData(uint16_t aRloc16, const NetworkData &aNetworkData);

    Error AddPrefix(const PrefixTlv &aPrefix, ChangedFlags &aChangedFlags);
    Error AddHasRoute(const HasRouteTlv &aHasRoute, PrefixTlv &aDstPrefix, ChangedFlags &aChangedFlags);
    Error AddBorderRouter(const BorderRouterTlv &aBorderRouter, PrefixTlv &aDstPrefix, ChangedFlags &aChangedFlags);
    Error AddService(const ServiceTlv &aService, ChangedFlags &aChangedFlags);
    Error AddServer(const ServerTlv &aServer, ServiceTlv &aDstService, ChangedFlags &aChangedFlags);

    Error AllocateServiceId(uint8_t &aServiceId) const;

    Error AllocateContextId(uint8_t &aContextId);
    void  FreeContextId(uint8_t aContextId);
    void  StartContextReuseTimer(uint8_t aContextId);
    void  StopContextReuseTimer(uint8_t aContextId);

    void RemoveContext(uint8_t aContextId);
    void RemoveContext(PrefixTlv &aPrefix, uint8_t aContextId);

    void RemoveCommissioningData(void);

    void RemoveRloc(uint16_t aRloc16, MatchMode aMatchMode, ChangedFlags &aChangedFlags);
    void RemoveRloc(uint16_t           aRloc16,
                    MatchMode          aMatchMode,
                    const NetworkData &aExcludeNetworkData,
                    ChangedFlags &     aChangedFlags);
    void RemoveRlocInPrefix(PrefixTlv &      aPrefix,
                            uint16_t         aRloc16,
                            MatchMode        aMatchMode,
                            const PrefixTlv *aExcludePrefix,
                            ChangedFlags &   aChangedFlags);
    void RemoveRlocInService(ServiceTlv &      aService,
                             uint16_t          aRloc16,
                             MatchMode         aMatchMode,
                             const ServiceTlv *aExcludeService,
                             ChangedFlags &    aChangedFlags);
    void RemoveRlocInHasRoute(PrefixTlv &      aPrefix,
                              HasRouteTlv &    aHasRoute,
                              uint16_t         aRloc16,
                              MatchMode        aMatchMode,
                              const PrefixTlv *aExcludePrefix,
                              ChangedFlags &   aChangedFlags);
    void RemoveRlocInBorderRouter(PrefixTlv &      aPrefix,
                                  BorderRouterTlv &aBorderRouter,
                                  uint16_t         aRloc16,
                                  MatchMode        aMatchMode,
                                  const PrefixTlv *aExcludePrefix,
                                  ChangedFlags &   aChangedFlags);

    static bool RlocMatch(uint16_t aFirstRloc16, uint16_t aSecondRloc16, MatchMode aMatchMode);

    static Error Validate(const NetworkData &aNetworkData, uint16_t aRloc16);
    static Error ValidatePrefix(const PrefixTlv &aPrefix, uint16_t aRloc16);
    static Error ValidateService(const ServiceTlv &aService, uint16_t aRloc16);

    static bool ContainsMatchingEntry(const PrefixTlv *aPrefix, bool aStable, const HasRouteEntry &aEntry);
    static bool ContainsMatchingEntry(const HasRouteTlv *aHasRoute, const HasRouteEntry &aEntry);
    static bool ContainsMatchingEntry(const PrefixTlv *aPrefix, bool aStable, const BorderRouterEntry &aEntry);
    static bool ContainsMatchingEntry(const BorderRouterTlv *aBorderRouter, const BorderRouterEntry &aEntry);
    static bool ContainsMatchingServer(const ServiceTlv *aService, const ServerTlv &aServer);

    UpdateStatus UpdatePrefix(PrefixTlv &aPrefix);
    UpdateStatus UpdateService(ServiceTlv &aService);
    UpdateStatus UpdateTlv(NetworkDataTlv &aTlv, const NetworkDataTlv *aSubTlvs);

    static void HandleCommissioningSet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleCommissioningSet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    static void HandleCommissioningGet(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleCommissioningGet(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo);

    void SendCommissioningGetResponse(const Coap::Message &   aRequest,
                                      uint16_t                aLength,
                                      const Ip6::MessageInfo &aMessageInfo);
    void SendCommissioningSetResponse(const Coap::Message &    aRequest,
                                      const Ip6::MessageInfo & aMessageInfo,
                                      MeshCoP::StateTlv::State aState);
    void IncrementVersions(bool aIncludeStable);
    void IncrementVersions(const ChangedFlags &aFlags);

    static constexpr uint8_t  kMinContextId        = 1;            // Minimum Context ID (0 is used for Mesh Local)
    static constexpr uint8_t  kNumContextIds       = 15;           // Maximum Context ID
    static constexpr uint32_t kContextIdReuseDelay = 48 * 60 * 60; // in seconds
    static constexpr uint32_t kStateUpdatePeriod   = 60 * 1000;    // State update period in milliseconds

    uint16_t   mContextUsed;
    TimeMilli  mContextLastUsed[kNumContextIds];
    uint32_t   mContextIdReuseDelay;
    TimerMilli mTimer;

    Coap::Resource mServerData;

    Coap::Resource mCommissioningDataGet;
    Coap::Resource mCommissioningDataSet;
};

/**
 * @}
 */

} // namespace NetworkData
} // namespace ot

#endif // OPENTHREAD_FTD

#endif // NETWORK_DATA_LEADER_FTD_HPP_
