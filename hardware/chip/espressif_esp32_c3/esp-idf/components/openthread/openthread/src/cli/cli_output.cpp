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
 *   This file contains implementation of the CLI output module.
 */

#include "cli_output.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if OPENTHREAD_FTD || OPENTHREAD_MTD
#include <openthread/dns.h>
#endif

#include "common/logging.hpp"
#include "common/string.hpp"

namespace ot {
namespace Cli {

Output::Output(otInstance *aInstance, otCliOutputCallback aCallback, void *aCallbackContext)
    : mInstance(aInstance)
    , mCallback(aCallback)
    , mCallbackContext(aCallbackContext)
#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
    , mOutputLength(0)
    , mEmittingCommandOutput(true)
#endif
{
}

void Output::OutputFormat(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    OutputFormatV(aFormat, args);
    va_end(args);
}

void Output::OutputFormat(uint8_t aIndentSize, const char *aFormat, ...)
{
    va_list args;

    OutputSpaces(aIndentSize);

    va_start(args, aFormat);
    OutputFormatV(aFormat, args);
    va_end(args);
}

void Output::OutputLine(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    OutputFormatV(aFormat, args);
    va_end(args);

    OutputFormat("\r\n");
}

void Output::OutputLine(uint8_t aIndentSize, const char *aFormat, ...)
{
    va_list args;

    OutputSpaces(aIndentSize);

    va_start(args, aFormat);
    OutputFormatV(aFormat, args);
    va_end(args);

    OutputFormat("\r\n");
}

void Output::OutputSpaces(uint8_t aCount)
{
    char format[sizeof("%256s")];

    snprintf(format, sizeof(format), "%%%us", aCount);

    OutputFormat(format, "");
}

void Output::OutputBytes(const uint8_t *aBytes, uint16_t aLength)
{
    for (uint16_t i = 0; i < aLength; i++)
    {
        OutputFormat("%02x", aBytes[i]);
    }
}

void Output::OutputBytesLine(const uint8_t *aBytes, uint16_t aLength)
{
    OutputBytes(aBytes, aLength);
    OutputLine("");
}

void Output::OutputEnabledDisabledStatus(bool aEnabled)
{
    OutputLine(aEnabled ? "Enabled" : "Disabled");
}

#if OPENTHREAD_FTD || OPENTHREAD_MTD

void Output::OutputIp6Address(const otIp6Address &aAddress)
{
    char string[OT_IP6_ADDRESS_STRING_SIZE];

    otIp6AddressToString(&aAddress, string, sizeof(string));

    return OutputFormat("%s", string);
}

void Output::OutputIp6AddressLine(const otIp6Address &aAddress)
{
    OutputIp6Address(aAddress);
    OutputLine("");
}

void Output::OutputIp6Prefix(const otIp6Prefix &aPrefix)
{
    char string[OT_IP6_PREFIX_STRING_SIZE];

    otIp6PrefixToString(&aPrefix, string, sizeof(string));

    OutputFormat("%s", string);
}

void Output::OutputIp6PrefixLine(const otIp6Prefix &aPrefix)
{
    OutputIp6Prefix(aPrefix);
    OutputLine("");
}

void Output::OutputIp6Prefix(const otIp6NetworkPrefix &aPrefix)
{
    OutputFormat("%x:%x:%x:%x::/64", (aPrefix.m8[0] << 8) | aPrefix.m8[1], (aPrefix.m8[2] << 8) | aPrefix.m8[3],
                 (aPrefix.m8[4] << 8) | aPrefix.m8[5], (aPrefix.m8[6] << 8) | aPrefix.m8[7]);
}

void Output::OutputIp6PrefixLine(const otIp6NetworkPrefix &aPrefix)
{
    OutputIp6Prefix(aPrefix);
    OutputLine("");
}

void Output::OutputDnsTxtData(const uint8_t *aTxtData, uint16_t aTxtDataLength)
{
    otDnsTxtEntry         entry;
    otDnsTxtEntryIterator iterator;
    bool                  isFirst = true;

    otDnsInitTxtEntryIterator(&iterator, aTxtData, aTxtDataLength);

    OutputFormat("[");

    while (otDnsGetNextTxtEntry(&iterator, &entry) == OT_ERROR_NONE)
    {
        if (!isFirst)
        {
            OutputFormat(", ");
        }

        if (entry.mKey == nullptr)
        {
            // A null `mKey` indicates that the key in the entry is
            // longer than the recommended max key length, so the entry
            // could not be parsed. In this case, the whole entry is
            // returned encoded in `mValue`.

            OutputFormat("[");
            OutputBytes(entry.mValue, entry.mValueLength);
            OutputFormat("]");
        }
        else
        {
            OutputFormat("%s", entry.mKey);

            if (entry.mValue != nullptr)
            {
                OutputFormat("=");
                OutputBytes(entry.mValue, entry.mValueLength);
            }
        }

        isFirst = false;
    }

    OutputFormat("]");
}
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

void Output::OutputFormatV(const char *aFormat, va_list aArguments)
{
#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
    va_list args;
    int     charsWritten;
    bool    truncated = false;

    va_copy(args, aArguments);
#endif

    mCallback(mCallbackContext, aFormat, aArguments);

#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
    VerifyOrExit(mEmittingCommandOutput);

    charsWritten = vsnprintf(&mOutputString[mOutputLength], sizeof(mOutputString) - mOutputLength, aFormat, args);

    VerifyOrExit(charsWritten >= 0, mOutputLength = 0);

    if (static_cast<uint32_t>(charsWritten) >= sizeof(mOutputString) - mOutputLength)
    {
        truncated     = true;
        mOutputLength = sizeof(mOutputString) - 1;
    }
    else
    {
        mOutputLength += charsWritten;
    }

    while (true)
    {
        char *lineEnd = strchr(mOutputString, '\r');

        if (lineEnd == nullptr)
        {
            break;
        }

        *lineEnd = '\0';

        if (lineEnd > mOutputString)
        {
            otLogNoteCli("Output: %s", mOutputString);
        }

        lineEnd++;

        while ((*lineEnd == '\n') || (*lineEnd == '\r'))
        {
            lineEnd++;
        }

        // Example of the pointers and lengths.
        //
        // - mOutputString = "hi\r\nmore"
        // - mOutputLength = 8
        // - lineEnd       = &mOutputString[4]
        //
        //
        //   0    1    2    3    4    5    6    7    8    9
        // +----+----+----+----+----+----+----+----+----+---
        // | h  | i  | \r | \n | m  | o  | r  | e  | \0 |
        // +----+----+----+----+----+----+----+----+----+---
        //                       ^                   ^
        //                       |                   |
        //                    lineEnd    mOutputString[mOutputLength]
        //
        //
        // New length is `&mOutputString[8] - &mOutputString[4] -> 4`.
        //
        // We move (newLen + 1 = 5) chars from `lineEnd` to start of
        // `mOutputString` which will include the `\0` char.
        //
        // If `lineEnd` and `mOutputString[mOutputLength]` are the same
        // the code works correctly as well  (new length set to zero and
        // the `\0` is copied).

        mOutputLength = static_cast<uint16_t>(&mOutputString[mOutputLength] - lineEnd);
        memmove(mOutputString, lineEnd, mOutputLength + 1);
    }

    if (truncated)
    {
        otLogNoteCli("Output: %s ...", mOutputString);
        mOutputLength = 0;
    }

exit:
    va_end(args);
#endif // OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
}

#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
void Output::LogInput(const Arg *aArgs)
{
    String<kInputOutputLogStringSize> inputString;

    for (bool isFirst = true; !aArgs->IsEmpty(); aArgs++, isFirst = false)
    {
        inputString.Append(isFirst ? "%s" : " %s", aArgs->GetCString());
    }

    otLogNoteCli("Input: %s", inputString.AsCString());
}
#endif

void Output::OutputTableHeader(uint8_t aNumColumns, const char *const aTitles[], const uint8_t aWidths[])
{
    for (uint8_t index = 0; index < aNumColumns; index++)
    {
        const char *title       = aTitles[index];
        uint8_t     width       = aWidths[index];
        size_t      titleLength = strlen(title);

        if (titleLength + 2 <= width)
        {
            // `title` fits in column width so we write it with extra space
            // at beginning and end ("| Title    |").

            OutputFormat("| %*s", -static_cast<int>(width - 1), title);
        }
        else
        {
            // Use narrow style (no space at beginning) and write as many
            // chars from `title` as it can fit in the given column width
            // ("|Title|").

            OutputFormat("|%*.*s", -static_cast<int>(width), width, title);
        }
    }

    OutputLine("|");
    OutputTableSeparator(aNumColumns, aWidths);
}

void Output::OutputTableSeparator(uint8_t aNumColumns, const uint8_t aWidths[])
{
    for (uint8_t index = 0; index < aNumColumns; index++)
    {
        OutputFormat("+");

        for (uint8_t width = aWidths[index]; width != 0; width--)
        {
            OutputFormat("-");
        }
    }

    OutputLine("+");
}

} // namespace Cli
} // namespace ot
