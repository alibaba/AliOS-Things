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

#ifndef DNS_SERVER_HPP_
#define DNS_SERVER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE

#include <openthread/dnssd_server.h>

#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "net/dns_types.hpp"
#include "net/ip6.hpp"
#include "net/netif.hpp"
#include "net/srp_server.hpp"

/**
 * @file
 *   This file includes definitions for the DNS-SD server.
 */

namespace ot {

namespace Srp {
class Server;
}

namespace Dns {
namespace ServiceDiscovery {

/**
 * This class implements DNS-SD server.
 *
 */
class Server : public InstanceLocator, private NonCopyable
{
    friend class Srp::Server;

public:
    /**
     * This enumeration specifies a DNS-SD query type.
     *
     */
    enum DnsQueryType : uint8_t
    {
        kDnsQueryNone        = OT_DNSSD_QUERY_TYPE_NONE,         ///< Service type unspecified.
        kDnsQueryBrowse      = OT_DNSSD_QUERY_TYPE_BROWSE,       ///< Service type browse service.
        kDnsQueryResolve     = OT_DNSSD_QUERY_TYPE_RESOLVE,      ///< Service type resolve service instance.
        kDnsQueryResolveHost = OT_DNSSD_QUERY_TYPE_RESOLVE_HOST, ///< Service type resolve hostname.
    };

    static constexpr uint16_t kPort = OPENTHREAD_CONFIG_DNSSD_SERVER_PORT; ///< The DNS-SD server port.

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit Server(Instance &aInstance);

    /**
     * This method starts the DNS-SD server.
     *
     * @retval kErrorNone     Successfully started the DNS-SD server.
     * @retval kErrorFailed   If failed to open or bind the UDP socket.
     *
     */
    Error Start(void);

    /**
     * This method stops the DNS-SD server.
     *
     */
    void Stop(void);

    /**
     * This method sets DNS-SD query callbacks.
     *
     * @param[in] aSubscribe    A pointer to the callback function to subscribe a service or service instance.
     * @param[in] aUnsubscribe  A pointer to the callback function to unsubscribe a service or service instance.
     * @param[in] aContext      A pointer to the application-specific context.
     *
     */
    void SetQueryCallbacks(otDnssdQuerySubscribeCallback   aSubscribe,
                           otDnssdQueryUnsubscribeCallback aUnsubscribe,
                           void *                          aContext);

    /**
     * This method notifies a discovered service instance.
     *
     * @param[in] aServiceFullName  The null-terminated full service name.
     * @param[in] aInstanceInfo     A reference to the discovered service instance information.
     *
     */
    void HandleDiscoveredServiceInstance(const char *aServiceFullName, const otDnssdServiceInstanceInfo &aInstanceInfo);

    /**
     * This method notifies a discovered host.
     *
     * @param[in] aHostFullName     The null-terminated full host name.
     * @param[in] aHostInfo         A reference to the discovered host information.
     *
     */
    void HandleDiscoveredHost(const char *aHostFullName, const otDnssdHostInfo &aHostInfo);

    /**
     * This method acquires the next query in the server.
     *
     * @param[in] aQuery            The query pointer. Pass nullptr to get the first query.
     *
     * @returns  A pointer to the query or nullptr if no more queries.
     *
     */
    const otDnssdQuery *GetNextQuery(const otDnssdQuery *aQuery) const;

    /**
     * This method acquires the DNS-SD query type and name for a specific query.
     *
     * @param[in]   aQuery            The query pointer.
     * @param[out]  aNameOutput       The name output buffer.
     *
     * @returns The DNS-SD query type.
     *
     */
    static DnsQueryType GetQueryTypeAndName(const otDnssdQuery *aQuery, char (&aName)[Name::kMaxNameSize]);

private:
    class NameCompressInfo : public Clearable<NameCompressInfo>
    {
    public:
        explicit NameCompressInfo(void) = default;

        explicit NameCompressInfo(const char *aDomainName)
            : mDomainName(aDomainName)
            , mDomainNameOffset(kUnknownOffset)
            , mServiceNameOffset(kUnknownOffset)
            , mInstanceNameOffset(kUnknownOffset)
            , mHostNameOffset(kUnknownOffset)
        {
        }

        static constexpr uint16_t kUnknownOffset = 0; // Unknown offset value (used when offset is not yet set).

        uint16_t GetDomainNameOffset(void) const { return mDomainNameOffset; }

        void SetDomainNameOffset(uint16_t aOffset) { mDomainNameOffset = aOffset; }

        const char *GetDomainName(void) const { return mDomainName; }

        uint16_t GetServiceNameOffset(const Message &aMessage, const char *aServiceName) const
        {
            return MatchCompressedName(aMessage, mServiceNameOffset, aServiceName)
                       ? mServiceNameOffset
                       : static_cast<uint16_t>(kUnknownOffset);
        };

        void SetServiceNameOffset(uint16_t aOffset)
        {
            if (mServiceNameOffset == kUnknownOffset)
            {
                mServiceNameOffset = aOffset;
            }
        }

