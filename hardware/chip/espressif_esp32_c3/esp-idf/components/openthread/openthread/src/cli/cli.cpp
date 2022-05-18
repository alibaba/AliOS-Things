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
 *   This file implements the CLI interpreter.
 */

#include "cli.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openthread/diag.h>
#include <openthread/dns.h>
#include <openthread/icmp6.h>
#include <openthread/link.h>
#include <openthread/logging.h>
#include <openthread/ncp.h>
#include <openthread/thread.h>
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#include <openthread/network_time.h>
#endif
#if OPENTHREAD_FTD
#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#include <openthread/border_router.h>
#endif
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#include <openthread/server.h>
#endif
#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#include <openthread/child_supervision.h>
#endif
#if OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE
#include <openthread/platform/misc.h>
#endif
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#include <openthread/backbone_router.h>
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#include <openthread/backbone_router_ftd.h>
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
#include <openthread/link_metrics.h>
#endif
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
#include <openthread/channel_manager.h>
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#include <openthread/channel_monitor.h>
#endif
#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART) && OPENTHREAD_POSIX
#include <openthread/platform/debug_uart.h>
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
#include <openthread/platform/trel-udp6.h>
#endif

#include "common/logging.hpp"
#include "common/new.hpp"
#include "common/string.hpp"
#include "mac/channel_mask.hpp"

namespace ot {
namespace Cli {

constexpr Interpreter::Command Interpreter::sCommands[];

Interpreter *Interpreter::sInterpreter = nullptr;
static OT_DEFINE_ALIGNED_VAR(sInterpreterRaw, sizeof(Interpreter), uint64_t);

Interpreter::Interpreter(Instance *aInstance, otCliOutputCallback aCallback, void *aContext)
    : Output(aInstance, aCallback, aContext)
    , mUserCommands(nullptr)
    , mUserCommandsLength(0)
    , mCommandIsPending(false)
    , mTimer(*aInstance, HandleTimer, this)
#if OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    , mSntpQueryingInProgress(false)
#endif
    , mDataset(*this)
    , mNetworkData(*this)
    , mUdp(*this)
#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
    , mTcp(*this)
#endif
#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    , mCoap(*this)
#endif
#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    , mCoapSecure(*this)
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    , mCommissioner(*this)
#endif
#if OPENTHREAD_CONFIG_JOINER_ENABLE
    , mJoiner(*this)
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    , mSrpClient(*this)
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    , mSrpServer(*this)
#endif
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    , mHistory(*this)
#endif
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    , mLocateInProgress(false)
#endif
#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
{
#if OPENTHREAD_FTD
    otThreadSetDiscoveryRequestCallback(GetInstancePtr(), &Interpreter::HandleDiscoveryRequest, this);
#endif

    OutputPrompt();
}

void Interpreter::OutputResult(otError aError)
{
    OT_ASSERT(mCommandIsPending);

    VerifyOrExit(aError != OT_ERROR_PENDING);

    if (aError == OT_ERROR_NONE)
    {
        OutputLine("Done");
    }
    else
    {
        OutputLine("Error %d: %s", aError, otThreadErrorToString(aError));
    }

    mCommandIsPending = false;
    mTimer.Stop();
    OutputPrompt();

exit:
    return;
}

const char *Interpreter::LinkModeToString(const otLinkModeConfig &aLinkMode, char (&aStringBuffer)[kLinkModeStringSize])
{
    char *flagsPtr = &aStringBuffer[0];

    if (aLinkMode.mRxOnWhenIdle)
    {
        *flagsPtr++ = 'r';
    }

    if (aLinkMode.mDeviceType)
    {
        *flagsPtr++ = 'd';
    }

    if (aLinkMode.mNetworkData)
    {
        *flagsPtr++ = 'n';
    }

    if (flagsPtr == &aStringBuffer[0])
    {
        *flagsPtr++ = '-';
    }

    *flagsPtr = '\0';

    return aStringBuffer;
}

#if OPENTHREAD_CONFIG_DIAG_ENABLE
otError Interpreter::ProcessDiag(Arg aArgs[])
{
    otError error;
    char *  args[kMaxArgs];
    char    output[OPENTHREAD_CONFIG_DIAG_OUTPUT_BUFFER_SIZE];

    // all diagnostics related features are processed within diagnostics module
    Arg::CopyArgsToStringArray(aArgs, args);

    error = otDiagProcessCmd(GetInstancePtr(), Arg::GetArgsLength(aArgs), args, output, sizeof(output));

    OutputFormat("%s", output);

    return error;
}
#endif

otError Interpreter::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    for (uint8_t i = 0; i < mUserCommandsLength; i++)
    {
        OutputLine("%s", mUserCommands[i].mName);
    }

    return OT_ERROR_NONE;
}

otError Interpreter::ProcessVersion(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%s", otGetVersionString());
        ExitNow();
    }

