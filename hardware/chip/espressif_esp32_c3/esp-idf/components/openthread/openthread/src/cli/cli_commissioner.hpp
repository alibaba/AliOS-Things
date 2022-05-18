/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file contains definitions for a simple CLI to control the Commissioner role.
 */

#ifndef CLI_COMMISSIONER_HPP_
#define CLI_COMMISSIONER_HPP_

#include "openthread-core-config.h"

#include <openthread/commissioner.h>

#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

namespace ot {
namespace Cli {

/**
 * This class implements the Commissioner CLI interpreter.
 *
 */
class Commissioner : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput The CLI console output context
     *
     */
    explicit Commissioner(Output &aOutput)
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
    enum
    {
        kDefaultJoinerTimeout = 120, ///< Default timeout for Joiners, in seconds.
    };

    struct Command
    {
        const char *mName;
        otError (Commissioner::*mHandler)(Arg aArgs[]);
    };

    otError ProcessHelp(Arg aArgs[]);
    otError ProcessAnnounce(Arg aArgs[]);
    otError ProcessEnergy(Arg aArgs[]);
    otError ProcessJoiner(Arg aArgs[]);
    otError ProcessMgmtGet(Arg aArgs[]);
    otError ProcessMgmtSet(Arg aArgs[]);
    otError ProcessPanId(Arg aArgs[]);
    otError ProcessProvisioningUrl(Arg aArgs[]);
    otError ProcessSessionId(Arg aArgs[]);
    otError ProcessStart(Arg aArgs[]);
    otError ProcessState(Arg aArgs[]);
    otError ProcessStop(Arg aArgs[]);

    static void HandleStateChanged(otCommissionerState aState, void *aContext);
    void        HandleStateChanged(otCommissionerState aState);

    static void HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                                  const otJoinerInfo *      aJoinerInfo,
                                  const otExtAddress *      aJoinerId,
                                  void *                    aContext);
    void        HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                                  const otJoinerInfo *      aJoinerInfo,
                                  const otExtAddress *      aJoinerId);

    static void HandleEnergyReport(uint32_t       aChannelMask,
                                   const uint8_t *aEnergyList,
                                   uint8_t        aEnergyListLength,
                                   void *         aContext);
    void        HandleEnergyReport(uint32_t aChannelMask, const uint8_t *aEnergyList, uint8_t aEnergyListLength);

    static void HandlePanIdConflict(uint16_t aPanId, uint32_t aChannelMask, void *aContext);
    void        HandlePanIdConflict(uint16_t aPanId, uint32_t aChannelMask);

    static const char *StateToString(otCommissionerState aState);

    static constexpr Command sCommands[] = {
        {"announce", &Commissioner::ProcessAnnounce},   {"energy", &Commissioner::ProcessEnergy},
        {"help", &Commissioner::ProcessHelp},           {"joiner", &Commissioner::ProcessJoiner},
        {"mgmtget", &Commissioner::ProcessMgmtGet},     {"mgmtset", &Commissioner::ProcessMgmtSet},
        {"panid", &Commissioner::ProcessPanId},         {"provisioningurl", &Commissioner::ProcessProvisioningUrl},
        {"sessionid", &Commissioner::ProcessSessionId}, {"start", &Commissioner::ProcessStart},
        {"state", &Commissioner::ProcessState},         {"stop", &Commissioner::ProcessStop},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");
};

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

#endif // CLI_COMMISSIONER_HPP_
