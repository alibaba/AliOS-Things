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
 *   This file contains definitions for CLI to control History Tracker
 */

#ifndef CLI_HISTORY_HPP_
#define CLI_HISTORY_HPP_

#include "openthread-core-config.h"

#include <openthread/history_tracker.h>

#include "cli/cli_config.h"
#include "cli/cli_output.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

namespace ot {
namespace Cli {

/**
 * This class implements the History Tracker CLI interpreter.
 *
 */
class History : private OutputWrapper
{
public:
    typedef Utils::CmdLineParser::Arg Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput The CLI console output context
     *
     */
    explicit History(Output &aOutput)
        : OutputWrapper(aOutput)
    {
    }

    /**
     * This method interprets a list of CLI arguments.
     *
     * @param[in]  aArgs        A pointer an array of command line arguments.
     *
     */
    otError Process(Arg aArgs[]);

private:
    static constexpr uint16_t kShortAddrInvalid   = 0xfffe;
    static constexpr uint16_t kShortAddrBroadcast = 0xffff;
    static constexpr int8_t   kInvalidRss         = OT_RADIO_RSSI_INVALID;

    struct Command
    {
        const char *mName;
        otError (History::*mHandler)(Arg aArgs[]);
    };

    enum RxTx : uint8_t
    {
        kRx,
        kTx,
        kRxTx,
    };

    otError ProcessHelp(Arg aArgs[]);
    otError ProcessIpAddr(Arg aArgs[]);
    otError ProcessIpMulticastAddr(Arg aArgs[]);
    otError ProcessNetInfo(Arg aArgs[]);
    otError ProcessNeighbor(Arg aArgs[]);
    otError ProcessRx(Arg aArgs[]);
    otError ProcessRxTx(Arg aArgs[]);
    otError ProcessTx(Arg aArgs[]);

    otError ParseArgs(Arg aArgs[], bool &aIsList, uint16_t &aNumEntries) const;
    otError ProcessRxTxHistory(RxTx aRxTx, Arg aArgs[]);
    void    OutputRxTxEntryListFormat(const otHistoryTrackerMessageInfo &aInfo, uint32_t aEntryAge, bool aIsRx);
    void    OutputRxTxEntryTableFormat(const otHistoryTrackerMessageInfo &aInfo, uint32_t aEntryAge, bool aIsRx);

    static const char *AddressOriginToString(uint8_t aOrigin);
    static const char *MessagePriorityToString(uint8_t aPriority);
    static const char *RadioTypeToString(const otHistoryTrackerMessageInfo &aInfo);
    static const char *MessageTypeToString(const otHistoryTrackerMessageInfo &aInfo);

    static constexpr Command sCommands[] = {
        {"help", &History::ProcessHelp},
        {"ipaddr", &History::ProcessIpAddr},
        {"ipmaddr", &History::ProcessIpMulticastAddr},
        {"neighbor", &History::ProcessNeighbor},
        {"netinfo", &History::ProcessNetInfo},
        {"rx", &History::ProcessRx},
        {"rxtx", &History::ProcessRxTx},
        {"tx", &History::ProcessTx},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");
};

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

#endif // CLI_HISTORY_HPP_
