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
 *   This file implements a spinel encoder.
 */

#include "spinel_encoder.hpp"

#include <string.h>

#include "common/code_utils.hpp"

namespace ot {
namespace Spinel {

otError Encoder::BeginFrame(Spinel::Buffer::Priority aPriority)
{
    mNumOpenStructs = 0;
    mNcpBuffer.InFrameBegin(aPriority);
    return OT_ERROR_NONE;
}

otError Encoder::BeginFrame(uint8_t aHeader, unsigned int aCommand)
{
    otError error = OT_ERROR_NONE;

    // Non-zero TID indicates this is a response to a spinel command.

    if (SPINEL_HEADER_GET_TID(aHeader) != 0)
    {
        SuccessOrExit(error = BeginFrame(Spinel::Buffer::kPriorityHigh));
    }
    else
    {
        SuccessOrExit(error = BeginFrame(Spinel::Buffer::kPriorityLow));
    }

    SuccessOrExit(error = WriteUint8(aHeader));
    SuccessOrExit(error = WriteUintPacked(aCommand));

exit:
    return error;
}

otError Encoder::BeginFrame(uint8_t aHeader, unsigned int aCommand, spinel_prop_key_t aKey)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = BeginFrame(aHeader, aCommand));

    // The write position is saved before writing the property key,
    // so that if fetching the property fails and we need to
    // reply with a `LAST_STATUS` error we can get back to
    // this saved write position and update the property key.
    // (Also see `OverwriteWithLastStatusError()`).

    SuccessOrExit(error = SavePosition());
    SuccessOrExit(error = WriteUintPacked(aKey));

exit:
    return error;
}

otError Encoder::OverwriteWithLastStatusError(spinel_status_t aStatus)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = ResetToSaved());
    SuccessOrExit(error = WriteUintPacked(SPINEL_PROP_LAST_STATUS));
    SuccessOrExit(error = WriteUintPacked(aStatus));

exit:
    return error;
}

otError Encoder::EndFrame(void)
{
    otError error = OT_ERROR_NONE;

    while (mNumOpenStructs > 0)
    {
        SuccessOrExit(error = CloseStruct());
    }

    error = mNcpBuffer.InFrameEnd();

exit:
    return error;
}

otError Encoder::WriteUint16(uint16_t aUint16)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint16 >> 0) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint16 >> 8) & 0xff));

exit:
    return error;
}

otError Encoder::WriteUint32(uint32_t aUint32)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint32 >> 0) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint32 >> 8) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint32 >> 16) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint32 >> 24) & 0xff));

exit:
    return error;
}

otError Encoder::WriteUint64(uint64_t aUint64)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 0) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 8) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 16) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 24) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 32) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 40) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 48) & 0xff));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte((aUint64 >> 56) & 0xff));

exit:
    return error;
}

otError Encoder::WriteUintPacked(unsigned int aUint)
{
    uint8_t        buffer[6];
    spinel_ssize_t len;

    len = spinel_packed_uint_encode(buffer, sizeof(buffer), aUint);

    return WriteData(buffer, static_cast<uint16_t>(len));
}

otError Encoder::WriteDataWithLen(const uint8_t *aData, uint16_t aDataLen)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = WriteUint16(aDataLen));
    SuccessOrExit(error = WriteData(aData, aDataLen));

exit:
    return error;
}

otError Encoder::WriteUtf8(const char *aUtf8)
{
    otError error;
    size_t  len = strlen(aUtf8);

    if (len >= 0xffff)
    {
        len = 0xffff;
    }

    SuccessOrExit(error = WriteData(reinterpret_cast<const uint8_t *>(aUtf8), static_cast<uint16_t>(len)));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte(0));

exit:
    return error;
}

otError Encoder::OpenStruct(void)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mNumOpenStructs < kMaxNestedStructs, error = OT_ERROR_INVALID_STATE);
    SuccessOrExit(error = mNcpBuffer.InFrameGetPosition(mStructPosition[mNumOpenStructs]));

    // Reserve bytes for the length to be filled when the struct gets closed.
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte(0));
    SuccessOrExit(error = mNcpBuffer.InFrameFeedByte(0));

    mNumOpenStructs++;

exit:
    return error;
}

otError Encoder::CloseStruct(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t len;
    uint8_t  buffer[sizeof(uint16_t)];

    VerifyOrExit(mNumOpenStructs > 0, error = OT_ERROR_INVALID_STATE);

    mNumOpenStructs--;

    len = mNcpBuffer.InFrameGetDistance(mStructPosition[mNumOpenStructs]);
    VerifyOrExit(len >= sizeof(uint16_t), error = OT_ERROR_INVALID_STATE);

    len -= sizeof(uint16_t);

    buffer[0] = (len >> 0 & 0xff);
    buffer[1] = (len >> 8 & 0xff);

    SuccessOrExit(error = mNcpBuffer.InFrameOverwrite(mStructPosition[mNumOpenStructs], buffer, sizeof(buffer)));

exit:
    return error;
}

otError Encoder::SavePosition(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcpBuffer.InFrameGetPosition(mSavedPosition));
    mSavedNumOpenStructs = mNumOpenStructs;

exit:
    return error;
}

otError Encoder::ResetToSaved(void)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcpBuffer.InFrameReset(mSavedPosition));
    mNumOpenStructs = mSavedNumOpenStructs;

exit:
    return error;
}

otError Encoder::WritePacked(const char *aPackFormat, ...)
{
    uint8_t        buf[kPackFormatBufferSize];
    otError        error = OT_ERROR_NONE;
    spinel_ssize_t packedLen;
    va_list        args;

    va_start(args, aPackFormat);

    packedLen = spinel_datatype_vpack(buf, sizeof(buf), aPackFormat, args);
    VerifyOrExit((packedLen > 0) && (packedLen <= static_cast<spinel_ssize_t>(sizeof(buf))), error = OT_ERROR_NO_BUFS);

    error = mNcpBuffer.InFrameFeedData(buf, static_cast<uint16_t>(packedLen));

exit:
    va_end(args);

    return error;
}

otError Encoder::WriteVPacked(const char *aPackFormat, va_list aArgs)
{
    uint8_t        buf[kPackFormatBufferSize];
    otError        error = OT_ERROR_NONE;
    spinel_ssize_t packedLen;

    packedLen = spinel_datatype_vpack(buf, sizeof(buf), aPackFormat, aArgs);
    VerifyOrExit((packedLen > 0) && (packedLen <= static_cast<spinel_ssize_t>(sizeof(buf))), error = OT_ERROR_NO_BUFS);

    error = mNcpBuffer.InFrameFeedData(buf, static_cast<uint16_t>(packedLen));

exit:
    return error;
}

} // namespace Spinel
} // namespace ot
