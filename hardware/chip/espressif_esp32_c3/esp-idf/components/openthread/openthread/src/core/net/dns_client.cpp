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

#include "dns_client.hpp"

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "net/udp6.hpp"
#include "thread/network_data_types.hpp"
#include "thread/thread_netif.hpp"

/**
 * @file
 *   This file implements the DNS client.
 */

namespace ot {
namespace Dns {

//---------------------------------------------------------------------------------------------------------------------
// Client::QueryConfig

const char Client::QueryConfig::kDefaultServerAddressString[] = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS;

Client::QueryConfig::QueryConfig(InitMode aMode)
{
    OT_UNUSED_VARIABLE(aMode);

    IgnoreError(GetServerSockAddr().GetAddress().FromString(kDefaultServerAddressString));
    GetServerSockAddr().SetPort(kDefaultServerPort);
    SetResponseTimeout(kDefaultResponseTimeout);
    SetMaxTxAttempts(kDefaultMaxTxAttempts);
    SetRecursionFlag(kDefaultRecursionDesired ? kFlagRecursionDesired : kFlagNoRecursion);
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    SetNat64Mode(kDefaultNat64Allowed ? kNat64Allow : kNat64Disallow);
#endif
}

void Client::QueryConfig::SetFrom(const QueryConfig &aConfig, const QueryConfig &aDefaultConfig)
{
    // This method sets the config from `aConfig` replacing any
    // unspecified fields (value zero) with the fields from
    // `aDefaultConfig`.

    *this = aConfig;

    if (GetServerSockAddr().GetAddress().IsUnspecified())
    {
        GetServerSockAddr().GetAddress() = aDefaultConfig.GetServerSockAddr().GetAddress();
    }

    if (GetServerSockAddr().GetPort() == 0)
    {
        GetServerSockAddr().SetPort(aDefaultConfig.GetServerSockAddr().GetPort());
    }

    if (GetResponseTimeout() == 0)
    {
        SetResponseTimeout(aDefaultConfig.GetResponseTimeout());
    }

    if (GetMaxTxAttempts() == 0)
    {
        SetMaxTxAttempts(aDefaultConfig.GetMaxTxAttempts());
    }

    if (GetRecursionFlag() == kFlagUnspecified)
    {
        SetRecursionFlag(aDefaultConfig.GetRecursionFlag());
    }

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    if (GetNat64Mode() == kNat64Unspecified)
    {
        SetNat64Mode(aDefaultConfig.GetNat64Mode());
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
// Client::Response

void Client::Response::SelectSection(Section aSection, uint16_t &aOffset, uint16_t &aNumRecord) const
{
    switch (aSection)
    {
    case kAnswerSection:
        aOffset    = mAnswerOffset;
        aNumRecord = mAnswerRecordCount;
        break;
    case kAdditionalDataSection:
    default:
        aOffset    = mAdditionalOffset;
        aNumRecord = mAdditionalRecordCount;
        break;
    }
}

Error Client::Response::GetName(char *aNameBuffer, uint16_t aNameBufferSize) const
{
    uint16_t offset = kNameOffsetInQuery;

    return Name::ReadName(*mQuery, offset, aNameBuffer, aNameBufferSize);
}

Error Client::Response::CheckForHostNameAlias(Section aSection, Name &aHostName) const
{
    // If the response includes a CNAME record mapping the query host
    // name to a canonical name, we update `aHostName` to the new alias
    // name. Otherwise `aHostName` remains as before.

    Error       error;
    uint16_t    offset;
    uint16_t    numRecords;
    CnameRecord cnameRecord;

    VerifyOrExit(mMessage != nullptr, error = kErrorNotFound);

    SelectSection(aSection, offset, numRecords);
    error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, /* aIndex */ 0, aHostName, cnameRecord);

    switch (error)
    {
    case kErrorNone:
        // A CNAME record was found. `offset` now points to after the
        // last read byte within the `mMessage` into the `cnameRecord`
        // (which is the start of the new canonical name).
        aHostName.SetFromMessage(*mMessage, offset);
        error = Name::ParseName(*mMessage, offset);
        break;

    case kErrorNotFound:
        error = kErrorNone;
        break;

    default:
        break;
    }

exit:
    return error;
}

Error Client::Response::FindHostAddress(Section       aSection,
                                        const Name &  aHostName,
                                        uint16_t      aIndex,
                                        Ip6::Address &aAddress,
                                        uint32_t &    aTtl) const
{
    Error      error;
    uint16_t   offset;
    uint16_t   numRecords;
    Name       name = aHostName;
    AaaaRecord aaaaRecord;

    SuccessOrExit(error = CheckForHostNameAlias(aSection, name));

    SelectSection(aSection, offset, numRecords);
    SuccessOrExit(error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, aIndex, name, aaaaRecord));
    aAddress = aaaaRecord.GetAddress();
    aTtl     = aaaaRecord.GetTtl();

exit:
    return error;
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

Error Client::Response::FindARecord(Section aSection, const Name &aHostName, uint16_t aIndex, ARecord &aARecord) const
{
    Error    error;
    uint16_t offset;
    uint16_t numRecords;
    Name     name = aHostName;

    SuccessOrExit(error = CheckForHostNameAlias(aSection, name));

    SelectSection(aSection, offset, numRecords);
    error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, aIndex, name, aARecord);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

Error Client::Response::FindServiceInfo(Section aSection, const Name &aName, ServiceInfo &aServiceInfo) const
{
    // This method searches for SRV and TXT records in the given
    // section matching the record name against `aName`, and updates
    // the `aServiceInfo` accordingly. It also searches for AAAA
    // record for host name associated with the service (from SRV
    // record). The search for AAAA record is always performed in
    // Additional Data section (independent of the value given in
    // `aSection`).

    Error     error;
    uint16_t  offset;
    uint16_t  numRecords;
    Name      hostName;
    SrvRecord srvRecord;
    TxtRecord txtRecord;

    VerifyOrExit(mMessage != nullptr, error = kErrorNotFound);

    // Search for a matching SRV record
    SelectSection(aSection, offset, numRecords);
    SuccessOrExit(error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, /* aIndex */ 0, aName, srvRecord));

    aServiceInfo.mTtl      = srvRecord.GetTtl();
    aServiceInfo.mPort     = srvRecord.GetPort();
    aServiceInfo.mPriority = srvRecord.GetPriority();
    aServiceInfo.mWeight   = srvRecord.GetWeight();

    hostName.SetFromMessage(*mMessage, offset);

    if (aServiceInfo.mHostNameBuffer != nullptr)
    {
        SuccessOrExit(error = srvRecord.ReadTargetHostName(*mMessage, offset, aServiceInfo.mHostNameBuffer,
                                                           aServiceInfo.mHostNameBufferSize));
    }
    else
    {
        SuccessOrExit(error = Name::ParseName(*mMessage, offset));
    }

    // Search in additional section for AAAA record for the host name.

    error = FindHostAddress(kAdditionalDataSection, hostName, /* aIndex */ 0,
                            static_cast<Ip6::Address &>(aServiceInfo.mHostAddress), aServiceInfo.mHostAddressTtl);

    if (error == kErrorNotFound)
    {
        static_cast<Ip6::Address &>(aServiceInfo.mHostAddress).Clear();
        aServiceInfo.mHostAddressTtl = 0;
    }
    else
    {
        SuccessOrExit(error);
    }

    // A null `mTxtData` indicates that caller does not want to retrieve TXT data.
    VerifyOrExit(aServiceInfo.mTxtData != nullptr);

    // Search for a matching TXT record. If not found, indicate this by
    // setting `aServiceInfo.mTxtDataSize` to zero.

    SelectSection(aSection, offset, numRecords);
    error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, /* aIndex */ 0, aName, txtRecord);

