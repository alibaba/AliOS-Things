/*
 *    Copyright (c) 2017, The OpenThread Authors.
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
 *   This file implements a spinel decoder.
 */

#include "spinel_decoder.hpp"

#include "common/code_utils.hpp"
#include "common/string.hpp"

namespace ot {
namespace Spinel {

Decoder::Decoder(void)
    : mFrame(nullptr)
    , mLength(0)
    , mIndex(0)
    , mEnd(0)
    , mNumOpenStructs(0)
    , mSavedNumOpenStructs(0)
    , mSavedIndex(0)
    , mSavedEnd(0)
{
}

void Decoder::Init(const uint8_t *aFrame, uint16_t aLength)
{
    mFrame  = aFrame;
    mLength = (mFrame != nullptr) ? aLength : 0;

    Reset();
    ClearSavedPosition();
}

void Decoder::Reset(void)
{
    mIndex          = 0;
    mEnd            = mLength;
    mNumOpenStructs = 0;
    ClearSavedPosition();
}

otError Decoder::ReadBool(bool &aBool)
{
    otError error = OT_ERROR_NONE;
    uint8_t byte;

    SuccessOrExit(error = ReadUint8(byte));

    // Boolean value are encoded in 8-bits as either 0x00 or 0x01. All other values are illegal.
    if (byte == 0x00)
    {
        aBool = false;
    }
    else if (byte == 0x01)
    {
        aBool = true;
    }
    else
    {
        error = OT_ERROR_PARSE;
    }

exit:
    return error;
}

otError Decoder::ReadUint8(uint8_t &aUint8)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mIndex + sizeof(uint8_t) <= mEnd, error = OT_ERROR_PARSE);
    aUint8 = mFrame[mIndex];
    mIndex += sizeof(uint8_t);

exit:
    return error;
}

otError Decoder::ReadInt8(int8_t &aInt8)
{
    otError error = OT_ERROR_NONE;
    uint8_t byte;

    SuccessOrExit(error = ReadUint8(byte));
    aInt8 = static_cast<int8_t>(byte);

exit:
    return error;
}

otError Decoder::ReadUint16(uint16_t &aUint16)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mIndex + sizeof(uint16_t) <= mEnd, error = OT_ERROR_PARSE);

    aUint16 = static_cast<uint16_t>(mFrame[mIndex] | (mFrame[mIndex + 1] << 8));

    mIndex += sizeof(uint16_t);

exit:
    return error;
}

otError Decoder::ReadInt16(int16_t &aInt16)
{
    otError  error = OT_ERROR_NONE;
    uint16_t u16;

    SuccessOrExit(error = ReadUint16(u16));
    aInt16 = static_cast<int16_t>(u16);

exit:
    return error;
}

otError Decoder::ReadUint32(uint32_t &aUint32)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mIndex + sizeof(uint32_t) <= mEnd, error = OT_ERROR_PARSE);

    aUint32 = ((static_cast<uint32_t>(mFrame[mIndex + 0]) << 0) | (static_cast<uint32_t>(mFrame[mIndex + 1]) << 8) |
               (static_cast<uint32_t>(mFrame[mIndex + 2]) << 16) | (static_cast<uint32_t>(mFrame[mIndex + 3]) << 24));

    mIndex += sizeof(uint32_t);

exit:
    return error;
}

otError Decoder::ReadInt32(int32_t &aInt32)
{
    otError  error = OT_ERROR_NONE;
    uint32_t u32;

    SuccessOrExit(error = ReadUint32(u32));
    aInt32 = static_cast<int32_t>(u32);

exit:
    return error;
}

otError Decoder::ReadUint64(uint64_t &aUint64)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mIndex + sizeof(uint64_t) <= mEnd, error = OT_ERROR_PARSE);

    aUint64 = ((static_cast<uint64_t>(mFrame[mIndex + 0]) << 0) | (static_cast<uint64_t>(mFrame[mIndex + 1]) << 8) |
               (static_cast<uint64_t>(mFrame[mIndex + 2]) << 16) | (static_cast<uint64_t>(mFrame[mIndex + 3]) << 24) |
               (static_cast<uint64_t>(mFrame[mIndex + 4]) << 32) | (static_cast<uint64_t>(mFrame[mIndex + 5]) << 40) |
               (static_cast<uint64_t>(mFrame[mIndex + 6]) << 48) | (static_cast<uint64_t>(mFrame[mIndex + 7]) << 56));

    mIndex += sizeof(uint64_t);

exit:
    return error;
}

otError Decoder::ReadInt64(int64_t &aInt64)
{
    otError  error = OT_ERROR_NONE;
    uint64_t u64;

    SuccessOrExit(error = ReadUint64(u64));
    aInt64 = static_cast<int64_t>(u64);

exit:
    return error;
}

otError Decoder::ReadUintPacked(unsigned int &aUint)
{
    otError        error = OT_ERROR_NONE;
    spinel_ssize_t parsedLen;
    unsigned int   uint;

    parsedLen = spinel_packed_uint_decode(&mFrame[mIndex], mEnd - mIndex, &uint);
    VerifyOrExit(parsedLen > 0, error = OT_ERROR_PARSE);

    mIndex += parsedLen;
    aUint = uint;

exit:
    return error;
}

