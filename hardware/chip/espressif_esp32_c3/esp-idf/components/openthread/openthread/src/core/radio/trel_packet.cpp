/*
 *    Copyright (c) 2019, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements Thread Radio Encapsulation Link (TREL) packet.
 */

#include "trel_packet.hpp"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"

namespace ot {
namespace Trel {

void Header::SetAckMode(AckMode aAckMode)
{
    switch (aAckMode)
    {
    case kNoAck:
        mControl &= ~kAckModeFlag;
        break;

    case kAckRequested:
        mControl |= kAckModeFlag;
        break;
    }
}

uint16_t Header::GetSize(Type aType)
{
    uint16_t size = sizeof(Header);

    switch (aType)
    {
    case kTypeBroadcast:
        size -= sizeof(Mac::ExtAddress); // Exclude mDestination field.
        break;

    case kTypeUnicast:
    case kTypeAck:
        break;
    }

    return size;
}

Header::InfoString Header::ToString(void) const
{
    Type       type = GetType();
    InfoString string;

    switch (type)
    {
    case kTypeBroadcast:
        string.Append("broadcast ch:%d", GetChannel());
        break;

    case kTypeUnicast:
        string.Append("unicast ch:%d", GetChannel());
        break;

    case kTypeAck:
        string.Append("ack");
        break;
    }

    string.Append(" panid:%04x num:%lu src:%s", GetPanId(), GetPacketNumber(), GetSource().ToString().AsCString());

    if ((type == kTypeUnicast) || (type == kTypeAck))
    {
        string.Append(" dst:%s", GetDestination().ToString().AsCString());
    }

    if ((type == kTypeUnicast) || (type == kTypeBroadcast))
    {
        string.Append(GetAckMode() == kNoAck ? " no-ack" : " ack-req");
    }

    return string;
}

void Packet::Init(uint8_t *aBuffer, uint16_t aLength)
{
    mBuffer = aBuffer;
    mLength = aLength;
}

void Packet::Init(Header::Type aType, uint8_t *aPayload, uint16_t aPayloadLength)
{
    uint16_t headerSize = Header::GetSize(aType);

    // The payload buffer should reserve enough bytes for
    // header (depending on type) before the payload.

    Init(aPayload - headerSize, aPayloadLength + headerSize);
    GetHeader().Init(aType);
}

bool Packet::IsHeaderValid(void) const
{
    return ((mBuffer != nullptr) && (mLength > 0) && GetHeader().IsVersionValid() &&
            (mLength >= GetHeader().GetLength()));
}

} // namespace Trel
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
