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
 *   This file implements the DNS-SD server.
 */

#include "dnssd_server.hpp"

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "net/srp_server.hpp"
#include "net/udp6.hpp"

namespace ot {
namespace Dns {
namespace ServiceDiscovery {

const char Server::kDnssdProtocolUdp[4] = {'_', 'u', 'd', 'p'};
const char Server::kDnssdProtocolTcp[4] = {'_', 't', 'c', 'p'};
const char Server::kDnssdSubTypeLabel[] = "._sub.";
const char Server::kDefaultDomainName[] = "default.service.arpa.";

Server::Server(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mSocket(aInstance)
    , mQueryCallbackContext(nullptr)
    , mQuerySubscribe(nullptr)
    , mQueryUnsubscribe(nullptr)
    , mTimer(aInstance, Server::HandleTimer)
{
}

Error Server::Start(void)
{
    Error error = kErrorNone;

    VerifyOrExit(!IsRunning());

    SuccessOrExit(error = mSocket.Open(&Server::HandleUdpReceive, this));
    SuccessOrExit(error = mSocket.Bind(kPort, kBindUnspecifiedNetif ? OT_NETIF_UNSPECIFIED : OT_NETIF_THREAD));

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    Get<Srp::Server>().HandleDnssdServerStateChange();
#endif

exit:
    otLogInfoDns("[server] started: %s", ErrorToString(error));

    if (error != kErrorNone)
    {
        IgnoreError(mSocket.Close());
    }

    return error;
}

void Server::Stop(void)
{
    // Abort all query transactions
    for (QueryTransaction &query : mQueryTransactions)
    {
        if (query.IsValid())
        {
            FinalizeQuery(query, Header::kResponseServerFailure);
        }
    }

    mTimer.Stop();

    IgnoreError(mSocket.Close());
    otLogInfoDns("[server] stopped");

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    Get<Srp::Server>().HandleDnssdServerStateChange();
#endif
}

void Server::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Server *>(aContext)->HandleUdpReceive(*static_cast<Message *>(aMessage),
                                                      *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Server::HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Header requestHeader;

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    // We first let the `Srp::Server` process the received message.
    // It returns `kErrorNone` to indicate that it successfully
    // processed the message.

    VerifyOrExit(Get<Srp::Server>().HandleDnssdServerUdpReceive(aMessage, aMessageInfo) != kErrorNone);
#endif

    SuccessOrExit(aMessage.Read(aMessage.GetOffset(), requestHeader));
    VerifyOrExit(requestHeader.GetType() == Header::kTypeQuery);

    ProcessQuery(requestHeader, aMessage, aMessageInfo);

exit:
    return;
}

void Server::ProcessQuery(const Header &aRequestHeader, Message &aRequestMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error            error           = kErrorNone;
    Message *        responseMessage = nullptr;
    Header           responseHeader;
    NameCompressInfo compressInfo(kDefaultDomainName);
    Header::Response response                = Header::kResponseSuccess;
    bool             resolveByQueryCallbacks = false;

    responseMessage = mSocket.NewMessage(0);
    VerifyOrExit(responseMessage != nullptr, error = kErrorNoBufs);

    // Allocate space for DNS header
    SuccessOrExit(error = responseMessage->SetLength(sizeof(Header)));

    // Setup initial DNS response header
    responseHeader.Clear();
    responseHeader.SetType(Header::kTypeResponse);
    responseHeader.SetMessageId(aRequestHeader.GetMessageId());

    // Validate the query
    VerifyOrExit(aRequestHeader.GetQueryType() == Header::kQueryTypeStandard,
                 response = Header::kResponseNotImplemented);
    VerifyOrExit(!aRequestHeader.IsTruncationFlagSet(), response = Header::kResponseFormatError);
    VerifyOrExit(aRequestHeader.GetQuestionCount() > 0, response = Header::kResponseFormatError);

    response = AddQuestions(aRequestHeader, aRequestMessage, responseHeader, *responseMessage, compressInfo);
    VerifyOrExit(response == Header::kResponseSuccess);

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    // Answer the questions
    response = ResolveBySrp(responseHeader, *responseMessage, compressInfo);
#endif

    // Resolve the question using query callbacks if SRP server failed to resolve the questions.
    if (responseHeader.GetAnswerCount() == 0 &&
        kErrorNone == ResolveByQueryCallbacks(responseHeader, *responseMessage, compressInfo, aMessageInfo))
    {
        resolveByQueryCallbacks = true;
    }

exit:
    if (error == kErrorNone && !resolveByQueryCallbacks)
    {
        SendResponse(responseHeader, response, *responseMessage, aMessageInfo, mSocket);
    }

    FreeMessageOnError(responseMessage, error);
}

void Server::SendResponse(Header                  aHeader,
                          Header::Response        aResponseCode,
                          Message &               aMessage,
                          const Ip6::MessageInfo &aMessageInfo,
                          Ip6::Udp::Socket &      aSocket)
{
    Error error;

    if (aResponseCode == Header::kResponseServerFailure)
    {
        otLogWarnDns("[server] failed to handle DNS query due to server failure");
        aHeader.SetQuestionCount(0);
        aHeader.SetAnswerCount(0);
        aHeader.SetAdditionalRecordCount(0);
        IgnoreError(aMessage.SetLength(sizeof(Header)));
    }

    aHeader.SetResponseCode(aResponseCode);
    aMessage.Write(0, aHeader);

    error = aSocket.SendTo(aMessage, aMessageInfo);

    FreeMessageOnError(&aMessage, error);

    if (error != kErrorNone)
    {
        otLogWarnDns("[server] failed to send DNS-SD reply: %s", otThreadErrorToString(error));
    }
    else
    {
        otLogInfoDns("[server] send DNS-SD reply: %s, RCODE=%d", otThreadErrorToString(error), aResponseCode);
    }
}

Header::Response Server::AddQuestions(const Header &    aRequestHeader,
                                      const Message &   aRequestMessage,
                                      Header &          aResponseHeader,
                                      Message &         aResponseMessage,
                                      NameCompressInfo &aCompressInfo)
{
    Question         question;
    uint16_t         readOffset;
    Header::Response response = Header::kResponseSuccess;
    char             name[Name::kMaxNameSize];

    readOffset = sizeof(Header);

    // Check and append the questions
    for (uint16_t i = 0; i < aRequestHeader.GetQuestionCount(); i++)
    {
        NameComponentsOffsetInfo nameComponentsOffsetInfo;
        uint16_t                 qtype;

        VerifyOrExit(kErrorNone == Name::ReadName(aRequestMessage, readOffset, name, sizeof(name)),
                     response = Header::kResponseFormatError);
        VerifyOrExit(kErrorNone == aRequestMessage.Read(readOffset, question), response = Header::kResponseFormatError);
        readOffset += sizeof(question);

        qtype = question.GetType();

        VerifyOrExit(qtype == ResourceRecord::kTypePtr || qtype == ResourceRecord::kTypeSrv ||
                         qtype == ResourceRecord::kTypeTxt || qtype == ResourceRecord::kTypeAaaa,
                     response = Header::kResponseNotImplemented);

        VerifyOrExit(kErrorNone == FindNameComponents(name, aCompressInfo.GetDomainName(), nameComponentsOffsetInfo),
                     response = Header::kResponseNameError);

        switch (question.GetType())
        {
        case ResourceRecord::kTypePtr:
            VerifyOrExit(nameComponentsOffsetInfo.IsServiceName(), response = Header::kResponseNameError);
            break;
        case ResourceRecord::kTypeSrv:
            VerifyOrExit(nameComponentsOffsetInfo.IsServiceInstanceName(), response = Header::kResponseNameError);
            break;
        case ResourceRecord::kTypeTxt:
            VerifyOrExit(nameComponentsOffsetInfo.IsServiceInstanceName(), response = Header::kResponseNameError);
            break;
        case ResourceRecord::kTypeAaaa:
            VerifyOrExit(nameComponentsOffsetInfo.IsHostName(), response = Header::kResponseNameError);
            break;
        default:
            ExitNow(response = Header::kResponseNotImplemented);
        }

        VerifyOrExit(AppendQuestion(name, question, aResponseMessage, aCompressInfo) == kErrorNone,
                     response = Header::kResponseServerFailure);
    }

    aResponseHeader.SetQuestionCount(aRequestHeader.GetQuestionCount());

exit:
    return response;
}

Error Server::AppendQuestion(const char *      aName,
                             const Question &  aQuestion,
                             Message &         aMessage,
                             NameCompressInfo &aCompressInfo)
{
    Error error = kErrorNone;

    switch (aQuestion.GetType())
    {
    case ResourceRecord::kTypePtr:
        SuccessOrExit(error = AppendServiceName(aMessage, aName, aCompressInfo));
        break;
    case ResourceRecord::kTypeSrv:
    case ResourceRecord::kTypeTxt:
        SuccessOrExit(error = AppendInstanceName(aMessage, aName, aCompressInfo));
        break;
    case ResourceRecord::kTypeAaaa:
        SuccessOrExit(error = AppendHostName(aMessage, aName, aCompressInfo));
        break;
    default:
        OT_ASSERT(false);
    }

    error = aMessage.Append(aQuestion);

exit:
    return error;
}

Error Server::AppendPtrRecord(Message &         aMessage,
                              const char *      aServiceName,
                              const char *      aInstanceName,
                              uint32_t          aTtl,
                              NameCompressInfo &aCompressInfo)
{
    Error     error;
    PtrRecord ptrRecord;
    uint16_t  recordOffset;

    ptrRecord.Init();
    ptrRecord.SetTtl(aTtl);

    SuccessOrExit(error = AppendServiceName(aMessage, aServiceName, aCompressInfo));

    recordOffset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.SetLength(recordOffset + sizeof(ptrRecord)));

    SuccessOrExit(error = AppendInstanceName(aMessage, aInstanceName, aCompressInfo));

    ptrRecord.SetLength(aMessage.GetLength() - (recordOffset + sizeof(ResourceRecord)));
    aMessage.Write(recordOffset, ptrRecord);

exit:
    return error;
}

Error Server::AppendSrvRecord(Message &         aMessage,
                              const char *      aInstanceName,
                              const char *      aHostName,
                              uint32_t          aTtl,
                              uint16_t          aPriority,
                              uint16_t          aWeight,
                              uint16_t          aPort,
                              NameCompressInfo &aCompressInfo)
{
    SrvRecord srvRecord;
    Error     error = kErrorNone;
    uint16_t  recordOffset;

    srvRecord.Init();
    srvRecord.SetTtl(aTtl);
    srvRecord.SetPriority(aPriority);
    srvRecord.SetWeight(aWeight);
    srvRecord.SetPort(aPort);

    SuccessOrExit(error = AppendInstanceName(aMessage, aInstanceName, aCompressInfo));

    recordOffset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.SetLength(recordOffset + sizeof(srvRecord)));

