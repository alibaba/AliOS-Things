/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *  This file implements OpenThread String class.
 */

#include "string.hpp"
#include "debug.hpp"

#include <string.h>

namespace ot {

uint16_t StringLength(const char *aString, uint16_t aMaxLength)
{
    uint16_t ret;

    for (ret = 0; (ret < aMaxLength) && (aString[ret] != 0); ret++)
    {
        // Empty loop.
    }

    return ret;
}

const char *StringFind(const char *aString, char aChar)
{
    const char *ret = nullptr;

    for (; *aString != '\0'; aString++)
    {
        if (*aString == aChar)
        {
            ret = aString;
            break;
        }
    }

    return ret;
}

const char *StringFind(const char *aString, const char *aSubString)
{
    const char *ret    = nullptr;
    size_t      len    = strlen(aString);
    size_t      subLen = strlen(aSubString);

    VerifyOrExit(subLen <= len);

    for (size_t index = 0; index <= static_cast<size_t>(len - subLen); index++)
    {
        if (memcmp(&aString[index], aSubString, subLen) == 0)
        {
            ExitNow(ret = &aString[index]);
        }
    }

exit:
    return ret;
}

bool StringEndsWith(const char *aString, char aChar)
{
    size_t len = strlen(aString);

    return len > 0 && aString[len - 1] == aChar;
}

bool StringEndsWith(const char *aString, const char *aSubString)
{
    size_t len    = strlen(aString);
    size_t subLen = strlen(aSubString);

    return (subLen > 0) && (len >= subLen) && (memcmp(aSubString, &aString[len - subLen], subLen) == 0);
}

StringWriter::StringWriter(char *aBuffer, uint16_t aSize)
    : mBuffer(aBuffer)
    , mLength(0)
    , mSize(aSize)
{
    mBuffer[0] = '\0';
}

StringWriter &StringWriter::Clear(void)
{
    mBuffer[0] = '\0';
    mLength    = 0;
    return *this;
}

StringWriter &StringWriter::Append(const char *aFormat, ...)
{
    va_list args;
    va_start(args, aFormat);
    AppendVarArgs(aFormat, args);
    va_end(args);

    return *this;
}

StringWriter &StringWriter::AppendVarArgs(const char *aFormat, va_list aArgs)
{
    int len;

    len = vsnprintf(mBuffer + mLength, (mSize > mLength ? (mSize - mLength) : 0), aFormat, aArgs);
    OT_ASSERT(len >= 0);

    mLength += static_cast<uint16_t>(len);

    if (IsTruncated())
    {
        mBuffer[mSize - 1] = '\0';
    }

    return *this;
}

StringWriter &StringWriter::AppendHexBytes(const uint8_t *aBytes, uint16_t aLength)
{
    while (aLength--)
    {
        Append("%02x", *aBytes++);
    }

    return *this;
}

bool IsValidUtf8String(const char *aString)
{
    return IsValidUtf8String(aString, strlen(aString));
}

bool IsValidUtf8String(const char *aString, size_t aLength)
{
    bool    ret = true;
    uint8_t byte;
    uint8_t continuationBytes = 0;
    size_t  position          = 0;

    while (position < aLength)
    {
        byte = *reinterpret_cast<const uint8_t *>(aString + position);
        ++position;

        if ((byte & 0x80) == 0)
        {
            // We don't allow control characters.
            VerifyOrExit(!iscntrl(byte), ret = false);
            continue;
        }

        // This is a leading byte 1xxx-xxxx.

        if ((byte & 0x40) == 0) // 10xx-xxxx
        {
            // We got a continuation byte pattern without seeing a leading byte earlier.
            ExitNow(ret = false);
        }
        else if ((byte & 0x20) == 0) // 110x-xxxx
        {
            continuationBytes = 1;
        }
        else if ((byte & 0x10) == 0) // 1110-xxxx
        {
            continuationBytes = 2;
        }
        else if ((byte & 0x08) == 0) // 1111-0xxx
        {
            continuationBytes = 3;
        }
        else // 1111-1xxx  (invalid pattern).
        {
            ExitNow(ret = false);
        }

        while (continuationBytes-- != 0)
        {
            VerifyOrExit(position < aLength, ret = false);

            byte = *reinterpret_cast<const uint8_t *>(aString + position);
            ++position;

            // Verify the continuation byte pattern 10xx-xxxx
            VerifyOrExit((byte & 0xc0) == 0x80, ret = false);
        }
    }

exit:
    return ret;
}

} // namespace ot
