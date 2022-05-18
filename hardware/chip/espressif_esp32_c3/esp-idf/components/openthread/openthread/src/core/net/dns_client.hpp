/*
 *  Copyright (c) 2017-2021, The OpenThread Authors.
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

#ifndef DNS_CLIENT_HPP_
#define DNS_CLIENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#include <openthread/dns_client.h>

#include "common/clearable.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "net/dns_types.hpp"
#include "net/ip6.hpp"
#include "net/netif.hpp"

/**
 * @file
 *   This file includes definitions for the DNS client.
 */

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE

#if !OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#error "DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE requires OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE"
#endif

#if !OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
#error "DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE requires OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE"
#endif

#endif

/**
 * This struct represents an opaque (and empty) type for a response to an address resolution DNS query.
 *
 */
struct otDnsAddressResponse
{
};

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

/**
 * This struct represents an opaque (and empty) type for a response to browse (service instance enumeration) DNS query.
 *
 */
struct otDnsBrowseResponse
{
};

/**
 * This struct represents an opaque (and empty) type for a response to service inst resolution DNS query.
 *
 */
struct otDnsServiceResponse
{
};

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

namespace ot {

namespace Srp {
class Client;
}

namespace Dns {

/**
 * This class implements DNS client.
 *
 */
class Client : public InstanceLocator, private NonCopyable
{
    friend class ot::Srp::Client;

    typedef Message Query; // `Message` is used to save `Query` related info.

public:
    /**
     * This type represents a DNS query configuration (e.g., server address, response wait timeout, etc).
     *
     */
    class QueryConfig : public otDnsQueryConfig, public Clearable<QueryConfig>
    {
        friend class Client;

    public:
        /**
         * This enumeration type represents the "Recursion Desired" (RD) flag in a `otDnsQueryConfig`.
         *
         */
        enum RecursionFlag : uint8_t
        {
            kFlagUnspecified      = OT_DNS_FLAG_UNSPECIFIED,       ///< The flag is not specified.
            kFlagRecursionDesired = OT_DNS_FLAG_RECURSION_DESIRED, ///< Server can resolve the query recursively.
            kFlagNoRecursion      = OT_DNS_FLAG_NO_RECURSION,      ///< Server can not resolve the query recursively.
        };

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        /**
         * This enumeration type represents the NAT64 mode.
         *
         */
        enum Nat64Mode : uint8_t
        {
            kNat64Unspecified = OT_DNS_NAT64_UNSPECIFIED, ///< NAT64 mode is not specified. Use default NAT64 mode.
            kNat64Allow       = OT_DNS_NAT64_ALLOW,       ///< Allow NAT64 address translation
            kNat64Disallow    = OT_DNS_NAT64_DISALLOW,    ///< Disallow NAT64 address translation.
        };
#endif

        /**
         * This is the default constructor for `QueryConfig` object.
         *
         */
        QueryConfig(void) = default;

        /**
         * This method gets the server socket address (IPv6 address and port number).
         *
         * @returns The server socket address.
         *
         */
        const Ip6::SockAddr &GetServerSockAddr(void) const
        {
            return static_cast<const Ip6::SockAddr &>(mServerSockAddr);
        }

        /**
         * This method gets the wait time to receive response from server (in msec).
         *
         * @returns The timeout interval in msec.
         *
         */
        uint32_t GetResponseTimeout(void) const { return mResponseTimeout; }

        /**
         * This method gets the maximum number of query transmit attempts before reporting failure.
         *
         * @returns The maximum number of query transmit attempts.
         *
         */
        uint8_t GetMaxTxAttempts(void) const { return mMaxTxAttempts; }

        /**
         * This method gets the recursion flag indicating whether the server can resolve the query recursively or not.
         *
         * @returns The recursion flag.
         *
         */
        RecursionFlag GetRecursionFlag(void) const { return static_cast<RecursionFlag>(mRecursionFlag); }

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        /**
         * This method gets the NAT64 mode.
         *
         * @returns The NAT64 mode.
         *
         */
        Nat64Mode GetNat64Mode(void) const { return static_cast<Nat64Mode>(mNat64Mode); }
#endif

    private:
        static constexpr uint32_t kDefaultResponseTimeout = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RESPONSE_TIMEOUT;
        static constexpr uint16_t kDefaultServerPort      = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_PORT;
        static constexpr uint8_t  kDefaultMaxTxAttempts   = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_MAX_TX_ATTEMPTS;
        static constexpr bool kDefaultRecursionDesired    = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RECURSION_DESIRED_FLAG;

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        static constexpr bool kDefaultNat64Allowed = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NAT64_ALLOWED;
#endif