    SuccessOrExit(error = AppendHostName(aMessage, aHostName, aCompressInfo));

    srvRecord.SetLength(aMessage.GetLength() - (recordOffset + sizeof(ResourceRecord)));
    aMessage.Write(recordOffset, srvRecord);

exit:
    return error;
}

Error Server::AppendAaaaRecord(Message &           aMessage,
                               const char *        aHostName,
                               const Ip6::Address &aAddress,
                               uint32_t            aTtl,
                               NameCompressInfo &  aCompressInfo)
{
    AaaaRecord aaaaRecord;
    Error      error;

    aaaaRecord.Init();
    aaaaRecord.SetTtl(aTtl);
    aaaaRecord.SetAddress(aAddress);

    SuccessOrExit(error = AppendHostName(aMessage, aHostName, aCompressInfo));
    error = aMessage.Append(aaaaRecord);

exit:
    return error;
}

Error Server::AppendServiceName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo)
{
    Error       error;
    uint16_t    serviceCompressOffset = aCompressInfo.GetServiceNameOffset(aMessage, aName);
    const char *serviceName;

    // Check whether `aName` is a sub-type service name.
    serviceName = StringFind(aName, kDnssdSubTypeLabel);

    if (serviceName != nullptr)
    {
        uint8_t subTypeLabelLength = static_cast<uint8_t>(serviceName - aName) + sizeof(kDnssdSubTypeLabel) - 1;

        SuccessOrExit(error = Name::AppendMultipleLabels(aName, subTypeLabelLength, aMessage));

        // Skip over the "._sub." label to get to the root service name.
        serviceName += sizeof(kDnssdSubTypeLabel) - 1;
    }
    else
    {
        serviceName = aName;
    }

    if (serviceCompressOffset != NameCompressInfo::kUnknownOffset)
    {
        error = Name::AppendPointerLabel(serviceCompressOffset, aMessage);
    }
    else
    {
        uint8_t  domainStart          = static_cast<uint8_t>(StringLength(serviceName, Name::kMaxNameSize - 1) -
                                                   StringLength(aCompressInfo.GetDomainName(), Name::kMaxNameSize - 1));
        uint16_t domainCompressOffset = aCompressInfo.GetDomainNameOffset();

        serviceCompressOffset = aMessage.GetLength();
        aCompressInfo.SetServiceNameOffset(serviceCompressOffset);

        if (domainCompressOffset == NameCompressInfo::kUnknownOffset)
        {
            aCompressInfo.SetDomainNameOffset(serviceCompressOffset + domainStart);
            error = Name::AppendName(serviceName, aMessage);
        }
        else
        {
            SuccessOrExit(error = Name::AppendMultipleLabels(serviceName, domainStart, aMessage));
            error = Name::AppendPointerLabel(domainCompressOffset, aMessage);
        }
    }

exit:
    return error;
}

Error Server::AppendInstanceName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo)
{
    Error    error;
    uint16_t instanceCompressOffset = aCompressInfo.GetInstanceNameOffset(aMessage, aName);

    if (instanceCompressOffset != NameCompressInfo::kUnknownOffset)
    {
        error = Name::AppendPointerLabel(instanceCompressOffset, aMessage);
    }
    else
    {
        NameComponentsOffsetInfo nameComponentsInfo;

        IgnoreError(FindNameComponents(aName, aCompressInfo.GetDomainName(), nameComponentsInfo));
        OT_ASSERT(nameComponentsInfo.IsServiceInstanceName());

        aCompressInfo.SetInstanceNameOffset(aMessage.GetLength());

        // Append the instance name as one label
        SuccessOrExit(error = Name::AppendLabel(aName, nameComponentsInfo.mServiceOffset - 1, aMessage));

        {
            const char *serviceName           = aName + nameComponentsInfo.mServiceOffset;
            uint16_t    serviceCompressOffset = aCompressInfo.GetServiceNameOffset(aMessage, serviceName);

            if (serviceCompressOffset != NameCompressInfo::kUnknownOffset)
            {
                error = Name::AppendPointerLabel(serviceCompressOffset, aMessage);
            }
            else
            {
                aCompressInfo.SetServiceNameOffset(aMessage.GetLength());
                error = Name::AppendName(serviceName, aMessage);
            }
        }
    }

exit:
    return error;
}