    if (aArgs[0] == "api")
    {
        OutputLine("%d", OPENTHREAD_API_VERSION);
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}

otError Interpreter::ProcessReset(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otInstanceReset(GetInstancePtr());

    return OT_ERROR_NONE;
}

void Interpreter::ProcessLine(char *aBuf)
{
    Arg            args[kMaxArgs + 1];
    const Command *command;
    otError        error = OT_ERROR_NONE;

    OT_ASSERT(aBuf != nullptr);

    // Ignore the command if another command is pending.
    VerifyOrExit(!mCommandIsPending, args[0].Clear());
    mCommandIsPending = true;

    VerifyOrExit(StringLength(aBuf, kMaxLineLength) <= kMaxLineLength - 1, error = OT_ERROR_PARSE);

    SuccessOrExit(error = Utils::CmdLineParser::ParseCmd(aBuf, args, kMaxArgs));
    VerifyOrExit(!args[0].IsEmpty(), mCommandIsPending = false);

    LogInput(args);

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    if (otDiagIsEnabled(GetInstancePtr()) && (args[0] != "diag"))
    {
        OutputLine("under diagnostics mode, execute 'diag stop' before running any other commands.");
        ExitNow(error = OT_ERROR_INVALID_STATE);
    }
#endif

    command = Utils::LookupTable::Find(args[0].GetCString(), sCommands);

    if (command != nullptr)
    {
        error = (this->*command->mHandler)(args + 1);
    }
    else
    {
        error = ProcessUserCommands(args);
    }

exit:
    if ((error != OT_ERROR_NONE) || !args[0].IsEmpty())
    {
        OutputResult(error);
    }
    else if (!mCommandIsPending)
    {
        OutputPrompt();
    }
}

otError Interpreter::ProcessUserCommands(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    for (uint8_t i = 0; i < mUserCommandsLength; i++)
    {
        if (aArgs[0] == mUserCommands[i].mName)
        {
            char *args[kMaxArgs];

            Arg::CopyArgsToStringArray(aArgs, args);
            mUserCommands[i].mCommand(mUserCommandsContext, Arg::GetArgsLength(aArgs) - 1, args + 1);
            error = OT_ERROR_NONE;
            break;
        }
    }

    return error;
}

void Interpreter::SetUserCommands(const otCliCommand *aCommands, uint8_t aLength, void *aContext)
{
    mUserCommands        = aCommands;
    mUserCommandsLength  = aLength;
    mUserCommandsContext = aContext;
}

#if OPENTHREAD_FTD || OPENTHREAD_MTD
otError Interpreter::ParseEnableOrDisable(const Arg &aArg, bool &aEnable)
{
    otError error = OT_ERROR_NONE;

    if (aArg == "enable")
    {
        aEnable = true;
    }
    else if (aArg == "disable")
    {
        aEnable = false;
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

    return error;
}

otError Interpreter::ParseJoinerDiscerner(Arg &aArg, otJoinerDiscerner &aDiscerner)
{
    otError error;
    char *  separator;

    VerifyOrExit(!aArg.IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    separator = strstr(aArg.GetCString(), "/");

    VerifyOrExit(separator != nullptr, error = OT_ERROR_NOT_FOUND);

    SuccessOrExit(error = Utils::CmdLineParser::ParseAsUint8(separator + 1, aDiscerner.mLength));
    VerifyOrExit(aDiscerner.mLength > 0 && aDiscerner.mLength <= 64, error = OT_ERROR_INVALID_ARGS);
    *separator = '\0';
    error      = aArg.ParseAsUint64(aDiscerner.mValue);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE

otError Interpreter::ParsePingInterval(const Arg &aArg, uint32_t &aInterval)
{
    otError        error    = OT_ERROR_NONE;
    const char *   string   = aArg.GetCString();
    const uint32_t msFactor = 1000;
    uint32_t       factor   = msFactor;

    aInterval = 0;

    while (*string)
    {
        if ('0' <= *string && *string <= '9')
        {
            // In the case of seconds, change the base of already calculated value.
            if (factor == msFactor)
            {
                aInterval *= 10;
            }

            aInterval += static_cast<uint32_t>(*string - '0') * factor;

            // In the case of milliseconds, change the multiplier factor.
            if (factor != msFactor)
            {
                factor /= 10;
            }
        }
        else if (*string == '.')
        {
            // Accept only one dot character.
            VerifyOrExit(factor == msFactor, error = OT_ERROR_INVALID_ARGS);

            // Start analyzing hundreds of milliseconds.
            factor /= 10;
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }

        string++;
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_PING_SENDER_ENABLE

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
otError Interpreter::ProcessHistory(Arg aArgs[])
{
    return mHistory.Process(aArgs);
}
#endif

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
otError Interpreter::ProcessBorderAgent(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "port")
    {
        OutputLine("%hu", otBorderAgentGetUdpPort(GetInstancePtr()));
    }
    else if (aArgs[0] == "state")
    {
        const char *state;

        switch (otBorderAgentGetState(GetInstancePtr()))
        {
        case OT_BORDER_AGENT_STATE_STOPPED:
            state = "Stopped";
            break;
        case OT_BORDER_AGENT_STATE_STARTED:
            state = "Started";
            break;
        case OT_BORDER_AGENT_STATE_ACTIVE:
            state = "Active";
            break;
        default:
            state = "Unknown";
            break;
        }
        OutputLine(state);
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
otError Interpreter::ProcessBorderRouting(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    bool    enable;

    if (ParseEnableOrDisable(aArgs[0], enable) == OT_ERROR_NONE)
    {
        SuccessOrExit(error = otBorderRoutingSetEnabled(GetInstancePtr(), enable));
    }
    else if (aArgs[0] == "omrprefix")
    {
        otIp6Prefix omrPrefix;

        SuccessOrExit(error = otBorderRoutingGetOmrPrefix(GetInstancePtr(), &omrPrefix));
        OutputIp6PrefixLine(omrPrefix);
    }
    else if (aArgs[0] == "onlinkprefix")
    {
        otIp6Prefix onLinkPrefix;

        SuccessOrExit(error = otBorderRoutingGetOnLinkPrefix(GetInstancePtr(), &onLinkPrefix));
        OutputIp6PrefixLine(onLinkPrefix);
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}
#endif

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
otError Interpreter::ProcessBackboneRouter(Arg aArgs[])
{
    otError                error = OT_ERROR_INVALID_COMMAND;
    otBackboneRouterConfig config;

    if (aArgs[0].IsEmpty())
    {
        if (otBackboneRouterGetPrimary(GetInstancePtr(), &config) == OT_ERROR_NONE)
        {
            OutputLine("BBR Primary:");
            OutputLine("server16: 0x%04X", config.mServer16);
            OutputLine("seqno:    %d", config.mSequenceNumber);
            OutputLine("delay:    %d secs", config.mReregistrationDelay);
            OutputLine("timeout:  %d secs", config.mMlrTimeout);
        }
        else
        {
            OutputLine("BBR Primary: None");
        }

        error = OT_ERROR_NONE;
    }
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    else
    {
        if (aArgs[0] == "mgmt")
        {
            if (aArgs[1].IsEmpty())
            {
                ExitNow(error = OT_ERROR_INVALID_COMMAND);
            }
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE && OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
            else if (aArgs[1] == "dua")
            {
                uint8_t                   status;
                otIp6InterfaceIdentifier *mlIid = nullptr;
                otIp6InterfaceIdentifier  iid;

                SuccessOrExit(error = aArgs[2].ParseAsUint8(status));

                if (!aArgs[3].IsEmpty())
                {
                    SuccessOrExit(error = aArgs[3].ParseAsHexString(iid.mFields.m8));
                    mlIid = &iid;
                    VerifyOrExit(aArgs[4].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
                }

                otBackboneRouterConfigNextDuaRegistrationResponse(GetInstancePtr(), mlIid, status);
                ExitNow();
            }
#endif
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
            else if (aArgs[1] == "mlr")
            {
                error = ProcessBackboneRouterMgmtMlr(aArgs + 2);
                ExitNow();
            }
#endif
        }
        SuccessOrExit(error = ProcessBackboneRouterLocal(aArgs));
    }

exit:
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    return error;
}

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
otError Interpreter::ProcessBackboneRouterMgmtMlr(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    if (aArgs[0] == "listener")
    {
        if (aArgs[1].IsEmpty())
        {
            PrintMulticastListenersTable();
            ExitNow(error = OT_ERROR_NONE);
        }

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        if (aArgs[1] == "clear")
        {
            otBackboneRouterMulticastListenerClear(GetInstancePtr());
            error = OT_ERROR_NONE;
        }
        else if (aArgs[1] == "add")
        {
            otIp6Address address;
            uint32_t     timeout = 0;

            SuccessOrExit(error = aArgs[2].ParseAsIp6Address(address));

            if (!aArgs[3].IsEmpty())
            {
                SuccessOrExit(error = aArgs[3].ParseAsUint32(timeout));
                VerifyOrExit(aArgs[4].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            }

            error = otBackboneRouterMulticastListenerAdd(GetInstancePtr(), &address, timeout);
        }
    }
    else if (aArgs[0] == "response")
    {
        error = ProcessSet(aArgs + 1, otBackboneRouterConfigNextMulticastListenerRegistrationResponse);
#endif
    }

exit:
    return error;
}

void Interpreter::PrintMulticastListenersTable(void)
{
    otBackboneRouterMulticastListenerIterator iter = OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    otBackboneRouterMulticastListenerInfo     listenerInfo;

    while (otBackboneRouterMulticastListenerGetNext(GetInstancePtr(), &iter, &listenerInfo) == OT_ERROR_NONE)
    {
        OutputIp6Address(listenerInfo.mAddress);
        OutputLine(" %u", listenerInfo.mTimeout);
    }
}

#endif // OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

otError Interpreter::ProcessBackboneRouterLocal(Arg aArgs[])
{
    otError                error = OT_ERROR_NONE;
    otBackboneRouterConfig config;
    bool                   enable;

    if (ParseEnableOrDisable(aArgs[0], enable) == OT_ERROR_NONE)
    {
        otBackboneRouterSetEnabled(GetInstancePtr(), enable);
    }
    else if (aArgs[0] == "jitter")
    {
        error = ProcessGetSet(aArgs + 1, otBackboneRouterGetRegistrationJitter, otBackboneRouterSetRegistrationJitter);
    }
    else if (aArgs[0] == "register")
    {
        SuccessOrExit(error = otBackboneRouterRegister(GetInstancePtr()));
    }
    else if (aArgs[0] == "state")
    {
        switch (otBackboneRouterGetState(GetInstancePtr()))
        {
        case OT_BACKBONE_ROUTER_STATE_DISABLED:
            OutputLine("Disabled");
            break;
        case OT_BACKBONE_ROUTER_STATE_SECONDARY:
            OutputLine("Secondary");
            break;
        case OT_BACKBONE_ROUTER_STATE_PRIMARY:
            OutputLine("Primary");
            break;
        }
    }
    else if (aArgs[0] == "config")
    {
        otBackboneRouterGetConfig(GetInstancePtr(), &config);

        if (aArgs[1].IsEmpty())
        {
            OutputLine("seqno:    %d", config.mSequenceNumber);
            OutputLine("delay:    %d secs", config.mReregistrationDelay);
            OutputLine("timeout:  %d secs", config.mMlrTimeout);
        }
        else
        {
            // Set local Backbone Router configuration.
            for (Arg *arg = &aArgs[1]; !arg->IsEmpty(); arg++)
            {
                if (*arg == "seqno")
                {
                    arg++;
                    SuccessOrExit(error = arg->ParseAsUint8(config.mSequenceNumber));
                }
                else if (*arg == "delay")
                {
                    arg++;
                    SuccessOrExit(error = arg->ParseAsUint16(config.mReregistrationDelay));
                }
                else if (*arg == "timeout")
                {
                    arg++;
                    SuccessOrExit(error = arg->ParseAsUint32(config.mMlrTimeout));
                }
                else
                {
                    ExitNow(error = OT_ERROR_INVALID_ARGS);
                }
            }

            SuccessOrExit(error = otBackboneRouterSetConfig(GetInstancePtr(), &config));
        }
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

exit:
    return error;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

otError Interpreter::ProcessDomainName(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%s", otThreadGetDomainName(GetInstancePtr()));
    }
    else
    {
        SuccessOrExit(error = otThreadSetDomainName(GetInstancePtr(), aArgs[0].GetCString()));
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_DUA_ENABLE
otError Interpreter::ProcessDua(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "iid")
    {
        if (aArgs[1].IsEmpty())
        {
            const otIp6InterfaceIdentifier *iid = otThreadGetFixedDuaInterfaceIdentifier(GetInstancePtr());

            if (iid != nullptr)
            {
                OutputBytesLine(iid->mFields.m8);
            }
        }
        else if (aArgs[1] == "clear")
        {
            error = otThreadSetFixedDuaInterfaceIdentifier(GetInstancePtr(), nullptr);
        }
        else
        {
            otIp6InterfaceIdentifier iid;

            SuccessOrExit(error = aArgs[1].ParseAsHexString(iid.mFields.m8));
            error = otThreadSetFixedDuaInterfaceIdentifier(GetInstancePtr(), &iid);
        }
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_DUA_ENABLE

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

otError Interpreter::ProcessBufferInfo(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    struct BufferInfoName
    {
        const uint16_t otBufferInfo::*mNumMessagesPtr;
        const uint16_t otBufferInfo::*mNumBuffersPtr;
        const char *                  mName;
    };

    static const BufferInfoName kBufferInfoNames[] = {
        {&otBufferInfo::m6loSendMessages, &otBufferInfo::m6loSendBuffers, "6lo send"},
        {&otBufferInfo::m6loReassemblyMessages, &otBufferInfo::m6loReassemblyBuffers, "6lo reas"},
        {&otBufferInfo::mIp6Messages, &otBufferInfo::mIp6Buffers, "ip6"},
        {&otBufferInfo::mMplMessages, &otBufferInfo::mMplBuffers, "mpl"},
        {&otBufferInfo::mMleMessages, &otBufferInfo::mMleBuffers, "mle"},
        {&otBufferInfo::mArpMessages, &otBufferInfo::mArpBuffers, "arp"},
        {&otBufferInfo::mCoapMessages, &otBufferInfo::mCoapBuffers, "coap"},
        {&otBufferInfo::mCoapSecureMessages, &otBufferInfo::mCoapSecureBuffers, "coap secure"},
        {&otBufferInfo::mApplicationCoapMessages, &otBufferInfo::mApplicationCoapBuffers, "application coap"},
    };

    otBufferInfo bufferInfo;

    otMessageGetBufferInfo(GetInstancePtr(), &bufferInfo);

    OutputLine("total: %d", bufferInfo.mTotalBuffers);
    OutputLine("free: %d", bufferInfo.mFreeBuffers);

    for (const BufferInfoName &info : kBufferInfoNames)
    {
        OutputLine("%s: %d %d", info.mName, bufferInfo.*info.mNumMessagesPtr, bufferInfo.*info.mNumBuffersPtr);
    }

    return OT_ERROR_NONE;
}

otError Interpreter::ProcessCcaThreshold(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    int8_t  cca;

    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = otPlatRadioGetCcaEnergyDetectThreshold(GetInstancePtr(), &cca));
        OutputLine("%d dBm", cca);
    }
    else
    {
        SuccessOrExit(error = aArgs[0].ParseAsInt8(cca));
        error = otPlatRadioSetCcaEnergyDetectThreshold(GetInstancePtr(), cca);
    }

exit:
    return error;
}

otError Interpreter::ProcessChannel(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "supported")
    {
        OutputLine("0x%x", otPlatRadioGetSupportedChannelMask(GetInstancePtr()));
    }
    else if (aArgs[0] == "preferred")
    {
        OutputLine("0x%x", otPlatRadioGetPreferredChannelMask(GetInstancePtr()));
    }
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    else if (aArgs[0] == "monitor")
    {
        if (aArgs[1].IsEmpty())
        {
            OutputLine("enabled: %d", otChannelMonitorIsEnabled(GetInstancePtr()));
            if (otChannelMonitorIsEnabled(GetInstancePtr()))
            {
                uint32_t channelMask = otLinkGetSupportedChannelMask(GetInstancePtr());
                uint8_t  channelNum  = sizeof(channelMask) * CHAR_BIT;

                OutputLine("interval: %u", otChannelMonitorGetSampleInterval(GetInstancePtr()));
                OutputLine("threshold: %d", otChannelMonitorGetRssiThreshold(GetInstancePtr()));
                OutputLine("window: %u", otChannelMonitorGetSampleWindow(GetInstancePtr()));
                OutputLine("count: %u", otChannelMonitorGetSampleCount(GetInstancePtr()));

                OutputLine("occupancies:");
                for (uint8_t channel = 0; channel < channelNum; channel++)
                {
                    uint32_t occupancy = 0;

                    if (!((1UL << channel) & channelMask))
                    {
                        continue;
                    }

                    occupancy = otChannelMonitorGetChannelOccupancy(GetInstancePtr(), channel);

                    OutputFormat("ch %d (0x%04x) ", channel, occupancy);
                    occupancy = (occupancy * 10000) / 0xffff;
                    OutputLine("%2d.%02d%% busy", occupancy / 100, occupancy % 100);
                }
                OutputLine("");
            }
        }
        else if (aArgs[1] == "start")
        {
            error = otChannelMonitorSetEnabled(GetInstancePtr(), true);
        }
        else if (aArgs[1] == "stop")
        {
            error = otChannelMonitorSetEnabled(GetInstancePtr(), false);
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }
#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
    else if (aArgs[0] == "manager")
    {
        if (aArgs[1].IsEmpty())
        {
            OutputLine("channel: %d", otChannelManagerGetRequestedChannel(GetInstancePtr()));
            OutputLine("auto: %d", otChannelManagerGetAutoChannelSelectionEnabled(GetInstancePtr()));

            if (otChannelManagerGetAutoChannelSelectionEnabled(GetInstancePtr()))
            {
                Mac::ChannelMask supportedMask(otChannelManagerGetSupportedChannels(GetInstancePtr()));
                Mac::ChannelMask favoredMask(otChannelManagerGetFavoredChannels(GetInstancePtr()));

                OutputLine("delay: %d", otChannelManagerGetDelay(GetInstancePtr()));
                OutputLine("interval: %u", otChannelManagerGetAutoChannelSelectionInterval(GetInstancePtr()));
                OutputLine("cca threshold: 0x%04x", otChannelManagerGetCcaFailureRateThreshold(GetInstancePtr()));
                OutputLine("supported: %s", supportedMask.ToString().AsCString());
                OutputLine("favored: %s", supportedMask.ToString().AsCString());
            }
        }
        else if (aArgs[1] == "change")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerRequestChannelChange);
        }
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
        else if (aArgs[1] == "select")
        {
            bool enable;

            SuccessOrExit(error = aArgs[2].ParseAsBool(enable));
            error = otChannelManagerRequestChannelSelect(GetInstancePtr(), enable);
        }
#endif
        else if (aArgs[1] == "auto")
        {
            bool enable;

            SuccessOrExit(error = aArgs[2].ParseAsBool(enable));
            otChannelManagerSetAutoChannelSelectionEnabled(GetInstancePtr(), enable);
        }
        else if (aArgs[1] == "delay")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerSetDelay);
        }
        else if (aArgs[1] == "interval")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerSetAutoChannelSelectionInterval);
        }
        else if (aArgs[1] == "supported")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerSetSupportedChannels);
        }
        else if (aArgs[1] == "favored")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerSetFavoredChannels);
        }
        else if (aArgs[1] == "threshold")
        {
            error = ProcessSet(aArgs + 2, otChannelManagerSetCcaFailureRateThreshold);
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }
#endif // OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
    else
    {
        ExitNow(error = ProcessGetSet(aArgs, otLinkGetChannel, otLinkSetChannel));
    }

exit:
    return error;
}

#if OPENTHREAD_FTD
otError Interpreter::ProcessChild(Arg aArgs[])
{
    otError          error = OT_ERROR_NONE;
    otChildInfo      childInfo;
    uint16_t         childId;
    bool             isTable;
    otLinkModeConfig linkMode;
    char             linkModeString[kLinkModeStringSize];

    isTable = (aArgs[0] == "table");

    if (isTable || (aArgs[0] == "list"))
    {
        uint16_t maxChildren;

        if (isTable)
        {
            static const char *const kChildTableTitles[] = {
                "ID", "RLOC16", "Timeout", "Age", "LQ In",   "C_VN",         "R",
                "D",  "N",      "Ver",     "CSL", "QMsgCnt", "Extended MAC",
            };

            static const uint8_t kChildTableColumnWidths[] = {
                5, 8, 12, 12, 7, 6, 1, 1, 1, 3, 3, 7, 18,
            };

            OutputTableHeader(kChildTableTitles, kChildTableColumnWidths);
        }

        maxChildren = otThreadGetMaxAllowedChildren(GetInstancePtr());

        for (uint16_t i = 0; i < maxChildren; i++)
        {
            if ((otThreadGetChildInfoByIndex(GetInstancePtr(), i, &childInfo) != OT_ERROR_NONE) ||
                childInfo.mIsStateRestoring)
            {
                continue;
            }

            if (isTable)
            {
                OutputFormat("| %3d ", childInfo.mChildId);
                OutputFormat("| 0x%04x ", childInfo.mRloc16);
                OutputFormat("| %10d ", childInfo.mTimeout);
                OutputFormat("| %10d ", childInfo.mAge);
                OutputFormat("| %5d ", childInfo.mLinkQualityIn);
                OutputFormat("| %4d ", childInfo.mNetworkDataVersion);
                OutputFormat("|%1d", childInfo.mRxOnWhenIdle);
                OutputFormat("|%1d", childInfo.mFullThreadDevice);
                OutputFormat("|%1d", childInfo.mFullNetworkData);
                OutputFormat("|%3d", childInfo.mVersion);
                OutputFormat("| %1d ", childInfo.mIsCslSynced);
                OutputFormat("| %5d ", childInfo.mQueuedMessageCnt);
                OutputFormat("| ");
                OutputExtAddress(childInfo.mExtAddress);
                OutputLine(" |");
            }
            else
            {
                OutputFormat("%d ", childInfo.mChildId);
            }
        }

        OutputLine("");
        ExitNow();
    }

    SuccessOrExit(error = aArgs[0].ParseAsUint16(childId));
    SuccessOrExit(error = otThreadGetChildInfoById(GetInstancePtr(), childId, &childInfo));

    OutputLine("Child ID: %d", childInfo.mChildId);
    OutputLine("Rloc: %04x", childInfo.mRloc16);
    OutputFormat("Ext Addr: ");
    OutputExtAddressLine(childInfo.mExtAddress);
    linkMode.mRxOnWhenIdle = childInfo.mRxOnWhenIdle;
    linkMode.mDeviceType   = childInfo.mFullThreadDevice;
    linkMode.mNetworkData  = childInfo.mFullThreadDevice;
    OutputLine("Mode: %s", LinkModeToString(linkMode, linkModeString));
    OutputLine("Net Data: %d", childInfo.mNetworkDataVersion);
    OutputLine("Timeout: %d", childInfo.mTimeout);
    OutputLine("Age: %d", childInfo.mAge);
    OutputLine("Link Quality In: %d", childInfo.mLinkQualityIn);
    OutputLine("RSSI: %d", childInfo.mAverageRssi);

exit:
    return error;
}

otError Interpreter::ProcessChildIp(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        uint16_t maxChildren = otThreadGetMaxAllowedChildren(GetInstancePtr());

        for (uint16_t childIndex = 0; childIndex < maxChildren; childIndex++)
        {
            otChildIp6AddressIterator iterator = OT_CHILD_IP6_ADDRESS_ITERATOR_INIT;
            otIp6Address              ip6Address;
            otChildInfo               childInfo;

            if ((otThreadGetChildInfoByIndex(GetInstancePtr(), childIndex, &childInfo) != OT_ERROR_NONE) ||
                childInfo.mIsStateRestoring)
            {
                continue;
            }

            iterator = OT_CHILD_IP6_ADDRESS_ITERATOR_INIT;

            while (otThreadGetChildNextIp6Address(GetInstancePtr(), childIndex, &iterator, &ip6Address) ==
                   OT_ERROR_NONE)
            {
                OutputFormat("%04x: ", childInfo.mRloc16);
                OutputIp6AddressLine(ip6Address);
            }
        }
    }
    else if (aArgs[0] == "max")
    {
#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        error = ProcessGet(aArgs + 1, otThreadGetMaxChildIpAddresses);
#else
        error = ProcessGetSet(aArgs + 1, otThreadGetMaxChildIpAddresses, otThreadSetMaxChildIpAddresses);
#endif
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

    return error;
}

otError Interpreter::ProcessChildMax(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetMaxAllowedChildren, otThreadSetMaxAllowedChildren);
}
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
otError Interpreter::ProcessChildSupervision(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;

    if (aArgs[0] == "checktimeout")
    {
        error = ProcessGetSet(aArgs + 1, otChildSupervisionGetCheckTimeout, otChildSupervisionSetCheckTimeout);
    }
#if OPENTHREAD_FTD
    else if (aArgs[0] == "interval")
    {
        error = ProcessGetSet(aArgs + 1, otChildSupervisionGetInterval, otChildSupervisionSetInterval);
    }
#endif

    return error;
}
#endif // OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE

otError Interpreter::ProcessChildTimeout(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetChildTimeout, otThreadSetChildTimeout);
}

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
otError Interpreter::ProcessCoap(Arg aArgs[])
{
    return mCoap.Process(aArgs);
}
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
otError Interpreter::ProcessCoapSecure(Arg aArgs[])
{
    return mCoapSecure.Process(aArgs);
}
#endif

#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
otError Interpreter::ProcessCoexMetrics(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    bool    enable;

    if (aArgs[0].IsEmpty())
    {
        OutputEnabledDisabledStatus(otPlatRadioIsCoexEnabled(GetInstancePtr()));
    }
    else if (ParseEnableOrDisable(aArgs[0], enable) == OT_ERROR_NONE)
    {
        error = otPlatRadioSetCoexEnabled(GetInstancePtr(), enable);
    }
    else if (aArgs[0] == "metrics")
    {
        struct RadioCoexMetricName
        {
            const uint32_t otRadioCoexMetrics::*mValuePtr;
            const char *                        mName;
        };

        static const RadioCoexMetricName kTxMetricNames[] = {
            {&otRadioCoexMetrics::mNumTxRequest, "Request"},
            {&otRadioCoexMetrics::mNumTxGrantImmediate, "Grant Immediate"},
            {&otRadioCoexMetrics::mNumTxGrantWait, "Grant Wait"},
            {&otRadioCoexMetrics::mNumTxGrantWaitActivated, "Grant Wait Activated"},
            {&otRadioCoexMetrics::mNumTxGrantWaitTimeout, "Grant Wait Timeout"},
            {&otRadioCoexMetrics::mNumTxGrantDeactivatedDuringRequest, "Grant Deactivated During Request"},
            {&otRadioCoexMetrics::mNumTxDelayedGrant, "Delayed Grant"},
            {&otRadioCoexMetrics::mAvgTxRequestToGrantTime, "Average Request To Grant Time"},
        };

        static const RadioCoexMetricName kRxMetricNames[] = {
            {&otRadioCoexMetrics::mNumRxRequest, "Request"},
            {&otRadioCoexMetrics::mNumRxGrantImmediate, "Grant Immediate"},
            {&otRadioCoexMetrics::mNumRxGrantWait, "Grant Wait"},
            {&otRadioCoexMetrics::mNumRxGrantWaitActivated, "Grant Wait Activated"},
            {&otRadioCoexMetrics::mNumRxGrantWaitTimeout, "Grant Wait Timeout"},
            {&otRadioCoexMetrics::mNumRxGrantDeactivatedDuringRequest, "Grant Deactivated During Request"},
            {&otRadioCoexMetrics::mNumRxDelayedGrant, "Delayed Grant"},
            {&otRadioCoexMetrics::mAvgRxRequestToGrantTime, "Average Request To Grant Time"},
            {&otRadioCoexMetrics::mNumRxGrantNone, "Grant None"},
        };

        otRadioCoexMetrics metrics;

        SuccessOrExit(error = otPlatRadioGetCoexMetrics(GetInstancePtr(), &metrics));

        OutputLine("Stopped: %s", metrics.mStopped ? "true" : "false");
        OutputLine("Grant Glitch: %u", metrics.mNumGrantGlitch);
        OutputLine("Transmit metrics");

        for (const RadioCoexMetricName &metric : kTxMetricNames)
        {
            OutputLine(kIndentSize, "%s: %u", metric.mName, metrics.*metric.mValuePtr);
        }

        OutputLine("Receive metrics");

        for (const RadioCoexMetricName &metric : kRxMetricNames)
        {
            OutputLine(kIndentSize, "%s: %u", metric.mName, metrics.*metric.mValuePtr);
        }
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE

#if OPENTHREAD_FTD
otError Interpreter::ProcessContextIdReuseDelay(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetContextIdReuseDelay, otThreadSetContextIdReuseDelay);
}
#endif

otError Interpreter::ProcessCounters(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("ip");
        OutputLine("mac");
        OutputLine("mle");
    }
    else if (aArgs[0] == "mac")
    {
        if (aArgs[1].IsEmpty())
        {
            struct MacCounterName
            {
                const uint32_t otMacCounters::*mValuePtr;
                const char *                   mName;
            };

            static const MacCounterName kTxCounterNames[] = {
                {&otMacCounters::mTxUnicast, "TxUnicast"},
                {&otMacCounters::mTxBroadcast, "TxBroadcast"},
                {&otMacCounters::mTxAckRequested, "TxAckRequested"},
                {&otMacCounters::mTxAcked, "TxAcked"},
                {&otMacCounters::mTxNoAckRequested, "TxNoAckRequested"},
                {&otMacCounters::mTxData, "TxData"},
                {&otMacCounters::mTxDataPoll, "TxDataPoll"},
                {&otMacCounters::mTxBeacon, "TxBeacon"},
                {&otMacCounters::mTxBeaconRequest, "TxBeaconRequest"},
                {&otMacCounters::mTxOther, "TxOther"},
                {&otMacCounters::mTxRetry, "TxRetry"},
                {&otMacCounters::mTxErrCca, "TxErrCca"},
                {&otMacCounters::mTxErrBusyChannel, "TxErrBusyChannel"},
            };

            static const MacCounterName kRxCounterNames[] = {
                {&otMacCounters::mRxUnicast, "RxUnicast"},
                {&otMacCounters::mRxBroadcast, "RxBroadcast"},
                {&otMacCounters::mRxData, "RxData"},
                {&otMacCounters::mRxDataPoll, "RxDataPoll"},
                {&otMacCounters::mRxBeacon, "RxBeacon"},
                {&otMacCounters::mRxBeaconRequest, "RxBeaconRequest"},
                {&otMacCounters::mRxOther, "RxOther"},
                {&otMacCounters::mRxAddressFiltered, "RxAddressFiltered"},
                {&otMacCounters::mRxDestAddrFiltered, "RxDestAddrFiltered"},
                {&otMacCounters::mRxDuplicated, "RxDuplicated"},
                {&otMacCounters::mRxErrNoFrame, "RxErrNoFrame"},
                {&otMacCounters::mRxErrUnknownNeighbor, "RxErrNoUnknownNeighbor"},
                {&otMacCounters::mRxErrInvalidSrcAddr, "RxErrInvalidSrcAddr"},
                {&otMacCounters::mRxErrSec, "RxErrSec"},
                {&otMacCounters::mRxErrFcs, "RxErrFcs"},
                {&otMacCounters::mRxErrOther, "RxErrOther"},
            };

            const otMacCounters *macCounters = otLinkGetCounters(GetInstancePtr());

            OutputLine("TxTotal: %d", macCounters->mTxTotal);

            for (const MacCounterName &counter : kTxCounterNames)
            {
                OutputLine(kIndentSize, "%s: %u", counter.mName, macCounters->*counter.mValuePtr);
            }

            OutputLine("RxTotal: %d", macCounters->mRxTotal);

            for (const MacCounterName &counter : kRxCounterNames)
            {
                OutputLine(kIndentSize, "%s: %u", counter.mName, macCounters->*counter.mValuePtr);
            }
        }
        else if ((aArgs[1] == "reset") && aArgs[2].IsEmpty())
        {
            otLinkResetCounters(GetInstancePtr());
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }
    else if (aArgs[0] == "mle")
    {
        if (aArgs[1].IsEmpty())
        {
            struct MleCounterName
            {
                const uint16_t otMleCounters::*mValuePtr;
                const char *                   mName;
            };

            static const MleCounterName kCounterNames[] = {
                {&otMleCounters::mDisabledRole, "Role Disabled"},
                {&otMleCounters::mDetachedRole, "Role Detached"},
                {&otMleCounters::mChildRole, "Role Child"},
                {&otMleCounters::mRouterRole, "Role Router"},
                {&otMleCounters::mLeaderRole, "Role Leader"},
                {&otMleCounters::mAttachAttempts, "Attach Attempts"},
                {&otMleCounters::mPartitionIdChanges, "Partition Id Changes"},
                {&otMleCounters::mBetterPartitionAttachAttempts, "Better Partition Attach Attempts"},
                {&otMleCounters::mParentChanges, "Parent Changes"},
            };

            const otMleCounters *mleCounters = otThreadGetMleCounters(GetInstancePtr());

            for (const MleCounterName &counter : kCounterNames)
            {
                OutputLine("%s: %d", counter.mName, mleCounters->*counter.mValuePtr);
            }
        }
        else if ((aArgs[1] == "reset") && aArgs[2].IsEmpty())
        {
            otThreadResetMleCounters(GetInstancePtr());
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }
    else if (aArgs[0] == "ip")
    {
        if (aArgs[1].IsEmpty())
        {
            struct IpCounterName
            {
                const uint32_t otIpCounters::*mValuePtr;
                const char *                  mName;
            };

            static const IpCounterName kCounterNames[] = {
                {&otIpCounters::mTxSuccess, "TxSuccess"},
                {&otIpCounters::mTxFailure, "TxFailed"},
                {&otIpCounters::mRxSuccess, "RxSuccess"},
                {&otIpCounters::mRxFailure, "RxFailed"},
            };

            const otIpCounters *ipCounters = otThreadGetIp6Counters(GetInstancePtr());

            for (const IpCounterName &counter : kCounterNames)
            {
                OutputLine("%s: %d", counter.mName, ipCounters->*counter.mValuePtr);
            }
        }
        else if ((aArgs[1] == "reset") && aArgs[2].IsEmpty())
        {
            otThreadResetIp6Counters(GetInstancePtr());
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
otError Interpreter::ProcessCsl(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    ;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("Channel: %u", otLinkCslGetChannel(GetInstancePtr()));
        OutputLine("Period: %u(in units of 10 symbols), %ums", otLinkCslGetPeriod(GetInstancePtr()),
                   otLinkCslGetPeriod(GetInstancePtr()) * kUsPerTenSymbols / 1000);
        OutputLine("Timeout: %us", otLinkCslGetTimeout(GetInstancePtr()));
    }
    else if (aArgs[0] == "channel")
    {
        error = ProcessSet(aArgs + 1, otLinkCslSetChannel);
    }
    else if (aArgs[0] == "period")
    {
        error = ProcessSet(aArgs + 1, otLinkCslSetPeriod);
    }
    else if (aArgs[0] == "timeout")
    {
        error = ProcessSet(aArgs + 1, otLinkCslSetTimeout);
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_FTD
otError Interpreter::ProcessDelayTimerMin(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%d", (otDatasetGetDelayTimerMinimal(GetInstancePtr()) / 1000));
    }
    else if (aArgs[1].IsEmpty())
    {
        uint32_t delay;
        SuccessOrExit(error = aArgs[0].ParseAsUint32(delay));
        SuccessOrExit(error = otDatasetSetDelayTimerMinimal(GetInstancePtr(), static_cast<uint32_t>(delay * 1000)));
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

exit:
    return error;
}
#endif

otError Interpreter::ProcessDiscover(Arg aArgs[])
{
    otError  error        = OT_ERROR_NONE;
    uint32_t scanChannels = 0;

    if (!aArgs[0].IsEmpty())
    {
        uint8_t channel;

        SuccessOrExit(error = aArgs[0].ParseAsUint8(channel));
        VerifyOrExit(channel < sizeof(scanChannels) * CHAR_BIT, error = OT_ERROR_INVALID_ARGS);
        scanChannels = 1 << channel;
    }

    SuccessOrExit(error = otThreadDiscover(GetInstancePtr(), scanChannels, OT_PANID_BROADCAST, false, false,
                                           &Interpreter::HandleActiveScanResult, this));

    OutputScanTableHeader();

    error = OT_ERROR_PENDING;

exit:
    return error;
}

otError Interpreter::ProcessDns(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otError error = OT_ERROR_NONE;
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    otDnsQueryConfig  queryConfig;
    otDnsQueryConfig *config = &queryConfig;
#endif

    if (aArgs[0].IsEmpty())
    {
        error = OT_ERROR_INVALID_ARGS;
    }
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    else if (aArgs[0] == "compression")
    {
        if (aArgs[1].IsEmpty())
        {
            OutputEnabledDisabledStatus(otDnsIsNameCompressionEnabled());
        }
        else
        {
            bool enable;

            SuccessOrExit(error = ParseEnableOrDisable(aArgs[1], enable));
            otDnsSetNameCompressionEnabled(enable);
        }
    }
#endif // OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    else if (aArgs[0] == "config")
    {
        if (aArgs[1].IsEmpty())
        {
            const otDnsQueryConfig *defaultConfig = otDnsClientGetDefaultConfig(GetInstancePtr());

            OutputFormat("Server: [");
            OutputIp6Address(defaultConfig->mServerSockAddr.mAddress);
            OutputLine("]:%d", defaultConfig->mServerSockAddr.mPort);
            OutputLine("ResponseTimeout: %u ms", defaultConfig->mResponseTimeout);
            OutputLine("MaxTxAttempts: %u", defaultConfig->mMaxTxAttempts);
            OutputLine("RecursionDesired: %s",
                       (defaultConfig->mRecursionFlag == OT_DNS_FLAG_RECURSION_DESIRED) ? "yes" : "no");
        }
        else
        {
            SuccessOrExit(error = GetDnsConfig(aArgs + 1, config));
            otDnsClientSetDefaultConfig(GetInstancePtr(), config);
        }
    }
    else if (aArgs[0] == "resolve")
    {
        VerifyOrExit(!aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = GetDnsConfig(aArgs + 2, config));
        SuccessOrExit(error = otDnsClientResolveAddress(GetInstancePtr(), aArgs[1].GetCString(),
                                                        &Interpreter::HandleDnsAddressResponse, this, config));
        error = OT_ERROR_PENDING;
    }
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    else if (aArgs[0] == "browse")
    {
        VerifyOrExit(!aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = GetDnsConfig(aArgs + 2, config));
        SuccessOrExit(error = otDnsClientBrowse(GetInstancePtr(), aArgs[1].GetCString(),
                                                &Interpreter::HandleDnsBrowseResponse, this, config));
        error = OT_ERROR_PENDING;
    }
    else if (aArgs[0] == "service")
    {
        VerifyOrExit(!aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = GetDnsConfig(aArgs + 3, config));
        SuccessOrExit(error = otDnsClientResolveService(GetInstancePtr(), aArgs[1].GetCString(), aArgs[2].GetCString(),
                                                        &Interpreter::HandleDnsServiceResponse, this, config));
        error = OT_ERROR_PENDING;
    }
#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

otError Interpreter::GetDnsConfig(Arg aArgs[], otDnsQueryConfig *&aConfig)
{
    // This method gets the optional DNS config from `aArgs[]`.
    // The format: `[server IPv6 address] [server port] [timeout]
    // [max tx attempt] [recursion desired]`.

    otError error = OT_ERROR_NONE;
    bool    recursionDesired;

    memset(aConfig, 0, sizeof(otDnsQueryConfig));

    VerifyOrExit(!aArgs[0].IsEmpty(), aConfig = nullptr);

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(aConfig->mServerSockAddr.mAddress));

    VerifyOrExit(!aArgs[1].IsEmpty());
    SuccessOrExit(error = aArgs[1].ParseAsUint16(aConfig->mServerSockAddr.mPort));

    VerifyOrExit(!aArgs[2].IsEmpty());
    SuccessOrExit(error = aArgs[2].ParseAsUint32(aConfig->mResponseTimeout));

    VerifyOrExit(!aArgs[3].IsEmpty());
    SuccessOrExit(error = aArgs[3].ParseAsUint8(aConfig->mMaxTxAttempts));

    VerifyOrExit(!aArgs[4].IsEmpty());
    SuccessOrExit(error = aArgs[4].ParseAsBool(recursionDesired));
    aConfig->mRecursionFlag = recursionDesired ? OT_DNS_FLAG_RECURSION_DESIRED : OT_DNS_FLAG_NO_RECURSION;

exit:
    return error;
}

void Interpreter::HandleDnsAddressResponse(otError aError, const otDnsAddressResponse *aResponse, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleDnsAddressResponse(aError, aResponse);
}

void Interpreter::HandleDnsAddressResponse(otError aError, const otDnsAddressResponse *aResponse)
{
    char         hostName[OT_DNS_MAX_NAME_SIZE];
    otIp6Address address;
    uint32_t     ttl;

    IgnoreError(otDnsAddressResponseGetHostName(aResponse, hostName, sizeof(hostName)));

    OutputFormat("DNS response for %s - ", hostName);

    if (aError == OT_ERROR_NONE)
    {
        uint16_t index = 0;

        while (otDnsAddressResponseGetAddress(aResponse, index, &address, &ttl) == OT_ERROR_NONE)
        {
            OutputIp6Address(address);
            OutputFormat(" TTL:%u ", ttl);
            index++;
        }
    }

    OutputLine("");
    OutputResult(aError);
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

void Interpreter::OutputDnsServiceInfo(uint8_t aIndentSize, const otDnsServiceInfo &aServiceInfo)
{
    OutputLine(aIndentSize, "Port:%d, Priority:%d, Weight:%d, TTL:%u", aServiceInfo.mPort, aServiceInfo.mPriority,
               aServiceInfo.mWeight, aServiceInfo.mTtl);
    OutputLine(aIndentSize, "Host:%s", aServiceInfo.mHostNameBuffer);
    OutputFormat(aIndentSize, "HostAddress:");
    OutputIp6Address(aServiceInfo.mHostAddress);
    OutputLine(" TTL:%u", aServiceInfo.mHostAddressTtl);
    OutputFormat(aIndentSize, "TXT:");
    OutputDnsTxtData(aServiceInfo.mTxtData, aServiceInfo.mTxtDataSize);
    OutputLine(" TTL:%u", aServiceInfo.mTxtDataTtl);
}

void Interpreter::HandleDnsBrowseResponse(otError aError, const otDnsBrowseResponse *aResponse, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleDnsBrowseResponse(aError, aResponse);
}

void Interpreter::HandleDnsBrowseResponse(otError aError, const otDnsBrowseResponse *aResponse)
{
    char             name[OT_DNS_MAX_NAME_SIZE];
    char             label[OT_DNS_MAX_LABEL_SIZE];
    uint8_t          txtBuffer[255];
    otDnsServiceInfo serviceInfo;

    IgnoreError(otDnsBrowseResponseGetServiceName(aResponse, name, sizeof(name)));

    OutputLine("DNS browse response for %s", name);

    if (aError == OT_ERROR_NONE)
    {
        uint16_t index = 0;

        while (otDnsBrowseResponseGetServiceInstance(aResponse, index, label, sizeof(label)) == OT_ERROR_NONE)
        {
            OutputLine("%s", label);
            index++;

            serviceInfo.mHostNameBuffer     = name;
            serviceInfo.mHostNameBufferSize = sizeof(name);
            serviceInfo.mTxtData            = txtBuffer;
            serviceInfo.mTxtDataSize        = sizeof(txtBuffer);

            if (otDnsBrowseResponseGetServiceInfo(aResponse, label, &serviceInfo) == OT_ERROR_NONE)
            {
                OutputDnsServiceInfo(kIndentSize, serviceInfo);
            }

            OutputLine("");
        }
    }

    OutputResult(aError);
}

void Interpreter::HandleDnsServiceResponse(otError aError, const otDnsServiceResponse *aResponse, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleDnsServiceResponse(aError, aResponse);
}

void Interpreter::HandleDnsServiceResponse(otError aError, const otDnsServiceResponse *aResponse)
{
    char             name[OT_DNS_MAX_NAME_SIZE];
    char             label[OT_DNS_MAX_LABEL_SIZE];
    uint8_t          txtBuffer[255];
    otDnsServiceInfo serviceInfo;

    IgnoreError(otDnsServiceResponseGetServiceName(aResponse, label, sizeof(label), name, sizeof(name)));

    OutputLine("DNS service resolution response for %s for service %s", label, name);

    if (aError == OT_ERROR_NONE)
    {
        serviceInfo.mHostNameBuffer     = name;
        serviceInfo.mHostNameBufferSize = sizeof(name);
        serviceInfo.mTxtData            = txtBuffer;
        serviceInfo.mTxtDataSize        = sizeof(txtBuffer);

        if (otDnsServiceResponseGetServiceInfo(aResponse, &serviceInfo) == OT_ERROR_NONE)
        {
            OutputDnsServiceInfo(/* aIndetSize */ 0, serviceInfo);
            OutputLine("");
        }
    }

    OutputResult(aError);
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#if OPENTHREAD_FTD
const char *EidCacheStateToString(otCacheEntryState aState)
{
    static const char *const kStateStrings[4] = {
        "cache",
        "snoop",
        "query",
        "retry",
    };

    return static_cast<uint8_t>(aState) < OT_ARRAY_LENGTH(kStateStrings) ? kStateStrings[aState] : "unknown";
}

void Interpreter::OutputEidCacheEntry(const otCacheEntryInfo &aEntry)
{
    OutputIp6Address(aEntry.mTarget);
    OutputFormat(" %04x", aEntry.mRloc16);
    OutputFormat(" %s", EidCacheStateToString(aEntry.mState));
    OutputFormat(" canEvict=%d", aEntry.mCanEvict);

    if (aEntry.mState == OT_CACHE_ENTRY_STATE_CACHED)
    {
        if (aEntry.mValidLastTrans)
        {
            OutputFormat(" transTime=%u eid=", aEntry.mLastTransTime);
            OutputIp6Address(aEntry.mMeshLocalEid);
        }
    }
    else
    {
        OutputFormat(" timeout=%u", aEntry.mTimeout);
    }

    if (aEntry.mState == OT_CACHE_ENTRY_STATE_RETRY_QUERY)
    {
        OutputFormat(" retryDelay=%u", aEntry.mRetryDelay);
    }

    OutputLine("");
}

otError Interpreter::ProcessEidCache(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otCacheEntryIterator iterator;
    otCacheEntryInfo     entry;

    memset(&iterator, 0, sizeof(iterator));

    for (uint8_t i = 0;; i++)
    {
        SuccessOrExit(otThreadGetNextCacheEntry(GetInstancePtr(), &entry, &iterator));
        OutputEidCacheEntry(entry);
    }

exit:
    return OT_ERROR_NONE;
}
#endif

otError Interpreter::ProcessEui64(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otError      error = OT_ERROR_NONE;
    otExtAddress extAddress;

    VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    otLinkGetFactoryAssignedIeeeEui64(GetInstancePtr(), &extAddress);
    OutputExtAddressLine(extAddress);

exit:
    return error;
}

otError Interpreter::ProcessExtAddress(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputExtAddressLine(*otLinkGetExtendedAddress(GetInstancePtr()));
    }
    else
    {
        otExtAddress extAddress;

        SuccessOrExit(error = aArgs[0].ParseAsHexString(extAddress.m8));
        error = otLinkSetExtendedAddress(GetInstancePtr(), &extAddress);
    }

exit:
    return error;
}

otError Interpreter::ProcessLog(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "level")
    {
        if (aArgs[1].IsEmpty())
        {
            OutputLine("%d", otLoggingGetLevel());
        }
        else
        {
#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
            uint8_t level;

            VerifyOrExit(aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            SuccessOrExit(error = aArgs[1].ParseAsUint8(level));
            error = otLoggingSetLevel(static_cast<otLogLevel>(level));
#else
            error = OT_ERROR_INVALID_ARGS;
#endif
        }
    }
#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART) && OPENTHREAD_POSIX
    else if (aArgs[0] == "filename")
    {
        VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = otPlatDebugUart_logfile(aArgs[1].GetCString()));
    }
#endif
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

exit:
    return error;
}

otError Interpreter::ProcessExtPanId(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputBytesLine(otThreadGetExtendedPanId(GetInstancePtr())->m8);
    }
    else
    {
        otExtendedPanId extPanId;

        SuccessOrExit(error = aArgs[0].ParseAsHexString(extPanId.m8));
        error = otThreadSetExtendedPanId(GetInstancePtr(), &extPanId);
    }

exit:
    return error;
}

otError Interpreter::ProcessFactoryReset(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otInstanceFactoryReset(GetInstancePtr());

    return OT_ERROR_NONE;
}

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
otError Interpreter::ProcessFake(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    if (aArgs[0] == "/a/an")
    {
        otIp6Address             destination, target;
        otIp6InterfaceIdentifier mlIid;

        SuccessOrExit(error = aArgs[1].ParseAsIp6Address(destination));
        SuccessOrExit(error = aArgs[2].ParseAsIp6Address(target));
        SuccessOrExit(error = aArgs[3].ParseAsHexString(mlIid.mFields.m8));
        otThreadSendAddressNotification(GetInstancePtr(), &destination, &target, &mlIid);
    }
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
    else if (aArgs[0] == "/b/ba")
    {
        otIp6Address             target;
        otIp6InterfaceIdentifier mlIid;
        uint32_t                 timeSinceLastTransaction;

        SuccessOrExit(error = aArgs[1].ParseAsIp6Address(target));
        SuccessOrExit(error = aArgs[2].ParseAsHexString(mlIid.mFields.m8));
        SuccessOrExit(error = aArgs[3].ParseAsUint32(timeSinceLastTransaction));

        error = otThreadSendProactiveBackboneNotification(GetInstancePtr(), &target, &mlIid, timeSinceLastTransaction);
    }
#endif

exit:
    return error;
}
#endif

otError Interpreter::ProcessFem(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        int8_t lnaGain;

        SuccessOrExit(error = otPlatRadioGetFemLnaGain(GetInstancePtr(), &lnaGain));
        OutputLine("LNA gain %d dBm", lnaGain);
    }
    else if (aArgs[0] == "lnagain")
    {
        if (aArgs[1].IsEmpty())
        {
            int8_t lnaGain;

            SuccessOrExit(error = otPlatRadioGetFemLnaGain(GetInstancePtr(), &lnaGain));
            OutputLine("%d", lnaGain);
        }
        else
        {
            int8_t lnaGain;

            SuccessOrExit(error = aArgs[1].ParseAsInt8(lnaGain));
            SuccessOrExit(error = otPlatRadioSetFemLnaGain(GetInstancePtr(), lnaGain));
        }
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

exit:
    return error;
}

otError Interpreter::ProcessIfconfig(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        if (otIp6IsEnabled(GetInstancePtr()))
        {
            OutputLine("up");
        }
        else
        {
            OutputLine("down");
        }
    }
    else if (aArgs[0] == "up")
    {
        SuccessOrExit(error = otIp6SetEnabled(GetInstancePtr(), true));
    }
    else if (aArgs[0] == "down")
    {
        SuccessOrExit(error = otIp6SetEnabled(GetInstancePtr(), false));
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

exit:
    return error;
}

otError Interpreter::ProcessIpAddrAdd(Arg aArgs[])
{
    otError        error;
    otNetifAddress aAddress;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(aAddress.mAddress));
    aAddress.mPrefixLength  = 64;
    aAddress.mPreferred     = true;
    aAddress.mValid         = true;
    aAddress.mAddressOrigin = OT_ADDRESS_ORIGIN_MANUAL;

    error = otIp6AddUnicastAddress(GetInstancePtr(), &aAddress);

exit:
    return error;
}

otError Interpreter::ProcessIpAddrDel(Arg aArgs[])
{
    otError      error;
    otIp6Address address;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));
    error = otIp6RemoveUnicastAddress(GetInstancePtr(), &address);

exit:
    return error;
}

otError Interpreter::ProcessIpAddr(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(GetInstancePtr());

        for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext)
        {
            OutputIp6AddressLine(addr->mAddress);
        }
    }
    else
    {
        if (aArgs[0] == "add")
        {
            SuccessOrExit(error = ProcessIpAddrAdd(aArgs + 1));
        }
        else if (aArgs[0] == "del")
        {
            SuccessOrExit(error = ProcessIpAddrDel(aArgs + 1));
        }
        else if (aArgs[0] == "linklocal")
        {
            OutputIp6AddressLine(*otThreadGetLinkLocalIp6Address(GetInstancePtr()));
        }
        else if (aArgs[0] == "rloc")
        {
            OutputIp6AddressLine(*otThreadGetRloc(GetInstancePtr()));
        }
        else if (aArgs[0] == "mleid")
        {
            OutputIp6AddressLine(*otThreadGetMeshLocalEid(GetInstancePtr()));
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_COMMAND);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessIpMulticastAddrAdd(Arg aArgs[])
{
    otError      error;
    otIp6Address address;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));
    error = otIp6SubscribeMulticastAddress(GetInstancePtr(), &address);

exit:
    return error;
}

otError Interpreter::ProcessIpMulticastAddrDel(Arg aArgs[])
{
    otError      error;
    otIp6Address address;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));
    error = otIp6UnsubscribeMulticastAddress(GetInstancePtr(), &address);

