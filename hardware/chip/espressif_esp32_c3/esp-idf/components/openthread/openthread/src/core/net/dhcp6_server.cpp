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
 *   This file implements DHCPv6 Server.
 */

#include "dhcp6_server.hpp"

#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "thread/mle.hpp"
#include "thread/thread_netif.hpp"

namespace ot {
namespace Dhcp6 {

Server::Server(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mSocket(aInstance)
    , mPrefixAgentsCount(0)
    , mPrefixAgentsMask(0)
{
    memset(mPrefixAgents, 0, sizeof(mPrefixAgents));
}

Error Server::UpdateService(void)
{
    Error                           error  = kErrorNone;
    uint16_t                        rloc16 = Get<Mle::MleRouter>().GetRloc16();
    NetworkData::Iterator           iterator;
    NetworkData::OnMeshPrefixConfig config;
    Lowpan::Context                 lowpanContext;

    // remove dhcp agent aloc and prefix delegation
    for (PrefixAgent &prefixAgent : mPrefixAgents)
    {
        bool found = false;

        if (!prefixAgent.IsValid())
        {
            continue;
        }

        iterator = NetworkData::kIteratorInit;

        while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, rloc16, config) == kErrorNone)
        {
            if (!(config.mDhcp || config.mConfigure))
            {
                continue;
            }

            error = Get<NetworkData::Leader>().GetContext(prefixAgent.GetPrefixAsAddress(), lowpanContext);

            if ((error == kErrorNone) && (prefixAgent.GetContextId() == lowpanContext.mContextId))
            {
                // still in network data
                found = true;
                break;
            }
        }

        if (!found)
        {
            Get<ThreadNetif>().RemoveUnicastAddress(prefixAgent.GetAloc());
            prefixAgent.Clear();
            mPrefixAgentsCount--;
        }
    }

    // add dhcp agent aloc and prefix delegation
    iterator = NetworkData::kIteratorInit;

    while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, rloc16, config) == kErrorNone)
    {
        if (!(config.mDhcp || config.mConfigure))
        {
            continue;
        }

        error = Get<NetworkData::Leader>().GetContext(static_cast<const Ip6::Address &>(config.mPrefix.mPrefix),
                                                      lowpanContext);

        if (error == kErrorNone)
        {
            AddPrefixAgent(config.GetPrefix(), lowpanContext);
        }
    }

    if (mPrefixAgentsCount > 0)
    {
        Start();
    }
    else
    {
        Stop();
    }

    return error;
}

void Server::Start(void)
{
    VerifyOrExit(!mSocket.IsOpen());

    IgnoreError(mSocket.Open(&Server::HandleUdpReceive, this));
    IgnoreError(mSocket.Bind(kDhcpServerPort));

exit:
    return;
}

void Server::Stop(void)
{
    IgnoreError(mSocket.Close());
}

void Server::AddPrefixAgent(const Ip6::Prefix &aIp6Prefix, const Lowpan::Context &aContext)
{
    Error        error    = kErrorNone;
    PrefixAgent *newEntry = nullptr;

    for (PrefixAgent &prefixAgent : mPrefixAgents)
    {
        if (!prefixAgent.IsValid())
        {
            newEntry = &prefixAgent;
        }
        else if (prefixAgent.GetPrefix() == aIp6Prefix)
        {
            // already added
            ExitNow();
        }
    }

    VerifyOrExit(newEntry != nullptr, error = kErrorNoBufs);

    newEntry->Set(aIp6Prefix, Get<Mle::MleRouter>().GetMeshLocalPrefix(), aContext.mContextId);
    Get<ThreadNetif>().AddUnicastAddress(newEntry->GetAloc());
    mPrefixAgentsCount++;

exit:

    if (error != kErrorNone)
    {
        otLogNoteIp6("Failed to add DHCPv6 prefix agent: %s", ErrorToString(error));
    }
}

void Server::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Server *>(aContext)->HandleUdpReceive(*static_cast<Message *>(aMessage),
                                                      *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Server::HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Header header;

    SuccessOrExit(aMessage.Read(aMessage.GetOffset(), header));
    aMessage.MoveOffset(sizeof(header));

    // discard if not solicit type
    VerifyOrExit((header.GetType() == kTypeSolicit));

    ProcessSolicit(aMessage, aMessageInfo.GetPeerAddr(), header.GetTransactionId());

exit:
    return;
}

