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
 *   This file contains definitions for the CLI output.
 */

#ifndef CLI_OUTPUT_HPP_
#define CLI_OUTPUT_HPP_

#include "openthread-core-config.h"

#include <stdarg.h>

#include <openthread/cli.h>

#include "cli_config.h"
#include "utils/parse_cmdline.hpp"

namespace ot {
namespace Cli {

/**
 * This class provides CLI output helper methods.
 *
 */
class Output
{
public:
    /**
     * This constructor initializes the `Output` object.
     *
     * @param[in] aInstance           A pointer to OpenThread instance.
     * @param[in] aCallback           A pointer to an `otCliOutputCallback` to deliver strings to the CLI console.
     * @param[in] aCallbackContext    An arbitrary context to pass in when invoking @p aCallback.
     *
     */
    Output(otInstance *aInstance, otCliOutputCallback aCallback, void *aCallbackContext);

    /**
     * This method returns the pointer to OpenThread instance.
     *
     * @returns The pointer to the OpenThread instance.
     *
     */
    otInstance *GetInstancePtr(void) { return mInstance; }

    /**
     * This method delivers a formatted output string to the CLI console.
     *
     * @param[in]  aFormat  A pointer to the format string.
     * @param[in]  ...      A variable list of arguments to format.
     *
     */
    void OutputFormat(const char *aFormat, ...);

    /**
     * This method delivers a formatted output string to the CLI console (to which it prepends a given number
     * indentation space chars).
     *
     * @param[in]  aIndentSize   Number of indentation space chars to prepend to the string.
     * @param[in]  aFormat       A pointer to the format string.
     * @param[in]  ...           A variable list of arguments to format.
     *
     */
    void OutputFormat(uint8_t aIndentSize, const char *aFormat, ...);

    /**
     * This method delivers a formatted output string to the CLI console (to which it also appends newline "\r\n").
     *
     * @param[in]  aFormat  A pointer to the format string.
     * @param[in]  ...      A variable list of arguments to format.
     *
     */
    void OutputLine(const char *aFormat, ...);

    /**
     * This method delivers a formatted output string to the CLI console (to which it prepends a given number
     * indentation space chars and appends newline "\r\n").
     *
     * @param[in]  aIndentSize   Number of indentation space chars to prepend to the string.
     * @param[in]  aFormat       A pointer to the format string.
     * @param[in]  ...           A variable list of arguments to format.
     *
     */
    void OutputLine(uint8_t aIndentSize, const char *aFormat, ...);

    /**
     * This method outputs a given number of space chars to the CLI console.
     *
     * @param[in] aCount  Number of space chars to output.
     *
     */
    void OutputSpaces(uint8_t aCount);

    /**
     * This method outputs a number of bytes to the CLI console as a hex string.
     *
     * @param[in]  aBytes   A pointer to data which should be printed.
     * @param[in]  aLength  @p aBytes length.
     *
     */
    void OutputBytes(const uint8_t *aBytes, uint16_t aLength);

    /**
     * This method outputs a number of bytes to the CLI console as a hex string and at the end it also outputs newline
     * "\r\n".
     *
     * @param[in]  aBytes   A pointer to data which should be printed.
     * @param[in]  aLength  @p aBytes length.
     *
     */
    void OutputBytesLine(const uint8_t *aBytes, uint16_t aLength);

    /**
     * This method outputs a number of bytes to the CLI console as a hex string.
     *
     * @tparam kBytesLength   The length of @p aBytes array.
     *
     * @param[in]  aBytes     A array of @p kBytesLength bytes which should be printed.
     *
     */
    template <uint8_t kBytesLength> void OutputBytes(const uint8_t (&aBytes)[kBytesLength])
    {
        OutputBytes(aBytes, kBytesLength);
    }

    /**
     * This method outputs a number of bytes to the CLI console as a hex string and at the end it also outputs newline
     * "\r\n".
     *
     * @tparam kBytesLength   The length of @p aBytes array.
     *
     * @param[in]  aBytes     A array of @p kBytesLength bytes which should be printed.
     *
     */
    template <uint8_t kBytesLength> void OutputBytesLine(const uint8_t (&aBytes)[kBytesLength])
    {
        OutputBytesLine(aBytes, kBytesLength);
    }

    /**
     * This method outputs an Extended MAC Address to the CLI console.
     *
     * param[in] aExtAddress  The Extended MAC Address to output.
     *
     */
    void OutputExtAddress(const otExtAddress &aExtAddress) { OutputBytes(aExtAddress.m8); }