Error Server::AppendTxtRecord(Message &         aMessage,
                              const char *      aInstanceName,
                              const void *      aTxtData,
                              uint16_t          aTxtLength,
                              uint32_t          aTtl,
                              NameCompressInfo &aCompressInfo)
{
    Error     error = kErrorNone;
    TxtRecord txtRecord;

    SuccessOrExit(error = AppendInstanceName(aMessage, aInstanceName, aCompressInfo));

    txtRecord.Init();
    txtRecord.SetTtl(aTtl);
    txtRecord.SetLength(aTxtLength);

    SuccessOrExit(error = aMessage.Append(txtRecord));
    error = aMessage.AppendBytes(aTxtData, aTxtLength);

exit:
    return error;
}

Error Server::AppendHostName(Message &aMessage, const char *aName, NameCompressInfo &aCompressInfo)
{
    Error    error;
    uint16_t hostCompressOffset = aCompressInfo.GetHostNameOffset(aMessage, aName);

    if (hostCompressOffset != NameCompressInfo::kUnknownOffset)
    {
        error = Name::AppendPointerLabel(hostCompressOffset, aMessage);
    }
    else
    {
        uint8_t  domainStart          = static_cast<uint8_t>(StringLength(aName, Name::kMaxNameLength) -
                                                   StringLength(aCompressInfo.GetDomainName(), Name::kMaxNameSize - 1));
        uint16_t domainCompressOffset = aCompressInfo.GetDomainNameOffset();

        hostCompressOffset = aMessage.GetLength();
        aCompressInfo.SetHostNameOffset(hostCompressOffset);

        if (domainCompressOffset == NameCompressInfo::kUnknownOffset)
        {
            aCompressInfo.SetDomainNameOffset(hostCompressOffset + domainStart);
            error = Name::AppendName(aName, aMessage);
        }
        else
        {
            SuccessOrExit(error = Name::AppendMultipleLabels(aName, domainStart, aMessage));
            error = Name::AppendPointerLabel(domainCompressOffset, aMessage);
        }
    }

exit:
    return error;
}