        enum InitMode : uint8_t
        {
            kInitFromDefaults,
        };

        static const char kDefaultServerAddressString[];

        explicit QueryConfig(InitMode aMode);

        Ip6::SockAddr &GetServerSockAddr(void) { return static_cast<Ip6::SockAddr &>(mServerSockAddr); }

        void SetResponseTimeout(uint32_t aResponseTimeout) { mResponseTimeout = aResponseTimeout; }
        void SetMaxTxAttempts(uint8_t aMaxTxAttempts) { mMaxTxAttempts = aMaxTxAttempts; }
        void SetRecursionFlag(RecursionFlag aFlag) { mRecursionFlag = static_cast<otDnsRecursionFlag>(aFlag); }
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        void SetNat64Mode(Nat64Mode aMode) { mNat64Mode = static_cast<otDnsNat64Mode>(aMode); }
#endif

        void SetFrom(const QueryConfig &aConfig, const QueryConfig &aDefaultConfig);
    };

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    /**
     * This structure provides info for a DNS service instance.
     *
     */
    typedef otDnsServiceInfo ServiceInfo;
#endif

    /**
     * This class represents a DNS query response.
     *
     */
    class Response : public otDnsAddressResponse,
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
                     public otDnsBrowseResponse,
                     public otDnsServiceResponse,
#endif
                     public Clearable<Response>
    {
        friend class Client;

    protected:
        enum Section : uint8_t
        {
            kAnswerSection,
            kAdditionalDataSection,
        };

        Response(void) { Clear(); }

        Error GetName(char *aNameBuffer, uint16_t aNameBufferSize) const;
        void  SelectSection(Section aSection, uint16_t &aOffset, uint16_t &aNumRecord) const;
        Error CheckForHostNameAlias(Section aSection, Name &aHostName) const;
        Error FindHostAddress(Section       aSection,
                              const Name &  aHostName,
                              uint16_t      aIndex,
                              Ip6::Address &aAddress,
                              uint32_t &    aTtl) const;
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        Error FindARecord(Section aSection, const Name &aHostName, uint16_t aIndex, ARecord &aARecord) const;
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        Error FindServiceInfo(Section aSection, const Name &aName, ServiceInfo &aServiceInfo) const;
#endif

        Instance *     mInstance;              // The OpenThread instance.
        Query *        mQuery;                 // The associated query.
        const Message *mMessage;               // The response message.
        uint16_t       mAnswerOffset;          // Answer section offset in `mMessage`.
        uint16_t       mAnswerRecordCount;     // Number of records in answer section.
        uint16_t       mAdditionalOffset;      // Additional data section offset in `mMessage`.
        uint16_t       mAdditionalRecordCount; // Number of records in additional data section.
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        // This flag is only used in an IPv6 address query response.
        // It indicates that the response does not contain any IPv6
        // addresses but server provided at least one IPv4 address
        // in the additional data section for NAT64 address synthesis.
        bool mIp6QueryResponseRequiresNat64;
#endif
    };

    /**
     * This type represents the function pointer callback which is called when a DNS response for an address resolution
     * query is received.
     *
     */
    typedef otDnsAddressCallback AddressCallback;

    /**
     * This type represents an address resolution query DNS response.
     *
     */
    class AddressResponse : public Response
    {
        friend class Client;

    public:
        /**
         * This method gets the host name associated with an address resolution DNS response.
         *
         * This method MUST only be used from `AddressCallback`.
         *
         * @param[out] aNameBuffer       A buffer to char array to output the host name.
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The host name was read successfully.
         * @retval kErrorNoBufs  The name does not fit in @p aNameBuffer.
         *
         */
        Error GetHostName(char *aNameBuffer, uint16_t aNameBufferSize) const
        {
            return GetName(aNameBuffer, aNameBufferSize);
        }

        /**
         * This method gets the IPv6 address associated with an address resolution DNS response.
         *
         * This method MUST only be used from `AddressCallback`.
         *
         * The response may include multiple IPv6 address records. @p aIndex can be used to iterate through the list of
         * addresses. Index zero gets the the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone       The address was read successfully.
         * @retval kErrorNotFound   No address record at @p aIndex.
         * @retval kErrorParse      Could not parse the records.
         *
         */
        Error GetAddress(uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;

    private:
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        Error GetNat64Prefix(Ip6::Prefix &aPrefix) const;
#endif
    };

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

    /**
     * This type represents the function pointer callback which is called when a response for a browse (service
     * instance enumeration) DNS query is received.
     *
     */
    typedef otDnsBrowseCallback BrowseCallback;

