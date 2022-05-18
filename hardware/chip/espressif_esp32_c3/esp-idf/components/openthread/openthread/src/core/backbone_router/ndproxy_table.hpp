/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definitions for NdProxy Table on Thread Backbone Border Router.
 */

#ifndef NDPROXY_TABLE_HPP_
#define NDPROXY_TABLE_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE

#include <openthread/backbone_router_ftd.h>

#include "backbone_router/bbr_leader.hpp"
#include "common/iterator_utils.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/time.hpp"
#include "net/ip6_address.hpp"
#include "thread/mle_types.hpp"

namespace ot {

namespace BackboneRouter {

/**
 * This class implements NdProxy Table maintenance on Primary Backbone Router.
 *
 */
class NdProxyTable : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This class represents a ND Proxy instance.
     *
     */
    class NdProxy : private Clearable<NdProxy>
    {
        friend class NdProxyTable;

    public:
        /**
         * This method gets the Mesh-Local IID of the ND Proxy.
         *
         * @returns  The Mesh-Local IID.
         *
         */
        const Ip6::InterfaceIdentifier &GetMeshLocalIid(void) const { return mMeshLocalIid; }

        /**
         * This method gets the time since last transaction of the ND Proxy.
         *
         * @returns  The time since last transaction in seconds.
         *
         */
        uint32_t GetTimeSinceLastTransaction(void) const
        {
            return TimeMilli::MsecToSec(TimerMilli::GetNow() - mLastRegistrationTime);
        }

        /**
         * This method gets the short address of the device who sends the DUA registration.
         *
         * @returns  The RLOC16 value.
         *
         */
        uint16_t GetRloc16(void) const { return mRloc16; }

        /**
         * This method gets the DAD flag of the ND Proxy.
         *
         * @returns  The DAD flag.
         *
         */
        bool GetDadFlag(void) const { return mDadFlag; }

    private:
        NdProxy(void) { Clear(); }

        void Init(const Ip6::InterfaceIdentifier &aAddressIid,
                  const Ip6::InterfaceIdentifier &aMeshLocalIid,
                  uint16_t                        aRloc16,
                  uint32_t                        aTimeSinceLastTransaction);

        void Update(uint16_t aRloc16, uint32_t aTimeSinceLastTransaction);
        void IncreaseDadAttampts(void) { mDadAttempts++; }
        bool IsDadAttamptsComplete() const { return mDadAttempts == Mle::kDuaDadRepeats; }

        Ip6::InterfaceIdentifier mAddressIid;
        Ip6::InterfaceIdentifier mMeshLocalIid;
        TimeMilli                mLastRegistrationTime; ///< in milliseconds
        uint16_t                 mRloc16;
        uint8_t                  mDadAttempts : 2;
        bool                     mDadFlag : 1;
        bool                     mValid : 1;

        static_assert(Mle::kDuaDadRepeats < 4, "Mle::kDuaDadRepeats does not fit in mDadAttempts field as 2-bit value");
    };

    /**
     * This constructor initializes the `NdProxyTable` object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit NdProxyTable(Instance &aInstance)
        : InstanceLocator(aInstance)
        , mCallback(nullptr)
        , mCallbackContext(nullptr)
        , mIsAnyDadInProcess(false)
    {
    }

    /**
     * This method registers a given IPv6 address IID with related information to the NdProxy table.
     *
     * @param[in] aAddressIid                The IPv6 address IID.
     * @param[in] aMeshLocalIid              The Mesh-Local IID.
     * @param[in] aRloc16                    The RLOC16.
     * @param[in] aTimeSinceLastTransaction  Time since last transaction (in seconds).
     *
     * @retval kErrorNone        If registered successfully.
     * @retval kErrorDuplicated  If the IPv6 address IID is a duplicate.
     * @retval kErrorNoBufs      Insufficient buffer space available to register.
     *
     */
    Error Register(const Ip6::InterfaceIdentifier &aAddressIid,
                   const Ip6::InterfaceIdentifier &aMeshLocalIid,
                   uint16_t                        aRloc16,
                   const uint32_t *                aTimeSinceLastTransaction);

    /**
     * This method checks if a given IPv6 address IID was registered.
     *
     * @param[in] aAddressIid  The IPv6 address IID.
     *
     * @retval TRUE   If the IPv6 address IID was registered.
     * @retval FALSE  If the IPv6 address IID was not registered.
     *
     */
    bool IsRegistered(const Ip6::InterfaceIdentifier &aAddressIid) { return FindByAddressIid(aAddressIid) != nullptr; }

