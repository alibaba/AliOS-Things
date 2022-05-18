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
 *   This file implements the logging related functions.
 */

#include "logging.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/string.hpp"

/*
 * Verify debug uart dependency.
 *
 * It is reasonable to only enable the debug uart and not enable logs to the DEBUG uart.
 */
#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART) && (!OPENTHREAD_CONFIG_ENABLE_DEBUG_UART)
#error "OPENTHREAD_CONFIG_ENABLE_DEBUG_UART_LOG requires OPENTHREAD_CONFIG_ENABLE_DEBUG_UART"
#endif

#if OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME && !OPENTHREAD_CONFIG_UPTIME_ENABLE
#error "OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME requires OPENTHREAD_CONFIG_UPTIME_ENABLE"
#endif

#if OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME && OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
#error "OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME is not supported under OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY

static void Log(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, va_list aArgs)
{
    ot::String<OPENTHREAD_CONFIG_LOG_MAX_SIZE> logString;

#if OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME
    ot::Uptime::UptimeToString(ot::Instance::Get().Get<ot::Uptime>().GetUptime(), logString);
    logString.Append(" ");
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    VerifyOrExit(otLoggingGetLevel() >= aLogLevel);
#endif

#if OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
    logString.Append("%s", otLogLevelToPrefixString(aLogLevel));
#endif

#if OPENTHREAD_CONFIG_LOG_PREPEND_REGION
    {
        static const char *const kRegionPrefixStrings[] = {
            _OT_REGION_SUFFIX,          _OT_REGION_API_PREFIX,      _OT_REGION_MLE_PREFIX,  _OT_REGION_ARP_PREFIX,
            _OT_REGION_NET_DATA_PREFIX, _OT_REGION_ICMP_PREFIX,     _OT_REGION_IP6_PREFIX,  _OT_REGION_TCP_PREFIX,
            _OT_REGION_MAC_PREFIX,      _OT_REGION_MEM_PREFIX,      _OT_REGION_NCP_PREFIX,  _OT_REGION_MESH_COP_PREFIX,
            _OT_REGION_NET_DIAG_PREFIX, _OT_REGION_PLATFORM_PREFIX, _OT_REGION_COAP_PREFIX, _OT_REGION_CLI_PREFIX,
            _OT_REGION_CORE_PREFIX,     _OT_REGION_UTIL_PREFIX,     _OT_REGION_BBR_PREFIX,  _OT_REGION_MLR_PREFIX,
            _OT_REGION_DUA_PREFIX,      _OT_REGION_BR_PREFIX,       _OT_REGION_SRP_PREFIX,  _OT_REGION_DNS_PREFIX,
        };

        if (aLogRegion < OT_ARRAY_LENGTH(kRegionPrefixStrings))
        {
            logString.Append("%s", kRegionPrefixStrings[aLogRegion]);
        }
        else
        {
            logString.Append("%s", _OT_REGION_SUFFIX);
        }
    }
#else
    logString.Append("%s", _OT_REGION_SUFFIX);
#endif

    logString.AppendVarArgs(aFormat, aArgs);
    otPlatLog(aLogLevel, aLogRegion, "%s" OPENTHREAD_CONFIG_LOG_SUFFIX, logString.AsCString());

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
exit:
    return;
#endif
}

