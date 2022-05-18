/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions related to Thread Network Data service/server entries.
 */

#ifndef NETWORK_DATA_SERVICE_HPP_
#define NETWORK_DATA_SERVICE_HPP_

#include "openthread-core-config.h"

#include <openthread/netdata.h>

#include "backbone_router/bbr_leader.hpp"
#include "common/encoding.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "net/socket.hpp"
#include "thread/network_data_tlvs.hpp"

namespace ot {
namespace NetworkData {
namespace Service {

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;

const uint32_t kThreadEnterpriseNumber = ServiceTlv::kThreadEnterpriseNumber; ///< Thread enterprise number.

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

/**
 * This type implements Thread Network Data "Backbone Router Service" server data generation and parsing.
 *
 */
class BackboneRouter
{
public:
    /**
     * This constant variable represents the Backbone Router service data.
     *
     * The service data contains only the service number (THREAD_SERVICE_DATA_BBR) as a single byte.
     *
     */
    static const uint8_t     kServiceData        = 0x01;
    static constexpr uint8_t kServiceDataMinSize = 1;

    /**
     * This class implements the generation and parsing of "Backbone Router Service" server data.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ServerData
    {
    public:
        /**
         * This method returns the length (in bytes) of server data.
         *
         * @returns The server data length in bytes.
         *
         */
        uint8_t GetLength(void) const { return sizeof(ServerData); }

        /**
         * This method returns the sequence number of Backbone Router.
         *
         * @returns  The sequence number of the Backbone Router.
         *
         */
        uint8_t GetSequenceNumber(void) const { return mSequenceNumber; }

        /**
         * This method sets the sequence number of Backbone Router.
         *
         * @param[in]  aSequenceNumber  The sequence number of Backbone Router.
         *
         */
        void SetSequenceNumber(uint8_t aSequenceNumber) { mSequenceNumber = aSequenceNumber; }

        /**
         * This method returns the Registration Delay (in seconds) of Backbone Router.
         *
         * @returns The BBR Registration Delay (in seconds) of Backbone Router.
         *
         */
        uint16_t GetReregistrationDelay(void) const { return HostSwap16(mReregistrationDelay); }

        /**
         * This method sets the Registration Delay (in seconds) of Backbone Router.
         *
         * @param[in]  aReregistrationDelay  The Registration Delay (in seconds) of Backbone Router.
         *
         */
        void SetReregistrationDelay(uint16_t aReregistrationDelay)
        {
            mReregistrationDelay = HostSwap16(aReregistrationDelay);
        }

        /**
         * This method returns the multicast listener report timeout (in seconds) of Backbone Router.
         *
         * @returns The multicast listener report timeout (in seconds) of Backbone Router.
         *
         */
        uint32_t GetMlrTimeout(void) const { return HostSwap32(mMlrTimeout); }

        /**
         * This method sets multicast listener report timeout (in seconds) of Backbone Router.
         *
         * @param[in]  aMlrTimeout  The multicast listener report timeout (in seconds) of Backbone Router.
         *
         */
        void SetMlrTimeout(uint32_t aMlrTimeout) { mMlrTimeout = HostSwap32(aMlrTimeout); }

    private:
        uint8_t  mSequenceNumber;
        uint16_t mReregistrationDelay;
        uint32_t mMlrTimeout;
    } OT_TOOL_PACKED_END;

    BackboneRouter(void) = delete;
};

#endif // #if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

/**
 * This type implements Thread Network Data "DNS/SRP Service Anycast Address" generation and parsing.
 *
 */
class DnsSrpAnycast
{
public:
    static constexpr uint8_t kServiceNumber = 0x5c; ///< The service number of a `DnsSrpAnycast` entry.

    /**
     * This constant variable represents the short version of service data.
     *
     * The short version of service data contains only service number as a single byte.
     *
     */
    static const uint8_t kServiceData = kServiceNumber;

    /**
     * This structure represents information about an DNS/SRP server parsed from related Network Data service entries.
     *
     */
    struct Info
    {
        /**
         * This method indicates whether or not the sequence number from the current `Info` is ahead of (more recent
         * than) the sequence number from another `Info`.
         *
         * The sequence numbers comparison follows the Serial Number Arithmetic logic from RFC-1982. It is semantically
         * equivalent to `GetSequenceNumber() > aOther.GetSequenceNumber()` while handling roll-over of the `uint8_t`
         * values.
         *
         * @param[in] aOther   The other `Info` to compare with.
         *
         * @retval TRUE  The `Info` is ahead of @p aOther.
         * @retval FALSE The `Info` is not ahead of @p aOther.
         *
         */
        bool IsSequenceNumberAheadOf(const Info &aOther) const
        {
            return (((aOther.mSequenceNumber - mSequenceNumber) & (1U << 7)) != 0);
        }

