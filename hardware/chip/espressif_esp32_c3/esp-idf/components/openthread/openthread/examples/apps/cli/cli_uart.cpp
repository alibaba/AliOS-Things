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
#include "openthread-core-config.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <openthread-system.h>
#include <openthread/cli.h>

#include "cli/cli_config.h"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/logging.hpp"
#include "utils/uart.h"

#if OPENTHREAD_POSIX
#include <signal.h>
#include <sys/types.h>
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE
 *
 * The size of CLI UART RX buffer in bytes.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE 640
#else
#define OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE 512
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_TX_BUFFER_SIZE
 *
 * The size of CLI message buffer in bytes.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_UART_TX_BUFFER_SIZE
#define OPENTHREAD_CONFIG_CLI_UART_TX_BUFFER_SIZE 1024
#endif

#if OPENTHREAD_CONFIG_DIAG_ENABLE
#if OPENTHREAD_CONFIG_DIAG_OUTPUT_BUFFER_SIZE > OPENTHREAD_CONFIG_CLI_UART_TX_BUFFER_SIZE
#error "diag output buffer should be smaller than CLI UART tx buffer"
#endif
#if OPENTHREAD_CONFIG_DIAG_CMD_LINE_BUFFER_SIZE > OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE
#error "diag command line should be smaller than CLI UART rx buffer"
#endif
#endif

#if OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH > OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE
#error "command line should be should be smaller than CLI rx buffer"
#endif

enum
{
    kRxBufferSize = OPENTHREAD_CONFIG_CLI_UART_RX_BUFFER_SIZE,
    kTxBufferSize = OPENTHREAD_CONFIG_CLI_UART_TX_BUFFER_SIZE,
};

char     sRxBuffer[kRxBufferSize];
uint16_t sRxLength;

char     sTxBuffer[kTxBufferSize];
uint16_t sTxHead;
uint16_t sTxLength;

uint16_t sSendLength;

#ifdef OT_CLI_UART_LOCK_HDR_FILE

#include OT_CLI_UART_LOCK_HDR_FILE

#else

/**
 * Macro to acquire an exclusive lock of uart cli output
 * Default implementation does nothing
 *
 */
#ifndef OT_CLI_UART_OUTPUT_LOCK
#define OT_CLI_UART_OUTPUT_LOCK() \
    do                            \
    {                             \
    } while (0)
#endif

/**
 * Macro to release the exclusive lock of uart cli output
 * Default implementation does nothing
 *
 */
#ifndef OT_CLI_UART_OUTPUT_UNLOCK
#define OT_CLI_UART_OUTPUT_UNLOCK() \
    do                              \
    {                               \
    } while (0)
#endif

#endif // OT_CLI_UART_LOCK_HDR_FILE

static int     Output(const char *aBuf, uint16_t aBufLength);
static otError ProcessCommand(void);

static void ReceiveTask(const uint8_t *aBuf, uint16_t aBufLength)
{
    static const char sEraseString[] = {'\b', ' ', '\b'};
    static const char CRNL[]         = {'\r', '\n'};
    static uint8_t    sLastChar      = '\0';
    const uint8_t *   end;

    end = aBuf + aBufLength;

    for (; aBuf < end; aBuf++)
    {
        switch (*aBuf)
        {
        case '\n':
            if (sLastChar == '\r')
            {
                break;
            }

            OT_FALL_THROUGH;

        case '\r':
            Output(CRNL, sizeof(CRNL));
            sRxBuffer[sRxLength] = '\0';
            IgnoreError(ProcessCommand());
            break;

#if OPENTHREAD_POSIX && !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
        case 0x03: // ASCII for Ctrl-C
            kill(0, SIGINT);
            break;

        case 0x04: // ASCII for Ctrl-D
            exit(EXIT_SUCCESS);
            break;
#endif

        case '\b':
        case 127:
            if (sRxLength > 0)
            {
                Output(sEraseString, sizeof(sEraseString));
                sRxBuffer[--sRxLength] = '\0';
            }

            break;

        default:
            if (sRxLength < kRxBufferSize - 1)
            {
                Output(reinterpret_cast<const char *>(aBuf), 1);
                sRxBuffer[sRxLength++] = static_cast<char>(*aBuf);
            }

            break;
        }

        sLastChar = *aBuf;
    }
}

static otError ProcessCommand(void)
{
    otError error = OT_ERROR_NONE;

    while (sRxLength > 0 && (sRxBuffer[sRxLength - 1] == '\n' || sRxBuffer[sRxLength - 1] == '\r'))
    {
        sRxBuffer[--sRxLength] = '\0';
    }

    otCliInputLine(sRxBuffer);
    sRxLength = 0;

    return error;
}