void Server::IncResourceRecordCount(Header &aHeader, bool aAdditional)
{
    if (aAdditional)
    {
        aHeader.SetAdditionalRecordCount(aHeader.GetAdditionalRecordCount() + 1);
    }
    else
    {
        aHeader.SetAnswerCount(aHeader.GetAnswerCount() + 1);
    }
}

Error Server::FindNameComponents(const char *aName, const char *aDomain, NameComponentsOffsetInfo &aInfo)
{
    uint8_t nameLen   = static_cast<uint8_t>(StringLength(aName, Name::kMaxNameLength));
    uint8_t domainLen = static_cast<uint8_t>(StringLength(aDomain, Name::kMaxNameLength));
    Error   error     = kErrorNone;
    uint8_t labelBegin, labelEnd;

    VerifyOrExit(Name::IsSubDomainOf(aName, aDomain), error = kErrorInvalidArgs);

    labelBegin          = nameLen - domainLen;
    aInfo.mDomainOffset = labelBegin;

    while (true)
    {
        error = FindPreviousLabel(aName, labelBegin, labelEnd);

        VerifyOrExit(error == kErrorNone, error = (error == kErrorNotFound ? kErrorNone : error));

        if (labelEnd == labelBegin + kProtocolLabelLength &&
            (memcmp(&aName[labelBegin], kDnssdProtocolUdp, kProtocolLabelLength) == 0 ||
             memcmp(&aName[labelBegin], kDnssdProtocolTcp, kProtocolLabelLength) == 0))
        {
            // <Protocol> label found
            aInfo.mProtocolOffset = labelBegin;
            break;
        }
    }

    // Get service label <Service>
    error = FindPreviousLabel(aName, labelBegin, labelEnd);
    VerifyOrExit(error == kErrorNone, error = (error == kErrorNotFound ? kErrorNone : error));

    aInfo.mServiceOffset = labelBegin;

    // Check for service subtype
    error = FindPreviousLabel(aName, labelBegin, labelEnd);
    VerifyOrExit(error == kErrorNone, error = (error == kErrorNotFound ? kErrorNone : error));

    // Note that `kDnssdSubTypeLabel` is "._sub.". Here we get the
    // label only so we want to compare it with "_sub".
    if ((labelEnd == labelBegin + kSubTypeLabelLength) &&
        (memcmp(&aName[labelBegin], kDnssdSubTypeLabel + 1, kSubTypeLabelLength) == 0))
    {
        SuccessOrExit(error = FindPreviousLabel(aName, labelBegin, labelEnd));
        VerifyOrExit(labelBegin == 0, error = kErrorInvalidArgs);
        aInfo.mSubTypeOffset = labelBegin;
        ExitNow();
    }

    // Treat everything before <Service> as <Instance> label
    aInfo.mInstanceOffset = 0;

exit:
    return error;
}