    /**
     * This type represents a browse (service instance enumeration) DNS response.
     *
     */
    class BrowseResponse : public Response
    {
        friend class Client;

    public:
        /**
         * This method gets the service name associated with a DNS browse response.
         *
         * This method MUST only be used from `BrowseCallback`.
         *
         * @param[out] aNameBuffer       A buffer to char array to output the host name.
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The host name was read successfully.
         * @retval kErrorNoBufs  The name does not fit in @p aNameBuffer.
         *
         */
        Error GetServiceName(char *aNameBuffer, uint16_t aNameBufferSize) const
        {
            return GetName(aNameBuffer, aNameBufferSize);
        }

        /**
         * This method gets a service instance associated with a DNS browse (service instance enumeration) response.
         *
         * This method MUST only be used from `BrowseCallback`.
         *
         * A response may include multiple service instance records. @p aIndex can be used to iterate through the list.
         * Index zero gives the the first record. When we reach end of the list, `kErrorNotFound` is returned.
         *
         * Note that this method gets the service instance label and not the full service instance name which is of the
         * form `<Instance>.<Service>.<Domain>`.
         *
         * @param[in]  aResponse          A pointer to a response.
         * @param[in]  aIndex             The service instance record index to retrieve.
         * @param[out] aLabelBuffer       A char array to output the service instance label (MUST NOT be NULL).
         * @param[in]  aLabelBufferSize   The size of @p aLabelBuffer.
         *
         * @retval kErrorNone         The service instance was read successfully.
         * @retval kErrorNoBufs       The name does not fit in @p aNameBuffer.
         * @retval kErrorNotFound     No service instance record at @p aIndex.
         * @retval kErrorParse        Could not parse the records.
         *
         */
        Error GetServiceInstance(uint16_t aIndex, char *aLabelBuffer, uint8_t aLabelBufferSize) const;

        /**
         * This method gets info for a service instance from a DNS browse (service instance enumeration) response.
         *
         * This method MUST only be used from `BrowseCallback`.
         *
         * A browse DNS response should include the SRV, TXT, and AAAA records for the service instances that are
         * enumerated (note that it is a SHOULD and not a MUST requirement). This method tries to retrieve this info
         * for a given service instance.
         *
         * - If no matching SRV record is found, `kErrorNotFound` is returned.
         * - If a matching SRV record is found, @p aServiceInfo is updated returning `kErrorNone`.
         * - If no matching TXT record is found, `mTxtDataSize` in @p aServiceInfo is set to zero.
         * - If no matching AAAA record is found, `mHostAddress is set to all zero or unspecified address.
         * - If there are multiple AAAA records for the host name `mHostAddress` is set to the first one. The other
         *   addresses can be retrieved using `GetHostAddress()` method.
         *
         * @param[in]  aInstanceLabel     The service instance label (MUST NOT be `nullptr`).
         * @param[out] aServiceInfo       A `ServiceInfo` to output the service instance information.
         *
         * @retval kErrorNone         The service instance info was read. @p aServiceInfo is updated.
         * @retval kErrorNotFound     Could not find a matching SRV record for @p aInstanceLabel.
         * @retval kErrorNoBufs       The host name and/or the TXT data could not fit in given buffers.
         * @retval kErrorParse        Could not parse the records.
         *
         */
        Error GetServiceInfo(const char *aInstanceLabel, ServiceInfo &aServiceInfo) const;

        /**
         * This method gets the host IPv6 address from a DNS browse (service instance enumeration) response.
         *
         * This method MUST only be used from `BrowseCallback`.
         *
         * The response can include zero or more IPv6 address records. @p aIndex can be used to iterate through the
         * list of addresses. Index zero gets the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aHostName     The host name to get the address (MUST NOT be `nullptr`).
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone       The address was read successfully.
         * @retval kErrorNotFound   No address record for @p aHostname at @p aIndex.
         * @retval kErrorParse      Could not parse the records.
         *
         */
        Error GetHostAddress(const char *aHostName, uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;

    private:
        Error FindPtrRecord(const char *aInstanceLabel, Name &aInstanceName) const;
    };

    /**
     * This type represents the function pointer callback which is called when a response for a service instance
     * resolution DNS query is received.
     *
     */
    typedef otDnsServiceCallback ServiceCallback;

    /**
     * This type represents a service instance resolution DNS response.
     *
     */
    class ServiceResponse : public Response
    {
        friend class Client;