// Reads an item of given size and updates the pointer `aPtr`.
otError Decoder::ReadItem(const uint8_t **aPtr, uint16_t aSize)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mIndex + aSize <= mEnd, error = OT_ERROR_PARSE);

    *aPtr = &mFrame[mIndex];

    mIndex += aSize;

exit:
    return error;
}

otError Decoder::ReadIp6Address(spinel_ipv6addr_t &aIp6Addr)
{
    otError                  error       = OT_ERROR_NONE;
    const spinel_ipv6addr_t *ipv6AddrPtr = nullptr;

    SuccessOrExit(error = ReadIp6Address(ipv6AddrPtr));
    VerifyOrExit(ipv6AddrPtr != nullptr, error = OT_ERROR_PARSE);
    aIp6Addr = *ipv6AddrPtr;

exit:
    return error;
}

otError Decoder::ReadIp6Address(otIp6Address &aIp6Addr)
{
    otError             error       = OT_ERROR_NONE;
    const otIp6Address *ipv6AddrPtr = nullptr;

    SuccessOrExit(error = ReadIp6Address(ipv6AddrPtr));
    VerifyOrExit(ipv6AddrPtr != nullptr, error = OT_ERROR_PARSE);
    aIp6Addr = *ipv6AddrPtr;

exit:
    return error;
}

otError Decoder::ReadEui64(spinel_eui64_t &aEui64)
{
    otError               error    = OT_ERROR_NONE;
    const spinel_eui64_t *eui64Ptr = nullptr;

    SuccessOrExit(error = ReadEui64(eui64Ptr));
    VerifyOrExit(eui64Ptr != nullptr, error = OT_ERROR_PARSE);
    aEui64 = *eui64Ptr;

exit:
    return error;
}

otError Decoder::ReadEui64(otExtAddress &aEui64)
{
    otError             error    = OT_ERROR_NONE;
    const otExtAddress *eui64Ptr = nullptr;

    SuccessOrExit(error = ReadEui64(eui64Ptr));
    VerifyOrExit(eui64Ptr != nullptr, error = OT_ERROR_PARSE);
    aEui64 = *eui64Ptr;

exit:
    return error;
}

otError Decoder::ReadEui48(spinel_eui48_t &aEui48)
{
    otError               error    = OT_ERROR_NONE;
    const spinel_eui48_t *eui48Ptr = nullptr;

    SuccessOrExit(error = ReadEui48(eui48Ptr));
    VerifyOrExit(eui48Ptr != nullptr, error = OT_ERROR_PARSE);
    aEui48 = *eui48Ptr;

exit:
    return error;
}

otError Decoder::ReadUtf8(const char *&aUtf8)
{
    otError error = OT_ERROR_NONE;
    size_t  len;

    // Ensure there is at least one byte (for null character).
    VerifyOrExit(mIndex + sizeof(uint8_t) <= mEnd, error = OT_ERROR_PARSE);

    len = StringLength(reinterpret_cast<const char *>(&mFrame[mIndex]), mEnd - mIndex);
    VerifyOrExit(len < static_cast<uint16_t>(mEnd - mIndex), error = OT_ERROR_PARSE);

    aUtf8 = reinterpret_cast<const char *>(&mFrame[mIndex]);

    // `sizeof(uint8_t)` is added for the terminating null character.
    mIndex += static_cast<uint16_t>(len + sizeof(uint8_t));

exit:
    return error;
}

otError Decoder::ReadData(const uint8_t *&aData, uint16_t &aDataLen)
{
    aDataLen = mEnd - mIndex;

    return ReadItem(&aData, aDataLen);
}

otError Decoder::ReadDataWithLen(const uint8_t *&aData, uint16_t &aDataLen)
{
    otError  error = OT_ERROR_NONE;
    uint16_t len;

    SuccessOrExit(error = ReadUint16(len));
    SuccessOrExit(error = ReadItem(&aData, len));
    aDataLen = len;

exit:
    return error;
}

otError Decoder::OpenStruct(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t structLen;

    VerifyOrExit(mNumOpenStructs < kMaxNestedStructs, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = ReadUint16(structLen));
    VerifyOrExit(structLen <= mEnd - mIndex, error = OT_ERROR_PARSE);

    mPrevEnd[mNumOpenStructs] = mEnd;
    mEnd                      = (mIndex + structLen);
    mNumOpenStructs++;

exit:
    return error;
}

otError Decoder::CloseStruct(void)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mNumOpenStructs > 0, error = OT_ERROR_INVALID_STATE);

    // If there is a saved position and it is contained
    // within the current struct being closed, the saved
    // position is cleared to ensure user cannot go back
    // to middle of an already closed struct.

    if (IsSavedPositionValid() && (mNumOpenStructs == mSavedNumOpenStructs))
    {
        ClearSavedPosition();
    }

    mNumOpenStructs--;
    mIndex = mEnd;
    mEnd   = mPrevEnd[mNumOpenStructs];

exit:
    return error;
}

void Decoder::SavePosition(void)
{
    mSavedIndex          = mIndex;
    mSavedEnd            = mEnd;
    mSavedNumOpenStructs = mNumOpenStructs;
}

otError Decoder::ResetToSaved(void)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(IsSavedPositionValid(), error = OT_ERROR_INVALID_STATE);

    mIndex          = mSavedIndex;
    mEnd            = mSavedEnd;
    mNumOpenStructs = mSavedNumOpenStructs;

exit:
    return error;
}

} // namespace Spinel
} // namespace ot