static void Send(void)
{
    VerifyOrExit(sSendLength == 0);

    if (sTxLength > kTxBufferSize - sTxHead)
    {
        sSendLength = kTxBufferSize - sTxHead;
    }
    else
    {
        sSendLength = sTxLength;
    }

    if (sSendLength > 0)
    {
#if OPENTHREAD_CONFIG_ENABLE_DEBUG_UART
        /* duplicate the output to the debug uart */
        otSysDebugUart_write_bytes(reinterpret_cast<uint8_t *>(sTxBuffer + sTxHead), sSendLength);
#endif
        IgnoreError(otPlatUartSend(reinterpret_cast<uint8_t *>(sTxBuffer + sTxHead), sSendLength));
    }

exit:
    return;
}

static void SendDoneTask(void)
{
    sTxHead = (sTxHead + sSendLength) % kTxBufferSize;
    sTxLength -= sSendLength;
    sSendLength = 0;

    Send();
}

static int Output(const char *aBuf, uint16_t aBufLength)
{
    OT_CLI_UART_OUTPUT_LOCK();
    uint16_t sent = 0;

    while (aBufLength > 0)
    {
        uint16_t remaining = kTxBufferSize - sTxLength;
        uint16_t tail;
        uint16_t sendLength = aBufLength;

        if (sendLength > remaining)
        {
            sendLength = remaining;
        }

        for (uint16_t i = 0; i < sendLength; i++)
        {
            tail            = (sTxHead + sTxLength) % kTxBufferSize;
            sTxBuffer[tail] = *aBuf++;
            aBufLength--;
            sTxLength++;
        }

        Send();

        sent += sendLength;

        if (aBufLength > 0)
        {
            // More to send, so flush what's waiting now
            otError err = otPlatUartFlush();

            if (err == OT_ERROR_NONE)
            {
                // Flush successful, reset the pointers
                SendDoneTask();
            }
            else
            {
                // Flush did not succeed, so abort here.
                break;
            }
        }
    }

    OT_CLI_UART_OUTPUT_UNLOCK();

    return sent;
}

static int CliUartOutput(void *aContext, const char *aFormat, va_list aArguments)
{
    OT_UNUSED_VARIABLE(aContext);

    int rval;

    if (sTxLength == 0)
    {
        rval = vsnprintf(sTxBuffer, kTxBufferSize, aFormat, aArguments);
        VerifyOrExit(rval >= 0 && rval < kTxBufferSize, otLogWarnPlat("Failed to format CLI output `%s`", aFormat));
        sTxHead     = 0;
        sTxLength   = static_cast<uint16_t>(rval);
        sSendLength = 0;
    }
    else
    {
        va_list  retryArguments;
        uint16_t tail      = (sTxHead + sTxLength) % kTxBufferSize;
        uint16_t remaining = (sTxHead > tail ? (sTxHead - tail) : (kTxBufferSize - tail));

        va_copy(retryArguments, aArguments);

        rval = vsnprintf(&sTxBuffer[tail], remaining, aFormat, aArguments);

        if (rval < 0)
        {
            otLogWarnPlat("Failed to format CLI output `%s`", aFormat);
        }
        else if (rval < remaining)
        {
            sTxLength += rval;
        }
        else if (rval < kTxBufferSize)
        {
            while (sTxLength != 0)
            {
                otError error;

                Send();

                error = otPlatUartFlush();

                if (error == OT_ERROR_NONE)
                {
                    // Flush successful, reset the pointers
                    SendDoneTask();
                }
                else
                {
                    // Flush did not succeed, so abort here.
                    otLogWarnPlat("Failed to output CLI: %s", otThreadErrorToString(error));
                    ExitNow();
                }
            }
            rval = vsnprintf(sTxBuffer, kTxBufferSize, aFormat, retryArguments);
            OT_ASSERT(rval > 0);
            sTxLength   = static_cast<uint16_t>(rval);
            sTxHead     = 0;
            sSendLength = 0;
        }
        else
        {
            otLogWarnPlat("CLI output `%s` truncated", aFormat);
        }

        va_end(retryArguments);
    }

    Send();

exit:
    return rval;
}

void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    ReceiveTask(aBuf, aBufLength);
}

void otPlatUartSendDone(void)
{
    SendDoneTask();
}

extern "C" void otAppCliInit(otInstance *aInstance)
{
    sRxLength   = 0;
    sTxHead     = 0;
    sTxLength   = 0;
    sSendLength = 0;

    IgnoreError(otPlatUartEnable());

    otCliInit(aInstance, CliUartOutput, aInstance);
}