void Server::ProcessSolicit(Message &aMessage, const Ip6::Address &aDst, const TransactionId &aTransactionId)
{
    IaNa             iana;
    ClientIdentifier clientIdentifier;
    uint16_t         optionOffset;
    uint16_t         offset = aMessage.GetOffset();
    uint16_t         length = aMessage.GetLength() - aMessage.GetOffset();

    // Client Identifier (discard if not present)
    VerifyOrExit((optionOffset = FindOption(aMessage, offset, length, kOptionClientIdentifier)) > 0);
    SuccessOrExit(ProcessClientIdentifier(aMessage, optionOffset, clientIdentifier));

    // Server Identifier (assuming Rapid Commit, discard if present)
    VerifyOrExit(FindOption(aMessage, offset, length, kOptionServerIdentifier) == 0);

    // Rapid Commit (assuming Rapid Commit, discard if not present)
    VerifyOrExit(FindOption(aMessage, offset, length, kOptionRapidCommit) > 0);

    // Elapsed Time if present
    if ((optionOffset = FindOption(aMessage, offset, length, kOptionElapsedTime)) > 0)
    {
        SuccessOrExit(ProcessElapsedTime(aMessage, optionOffset));
    }

    // IA_NA (discard if not present)
    VerifyOrExit((optionOffset = FindOption(aMessage, offset, length, kOptionIaNa)) > 0);
    SuccessOrExit(ProcessIaNa(aMessage, optionOffset, iana));

    SuccessOrExit(SendReply(aDst, aTransactionId, clientIdentifier, iana));

exit:
    return;
}

uint16_t Server::FindOption(Message &aMessage, uint16_t aOffset, uint16_t aLength, Code aCode)
{
    uint16_t end  = aOffset + aLength;
    uint16_t rval = 0;

    while (aOffset <= end)
    {
        Option option;

        SuccessOrExit(aMessage.Read(aOffset, option));

        if (option.GetCode() == aCode)
        {
            ExitNow(rval = aOffset);
        }

        aOffset += sizeof(option) + option.GetLength();
    }

exit:
    return rval;
}
Error Server::ProcessClientIdentifier(Message &aMessage, uint16_t aOffset, ClientIdentifier &aClientId)
{
    Error error = kErrorNone;

    SuccessOrExit(error = aMessage.Read(aOffset, aClientId));
    VerifyOrExit((aClientId.GetLength() == sizeof(aClientId) - sizeof(Option)) &&
                     (aClientId.GetDuidType() == kDuidLinkLayerAddress) &&
                     (aClientId.GetDuidHardwareType() == kHardwareTypeEui64),
                 error = kErrorParse);
exit:
    return error;
}

Error Server::ProcessElapsedTime(Message &aMessage, uint16_t aOffset)
{
    Error       error = kErrorNone;
    ElapsedTime option;

    SuccessOrExit(error = aMessage.Read(aOffset, option));
    VerifyOrExit(option.GetLength() == sizeof(option) - sizeof(Option), error = kErrorParse);
exit:
    return error;
}

Error Server::ProcessIaNa(Message &aMessage, uint16_t aOffset, IaNa &aIaNa)
{
    Error    error = kErrorNone;
    uint16_t optionOffset;
    uint16_t length;

    SuccessOrExit(error = aMessage.Read(aOffset, aIaNa));

    aOffset += sizeof(aIaNa);
    length = aIaNa.GetLength() + sizeof(Option) - sizeof(IaNa);

    VerifyOrExit(length <= aMessage.GetLength() - aOffset, error = kErrorParse);

    mPrefixAgentsMask = 0;

    while (length > 0)
    {
        VerifyOrExit((optionOffset = FindOption(aMessage, aOffset, length, kOptionIaAddress)) > 0);
        SuccessOrExit(error = ProcessIaAddress(aMessage, optionOffset));

        length -= ((optionOffset - aOffset) + sizeof(IaAddress));
        aOffset = optionOffset + sizeof(IaAddress);
    }

exit:
    return error;
}

Error Server::ProcessIaAddress(Message &aMessage, uint16_t aOffset)
{
    Error     error = kErrorNone;
    IaAddress option;

    SuccessOrExit(error = aMessage.Read(aOffset, option));
    VerifyOrExit(option.GetLength() == sizeof(option) - sizeof(Option), error = kErrorParse);

    // mask matching prefix
    for (uint16_t i = 0; i < OT_ARRAY_LENGTH(mPrefixAgents); i++)
    {
        if (mPrefixAgents[i].IsValid() && mPrefixAgents[i].IsPrefixMatch(option.GetAddress()))
        {
            mPrefixAgentsMask |= (1 << i);
            break;
        }
    }

exit:
    return error;
}