Error Server::FindPreviousLabel(const char *aName, uint8_t &aStart, uint8_t &aStop)
{
    // This method finds the previous label before the current label (whose start index is @p aStart), and updates @p
    // aStart to the start index of the label and @p aStop to the index of the dot just after the label.
    // @note The input value of @p aStop does not matter because it is only used to output.

    Error   error = kErrorNone;
    uint8_t start = aStart;
    uint8_t end;

    VerifyOrExit(start > 0, error = kErrorNotFound);
    VerifyOrExit(aName[--start] == Name::kLabelSeperatorChar, error = kErrorInvalidArgs);

    end = start;
    while (start > 0 && aName[start - 1] != Name::kLabelSeperatorChar)
    {
        start--;
    }

    VerifyOrExit(start < end, error = kErrorInvalidArgs);

    aStart = start;
    aStop  = end;

exit:
    return error;
}

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
Header::Response Server::ResolveBySrp(Header &                  aResponseHeader,
                                      Message &                 aResponseMessage,
                                      Server::NameCompressInfo &aCompressInfo)
{
    Question         question;
    uint16_t         readOffset = sizeof(Header);
    Header::Response response   = Header::kResponseSuccess;
    char             name[Name::kMaxNameSize];

    for (uint16_t i = 0; i < aResponseHeader.GetQuestionCount(); i++)
    {
        IgnoreError(Name::ReadName(aResponseMessage, readOffset, name, sizeof(name)));
        IgnoreError(aResponseMessage.Read(readOffset, question));
        readOffset += sizeof(question);

        response = ResolveQuestionBySrp(name, question, aResponseHeader, aResponseMessage, aCompressInfo,
                                        /* aAdditional */ false);

        otLogInfoDns("[server] ANSWER: TRANSACTION=0x%04x, QUESTION=[%s %d %d], RCODE=%d",
                     aResponseHeader.GetMessageId(), name, question.GetClass(), question.GetType(), response);
    }

    // Answer the questions with additional RRs if required
    if (aResponseHeader.GetAnswerCount() > 0)
    {
        readOffset = sizeof(Header);
        for (uint16_t i = 0; i < aResponseHeader.GetQuestionCount(); i++)
        {
            IgnoreError(Name::ReadName(aResponseMessage, readOffset, name, sizeof(name)));
            IgnoreError(aResponseMessage.Read(readOffset, question));
            readOffset += sizeof(question);

            VerifyOrExit(Header::kResponseServerFailure != ResolveQuestionBySrp(name, question, aResponseHeader,
                                                                                aResponseMessage, aCompressInfo,
                                                                                /* aAdditional */ true),
                         response = Header::kResponseServerFailure);

            otLogInfoDns("[server] ADDITIONAL: TRANSACTION=0x%04x, QUESTION=[%s %d %d], RCODE=%d",
                         aResponseHeader.GetMessageId(), name, question.GetClass(), question.GetType(), response);
        }
    }
exit:
    return response;
}

Header::Response Server::ResolveQuestionBySrp(const char *      aName,
                                              const Question &  aQuestion,
                                              Header &          aResponseHeader,
                                              Message &         aResponseMessage,
                                              NameCompressInfo &aCompressInfo,
                                              bool              aAdditional)
{
    Error                    error    = kErrorNone;
    const Srp::Server::Host *host     = nullptr;
    TimeMilli                now      = TimerMilli::GetNow();
    uint16_t                 qtype    = aQuestion.GetType();
    Header::Response         response = Header::kResponseNameError;

    while ((host = GetNextSrpHost(host)) != nullptr)
    {
        bool        needAdditionalAaaaRecord = false;
        const char *hostName                 = host->GetFullName();

        // Handle PTR/SRV/TXT query
        if (qtype == ResourceRecord::kTypePtr || qtype == ResourceRecord::kTypeSrv || qtype == ResourceRecord::kTypeTxt)
        {
            const Srp::Server::Service *service = nullptr;

            while ((service = GetNextSrpService(*host, service)) != nullptr)
            {
                uint32_t    instanceTtl         = TimeMilli::MsecToSec(service->GetExpireTime() - TimerMilli::GetNow());
                const char *instanceName        = service->GetInstanceName();
                bool        serviceNameMatched  = service->MatchesServiceName(aName);
                bool        instanceNameMatched = service->MatchesInstanceName(aName);
                bool        ptrQueryMatched     = qtype == ResourceRecord::kTypePtr && serviceNameMatched;
                bool        srvQueryMatched     = qtype == ResourceRecord::kTypeSrv && instanceNameMatched;
                bool        txtQueryMatched     = qtype == ResourceRecord::kTypeTxt && instanceNameMatched;

                if (ptrQueryMatched || srvQueryMatched)
                {
                    needAdditionalAaaaRecord = true;
                }

                if (!aAdditional && ptrQueryMatched)
                {
                    SuccessOrExit(
                        error = AppendPtrRecord(aResponseMessage, aName, instanceName, instanceTtl, aCompressInfo));
                    IncResourceRecordCount(aResponseHeader, aAdditional);
                    response = Header::kResponseSuccess;
                }

                if ((!aAdditional && srvQueryMatched) ||
                    (aAdditional && ptrQueryMatched &&
                     !HasQuestion(aResponseHeader, aResponseMessage, instanceName, ResourceRecord::kTypeSrv)))
                {
                    SuccessOrExit(error = AppendSrvRecord(aResponseMessage, instanceName, hostName, instanceTtl,
                                                          service->GetPriority(), service->GetWeight(),
                                                          service->GetPort(), aCompressInfo));
                    IncResourceRecordCount(aResponseHeader, aAdditional);
                    response = Header::kResponseSuccess;
                }

                if ((!aAdditional && txtQueryMatched) ||
                    (aAdditional && ptrQueryMatched &&
                     !HasQuestion(aResponseHeader, aResponseMessage, instanceName, ResourceRecord::kTypeTxt)))
                {
                    SuccessOrExit(error = AppendTxtRecord(aResponseMessage, instanceName, service->GetTxtData(),
                                                          service->GetTxtDataLength(), instanceTtl, aCompressInfo));
                    IncResourceRecordCount(aResponseHeader, aAdditional);
                    response = Header::kResponseSuccess;
                }
            }
        }

        // Handle AAAA query
        if ((!aAdditional && qtype == ResourceRecord::kTypeAaaa && host->Matches(aName)) ||
            (aAdditional && needAdditionalAaaaRecord &&
             !HasQuestion(aResponseHeader, aResponseMessage, hostName, ResourceRecord::kTypeAaaa)))
        {
            uint8_t             addrNum;
            const Ip6::Address *addrs   = host->GetAddresses(addrNum);
            uint32_t            hostTtl = TimeMilli::MsecToSec(host->GetExpireTime() - now);

            for (uint8_t i = 0; i < addrNum; i++)
            {
                SuccessOrExit(error = AppendAaaaRecord(aResponseMessage, hostName, addrs[i], hostTtl, aCompressInfo));
                IncResourceRecordCount(aResponseHeader, aAdditional);
            }

            response = Header::kResponseSuccess;
        }
    }

exit:
    return error == kErrorNone ? response : Header::kResponseServerFailure;
}

