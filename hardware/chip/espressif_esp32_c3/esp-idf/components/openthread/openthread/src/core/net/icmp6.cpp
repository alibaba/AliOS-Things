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
 *   This file implements ICMPv6.
 */

#include "icmp6.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/message.hpp"
#include "net/checksum.hpp"
#include "net/ip6.hpp"

namespace ot {
namespace Ip6 {

Icmp::Icmp(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mEchoSequence(1)
    , mEchoMode(OT_ICMP6_ECHO_HANDLER_ALL)
{
}

Message *Icmp::NewMessage(uint16_t aReserved)
{
    return Get<Ip6>().NewMessage(sizeof(Header) + aReserved);
}

Error Icmp::RegisterHandler(Handler &aHandler)
{
    return mHandlers.Add(aHandler);
}

Error Icmp::SendEchoRequest(Message &aMessage, const MessageInfo &aMessageInfo, uint16_t aIdentifier)
{
    Error       error = kErrorNone;
    MessageInfo messageInfoLocal;
    Header      icmpHeader;

    messageInfoLocal = aMessageInfo;

    icmpHeader.Clear();
    icmpHeader.SetType(Header::kTypeEchoRequest);
    icmpHeader.SetId(aIdentifier);
    icmpHeader.SetSequence(mEchoSequence++);

    SuccessOrExit(error = aMessage.Prepend(icmpHeader));
    aMessage.SetOffset(0);
    SuccessOrExit(error = Get<Ip6>().SendDatagram(aMessage, messageInfoLocal, kProtoIcmp6));

    otLogInfoIcmp("Sent echo request: (seq = %d)", icmpHeader.GetSequence());

exit:
    return error;
}

Error Icmp::SendError(Header::Type aType, Header::Code aCode, const MessageInfo &aMessageInfo, const Message &aMessage)
{
    Error             error = kErrorNone;
    MessageInfo       messageInfoLocal;
    Message *         message = nullptr;
    Header            icmp6Header;
    ot::Ip6::Header   ip6Header;
    Message::Settings settings(Message::kWithLinkSecurity, Message::kPriorityNet);

    SuccessOrExit(error = aMessage.Read(0, ip6Header));

    if (ip6Header.GetNextHeader() == kProtoIcmp6)
    {
        SuccessOrExit(aMessage.Read(sizeof(ip6Header), icmp6Header));
        VerifyOrExit(!icmp6Header.IsError());
    }

    messageInfoLocal = aMessageInfo;

    VerifyOrExit((message = Get<Ip6>().NewMessage(0, settings)) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = message->SetLength(sizeof(icmp6Header) + sizeof(ip6Header)));

    message->Write(sizeof(icmp6Header), ip6Header);

    icmp6Header.Clear();
    icmp6Header.SetType(aType);
    icmp6Header.SetCode(aCode);
    message->Write(0, icmp6Header);

    SuccessOrExit(error = Get<Ip6>().SendDatagram(*message, messageInfoLocal, kProtoIcmp6));

    otLogInfoIcmp("Sent ICMPv6 Error");

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error Icmp::HandleMessage(Message &aMessage, MessageInfo &aMessageInfo)
{
    Error  error = kErrorNone;
    Header icmp6Header;

    SuccessOrExit(error = aMessage.Read(aMessage.GetOffset(), icmp6Header));

    SuccessOrExit(error = Checksum::VerifyMessageChecksum(aMessage, aMessageInfo, kProtoIcmp6));

    if (icmp6Header.GetType() == Header::kTypeEchoRequest)
    {
        SuccessOrExit(error = HandleEchoRequest(aMessage, aMessageInfo));
    }

    aMessage.MoveOffset(sizeof(icmp6Header));

    for (Handler &handler : mHandlers)
    {
        handler.HandleReceiveMessage(aMessage, aMessageInfo, icmp6Header);
    }

exit:
    return error;
}

bool Icmp::ShouldHandleEchoRequest(const MessageInfo &aMessageInfo)
{
    bool rval = false;

    switch (mEchoMode)
    {
    case OT_ICMP6_ECHO_HANDLER_DISABLED:
        rval = false;
        break;
    case OT_ICMP6_ECHO_HANDLER_UNICAST_ONLY:
        rval = !aMessageInfo.GetSockAddr().IsMulticast();
        break;
    case OT_ICMP6_ECHO_HANDLER_MULTICAST_ONLY:
        rval = aMessageInfo.GetSockAddr().IsMulticast();
        break;
    case OT_ICMP6_ECHO_HANDLER_ALL:
        rval = true;
        break;
    }

    return rval;
}

Error Icmp::HandleEchoRequest(Message &aRequestMessage, const MessageInfo &aMessageInfo)
{
    Error       error = kErrorNone;
    Header      icmp6Header;
    Message *   replyMessage = nullptr;
    MessageInfo replyMessageInfo;
    uint16_t    payloadLength;

    // always handle Echo Request destined for RLOC or ALOC
    VerifyOrExit(ShouldHandleEchoRequest(aMessageInfo) || aMessageInfo.GetSockAddr().GetIid().IsLocator());

    otLogInfoIcmp("Received Echo Request");

    icmp6Header.Clear();
    icmp6Header.SetType(Header::kTypeEchoReply);

    if ((replyMessage = Get<Ip6>().NewMessage(0)) == nullptr)
    {
        otLogDebgIcmp("Failed to allocate a new message");
        ExitNow();
    }

    payloadLength = aRequestMessage.GetLength() - aRequestMessage.GetOffset() - Header::kDataFieldOffset;
    SuccessOrExit(error = replyMessage->SetLength(Header::kDataFieldOffset + payloadLength));

    replyMessage->WriteBytes(0, &icmp6Header, Header::kDataFieldOffset);
    aRequestMessage.CopyTo(aRequestMessage.GetOffset() + Header::kDataFieldOffset, Header::kDataFieldOffset,
                           payloadLength, *replyMessage);

    replyMessageInfo.SetPeerAddr(aMessageInfo.GetPeerAddr());

    if (!aMessageInfo.GetSockAddr().IsMulticast())
    {
        replyMessageInfo.SetSockAddr(aMessageInfo.GetSockAddr());
    }

    SuccessOrExit(error = Get<Ip6>().SendDatagram(*replyMessage, replyMessageInfo, kProtoIcmp6));

    IgnoreError(replyMessage->Read(replyMessage->GetOffset(), icmp6Header));
    otLogInfoIcmp("Sent Echo Reply (seq = %d)", icmp6Header.GetSequence());

exit:
    FreeMessageOnError(replyMessage, error);
    return error;
}

} // namespace Ip6
} // namespace ot