exit:
    return error;
}

otError Interpreter::ProcessMulticastPromiscuous(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputEnabledDisabledStatus(otIp6IsMulticastPromiscuousEnabled(GetInstancePtr()));
    }
    else
    {
        bool enable;

        SuccessOrExit(error = ParseEnableOrDisable(aArgs[0], enable));
        otIp6SetMulticastPromiscuousEnabled(GetInstancePtr(), enable);
    }

exit:
    return error;
}

otError Interpreter::ProcessIpMulticastAddr(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        for (const otNetifMulticastAddress *addr = otIp6GetMulticastAddresses(GetInstancePtr()); addr;
             addr                                = addr->mNext)
        {
            OutputIp6AddressLine(addr->mAddress);
        }
    }
    else
    {
        if (aArgs[0] == "add")
        {
            SuccessOrExit(error = ProcessIpMulticastAddrAdd(aArgs + 1));
        }
        else if (aArgs[0] == "del")
        {
            SuccessOrExit(error = ProcessIpMulticastAddrDel(aArgs + 1));
        }
        else if (aArgs[0] == "promiscuous")
        {
            SuccessOrExit(error = ProcessMulticastPromiscuous(aArgs + 1));
        }
        else if (aArgs[0] == "llatn")
        {
            OutputIp6AddressLine(*otThreadGetLinkLocalAllThreadNodesMulticastAddress(GetInstancePtr()));
        }
        else if (aArgs[0] == "rlatn")
        {
            OutputIp6AddressLine(*otThreadGetRealmLocalAllThreadNodesMulticastAddress(GetInstancePtr()));
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_COMMAND);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessKeySequence(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;

    if (aArgs[0] == "counter")
    {
        error = ProcessGetSet(aArgs + 1, otThreadGetKeySequenceCounter, otThreadSetKeySequenceCounter);
    }
    else if (aArgs[0] == "guardtime")
    {
        error = ProcessGetSet(aArgs + 1, otThreadGetKeySwitchGuardTime, otThreadSetKeySwitchGuardTime);
    }

    return error;
}

otError Interpreter::ProcessLeaderData(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otError      error;
    otLeaderData leaderData;

    SuccessOrExit(error = otThreadGetLeaderData(GetInstancePtr(), &leaderData));

    OutputLine("Partition ID: %u", leaderData.mPartitionId);
    OutputLine("Weighting: %d", leaderData.mWeighting);
    OutputLine("Data Version: %d", leaderData.mDataVersion);
    OutputLine("Stable Data Version: %d", leaderData.mStableDataVersion);
    OutputLine("Leader Router ID: %d", leaderData.mLeaderRouterId);

exit:
    return error;
}

#if OPENTHREAD_FTD
otError Interpreter::ProcessPartitionId(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%u", otThreadGetPartitionId(GetInstancePtr()));
        error = OT_ERROR_NONE;
    }
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    else if (aArgs[0] == "preferred")
    {
        error = ProcessGetSet(aArgs + 1, otThreadGetPreferredLeaderPartitionId, otThreadSetPreferredLeaderPartitionId);
    }
#endif

    return error;
}