const Srp::Server::Host *Server::GetNextSrpHost(const Srp::Server::Host *aHost)
{
    const Srp::Server::Host *host = Get<Srp::Server>().GetNextHost(aHost);

    while (host != nullptr && host->IsDeleted())
    {
        host = Get<Srp::Server>().GetNextHost(host);
    }

    return host;
}

const Srp::Server::Service *Server::GetNextSrpService(const Srp::Server::Host &   aHost,
                                                      const Srp::Server::Service *aService)
{
    return aHost.FindNextService(aService, Srp::Server::kFlagsAnyTypeActiveService);
}
#endif // OPENTHREAD_CONFIG_SRP_SERVER_ENABLE

Error Server::ResolveByQueryCallbacks(Header &                aResponseHeader,
                                      Message &               aResponseMessage,
                                      NameCompressInfo &      aCompressInfo,
                                      const Ip6::MessageInfo &aMessageInfo)
{
    QueryTransaction *query = nullptr;
    DnsQueryType      queryType;
    char              name[Name::kMaxNameSize];

    Error error = kErrorNone;

    VerifyOrExit(mQuerySubscribe != nullptr, error = kErrorFailed);

    queryType = GetQueryTypeAndName(aResponseHeader, aResponseMessage, name);
    VerifyOrExit(queryType != kDnsQueryNone, error = kErrorNotImplemented);

    query = NewQuery(aResponseHeader, aResponseMessage, aCompressInfo, aMessageInfo);
    VerifyOrExit(query != nullptr, error = kErrorNoBufs);

    mQuerySubscribe(mQueryCallbackContext, name);

exit:
    return error;
}

Server::QueryTransaction *Server::NewQuery(const Header &          aResponseHeader,
                                           Message &               aResponseMessage,
                                           const NameCompressInfo &aCompressInfo,
                                           const Ip6::MessageInfo &aMessageInfo)
{
    QueryTransaction *newQuery = nullptr;

    for (QueryTransaction &query : mQueryTransactions)
    {
        if (query.IsValid())
        {
            continue;
        }

        query.Init(aResponseHeader, aResponseMessage, aCompressInfo, aMessageInfo);
        ExitNow(newQuery = &query);
    }

exit:
    if (newQuery != nullptr)
    {
        ResetTimer();
    }

    return newQuery;
}

bool Server::CanAnswerQuery(const QueryTransaction &          aQuery,
                            const char *                      aServiceFullName,
                            const otDnssdServiceInstanceInfo &aInstanceInfo)
{
    char         name[Name::kMaxNameSize];
    DnsQueryType sdType;
    bool         canAnswer = false;

    sdType = GetQueryTypeAndName(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), name);

    switch (sdType)
    {
    case kDnsQueryBrowse:
        canAnswer = (strcmp(name, aServiceFullName) == 0);
        break;
    case kDnsQueryResolve:
        canAnswer = (strcmp(name, aInstanceInfo.mFullName) == 0);
        break;
    default:
        break;
    }

    return canAnswer;
}

bool Server::CanAnswerQuery(const Server::QueryTransaction &aQuery, const char *aHostFullName)
{
    char         name[Name::kMaxNameSize];
    DnsQueryType sdType;

    sdType = GetQueryTypeAndName(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), name);
    return (sdType == kDnsQueryResolveHost) && (strcmp(name, aHostFullName) == 0);
}