        Ip6::Address mAnycastAddress; ///< The anycast address associated with the DNS/SRP servers.
        uint8_t      mSequenceNumber; ///< Sequence number used to notify SRP client if they need to re-register.
    };

    /**
     * This class represents the "DNS/SRP Service (Anycast)" service data.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ServiceData
    {
    public:
        /**
         * This constructor initializes the `ServiceData` object.
         *
         * @param[in] aSequenceNumber   The sequence number of "DNS/SRP server" service.
         *
         */
        explicit ServiceData(uint8_t aSequenceNumber)
            : mServiceNumber(kServiceNumber)
            , mSequenceNumber(aSequenceNumber)
        {
            OT_UNUSED_VARIABLE(mServiceNumber);
        }

        /**
         * This method returns the length (in bytes) of service data.
         *
         * @returns The data length in bytes.
         *
         */
        uint8_t GetLength(void) const { return sizeof(ServiceData); }

        /**
         * This method returns the sequence number.
         *
         * @returns The sequence number.
         *
         */
        uint8_t GetSequenceNumber(void) const { return mSequenceNumber; }

    private:
        uint8_t mServiceNumber;
        uint8_t mSequenceNumber;
    } OT_TOOL_PACKED_END;

    DnsSrpAnycast(void) = delete;
};

/**
 * This type implements Thread Network Data DNS/SRP Service (Unicast Address) generation and parsing.
 *
 */
class DnsSrpUnicast
{
public:
    static constexpr uint8_t kServiceNumber = 0x5d; ///< The service number of `DnsSrpUnicast` entry.

    /**
     * This constant variable represents the short version of service data.
     *
     * The short version of service data contains only service number as a single byte.
     *
     */
    static const uint8_t kServiceData = kServiceNumber;

    /**
     * This structure represents information about an DNS/SRP server parsed from related Network Data service entries.
     *
     */
    struct Info
    {
        Ip6::SockAddr mSockAddr; ///< The socket address (IPv6 address and port) of the DNS/SRP server.
    };

    /**
     * This class represents long version of "DNS/SRP Service (Unicast)" service data.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ServiceData
    {
    public:
        /**
         * This constructor initializes the `ServiceData` object.
         *
         * @param[in] aAddress   The IPv6 address of DNS/SRP server.
         * @param[in] aPort      The port number of DNS/SRP server.
         *
         */
        explicit ServiceData(const Ip6::Address &aAddress, uint16_t aPort)
            : mServiceNumber(kServiceNumber)
            , mAddress(aAddress)
            , mPort(HostSwap16(aPort))
        {
            OT_UNUSED_VARIABLE(mServiceNumber);
        }

        /**
         * This method returns the length (in bytes) of service data.
         *
         * @returns The data length in bytes.
         *
         */
        uint8_t GetLength(void) const { return sizeof(ServiceData); }

        /**
         * This method returns the IPv6 address.
         *
         * @returns The IPv6 address
         *
         */
        const Ip6::Address &GetAddress(void) const { return mAddress; }

        /**
         * This method returns the port number.
         *
         * @returns The port number.
         *
         */
        uint16_t GetPort(void) const { return HostSwap16(mPort); }

    private:
        uint8_t      mServiceNumber;
        Ip6::Address mAddress;
        uint16_t     mPort;
    } OT_TOOL_PACKED_END;

    /**
     * This class represents long version of "DNS/SRP Service (Unicast)" server data.
     *
     */
    OT_TOOL_PACKED_BEGIN
    class ServerData
    {
    public:
        /**
         * This constructor initializes the `ServerData` object.
         *
         * @param[in] aAddress   The IPv6 address of DNS/SRP server.
         * @param[in] aPort      The port number of DNS/SRP server.
         *
         */
        ServerData(const Ip6::Address &aAddress, uint16_t aPort)
            : mAddress(aAddress)
            , mPort(HostSwap16(aPort))
        {
        }

        /**
         * This method returns the length (in bytes) of server data.
         *
         * @returns The data length in bytes.
         *
         */
        uint8_t GetLength(void) const { return sizeof(ServerData); }

        /**
         * This method returns the IPv6 address.
         *
         * @returns The IPv6 address
         *
         */
        const Ip6::Address &GetAddress(void) const { return mAddress; }

        /**
         * This method returns the port number.
         *
         * @returns The port number.
         *
         */
        uint16_t GetPort(void) const { return HostSwap16(mPort); }