otError Interpreter::ProcessLeaderWeight(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetLocalLeaderWeight, otThreadSetLocalLeaderWeight);
}
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
void Interpreter::HandleLinkMetricsReport(const otIp6Address *       aAddress,
                                          const otLinkMetricsValues *aMetricsValues,
                                          uint8_t                    aStatus,
                                          void *                     aContext)
{
    static_cast<Interpreter *>(aContext)->HandleLinkMetricsReport(aAddress, aMetricsValues, aStatus);
}

void Interpreter::PrintLinkMetricsValue(const otLinkMetricsValues *aMetricsValues)
{
    const char kLinkMetricsTypeCount[]   = "(Count/Summation)";
    const char kLinkMetricsTypeAverage[] = "(Exponential Moving Average)";

    if (aMetricsValues->mMetrics.mPduCount)
    {
        OutputLine(" - PDU Counter: %d %s", aMetricsValues->mPduCountValue, kLinkMetricsTypeCount);
    }

    if (aMetricsValues->mMetrics.mLqi)
    {
        OutputLine(" - LQI: %d %s", aMetricsValues->mLqiValue, kLinkMetricsTypeAverage);
    }

    if (aMetricsValues->mMetrics.mLinkMargin)
    {
        OutputLine(" - Margin: %d (dB) %s", aMetricsValues->mLinkMarginValue, kLinkMetricsTypeAverage);
    }

    if (aMetricsValues->mMetrics.mRssi)
    {
        OutputLine(" - RSSI: %d (dBm) %s", aMetricsValues->mRssiValue, kLinkMetricsTypeAverage);
    }
}

void Interpreter::HandleLinkMetricsReport(const otIp6Address *       aAddress,
                                          const otLinkMetricsValues *aMetricsValues,
                                          uint8_t                    aStatus)
{
    OutputFormat("Received Link Metrics Report from: ");
    OutputIp6AddressLine(*aAddress);

    if (aMetricsValues != nullptr)
    {
        PrintLinkMetricsValue(aMetricsValues);
    }
    else
    {
        OutputLine("Link Metrics Report, status: %s", LinkMetricsStatusToStr(aStatus));
    }
}

void Interpreter::HandleLinkMetricsMgmtResponse(const otIp6Address *aAddress, uint8_t aStatus, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleLinkMetricsMgmtResponse(aAddress, aStatus);
}

void Interpreter::HandleLinkMetricsMgmtResponse(const otIp6Address *aAddress, uint8_t aStatus)
{
    OutputFormat("Received Link Metrics Management Response from: ");
    OutputIp6AddressLine(*aAddress);

    OutputLine("Status: %s", LinkMetricsStatusToStr(aStatus));
}

void Interpreter::HandleLinkMetricsEnhAckProbingIe(otShortAddress             aShortAddress,
                                                   const otExtAddress *       aExtAddress,
                                                   const otLinkMetricsValues *aMetricsValues,
                                                   void *                     aContext)
{
    static_cast<Interpreter *>(aContext)->HandleLinkMetricsEnhAckProbingIe(aShortAddress, aExtAddress, aMetricsValues);
}

void Interpreter::HandleLinkMetricsEnhAckProbingIe(otShortAddress             aShortAddress,
                                                   const otExtAddress *       aExtAddress,
                                                   const otLinkMetricsValues *aMetricsValues)
{
    OutputFormat("Received Link Metrics data in Enh Ack from neighbor, short address:0x%02x , extended address:",
                 aShortAddress);
    OutputExtAddressLine(*aExtAddress);

    if (aMetricsValues != nullptr)
    {
        PrintLinkMetricsValue(aMetricsValues);
    }
}

const char *Interpreter::LinkMetricsStatusToStr(uint8_t aStatus)
{
    uint8_t            strIndex                = 0;
    static const char *linkMetricsStatusText[] = {
        "Success",
        "Cannot support new series",
        "Series ID already registered",
        "Series ID not recognized",
        "No matching series ID",
        "Other error",
        "Unknown error",
    };

    switch (aStatus)
    {
    case OT_LINK_METRICS_STATUS_SUCCESS:
        strIndex = 0;
        break;
    case OT_LINK_METRICS_STATUS_CANNOT_SUPPORT_NEW_SERIES:
        strIndex = 1;
        break;
    case OT_LINK_METRICS_STATUS_SERIESID_ALREADY_REGISTERED:
        strIndex = 2;
        break;
    case OT_LINK_METRICS_STATUS_SERIESID_NOT_RECOGNIZED:
        strIndex = 3;
        break;
    case OT_LINK_METRICS_STATUS_NO_MATCHING_FRAMES_RECEIVED:
        strIndex = 4;
        break;
    case OT_LINK_METRICS_STATUS_OTHER_ERROR:
        strIndex = 5;
        break;
    default:
        strIndex = 6;
        break;
    }

    return linkMetricsStatusText[strIndex];
}

otError Interpreter::ProcessLinkMetrics(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    if (aArgs[0] == "query")
    {
        error = ProcessLinkMetricsQuery(aArgs + 1);
    }
    else if (aArgs[0] == "mgmt")
    {
        error = ProcessLinkMetricsMgmt(aArgs + 1);
    }
    else if (aArgs[0] == "probe")
    {
        error = ProcessLinkMetricsProbe(aArgs + 1);
    }

    return error;
}

otError Interpreter::ProcessLinkMetricsQuery(Arg aArgs[])
{
    otError       error;
    otIp6Address  address;
    otLinkMetrics linkMetrics;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));

    if (aArgs[1] == "single")
    {
        VerifyOrExit(!aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = ParseLinkMetricsFlags(linkMetrics, aArgs[2]));
        error = otLinkMetricsQuery(GetInstancePtr(), &address, /* aSeriesId */ 0, &linkMetrics,
                                   &Interpreter::HandleLinkMetricsReport, this);
    }
    else if (aArgs[1] == "forward")
    {
        uint8_t seriesId;

        SuccessOrExit(error = aArgs[2].ParseAsUint8(seriesId));
        error = otLinkMetricsQuery(GetInstancePtr(), &address, seriesId, nullptr, &Interpreter::HandleLinkMetricsReport,
                                   this);
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

exit:
    return error;
}

