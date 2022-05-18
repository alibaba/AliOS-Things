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
 *   This file implements a TCP CLI tool.
 */

#include "openthread-core-config.h"

#include "cli_config.h"

#if OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE

#include "cli_tcp.hpp"

#include <openthread/tcp.h>

#include "cli/cli.hpp"
#include "common/encoding.hpp"
#include "common/timer.hpp"

namespace ot {
namespace Cli {

constexpr TcpExample::Command TcpExample::sCommands[];

TcpExample::TcpExample(Output &aOutput)
    : OutputWrapper(aOutput)
    , mInitialized(false)
    , mEndpointConnected(false)
    , mSendBusy(false)
    , mBenchmarkBytesTotal(0)
    , mBenchmarkLinksLeft(0)
{
}

otError TcpExample::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError TcpExample::ProcessInit(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    size_t  receiveBufferSize;

    VerifyOrExit(!mInitialized, error = OT_ERROR_ALREADY);

    if (aArgs[0].IsEmpty())
    {
        receiveBufferSize = sizeof(mReceiveBuffer);
    }
    else
    {
        uint32_t windowSize;

        SuccessOrExit(error = aArgs[0].ParseAsUint32(windowSize));
        VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

        receiveBufferSize = windowSize + ((windowSize + 7) >> 3);
        VerifyOrExit(receiveBufferSize <= sizeof(mReceiveBuffer) && receiveBufferSize != 0,
                     error = OT_ERROR_INVALID_ARGS);
    }

    {
        otTcpEndpointInitializeArgs endpointArgs;

        memset(&endpointArgs, 0x00, sizeof(endpointArgs));
        endpointArgs.mEstablishedCallback      = HandleTcpEstablishedCallback;
        endpointArgs.mSendDoneCallback         = HandleTcpSendDoneCallback;
        endpointArgs.mReceiveAvailableCallback = HandleTcpReceiveAvailableCallback;
        endpointArgs.mDisconnectedCallback     = HandleTcpDisconnectedCallback;
        endpointArgs.mContext                  = this;
        endpointArgs.mReceiveBuffer            = mReceiveBuffer;
        endpointArgs.mReceiveBufferSize        = receiveBufferSize;

        SuccessOrExit(error = otTcpEndpointInitialize(GetInstancePtr(), &mEndpoint, &endpointArgs));
    }

    {
        otTcpListenerInitializeArgs listenerArgs;

        memset(&listenerArgs, 0x00, sizeof(listenerArgs));
        listenerArgs.mAcceptReadyCallback = HandleTcpAcceptReadyCallback;
        listenerArgs.mAcceptDoneCallback  = HandleTcpAcceptDoneCallback;
        listenerArgs.mContext             = this;

        error = otTcpListenerInitialize(GetInstancePtr(), &mListener, &listenerArgs);
        if (error != OT_ERROR_NONE)
        {
            IgnoreReturnValue(otTcpEndpointDeinitialize(&mEndpoint));
            ExitNow();
        }
    }

    mInitialized = true;

exit:
    return error;
}

otError TcpExample::ProcessDeinit(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;
    otError endpointError;
    otError listenerError;

    VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    endpointError = otTcpEndpointDeinitialize(&mEndpoint);
    mSendBusy     = false;

    listenerError = otTcpListenerDeinitialize(&mListener);
    mInitialized  = false;

    SuccessOrExit(error = endpointError);
    SuccessOrExit(error = listenerError);

exit:
    return error;
}

otError TcpExample::ProcessBind(Arg aArgs[])
{
    otError    error;
    otSockAddr sockaddr;

    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(sockaddr.mAddress));
    SuccessOrExit(error = aArgs[1].ParseAsUint16(sockaddr.mPort));
    VerifyOrExit(aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    error = otTcpBind(&mEndpoint, &sockaddr);

exit:
    return error;
}

otError TcpExample::ProcessConnect(Arg aArgs[])
{
    otError    error;
    otSockAddr sockaddr;

    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(sockaddr.mAddress));
    SuccessOrExit(error = aArgs[1].ParseAsUint16(sockaddr.mPort));
    VerifyOrExit(aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = otTcpConnect(&mEndpoint, &sockaddr, OT_TCP_CONNECT_NO_FAST_OPEN));
    mEndpointConnected = false;

exit:
    return error;
}

otError TcpExample::ProcessSend(Arg aArgs[])
{
    otError error;

    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(!mSendBusy, error = OT_ERROR_BUSY);
    VerifyOrExit(mBenchmarkBytesTotal == 0, error = OT_ERROR_BUSY);

    mSendLink.mNext = nullptr;
    mSendLink.mData = mSendBuffer;
    VerifyOrExit(!aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    mSendLink.mLength = OT_MIN(aArgs[0].GetLength(), sizeof(mSendBuffer));
    memcpy(mSendBuffer, aArgs[0].GetCString(), mSendLink.mLength);
    VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = otTcpSendByReference(&mEndpoint, &mSendLink, 0));
    mSendBusy = true;

exit:
    return error;
}

otError TcpExample::ProcessBenchmark(Arg aArgs[])
{
    otError  error = OT_ERROR_NONE;
    uint32_t toSendOut;

    VerifyOrExit(!mSendBusy, error = OT_ERROR_BUSY);
    VerifyOrExit(mBenchmarkBytesTotal == 0, error = OT_ERROR_BUSY);

    if (aArgs[0].IsEmpty())
    {
        mBenchmarkBytesTotal = OPENTHREAD_CONFIG_CLI_TCP_DEFAULT_BENCHMARK_SIZE;
    }
    else
    {
        SuccessOrExit(error = aArgs[0].ParseAsUint32(mBenchmarkBytesTotal));
        VerifyOrExit(mBenchmarkBytesTotal != 0, error = OT_ERROR_INVALID_ARGS);
    }
    VerifyOrExit(aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    memset(mSendBuffer, 'a', sizeof(mSendBuffer));

    mBenchmarkLinksLeft = (mBenchmarkBytesTotal + sizeof(mSendBuffer) - 1) / sizeof(mSendBuffer);
    toSendOut           = OT_MIN(OT_ARRAY_LENGTH(mBenchmarkLinks), mBenchmarkLinksLeft);
    mBenchmarkStart     = TimerMilli::GetNow();
    for (uint32_t i = 0; i != toSendOut; i++)
    {
        mBenchmarkLinks[i].mNext   = nullptr;
        mBenchmarkLinks[i].mData   = mSendBuffer;
        mBenchmarkLinks[i].mLength = sizeof(mSendBuffer);
        if (i == 0 && mBenchmarkBytesTotal % sizeof(mSendBuffer) != 0)
        {
            mBenchmarkLinks[i].mLength = mBenchmarkBytesTotal % sizeof(mSendBuffer);
        }
        SuccessOrExit(error = otTcpSendByReference(&mEndpoint, &mBenchmarkLinks[i],
                                                   i == toSendOut - 1 ? 0 : OT_TCP_SEND_MORE_TO_COME));
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        mBenchmarkBytesTotal = 0;
        mBenchmarkLinksLeft  = 0;
    }
    return error;
}

otError TcpExample::ProcessSendEnd(Arg aArgs[])
{
    otError error;

    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    error = otTcpSendEndOfStream(&mEndpoint);

exit:
    return error;
}

otError TcpExample::ProcessAbort(Arg aArgs[])
{
    otError error;

    VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = otTcpAbort(&mEndpoint));
    mEndpointConnected = false;

exit:
    return error;
}

otError TcpExample::ProcessListen(Arg aArgs[])
{
    otError    error;
    otSockAddr sockaddr;

    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(sockaddr.mAddress));
    SuccessOrExit(error = aArgs[1].ParseAsUint16(sockaddr.mPort));
    VerifyOrExit(aArgs[2].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    SuccessOrExit(error = otTcpStopListening(&mListener));
    error = otTcpListen(&mListener, &sockaddr);

exit:
    return error;
}

otError TcpExample::ProcessStopListening(Arg aArgs[])
{
    otError error;

    VerifyOrExit(aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mInitialized, error = OT_ERROR_INVALID_STATE);

    error = otTcpStopListening(&mListener);

exit:
    return error;
}

otError TcpExample::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_ARGS;
    const Command *command;

