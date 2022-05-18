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
 *   This file includes definitions for IPv6 network interfaces.
 */

#ifndef NET_NETIF_HPP_
#define NET_NETIF_HPP_

#include "openthread-core-config.h"

#include "common/clearable.hpp"
#include "common/code_utils.hpp"
#include "common/const_cast.hpp"
#include "common/iterator_utils.hpp"
#include "common/linked_list.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/tasklet.hpp"
#include "mac/mac_types.hpp"
#include "net/ip6_address.hpp"
#include "net/socket.hpp"
#include "thread/mlr_types.hpp"

namespace ot {
namespace Ip6 {

class Ip6;

/**
 * @addtogroup core-ip6-netif
 *
 * @brief
 *   This module includes definitions for IPv6 network interfaces.
 *
 * @{
 *
 */

/**
 * This class implements an IPv6 network interface.
 *
 */
class Netif : public InstanceLocator, private NonCopyable
{
    friend class Ip6;
    friend class Address;

public:
    /**
     * This enumeration represent an address event (added or removed)
     *
     * The boolean values are used for `aIsAdded` parameter in the call of `otIp6AddressCallback`.
     *
     */
    enum AddressEvent : bool
    {
        kAddressRemoved = false, ///< Indicates that address was added.
        kAddressAdded   = true,  ///< Indicates that address was removed.
    };

    /**
     * This enumeration represents the address origin.
     *
     */
    enum AddressOrigin : uint8_t
    {
        kOriginThread = OT_ADDRESS_ORIGIN_THREAD, ///< Thread assigned address (ALOC, RLOC, MLEID, etc)
        kOriginSlaac  = OT_ADDRESS_ORIGIN_SLAAC,  ///< SLAAC assigned address
        kOriginDhcp6  = OT_ADDRESS_ORIGIN_DHCPV6, ///< DHCPv6 assigned address
        kOriginManual = OT_ADDRESS_ORIGIN_MANUAL, ///< Manually assigned address
    };

    /**
     * This class implements an IPv6 network interface unicast address.
     *
     */
    class UnicastAddress : public otNetifAddress,
                           public LinkedListEntry<UnicastAddress>,
                           public Clearable<UnicastAddress>
    {
        friend class LinkedList<UnicastAddress>;

    public:
        /**
         * This method clears and initializes the unicast address as a preferred, valid, thread-origin address with
         * 64-bit prefix length.
         *
         * @param[in]   aPreferred  Whether to initialize as a preferred address.
         *
         */
        void InitAsThreadOrigin(bool aPreferred = false);

        /**
         * This method clears and initializes the unicast address as a valid (but not preferred), thread-origin,
         * realm-local scope (overridden) address with 64-bit prefix length.
         *
         */
        void InitAsThreadOriginRealmLocalScope(void);

        /**
         * This method clears and initializes the unicast address as a valid (but not preferred), thread-origin, global
         * scope address.
         *
         */
        void InitAsThreadOriginGlobalScope(void);

        /**
         * This method clears and initializes the unicast address as a valid, SLAAC-origin address with a given
         * preferred flag and a given prefix length.
         *
         * @param[in] aPrefixLength    The prefix length (in bits).
         * @param[in] aPreferred       The preferred flag.
         *
         */
        void InitAsSlaacOrigin(uint8_t aPrefixLength, bool aPreferred);

        /**
         * This method returns the unicast address.
         *
         * @returns The unicast address.
         *
         */
        const Address &GetAddress(void) const { return static_cast<const Address &>(mAddress); }

        /**
         * This method returns the unicast address.
         *
         * @returns The unicast address.
         *
         */
        Address &GetAddress(void) { return static_cast<Address &>(mAddress); }

        /**
         * This method returns the address's prefix length (in bits).
         *
         * @returns The prefix length (in bits).
         *
         */
        uint8_t GetPrefixLength(void) const { return mPrefixLength; }

        /**
         * This method indicates whether the address has a given prefix (i.e. same prefix length and matches the
         * prefix).
         *
         * @param[in] aPrefix   A prefix to check against.
         *
         * @retval TRUE  The address has and fully matches the @p aPrefix.
         * @retval FALSE The address does not contain or match the @p aPrefix.
         *
         */
        bool HasPrefix(const Prefix &aPrefix) const
        {
            return (mPrefixLength == aPrefix.GetLength()) && GetAddress().MatchesPrefix(aPrefix);
        }

        /**
         * This method returns the IPv6 scope value.
         *
         * @returns The IPv6 scope value.
         *
         */
        uint8_t GetScope(void) const
        {
            return mScopeOverrideValid ? static_cast<uint8_t>(mScopeOverride) : GetAddress().GetScope();
        }

        /**
         * This method sets the IPv6 scope override value.
         *
         * @param[in]  aScope  The IPv6 scope value.
         *
         */
        void SetScopeOverride(uint8_t aScope)
        {
            mScopeOverride      = aScope;
            mScopeOverrideValid = true;
        }

        /**
         * This method gets the IPv6 address origin.
         *
         * @returns The address origin.
         *
         */
        AddressOrigin GetOrigin(void) const { return static_cast<AddressOrigin>(mAddressOrigin); }

    private:
        bool Matches(const Address &aAddress) const { return GetAddress() == aAddress; }
    };