#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_CRIT
void _otLogCrit(otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_CRIT, aRegion, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN
void _otLogWarn(otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_WARN, aRegion, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE
void _otLogNote(otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_NOTE, aRegion, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO
void _otLogInfo(otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_INFO, aRegion, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG
void _otLogDebg(otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_DEBG, aRegion, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_LOG_MAC
void otLogMac(otLogLevel aLogLevel, const char *aFormat, ...)
{
    va_list args;

    VerifyOrExit(otLoggingGetLevel() >= aLogLevel);

    va_start(args, aFormat);
    Log(aLogLevel, OT_LOG_REGION_MAC, aFormat, args);
    va_end(args);

exit:
    return;
}

void otDumpMacFrame(otLogLevel aLogLevel, const char *aId, const void *aBuf, const size_t aLength)
{
    constexpr uint8_t kFixedStringPart = 10; // strlen(" seqno=000")
    constexpr uint8_t kSeqnoIdx        = 2;  // index of the sequence number within aBuf

    size_t idLength = strlen(aId) + kFixedStringPart + 1; // allow for the '\0' character
    char   newId[25];

    VerifyOrExit(idLength <= sizeof(newId));
    snprintf(newId, idLength, "%s seqno=%03u", aId, static_cast<const uint8_t *>(aBuf)[kSeqnoIdx]);
    otDump(aLogLevel, OT_LOG_REGION_MAC, newId, aBuf, aLength);
exit:
    return;
}
#endif

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
void otLogCertMeshCoP(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_NONE, OT_LOG_REGION_MESH_COP, aFormat, args);
    va_end(args);
}
#endif

#if OPENTHREAD_CONFIG_OTNS_ENABLE
void otLogOtns(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);
    Log(OT_LOG_LEVEL_NONE, OT_LOG_REGION_CORE, aFormat, args);
    va_end(args);
}
#endif

#endif // #if !OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY

#if OPENTHREAD_CONFIG_LOG_PKT_DUMP

#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define otLogDump(aLogLevel, aLogRegion, aFormat, ...) \
    _otDynamicLog(aLogLevel, aLogRegion, aFormat OPENTHREAD_CONFIG_LOG_SUFFIX, __VA_ARGS__)
#else
static void otLogDump(otLogLevel aLogLevel, otLogRegion aRegion, const char *aFormat, ...)
{
    va_list args;

    VerifyOrExit(otLoggingGetLevel() >= aLogLevel);

    va_start(args, aFormat);
    Log(aLogLevel, aRegion, aFormat, args);
    va_end(args);

exit:
    return;
}
#endif

static constexpr uint8_t kStringLineLength = 80;
static constexpr uint8_t kDumpBytesPerLine = 16;

static void DumpLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const uint8_t *aBytes, const size_t aLength)
{
    ot::String<kStringLineLength> string;

    string.Append("|");

    for (uint8_t i = 0; i < kDumpBytesPerLine; i++)
    {
        if (i < aLength)
        {
            string.Append(" %02X", aBytes[i]);
        }
        else
        {
            string.Append(" ..");
        }

        if (!((i + 1) % 8))
        {
            string.Append(" |");
        }
    }

    string.Append(" ");

    for (uint8_t i = 0; i < kDumpBytesPerLine; i++)
    {
        char c = '.';

        if (i < aLength)
        {
            char byteAsChar = static_cast<char>(0x7f & aBytes[i]);

            if (isprint(byteAsChar))
            {
                c = byteAsChar;
            }
        }

        string.Append("%c", c);
    }

    otLogDump(aLogLevel, aLogRegion, "%s", string.AsCString());
}

void otDump(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aId, const void *aBuf, const size_t aLength)
{
    constexpr uint8_t kWidth           = 72;
    constexpr uint8_t kFixedStringPart = 10; // strlen("[ len=000]")

    size_t                        idLen = strlen(aId) + kFixedStringPart;
    ot::String<kStringLineLength> string;

    for (size_t i = 0; i < (kWidth - idLen) / 2; i++)
    {
        string.Append("=");
    }

    string.Append("[%s len=%03u]", aId, static_cast<unsigned>(aLength));

    for (size_t i = 0; i < kWidth - idLen - (kWidth - idLen) / 2; i++)
    {
        string.Append("=");
    }

    otLogDump(aLogLevel, aLogRegion, "%s", string.AsCString());

    for (size_t i = 0; i < aLength; i += kDumpBytesPerLine)
    {
        DumpLine(aLogLevel, aLogRegion, static_cast<const uint8_t *>(aBuf) + i,
                 OT_MIN((aLength - i), static_cast<size_t>(kDumpBytesPerLine)));
    }

    string.Clear();

    for (size_t i = 0; i < kWidth; i++)
    {
        string.Append("-");
    }

    otLogDump(aLogLevel, aLogRegion, "%s", string.AsCString());
}
#else  // OPENTHREAD_CONFIG_LOG_PKT_DUMP
void otDump(otLogLevel, otLogRegion, const char *, const void *, const size_t)
{
}
#endif // OPENTHREAD_CONFIG_LOG_PKT_DUMP

#if OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY || OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL

const char *otLogLevelToPrefixString(otLogLevel aLogLevel)
{
    static const char *const kLevelStrings[] = {
        _OT_LEVEL_NONE_PREFIX, _OT_LEVEL_CRIT_PREFIX, _OT_LEVEL_WARN_PREFIX,
        _OT_LEVEL_NOTE_PREFIX, _OT_LEVEL_INFO_PREFIX, _OT_LEVEL_DEBG_PREFIX,
    };

    return ((aLogLevel >= 0) && (aLogLevel < static_cast<int>(OT_ARRAY_LENGTH(kLevelStrings))))
               ? kLevelStrings[aLogLevel]
               : "";
}
#endif

#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_NONE
/* this provides a stub, in case something uses the function */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);
}
#endif

OT_TOOL_WEAK void otPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine)
{
    otPlatLog(aLogLevel, aLogRegion, "%s", aLogLine);
}

#ifdef __cplusplus
}
#endif