    VerifyOrExit(!aArgs[0].IsEmpty(), IgnoreError(ProcessHelp(nullptr)));

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr, error = OT_ERROR_INVALID_COMMAND);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

void TcpExample::HandleTcpEstablishedCallback(otTcpEndpoint *aEndpoint)
{
    static_cast<TcpExample *>(otTcpEndpointGetContext(aEndpoint))->HandleTcpEstablished(aEndpoint);
}

void TcpExample::HandleTcpSendDoneCallback(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData)
{
    static_cast<TcpExample *>(otTcpEndpointGetContext(aEndpoint))->HandleTcpSendDone(aEndpoint, aData);
}

void TcpExample::HandleTcpReceiveAvailableCallback(otTcpEndpoint *aEndpoint,
                                                   size_t         aBytesAvailable,
                                                   bool           aEndOfStream,
                                                   size_t         aBytesRemaining)
{
    static_cast<TcpExample *>(otTcpEndpointGetContext(aEndpoint))
        ->HandleTcpReceiveAvailable(aEndpoint, aBytesAvailable, aEndOfStream, aBytesRemaining);
}

void TcpExample::HandleTcpDisconnectedCallback(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason)
{
    static_cast<TcpExample *>(otTcpEndpointGetContext(aEndpoint))->HandleTcpDisconnected(aEndpoint, aReason);
}

