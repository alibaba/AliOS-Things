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
 *   This file contains definitions for a simple CLI to control the SRP server.
 */

#ifndef CLI_SRP_SERVER_HPP_
#define CLI_SRP_SERVER_HPP_

#include "openthread-core-config.h"

#include <openthread/srp_server.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE

namespace ot {
namespace Cli {

/**
 * This class implements the SRP Server CLI interpreter.
 *
 */
class SrpServer : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput  The CLI console output context.
     *
     */
    explicit SrpServer(Output &aOutput)
        : OutputWrapper(aOutput)
    {
    }

    /**
     * This method interprets a list of CLI arguments.
     *
     * @param[in]  aArgs        A pointer to an array of command line arguments.
     *
     * @retval  OT_ERROR_NONE  Successfully executed the CLI command.
     * @retval  ...            Failed to execute the CLI command.
     *
     */
    otError Process(Arg aArgs[]);

private:
    static constexpr uint8_t kIndentSize = 4;

    struct Command
    {
        const char *mName;
        otError (SrpServer::*mHandler)(Arg aArgs[]);
    };

    otError ProcessAddrMode(Arg aArgs[]);
    otError ProcessDomain(Arg aArgs[]);
    otError ProcessState(Arg aArgs[]);
    otError ProcessEnable(Arg aArgs[]);
    otError ProcessDisable(Arg aArgs[]);
    otError ProcessLease(Arg aArgs[]);
    otError ProcessHost(Arg aArgs[]);
    otError ProcessService(Arg aArgs[]);
    otError ProcessSeqNum(Arg aArgs[]);
    otError ProcessHelp(Arg aArgs[]);

    void OutputHostAddresses(const otSrpServerHost *aHost);

    static constexpr Command sCommands[] = {
        {"addrmode", &SrpServer::ProcessAddrMode}, {"disable", &SrpServer::ProcessDisable},
        {"domain", &SrpServer::ProcessDomain},     {"enable", &SrpServer::ProcessEnable},
        {"help", &SrpServer::ProcessHelp},         {"host", &SrpServer::ProcessHost},
        {"lease", &SrpServer::ProcessLease},       {"seqnum", &SrpServer::ProcessSeqNum},
        {"service", &SrpServer::ProcessService},   {"state", &SrpServer::ProcessState},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");
};

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_SRP_SERVER_ENABLE

#endif // CLI_SRP_SERVER_HPP_