    /**
     * This method outputs an Extended MAC Address to the CLI console and at the end it also outputs newline "\r\n".
     *
     * param[in] aExtAddress  The Extended MAC Address to output.
     *
     */
    void OutputExtAddressLine(const otExtAddress &aExtAddress) { OutputBytesLine(aExtAddress.m8); }

    /**
     * This method outputs "Enabled" or "Disabled" status to the CLI console (it also appends newline "\r\n").
     *
     * @param[in] aEnabled  A boolean indicating the status. TRUE outputs "Enabled", FALSE outputs "Disabled".
     *
     */
    void OutputEnabledDisabledStatus(bool aEnabled);

#if OPENTHREAD_FTD || OPENTHREAD_MTD

    /**
     * This method outputs an IPv6 address to the CLI console.
     *
     * @param[in]  aAddress  A reference to the IPv6 address.
     *
     */
    void OutputIp6Address(const otIp6Address &aAddress);

    /**
     * This method outputs an IPv6 address to the CLI console and at the end it also outputs newline "\r\n".
     *
     * @param[in]  aAddress  A reference to the IPv6 address.
     *
     */
    void OutputIp6AddressLine(const otIp6Address &aAddress);

    /**
     * This method outputs an IPv6 prefix to the CLI console.
     *
     * @param[in]  aPrefix  A reference to the IPv6 prefix.
     *
     */
    void OutputIp6Prefix(const otIp6Prefix &aPrefix);

    /**
     * This method outputs an IPv6 prefix to the CLI console and at the end it also outputs newline "\r\n".
     *
     * @param[in]  aPrefix  A reference to the IPv6 prefix.
     *
     */
    void OutputIp6PrefixLine(const otIp6Prefix &aPrefix);

    /**
     * This method outputs an IPv6 network prefix to the CLI console.
     *
     * @param[in]  aPrefix  A reference to the IPv6 network prefix.
     *
     */
    void OutputIp6Prefix(const otIp6NetworkPrefix &aPrefix);

    /**
     * This method outputs an IPv6 network prefix to the CLI console and at the end it also outputs newline "\r\n".
     *
     * @param[in]  aPrefix  A reference to the IPv6 network prefix.
     *
     */
    void OutputIp6PrefixLine(const otIp6NetworkPrefix &aPrefix);

    /**
     * This method outputs DNS TXT data to the CLI console.
     *
     * @param[in] aTxtData        A pointer to a buffer containing the DNS TXT data.
     * @param[in] aTxtDataLength  The length of @p aTxtData (in bytes).
     *
     */
    void OutputDnsTxtData(const uint8_t *aTxtData, uint16_t aTxtDataLength);

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

    /**
     * This method outputs a table header to the CLI console.
     *
     * An example of the table header format:
     *
     *    | Title1    | Title2 |Title3| Title4               |
     *    +-----------+--------+------+----------------------+
     *
     * The titles are left adjusted (extra white space is added at beginning if the column is width enough). The widths
     * are specified as the number chars between two `|` chars (excluding the char `|` itself).
     *
     * @tparam kTableNumColumns   The number columns in the table.
     *
     * @param[in] aTitles   An array specifying the table column titles.
     * @param[in] aWidth    An array specifying the table column widths (in number of chars).
     *
     */
    template <uint8_t kTableNumColumns>
    void OutputTableHeader(const char *const (&aTitles)[kTableNumColumns], const uint8_t (&aWidths)[kTableNumColumns])
    {
        OutputTableHeader(kTableNumColumns, &aTitles[0], &aWidths[0]);
    }

    /**
     * This method outputs a table separator to the CLI console.
     *
     * An example of the table separator:
     *
     *    +-----------+--------+------+----------------------+
     *
     * The widths are specified as number chars between two `+` chars (excluding the char `+` itself).
     *
     * @tparam kTableNumColumns   The number columns in the table.
     *
     * @param[in] aWidth    An array specifying the table column widths (in number of chars).
     *
     */
    template <uint8_t kTableNumColumns> void OutputTableSeparator(const uint8_t (&aWidths)[kTableNumColumns])
    {
        OutputTableSeparator(kTableNumColumns, &aWidths[0]);
    }

protected:
    typedef Utils::CmdLineParser::Arg Arg;

    void OutputFormatV(const char *aFormat, va_list aArguments);

#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
    void LogInput(const Arg *aArgs);
    void SetEmittingCommandOutput(bool aEmittingOutput) { mEmittingCommandOutput = aEmittingOutput; }
#else
    void LogInput(const Arg *) {}
    void SetEmittingCommandOutput(bool) {}
#endif

private:
    static constexpr uint16_t kInputOutputLogStringSize = OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_LOG_STRING_SIZE;