        uint16_t GetInstanceNameOffset(const Message &aMessage, const char *aName) const
        {
            return MatchCompressedName(aMessage, mInstanceNameOffset, aName) ? mInstanceNameOffset
                                                                             : static_cast<uint16_t>(kUnknownOffset);
        }

        void SetInstanceNameOffset(uint16_t aOffset)
        {
            if (mInstanceNameOffset == kUnknownOffset)
            {
                mInstanceNameOffset = aOffset;
            }
        }

        uint16_t GetHostNameOffset(const Message &aMessage, const char *aName) const
        {
            return MatchCompressedName(aMessage, mHostNameOffset, aName) ? mHostNameOffset
                                                                         : static_cast<uint16_t>(kUnknownOffset);
        }

        void SetHostNameOffset(uint16_t aOffset)
        {
            if (mHostNameOffset == kUnknownOffset)
            {
                mHostNameOffset = aOffset;
            }
        }

    private:
        static bool MatchCompressedName(const Message &aMessage, uint16_t aOffset, const char *aName)
        {
            return aOffset != kUnknownOffset && Name::CompareName(aMessage, aOffset, aName) == kErrorNone;
        }

        const char *mDomainName;         // The serialized domain name.
        uint16_t    mDomainNameOffset;   // Offset of domain name serialization into the response message.
        uint16_t    mServiceNameOffset;  // Offset of service name serialization into the response message.
        uint16_t    mInstanceNameOffset; // Offset of instance name serialization into the response message.
        uint16_t    mHostNameOffset;     // Offset of host name serialization into the response message.
    };

    static constexpr bool     kBindUnspecifiedNetif = OPENTHREAD_CONFIG_DNSSD_SERVER_BIND_UNSPECIFIED_NETIF;
    static constexpr uint8_t  kProtocolLabelLength  = 4;
    static constexpr uint8_t  kSubTypeLabelLength   = 4;
    static constexpr uint16_t kMaxConcurrentQueries = 32;

    // This structure represents the splitting information of a full name.
    struct NameComponentsOffsetInfo
    {
        static constexpr uint8_t kNotPresent = 0xff; // Indicates the component is not present.

        explicit NameComponentsOffsetInfo(void)
            : mDomainOffset(kNotPresent)
            , mProtocolOffset(kNotPresent)
            , mServiceOffset(kNotPresent)
            , mSubTypeOffset(kNotPresent)
            , mInstanceOffset(kNotPresent)
        {
        }

        bool IsServiceInstanceName(void) const { return mInstanceOffset != kNotPresent; }

        bool IsServiceName(void) const { return mServiceOffset != kNotPresent && mInstanceOffset == kNotPresent; }

        bool IsHostName(void) const { return mProtocolOffset == kNotPresent && mDomainOffset != 0; }

        uint8_t mDomainOffset;   // The offset to the beginning of <Domain>.
        uint8_t mProtocolOffset; // The offset to the beginning of <Protocol> (i.e. _tcp or _udp) or `kNotPresent` if
                                 // the name is not a service or instance.
        uint8_t mServiceOffset;  // The offset to the beginning of <Service> or `kNotPresent` if the name is not a
                                 // service or instance.
        uint8_t mSubTypeOffset;  // The offset to the beginning of sub-type label or `kNotPresent` is not a sub-type.
        uint8_t mInstanceOffset; // The offset to the beginning of <Instance> or `kNotPresent` if the name is not a
                                 // instance.
    };

    /**
     * This class contains the compress information for a dns packet.
     *
     */
    class QueryTransaction
    {
    public:
        explicit QueryTransaction(void)
            : mResponseMessage(nullptr)
        {
        }

        void                    Init(const Header &          aResponseHeader,
                                     Message &               aResponseMessage,
                                     const NameCompressInfo &aCompressInfo,
                                     const Ip6::MessageInfo &aMessageInfo);
        bool                    IsValid(void) const { return mResponseMessage != nullptr; }
        const Ip6::MessageInfo &GetMessageInfo(void) const { return mMessageInfo; }
        const Header &          GetResponseHeader(void) const { return mResponseHeader; }
        Header &                GetResponseHeader(void) { return mResponseHeader; }
        const Message &         GetResponseMessage(void) const { return *mResponseMessage; }
        Message &               GetResponseMessage(void) { return *mResponseMessage; }
        TimeMilli               GetStartTime(void) const { return mStartTime; }
        NameCompressInfo &      GetNameCompressInfo(void) { return mCompressInfo; };
        void                    Finalize(Header::Response aResponseMessage, Ip6::Udp::Socket &aSocket);

        Header           mResponseHeader;
        Message *        mResponseMessage;
        NameCompressInfo mCompressInfo;
        Ip6::MessageInfo mMessageInfo;
        TimeMilli        mStartTime;
    };

    static constexpr uint32_t kQueryTimeout = OPENTHREAD_CONFIG_DNSSD_QUERY_TIMEOUT;