void Server::AnswerQuery(QueryTransaction &                aQuery,
                         const char *                      aServiceFullName,
                         const otDnssdServiceInstanceInfo &aInstanceInfo)
{
    Header &          responseHeader  = aQuery.GetResponseHeader();
    Message &         responseMessage = aQuery.GetResponseMessage();
    Error             error           = kErrorNone;
    NameCompressInfo &compressInfo    = aQuery.GetNameCompressInfo();

    if (HasQuestion(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), aServiceFullName,
                    ResourceRecord::kTypePtr))
    {
        SuccessOrExit(error = AppendPtrRecord(responseMessage, aServiceFullName, aInstanceInfo.mFullName,
                                              aInstanceInfo.mTtl, compressInfo));
        IncResourceRecordCount(responseHeader, false);
    }

    for (uint8_t additional = 0; additional <= 1; additional++)
    {
        if (HasQuestion(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), aInstanceInfo.mFullName,
                        ResourceRecord::kTypeSrv) == !additional)
        {
            SuccessOrExit(error = AppendSrvRecord(responseMessage, aInstanceInfo.mFullName, aInstanceInfo.mHostName,
                                                  aInstanceInfo.mTtl, aInstanceInfo.mPriority, aInstanceInfo.mWeight,
                                                  aInstanceInfo.mPort, compressInfo));
            IncResourceRecordCount(responseHeader, additional);
        }

        if (HasQuestion(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), aInstanceInfo.mFullName,
                        ResourceRecord::kTypeTxt) == !additional)
        {
            SuccessOrExit(error = AppendTxtRecord(responseMessage, aInstanceInfo.mFullName, aInstanceInfo.mTxtData,
                                                  aInstanceInfo.mTxtLength, aInstanceInfo.mTtl, compressInfo));
            IncResourceRecordCount(responseHeader, additional);
        }

        if (HasQuestion(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), aInstanceInfo.mHostName,
                        ResourceRecord::kTypeAaaa) == !additional)
        {
            for (uint8_t i = 0; i < aInstanceInfo.mAddressNum; i++)
            {
                const Ip6::Address &address = static_cast<const Ip6::Address &>(aInstanceInfo.mAddresses[i]);

                OT_ASSERT(!address.IsUnspecified() && !address.IsLinkLocal() && !address.IsMulticast() &&
                          !address.IsLoopback());

                SuccessOrExit(error = AppendAaaaRecord(responseMessage, aInstanceInfo.mHostName, address,
                                                       aInstanceInfo.mTtl, compressInfo));
                IncResourceRecordCount(responseHeader, additional);
            }
        }
    }

exit:
    FinalizeQuery(aQuery, error == kErrorNone ? Header::kResponseSuccess : Header::kResponseServerFailure);
    ResetTimer();
}

void Server::AnswerQuery(QueryTransaction &aQuery, const char *aHostFullName, const otDnssdHostInfo &aHostInfo)
{
    Header &          responseHeader  = aQuery.GetResponseHeader();
    Message &         responseMessage = aQuery.GetResponseMessage();
    Error             error           = kErrorNone;
    NameCompressInfo &compressInfo    = aQuery.GetNameCompressInfo();

    if (HasQuestion(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), aHostFullName, ResourceRecord::kTypeAaaa))
    {
        for (uint8_t i = 0; i < aHostInfo.mAddressNum; i++)
        {
            const Ip6::Address &address = static_cast<const Ip6::Address &>(aHostInfo.mAddresses[i]);

            OT_ASSERT(!address.IsUnspecified() && !address.IsMulticast() && !address.IsLinkLocal() &&
                      !address.IsLoopback());

            SuccessOrExit(error =
                              AppendAaaaRecord(responseMessage, aHostFullName, address, aHostInfo.mTtl, compressInfo));
            IncResourceRecordCount(responseHeader, /* aAdditional */ false);
        }
    }

exit:
    FinalizeQuery(aQuery, error == kErrorNone ? Header::kResponseSuccess : Header::kResponseServerFailure);
    ResetTimer();
}

void Server::SetQueryCallbacks(otDnssdQuerySubscribeCallback   aSubscribe,
                               otDnssdQueryUnsubscribeCallback aUnsubscribe,
                               void *                          aContext)
{
    OT_ASSERT((aSubscribe == nullptr) == (aUnsubscribe == nullptr));

    mQuerySubscribe       = aSubscribe;
    mQueryUnsubscribe     = aUnsubscribe;
    mQueryCallbackContext = aContext;
}

void Server::HandleDiscoveredServiceInstance(const char *                      aServiceFullName,
                                             const otDnssdServiceInstanceInfo &aInstanceInfo)
{
    OT_ASSERT(StringEndsWith(aServiceFullName, Name::kLabelSeperatorChar));
    OT_ASSERT(StringEndsWith(aInstanceInfo.mFullName, Name::kLabelSeperatorChar));
    OT_ASSERT(StringEndsWith(aInstanceInfo.mHostName, Name::kLabelSeperatorChar));

    for (QueryTransaction &query : mQueryTransactions)
    {
        if (query.IsValid() && CanAnswerQuery(query, aServiceFullName, aInstanceInfo))
        {
            AnswerQuery(query, aServiceFullName, aInstanceInfo);
        }
    }
}

void Server::HandleDiscoveredHost(const char *aHostFullName, const otDnssdHostInfo &aHostInfo)
{
    OT_ASSERT(StringEndsWith(aHostFullName, Name::kLabelSeperatorChar));

    for (QueryTransaction &query : mQueryTransactions)
    {
        if (query.IsValid() && CanAnswerQuery(query, aHostFullName))
        {
            AnswerQuery(query, aHostFullName, aHostInfo);
        }
    }
}

const otDnssdQuery *Server::GetNextQuery(const otDnssdQuery *aQuery) const
{
    const QueryTransaction *cur   = &mQueryTransactions[0];
    const QueryTransaction *found = nullptr;
    const QueryTransaction *query = static_cast<const QueryTransaction *>(aQuery);

    if (aQuery != nullptr)
    {
        cur = query + 1;
    }

    for (; cur < OT_ARRAY_END(mQueryTransactions); cur++)
    {
        if (cur->IsValid())
        {
            found = cur;
            break;
        }
    }

    return static_cast<const otDnssdQuery *>(found);
}