otError Interpreter::ParseLinkMetricsFlags(otLinkMetrics &aLinkMetrics, const Arg &aFlags)
{
    otError error = OT_ERROR_NONE;

    memset(&aLinkMetrics, 0, sizeof(aLinkMetrics));

    for (const char *arg = aFlags.GetCString(); *arg != '\0'; arg++)
    {
        switch (*arg)
        {
        case 'p':
            aLinkMetrics.mPduCount = true;
            break;

        case 'q':
            aLinkMetrics.mLqi = true;
            break;

        case 'm':
            aLinkMetrics.mLinkMargin = true;
            break;

        case 'r':
            aLinkMetrics.mRssi = true;
            break;

        default:
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessLinkMetricsMgmt(Arg aArgs[])
{
    otError                  error;
    otIp6Address             address;
    otLinkMetricsSeriesFlags seriesFlags;
    bool                     clear = false;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));

    memset(&seriesFlags, 0, sizeof(otLinkMetricsSeriesFlags));

    if (aArgs[1] == "forward")
    {
        uint8_t       seriesId;
        otLinkMetrics linkMetrics;

        memset(&linkMetrics, 0, sizeof(otLinkMetrics));
        SuccessOrExit(error = aArgs[2].ParseAsUint8(seriesId));
        VerifyOrExit(!aArgs[3].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

        for (const char *arg = aArgs[3].GetCString(); *arg != '\0'; arg++)
        {
            switch (*arg)
            {
            case 'l':
                seriesFlags.mLinkProbe = true;
                break;

            case 'd':
                seriesFlags.mMacData = true;
                break;

            case 'r':
                seriesFlags.mMacDataRequest = true;
                break;

            case 'a':
                seriesFlags.mMacAck = true;
                break;

            case 'X':
                VerifyOrExit(arg == aArgs[3].GetCString() && *(arg + 1) == '\0' && aArgs[4].IsEmpty(),
                             error = OT_ERROR_INVALID_ARGS); // Ensure the flags only contain 'X'
                clear = true;
                break;

            default:
                ExitNow(error = OT_ERROR_INVALID_ARGS);
            }
        }

        if (!clear)
        {
            VerifyOrExit(!aArgs[4].IsEmpty() && aArgs[5].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            SuccessOrExit(error = ParseLinkMetricsFlags(linkMetrics, aArgs[4]));
        }

        error = otLinkMetricsConfigForwardTrackingSeries(GetInstancePtr(), &address, seriesId, seriesFlags,
                                                         clear ? nullptr : &linkMetrics,
                                                         &Interpreter::HandleLinkMetricsMgmtResponse, this);
    }
    else if (aArgs[1] == "enhanced-ack")
    {
        otLinkMetricsEnhAckFlags enhAckFlags;
        otLinkMetrics            linkMetrics;
        otLinkMetrics *          pLinkMetrics = &linkMetrics;

        if (aArgs[2] == "clear")
        {
            enhAckFlags  = OT_LINK_METRICS_ENH_ACK_CLEAR;
            pLinkMetrics = nullptr;
        }
        else if (aArgs[2] == "register")
        {
            enhAckFlags = OT_LINK_METRICS_ENH_ACK_REGISTER;
            VerifyOrExit(!aArgs[3].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            SuccessOrExit(error = ParseLinkMetricsFlags(linkMetrics, aArgs[3]));
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
            if (aArgs[4] == "r")
            {
                linkMetrics.mReserved = true;
            }
#endif
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }

        error = otLinkMetricsConfigEnhAckProbing(GetInstancePtr(), &address, enhAckFlags, pLinkMetrics,
                                                 &Interpreter::HandleLinkMetricsMgmtResponse, this,
                                                 &Interpreter::HandleLinkMetricsEnhAckProbingIe, this);
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

exit:
    return error;
}

otError Interpreter::ProcessLinkMetricsProbe(Arg aArgs[])
{
    otError      error = OT_ERROR_INVALID_ARGS;
    otIp6Address address;
    uint8_t      seriesId = 0;
    uint8_t      length   = 0;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(address));
    SuccessOrExit(error = aArgs[1].ParseAsUint8(seriesId));
    SuccessOrExit(error = aArgs[2].ParseAsUint8(length));

    error = otLinkMetricsSendLinkProbe(GetInstancePtr(), &address, seriesId, length);

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE

#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE

otError Interpreter::ProcessLocate(Arg aArgs[])
{
    otError      error = OT_ERROR_INVALID_ARGS;
    otIp6Address anycastAddress;

    if (aArgs[0].IsEmpty())
    {
        OutputLine(otThreadIsAnycastLocateInProgress(GetInstancePtr()) ? "In Progress" : "Idle");
        ExitNow(error = OT_ERROR_NONE);
    }

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(anycastAddress));
    SuccessOrExit(error =
                      otThreadLocateAnycastDestination(GetInstancePtr(), &anycastAddress, HandleLocateResult, this));
    SetCommandTimeout(kLocateTimeoutMsecs);
    mLocateInProgress = true;
    error             = OT_ERROR_PENDING;

exit:
    return error;
}

void Interpreter::HandleLocateResult(void *              aContext,
                                     otError             aError,
                                     const otIp6Address *aMeshLocalAddress,
                                     uint16_t            aRloc16)
{
    static_cast<Interpreter *>(aContext)->HandleLocateResult(aError, aMeshLocalAddress, aRloc16);
}

void Interpreter::HandleLocateResult(otError aError, const otIp6Address *aMeshLocalAddress, uint16_t aRloc16)
{
    VerifyOrExit(mLocateInProgress);

    mLocateInProgress = false;

    if (aError == OT_ERROR_NONE)
    {
        OutputIp6Address(*aMeshLocalAddress);
        OutputLine(" 0x%04x", aRloc16);
    }

    OutputResult(aError);

exit:
    return;
}

#endif //  OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE

#if OPENTHREAD_FTD
otError Interpreter::ProcessPskc(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        otPskc pskc;

        otThreadGetPskc(GetInstancePtr(), &pskc);
        OutputBytesLine(pskc.m8);
    }
    else
    {
        otPskc pskc;

        if (aArgs[1].IsEmpty())
        {
            SuccessOrExit(error = aArgs[0].ParseAsHexString(pskc.m8));
        }
        else if (aArgs[0] == "-p")
        {
            SuccessOrExit(error = otDatasetGeneratePskc(
                              aArgs[1].GetCString(),
                              reinterpret_cast<const otNetworkName *>(otThreadGetNetworkName(GetInstancePtr())),
                              otThreadGetExtendedPanId(GetInstancePtr()), &pskc));
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }

        SuccessOrExit(error = otThreadSetPskc(GetInstancePtr(), &pskc));
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
otError Interpreter::ProcessPskcRef(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("0x%04x", otThreadGetPskcRef(GetInstancePtr()));
    }
    else
    {
        otPskcRef pskcRef;

        if (aArgs[1].IsEmpty())
        {
            SuccessOrExit(error = aArgs[0].ParseAsUint32(pskcRef));
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }

        SuccessOrExit(error = otThreadSetPskcRef(GetInstancePtr(), pskcRef));
    }

exit:
    return error;
}
#endif
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
otError Interpreter::ProcessMlIid(Arg aArgs[])
{
    otError                  error = OT_ERROR_NONE;
    otIp6InterfaceIdentifier iid;

    VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = aArgs[0].ParseAsHexString(iid.mFields.m8));
    SuccessOrExit(error = otIp6SetMeshLocalIid(GetInstancePtr(), &iid));

exit:
    return error;
}
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

otError Interpreter::ProcessMlr(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_COMMAND;

    if (aArgs[0] == "reg")
    {
        error = ProcessMlrReg(aArgs + 1);
    }

    return error;
}

otError Interpreter::ProcessMlrReg(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    otIp6Address addresses[kIp6AddressesNumMax];
    uint32_t     timeout;
    bool         hasTimeout   = false;
    uint8_t      numAddresses = 0;

    while (aArgs->ParseAsIp6Address(addresses[numAddresses]) == OT_ERROR_NONE)
    {
        aArgs++;
        numAddresses++;

        if (numAddresses == OT_ARRAY_LENGTH(addresses))
        {
            break;
        }
    }

    if (aArgs->ParseAsUint32(timeout) == OT_ERROR_NONE)
    {
        aArgs++;
        hasTimeout = true;
    }

    VerifyOrExit(aArgs->IsEmpty() && (numAddresses > 0), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = otIp6RegisterMulticastListeners(GetInstancePtr(), addresses, numAddresses,
                                                          hasTimeout ? &timeout : nullptr,
                                                          Interpreter::HandleMlrRegResult, this));

    error = OT_ERROR_PENDING;

exit:
    return error;
}

void Interpreter::HandleMlrRegResult(void *              aContext,
                                     otError             aError,
                                     uint8_t             aMlrStatus,
                                     const otIp6Address *aFailedAddresses,
                                     uint8_t             aFailedAddressNum)
{
    static_cast<Interpreter *>(aContext)->HandleMlrRegResult(aError, aMlrStatus, aFailedAddresses, aFailedAddressNum);
}

void Interpreter::HandleMlrRegResult(otError             aError,
                                     uint8_t             aMlrStatus,
                                     const otIp6Address *aFailedAddresses,
                                     uint8_t             aFailedAddressNum)
{
    if (aError == OT_ERROR_NONE)
    {
        OutputLine("status %d, %d failed", aMlrStatus, aFailedAddressNum);

        for (uint8_t i = 0; i < aFailedAddressNum; i++)
        {
            OutputIp6AddressLine(aFailedAddresses[i]);
        }
    }

    OutputResult(aError);
}

#endif // (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

otError Interpreter::ProcessMode(Arg aArgs[])
{
    otError          error = OT_ERROR_NONE;
    otLinkModeConfig linkMode;

    memset(&linkMode, 0, sizeof(otLinkModeConfig));

    if (aArgs[0].IsEmpty())
    {
        char linkModeString[kLinkModeStringSize];

        OutputLine("%s", LinkModeToString(otThreadGetLinkMode(GetInstancePtr()), linkModeString));
        ExitNow();
    }

    if (aArgs[0] != "-")
    {
        for (const char *arg = aArgs[0].GetCString(); *arg != '\0'; arg++)
        {
            switch (*arg)
            {
            case 'r':
                linkMode.mRxOnWhenIdle = true;
                break;

            case 'd':
                linkMode.mDeviceType = true;
                break;

            case 'n':
                linkMode.mNetworkData = true;
                break;

            default:
                ExitNow(error = OT_ERROR_INVALID_ARGS);
            }
        }
    }

    error = otThreadSetLinkMode(GetInstancePtr(), linkMode);

exit:
    return error;
}

otError Interpreter::ProcessMultiRadio(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    OT_UNUSED_VARIABLE(aArgs);

    if (aArgs[0].IsEmpty())
    {
        bool isFirst = true;

        OutputFormat("[");
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        OutputFormat("15.4");
        isFirst = false;
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        OutputFormat("%sTREL", isFirst ? "" : ", ");
#endif
        OutputLine("]");

        OT_UNUSED_VARIABLE(isFirst);
    }
#if OPENTHREAD_CONFIG_MULTI_RADIO
    else if (aArgs[0] == "neighbor")
    {
        otMultiRadioNeighborInfo multiRadioInfo;

        if (aArgs[1] == "list")
        {
            otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;
            otNeighborInfo         neighInfo;

            while (otThreadGetNextNeighborInfo(GetInstancePtr(), &iterator, &neighInfo) == OT_ERROR_NONE)
            {
                if (otMultiRadioGetNeighborInfo(GetInstancePtr(), &neighInfo.mExtAddress, &multiRadioInfo) !=
                    OT_ERROR_NONE)
                {
                    continue;
                }

                OutputFormat("ExtAddr:");
                OutputExtAddress(neighInfo.mExtAddress);
                OutputFormat(", RLOC16:0x%04x, Radios:", neighInfo.mRloc16);
                OutputMultiRadioInfo(multiRadioInfo);
            }
        }
        else
        {
            otExtAddress extAddress;

            SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddress.m8));
            SuccessOrExit(error = otMultiRadioGetNeighborInfo(GetInstancePtr(), &extAddress, &multiRadioInfo));
            OutputMultiRadioInfo(multiRadioInfo);
        }
    }
#endif // OPENTHREAD_CONFIG_MULTI_RADIO
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MULTI_RADIO
void Interpreter::OutputMultiRadioInfo(const otMultiRadioNeighborInfo &aMultiRadioInfo)
{
    bool isFirst = true;

    OutputFormat("[");

    if (aMultiRadioInfo.mSupportsIeee802154)
    {
        OutputFormat("15.4(%d)", aMultiRadioInfo.mIeee802154Info.mPreference);
        isFirst = false;
    }

    if (aMultiRadioInfo.mSupportsTrelUdp6)
    {
        OutputFormat("%sTREL(%d)", isFirst ? "" : ", ", aMultiRadioInfo.mTrelUdp6Info.mPreference);
    }

    OutputLine("]");
}
#endif // OPENTHREAD_CONFIG_MULTI_RADIO

#if OPENTHREAD_FTD
otError Interpreter::ProcessNeighbor(Arg aArgs[])
{
    otError                error = OT_ERROR_NONE;
    otNeighborInfo         neighborInfo;
    bool                   isTable;
    otNeighborInfoIterator iterator = OT_NEIGHBOR_INFO_ITERATOR_INIT;

    isTable = (aArgs[0] == "table");

    if (isTable || (aArgs[0] == "list"))
    {
        if (isTable)
        {
            static const char *const kNeighborTableTitles[] = {
                "Role", "RLOC16", "Age", "Avg RSSI", "Last RSSI", "R", "D", "N", "Extended MAC",
            };

            static const uint8_t kNeighborTableColumnWidths[] = {
                6, 8, 5, 10, 11, 1, 1, 1, 18,
            };

            OutputTableHeader(kNeighborTableTitles, kNeighborTableColumnWidths);
        }

        while (otThreadGetNextNeighborInfo(GetInstancePtr(), &iterator, &neighborInfo) == OT_ERROR_NONE)
        {
            if (isTable)
            {
                OutputFormat("| %3c  ", neighborInfo.mIsChild ? 'C' : 'R');
                OutputFormat("| 0x%04x ", neighborInfo.mRloc16);
                OutputFormat("| %3d ", neighborInfo.mAge);
                OutputFormat("| %8d ", neighborInfo.mAverageRssi);
                OutputFormat("| %9d ", neighborInfo.mLastRssi);
                OutputFormat("|%1d", neighborInfo.mRxOnWhenIdle);
                OutputFormat("|%1d", neighborInfo.mFullThreadDevice);
                OutputFormat("|%1d", neighborInfo.mFullNetworkData);
                OutputFormat("| ");
                OutputExtAddress(neighborInfo.mExtAddress);
                OutputLine(" |");
            }
            else
            {
                OutputFormat("0x%04x ", neighborInfo.mRloc16);
            }
        }

        OutputLine("");
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}
#endif // OPENTHREAD_FTD

otError Interpreter::ProcessNetstat(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    static const char *const kNetstatTableTitles[]       = {"Local Address", "Peer Address"};
    static const uint8_t     kNetstatTableColumnWidths[] = {49, 49};

    char string[OT_IP6_SOCK_ADDR_STRING_SIZE];

    OutputTableHeader(kNetstatTableTitles, kNetstatTableColumnWidths);

    for (const otUdpSocket *socket = otUdpGetSockets(GetInstancePtr()); socket != nullptr; socket = socket->mNext)
    {
        otIp6SockAddrToString(&socket->mSockName, string, sizeof(string));
        OutputFormat("| %-47s ", string);
        otIp6SockAddrToString(&socket->mPeerName, string, sizeof(string));
        OutputLine("| %-47s |", string);
    }

    return OT_ERROR_NONE;
}

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
otError Interpreter::ProcessServiceList(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otServiceConfig       config;

    while (otServerGetNextService(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        mNetworkData.OutputService(config);
    }

    return OT_ERROR_NONE;
}

otError Interpreter::ProcessService(Arg aArgs[])
{
    otError         error = OT_ERROR_INVALID_COMMAND;
    otServiceConfig cfg;

    if (aArgs[0].IsEmpty())
    {
        error = ProcessServiceList();
    }
    else
    {
        uint16_t length;

        SuccessOrExit(error = aArgs[1].ParseAsUint32(cfg.mEnterpriseNumber));

        length = sizeof(cfg.mServiceData);
        SuccessOrExit(error = aArgs[2].ParseAsHexString(length, cfg.mServiceData));
        VerifyOrExit(length > 0, error = OT_ERROR_INVALID_ARGS);
        cfg.mServiceDataLength = static_cast<uint8_t>(length);

        if (aArgs[0] == "add")
        {
            length = sizeof(cfg.mServerConfig.mServerData);
            SuccessOrExit(error = aArgs[3].ParseAsHexString(length, cfg.mServerConfig.mServerData));
            VerifyOrExit(length > 0, error = OT_ERROR_INVALID_ARGS);
            cfg.mServerConfig.mServerDataLength = static_cast<uint8_t>(length);

            cfg.mServerConfig.mStable = true;

            error = otServerAddService(GetInstancePtr(), &cfg);
        }
        else if (aArgs[0] == "remove")
        {
            error = otServerRemoveService(GetInstancePtr(), cfg.mEnterpriseNumber, cfg.mServiceData,
                                          cfg.mServiceDataLength);
        }
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

otError Interpreter::ProcessNetworkData(Arg aArgs[])
{
    return mNetworkData.Process(aArgs);
}

#if OPENTHREAD_FTD
otError Interpreter::ProcessNetworkIdTimeout(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetNetworkIdTimeout, otThreadSetNetworkIdTimeout);
}
#endif

otError Interpreter::ProcessNetworkKey(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        otNetworkKey networkKey;

        otThreadGetNetworkKey(GetInstancePtr(), &networkKey);
        OutputBytesLine(networkKey.m8);
    }
    else
    {
        otNetworkKey key;

        SuccessOrExit(error = aArgs[0].ParseAsHexString(key.m8));
        SuccessOrExit(error = otThreadSetNetworkKey(GetInstancePtr(), &key));
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
otError Interpreter::ProcessNetworkKeyRef(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("0x%04x", otThreadGetNetworkKeyRef(GetInstancePtr()));
    }
    else
    {
        otNetworkKeyRef keyRef;

        SuccessOrExit(error = aArgs[0].ParseAsUint32(keyRef));
        SuccessOrExit(error = otThreadSetNetworkKeyRef(GetInstancePtr(), keyRef));
    }

exit:
    return error;
}
#endif

otError Interpreter::ProcessNetworkName(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%s", otThreadGetNetworkName(GetInstancePtr()));
    }
    else
    {
        SuccessOrExit(error = otThreadSetNetworkName(GetInstancePtr(), aArgs[0].GetCString()));
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
otError Interpreter::ProcessNetworkTime(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        uint64_t            time;
        otNetworkTimeStatus networkTimeStatus;

        networkTimeStatus = otNetworkTimeGet(GetInstancePtr(), &time);

        OutputFormat("Network Time:     %luus", time);

        switch (networkTimeStatus)
        {
        case OT_NETWORK_TIME_UNSYNCHRONIZED:
            OutputLine(" (unsynchronized)");
            break;

        case OT_NETWORK_TIME_RESYNC_NEEDED:
            OutputLine(" (resync needed)");
            break;

        case OT_NETWORK_TIME_SYNCHRONIZED:
            OutputLine(" (synchronized)");
            break;

        default:
            break;
        }

        OutputLine("Time Sync Period: %ds", otNetworkTimeGetSyncPeriod(GetInstancePtr()));
        OutputLine("XTAL Threshold:   %dppm", otNetworkTimeGetXtalThreshold(GetInstancePtr()));
    }
    else
    {
        uint16_t period;
        uint16_t xtalThreshold;

        SuccessOrExit(error = aArgs[0].ParseAsUint16(period));
        SuccessOrExit(error = aArgs[1].ParseAsUint16(xtalThreshold));
        SuccessOrExit(error = otNetworkTimeSetSyncPeriod(GetInstancePtr(), period));
        SuccessOrExit(error = otNetworkTimeSetXtalThreshold(GetInstancePtr(), xtalThreshold));
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

otError Interpreter::ProcessPanId(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("0x%04x", otLinkGetPanId(GetInstancePtr()));
    }
    else
    {
        error = ProcessSet(aArgs, otLinkSetPanId);
    }

    return error;
}

otError Interpreter::ProcessParent(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otError      error = OT_ERROR_NONE;
    otRouterInfo parentInfo;

    SuccessOrExit(error = otThreadGetParentInfo(GetInstancePtr(), &parentInfo));
    OutputFormat("Ext Addr: ");
    OutputExtAddressLine(parentInfo.mExtAddress);
    OutputLine("Rloc: %x", parentInfo.mRloc16);
    OutputLine("Link Quality In: %d", parentInfo.mLinkQualityIn);
    OutputLine("Link Quality Out: %d", parentInfo.mLinkQualityOut);
    OutputLine("Age: %d", parentInfo.mAge);

exit:
    return error;
}

#if OPENTHREAD_FTD
otError Interpreter::ProcessParentPriority(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetParentPriority, otThreadSetParentPriority);
}
#endif

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE

void Interpreter::HandlePingReply(const otPingSenderReply *aReply, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandlePingReply(aReply);
}

void Interpreter::HandlePingReply(const otPingSenderReply *aReply)
{
    OutputFormat("%u bytes from ", static_cast<uint16_t>(aReply->mSize + sizeof(otIcmp6Header)));
    OutputIp6Address(aReply->mSenderAddress);
    OutputLine(": icmp_seq=%d hlim=%d time=%dms", aReply->mSequenceNumber, aReply->mHopLimit, aReply->mRoundTripTime);
}

void Interpreter::HandlePingStatistics(const otPingSenderStatistics *aStatistics, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandlePingStatistics(aStatistics);
}

void Interpreter::HandlePingStatistics(const otPingSenderStatistics *aStatistics)
{
    OutputFormat("%u packets transmitted, %u packets received.", aStatistics->mSentCount, aStatistics->mReceivedCount);

    if ((aStatistics->mSentCount != 0) && !aStatistics->mIsMulticast &&
        aStatistics->mReceivedCount <= aStatistics->mSentCount)
    {
        uint32_t packetLossRate =
            1000 * (aStatistics->mSentCount - aStatistics->mReceivedCount) / aStatistics->mSentCount;
        OutputFormat(" Packet loss = %u.%u%%.", packetLossRate / 10, packetLossRate % 10);
    }

    if (aStatistics->mReceivedCount != 0)
    {
        uint32_t avgRoundTripTime = 1000 * aStatistics->mTotalRoundTripTime / aStatistics->mReceivedCount;
        OutputFormat(" Round-trip min/avg/max = %u/%u.%u/%u ms.", aStatistics->mMinRoundTripTime,
                     avgRoundTripTime / 1000, avgRoundTripTime % 1000, aStatistics->mMaxRoundTripTime);
    }

    OutputLine("");

    if (!mPingIsAsync)
    {
        OutputResult(OT_ERROR_NONE);
    }
}

otError Interpreter::ProcessPing(Arg aArgs[])
{
    otError            error = OT_ERROR_NONE;
    otPingSenderConfig config;
    bool               async = false;

    if (aArgs[0] == "stop")
    {
        otPingSenderStop(GetInstancePtr());
        ExitNow();
    }
    else if (aArgs[0] == "async")
    {
        async = true;
        aArgs++;
    }

    memset(&config, 0, sizeof(config));

    if (aArgs[0] == "-I")
    {
        SuccessOrExit(error = aArgs[1].ParseAsIp6Address(config.mSource));

#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        {
            bool                  valid        = false;
            const otNetifAddress *unicastAddrs = otIp6GetUnicastAddresses(GetInstancePtr());

            for (const otNetifAddress *addr = unicastAddrs; addr; addr = addr->mNext)
            {
                if (otIp6IsAddressEqual(&addr->mAddress, &config.mSource))
                {
                    valid = true;
                    break;
                }
            }

            VerifyOrExit(valid, error = OT_ERROR_INVALID_ARGS);
        }
#endif

        aArgs += 2;
    }

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(config.mDestination));

    if (!aArgs[1].IsEmpty())
    {
        SuccessOrExit(error = aArgs[1].ParseAsUint16(config.mSize));
    }

    if (!aArgs[2].IsEmpty())
    {
        SuccessOrExit(error = aArgs[2].ParseAsUint16(config.mCount));
    }

    if (!aArgs[3].IsEmpty())
    {
        SuccessOrExit(error = ParsePingInterval(aArgs[3], config.mInterval));
    }

    if (!aArgs[4].IsEmpty())
    {
        SuccessOrExit(error = aArgs[4].ParseAsUint8(config.mHopLimit));
        config.mAllowZeroHopLimit = (config.mHopLimit == 0);
    }

    if (!aArgs[5].IsEmpty())
    {
        uint32_t timeout;

        SuccessOrExit(error = ParsePingInterval(aArgs[5], timeout));
        VerifyOrExit(timeout <= NumericLimits<uint16_t>::kMax, error = OT_ERROR_INVALID_ARGS);
        config.mTimeout = static_cast<uint16_t>(timeout);
    }

    VerifyOrExit(aArgs[6].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    config.mReplyCallback      = Interpreter::HandlePingReply;
    config.mStatisticsCallback = Interpreter::HandlePingStatistics;
    config.mCallbackContext    = this;

    SuccessOrExit(error = otPingSenderPing(GetInstancePtr(), &config));

    mPingIsAsync = async;

    if (!async)
    {
        error = OT_ERROR_PENDING;
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_PING_SENDER_ENABLE

otError Interpreter::ProcessPollPeriod(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otLinkGetPollPeriod, otLinkSetPollPeriod);
}

otError Interpreter::ProcessPromiscuous(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputEnabledDisabledStatus(otLinkIsPromiscuous(GetInstancePtr()) &&
                                    otPlatRadioGetPromiscuous(GetInstancePtr()));
    }
    else
    {
        bool enable;

        SuccessOrExit(error = ParseEnableOrDisable(aArgs[0], enable));

        if (!enable)
        {
            otLinkSetPcapCallback(GetInstancePtr(), nullptr, nullptr);
        }

        SuccessOrExit(error = otLinkSetPromiscuous(GetInstancePtr(), enable));

        if (enable)
        {
            otLinkSetPcapCallback(GetInstancePtr(), &HandleLinkPcapReceive, this);
        }
    }

exit:
    return error;
}

void Interpreter::HandleLinkPcapReceive(const otRadioFrame *aFrame, bool aIsTx, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleLinkPcapReceive(aFrame, aIsTx);
}

void Interpreter::HandleLinkPcapReceive(const otRadioFrame *aFrame, bool aIsTx)
{
    OT_UNUSED_VARIABLE(aIsTx);

    OutputLine("");

    for (size_t i = 0; i < 44; i++)
    {
        OutputFormat("=");
    }

    OutputFormat("[len = %3u]", aFrame->mLength);

    for (size_t i = 0; i < 28; i++)
    {
        OutputFormat("=");
    }

    OutputLine("");

    for (size_t i = 0; i < aFrame->mLength; i += 16)
    {
        OutputFormat("|");

        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < aFrame->mLength)
            {
                OutputFormat(" %02X", aFrame->mPsdu[i + j]);
            }
            else
            {
                OutputFormat(" ..");
            }
        }

        OutputFormat("|");

        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < aFrame->mLength)
            {
                if (31 < aFrame->mPsdu[i + j] && aFrame->mPsdu[i + j] < 127)
                {
                    OutputFormat(" %c", aFrame->mPsdu[i + j]);
                }
                else
                {
                    OutputFormat(" ?");
                }
            }
            else
            {
                OutputFormat(" .");
            }
        }

        OutputLine("|");
    }

    for (size_t i = 0; i < 83; i++)
    {
        OutputFormat("-");
    }

    OutputLine("");
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
otError Interpreter::ParsePrefix(Arg aArgs[], otBorderRouterConfig &aConfig)
{
    otError error = OT_ERROR_NONE;

    memset(&aConfig, 0, sizeof(otBorderRouterConfig));

    SuccessOrExit(error = aArgs[0].ParseAsIp6Prefix(aConfig.mPrefix));
    aArgs++;

    for (; !aArgs->IsEmpty(); aArgs++)
    {
        if (*aArgs == "high")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_HIGH;
        }
        else if (*aArgs == "med")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_MED;
        }
        else if (*aArgs == "low")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_LOW;
        }
        else
        {
            for (char *arg = aArgs->GetCString(); *arg != '\0'; arg++)
            {
                switch (*arg)
                {
                case 'p':
                    aConfig.mPreferred = true;
                    break;

                case 'a':
                    aConfig.mSlaac = true;
                    break;

                case 'd':
                    aConfig.mDhcp = true;
                    break;

                case 'c':
                    aConfig.mConfigure = true;
                    break;

                case 'r':
                    aConfig.mDefaultRoute = true;
                    break;

                case 'o':
                    aConfig.mOnMesh = true;
                    break;

                case 's':
                    aConfig.mStable = true;
                    break;

                case 'n':
                    aConfig.mNdDns = true;
                    break;

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
                case 'D':
                    aConfig.mDp = true;
                    break;
#endif
                default:
                    ExitNow(error = OT_ERROR_INVALID_ARGS);
                }
            }
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessPrefixAdd(Arg aArgs[])
{
    otError              error = OT_ERROR_NONE;
    otBorderRouterConfig config;

    SuccessOrExit(error = ParsePrefix(aArgs, config));
    error = otBorderRouterAddOnMeshPrefix(GetInstancePtr(), &config);

exit:
    return error;
}

otError Interpreter::ProcessPrefixRemove(Arg aArgs[])
{
    otError     error = OT_ERROR_NONE;
    otIp6Prefix prefix;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Prefix(prefix));

    error = otBorderRouterRemoveOnMeshPrefix(GetInstancePtr(), &prefix);

exit:
    return error;
}