    /**
     * This class implements an IPv6 network interface multicast address.
     *
     */
    class MulticastAddress : public otNetifMulticastAddress,
                             public LinkedListEntry<MulticastAddress>,
                             public Clearable<MulticastAddress>
    {
        friend class LinkedList<MulticastAddress>;

    public:
        /**
         * This method returns the multicast address.
         *
         * @returns The multicast address.
         *
         */
        const Address &GetAddress(void) const { return static_cast<const Address &>(mAddress); }

        /**
         * This method returns the multicast address.
         *
         * @returns The multicast address.
         *
         */
        Address &GetAddress(void) { return static_cast<Address &>(mAddress); }

        /**
         * This method returns the next multicast address subscribed to the interface.
         *
         * @returns A pointer to the next multicast address.
         *
         */
        const MulticastAddress *GetNext(void) const { return static_cast<const MulticastAddress *>(mNext); }

        /**
         * This method returns the next multicast address subscribed to the interface.
         *
         * @returns A pointer to the next multicast address.
         *
         */
        MulticastAddress *GetNext(void) { return static_cast<MulticastAddress *>(AsNonConst(mNext)); }

    private:
        bool Matches(const Address &aAddress) const { return GetAddress() == aAddress; }
    };

    class ExternalMulticastAddress : public MulticastAddress
    {
        friend class Netif;
        friend class LinkedList<ExternalMulticastAddress>;

    public:
        /**
         * This class represents an iterator for iterating external multicast addresses in a `Netif` instance.
         *
         */
        class Iterator : public ItemPtrIterator<ExternalMulticastAddress, Iterator>
        {
            friend class ItemPtrIterator<ExternalMulticastAddress, Iterator>;
            friend class Netif;

        public:
            /**
             * This constructor initializes an `Iterator` instance to start from the first external multicast address
             * that matches a given IPv6 address type filter.
             *
             * @param[in] aNetif   A reference to the `Netif` instance.
             * @param[in] aFilter  The IPv6 address type filter.
             *
             */
            explicit Iterator(const Netif &aNetif, Address::TypeFilter aFilter = Address::kTypeAny);

        private:
            class Builder
            {
            public:
                Builder(const Netif &aNetif, Address::TypeFilter aFilter)
                    : mNetif(aNetif)
                    , mFilter(aFilter)
                {
                }

                Iterator begin(void) { return Iterator(mNetif, mFilter); }
                Iterator end(void) { return Iterator(mNetif, Iterator::kEndIterator); }

            private:
                const Netif &       mNetif;
                Address::TypeFilter mFilter;
            };

            enum IteratorType : uint8_t
            {
                kEndIterator,
            };

            Iterator(const Netif &aNetif, IteratorType)
                : mNetif(aNetif)
            {
            }

