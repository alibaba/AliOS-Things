/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file implements CLI commands for Network Data.
 */

#include "cli_network_data.hpp"

#include <openthread/border_router.h>
#include <openthread/netdata_publisher.h>
#include <openthread/server.h>

#include "cli/cli.hpp"
#include "common/encoding.hpp"

namespace ot {
namespace Cli {

constexpr NetworkData::Command NetworkData::sCommands[];

void NetworkData::OutputPrefix(const otBorderRouterConfig &aConfig)
{
    enum
    {
        // BorderRouter flag is `uint16_t` (though some of the bits are
        // reserved for future use), so we use 17 chars string (16 flags
        // plus null char at end of string).
        kMaxFlagStringSize = 17,
    };

    char  flagsString[kMaxFlagStringSize];
    char *flagsPtr = &flagsString[0];

    OutputIp6Prefix(aConfig.mPrefix);

    if (aConfig.mPreferred)
    {
        *flagsPtr++ = 'p';
    }

    if (aConfig.mSlaac)
    {
        *flagsPtr++ = 'a';
    }

    if (aConfig.mDhcp)
    {
        *flagsPtr++ = 'd';
    }

    if (aConfig.mConfigure)
    {
        *flagsPtr++ = 'c';
    }

    if (aConfig.mDefaultRoute)
    {
        *flagsPtr++ = 'r';
    }

    if (aConfig.mOnMesh)
    {
        *flagsPtr++ = 'o';
    }

    if (aConfig.mStable)
    {
        *flagsPtr++ = 's';
    }

    if (aConfig.mNdDns)
    {
        *flagsPtr++ = 'n';
    }

    if (aConfig.mDp)
    {
        *flagsPtr++ = 'D';
    }

    *flagsPtr = '\0';

    if (flagsPtr != &flagsString[0])
    {
        OutputFormat(" %s", flagsString);
    }

    OutputPreference(aConfig.mPreference);

    OutputLine(" %04x", aConfig.mRloc16);
}

void NetworkData::OutputRoute(const otExternalRouteConfig &aConfig)
{
    enum
    {
        // ExternalRoute flag is `uint8_t` (though some of the bits are
        // reserved for future use), so we use 9 chars string (8 flags
        // plus null char at end of string).
        kMaxFlagStringSize = 9,
    };

    char  flagsString[kMaxFlagStringSize];
    char *flagsPtr = &flagsString[0];

    OutputIp6Prefix(aConfig.mPrefix);

    if (aConfig.mStable)
    {
        *flagsPtr++ = 's';
    }

    if (aConfig.mNat64)
    {
        *flagsPtr++ = 'n';
    }

    *flagsPtr = '\0';

    if (flagsPtr != &flagsString[0])
    {
        OutputFormat(" %s", flagsString);
    }

    OutputPreference(aConfig.mPreference);

    OutputLine(" %04x", aConfig.mRloc16);
}

void NetworkData::OutputPreference(signed int aPreference)
{
    switch (aPreference)
    {
    case OT_ROUTE_PREFERENCE_LOW:
        OutputFormat(" low");
        break;

    case OT_ROUTE_PREFERENCE_MED:
        OutputFormat(" med");
        break;

    case OT_ROUTE_PREFERENCE_HIGH:
        OutputFormat(" high");
        break;

    default:
        break;
    }
}

void NetworkData::OutputService(const otServiceConfig &aConfig)
{
    OutputFormat("%u ", aConfig.mEnterpriseNumber);
    OutputBytes(aConfig.mServiceData, aConfig.mServiceDataLength);
    OutputFormat(" ");
    OutputBytes(aConfig.mServerConfig.mServerData, aConfig.mServerConfig.mServerDataLength);

    if (aConfig.mServerConfig.mStable)
    {
        OutputFormat(" s");
    }

    OutputLine(" %04x", aConfig.mServerConfig.mRloc16);
}

otError NetworkData::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
otError NetworkData::ProcessPublish(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (aArgs[0] == "dnssrp")
    {
        if (aArgs[1] == "anycast")
        {
            uint8_t sequenceNumber;

            SuccessOrExit(error = aArgs[2].ParseAsUint8(sequenceNumber));
            otNetDataPublishDnsSrpServiceAnycast(GetInstancePtr(), sequenceNumber);
            ExitNow();
        }

        if (aArgs[1] == "unicast")
        {
            otIp6Address address;
            uint16_t     port;

            if (aArgs[3].IsEmpty())
            {
                SuccessOrExit(error = aArgs[2].ParseAsUint16(port));
                otNetDataPublishDnsSrpServiceUnicastMeshLocalEid(GetInstancePtr(), port);
                ExitNow();
            }

            SuccessOrExit(error = aArgs[2].ParseAsIp6Address(address));
            SuccessOrExit(error = aArgs[3].ParseAsUint16(port));
            otNetDataPublishDnsSrpServiceUnicast(GetInstancePtr(), &address, port);
            ExitNow();
        }
    }
#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    if (aArgs[0] == "prefix")
    {
        otBorderRouterConfig config;

        SuccessOrExit(error = Interpreter::ParsePrefix(aArgs + 1, config));
        error = otNetDataPublishOnMeshPrefix(GetInstancePtr(), &config);
        ExitNow();
    }

    if (aArgs[0] == "route")
    {
        otExternalRouteConfig config;

        SuccessOrExit(error = Interpreter::ParseRoute(aArgs + 1, config));
        error = otNetDataPublishExternalRoute(GetInstancePtr(), &config);
        ExitNow();
    }
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

    error = OT_ERROR_INVALID_ARGS;

exit:
    return error;
}

otError NetworkData::ProcessUnpublish(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (aArgs[0] == "dnssrp")
    {
        otNetDataUnpublishDnsSrpService(GetInstancePtr());
        ExitNow();
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    {
        otIp6Prefix prefix;

        if (aArgs[0].ParseAsIp6Prefix(prefix) == OT_ERROR_NONE)
        {
            error = otNetDataUnpublishPrefix(GetInstancePtr(), &prefix);
            ExitNow();
        }
    }
#endif

    error = OT_ERROR_INVALID_ARGS;

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
otError NetworkData::ProcessRegister(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otError error = OT_ERROR_NONE;

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    SuccessOrExit(error = otBorderRouterRegister(GetInstancePtr()));
#else
    SuccessOrExit(error = otServerRegister(GetInstancePtr()));
#endif

exit:
    return error;
}
#endif

otError NetworkData::ProcessSteeringData(Arg aArgs[])
{
    otError           error;
    otExtAddress      addr;
    otJoinerDiscerner discerner;

    VerifyOrExit(aArgs[0] == "check", error = OT_ERROR_INVALID_ARGS);

    error = Interpreter::ParseJoinerDiscerner(aArgs[1], discerner);

    if (error == OT_ERROR_NOT_FOUND)
    {
        discerner.mLength = 0;
        error             = aArgs[1].ParseAsHexString(addr.m8);
    }

    SuccessOrExit(error);

    if (discerner.mLength)
    {
        error = otNetDataSteeringDataCheckJoinerWithDiscerner(GetInstancePtr(), &discerner);
    }
    else
    {
        error = otNetDataSteeringDataCheckJoiner(GetInstancePtr(), &addr);
    }

exit:
    return error;
}

void NetworkData::OutputPrefixes(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig  config;

    OutputLine("Prefixes:");

    while (otNetDataGetNextOnMeshPrefix(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        OutputPrefix(config);
    }
}

void NetworkData::OutputRoutes(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig config;

    OutputLine("Routes:");

    while (otNetDataGetNextRoute(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        OutputRoute(config);
    }
}

void NetworkData::OutputServices(void)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otServiceConfig       config;

    OutputLine("Services:");

    while (otNetDataGetNextService(GetInstancePtr(), &iterator, &config) == OT_ERROR_NONE)
    {
        OutputService(config);
    }
}

otError NetworkData::OutputBinary(void)
{
    otError error = OT_ERROR_NONE;
    uint8_t data[255];
    uint8_t len = sizeof(data);

    SuccessOrExit(error = otNetDataGet(GetInstancePtr(), false, data, &len));

    OutputBytesLine(data, static_cast<uint8_t>(len));

exit:
    return error;
}

otError NetworkData::ProcessShow(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;

    if (aArgs[0].IsEmpty())
    {
        OutputPrefixes();
        OutputRoutes();
        OutputServices();
        error = OT_ERROR_NONE;
    }
    else if (aArgs[0] == "-x")
    {
        error = OutputBinary();
    }

    return error;
}

otError NetworkData::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_COMMAND;
    const Command *command;

    if (aArgs[0].IsEmpty())
    {
        IgnoreError(ProcessHelp(aArgs));
        ExitNow();
    }

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

} // namespace Cli
} // namespace ot