otError Interpreter::ProcessPrefixList(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig  config;

    while (otBorderRouterGetNextOnMeshPrefix(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        mNetworkData.OutputPrefix(config);
    }

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (otBackboneRouterGetState(GetInstancePtr()) == OT_BACKBONE_ROUTER_STATE_DISABLED)
    {
        SuccessOrExit(otBackboneRouterGetDomainPrefix(GetInstancePtr(), &config));
        OutputFormat("- ");
        mNetworkData.OutputPrefix(config);
    }
    // Else already printed via above while loop.
exit:
#endif

    return OT_ERROR_NONE;
}

otError Interpreter::ProcessPrefix(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = ProcessPrefixList());
    }
    else if (aArgs[0] == "add")
    {
        SuccessOrExit(error = ProcessPrefixAdd(aArgs + 1));
    }
    else if (aArgs[0] == "remove")
    {
        SuccessOrExit(error = ProcessPrefixRemove(aArgs + 1));
    }
    else if (aArgs[0] == "meshlocal")
    {
        OutputIp6PrefixLine(*otThreadGetMeshLocalPrefix(GetInstancePtr()));
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#if OPENTHREAD_FTD
otError Interpreter::ProcessPreferRouterId(Arg aArgs[])
{
    return ProcessSet(aArgs, otThreadSetPreferredRouterId);
}
#endif

otError Interpreter::ProcessRcp(Arg aArgs[])
{
    otError     error   = OT_ERROR_NONE;
    const char *version = otPlatRadioGetVersionString(GetInstancePtr());

    VerifyOrExit(version != otGetVersionString(), error = OT_ERROR_NOT_IMPLEMENTED);

    if (aArgs[0] == "version")
    {
        OutputLine("%s", version);
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

exit:
    return error;
}

otError Interpreter::ProcessRegion(Arg aArgs[])
{
    otError  error = OT_ERROR_NONE;
    uint16_t regionCode;

    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = otPlatRadioGetRegion(GetInstancePtr(), &regionCode));
        OutputLine("%c%c", regionCode >> 8, regionCode & 0xff);
    }
    else
    {
        VerifyOrExit(aArgs[0].GetLength() == 2, error = OT_ERROR_INVALID_ARGS);

        regionCode = static_cast<uint16_t>(static_cast<uint16_t>(aArgs[0].GetCString()[0]) << 8) +
                     static_cast<uint16_t>(aArgs[0].GetCString()[1]);
        error = otPlatRadioSetRegion(GetInstancePtr(), regionCode);
    }

exit:
    return error;
}

#if OPENTHREAD_FTD
otError Interpreter::ProcessReleaseRouterId(Arg aArgs[])
{
    return ProcessSet(aArgs, otThreadReleaseRouterId);
}
#endif

