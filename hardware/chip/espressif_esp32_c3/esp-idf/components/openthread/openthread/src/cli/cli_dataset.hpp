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
 *   This file contains definitions for the CLI interpreter.
 */

#ifndef CLI_DATASET_HPP_
#define CLI_DATASET_HPP_

#include "openthread-core-config.h"

#include <stdarg.h>

#include <openthread/dataset.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {
namespace Cli {

/**
 * This class implements the Dataset CLI interpreter.
 *
 */
class Dataset : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    explicit Dataset(Output &aOutput)
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

private:
    struct Command
    {
        const char *mName;
        otError (Dataset::*mHandler)(Arg aArgs[]);
    };

    otError Print(otOperationalDataset &aDataset);

    otError ProcessHelp(Arg aArgs[]);
    otError ProcessActive(Arg aArgs[]);
    otError ProcessActiveTimestamp(Arg aArgs[]);
    otError ProcessChannel(Arg aArgs[]);
    otError ProcessChannelMask(Arg aArgs[]);
    otError ProcessClear(Arg aArgs[]);
    otError ProcessCommit(Arg aArgs[]);
    otError ProcessDelay(Arg aArgs[]);
    otError ProcessExtPanId(Arg aArgs[]);
    otError ProcessInit(Arg aArgs[]);
    otError ProcessMeshLocalPrefix(Arg aArgs[]);
    otError ProcessNetworkName(Arg aArgs[]);
    otError ProcessNetworkKey(Arg aArgs[]);
    otError ProcessPanId(Arg aArgs[]);
    otError ProcessPending(Arg aArgs[]);
    otError ProcessPendingTimestamp(Arg aArgs[]);
    otError ProcessMgmtSetCommand(Arg aArgs[]);
    otError ProcessMgmtGetCommand(Arg aArgs[]);
    otError ProcessPskc(Arg aArgs[]);
    otError ProcessSecurityPolicy(Arg aArgs[]);
    otError ProcessSet(Arg aArgs[]);

#if OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE && OPENTHREAD_FTD
    otError     ProcessUpdater(Arg aArgs[]);
    static void HandleDatasetUpdater(otError aError, void *aContext);
    void        HandleDatasetUpdater(otError aError);
#endif

    void    OutputSecurityPolicy(const otSecurityPolicy &aSecurityPolicy);
    otError ParseSecurityPolicy(otSecurityPolicy &aSecurityPolicy, Arg *&aArgs);

    static constexpr Command sCommands[] = {
        {"active", &Dataset::ProcessActive},
        {"activetimestamp", &Dataset::ProcessActiveTimestamp},
        {"channel", &Dataset::ProcessChannel},
        {"channelmask", &Dataset::ProcessChannelMask},
        {"clear", &Dataset::ProcessClear},
        {"commit", &Dataset::ProcessCommit},
        {"delay", &Dataset::ProcessDelay},
        {"extpanid", &Dataset::ProcessExtPanId},
        {"help", &Dataset::ProcessHelp},
        {"init", &Dataset::ProcessInit},
        {"meshlocalprefix", &Dataset::ProcessMeshLocalPrefix},
        {"mgmtgetcommand", &Dataset::ProcessMgmtGetCommand},
        {"mgmtsetcommand", &Dataset::ProcessMgmtSetCommand},
        {"networkkey", &Dataset::ProcessNetworkKey},
        {"networkname", &Dataset::ProcessNetworkName},
        {"panid", &Dataset::ProcessPanId},
        {"pending", &Dataset::ProcessPending},
        {"pendingtimestamp", &Dataset::ProcessPendingTimestamp},
        {"pskc", &Dataset::ProcessPskc},
        {"securitypolicy", &Dataset::ProcessSecurityPolicy},
        {"set", &Dataset::ProcessSet},
#if OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE && OPENTHREAD_FTD
        {"updater", &Dataset::ProcessUpdater},
#endif
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");

    static otOperationalDataset sDataset;
};

} // namespace Cli
} // namespace ot

#endif // CLI_DATASET_HPP_