            void AdvanceFrom(const MulticastAddress *aAddr);
            void Advance(void) { AdvanceFrom(mItem->GetNext()); }

            const Netif &       mNetif;
            Address::TypeFilter mFilter;
        };

#if OPENTHREAD_CONFIG_MLR_ENABLE
        /**
         * This method returns the current Multicast Listener Registration (MLR) state.
         *
         * @returns The current Multicast Listener Registration state.
         *
         */
        MlrState GetMlrState(void) const { return mMlrState; }

        /**
         * This method sets the Multicast Listener Registration (MLR) state.
         *
         * @param[in] aState  The new Multicast Listener Registration state.
         *
         */
        void SetMlrState(MlrState aState) { mMlrState = aState; }
#endif

    private:
        ExternalMulticastAddress *GetNext(void) { return static_cast<ExternalMulticastAddress *>(AsNonConst(mNext)); }

#if OPENTHREAD_CONFIG_MLR_ENABLE
        MlrState mMlrState;
#endif
    };

    /**
     * This constructor initializes the network interface.
     *
     * @param[in]  aInstance        A reference to the OpenThread instance.
     *
     */
    explicit Netif(Instance &aInstance);

    /**
     * This method registers a callback to notify internal IPv6 address changes.
     *
     * @param[in]  aCallback         A pointer to a function that is called when an IPv6 address is added or removed.
     * @param[in]  aCallbackContext  A pointer to application-specific context.
     *
     */
    void SetAddressCallback(otIp6AddressCallback aCallback, void *aCallbackContext);

    /**
     * This method returns the linked list of unicast addresses.
     *
     * @returns The linked list of unicast addresses.
     *
     */
    const LinkedList<UnicastAddress> &GetUnicastAddresses(void) const { return mUnicastAddresses; }

    /**
     * This method adds a unicast address to the network interface.
     *
     * This method is intended for addresses internal to OpenThread. The @p aAddress instance is directly added in the
     * unicast address linked list.
     *
     * If @p aAddress is already added, the call to `AddUnicastAddress()` with the same address will perform no action.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     */
    void AddUnicastAddress(UnicastAddress &aAddress);

    /**
     * This method removes a unicast address from the network interface.
     *
     * This method is intended for addresses internal to OpenThread. The @p aAddress instance is removed from the
     * unicast address linked list.
     *
     * If @p aAddress is not in the list, the call to `RemoveUnicastAddress()` will perform no action.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     */
    void RemoveUnicastAddress(const UnicastAddress &aAddress);

    /**
     * This method indicates whether or not an address is assigned to the interface.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     * @retval TRUE   If @p aAddress is assigned to the network interface,
     * @retval FALSE  If @p aAddress is not assigned to the network interface.
     *
     */
    bool HasUnicastAddress(const Address &aAddress) const;

    /**
     * This method indicates whether or not a unicast address is assigned to the network interface.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     * @retval TRUE   If @p aAddress is assigned to the network interface,
     * @retval FALSE  If @p aAddress is not assigned to the network interface.
     *
     */
    bool HasUnicastAddress(const UnicastAddress &aAddress) const { return mUnicastAddresses.Contains(aAddress); }

    /**
     * This method indicates whether a unicast address is an external or internal address.
     *
     * @param[in] aAddress  A reference to the unicast address.
     *
     * @retval TRUE   The address is an external address.
     * @retval FALSE  The address is not an external address (it is an OpenThread internal address).
     *
     */
    bool IsUnicastAddressExternal(const UnicastAddress &aAddress) const;

    /**
     * This method adds an external (to OpenThread) unicast address to the network interface.
     *
     * For external address, the @p aAddress instance is not directly used (i.e., it can be temporary). It is copied
     * into a local entry (allocated from an internal pool) before being added in the unicast address linked list.
     * The maximum number of external addresses is specified by `OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS`.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     * @retval kErrorNone         Successfully added (or updated) the unicast address.
     * @retval kErrorInvalidArgs  The address indicated by @p aAddress is an internal address.
     * @retval kErrorNoBufs       The maximum number of allowed external addresses are already added.
     *
     */
    Error AddExternalUnicastAddress(const UnicastAddress &aAddress);