otTcpIncomingConnectionAction TcpExample::HandleTcpAcceptReadyCallback(otTcpListener *   aListener,
                                                                       const otSockAddr *aPeer,
                                                                       otTcpEndpoint **  aAcceptInto)
{
    return static_cast<TcpExample *>(otTcpListenerGetContext(aListener))
        ->HandleTcpAcceptReady(aListener, aPeer, aAcceptInto);
}

void TcpExample::HandleTcpAcceptDoneCallback(otTcpListener *   aListener,
                                             otTcpEndpoint *   aEndpoint,
                                             const otSockAddr *aPeer)
{
    static_cast<TcpExample *>(otTcpListenerGetContext(aListener))->HandleTcpAcceptDone(aListener, aEndpoint, aPeer);
}

void TcpExample::HandleTcpEstablished(otTcpEndpoint *aEndpoint)
{
    OT_UNUSED_VARIABLE(aEndpoint);
    OutputLine("TCP: Connection established");
}

void TcpExample::HandleTcpSendDone(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData)
{
    OT_UNUSED_VARIABLE(aEndpoint);

    if (mBenchmarkBytesTotal == 0)
    {
        // If the benchmark encountered an error, we might end up here. So,
        // tolerate some benchmark links finishing in this case.
        if (aData == &mSendLink)
        {
            OT_ASSERT(mSendBusy);
            mSendBusy = false;
        }
    }
    else
    {
        OT_ASSERT(aData != &mSendLink);
        mBenchmarkLinksLeft--;
        if (mBenchmarkLinksLeft >= OT_ARRAY_LENGTH(mBenchmarkLinks))
        {
            aData->mLength = sizeof(mSendBuffer);
            if (otTcpSendByReference(&mEndpoint, aData, 0) != OT_ERROR_NONE)
            {
                OutputLine("TCP Benchmark Failed");
                mBenchmarkBytesTotal = 0;
            }
        }
        else if (mBenchmarkLinksLeft == 0)
        {
            uint32_t milliseconds         = TimerMilli::GetNow() - mBenchmarkStart;
            uint32_t thousandTimesGoodput = (1000 * (mBenchmarkBytesTotal << 3) + (milliseconds >> 1)) / milliseconds;

            OutputLine("TCP Benchmark Complete: Transferred %u bytes in %u milliseconds",
                       static_cast<unsigned int>(mBenchmarkBytesTotal), static_cast<unsigned int>(milliseconds));
            OutputLine("TCP Goodput: %u.%03u kb/s", thousandTimesGoodput / 1000, thousandTimesGoodput % 1000);
            mBenchmarkBytesTotal = 0;
        }
    }
}