    private:
        Ip6::Address mAddress;
        uint16_t     mPort;
    } OT_TOOL_PACKED_END;

    DnsSrpUnicast(void) = delete;
};

/**
 * This class manages the Thread Service entries in Thread Network Data.
 *
 */
class Manager : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This class represents an iterator used to iterate through Network Data Service entries.
     *
     */
    class Iterator : public Clearable<Iterator>
    {
        friend class Manager;

    public:
        /**
         * This constructor initializes the iterator (as empty/clear).
         *
         */
        Iterator(void)
            : mServiceTlv(nullptr)
            , mServerSubTlv(nullptr)
        {
        }

        /**
         * This method resets the iterator to start from beginning.
         *
         */
        void Reset(void)
        {
            mServiceTlv   = nullptr;
            mServerSubTlv = nullptr;
        }

    private:
        const ServiceTlv *mServiceTlv;
        const ServerTlv * mServerSubTlv;
    };

    /**
     * This constructor initializes the `Manager` object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Manager(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
    }

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    /**
     * This method adds a Thread Service entry to the local Thread Network Data.
     *
     * This version of `Add<SeviceType>()` is intended for use with a `ServiceType` that has a constant service data
     * format with a non-empty and potentially non-const server data format (provided as input parameter).
     *
     * The template type `ServiceType` has the following requirements:
     *   - It MUST have a constant variable `ServiceType::kServiceData` specifying the service data.
     *   - It MUST define nested type `ServiceType::ServerData` representing the server data (and its format).
     *   - The `ServiceType::ServerData` MUST provide `GetLength()` method returning the length of server data.
     *
     * @tparam    ServiceType    The service type to be added.
     *
     * @param[in] aServerData    The server data.
     * @param[in] aServerStable  The Stable flag value for Server TLV.
     *
     * @retval kErrorNone     Successfully added the Service entry.
     * @retval kErrorNoBufs   Insufficient space to add the Service entry.
     *
     */
    template <typename ServiceType>
    Error Add(const typename ServiceType::ServerData &aServerData, bool aServerStable = true)
    {
        return AddService(&ServiceType::kServiceData, sizeof(ServiceType::kServiceData), aServerStable, &aServerData,
                          aServerData.GetLength());
    }

    /**
     * This method adds a Thread Service entry to the local Thread Network Data.
     *
     * This version of `Add<SeviceType>()` is intended for use with a `ServiceType` that has a non-const service data
     * format (provided as input parameter) with an empty server data.
     *
     * The template type `ServiceType` has the following requirements:
     *   - It MUST define nested type `ServiceType::ServiceData` representing the service data (and its format).
     *   - The `ServiceType::ServiceData` MUST provide `GetLength()` method returning the length of service data.
     *
     * @tparam    ServiceType    The service type to be added.
     *
     * @param[in] aServiceData   The service data.
     * @param[in] aServerStable  The Stable flag value for Server TLV.
     *
     * @retval kErrorNone     Successfully added the Service entry.
     * @retval kErrorNoBufs   Insufficient space to add the Service entry.
     *
     */
    template <typename ServiceType>
    Error Add(const typename ServiceType::ServiceData &aServiceData, bool aServerStable = true)
    {
        return AddService(&aServiceData, aServiceData.GetLength(), aServerStable, nullptr, 0);
    }

    /**
     * This method removes a Thread Service entry from the local Thread Network Data.
     *
     * This version of `Remove<SeviceType>()` is intended for use with a `ServiceType` that has a constant service data
     * format.
     *
     * The template type `ServiceType` has the following requirements:
     *   - It MUST have a constant variable `ServiceType::kServiceData` specifying the service data.
     *
     * @tparam   ServiceType       The service type to be removed.
     *
     * @retval kErrorNone       Successfully removed the Service entry.
     * @retval kErrorNotFound   Could not find the Service entry.
     *
     */
    template <typename ServiceType> Error Remove(void)
    {
        return RemoveService(&ServiceType::kServiceData, sizeof(ServiceType::kServiceData));
    }

    /**
     * This method removes a Thread Service entry from the local Thread Network Data.
     *
     * This version of `Remove<SeviceType>()` is intended for use with a `ServiceType` that has a non-const service data
     * format (provided as input parameter).
     *
     * The template type `ServiceType` has the following requirements:
     *   - It MUST define nested type `ServiceType::ServiceData` representing the service data (and its format).
     *   - The `ServiceType::ServiceData` MUST provide `GetLength()` method returning the length of service data.
     *
     * @tparam   ServiceType       The service type to be removed.
     *
     * @param[in] aServiceData     The service data.
     *
     * @retval kErrorNone       Successfully removed the Service entry.
     * @retval kErrorNotFound   Could not find the Service entry.
     *
     */
    template <typename ServiceType> Error Remove(const typename ServiceType::ServiceData &aServiceData)
    {
        return RemoveService(&aServiceData, aServiceData.GetLength());
    }