otError Interpreter::ProcessRloc16(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    OutputLine("%04x", otThreadGetRloc16(GetInstancePtr()));

    return OT_ERROR_NONE;
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
otError Interpreter::ParseRoute(Arg aArgs[], otExternalRouteConfig &aConfig)
{
    otError error = OT_ERROR_NONE;

    memset(&aConfig, 0, sizeof(otExternalRouteConfig));

    SuccessOrExit(error = aArgs[0].ParseAsIp6Prefix(aConfig.mPrefix));
    aArgs++;

    for (; !aArgs->IsEmpty(); aArgs++)
    {
        if (*aArgs == "s")
        {
            aConfig.mStable = true;
        }
        else if (*aArgs == "n")
        {
            aConfig.mNat64 = true;
        }
        else if (*aArgs == "high")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_HIGH;
        }
        else if (*aArgs == "med")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_MED;
        }
        else if (*aArgs == "low")
        {
            aConfig.mPreference = OT_ROUTE_PREFERENCE_LOW;
        }
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessRouteAdd(Arg aArgs[])
{
    otError               error;
    otExternalRouteConfig config;

    SuccessOrExit(error = ParseRoute(aArgs, config));
    error = otBorderRouterAddRoute(GetInstancePtr(), &config);

exit:
    return error;
}

otError Interpreter::ProcessRouteRemove(Arg aArgs[])
{
    otError     error = OT_ERROR_NONE;
    otIp6Prefix prefix;

    SuccessOrExit(error = aArgs[0].ParseAsIp6Prefix(prefix));

    error = otBorderRouterRemoveRoute(GetInstancePtr(), &prefix);

exit:
    return error;
}

otError Interpreter::ProcessRouteList(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig config;

    while (otBorderRouterGetNextRoute(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        mNetworkData.OutputRoute(config);
    }

    return OT_ERROR_NONE;
}

otError Interpreter::ProcessRoute(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = ProcessRouteList());
    }
    else if (aArgs[0] == "add")
    {
        SuccessOrExit(error = ProcessRouteAdd(aArgs + 1));
    }
    else if (aArgs[0] == "remove")
    {
        SuccessOrExit(error = ProcessRouteRemove(aArgs + 1));
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#if OPENTHREAD_FTD
otError Interpreter::ProcessRouter(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    otRouterInfo routerInfo;
    uint16_t     routerId;
    bool         isTable;

    isTable = (aArgs[0] == "table");

    if (isTable || (aArgs[0] == "list"))
    {
        uint8_t maxRouterId;

        if (isTable)
        {
            static const char *const kRouterTableTitles[] = {
                "ID", "RLOC16", "Next Hop", "Path Cost", "LQ In", "LQ Out", "Age", "Extended MAC", "Link",
            };

            static const uint8_t kRouterTableColumnWidths[] = {
                4, 8, 10, 11, 7, 8, 5, 18, 6,
            };

            OutputTableHeader(kRouterTableTitles, kRouterTableColumnWidths);
        }

        maxRouterId = otThreadGetMaxRouterId(GetInstancePtr());

        for (uint8_t i = 0; i <= maxRouterId; i++)
        {
            if (otThreadGetRouterInfo(GetInstancePtr(), i, &routerInfo) != OT_ERROR_NONE)
            {
                continue;
            }

            if (isTable)
            {
                OutputFormat("| %2d ", routerInfo.mRouterId);
                OutputFormat("| 0x%04x ", routerInfo.mRloc16);
                OutputFormat("| %8d ", routerInfo.mNextHop);
                OutputFormat("| %9d ", routerInfo.mPathCost);
                OutputFormat("| %5d ", routerInfo.mLinkQualityIn);
                OutputFormat("| %6d ", routerInfo.mLinkQualityOut);
                OutputFormat("| %3d ", routerInfo.mAge);
                OutputFormat("| ");
                OutputExtAddress(routerInfo.mExtAddress);
                OutputLine(" | %4d |", routerInfo.mLinkEstablished);
            }
            else
            {
                OutputFormat("%d ", i);
            }
        }

        OutputLine("");
        ExitNow();
    }

    SuccessOrExit(error = aArgs[0].ParseAsUint16(routerId));
    SuccessOrExit(error = otThreadGetRouterInfo(GetInstancePtr(), routerId, &routerInfo));

    OutputLine("Alloc: %d", routerInfo.mAllocated);

    if (routerInfo.mAllocated)
    {
        OutputLine("Router ID: %d", routerInfo.mRouterId);
        OutputLine("Rloc: %04x", routerInfo.mRloc16);
        OutputLine("Next Hop: %04x", static_cast<uint16_t>(routerInfo.mNextHop) << 10);
        OutputLine("Link: %d", routerInfo.mLinkEstablished);

        if (routerInfo.mLinkEstablished)
        {
            OutputFormat("Ext Addr: ");
            OutputExtAddressLine(routerInfo.mExtAddress);
            OutputLine("Cost: %d", routerInfo.mPathCost);
            OutputLine("Link Quality In: %d", routerInfo.mLinkQualityIn);
            OutputLine("Link Quality Out: %d", routerInfo.mLinkQualityOut);
            OutputLine("Age: %d", routerInfo.mAge);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessRouterDowngradeThreshold(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetRouterDowngradeThreshold, otThreadSetRouterDowngradeThreshold);
}

otError Interpreter::ProcessRouterEligible(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputEnabledDisabledStatus(otThreadIsRouterEligible(GetInstancePtr()));
    }
    else
    {
        bool enable;

        SuccessOrExit(error = ParseEnableOrDisable(aArgs[0], enable));
        error = otThreadSetRouterEligible(GetInstancePtr(), enable);
    }

exit:
    return error;
}

otError Interpreter::ProcessRouterSelectionJitter(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetRouterSelectionJitter, otThreadSetRouterSelectionJitter);
}

otError Interpreter::ProcessRouterUpgradeThreshold(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetRouterUpgradeThreshold, otThreadSetRouterUpgradeThreshold);
}
#endif // OPENTHREAD_FTD

otError Interpreter::ProcessScan(Arg aArgs[])
{
    otError  error        = OT_ERROR_NONE;
    uint32_t scanChannels = 0;
    uint16_t scanDuration = 0;
    bool     energyScan   = false;

    if (aArgs[0] == "energy")
    {
        energyScan = true;
        aArgs++;

        if (!aArgs->IsEmpty())
        {
            SuccessOrExit(error = aArgs->ParseAsUint16(scanDuration));
            aArgs++;
        }
    }

    if (!aArgs->IsEmpty())
    {
        uint8_t channel;

        SuccessOrExit(error = aArgs->ParseAsUint8(channel));
        VerifyOrExit(channel < sizeof(scanChannels) * CHAR_BIT, error = OT_ERROR_INVALID_ARGS);
        scanChannels = 1 << channel;
    }

    if (energyScan)
    {
        static const char *const kEnergyScanTableTitles[]       = {"Ch", "RSSI"};
        static const uint8_t     kEnergyScanTableColumnWidths[] = {4, 6};

        OutputTableHeader(kEnergyScanTableTitles, kEnergyScanTableColumnWidths);
        SuccessOrExit(error = otLinkEnergyScan(GetInstancePtr(), scanChannels, scanDuration,
                                               &Interpreter::HandleEnergyScanResult, this));
    }
    else
    {
        OutputScanTableHeader();
        SuccessOrExit(error = otLinkActiveScan(GetInstancePtr(), scanChannels, scanDuration,
                                               &Interpreter::HandleActiveScanResult, this));
    }

    error = OT_ERROR_PENDING;

exit:
    return error;
}

void Interpreter::OutputScanTableHeader(void)
{
    static const char *const kScanTableTitles[] = {
        "J", "Network Name", "Extended PAN", "PAN", "MAC Address", "Ch", "dBm", "LQI",
    };

    static const uint8_t kScanTableColumnWidths[] = {
        3, 18, 18, 6, 18, 4, 5, 5,
    };

    OutputTableHeader(kScanTableTitles, kScanTableColumnWidths);
}

void Interpreter::HandleActiveScanResult(otActiveScanResult *aResult, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleActiveScanResult(aResult);
}

void Interpreter::HandleActiveScanResult(otActiveScanResult *aResult)
{
    if (aResult == nullptr)
    {
        OutputResult(OT_ERROR_NONE);
        ExitNow();
    }

    OutputFormat("| %d ", aResult->mIsJoinable);

    OutputFormat("| %-16s ", aResult->mNetworkName.m8);

    OutputFormat("| ");
    OutputBytes(aResult->mExtendedPanId.m8);
    OutputFormat(" ");

    OutputFormat("| %04x | ", aResult->mPanId);
    OutputExtAddress(aResult->mExtAddress);
    OutputFormat(" | %2d ", aResult->mChannel);
    OutputFormat("| %3d ", aResult->mRssi);
    OutputLine("| %3d |", aResult->mLqi);

exit:
    return;
}

void Interpreter::HandleEnergyScanResult(otEnergyScanResult *aResult, void *aContext)
{
    static_cast<Interpreter *>(aContext)->HandleEnergyScanResult(aResult);
}

void Interpreter::HandleEnergyScanResult(otEnergyScanResult *aResult)
{
    if (aResult == nullptr)
    {
        OutputResult(OT_ERROR_NONE);
        ExitNow();
    }

    OutputLine("| %2d | %4d |", aResult->mChannel, aResult->mMaxRssi);

exit:
    return;
}

otError Interpreter::ProcessSingleton(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    if (otThreadIsSingleton(GetInstancePtr()))
    {
        OutputLine("true");
    }
    else
    {
        OutputLine("false");
    }

    return OT_ERROR_NONE;
}

#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
otError Interpreter::ProcessSntp(Arg aArgs[])
{
    otError          error = OT_ERROR_NONE;
    uint16_t         port  = OT_SNTP_DEFAULT_SERVER_PORT;
    Ip6::MessageInfo messageInfo;
    otSntpQuery      query;

    if (aArgs[0] == "query")
    {
        VerifyOrExit(!mSntpQueryingInProgress, error = OT_ERROR_BUSY);

        if (!aArgs[1].IsEmpty())
        {
            SuccessOrExit(error = aArgs[1].ParseAsIp6Address(messageInfo.GetPeerAddr()));
        }
        else
        {
            // Use IPv6 address of default SNTP server.
            SuccessOrExit(error = messageInfo.GetPeerAddr().FromString(OT_SNTP_DEFAULT_SERVER_IP));
        }

        if (!aArgs[2].IsEmpty())
        {
            SuccessOrExit(error = aArgs[2].ParseAsUint16(port));
        }

        messageInfo.SetPeerPort(port);

        query.mMessageInfo = static_cast<const otMessageInfo *>(&messageInfo);

        SuccessOrExit(error = otSntpClientQuery(GetInstancePtr(), &query, &Interpreter::HandleSntpResponse, this));

        mSntpQueryingInProgress = true;
        error                   = OT_ERROR_PENDING;
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

exit:
    return error;
}

void Interpreter::HandleSntpResponse(void *aContext, uint64_t aTime, otError aResult)
{
    static_cast<Interpreter *>(aContext)->HandleSntpResponse(aTime, aResult);
}

void Interpreter::HandleSntpResponse(uint64_t aTime, otError aResult)
{
    if (aResult == OT_ERROR_NONE)
    {
        // Some Embedded C libraries do not support printing of 64-bit unsigned integers.
        // To simplify, unix epoch time and era number are printed separately.
        OutputLine("SNTP response - Unix time: %u (era: %u)", static_cast<uint32_t>(aTime),
                   static_cast<uint32_t>(aTime >> 32));
    }
    else
    {
        OutputLine("SNTP error - %s", otThreadErrorToString(aResult));
    }

    mSntpQueryingInProgress = false;

    OutputResult(OT_ERROR_NONE);
}
#endif // OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE || OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
otError Interpreter::ProcessSrp(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
        OutputLine("client");
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
        OutputLine("server");
#endif
        ExitNow();
    }

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    if (aArgs[0] == "client")
    {
        ExitNow(error = mSrpClient.Process(aArgs + 1));
    }
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    if (aArgs[0] == "server")
    {
        ExitNow(error = mSrpServer.Process(aArgs + 1));
    }
#endif

    error = OT_ERROR_INVALID_COMMAND;

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE || OPENTHREAD_CONFIG_SRP_SERVER_ENABLE

otError Interpreter::ProcessState(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%s", otThreadDeviceRoleToString(otThreadGetDeviceRole(GetInstancePtr())));
    }
    else
    {
        if (aArgs[0] == "detached")
        {
            SuccessOrExit(error = otThreadBecomeDetached(GetInstancePtr()));
        }
        else if (aArgs[0] == "child")
        {
            SuccessOrExit(error = otThreadBecomeChild(GetInstancePtr()));
        }

#if OPENTHREAD_FTD
        else if (aArgs[0] == "router")
        {
            SuccessOrExit(error = otThreadBecomeRouter(GetInstancePtr()));
        }
        else if (aArgs[0] == "leader")
        {
            SuccessOrExit(error = otThreadBecomeLeader(GetInstancePtr()));
        }
#endif
        else
        {
            ExitNow(error = OT_ERROR_INVALID_ARGS);
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessThread(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "start")
    {
        error = otThreadSetEnabled(GetInstancePtr(), true);
    }
    else if (aArgs[0] == "stop")
    {
        error = otThreadSetEnabled(GetInstancePtr(), false);
    }
    else if (aArgs[0] == "version")
    {
        OutputLine("%u", otThreadGetVersion());
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

    return error;
}

otError Interpreter::ProcessDataset(Arg aArgs[])
{
    return mDataset.Process(aArgs);
}

otError Interpreter::ProcessTxPower(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    int8_t  power;

    if (aArgs[0].IsEmpty())
    {
        SuccessOrExit(error = otPlatRadioGetTransmitPower(GetInstancePtr(), &power));
        OutputLine("%d dBm", power);
    }
    else
    {
        SuccessOrExit(error = aArgs[0].ParseAsInt8(power));
        SuccessOrExit(error = otPlatRadioSetTransmitPower(GetInstancePtr(), power));
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
otError Interpreter::ProcessTcp(Arg aArgs[])
{
    return mTcp.Process(aArgs);
}
#endif

otError Interpreter::ProcessUdp(Arg aArgs[])
{
    return mUdp.Process(aArgs);
}

otError Interpreter::ProcessUnsecurePort(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "add")
    {
        error = ProcessSet(aArgs + 1, otIp6AddUnsecurePort);
    }
    else if (aArgs[0] == "remove")
    {
        if (aArgs[1] == "all")
        {
            otIp6RemoveAllUnsecurePorts(GetInstancePtr());
        }
        else
        {
            error = ProcessSet(aArgs + 1, otIp6RemoveUnsecurePort);
        }
    }
    else if (aArgs[0] == "get")
    {
        const uint16_t *ports;
        uint8_t         numPorts;

        ports = otIp6GetUnsecurePorts(GetInstancePtr(), &numPorts);

        if (ports != nullptr)
        {
            for (uint8_t i = 0; i < numPorts; i++)
            {
                OutputFormat("%d ", ports[i]);
            }
        }

        OutputLine("");
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_COMMAND);
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_UPTIME_ENABLE
otError Interpreter::ProcessUptime(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        char string[OT_UPTIME_STRING_SIZE];

        otInstanceGetUptimeAsString(GetInstancePtr(), string, sizeof(string));
        OutputLine("%s", string);
    }
    else if (aArgs[0] == "ms")
    {
        OutputLine("%lu", otInstanceGetUptime(GetInstancePtr()));
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}
#endif

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
otError Interpreter::ProcessCommissioner(Arg aArgs[])
{
    return mCommissioner.Process(aArgs);
}
#endif

#if OPENTHREAD_CONFIG_JOINER_ENABLE
otError Interpreter::ProcessJoiner(Arg aArgs[])
{
    return mJoiner.Process(aArgs);
}
#endif

#if OPENTHREAD_FTD
otError Interpreter::ProcessJoinerPort(Arg aArgs[])
{
    return ProcessGetSet(aArgs, otThreadGetJoinerUdpPort, otThreadSetJoinerUdpPort);
}
#endif

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
otError Interpreter::ProcessMacFilter(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0].IsEmpty())
    {
        PrintMacFilter();
    }
    else
    {
        if (aArgs[0] == "addr")
        {
            error = ProcessMacFilterAddress(aArgs + 1);
        }
        else if (aArgs[0] == "rss")
        {
            error = ProcessMacFilterRss(aArgs + 1);
        }
        else
        {
            error = OT_ERROR_INVALID_COMMAND;
        }
    }

    return error;
}

void Interpreter::PrintMacFilter(void)
{
    otMacFilterEntry       entry;
    otMacFilterIterator    iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otMacFilterAddressMode mode     = otLinkFilterGetAddressMode(GetInstancePtr());

    if (mode == OT_MAC_FILTER_ADDRESS_MODE_DISABLED)
    {
        OutputLine("Address Mode: Disabled");
    }
    else if (mode == OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST)
    {
        OutputLine("Address Mode: Allowlist");
    }
    else if (mode == OT_MAC_FILTER_ADDRESS_MODE_DENYLIST)
    {
        OutputLine("Address Mode: Denylist");
    }

    while (otLinkFilterGetNextAddress(GetInstancePtr(), &iterator, &entry) == OT_ERROR_NONE)
    {
        OutputExtAddress(entry.mExtAddress);

        if (entry.mRssIn != OT_MAC_FILTER_FIXED_RSS_DISABLED)
        {
            OutputFormat(" : rss %d (lqi %d)", entry.mRssIn,
                         otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
        }

        OutputLine("");
    }

    iterator = OT_MAC_FILTER_ITERATOR_INIT;
    OutputLine("RssIn List:");

    while (otLinkFilterGetNextRssIn(GetInstancePtr(), &iterator, &entry) == OT_ERROR_NONE)
    {
        uint8_t i = 0;

        for (; i < OT_EXT_ADDRESS_SIZE; i++)
        {
            if (entry.mExtAddress.m8[i] != 0xff)
            {
                break;
            }
        }

        if (i == OT_EXT_ADDRESS_SIZE)
        {
            OutputLine("Default rss : %d (lqi %d)", entry.mRssIn,
                       otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
        }
        else
        {
            OutputExtAddress(entry.mExtAddress);
            OutputLine(" : rss %d (lqi %d)", entry.mRssIn,
                       otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
        }
    }
}

otError Interpreter::ProcessMacFilterAddress(Arg aArgs[])
{
    otError                error = OT_ERROR_NONE;
    otExtAddress           extAddr;
    otMacFilterEntry       entry;
    otMacFilterIterator    iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otMacFilterAddressMode mode     = otLinkFilterGetAddressMode(GetInstancePtr());

    if (aArgs[0].IsEmpty())
    {
        if (mode == OT_MAC_FILTER_ADDRESS_MODE_DISABLED)
        {
            OutputLine("Disabled");
        }
        else if (mode == OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST)
        {
            OutputLine("Allowlist");
        }
        else if (mode == OT_MAC_FILTER_ADDRESS_MODE_DENYLIST)
        {
            OutputLine("Denylist");
        }

        while (otLinkFilterGetNextAddress(GetInstancePtr(), &iterator, &entry) == OT_ERROR_NONE)
        {
            OutputExtAddress(entry.mExtAddress);

            if (entry.mRssIn != OT_MAC_FILTER_FIXED_RSS_DISABLED)
            {
                OutputFormat(" : rss %d (lqi %d)", entry.mRssIn,
                             otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
            }

            OutputLine("");
        }
    }
    else
    {
        if (aArgs[0] == "disable")
        {
            VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            otLinkFilterSetAddressMode(GetInstancePtr(), OT_MAC_FILTER_ADDRESS_MODE_DISABLED);
        }
        else if (aArgs[0] == "allowlist")
        {
            VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            otLinkFilterSetAddressMode(GetInstancePtr(), OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST);
        }
        else if (aArgs[0] == "denylist")
        {
            VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
            otLinkFilterSetAddressMode(GetInstancePtr(), OT_MAC_FILTER_ADDRESS_MODE_DENYLIST);
        }
        else if (aArgs[0] == "add")
        {
            SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddr.m8));
            error = otLinkFilterAddAddress(GetInstancePtr(), &extAddr);

            VerifyOrExit(error == OT_ERROR_NONE || error == OT_ERROR_ALREADY);

            if (!aArgs[2].IsEmpty())
            {
                int8_t rss;

                SuccessOrExit(error = aArgs[2].ParseAsInt8(rss));
                SuccessOrExit(error = otLinkFilterAddRssIn(GetInstancePtr(), &extAddr, rss));
            }
        }
        else if (aArgs[0] == "remove")
        {
            SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddr.m8));
            otLinkFilterRemoveAddress(GetInstancePtr(), &extAddr);
        }
        else if (aArgs[0] == "clear")
        {
            otLinkFilterClearAddresses(GetInstancePtr());
        }
        else
        {
            error = OT_ERROR_INVALID_COMMAND;
        }
    }

exit:
    return error;
}

otError Interpreter::ProcessMacFilterRss(Arg aArgs[])
{
    otError             error = OT_ERROR_NONE;
    otMacFilterEntry    entry;
    otMacFilterIterator iterator = OT_MAC_FILTER_ITERATOR_INIT;
    otExtAddress        extAddr;
    int8_t              rss;

    if (aArgs[0].IsEmpty())
    {
        while (otLinkFilterGetNextRssIn(GetInstancePtr(), &iterator, &entry) == OT_ERROR_NONE)
        {
            uint8_t i = 0;

            for (; i < OT_EXT_ADDRESS_SIZE; i++)
            {
                if (entry.mExtAddress.m8[i] != 0xff)
                {
                    break;
                }
            }

            if (i == OT_EXT_ADDRESS_SIZE)
            {
                OutputLine("Default rss: %d (lqi %d)", entry.mRssIn,
                           otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
            }
            else
            {
                OutputExtAddress(entry.mExtAddress);
                OutputLine(" : rss %d (lqi %d)", entry.mRssIn,
                           otLinkConvertRssToLinkQuality(GetInstancePtr(), entry.mRssIn));
            }
        }
    }
    else
    {
        if (aArgs[0] == "add-lqi")
        {
            uint8_t linkQuality;

            SuccessOrExit(error = aArgs[2].ParseAsUint8(linkQuality));
            VerifyOrExit(linkQuality <= 3, error = OT_ERROR_INVALID_ARGS);
            rss = otLinkConvertLinkQualityToRss(GetInstancePtr(), linkQuality);

            if (aArgs[1] == "*")
            {
                otLinkFilterSetDefaultRssIn(GetInstancePtr(), rss);
            }
            else
            {
                SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddr.m8));
                SuccessOrExit(error = otLinkFilterAddRssIn(GetInstancePtr(), &extAddr, rss));
            }
        }
        else if (aArgs[0] == "add")
        {
            SuccessOrExit(error = aArgs[2].ParseAsInt8(rss));

            if (aArgs[1] == "*")
            {
                otLinkFilterSetDefaultRssIn(GetInstancePtr(), rss);
            }
            else
            {
                SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddr.m8));
                SuccessOrExit(error = otLinkFilterAddRssIn(GetInstancePtr(), &extAddr, rss));
            }
        }
        else if (aArgs[0] == "remove")
        {
            if (aArgs[1] == "*")
            {
                otLinkFilterClearDefaultRssIn(GetInstancePtr());
            }
            else
            {
                SuccessOrExit(error = aArgs[1].ParseAsHexString(extAddr.m8));
                otLinkFilterRemoveRssIn(GetInstancePtr(), &extAddr);
            }
        }
        else if (aArgs[0] == "clear")
        {
            otLinkFilterClearAllRssIn(GetInstancePtr());
        }
        else
        {
            error = OT_ERROR_INVALID_COMMAND;
        }
    }

exit:
    return error;
}

#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

otError Interpreter::ProcessMac(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "retries")
    {
        error = ProcessMacRetries(aArgs + 1);
    }
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    else if (aArgs[0] == "send")
    {
        error = ProcessMacSend(aArgs + 1);
    }
#endif
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

    return error;
}