    /**
     * This method notifies Domain Prefix status.
     *
     * @param[in]  aState  The Domain Prefix state or state change.
     *
     */
    void HandleDomainPrefixUpdate(Leader::DomainPrefixState aState);

    /**
     * This method notifies ND Proxy table of the timer tick.
     *
     */
    void HandleTimer(void);

    /**
     * This method gets the ND Proxy info for a given Domain Unicast Address.
     *
     * @param[in] aDua  The Domain Unicast Address.
     *
     * @returns The `NdProxy` instance matching the specified @p aDua, or nullptr if not found.
     *
     */
    NdProxy *ResolveDua(const Ip6::Address &aDua);

    /**
     * This method notifies DAD completed for a given ND Proxy.
     *
     * @param[in] aNdProxy      The ND Proxy to notify of.
     * @param[in] aDuplicated   Whether duplicate was detected.
     *
     */
    static void NotifyDadComplete(NdProxy &aNdProxy, bool aDuplicated);

    /**
     * This method removes the ND Proxy.
     *
     * @param[in] aNdProxy      The ND Proxy to remove.
     *
     */
    static void Erase(NdProxy &aNdProxy);

    /*
     * This method sets the ND Proxy callback.
     *
     * @param[in] aCallback  The callback function.
     * @param[in] aContext   A user context pointer.
     *
     */
    void SetCallback(otBackboneRouterNdProxyCallback aCallback, void *aContext);

    /**
     * This method retrieves the ND Proxy info of the Domain Unicast Address.
     *
     * @param[in] aDua          The Domain Unicast Address to get info.
     * @param[in] aNdProxyInfo  A pointer to the ND Proxy info.
     *
     * @retval kErrorNone       Successfully retrieve the ND Proxy info.
     * @retval kErrorNotFound   Failed to find the Domain Unicast Address in the ND Proxy table.
     *
     */
    Error GetInfo(const Ip6::Address &aDua, otBackboneRouterNdProxyInfo &aNdProxyInfo);

private:
    static constexpr uint16_t kMaxNdProxyNum = OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM;

    enum Filter : uint8_t
    {
        kFilterInvalid,
        kFilterValid,
        kFilterDadInProcess,
    };

    /**
     * This class represents an iterator for iterating through the NdProxy Table.
     *
     */
    class Iterator : public InstanceLocator, public ItemPtrIterator<NdProxy, Iterator>
    {
        friend class ItemPtrIterator<NdProxy, Iterator>;
        friend class NdProxyTable;
        friend class IteratorBuilder;

    private:
        enum IteratorType : uint8_t
        {
            kEndIterator,
        };

        Iterator(Instance &aInstance, Filter aFilter);
        Iterator(Instance &aInstance, IteratorType);

        void Advance(void);

        Filter mFilter;
    };

    class IteratorBuilder : public InstanceLocator
    {
        friend class NdProxyTable;

    private:
        IteratorBuilder(Instance &aInstance, Filter aFilter)
            : InstanceLocator(aInstance)
            , mFilter(aFilter)
        {
        }

        Iterator begin(void) { return Iterator(GetInstance(), mFilter); }
        Iterator end(void) { return Iterator(GetInstance(), Iterator::kEndIterator); }

        Filter mFilter;
    };

    IteratorBuilder Iterate(Filter aFilter) { return IteratorBuilder(GetInstance(), aFilter); }
    void            Clear(void);
    static bool     MatchesFilter(const NdProxy &aProxy, Filter aFilter);
    NdProxy *       FindByAddressIid(const Ip6::InterfaceIdentifier &aAddressIid);
    NdProxy *       FindByMeshLocalIid(const Ip6::InterfaceIdentifier &aMeshLocalIid);
    NdProxy *       FindInvalid(void);
    Ip6::Address    GetDua(NdProxy &aNdProxy);
    void            NotifyDuaRegistrationOnBackboneLink(NdProxy &aNdProxy, bool aIsRenew);
    void TriggerCallback(otBackboneRouterNdProxyEvent aEvent, const Ip6::InterfaceIdentifier &aAddressIid) const;

    NdProxy                         mProxies[kMaxNdProxyNum];
    otBackboneRouterNdProxyCallback mCallback;
    void *                          mCallbackContext;
    bool                            mIsAnyDadInProcess : 1;
};

} // namespace BackboneRouter

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE

#endif // NDPROXY_TABLE_HPP_