void TcpExample::HandleTcpReceiveAvailable(otTcpEndpoint *aEndpoint,
                                           size_t         aBytesAvailable,
                                           bool           aEndOfStream,
                                           size_t         aBytesRemaining)
{
    OT_UNUSED_VARIABLE(aBytesRemaining);
    OT_ASSERT(aEndpoint == &mEndpoint);

    if (aBytesAvailable > 0)
    {
        const otLinkedBuffer *data;
        size_t                totalReceived = 0;

        IgnoreError(otTcpReceiveByReference(aEndpoint, &data));
        for (; data != nullptr; data = data->mNext)
        {
            OutputLine("TCP: Received %u bytes: %.*s", static_cast<unsigned int>(data->mLength), data->mLength,
                       reinterpret_cast<const char *>(data->mData));
            totalReceived += data->mLength;
        }
        OT_ASSERT(aBytesAvailable == totalReceived);
        IgnoreReturnValue(otTcpCommitReceive(aEndpoint, totalReceived, 0));
    }

    if (aEndOfStream)
    {
        OutputLine("TCP: Reached end of stream");
    }
}

void TcpExample::HandleTcpDisconnected(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason)
{
    OT_UNUSED_VARIABLE(aEndpoint);

    switch (aReason)
    {
    case OT_TCP_DISCONNECTED_REASON_NORMAL:
        OutputLine("TCP: Disconnected");
        break;
    case OT_TCP_DISCONNECTED_REASON_TIME_WAIT:
        OutputLine("TCP: Entered TIME-WAIT state");
        break;
    case OT_TCP_DISCONNECTED_REASON_TIMED_OUT:
        OutputLine("TCP: Connection timed out");
        break;
    case OT_TCP_DISCONNECTED_REASON_REFUSED:
        OutputLine("TCP: Connection refused");
        break;
    case OT_TCP_DISCONNECTED_REASON_RESET:
        OutputLine("TCP: Connection reset");
        break;
    }

    // We set this to false even for the TIME-WAIT state, so that we can reuse
    // the active socket if an incoming connection comes in instead of waiting
    // for the 2MSL timeout.
    mEndpointConnected = false;
    mSendBusy          = false;

    // Mark the benchmark as inactive if the connction was disconnected.
    if (mBenchmarkBytesTotal != 0)
    {
        mBenchmarkBytesTotal = 0;
        mBenchmarkLinksLeft  = 0;
    }
}

otTcpIncomingConnectionAction TcpExample::HandleTcpAcceptReady(otTcpListener *   aListener,
                                                               const otSockAddr *aPeer,
                                                               otTcpEndpoint **  aAcceptInto)
{
    OT_UNUSED_VARIABLE(aListener);

    if (mEndpointConnected)
    {
        OutputFormat("TCP: Ignoring incoming connection request from [");
        OutputIp6Address(aPeer->mAddress);
        OutputLine("]:%u (active socket is busy)", static_cast<unsigned int>(aPeer->mPort));

        return OT_TCP_INCOMING_CONNECTION_ACTION_DEFER;
    }

    *aAcceptInto = &mEndpoint;
    return OT_TCP_INCOMING_CONNECTION_ACTION_ACCEPT;
}

void TcpExample::HandleTcpAcceptDone(otTcpListener *aListener, otTcpEndpoint *aEndpoint, const otSockAddr *aPeer)
{
    OT_UNUSED_VARIABLE(aListener);
    OT_UNUSED_VARIABLE(aEndpoint);

    OutputFormat("Accepted connection from [");
    OutputIp6Address(aPeer->mAddress);
    OutputLine("]:%u", static_cast<unsigned int>(aPeer->mPort));
}

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_CLI_TCP_ENABLE