    /**
     * This method removes a external (to OpenThread) unicast address from the network interface.
     *
     * @param[in]  aAddress  A reference to the unicast address.
     *
     * @retval kErrorNone         Successfully removed the unicast address.
     * @retval kErrorInvalidArgs  The address indicated by @p aAddress is an internal address.
     * @retval kErrorNotFound     The unicast address was not found.
     *
     */
    Error RemoveExternalUnicastAddress(const Address &aAddress);

    /**
     * This method removes all the previously added external (to OpenThread) unicast addresses from the
     * network interface.
     *
     */
    void RemoveAllExternalUnicastAddresses(void);

    /**
     * This method indicates whether or not the network interface is subscribed to a multicast address.
     *
     * @param[in]  aAddress  The multicast address to check.
     *
     * @retval TRUE   If the network interface is subscribed to @p aAddress.
     * @retval FALSE  If the network interface is not subscribed to @p aAddress.
     *
     */
    bool IsMulticastSubscribed(const Address &aAddress) const;

    /**
     * This method subscribes the network interface to the link-local and realm-local all routers addresses.
     *
     * @note This method MUST be called after `SubscribeAllNodesMulticast()` or its behavior is undefined.
     *
     */
    void SubscribeAllRoutersMulticast(void);

    /**
     * This method unsubscribes the network interface to the link-local and realm-local all routers address.
     *
     */
    void UnsubscribeAllRoutersMulticast(void);

    /**
     * This method returns the linked list of multicast addresses.
     *
     * @returns The linked list of multicast addresses.
     *
     */
    const LinkedList<MulticastAddress> &GetMulticastAddresses(void) const { return mMulticastAddresses; }

    /**
     * This method indicates whether a multicast address is an external or internal address.
     *
     * @param[in] aAddress  A reference to the multicast address.
     *
     * @retval TRUE   The address is an external address.
     * @retval FALSE  The address is not an external address (it is an OpenThread internal address).
     *
     */
    bool IsMulticastAddressExternal(const MulticastAddress &aAddress) const;

    /**
     * This method subscribes the network interface to a multicast address.
     *
     * This method is intended for addresses internal to OpenThread. The @p aAddress instance is directly added in the
     * multicast address linked list.
     *
     * @param[in]  aAddress  A reference to the multicast address.
     *
     */
    void SubscribeMulticast(MulticastAddress &aAddress);

    /**
     * This method unsubscribes the network interface to a multicast address.
     *
     * This method is intended for addresses internal to OpenThread. The @p aAddress instance is directly removed from
     * the multicast address linked list.
     *
     * @param[in]  aAddress  A reference to the multicast address.
     *
     */
    void UnsubscribeMulticast(const MulticastAddress &aAddress);

    /**
     * This method subscribes the network interface to the external (to OpenThread) multicast address.
     *
     * For external address, the @p aAddress instance is not directly used (i.e., it can be temporary). It is copied
     * into a local entry (allocated from an internal pool) before being added in the multicast address linked list.
     * The maximum number of external addresses is specified by `OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS`.
     *
     * @param[in]  aAddress  A reference to the multicast address.
     *
     * @retval kErrorNone          Successfully subscribed to @p aAddress.
     * @retval kErrorAlready       The multicast address is already subscribed.
     * @retval kErrorInvalidArgs   The address indicated by @p aAddress is an internal multicast address.
     * @retval kErrorNoBufs        The maximum number of allowed external multicast addresses are already added.
     *
     */
    Error SubscribeExternalMulticast(const Address &aAddress);

    /**
     * This method unsubscribes the network interface to the external (to OpenThread) multicast address.
     *
     * @param[in]  aAddress  A reference to the multicast address.
     *
     * @retval kErrorNone         Successfully unsubscribed to the unicast address.
     * @retval kErrorInvalidArgs  The address indicated by @p aAddress is an internal address.
     * @retval kErrorNotFound     The multicast address was not found.
     *
     */
    Error UnsubscribeExternalMulticast(const Address &aAddress);