#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

    /**
     * This method gets the Service ID for the specified service from Thread Network Data.
     *
     * The template type `ServiceType` has the following requirements:
     *   - It MUST have a constant `ServiceType::kServiceNumber` specifying the service number.
     *
     * @tparam     ServiceType     The service type to be added.
     *
     * @param[in]  aServerStable   The Stable flag value for Server TLV
     * @param[out] aServiceId      A reference where to put the Service ID.
     *
     * @retval kErrorNone       Successfully got the Service ID.
     * @retval kErrorNotFound   The specified service was not found.
     *
     */
    template <typename ServiceType> Error GetServiceId(bool aServerStable, uint8_t &aServiceId) const
    {
        return GetServiceId(&ServiceType::kServiceData, sizeof(ServiceType::kServiceData), aServerStable, aServiceId);
    }

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    /**
     * This method gets the Primary Backbone Router (PBBR) in the Thread Network Data.
     *
     * @param[out]  aConfig      The Primary Backbone Router configuration.
     *
     */
    void GetBackboneRouterPrimary(ot::BackboneRouter::BackboneRouterConfig &aConfig) const;
#endif

    /**
     * This method gets the next DNS/SRP info from the Thread Network Data "DNS/SRP Service Anycast Address" entries.
     *
     * To get the first entry, @p aIterator should be cleared (e.g., a new instance of `Iterator` or calling `Clear()`
     * method).
     *
     * @param[inout] aIterator     A reference to an iterator.
     * @param[out]   aInfo         A reference to `DnsSrpAnycast::Info` to return the info.
     *
     * @retval kErrorNone       Successfully got the next info. @p aInfo and @p aIterator are updated.
     * @retval kErrorNotFound   No more matching entries in the Network Data.
     *
     */
    Error GetNextDnsSrpAnycastInfo(Iterator &aIterator, DnsSrpAnycast::Info &aInfo) const;

    /**
     * This method finds the preferred DNS/SRP info among all the Thread Network Data "DNS/SRP Service Anycast Address"
     * entries.
     *
     * The preferred entry is determined based on the sequence number value where a larger value (in the sense
     * specified by Serial Number Arithmetic logic in RFC-1982) is considered more recent and therefore preferred.
     *
     * @param[out] aInfo        A reference to `DnsSrpAnycast::Info` to return the info.
     *
     * @retval kErrorNone       Successfully found the preferred info. @p aInfo is updated.
     * @retval kErrorNotFound   No "DNS/SRP Service Anycast" entry in Network Data.
     *
     */
    Error FindPreferredDnsSrpAnycastInfo(DnsSrpAnycast::Info &aInfo) const;

    /**
     * This method gets the next DNS/SRP info from the Thread Network Data "DNS/SRP Service Unicast Address" entries.
     *
     * To get the first entry @p aIterator should be cleared (e.g., a new instance of `Iterator` or calling `Clear()`
     * method).
     *
     * @param[inout] aIterator     A reference to an iterator.
     * @param[out]   aInfo         A reference to `DnsSrpUnicast::Info` to return the info.
     *
     * @retval kErrorNone       Successfully got the next info. @p aInfo and @p aIterator are updated.
     * @retval kErrorNotFound   No more matching entries in the Network Data.
     *
     */
    Error GetNextDnsSrpUnicastInfo(Iterator &aIterator, DnsSrpUnicast::Info &aInfo) const;

private:
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    Error AddService(const void *aServiceData,
                     uint8_t     aServiceDataLength,
                     bool        aServerStable,
                     const void *aServerData,
                     uint8_t     aServerDataLength);
    Error RemoveService(const void *aServiceData, uint8_t aServiceDataLength);
#endif

    Error GetServiceId(const void *aServiceData,
                       uint8_t     aServiceDataLength,
                       bool        aServerStable,
                       uint8_t &   aServiceId) const;
    Error IterateToNextServer(Iterator &aIterator) const;

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    bool IsBackboneRouterPreferredTo(const ServerTlv &                 aServerTlv,
                                     const BackboneRouter::ServerData &aServerData,
                                     const ServerTlv &                 aOtherServerTlv,
                                     const BackboneRouter::ServerData &aOtherServerData) const;
#endif
};

} // namespace Service
} // namespace NetworkData
} // namespace ot

#endif // NETWORK_DATA_SERVICE_HPP_