Server::DnsQueryType Server::GetQueryTypeAndName(const otDnssdQuery *aQuery, char (&aName)[Name::kMaxNameSize])
{
    const QueryTransaction *query = static_cast<const QueryTransaction *>(aQuery);

    OT_ASSERT(query->IsValid());
    return GetQueryTypeAndName(query->GetResponseHeader(), query->GetResponseMessage(), aName);
}

Server::DnsQueryType Server::GetQueryTypeAndName(const Header & aHeader,
                                                 const Message &aMessage,
                                                 char (&aName)[Name::kMaxNameSize])
{
    DnsQueryType sdType = kDnsQueryNone;

    for (uint16_t i = 0, readOffset = sizeof(Header); i < aHeader.GetQuestionCount(); i++)
    {
        Question question;

        IgnoreError(Name::ReadName(aMessage, readOffset, aName, sizeof(aName)));
        IgnoreError(aMessage.Read(readOffset, question));
        readOffset += sizeof(question);

        switch (question.GetType())
        {
        case ResourceRecord::kTypePtr:
            ExitNow(sdType = kDnsQueryBrowse);
        case ResourceRecord::kTypeSrv:
        case ResourceRecord::kTypeTxt:
            ExitNow(sdType = kDnsQueryResolve);
        }
    }

    for (uint16_t i = 0, readOffset = sizeof(Header); i < aHeader.GetQuestionCount(); i++)
    {
        Question question;

        IgnoreError(Name::ReadName(aMessage, readOffset, aName, sizeof(aName)));
        IgnoreError(aMessage.Read(readOffset, question));
        readOffset += sizeof(question);

        switch (question.GetType())
        {
        case ResourceRecord::kTypeAaaa:
        case ResourceRecord::kTypeA:
            ExitNow(sdType = kDnsQueryResolveHost);
        }
    }

exit:
    return sdType;
}

bool Server::HasQuestion(const Header &aHeader, const Message &aMessage, const char *aName, uint16_t aQuestionType)
{
    bool found = false;

    for (uint16_t i = 0, readOffset = sizeof(Header); i < aHeader.GetQuestionCount(); i++)
    {
        Question question;
        Error    error;

        error = Name::CompareName(aMessage, readOffset, aName);
        IgnoreError(aMessage.Read(readOffset, question));
        readOffset += sizeof(question);

        if (error == kErrorNone && aQuestionType == question.GetType())
        {
            ExitNow(found = true);
        }
    }

exit:
    return found;
}

void Server::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Server>().HandleTimer();
}

void Server::HandleTimer(void)
{
    TimeMilli now = TimerMilli::GetNow();

    for (QueryTransaction &query : mQueryTransactions)
    {
        TimeMilli expire;

        if (!query.IsValid())
        {
            continue;
        }

        expire = query.GetStartTime() + kQueryTimeout;
        if (expire <= now)
        {
            FinalizeQuery(query, Header::kResponseSuccess);
        }
    }

    ResetTimer();
}

void Server::ResetTimer(void)
{
    TimeMilli now        = TimerMilli::GetNow();
    TimeMilli nextExpire = now.GetDistantFuture();

    for (QueryTransaction &query : mQueryTransactions)
    {
        TimeMilli expire;

        if (!query.IsValid())
        {
            continue;
        }

        expire = query.GetStartTime() + kQueryTimeout;
        if (expire <= now)
        {
            nextExpire = now;
        }
        else if (expire < nextExpire)
        {
            nextExpire = expire;
        }
    }

    if (nextExpire < now.GetDistantFuture())
    {
        mTimer.FireAt(nextExpire);
    }
    else
    {
        mTimer.Stop();
    }
}

void Server::FinalizeQuery(QueryTransaction &aQuery, Header::Response aResponseCode)
{
    char         name[Name::kMaxNameSize];
    DnsQueryType sdType;

    OT_ASSERT(mQueryUnsubscribe != nullptr);

    sdType = GetQueryTypeAndName(aQuery.GetResponseHeader(), aQuery.GetResponseMessage(), name);

    OT_ASSERT(sdType != kDnsQueryNone);
    OT_UNUSED_VARIABLE(sdType);

    mQueryUnsubscribe(mQueryCallbackContext, name);
    aQuery.Finalize(aResponseCode, mSocket);
}

void Server::QueryTransaction::Init(const Header &          aResponseHeader,
                                    Message &               aResponseMessage,
                                    const NameCompressInfo &aCompressInfo,
                                    const Ip6::MessageInfo &aMessageInfo)
{
    OT_ASSERT(mResponseMessage == nullptr);

    mResponseHeader  = aResponseHeader;
    mResponseMessage = &aResponseMessage;
    mCompressInfo    = aCompressInfo;
    mMessageInfo     = aMessageInfo;
    mStartTime       = TimerMilli::GetNow();
}

void Server::QueryTransaction::Finalize(Header::Response aResponseMessage, Ip6::Udp::Socket &aSocket)
{
    OT_ASSERT(mResponseMessage != nullptr);

    SendResponse(mResponseHeader, aResponseMessage, *mResponseMessage, mMessageInfo, aSocket);
    mResponseMessage = nullptr;
}

} // namespace ServiceDiscovery
} // namespace Dns
} // namespace ot

#endif // OPENTHREAD_CONFIG_DNS_SERVER_ENABLE