Error Server::SendReply(const Ip6::Address & aDst,
                        const TransactionId &aTransactionId,
                        ClientIdentifier &   aClientId,
                        IaNa &               aIaNa)
{
    Error            error = kErrorNone;
    Ip6::MessageInfo messageInfo;
    Message *        message;

    VerifyOrExit((message = mSocket.NewMessage(0)) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, aTransactionId));
    SuccessOrExit(error = AppendServerIdentifier(*message));
    SuccessOrExit(error = AppendClientIdentifier(*message, aClientId));
    SuccessOrExit(error = AppendIaNa(*message, aIaNa));
    SuccessOrExit(error = AppendStatusCode(*message, kStatusSuccess));
    SuccessOrExit(error = AppendIaAddress(*message, aClientId));
    SuccessOrExit(error = AppendRapidCommit(*message));

    messageInfo.SetPeerAddr(aDst);
    messageInfo.SetPeerPort(kDhcpClientPort);
    SuccessOrExit(error = mSocket.SendTo(*message, messageInfo));

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error Server::AppendHeader(Message &aMessage, const TransactionId &aTransactionId)
{
    Header header;

    header.Clear();
    header.SetType(kTypeReply);
    header.SetTransactionId(aTransactionId);
    return aMessage.Append(header);
}

Error Server::AppendClientIdentifier(Message &aMessage, ClientIdentifier &aClientId)
{
    return aMessage.Append(aClientId);
}

Error Server::AppendServerIdentifier(Message &aMessage)
{
    Error            error = kErrorNone;
    ServerIdentifier option;
    Mac::ExtAddress  eui64;

    Get<Radio>().GetIeeeEui64(eui64);

    option.Init();
    option.SetDuidType(kDuidLinkLayerAddress);
    option.SetDuidHardwareType(kHardwareTypeEui64);
    option.SetDuidLinkLayerAddress(eui64);
    SuccessOrExit(error = aMessage.Append(option));

exit:
    return error;
}

Error Server::AppendIaNa(Message &aMessage, IaNa &aIaNa)
{
    Error    error  = kErrorNone;
    uint16_t length = 0;

    if (mPrefixAgentsMask)
    {
        for (uint16_t i = 0; i < OT_ARRAY_LENGTH(mPrefixAgents); i++)
        {
            if (mPrefixAgentsMask & (1 << i))
            {
                length += sizeof(IaAddress);
            }
        }
    }
    else
    {
        length += sizeof(IaAddress) * mPrefixAgentsCount;
    }

    length += sizeof(IaNa) + sizeof(StatusCode) - sizeof(Option);

    aIaNa.SetLength(length);
    aIaNa.SetT1(IaNa::kDefaultT1);
    aIaNa.SetT2(IaNa::kDefaultT2);
    SuccessOrExit(error = aMessage.Append(aIaNa));

exit:
    return error;
}

Error Server::AppendStatusCode(Message &aMessage, Status aStatusCode)
{
    StatusCode option;

    option.Init();
    option.SetStatusCode(aStatusCode);
    return aMessage.Append(option);
}

Error Server::AppendIaAddress(Message &aMessage, ClientIdentifier &aClientId)
{
    Error error = kErrorNone;

    if (mPrefixAgentsMask)
    {
        // if specified, only apply specified prefixes
        for (uint16_t i = 0; i < OT_ARRAY_LENGTH(mPrefixAgents); i++)
        {
            if (mPrefixAgentsMask & (1 << i))
            {
                SuccessOrExit(error = AddIaAddress(aMessage, mPrefixAgents[i].GetPrefixAsAddress(), aClientId));
            }
        }
    }
    else
    {
        // if not specified, apply all configured prefixes
        for (const PrefixAgent &prefixAgent : mPrefixAgents)
        {
            if (prefixAgent.IsValid())
            {
                SuccessOrExit(error = AddIaAddress(aMessage, prefixAgent.GetPrefixAsAddress(), aClientId));
            }
        }
    }

exit:
    return error;
}

Error Server::AddIaAddress(Message &aMessage, const Ip6::Address &aPrefix, ClientIdentifier &aClientId)
{
    Error     error = kErrorNone;
    IaAddress option;

    option.Init();
    option.GetAddress().SetPrefix(aPrefix.mFields.m8, OT_IP6_PREFIX_BITSIZE);
    option.GetAddress().GetIid().SetFromExtAddress(aClientId.GetDuidLinkLayerAddress());
    option.SetPreferredLifetime(IaAddress::kDefaultPreferredLifetime);
    option.SetValidLifetime(IaAddress::kDefaultValidLiftetime);
    SuccessOrExit(error = aMessage.Append(option));

exit:
    return error;
}

Error Server::AppendRapidCommit(Message &aMessage)
{
    RapidCommit option;

    option.Init();
    return aMessage.Append(option);
}

void Server::ApplyMeshLocalPrefix(void)
{
    for (PrefixAgent &prefixAgent : mPrefixAgents)
    {
        if (prefixAgent.IsValid())
        {
            PrefixAgent *entry = &prefixAgent;

            Get<ThreadNetif>().RemoveUnicastAddress(entry->GetAloc());
            entry->GetAloc().GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
            Get<ThreadNetif>().AddUnicastAddress(entry->GetAloc());
        }
    }
}

} // namespace Dhcp6
} // namespace ot

#endif //  OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
