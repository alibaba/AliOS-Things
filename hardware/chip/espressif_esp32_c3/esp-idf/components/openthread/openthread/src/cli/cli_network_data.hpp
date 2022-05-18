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
 *   This file contains definitions for Network Data CLI commands.
 */

#ifndef CLI_NETWORK_DATA_HPP_
#define CLI_NETWORK_DATA_HPP_

#include "openthread-core-config.h"

#include <openthread/netdata.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {
namespace Cli {

/**
 * This class implements the Network Data CLI.
 *
 */
class NetworkData : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput The CLI console output context
     *
     */
    explicit NetworkData(Output &aOutput)
        : OutputWrapper(aOutput)
    {
    }

    /**
     * This method interprets a list of CLI arguments.
     *
     * @param[in]  aArgs        An array of command line arguments.
     *
     */
    otError Process(Arg aArgs[]);

    /**
     * This method outputs the prefix config.
     *
     * @param[in]  aConfig  The prefix config.
     *
     */
    void OutputPrefix(const otBorderRouterConfig &aConfig);

    /**
     * This method outputs the route config.
     *
     * @param[in]  aConfig  The route config.
     *
     */
    void OutputRoute(const otExternalRouteConfig &aConfig);

    /**
     * This method outputs the service config.
     *
     * @param[in]  aConfig  The service config.
     *
     */
    void OutputService(const otServiceConfig &aConfig);

private:
    struct Command
    {
        const char *mName;
        otError (NetworkData::*mHandler)(Arg aArgs[]);
    };

    otError ProcessHelp(Arg aArgs[]);
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
    otError ProcessPublish(Arg aArgs[]);
    otError ProcessUnpublish(Arg aArgs[]);
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    otError ProcessRegister(Arg aArgs[]);
#endif
    otError ProcessShow(Arg aArgs[]);
    otError ProcessSteeringData(Arg aArgs[]);

    otError OutputBinary(void);
    void    OutputPrefixes(void);
    void    OutputRoutes(void);
    void    OutputServices(void);
    void    OutputPreference(signed int aPreference);

    static constexpr Command sCommands[] = {
        {"help", &NetworkData::ProcessHelp},
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
        {"publish", &NetworkData::ProcessPublish},
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
        {"register", &NetworkData::ProcessRegister},
#endif
        {"show", &NetworkData::ProcessShow},
        {"steeringdata", &NetworkData::ProcessSteeringData},
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
        {"unpublish", &NetworkData::ProcessUnpublish},
#endif
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");
};

} // namespace Cli
} // namespace ot

#endif // CLI_NETWORK_DATA_HPP_