    /**
     * This method unsubscribes the network interface from all previously added external (to OpenThread) multicast
     * addresses.
     *
     */
    void UnsubscribeAllExternalMulticastAddresses(void);

    /**
     * This method checks if multicast promiscuous mode is enabled on the network interface.
     *
     * @retval TRUE   If the multicast promiscuous mode is enabled.
     * @retval FALSE  If the multicast promiscuous mode is disabled.
     *
     */
    bool IsMulticastPromiscuousEnabled(void) const { return mMulticastPromiscuous; }

    /**
     * This method enables multicast promiscuous mode on the network interface.
     *
     * @param[in]  aEnabled  TRUE if Multicast Promiscuous mode is enabled, FALSE otherwise.
     *
     */
    void SetMulticastPromiscuous(bool aEnabled) { mMulticastPromiscuous = aEnabled; }

    /**
     * This method enables range-based `for` loop iteration over external multicast addresses on the Netif that matches
     * a given IPv6 address type filter.
     *
     * This method should be used like follows: to iterate over all external multicast addresses
     *
     *     for (Ip6::Netif::ExternalMulticastAddress &addr : Get<ThreadNetif>().IterateExternalMulticastAddresses())
     *     { ... }
     *
     * or to iterate over a subset of external multicast addresses determined by a given address type filter
     *
     *     for (Ip6::Netif::ExternalMulticastAddress &addr :
     *          Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
     *     { ... }
     *
     * @param[in] aFilter  The IPv6 address type filter.
     *
     * @returns An `ExternalMulticastAddress::Iterator::Builder` instance.
     *
     */
    ExternalMulticastAddress::Iterator::Builder IterateExternalMulticastAddresses(
        Address::TypeFilter aFilter = Address::kTypeAny)
    {
        return ExternalMulticastAddress::Iterator::Builder(*this, aFilter);
    }

    /**
     * This method indicates whether or not the network interfaces is subscribed to any external multicast address.
     *
     * @retval TRUE  The network interface is subscribed to at least one external multicast address.
     * @retval FALSE The network interface is not subscribed to any external multicast address.
     *
     */
    bool HasAnyExternalMulticastAddress(void) const { return !ExternalMulticastAddress::Iterator(*this).IsDone(); }

protected:
    /**
     * This method subscribes the network interface to the realm-local all MPL forwarders, link-local, and realm-local
     * all nodes address.
     *
     */
    void SubscribeAllNodesMulticast(void);

    /**
     * This method unsubscribes the network interface from the realm-local all MPL forwarders, link-local and
     * realm-local all nodes address.
     *
     * @note This method MUST be called after `UnsubscribeAllRoutersMulticast()` or its behavior is undefined
     *
     */
    void UnsubscribeAllNodesMulticast(void);

private:
    LinkedList<UnicastAddress>   mUnicastAddresses;
    LinkedList<MulticastAddress> mMulticastAddresses;
    bool                         mMulticastPromiscuous;

    otIp6AddressCallback mAddressCallback;
    void *               mAddressCallbackContext;

    Pool<UnicastAddress, OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS>           mExtUnicastAddressPool;
    Pool<ExternalMulticastAddress, OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS> mExtMulticastAddressPool;

    static const otNetifMulticastAddress kRealmLocalAllMplForwardersMulticastAddress;
    static const otNetifMulticastAddress kLinkLocalAllNodesMulticastAddress;
    static const otNetifMulticastAddress kRealmLocalAllNodesMulticastAddress;
    static const otNetifMulticastAddress kLinkLocalAllRoutersMulticastAddress;
    static const otNetifMulticastAddress kRealmLocalAllRoutersMulticastAddress;
};

/**
 * @}
 *
 */

} // namespace Ip6
} // namespace ot

#endif // NET_NETIF_HPP_
