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
 *   This file contains definitions for a TCP CLI tool.
 */

#ifndef CLI_TCP_EXAMPLE_HPP_
#define CLI_TCP_EXAMPLE_HPP_

#include "openthread-core-config.h"

#include <openthread/tcp.h>

#include "cli/cli_config.h"
#include "cli/cli_output.hpp"
#include "common/time.hpp"
#include "utils/lookup_table.hpp"
#include "utils/parse_cmdline.hpp"

namespace ot {
namespace Cli {

/**
 * This class implements a CLI-based TCP example.
 *
 */
class TcpExample : private OutputWrapper
{
public:
    using Arg = Utils::CmdLineParser::Arg;

    /**
     * Constructor
     *
     * @param[in]  aOutput  The CLI console output context.
     *
     */
    explicit TcpExample(Output &aOutput);

    /**
     * This method interprets a list of CLI arguments.
     *
     * @param[in]  aArgs   An array of command line arguments.
     *
     */
    otError Process(Arg aArgs[]);

private:
    struct Command
    {
        const char *mName;
        otError (TcpExample::*mHandler)(Arg aArgs[]);
    };

    otError ProcessHelp(Arg aArgs[]);
    otError ProcessInit(Arg aArgs[]);
    otError ProcessDeinit(Arg aArgs[]);
    otError ProcessBind(Arg aArgs[]);
    otError ProcessConnect(Arg aArgs[]);
    otError ProcessSend(Arg aArgs[]);
    otError ProcessBenchmark(Arg aArgs[]);
    otError ProcessSendEnd(Arg aArgs[]);
    otError ProcessAbort(Arg aArgs[]);
    otError ProcessListen(Arg aArgs[]);
    otError ProcessStopListening(Arg aArgs[]);

    static void HandleTcpEstablishedCallback(otTcpEndpoint *aEndpoint);
    static void HandleTcpSendDoneCallback(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);
    static void HandleTcpReceiveAvailableCallback(otTcpEndpoint *aEndpoint,
                                                  size_t         aBytesAvailable,
                                                  bool           aEndOfStream,
                                                  size_t         aBytesRemaining);
    static void HandleTcpDisconnectedCallback(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);
    static otTcpIncomingConnectionAction HandleTcpAcceptReadyCallback(otTcpListener *   aListener,
                                                                      const otSockAddr *aPeer,
                                                                      otTcpEndpoint **  aAcceptInto);
    static void                          HandleTcpAcceptDoneCallback(otTcpListener *   aListener,
                                                                     otTcpEndpoint *   aEndpoint,
                                                                     const otSockAddr *aPeer);

    void                          HandleTcpEstablished(otTcpEndpoint *aEndpoint);
    void                          HandleTcpSendDone(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);
    void                          HandleTcpReceiveAvailable(otTcpEndpoint *aEndpoint,
                                                            size_t         aBytesAvailable,
                                                            bool           aEndOfStream,
                                                            size_t         aBytesRemaining);
    void                          HandleTcpDisconnected(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);
    otTcpIncomingConnectionAction HandleTcpAcceptReady(otTcpListener *   aListener,
                                                       const otSockAddr *aPeer,
                                                       otTcpEndpoint **  aAcceptInto);
    void HandleTcpAcceptDone(otTcpListener *aListener, otTcpEndpoint *aEndpoint, const otSockAddr *aPeer);

    static constexpr Command sCommands[] = {
        {"abort", &TcpExample::ProcessAbort},
        {"benchmark", &TcpExample::ProcessBenchmark},
        {"bind", &TcpExample::ProcessBind},
        {"connect", &TcpExample::ProcessConnect},
        {"deinit", &TcpExample::ProcessDeinit},
        {"help", &TcpExample::ProcessHelp},
        {"init", &TcpExample::ProcessInit},
        {"listen", &TcpExample::ProcessListen},
        {"send", &TcpExample::ProcessSend},
        {"sendend", &TcpExample::ProcessSendEnd},
        {"stoplistening", &TcpExample::ProcessStopListening},
    };

    static_assert(Utils::LookupTable::IsSorted(sCommands), "Command Table is not sorted");

    otTcpEndpoint mEndpoint;
    otTcpListener mListener;

    bool mInitialized;
    bool mEndpointConnected;
    bool mSendBusy;

    otLinkedBuffer mSendLink;
    uint8_t        mSendBuffer[OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH];
    uint8_t        mReceiveBuffer[OPENTHREAD_CONFIG_CLI_TCP_RECEIVE_BUFFER_SIZE];

    otLinkedBuffer mBenchmarkLinks[(sizeof(mReceiveBuffer) + sizeof(mSendBuffer) - 1) / sizeof(mSendBuffer)];
    uint32_t       mBenchmarkBytesTotal;
    uint32_t       mBenchmarkLinksLeft;
    TimeMilli      mBenchmarkStart;
};

} // namespace Cli
} // namespace ot

#endif // CLI_TCP_EXAMPLE_HPP_