    public:
        /**
         * This method gets the service instance name associated with a DNS service instance resolution response.
         *
         * This method MUST only be used from `ServiceCallback`.
         *
         * @param[out] aLabelBuffer      A buffer to char array to output the service instance label (MUST NOT be NULL).
         * @param[in]  aLabelBufferSize  The size of @p aLabelBuffer.
         * @param[out] aNameBuffer       A buffer to char array to output the rest of service name (can be NULL if user
         *                               is not interested in getting the name).
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The service instance name was read successfully.
         * @retval kErrorNoBufs  Either the label or name does not fit in the given buffers.
         *
         */
        Error GetServiceName(char *   aLabelBuffer,
                             uint8_t  aLabelBufferSize,
                             char *   aNameBuffer,
                             uint16_t aNameBufferSize) const;

        /**
         * This method gets info for a service instance from a DNS service instance resolution response.
         *
         * This method MUST only be used from `ServiceCallback`.
         *
         * - If no matching SRV record is found, `kErrorNotFound` is returned.
         * - If a matching SRV record is found, @p aServiceInfo is updated and `kErrorNone` is returned.
         * - If no matching TXT record is found, `mTxtDataSize` in @p aServiceInfo is set to zero.
         * - If no matching AAAA record is found, `mHostAddress is set to all zero or unspecified address.
         * - If there are multiple AAAA records for the host name, `mHostAddress` is set to the first one. The other
         *   addresses can be retrieved using `GetHostAddress()` method.
         *
         * @param[out] aServiceInfo       A `ServiceInfo` to output the service instance information
         *
         * @retval kErrorNone         The service instance info was read. @p aServiceInfo is updated.
         * @retval kErrorNotFound     Could not find a matching SRV record.
         * @retval kErrorNoBufs       The host name and/or TXT data could not fit in the given buffers.
         * @retval kErrorParse        Could not parse the records in the @p aResponse.
         *
         */
        Error GetServiceInfo(ServiceInfo &aServiceInfo) const;

        /**
         * This method gets the host IPv6 address from a DNS service instance resolution response.
         *
         * This method MUST only be used from `ServiceCallback`.
         *
         * The response can include zero or more IPv6 address records. @p aIndex can be used to iterate through the
         * list of addresses. Index zero gets the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aHostName     The host name to get the address (MUST NOT be `nullptr`).
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone       The address was read successfully.
         * @retval kErrorNotFound   No address record for @p aHostname at @p aIndex.
         * @retval kErrorParse      Could not parse the records.
         *
         */
        Error GetHostAddress(const char *aHostName, uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;
    };

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Client(Instance &aInstance);

    /**
     * This method starts the DNS client.
     *
     * @retval kErrorNone     Successfully started the DNS client.
     * @retval kErrorAlready  The socket is already open.
     *
     */
    Error Start(void);

    /**
     * This method stops the DNS client.
     *
     */
    void Stop(void);

    /**
     * This method gets the current default query config being used by DNS client.
     *
     * @returns The current default query config.
     *
     */
    const QueryConfig &GetDefaultConfig(void) const { return mDefaultConfig; }

    /**
     * This method sets the default query config.
     *
     * @param[in] aQueryConfig   The new default query config.
     *
     */
    void SetDefaultConfig(const QueryConfig &aQueryConfig);

    /**
     * This method resets the default config to the config used when the OpenThread stack starts.
     *
     * When OpenThread stack starts, the default DNS query config is determined from a set of OT config options such as
     * `OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS`, `_DEFAULT_SERVER_PORT`, or `_DEFAULT_RESPONSE_TIMEOUT`
     * etc. (see `config/dns_client.h` for all related config options).
     *
     */
    void ResetDefaultConfig(void);

    /**
     * This method sends an address resolution DNS query for AAAA (IPv6) record for a given host name.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aHostName        The host name for which to query the address (MUST NOT be `nullptr`).
     * @param[in]  aCallback        A callback function pointer to report the result of query.
     * @param[in]  aContext         A pointer to arbitrary context information passed to @p aCallback.
     * @param[in]  aConfig          The config to use for this query.
     *
     * @retval kErrorNone           Successfully sent DNS query.
     * @retval kErrorNoBufs         Failed to allocate retransmission data.
     * @retval kErrorInvalidArgs    The host name is not valid format.
     * @retval kErrorInvalidState   Cannot send query since Thread interface is not up.
     *
     */
    Error ResolveAddress(const char *       aHostName,
                         AddressCallback    aCallback,
                         void *             aContext,
                         const QueryConfig *aConfig = nullptr);

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