otError Interpreter::ProcessMacRetries(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (aArgs[0] == "direct")
    {
        error = ProcessGetSet(aArgs + 1, otLinkGetMaxFrameRetriesDirect, otLinkSetMaxFrameRetriesDirect);
    }
#if OPENTHREAD_FTD
    else if (aArgs[0] == "indirect")
    {
        error = ProcessGetSet(aArgs + 1, otLinkGetMaxFrameRetriesIndirect, otLinkSetMaxFrameRetriesIndirect);
    }
#endif
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
otError Interpreter::ProcessMacSend(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;

    VerifyOrExit(aArgs[1].IsEmpty());

    if (aArgs[0] == "datarequest")
    {
        error = otLinkSendDataRequest(GetInstancePtr());
    }
    else if (aArgs[0] == "emptydata")
    {
        error = otLinkSendEmptyData(GetInstancePtr());
    }

exit:
    return error;
}
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
otError Interpreter::ProcessTrel(Arg aArgs[])
{
    otError error;
    bool    enable;

    SuccessOrExit(error = ParseEnableOrDisable(aArgs[0], enable));

    error = otPlatTrelUdp6SetTestMode(GetInstancePtr(), enable);

exit:
    return error;
}
#endif

#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
otError Interpreter::ProcessNetworkDiagnostic(Arg aArgs[])
{
    otError      error = OT_ERROR_NONE;
    otIp6Address address;
    uint8_t      tlvTypes[OT_NETWORK_DIAGNOSTIC_TYPELIST_MAX_ENTRIES];
    uint8_t      count = 0;

    SuccessOrExit(error = aArgs[1].ParseAsIp6Address(address));

    for (Arg *arg = &aArgs[2]; !arg->IsEmpty(); arg++)
    {
        VerifyOrExit(count < sizeof(tlvTypes), error = OT_ERROR_INVALID_ARGS);
        SuccessOrExit(error = arg->ParseAsUint8(tlvTypes[count++]));
    }

    if (aArgs[0] == "get")
    {
        SuccessOrExit(error = otThreadSendDiagnosticGet(GetInstancePtr(), &address, tlvTypes, count,
                                                        &Interpreter::HandleDiagnosticGetResponse, this));
        SetCommandTimeout(kNetworkDiagnosticTimeoutMsecs);
        error = OT_ERROR_PENDING;
    }
    else if (aArgs[0] == "reset")
    {
        IgnoreError(otThreadSendDiagnosticReset(GetInstancePtr(), &address, tlvTypes, count));
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

exit:
    return error;
}

void Interpreter::HandleDiagnosticGetResponse(otError              aError,
                                              otMessage *          aMessage,
                                              const otMessageInfo *aMessageInfo,
                                              void *               aContext)
{
    static_cast<Interpreter *>(aContext)->HandleDiagnosticGetResponse(
        aError, aMessage, static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Interpreter::HandleDiagnosticGetResponse(otError                 aError,
                                              const otMessage *       aMessage,
                                              const Ip6::MessageInfo *aMessageInfo)
{
    uint8_t               buf[16];
    uint16_t              bytesToPrint;
    uint16_t              bytesPrinted = 0;
    uint16_t              length;
    otNetworkDiagTlv      diagTlv;
    otNetworkDiagIterator iterator = OT_NETWORK_DIAGNOSTIC_ITERATOR_INIT;

    SuccessOrExit(aError);

    OutputFormat("DIAG_GET.rsp/ans from ");
    OutputIp6Address(aMessageInfo->mPeerAddr);
    OutputFormat(": ");

    length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);

    while (length > 0)
    {
        bytesToPrint = (length < sizeof(buf)) ? length : sizeof(buf);
        otMessageRead(aMessage, otMessageGetOffset(aMessage) + bytesPrinted, buf, bytesToPrint);

        OutputBytes(buf, static_cast<uint8_t>(bytesToPrint));

        length -= bytesToPrint;
        bytesPrinted += bytesToPrint;
    }

    OutputLine("");

    // Output Network Diagnostic TLV values in standard YAML format.
    while ((aError = otThreadGetNextDiagnosticTlv(aMessage, &iterator, &diagTlv)) == OT_ERROR_NONE)
    {
        switch (diagTlv.mType)
        {
        case OT_NETWORK_DIAGNOSTIC_TLV_EXT_ADDRESS:
            OutputFormat("Ext Address: '");
            OutputExtAddressLine(diagTlv.mData.mExtAddress);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_SHORT_ADDRESS:
            OutputLine("Rloc16: 0x%04x", diagTlv.mData.mAddr16);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_MODE:
            OutputLine("Mode:");
            OutputMode(kIndentSize, diagTlv.mData.mMode);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_TIMEOUT:
            OutputLine("Timeout: %u", diagTlv.mData.mTimeout);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_CONNECTIVITY:
            OutputLine("Connectivity:");
            OutputConnectivity(kIndentSize, diagTlv.mData.mConnectivity);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_ROUTE:
            OutputLine("Route:");
            OutputRoute(kIndentSize, diagTlv.mData.mRoute);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_LEADER_DATA:
            OutputLine("Leader Data:");
            OutputLeaderData(kIndentSize, diagTlv.mData.mLeaderData);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_NETWORK_DATA:
            OutputFormat("Network Data: '");
            OutputBytesLine(diagTlv.mData.mNetworkData.m8, diagTlv.mData.mNetworkData.mCount);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_IP6_ADDR_LIST:
            OutputLine("IP6 Address List:");
            for (uint16_t i = 0; i < diagTlv.mData.mIp6AddrList.mCount; ++i)
            {
                OutputFormat(kIndentSize, "- ");
                OutputIp6AddressLine(diagTlv.mData.mIp6AddrList.mList[i]);
            }
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_MAC_COUNTERS:
            OutputLine("MAC Counters:");
            OutputNetworkDiagMacCounters(kIndentSize, diagTlv.mData.mMacCounters);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_BATTERY_LEVEL:
            OutputLine("Battery Level: %u%%", diagTlv.mData.mBatteryLevel);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_SUPPLY_VOLTAGE:
            OutputLine("Supply Voltage: %umV", diagTlv.mData.mSupplyVoltage);
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE:
            OutputLine("Child Table:");
            for (uint16_t i = 0; i < diagTlv.mData.mChildTable.mCount; ++i)
            {
                OutputFormat(kIndentSize, "- ");
                OutputChildTableEntry(kIndentSize + 2, diagTlv.mData.mChildTable.mTable[i]);
            }
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_CHANNEL_PAGES:
            OutputFormat("Channel Pages: '");
            OutputBytes(diagTlv.mData.mChannelPages.m8, diagTlv.mData.mChannelPages.mCount);
            OutputLine("'");
            break;
        case OT_NETWORK_DIAGNOSTIC_TLV_MAX_CHILD_TIMEOUT:
            OutputLine("Max Child Timeout: %u", diagTlv.mData.mMaxChildTimeout);
            break;
        }
    }

exit:
    return;
}

void Interpreter::OutputMode(uint8_t aIndentSize, const otLinkModeConfig &aMode)
{
    OutputLine(aIndentSize, "RxOnWhenIdle: %d", aMode.mRxOnWhenIdle);
    OutputLine(aIndentSize, "DeviceType: %d", aMode.mDeviceType);
    OutputLine(aIndentSize, "NetworkData: %d", aMode.mNetworkData);
}

void Interpreter::OutputConnectivity(uint8_t aIndentSize, const otNetworkDiagConnectivity &aConnectivity)
{
    OutputLine(aIndentSize, "ParentPriority: %d", aConnectivity.mParentPriority);
    OutputLine(aIndentSize, "LinkQuality3: %u", aConnectivity.mLinkQuality3);
    OutputLine(aIndentSize, "LinkQuality2: %u", aConnectivity.mLinkQuality2);
    OutputLine(aIndentSize, "LinkQuality1: %u", aConnectivity.mLinkQuality1);
    OutputLine(aIndentSize, "LeaderCost: %u", aConnectivity.mLeaderCost);
    OutputLine(aIndentSize, "IdSequence: %u", aConnectivity.mIdSequence);
    OutputLine(aIndentSize, "ActiveRouters: %u", aConnectivity.mActiveRouters);
    OutputLine(aIndentSize, "SedBufferSize: %u", aConnectivity.mSedBufferSize);
    OutputLine(aIndentSize, "SedDatagramCount: %u", aConnectivity.mSedDatagramCount);
}
void Interpreter::OutputRoute(uint8_t aIndentSize, const otNetworkDiagRoute &aRoute)
{
    OutputLine(aIndentSize, "IdSequence: %u", aRoute.mIdSequence);
    OutputLine(aIndentSize, "RouteData:");

    aIndentSize += kIndentSize;
    for (uint16_t i = 0; i < aRoute.mRouteCount; ++i)
    {
        OutputFormat(aIndentSize, "- ");
        OutputRouteData(aIndentSize + 2, aRoute.mRouteData[i]);
    }
}

void Interpreter::OutputRouteData(uint8_t aIndentSize, const otNetworkDiagRouteData &aRouteData)
{
    OutputLine("RouteId: 0x%02x", aRouteData.mRouterId);

    OutputLine(aIndentSize, "LinkQualityOut: %u", aRouteData.mLinkQualityOut);
    OutputLine(aIndentSize, "LinkQualityIn: %u", aRouteData.mLinkQualityIn);
    OutputLine(aIndentSize, "RouteCost: %u", aRouteData.mRouteCost);
}

void Interpreter::OutputLeaderData(uint8_t aIndentSize, const otLeaderData &aLeaderData)
{
    OutputLine(aIndentSize, "PartitionId: 0x%08x", aLeaderData.mPartitionId);
    OutputLine(aIndentSize, "Weighting: %u", aLeaderData.mWeighting);
    OutputLine(aIndentSize, "DataVersion: %u", aLeaderData.mDataVersion);
    OutputLine(aIndentSize, "StableDataVersion: %u", aLeaderData.mStableDataVersion);
    OutputLine(aIndentSize, "LeaderRouterId: 0x%02x", aLeaderData.mLeaderRouterId);
}

void Interpreter::OutputNetworkDiagMacCounters(uint8_t aIndentSize, const otNetworkDiagMacCounters &aMacCounters)
{
    OutputLine(aIndentSize, "IfInUnknownProtos: %u", aMacCounters.mIfInUnknownProtos);
    OutputLine(aIndentSize, "IfInErrors: %u", aMacCounters.mIfInErrors);
    OutputLine(aIndentSize, "IfOutErrors: %u", aMacCounters.mIfOutErrors);
    OutputLine(aIndentSize, "IfInUcastPkts: %u", aMacCounters.mIfInUcastPkts);
    OutputLine(aIndentSize, "IfInBroadcastPkts: %u", aMacCounters.mIfInBroadcastPkts);
    OutputLine(aIndentSize, "IfInDiscards: %u", aMacCounters.mIfInDiscards);
    OutputLine(aIndentSize, "IfOutUcastPkts: %u", aMacCounters.mIfOutUcastPkts);
    OutputLine(aIndentSize, "IfOutBroadcastPkts: %u", aMacCounters.mIfOutBroadcastPkts);
    OutputLine(aIndentSize, "IfOutDiscards: %u", aMacCounters.mIfOutDiscards);
}

void Interpreter::OutputChildTableEntry(uint8_t aIndentSize, const otNetworkDiagChildEntry &aChildEntry)
{
    OutputLine("ChildId: 0x%04x", aChildEntry.mChildId);

    OutputLine(aIndentSize, "Timeout: %u", aChildEntry.mTimeout);
    OutputLine(aIndentSize, "Mode:");
    OutputMode(aIndentSize + kIndentSize, aChildEntry.mMode);
}
#endif // OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE

void Interpreter::HandleDiscoveryRequest(const otThreadDiscoveryRequestInfo &aInfo)
{
    OutputFormat("~ Discovery Request from ");
    OutputExtAddress(aInfo.mExtAddress);
    OutputLine(": version=%u,joiner=%d", aInfo.mVersion, aInfo.mIsJoiner);
}

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD

void Interpreter::Initialize(otInstance *aInstance, otCliOutputCallback aCallback, void *aContext)
{
    Instance *instance = static_cast<Instance *>(aInstance);

    Interpreter::sInterpreter = new (&sInterpreterRaw) Interpreter(instance, aCallback, aContext);
}

void Interpreter::OutputPrompt(void)
{
    static const char sPrompt[] = "> ";

    // The `OutputFormat()` below is adding the prompt which is not
    // part of any command output, so we set the `EmittingCommandOutput`
    // flag to false to avoid it being included in the command output
    // log (under `OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE`).

    SetEmittingCommandOutput(false);
    OutputFormat("%s", sPrompt);
    SetEmittingCommandOutput(true);
}

void Interpreter::HandleTimer(Timer &aTimer)
{
    static_cast<Interpreter *>(static_cast<TimerMilliContext &>(aTimer).GetContext())->HandleTimer();
}

void Interpreter::HandleTimer(void)
{
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    if (mLocateInProgress)
    {
        mLocateInProgress = false;
        OutputResult(OT_ERROR_RESPONSE_TIMEOUT);
    }
    else
#endif
    {
        OutputResult(OT_ERROR_NONE);
    }
}

void Interpreter::SetCommandTimeout(uint32_t aTimeoutMilli)
{
    OT_ASSERT(mCommandIsPending);
    mTimer.Start(aTimeoutMilli);
}

extern "C" void otCliInit(otInstance *aInstance, otCliOutputCallback aCallback, void *aContext)
{
    Interpreter::Initialize(aInstance, aCallback, aContext);
}

extern "C" void otCliInputLine(char *aBuf)
{
    Interpreter::GetInterpreter().ProcessLine(aBuf);
}

extern "C" void otCliSetUserCommands(const otCliCommand *aUserCommands, uint8_t aLength, void *aContext)
{
    Interpreter::GetInterpreter().SetUserCommands(aUserCommands, aLength, aContext);
}

extern "C" void otCliOutputBytes(const uint8_t *aBytes, uint8_t aLength)
{
    Interpreter::GetInterpreter().OutputBytes(aBytes, aLength);
}

extern "C" void otCliOutputFormat(const char *aFmt, ...)
{
    va_list aAp;
    va_start(aAp, aFmt);
    Interpreter::GetInterpreter().OutputFormatV(aFmt, aAp);
    va_end(aAp);
}

extern "C" void otCliAppendResult(otError aError)
{
    Interpreter::GetInterpreter().OutputResult(aError);
}

extern "C" void otCliPlatLogv(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, va_list aArgs)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);

    VerifyOrExit(Interpreter::IsInitialized());

    // CLI output is being used for logging, so we set the flag
    // `EmittingCommandOutput` to false indicate this.
    Interpreter::GetInterpreter().SetEmittingCommandOutput(false);
    Interpreter::GetInterpreter().OutputFormatV(aFormat, aArgs);
    Interpreter::GetInterpreter().OutputLine("");
    Interpreter::GetInterpreter().SetEmittingCommandOutput(true);

exit:
    return;
}

extern "C" void otCliPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);

    VerifyOrExit(Interpreter::IsInitialized());

    Interpreter::GetInterpreter().SetEmittingCommandOutput(false);
    Interpreter::GetInterpreter().OutputLine(aLogLine);
    Interpreter::GetInterpreter().SetEmittingCommandOutput(true);

exit:
    return;
}

} // namespace Cli
} // namespace ot

#if OPENTHREAD_CONFIG_LEGACY_ENABLE
OT_TOOL_WEAK void otNcpRegisterLegacyHandlers(const otNcpLegacyHandlers *aHandlers)
{
    OT_UNUSED_VARIABLE(aHandlers);
}

OT_TOOL_WEAK void otNcpHandleDidReceiveNewLegacyUlaPrefix(const uint8_t *aUlaPrefix)
{
    OT_UNUSED_VARIABLE(aUlaPrefix);
}

OT_TOOL_WEAK void otNcpHandleLegacyNodeDidJoin(const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aExtAddr);
}
#endif