    void OutputTableHeader(uint8_t aNumColumns, const char *const aTitles[], const uint8_t aWidths[]);
    void OutputTableSeparator(uint8_t aNumColumns, const uint8_t aWidths[]);

    otInstance *        mInstance;
    otCliOutputCallback mCallback;
    void *              mCallbackContext;
#if OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
    char     mOutputString[kInputOutputLogStringSize];
    uint16_t mOutputLength;
    bool     mEmittingCommandOutput;
#endif
};

class OutputWrapper
{
protected:
    explicit OutputWrapper(Output &aOutput)
        : mOutput(aOutput)
    {
    }

    otInstance *GetInstancePtr(void) { return mOutput.GetInstancePtr(); }

    template <typename... Args> void OutputFormat(const char *aFormat, Args... aArgs)
    {
        mOutput.OutputFormat(aFormat, aArgs...);
    }

    template <typename... Args> void OutputFormat(uint8_t aIndentSize, const char *aFormat, Args... aArgs)
    {
        mOutput.OutputFormat(aIndentSize, aFormat, aArgs...);
    }

    template <typename... Args> void OutputLine(const char *aFormat, Args... aArgs)
    {
        return mOutput.OutputLine(aFormat, aArgs...);
    }

    template <typename... Args> void OutputLine(uint8_t aIndentSize, const char *aFormat, Args... aArgs)
    {
        return mOutput.OutputLine(aIndentSize, aFormat, aArgs...);
    }

    template <uint8_t kBytesLength> void OutputBytes(const uint8_t (&aBytes)[kBytesLength])
    {
        mOutput.OutputBytes(aBytes, kBytesLength);
    }

    template <uint8_t kBytesLength> void OutputBytesLine(const uint8_t (&aBytes)[kBytesLength])
    {
        mOutput.OutputBytesLine(aBytes, kBytesLength);
    }

    void OutputSpaces(uint8_t aCount) { return mOutput.OutputSpaces(aCount); }
    void OutputBytes(const uint8_t *aBytes, uint16_t aLength) { return mOutput.OutputBytes(aBytes, aLength); }
    void OutputBytesLine(const uint8_t *aBytes, uint16_t aLength) { return mOutput.OutputBytesLine(aBytes, aLength); }
    void OutputExtAddress(const otExtAddress &aExtAddress) { mOutput.OutputExtAddress(aExtAddress); }
    void OutputExtAddressLine(const otExtAddress &aExtAddress) { mOutput.OutputExtAddressLine(aExtAddress); }
    void OutputEnabledDisabledStatus(bool aEnabled) { mOutput.OutputEnabledDisabledStatus(aEnabled); }

#if OPENTHREAD_FTD || OPENTHREAD_MTD
    void OutputIp6Address(const otIp6Address &aAddress) { mOutput.OutputIp6Address(aAddress); }
    void OutputIp6AddressLine(const otIp6Address &aAddress) { mOutput.OutputIp6AddressLine(aAddress); }
    void OutputIp6Prefix(const otIp6Prefix &aPrefix) { mOutput.OutputIp6Prefix(aPrefix); }
    void OutputIp6PrefixLine(const otIp6Prefix &aPrefix) { mOutput.OutputIp6PrefixLine(aPrefix); }
    void OutputIp6Prefix(const otIp6NetworkPrefix &aPrefix) { mOutput.OutputIp6Prefix(aPrefix); }
    void OutputIp6PrefixLine(const otIp6NetworkPrefix &aPrefix) { mOutput.OutputIp6PrefixLine(aPrefix); }
    void OutputDnsTxtData(const uint8_t *aTxtData, uint16_t aTxtDataLength)
    {
        mOutput.OutputDnsTxtData(aTxtData, aTxtDataLength);
    }
#endif

    template <uint8_t kTableNumColumns>
    void OutputTableHeader(const char *const (&aTitles)[kTableNumColumns], const uint8_t (&aWidths)[kTableNumColumns])
    {
        mOutput.OutputTableHeader(aTitles, aWidths);
    }

    template <uint8_t kTableNumColumns> void OutputTableSeparator(const uint8_t (&aWidths)[kTableNumColumns])
    {
        mOutput.OutputTableSeparator(aWidths);
    }

private:
    Output &mOutput;
};

} // namespace Cli
} // namespace ot

#endif // CLI_OUTPUT_HPP_