    switch (error)
    {
    case kErrorNone:
        SuccessOrExit(error =
                          txtRecord.ReadTxtData(*mMessage, offset, aServiceInfo.mTxtData, aServiceInfo.mTxtDataSize));
        aServiceInfo.mTxtDataTtl = txtRecord.GetTtl();
        break;

    case kErrorNotFound:
        aServiceInfo.mTxtDataSize = 0;
        aServiceInfo.mTxtDataTtl  = 0;
        break;

    default:
        ExitNow();
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Client::AddressResponse

Error Client::AddressResponse::GetAddress(uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const
{
    Error error = kErrorNone;
    Name  name(*mQuery, kNameOffsetInQuery);

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

    // If the response is for an IPv4 address query or if it is an
    // IPv6 address query response with no IPv6 address but with
    // an IPv4 in its additional section, we read the IPv4 address
    // and translate it to an IPv6 address.

    QueryInfo info;

    info.ReadFrom(*mQuery);

    if ((info.mQueryType == kIp4AddressQuery) || mIp6QueryResponseRequiresNat64)
    {
        Section     section;
        ARecord     aRecord;
        Ip6::Prefix nat64Prefix;

        SuccessOrExit(error = GetNat64Prefix(nat64Prefix));

        section = (info.mQueryType == kIp4AddressQuery) ? kAnswerSection : kAdditionalDataSection;
        SuccessOrExit(error = FindARecord(section, name, aIndex, aRecord));

        aAddress.SynthesizeFromIp4Address(nat64Prefix, aRecord.GetAddress());
        aTtl = aRecord.GetTtl();

        ExitNow();
    }

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

    ExitNow(error = FindHostAddress(kAnswerSection, name, aIndex, aAddress, aTtl));

exit:
    return error;
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

Error Client::AddressResponse::GetNat64Prefix(Ip6::Prefix &aPrefix) const
{
    Error                            error      = kErrorNotFound;
    NetworkData::Iterator            iterator   = NetworkData::kIteratorInit;
    signed int                       preference = NetworkData::kRoutePreferenceLow;
    NetworkData::ExternalRouteConfig config;

    aPrefix.Clear();

    while (mInstance->Get<NetworkData::Leader>().GetNextExternalRoute(iterator, config) == kErrorNone)
    {
        if (!config.mNat64 || !config.GetPrefix().IsValidNat64())
        {
            continue;
        }

        if ((aPrefix.GetLength() == 0) || (config.mPreference > preference))
        {
            aPrefix    = config.GetPrefix();
            preference = config.mPreference;
            error      = kErrorNone;
        }
    }

    return error;
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Client::BrowseResponse

Error Client::BrowseResponse::GetServiceInstance(uint16_t aIndex, char *aLabelBuffer, uint8_t aLabelBufferSize) const
{
    Error     error;
    uint16_t  offset;
    uint16_t  numRecords;
    Name      serviceName(*mQuery, kNameOffsetInQuery);
    PtrRecord ptrRecord;

    VerifyOrExit(mMessage != nullptr, error = kErrorNotFound);

    SelectSection(kAnswerSection, offset, numRecords);
    SuccessOrExit(error = ResourceRecord::FindRecord(*mMessage, offset, numRecords, aIndex, serviceName, ptrRecord));
    error = ptrRecord.ReadPtrName(*mMessage, offset, aLabelBuffer, aLabelBufferSize, nullptr, 0);

exit:
    return error;
}

Error Client::BrowseResponse::GetServiceInfo(const char *aInstanceLabel, ServiceInfo &aServiceInfo) const
{
    Error error;
    Name  instanceName;

    // Find a matching PTR record for the service instance label.
    // Then search and read SRV, TXT and AAAA records in Additional Data section
    // matching the same name to populate `aServiceInfo`.

    SuccessOrExit(error = FindPtrRecord(aInstanceLabel, instanceName));
    error = FindServiceInfo(kAdditionalDataSection, instanceName, aServiceInfo);

exit:
    return error;
}

Error Client::BrowseResponse::GetHostAddress(const char *  aHostName,
                                             uint16_t      aIndex,
                                             Ip6::Address &aAddress,
                                             uint32_t &    aTtl) const
{
    return FindHostAddress(kAdditionalDataSection, Name(aHostName), aIndex, aAddress, aTtl);
}

Error Client::BrowseResponse::FindPtrRecord(const char *aInstanceLabel, Name &aInstanceName) const
{
    // This method searches within the Answer Section for a PTR record
    // matching a given instance label @aInstanceLabel. If found, the
    // `aName` is updated to return the name in the message.

    Error     error;
    uint16_t  offset;
    Name      serviceName(*mQuery, kNameOffsetInQuery);
    uint16_t  numRecords;
    uint16_t  labelOffset;
    PtrRecord ptrRecord;

    VerifyOrExit(mMessage != nullptr, error = kErrorNotFound);

    SelectSection(kAnswerSection, offset, numRecords);

    for (; numRecords > 0; numRecords--)
    {
        SuccessOrExit(error = Name::CompareName(*mMessage, offset, serviceName));

        error = ResourceRecord::ReadRecord(*mMessage, offset, ptrRecord);

        if (error == kErrorNotFound)
        {
            // `ReadRecord()` updates `offset` to skip over a
            // non-matching record.
            continue;
        }

        SuccessOrExit(error);

        // It is a PTR record. Check the first label to match the
        // instance label.

        labelOffset = offset;
        error       = Name::CompareLabel(*mMessage, labelOffset, aInstanceLabel);

        if (error == kErrorNone)
        {
            aInstanceName.SetFromMessage(*mMessage, offset);
            ExitNow();
        }

        VerifyOrExit(error == kErrorNotFound);

        // Update offset to skip over the PTR record.
        offset += static_cast<uint16_t>(ptrRecord.GetSize()) - sizeof(ptrRecord);
    }

    error = kErrorNotFound;

exit:
    return error;
}

//---------------------------------------------------------------------------------------------------------------------
// Client::ServiceResponse

Error Client::ServiceResponse::GetServiceName(char *   aLabelBuffer,
                                              uint8_t  aLabelBufferSize,
                                              char *   aNameBuffer,
                                              uint16_t aNameBufferSize) const
{
    Error    error;
    uint16_t offset = kNameOffsetInQuery;

    SuccessOrExit(error = Name::ReadLabel(*mQuery, offset, aLabelBuffer, aLabelBufferSize));

    VerifyOrExit(aNameBuffer != nullptr);
    SuccessOrExit(error = Name::ReadName(*mQuery, offset, aNameBuffer, aNameBufferSize));

exit:
    return error;
}

Error Client::ServiceResponse::GetServiceInfo(ServiceInfo &aServiceInfo) const
{
    // Search and read SRV, TXT records in Answer Section
    // matching name from query.

    return FindServiceInfo(kAnswerSection, Name(*mQuery, kNameOffsetInQuery), aServiceInfo);
}

Error Client::ServiceResponse::GetHostAddress(const char *  aHostName,
                                              uint16_t      aIndex,
                                              Ip6::Address &aAddress,
                                              uint32_t &    aTtl) const
{
    return FindHostAddress(kAdditionalDataSection, Name(aHostName), aIndex, aAddress, aTtl);
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Client

const uint16_t Client::kIp6AddressQueryRecordTypes[] = {ResourceRecord::kTypeAaaa};
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
const uint16_t Client::kIp4AddressQueryRecordTypes[] = {ResourceRecord::kTypeA};
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
const uint16_t Client::kBrowseQueryRecordTypes[]  = {ResourceRecord::kTypePtr};
const uint16_t Client::kServiceQueryRecordTypes[] = {ResourceRecord::kTypeSrv, ResourceRecord::kTypeTxt};
#endif

const uint8_t Client::kQuestionCount[] = {
    /* kIp6AddressQuery -> */ OT_ARRAY_LENGTH(kIp6AddressQueryRecordTypes), // AAAA records
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    /* kIp4AddressQuery -> */ OT_ARRAY_LENGTH(kIp4AddressQueryRecordTypes), // A records
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    /* kBrowseQuery  -> */ OT_ARRAY_LENGTH(kBrowseQueryRecordTypes),  // PTR records
    /* kServiceQuery -> */ OT_ARRAY_LENGTH(kServiceQueryRecordTypes), // SRV and TXT records
#endif
};

const uint16_t *Client::kQuestionRecordTypes[] = {
    /* kIp6AddressQuery -> */ kIp6AddressQueryRecordTypes,
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    /* kIp4AddressQuery -> */ kIp4AddressQueryRecordTypes,
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    /* kBrowseQuery  -> */ kBrowseQueryRecordTypes,
    /* kServiceQuery -> */ kServiceQueryRecordTypes,
#endif
};

Client::Client(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mSocket(aInstance)
    , mTimer(aInstance, Client::HandleTimer)
    , mDefaultConfig(QueryConfig::kInitFromDefaults)
#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    , mUserDidSetDefaultAddress(false)
#endif
{
    static_assert(kIp6AddressQuery == 0, "kIp6AddressQuery value is not correct");
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    static_assert(kIp4AddressQuery == 1, "kIp4AddressQuery value is not correct");
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    static_assert(kBrowseQuery == 2, "kBrowseQuery value is not correct");
    static_assert(kServiceQuery == 3, "kServiceQuery value is not correct");
#endif
#elif OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    static_assert(kBrowseQuery == 1, "kBrowseQuery value is not correct");
    static_assert(kServiceQuery == 2, "kServiceQuery value is not correct");
#endif
}

Error Client::Start(void)
{
    Error error;

    SuccessOrExit(error = mSocket.Open(&Client::HandleUdpReceive, this));
    SuccessOrExit(error = mSocket.Bind(0, OT_NETIF_UNSPECIFIED));

exit:
    return error;
}

void Client::Stop(void)
{
    Query *query;

    while ((query = mQueries.GetHead()) != nullptr)
    {
        FinalizeQuery(*query, kErrorAbort);
    }

    IgnoreError(mSocket.Close());
}

void Client::SetDefaultConfig(const QueryConfig &aQueryConfig)
{
    QueryConfig startingDefault(QueryConfig::kInitFromDefaults);

    mDefaultConfig.SetFrom(aQueryConfig, startingDefault);

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    mUserDidSetDefaultAddress = !aQueryConfig.GetServerSockAddr().GetAddress().IsUnspecified();
    UpdateDefaultConfigAddress();
#endif
}

void Client::ResetDefaultConfig(void)
{
    mDefaultConfig = QueryConfig(QueryConfig::kInitFromDefaults);

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    mUserDidSetDefaultAddress = false;
    UpdateDefaultConfigAddress();
#endif
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
void Client::UpdateDefaultConfigAddress(void)
{
    const Ip6::Address &srpServerAddr = Get<Srp::Client>().GetServerAddress().GetAddress();

    if (!mUserDidSetDefaultAddress && Get<Srp::Client>().IsServerSelectedByAutoStart() &&
        !srpServerAddr.IsUnspecified())
    {
        mDefaultConfig.GetServerSockAddr().SetAddress(srpServerAddr);
    }
}
#endif

Error Client::ResolveAddress(const char *       aHostName,
                             AddressCallback    aCallback,
                             void *             aContext,
                             const QueryConfig *aConfig)
{
    QueryInfo info;

    info.Clear();
    info.mQueryType                 = kIp6AddressQuery;
    info.mCallback.mAddressCallback = aCallback;

    return StartQuery(info, aConfig, nullptr, aHostName, aContext);
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

Error Client::Browse(const char *aServiceName, BrowseCallback aCallback, void *aContext, const QueryConfig *aConfig)
{
    QueryInfo info;

    info.Clear();
    info.mQueryType                = kBrowseQuery;
    info.mCallback.mBrowseCallback = aCallback;

    return StartQuery(info, aConfig, nullptr, aServiceName, aContext);
}

Error Client::ResolveService(const char *       aInstanceLabel,
                             const char *       aServiceName,
                             ServiceCallback    aCallback,
                             void *             aContext,
                             const QueryConfig *aConfig)
{
    QueryInfo info;
    Error     error;

    VerifyOrExit(aInstanceLabel != nullptr, error = kErrorInvalidArgs);

    info.Clear();
    info.mQueryType                 = kServiceQuery;
    info.mCallback.mServiceCallback = aCallback;

    error = StartQuery(info, aConfig, aInstanceLabel, aServiceName, aContext);

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

Error Client::StartQuery(QueryInfo &        aInfo,
                         const QueryConfig *aConfig,
                         const char *       aLabel,
                         const char *       aName,
                         void *             aContext)
{
    // This method assumes that `mQueryType` and `mCallback` to be
    // already set by caller on `aInfo`. The `aLabel` can be `nullptr`
    // and then `aName` provides the full name, otherwise the name is
    // appended as `{aLabel}.{aName}`.

    Error  error;
    Query *query;

    VerifyOrExit(mSocket.IsBound(), error = kErrorInvalidState);

    if (aConfig == nullptr)
    {
        aInfo.mConfig = mDefaultConfig;
    }
    else
    {
        // To form the config for this query, replace any unspecified
        // fields (zero value) in the given `aConfig` with the fields
        // from `mDefaultConfig`.

        aInfo.mConfig.SetFrom(*aConfig, mDefaultConfig);
    }

    aInfo.mCallbackContext = aContext;

    SuccessOrExit(error = AllocateQuery(aInfo, aLabel, aName, query));
    mQueries.Enqueue(*query);

    SendQuery(*query, aInfo, /* aUpdateTimer */ true);

exit:
    return error;
}

Error Client::AllocateQuery(const QueryInfo &aInfo, const char *aLabel, const char *aName, Query *&aQuery)
{
    Error error = kErrorNone;

    aQuery = Get<MessagePool>().New(Message::kTypeOther, /* aReserveHeader */ 0);
    VerifyOrExit(aQuery != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = aQuery->Append(aInfo));

    if (aLabel != nullptr)
    {
        SuccessOrExit(error = Name::AppendLabel(aLabel, *aQuery));
    }

    SuccessOrExit(error = Name::AppendName(aName, *aQuery));

exit:
    FreeAndNullMessageOnError(aQuery, error);
    return error;
}

void Client::FreeQuery(Query &aQuery)
{
    mQueries.DequeueAndFree(aQuery);
}

void Client::SendQuery(Query &aQuery, QueryInfo &aInfo, bool aUpdateTimer)
{
    // This method prepares and sends a query message represented by
    // `aQuery` and `aInfo`. This method updates `aInfo` (e.g., sets
    // the new `mRetransmissionTime`) and updates it in `aQuery` as
    // well. `aUpdateTimer` indicates whether the timer should be
    // updated when query is sent or not (used in the case where timer
    // is handled by caller).

    Error            error   = kErrorNone;
    Message *        message = nullptr;
    Header           header;
    Ip6::MessageInfo messageInfo;

    aInfo.mTransmissionCount++;
    aInfo.mRetransmissionTime = TimerMilli::GetNow() + aInfo.mConfig.GetResponseTimeout();

    if (aInfo.mMessageId == 0)
    {
        do
        {
            SuccessOrExit(error = header.SetRandomMessageId());
        } while ((header.GetMessageId() == 0) || (FindQueryById(header.GetMessageId()) != nullptr));

        aInfo.mMessageId = header.GetMessageId();
    }
    else
    {
        header.SetMessageId(aInfo.mMessageId);
    }

    header.SetType(Header::kTypeQuery);
    header.SetQueryType(Header::kQueryTypeStandard);

    if (aInfo.mConfig.GetRecursionFlag() == QueryConfig::kFlagRecursionDesired)
    {
        header.SetRecursionDesiredFlag();
    }

    header.SetQuestionCount(kQuestionCount[aInfo.mQueryType]);

    message = mSocket.NewMessage(0);
    VerifyOrExit(message != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->Append(header));

    // Prepare the question section.

    for (uint8_t num = 0; num < kQuestionCount[aInfo.mQueryType]; num++)
    {
        SuccessOrExit(error = AppendNameFromQuery(aQuery, *message));
        SuccessOrExit(error = message->Append(Question(kQuestionRecordTypes[aInfo.mQueryType][num])));
    }

    messageInfo.SetPeerAddr(aInfo.mConfig.GetServerSockAddr().GetAddress());
    messageInfo.SetPeerPort(aInfo.mConfig.GetServerSockAddr().GetPort());

    SuccessOrExit(error = mSocket.SendTo(*message, messageInfo));

exit:
    FreeMessageOnError(message, error);

    UpdateQuery(aQuery, aInfo);

    if (aUpdateTimer)
    {
        mTimer.FireAtIfEarlier(aInfo.mRetransmissionTime);
    }
}

Error Client::AppendNameFromQuery(const Query &aQuery, Message &aMessage)
{
    Error    error = kErrorNone;
    uint16_t offset;
    uint16_t length;

    // The name is encoded and included after the `Info` in `aQuery`. We
    // first calculate the encoded length of the name, then grow the
    // message, and finally copy the encoded name bytes from `aQuery`
    // into `aMessage`.

    length = aQuery.GetLength() - kNameOffsetInQuery;

    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.SetLength(offset + length));

    aQuery.CopyTo(/* aSourceOffset */ kNameOffsetInQuery, /* aDestOffset */ offset, length, aMessage);

exit:
    return error;
}

void Client::FinalizeQuery(Query &aQuery, Error aError)
{
    Response  response;
    QueryInfo info;

    response.mInstance = &Get<Instance>();
    response.mQuery    = &aQuery;
    info.ReadFrom(aQuery);

    FinalizeQuery(response, info.mQueryType, aError);
}

void Client::FinalizeQuery(Response &aResponse, QueryType aType, Error aError)
{
    Callback callback;
    void *   context;

    GetCallback(*aResponse.mQuery, callback, context);

    switch (aType)
    {
    case kIp6AddressQuery:
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    case kIp4AddressQuery:
#endif
        if (callback.mAddressCallback != nullptr)
        {
            callback.mAddressCallback(aError, &aResponse, context);
        }
        break;

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    case kBrowseQuery:
        if (callback.mBrowseCallback != nullptr)
        {
            callback.mBrowseCallback(aError, &aResponse, context);
        }
        break;

    case kServiceQuery:
        if (callback.mServiceCallback != nullptr)
        {
            callback.mServiceCallback(aError, &aResponse, context);
        }
        break;
#endif
    }

    FreeQuery(*aResponse.mQuery);
}

void Client::GetCallback(const Query &aQuery, Callback &aCallback, void *&aContext)
{
    QueryInfo info;

    info.ReadFrom(aQuery);

    aCallback = info.mCallback;
    aContext  = info.mCallbackContext;
}

Client::Query *Client::FindQueryById(uint16_t aMessageId)
{
    Query *   query;
    QueryInfo info;

    for (query = mQueries.GetHead(); query != nullptr; query = query->GetNext())
    {
        info.ReadFrom(*query);

        if (info.mMessageId == aMessageId)
        {
            break;
        }
    }

    return query;
}

void Client::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMsgInfo)
{
    OT_UNUSED_VARIABLE(aMsgInfo);

    static_cast<Client *>(aContext)->ProcessResponse(*static_cast<Message *>(aMessage));
}

void Client::ProcessResponse(const Message &aMessage)
{
    Response  response;
    QueryType type;
    Error     responseError;

    response.mInstance = &Get<Instance>();
    response.mMessage  = &aMessage;

    // We intentionally parse the response in a separate method
    // `ParseResponse()` to free all the stack allocated variables
    // (e.g., `QueryInfo`) used during parsing of the message before
    // finalizing the query and invoking the user's callback.

    SuccessOrExit(ParseResponse(response, type, responseError));
    FinalizeQuery(response, type, responseError);

exit:
    return;
}

Error Client::ParseResponse(Response &aResponse, QueryType &aType, Error &aResponseError)
{
    Error          error   = kErrorNone;
    const Message &message = *aResponse.mMessage;
    uint16_t       offset  = message.GetOffset();
    Header         header;
    QueryInfo      info;
    Name           queryName;

    SuccessOrExit(error = message.Read(offset, header));
    offset += sizeof(Header);

    VerifyOrExit((header.GetType() == Header::kTypeResponse) && (header.GetQueryType() == Header::kQueryTypeStandard) &&
                     !header.IsTruncationFlagSet(),
                 error = kErrorDrop);

    aResponse.mQuery = FindQueryById(header.GetMessageId());
    VerifyOrExit(aResponse.mQuery != nullptr, error = kErrorNotFound);

    info.ReadFrom(*aResponse.mQuery);
    aType = info.mQueryType;

    queryName.SetFromMessage(*aResponse.mQuery, kNameOffsetInQuery);

    // Check the Question Section

    VerifyOrExit(header.GetQuestionCount() == kQuestionCount[aType], error = kErrorParse);

    for (uint8_t num = 0; num < kQuestionCount[aType]; num++)
    {
        SuccessOrExit(error = Name::CompareName(message, offset, queryName));
        offset += sizeof(Question);
    }

    // Check the answer, authority and additional record sections

    aResponse.mAnswerOffset = offset;
    SuccessOrExit(error = ResourceRecord::ParseRecords(message, offset, header.GetAnswerCount()));
    SuccessOrExit(error = ResourceRecord::ParseRecords(message, offset, header.GetAuthorityRecordCount()));
    aResponse.mAdditionalOffset = offset;
    SuccessOrExit(error = ResourceRecord::ParseRecords(message, offset, header.GetAdditionalRecordCount()));

    aResponse.mAnswerRecordCount     = header.GetAnswerCount();
    aResponse.mAdditionalRecordCount = header.GetAdditionalRecordCount();

    // Check the response code from server

    aResponseError = Header::ResponseCodeToError(header.GetResponseCode());

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

    if (aType == kIp6AddressQuery)
    {
        Ip6::Address ip6ddress;
        uint32_t     ttl;
        ARecord      aRecord;

        // If the response does not contain an answer for the IPv6 address
        // resolution query and if NAT64 is allowed for this query, we can
        // perform IPv4 to IPv6 address translation.

        VerifyOrExit(aResponse.FindHostAddress(Response::kAnswerSection, queryName, /* aIndex */ 0, ip6ddress, ttl) !=
                     kErrorNone);
        VerifyOrExit(info.mConfig.GetNat64Mode() == QueryConfig::kNat64Allow);

        // First, we check if the response already contains an A record
        // (IPv4 address) for the query name.

        if (aResponse.FindARecord(Response::kAdditionalDataSection, queryName, /* aIndex */ 0, aRecord) == kErrorNone)
        {
            aResponse.mIp6QueryResponseRequiresNat64 = true;
            aResponseError                           = kErrorNone;
            ExitNow();
        }

        // Otherwise, we send a new query for IPv4 address resolution
        // for the same host name. We reuse the existing `query`
        // instance and keep all the info but clear `mTransmissionCount`
        // and `mMessageId` (so that a new random message ID is
        // selected). The new `info` will be saved in the query in
        // `SendQuery()`. Note that the current query is still in the
        // `mQueries` list when `SendQuery()` selects a new random
        // message ID, so the existing message ID for this query will
        // not be reused. Since the query is not yet resolved, we
        // return `kErrorPending`.

        info.mQueryType         = kIp4AddressQuery;
        info.mMessageId         = 0;
        info.mTransmissionCount = 0;

        SendQuery(*aResponse.mQuery, info, /* aUpdateTimer */ true);

        error = kErrorPending;
    }

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE

exit:
    if (error != kErrorNone)
    {
        otLogInfoDns("Failed to parse response %s", ErrorToString(error));
    }

    return error;
}

void Client::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Client>().HandleTimer();
}

void Client::HandleTimer(void)
{
    TimeMilli now      = TimerMilli::GetNow();
    TimeMilli nextTime = now.GetDistantFuture();
    Query *   nextQuery;
    QueryInfo info;

    for (Query *query = mQueries.GetHead(); query != nullptr; query = nextQuery)
    {
        nextQuery = query->GetNext();

        info.ReadFrom(*query);

        if (now >= info.mRetransmissionTime)
        {
            if (info.mTransmissionCount >= info.mConfig.GetMaxTxAttempts())
            {
                FinalizeQuery(*query, kErrorResponseTimeout);
                continue;
            }

            SendQuery(*query, info, /* aUpdateTimer */ false);
        }

        if (nextTime > info.mRetransmissionTime)
        {
            nextTime = info.mRetransmissionTime;
        }
    }

    if (nextTime < now.GetDistantFuture())
    {
        mTimer.FireAt(nextTime);
    }
}

} // namespace Dns
} // namespace ot

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