    /**
     * This method sends a browse (service instance enumeration) DNS query for a given service name.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aServiceName     The service name to query for (MUST NOT be `nullptr`).
     * @param[in]  aCallback        The callback to report the response or errors (such as time-out).
     * @param[in]  aContext         A pointer to arbitrary context information.
     * @param[in]  aConfig          The config to use for this query.
     *
     * @retval kErrorNone       Query sent successfully. @p aCallback will be invoked to report the status.
     * @retval kErrorNoBufs     Insufficient buffer to prepare and send query.
     *
     */
    Error Browse(const char *       aServiceName,
                 BrowseCallback     aCallback,
                 void *             aContext,
                 const QueryConfig *aConfig = nullptr);

    /**
     * This method sends a DNS service instance resolution query for a given service instance.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aServerSockAddr    The server socket address.
     * @param[in]  aInstanceLabel     The service instance label.
     * @param[in]  aServiceName       The service name (together with @p aInstanceLabel form full instance name).
     * @param[in]  aCallback          A function pointer that shall be called on response reception or time-out.
     * @param[in]  aContext           A pointer to arbitrary context information.
     * @param[in]  aConfig            The config to use for this query.
     *
     * @retval kErrorNone         Query sent successfully. @p aCallback will be invoked to report the status.
     * @retval kErrorNoBufs       Insufficient buffer to prepare and send query.
     * @retval kErrorInvalidArgs  @p aInstanceLabel is `nullptr`.
     *
     */
    Error ResolveService(const char *         aInstanceLabel,
                         const char *         aServiceName,
                         otDnsServiceCallback aCallback,
                         void *               aContext,
                         const QueryConfig *  aConfig = nullptr);

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

private:
    enum QueryType : uint8_t
    {
        kIp6AddressQuery, // IPv6 Address resolution.
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        kIp4AddressQuery, // IPv4 Address resolution
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        kBrowseQuery,  // Browse (service instance enumeration).
        kServiceQuery, // Service instance resolution.
#endif
    };

    union Callback
    {
        AddressCallback mAddressCallback;
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        BrowseCallback  mBrowseCallback;
        ServiceCallback mServiceCallback;
#endif
    };

    typedef MessageQueue QueryList; // List of queries.

    struct QueryInfo : public Clearable<QueryInfo> // Query related Info
    {
        void ReadFrom(const Query &aQuery) { IgnoreError(aQuery.Read(0, *this)); }

        QueryType   mQueryType;
        uint16_t    mMessageId;
        Callback    mCallback;
        void *      mCallbackContext;
        TimeMilli   mRetransmissionTime;
        QueryConfig mConfig;
        uint8_t     mTransmissionCount;
        // Followed by the name (service, host, instance) encoded as a `Dns::Name`.
    };

    static constexpr uint16_t kNameOffsetInQuery = sizeof(QueryInfo);

    Error       StartQuery(QueryInfo &        aInfo,
                           const QueryConfig *aConfig,
                           const char *       aLabel,
                           const char *       aName,
                           void *             aContext);
    Error       AllocateQuery(const QueryInfo &aInfo, const char *aLabel, const char *aName, Query *&aQuery);
    void        FreeQuery(Query &aQuery);
    void        UpdateQuery(Query &aQuery, const QueryInfo &aInfo) { aQuery.Write(0, aInfo); }
    void        SendQuery(Query &aQuery, QueryInfo &aInfo, bool aUpdateTimer);
    void        FinalizeQuery(Query &aQuery, Error aError);
    void        FinalizeQuery(Response &Response, QueryType aType, Error aError);
    static void GetCallback(const Query &aQuery, Callback &aCallback, void *&aContext);
    Error       AppendNameFromQuery(const Query &aQuery, Message &aMessage);
    Query *     FindQueryById(uint16_t aMessageId);
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMsgInfo);
    void        ProcessResponse(const Message &aMessage);
    Error       ParseResponse(Response &aResponse, QueryType &aType, Error &aResponseError);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    Error CheckAddressResponse(Response &aResponse, Error aResponseError) const;
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    void UpdateDefaultConfigAddress(void);
#endif

    static const uint8_t   kQuestionCount[];
    static const uint16_t *kQuestionRecordTypes[];

    static const uint16_t kIp6AddressQueryRecordTypes[];
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    static const uint16_t kIp4AddressQueryRecordTypes[];
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    static const uint16_t kBrowseQueryRecordTypes[];
    static const uint16_t kServiceQueryRecordTypes[];
#endif

    Ip6::Udp::Socket mSocket;
    QueryList        mQueries;
    TimerMilli       mTimer;
    QueryConfig      mDefaultConfig;
#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    bool mUserDidSetDefaultAddress;
#endif
};

} // namespace Dns
} // namespace ot

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#endif // DNS_CLIENT_HPP_
