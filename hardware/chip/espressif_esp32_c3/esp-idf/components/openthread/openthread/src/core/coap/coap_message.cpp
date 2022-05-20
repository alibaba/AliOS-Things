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
 *   This file implements the CoAP message generation and parsing.
 */

#include "coap_message.hpp"

#include "coap/coap.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/random.hpp"
#include "common/string.hpp"

namespace ot {
namespace Coap {

void Message::Init(void)
{
    GetHelpData().Clear();
    SetVersion(kVersion1);
    SetOffset(0);
    GetHelpData().mHeaderLength = kMinHeaderLength;

    IgnoreError(SetLength(GetHelpData().mHeaderLength));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    SetBlockWiseBlockNumber(0);
    SetMoreBlocksFlag(false);
    SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_16);
#endif
}

void Message::Init(Type aType, Code aCode)
{
    Init();
    SetType(aType);
    SetCode(aCode);
}

Error Message::Init(Type aType, Code aCode, const char *aUriPath)
{
    Error error;

    Init(aType, aCode);
    SuccessOrExit(error = GenerateRandomToken(kDefaultTokenLength));
    SuccessOrExit(error = AppendUriPathOptions(aUriPath));

exit:
    return error;
}

void Message::InitAsConfirmablePost(void)
{
    Init(kTypeConfirmable, kCodePost);
}

void Message::InitAsNonConfirmablePost(void)
{
    Init(kTypeNonConfirmable, kCodePost);
}

Error Message::InitAsConfirmablePost(const char *aUriPath)
{
    return Init(kTypeConfirmable, kCodePost, aUriPath);
}

Error Message::InitAsNonConfirmablePost(const char *aUriPath)
{
    return Init(kTypeNonConfirmable, kCodePost, aUriPath);
}

Error Message::InitAsPost(const Ip6::Address &aDestination, const char *aUriPath)
{
    return Init(aDestination.IsMulticast() ? kTypeNonConfirmable : kTypeConfirmable, kCodePost, aUriPath);
}

bool Message::IsConfirmablePostRequest(void) const
{
    return IsConfirmable() && IsPostRequest();
}

bool Message::IsNonConfirmablePostRequest(void) const
{
    return IsNonConfirmable() && IsPostRequest();
}

void Message::Finish(void)
{
    // If the payload marker is set but the message contains no
    // payload, we remove the payload marker from the message. Note
    // that the presence of a marker followed by a zero-length payload
    // will be processed as a message format error on the receiver.

    if (GetHelpData().mPayloadMarkerSet && (GetHelpData().mHeaderLength == GetLength()))
    {
        IgnoreError(SetLength(GetLength() - 1));
    }

    WriteBytes(0, &GetHelpData().mHeader, GetOptionStart());
}

uint8_t Message::WriteExtendedOptionField(uint16_t aValue, uint8_t *&aBuffer)
{
    /*
     * This method encodes a CoAP Option header field (Option Delta/Length) per
     * RFC 7252. The returned value is a 4-bit unsigned integer. Extended fields
     * (if needed) are written into the given buffer `aBuffer` and the pointer
     * would also be updated.
     *
     * If `aValue < 13 (kOption1ByteExtensionOffset)`, it is returned as is
     * (no extension).
     *
     * If `13 <= aValue < 269 (kOption2ByteExtensionOffset)`, one-byte
     * extension is used, and the value minus 13 is written in `aBuffer` as an
     * 8-bit unsigned integer, and `13 (kOption1ByteExtension)` is returned.
     *
     * If `269 <= aValue`, two-byte extension is used and the value minis 269
     * is written as a 16-bit unsigned integer and `14 (kOption2ByteExtension)`
     * is returned.
     *
     */

    uint8_t rval;

    if (aValue < kOption1ByteExtensionOffset)
    {
        rval = static_cast<uint8_t>(aValue);
    }
    else if (aValue < kOption2ByteExtensionOffset)
    {
        rval     = kOption1ByteExtension;
        *aBuffer = static_cast<uint8_t>(aValue - kOption1ByteExtensionOffset);
        aBuffer += sizeof(uint8_t);
    }
    else
    {
        rval = kOption2ByteExtension;
        Encoding::BigEndian::WriteUint16(aValue - kOption2ByteExtensionOffset, aBuffer);
        aBuffer += sizeof(uint16_t);
    }

    return rval;
}

Error Message::AppendOption(uint16_t aNumber, uint16_t aLength, const void *aValue)
{
    Error    error = kErrorNone;
    uint16_t delta;
    uint8_t  header[kMaxOptionHeaderSize];
    uint16_t headerLength;
    uint8_t *cur;

    VerifyOrExit(aNumber >= GetHelpData().mOptionLast, error = kErrorInvalidArgs);
    delta = aNumber - GetHelpData().mOptionLast;

    cur = &header[1];

    header[0] = static_cast<uint8_t>(WriteExtendedOptionField(delta, cur) << kOptionDeltaOffset);
    header[0] |= static_cast<uint8_t>(WriteExtendedOptionField(aLength, cur) << kOptionLengthOffset);

    headerLength = static_cast<uint16_t>(cur - header);

    VerifyOrExit(static_cast<uint32_t>(GetLength()) + headerLength + aLength < kMaxHeaderLength, error = kErrorNoBufs);

    SuccessOrExit(error = AppendBytes(header, headerLength));
    SuccessOrExit(error = AppendBytes(aValue, aLength));

    GetHelpData().mOptionLast = aNumber;

    GetHelpData().mHeaderLength = GetLength();

exit:
    return error;
}

Error Message::AppendUintOption(uint16_t aNumber, uint32_t aValue)
{
    uint8_t        buffer[sizeof(uint32_t)];
    const uint8_t *value  = &buffer[0];
    uint16_t       length = sizeof(uint32_t);

    Encoding::BigEndian::WriteUint32(aValue, buffer);

    while ((length > 0) && (value[0] == 0))
    {
        value++;
        length--;
    }

    return AppendOption(aNumber, length, value);
}

Error Message::AppendStringOption(uint16_t aNumber, const char *aValue)
{
    return AppendOption(aNumber, static_cast<uint16_t>(strlen(aValue)), aValue);
}

Error Message::AppendUriPathOptions(const char *aUriPath)
{
    Error       error = kErrorNone;
    const char *cur   = aUriPath;
    const char *end;

    while ((end = StringFind(cur, '/')) != nullptr)
    {
        SuccessOrExit(error = AppendOption(kOptionUriPath, static_cast<uint16_t>(end - cur), cur));
        cur = end + 1;
    }

    SuccessOrExit(error = AppendStringOption(kOptionUriPath, cur));

exit:
    return error;
}

Error Message::ReadUriPathOptions(char (&aUriPath)[kMaxReceivedUriPath + 1]) const
{
    char *           curUriPath = aUriPath;
    Error            error      = kErrorNone;
    Option::Iterator iterator;

    SuccessOrExit(error = iterator.Init(*this, kOptionUriPath));

    while (!iterator.IsDone())
    {
        uint16_t optionLength = iterator.GetOption()->GetLength();

        if (curUriPath != aUriPath)
        {
            *curUriPath++ = '/';
        }

        VerifyOrExit(curUriPath + optionLength < OT_ARRAY_END(aUriPath), error = kErrorParse);

        IgnoreError(iterator.ReadOptionValue(curUriPath));
        curUriPath += optionLength;

        SuccessOrExit(error = iterator.Advance(kOptionUriPath));
    }

    *curUriPath = '\0';

exit:
    return error;
}

Error Message::AppendBlockOption(Message::BlockType aType, uint32_t aNum, bool aMore, otCoapBlockSzx aSize)
{
    Error    error   = kErrorNone;
    uint32_t encoded = aSize;

    VerifyOrExit(aType == kBlockType1 || aType == kBlockType2, error = kErrorInvalidArgs);
    VerifyOrExit(aSize <= OT_COAP_OPTION_BLOCK_SZX_1024, error = kErrorInvalidArgs);
    VerifyOrExit(aNum < kBlockNumMax, error = kErrorInvalidArgs);

    encoded |= static_cast<uint32_t>(aMore << kBlockMOffset);
    encoded |= aNum << kBlockNumOffset;

    error = AppendUintOption((aType == kBlockType1) ? kOptionBlock1 : kOptionBlock2, encoded);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
Error Message::ReadBlockOptionValues(uint16_t aBlockType)
{
    Error            error                     = kErrorNone;
    uint8_t          buf[kMaxOptionHeaderSize] = {0};
    Option::Iterator iterator;

    VerifyOrExit((aBlockType == kOptionBlock1) || (aBlockType == kOptionBlock2), error = kErrorInvalidArgs);

    SuccessOrExit(error = iterator.Init(*this, aBlockType));
    SuccessOrExit(error = iterator.ReadOptionValue(buf));

    SetBlockWiseBlockNumber(0);
    SetMoreBlocksFlag(false);

    switch (iterator.GetOption()->GetLength())
    {
    case 0:
    case 1:
        SetBlockWiseBlockNumber(static_cast<uint32_t>((buf[0] & 0xf0) >> 4));
        SetMoreBlocksFlag(static_cast<bool>((buf[0] & 0x08) >> 3 == 1));
        SetBlockWiseBlockSize(static_cast<otCoapBlockSzx>(buf[0] & 0x07));
        break;
    case 2:
        SetBlockWiseBlockNumber(static_cast<uint32_t>((buf[0] << 4) + ((buf[1] & 0xf0) >> 4)));
        SetMoreBlocksFlag(static_cast<bool>((buf[1] & 0x08) >> 3 == 1));
        SetBlockWiseBlockSize(static_cast<otCoapBlockSzx>(buf[1] & 0x07));
        break;
    case 3:
        SetBlockWiseBlockNumber(static_cast<uint32_t>((buf[0] << 12) + (buf[1] << 4) + ((buf[2] & 0xf0) >> 4)));
        SetMoreBlocksFlag(static_cast<bool>((buf[2] & 0x08) >> 3 == 1));
        SetBlockWiseBlockSize(static_cast<otCoapBlockSzx>(buf[2] & 0x07));
        break;
    default:
        error = kErrorInvalidArgs;
        break;
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

Error Message::SetPayloadMarker(void)
{
    Error   error  = kErrorNone;
    uint8_t marker = kPayloadMarker;

    VerifyOrExit(GetLength() < kMaxHeaderLength, error = kErrorNoBufs);
    SuccessOrExit(error = Append(marker));
    GetHelpData().mPayloadMarkerSet = true;
    GetHelpData().mHeaderLength     = GetLength();

    // Set offset to the start of payload.
    SetOffset(GetHelpData().mHeaderLength);

exit:
    return error;
}

Error Message::ParseHeader(void)
{
    Error            error = kErrorNone;
    Option::Iterator iterator;

    OT_ASSERT(GetReserved() >=
              sizeof(HelpData) + static_cast<size_t>((reinterpret_cast<uint8_t *>(&GetHelpData()) - GetFirstData())));

    GetHelpData().Clear();

    GetHelpData().mHeaderOffset = GetOffset();
    IgnoreError(Read(GetHelpData().mHeaderOffset, GetHelpData().mHeader));

    VerifyOrExit(GetTokenLength() <= kMaxTokenLength, error = kErrorParse);

    SuccessOrExit(error = iterator.Init(*this));

    while (!iterator.IsDone())
    {
        SuccessOrExit(error = iterator.Advance());
    }

    GetHelpData().mHeaderLength = iterator.GetPayloadMessageOffset() - GetHelpData().mHeaderOffset;
    MoveOffset(GetHelpData().mHeaderLength);

exit:
    return error;
}

Error Message::SetToken(const uint8_t *aToken, uint8_t aTokenLength)
{
    OT_ASSERT(aTokenLength <= kMaxTokenLength);

    SetTokenLength(aTokenLength);
    memcpy(GetToken(), aToken, aTokenLength);
    GetHelpData().mHeaderLength += aTokenLength;

    return SetLength(GetHelpData().mHeaderLength);
}

Error Message::GenerateRandomToken(uint8_t aTokenLength)
{
    uint8_t token[kMaxTokenLength];

    OT_ASSERT(aTokenLength <= sizeof(token));

    IgnoreError(Random::Crypto::FillBuffer(token, aTokenLength));

    return SetToken(token, aTokenLength);
}

Error Message::SetTokenFromMessage(const Message &aMessage)
{
    return SetToken(aMessage.GetToken(), aMessage.GetTokenLength());
}

bool Message::IsTokenEqual(const Message &aMessage) const
{
    uint8_t tokenLength = GetTokenLength();

    return ((tokenLength == aMessage.GetTokenLength()) && (memcmp(GetToken(), aMessage.GetToken(), tokenLength) == 0));
}

Error Message::SetDefaultResponseHeader(const Message &aRequest)
{
    Init(kTypeAck, kCodeChanged);

    SetMessageId(aRequest.GetMessageId());

    return SetTokenFromMessage(aRequest);
}

Message *Message::Clone(uint16_t aLength) const
{
    Message *message = static_cast<Message *>(ot::Message::Clone(aLength));

    VerifyOrExit(message != nullptr);

    message->GetHelpData() = GetHelpData();

exit:
    return message;
}

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
const char *Message::CodeToString(void) const
{
    const char *string;

    switch (GetCode())
    {
    case kCodeEmpty:
        string = "Empty";
        break;
    case kCodeGet:
        string = "Get";
        break;
    case kCodePost:
        string = "Post";
        break;
    case kCodePut:
        string = "Put";
        break;
    case kCodeDelete:
        string = "Delete";
        break;
    case kCodeCreated:
        string = "Created";
        break;
    case kCodeDeleted:
        string = "Deleted";
        break;
    case kCodeValid:
        string = "Valid";
        break;
    case kCodeChanged:
        string = "Changed";
        break;
    case kCodeContent:
        string = "Content";
        break;
    case kCodeContinue:
        string = "Continue";
        break;
    case kCodeBadRequest:
        string = "BadRequest";
        break;
    case kCodeUnauthorized:
        string = "Unauthorized";
        break;
    case kCodeBadOption:
        string = "BadOption";
        break;
    case kCodeForbidden:
        string = "Forbidden";
        break;
    case kCodeNotFound:
        string = "NotFound";
        break;
    case kCodeMethodNotAllowed:
        string = "MethodNotAllowed";
        break;
    case kCodeNotAcceptable:
        string = "NotAcceptable";
        break;
    case kCodeRequestIncomplete:
        string = "RequestIncomplete";
        break;
    case kCodePreconditionFailed:
        string = "PreconditionFailed";
        break;
    case kCodeRequestTooLarge:
        string = "RequestTooLarge";
        break;
    case kCodeUnsupportedFormat:
        string = "UnsupportedFormat";
        break;
    case kCodeInternalError:
        string = "InternalError";
        break;
    case kCodeNotImplemented:
        string = "NotImplemented";
        break;
    case kCodeBadGateway:
        string = "BadGateway";
        break;
    case kCodeServiceUnavailable:
        string = "ServiceUnavailable";
        break;
    case kCodeGatewayTimeout:
        string = "GatewayTimeout";
        break;
    case kCodeProxyNotSupported:
        string = "ProxyNotSupported";
        break;
    default:
        string = "Unknown";
        break;
    }

    return string;
}
#endif // OPENTHREAD_CONFIG_COAP_API_ENABLE

Error Option::Iterator::Init(const Message &aMessage)
{
    Error    error  = kErrorParse;
    uint32_t offset = static_cast<uint32_t>(aMessage.GetHelpData().mHeaderOffset) + aMessage.GetOptionStart();

    // Note that the case where `offset == aMessage.GetLength())` is
    // valid and indicates an empty payload (no CoAP Option and no
    // Payload Marker).

    VerifyOrExit(offset <= aMessage.GetLength(), MarkAsParseErrored());

    mOption.mNumber   = 0;
    mOption.mLength   = 0;
    mMessage          = &aMessage;
    mNextOptionOffset = static_cast<uint16_t>(offset);

    error = Advance();

exit:
    return error;
}

Error Option::Iterator::Advance(void)
{
    Error    error = kErrorNone;
    uint8_t  headerByte;
    uint16_t optionDelta;
    uint16_t optionLength;

    VerifyOrExit(!IsDone());

    error = Read(sizeof(uint8_t), &headerByte);

    if ((error != kErrorNone) || (headerByte == Message::kPayloadMarker))
    {
        // Payload Marker indicates end of options and start of payload.
        // Absence of a Payload Marker indicates a zero-length payload.

        MarkAsDone();

        if (error == kErrorNone)
        {
            // The presence of a marker followed by a zero-length payload
            // MUST be processed as a message format error.

            VerifyOrExit(mNextOptionOffset < GetMessage().GetLength(), error = kErrorParse);
        }

        ExitNow(error = kErrorNone);
    }

    optionDelta = (headerByte & Message::kOptionDeltaMask) >> Message::kOptionDeltaOffset;
    SuccessOrExit(error = ReadExtendedOptionField(optionDelta));

    optionLength = (headerByte & Message::kOptionLengthMask) >> Message::kOptionLengthOffset;
    SuccessOrExit(error = ReadExtendedOptionField(optionLength));

    VerifyOrExit(optionLength <= GetMessage().GetLength() - mNextOptionOffset, error = kErrorParse);
    mNextOptionOffset += optionLength;

    mOption.mNumber += optionDelta;
    mOption.mLength = optionLength;

exit:
    if (error != kErrorNone)
    {
        MarkAsParseErrored();
    }

    return error;
}

Error Option::Iterator::ReadOptionValue(void *aValue) const
{
    Error error = kErrorNone;

    VerifyOrExit(!IsDone(), error = kErrorNotFound);
    GetMessage().ReadBytes(mNextOptionOffset - mOption.mLength, aValue, mOption.mLength);

exit:
    return error;
}

Error Option::Iterator::ReadOptionValue(uint64_t &aUintValue) const
{
    Error   error = kErrorNone;
    uint8_t buffer[sizeof(uint64_t)];

    VerifyOrExit(!IsDone(), error = kErrorNotFound);

    VerifyOrExit(mOption.mLength <= sizeof(uint64_t), error = kErrorNoBufs);
    IgnoreError(ReadOptionValue(buffer));

    aUintValue = 0;

    for (uint16_t pos = 0; pos < mOption.mLength; pos++)
    {
        aUintValue <<= CHAR_BIT;
        aUintValue |= buffer[pos];
    }

exit:
    return error;
}

Error Option::Iterator::Read(uint16_t aLength, void *aBuffer)
{
    // Reads `aLength` bytes from the message into `aBuffer` at
    // `mNextOptionOffset` and updates the `mNextOptionOffset` on a
    // successful read (i.e., when entire `aLength` bytes can be read).

    Error error = kErrorNone;

    SuccessOrExit(error = GetMessage().Read(mNextOptionOffset, aBuffer, aLength));
    mNextOptionOffset += aLength;

exit:
    return error;
}

Error Option::Iterator::ReadExtendedOptionField(uint16_t &aValue)
{
    Error error = kErrorNone;

    VerifyOrExit(aValue >= Message::kOption1ByteExtension);

    if (aValue == Message::kOption1ByteExtension)
    {
        uint8_t value8;

        SuccessOrExit(error = Read(sizeof(uint8_t), &value8));
        aValue = static_cast<uint16_t>(value8) + Message::kOption1ByteExtensionOffset;
    }
    else if (aValue == Message::kOption2ByteExtension)
    {
        uint16_t value16;

        SuccessOrExit(error = Read(sizeof(uint16_t), &value16));
        value16 = Encoding::BigEndian::HostSwap16(value16);
        aValue  = value16 + Message::kOption2ByteExtensionOffset;
    }
    else
    {
        error = kErrorParse;
    }

exit:
    return error;
}

Error Option::Iterator::InitOrAdvance(const Message *aMessage, uint16_t aNumber)
{
    Error error = (aMessage != nullptr) ? Init(*aMessage) : Advance();

    while ((error == kErrorNone) && !IsDone() && (GetOption()->GetNumber() != aNumber))
    {
        error = Advance();
    }

    return error;
}

} // namespace Coap
} // namespace ot