    bool        IsRunning(void) const { return mSocket.IsBound(); }
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void ProcessQuery(const Header &aRequestHeader, Message &aRequestMessage, const Ip6::MessageInfo &aMessageInfo);
    static Header::Response AddQuestions(const Header &    aRequestHeader,
                                         const Message &   aRequestMessage,
                                         Header &          aResponseHeader,
                                         Message &         aResponseMessage,
                                         NameCompressInfo &aCompressInfo);
    static Error            AppendQuestion(const char *      aName,
                                           const Question &  aQuestion,
                                           Message &         aMessage,
                                           NameCompressInfo &aCompressInfo);
    static Error            AppendPtrRecord(Message &         aMessage,
                                            const char *      aServiceName,
                                            const char *      aInstanceName,
                                            uint32_t          aTtl,
                                            NameCompressInfo &aCompressInfo);
    static Error            AppendSrvRecord(Message &         aMessage,
                                            const char *      aInstanceName,
                                            const char *      aHostName,
                                            uint32_t          aTtl,
                                            uint16_t          aPriority,
                                            uint16_t          aWeight,
                                            uint16_t          aPort,
                                            NameCompressInfo &aCompressInfo);
    static Error            AppendTxtRecord(Message &         aMessage,
                                            const char *      aInstanceName,
                                            const void *      aTxtData,
                                            uint16_t          aTxtLength,
                                            uint32_t          aTtl,
                                            NameCompressInfo &aCompressInfo);
    static Error            AppendAaaaRecord(Message &           aMessage,
                                             const char *        aHostName,
                                             const Ip6::Address &aAddress,
                                             uint32_t            aTtl,
                                             NameCompressInfo &  aCompressInfo);
    static Error            AppendServiceName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo);
    static Error            AppendInstanceName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo);
    static Error            AppendHostName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo);
    static void             IncResourceRecordCount(Header &aHeader, bool aAdditional);
    static Error            FindNameComponents(const char *aName, const char *aDomain, NameComponentsOffsetInfo &aInfo);
    static Error            FindPreviousLabel(const char *aName, uint8_t &aStart, uint8_t &aStop);
    static void             SendResponse(Header                  aHeader,
                                         Header::Response        aResponseCode,
                                         Message &               aMessage,
                                         const Ip6::MessageInfo &aMessageInfo,
                                         Ip6::Udp::Socket &      aSocket);
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    Header::Response                   ResolveBySrp(Header &                  aResponseHeader,
                                                    Message &                 aResponseMessage,
                                                    Server::NameCompressInfo &aCompressInfo);
    Header::Response                   ResolveQuestionBySrp(const char *      aName,
                                                            const Question &  aQuestion,
                                                            Header &          aResponseHeader,
                                                            Message &         aResponseMessage,
                                                            NameCompressInfo &aCompressInfo,
                                                            bool              aAdditional);
    const Srp::Server::Host *          GetNextSrpHost(const Srp::Server::Host *aHost);
    static const Srp::Server::Service *GetNextSrpService(const Srp::Server::Host &   aHost,
                                                         const Srp::Server::Service *aService);
#endif

    Error             ResolveByQueryCallbacks(Header &                aResponseHeader,
                                              Message &               aResponseMessage,
                                              NameCompressInfo &      aCompressInfo,
                                              const Ip6::MessageInfo &aMessageInfo);
    QueryTransaction *NewQuery(const Header &          aResponseHeader,
                               Message &               aResponseMessage,
                               const NameCompressInfo &aCompressInfo,
                               const Ip6::MessageInfo &aMessageInfo);
    static bool       CanAnswerQuery(const QueryTransaction &          aQuery,
                                     const char *                      aServiceFullName,
                                     const otDnssdServiceInstanceInfo &aInstanceInfo);
    void              AnswerQuery(QueryTransaction &                aQuery,
                                  const char *                      aServiceFullName,
                                  const otDnssdServiceInstanceInfo &aInstanceInfo);
    static bool       CanAnswerQuery(const Server::QueryTransaction &aQuery, const char *aHostFullName);
    void AnswerQuery(QueryTransaction &aQuery, const char *aHostFullName, const otDnssdHostInfo &aHostInfo);
    void FinalizeQuery(QueryTransaction &aQuery, Header::Response aResponseCode);
    static DnsQueryType GetQueryTypeAndName(const Header & aHeader,
                                            const Message &aMessage,
                                            char (&aName)[Name::kMaxNameSize]);
    static bool HasQuestion(const Header &aHeader, const Message &aMessage, const char *aName, uint16_t aQuestionType);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    void        ResetTimer(void);

    static const char kDnssdProtocolUdp[4];
    static const char kDnssdProtocolTcp[4];
    static const char kDnssdSubTypeLabel[];
    static const char kDefaultDomainName[];
    Ip6::Udp::Socket  mSocket;

    QueryTransaction                mQueryTransactions[kMaxConcurrentQueries];
    void *                          mQueryCallbackContext;
    otDnssdQuerySubscribeCallback   mQuerySubscribe;
    otDnssdQueryUnsubscribeCallback mQueryUnsubscribe;
    TimerMilli                      mTimer;
};

} // namespace ServiceDiscovery
} // namespace Dns
} // namespace ot

#endif // OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE

#endif // DNS_SERVER_HPP_
