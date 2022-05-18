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
 *   This file implements the OpenThread platform abstraction for UART communication.
 *
 */

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <openthread/platform/debug_uart.h>
#include <openthread/platform/logging.h>

#include "platform-cc2538.h"
#include "utils/code_utils.h"
#include "utils/uart.h"

enum
{
    kPlatformClock     = 32000000,
    kBaudRate          = 115200,
    kReceiveBufferSize = 128,
};

extern void UART0IntHandler(void);

static void processReceive(void);
static void processTransmit(void);

static const uint8_t *sTransmitBuffer = NULL;
static uint16_t       sTransmitLength = 0;

typedef struct RecvBuffer
{
    // The data buffer
    uint8_t mBuffer[kReceiveBufferSize];
    // The offset of the first item written to the list.
    uint16_t mHead;
    // The offset of the next item to be written to the list.
    uint16_t mTail;
} RecvBuffer;

static RecvBuffer sReceive;

static void enable_uart_clocks(void)
{
    static int uart_clocks_done = 0;

    if (uart_clocks_done)
    {
        return;
    }

    uart_clocks_done = 1;

#if OPENTHREAD_CONFIG_ENABLE_DEBUG_UART
    HWREG(SYS_CTRL_RCGCUART) = (SYS_CTRL_RCGCUART_UART0 | SYS_CTRL_RCGCUART_UART1);
    HWREG(SYS_CTRL_SCGCUART) = (SYS_CTRL_SCGCUART_UART0 | SYS_CTRL_SCGCUART_UART1);
    HWREG(SYS_CTRL_DCGCUART) = (SYS_CTRL_DCGCUART_UART0 | SYS_CTRL_DCGCUART_UART1);
#else
    HWREG(SYS_CTRL_RCGCUART) = SYS_CTRL_RCGCUART_UART0;
    HWREG(SYS_CTRL_SCGCUART) = SYS_CTRL_SCGCUART_UART0;
    HWREG(SYS_CTRL_DCGCUART) = SYS_CTRL_DCGCUART_UART0;
#endif
}

otError otPlatUartEnable(void)
{
    uint32_t div;

    sReceive.mHead = 0;
    sReceive.mTail = 0;

    // clock
    enable_uart_clocks();

    HWREG(UART0_BASE + UART_O_CC) = 0;

    // tx pin
    HWREG(IOC_PA1_SEL)  = IOC_MUX_OUT_SEL_UART0_TXD;
    HWREG(IOC_PA1_OVER) = IOC_OVERRIDE_OE;
    HWREG(GPIO_A_BASE + GPIO_O_AFSEL) |= GPIO_PIN_1;

    // rx pin
    HWREG(IOC_UARTRXD_UART0) = IOC_PAD_IN_SEL_PA0;
    HWREG(IOC_PA0_OVER)      = IOC_OVERRIDE_DIS;
    HWREG(GPIO_A_BASE + GPIO_O_AFSEL) |= GPIO_PIN_0;

    HWREG(UART0_BASE + UART_O_CTL) = 0;

    // baud rate
    div                             = (((kPlatformClock * 8) / kBaudRate) + 1) / 2;
    HWREG(UART0_BASE + UART_O_IBRD) = div / 64;
    HWREG(UART0_BASE + UART_O_FBRD) = div % 64;
    HWREG(UART0_BASE + UART_O_LCRH) = UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE;

    // configure interrupts
    HWREG(UART0_BASE + UART_O_IM) |= UART_IM_RXIM | UART_IM_RTIM;

    // enable
    HWREG(UART0_BASE + UART_O_CTL) = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

    // enable interrupts
    HWREG(NVIC_EN0) = 1 << ((INT_UART0 - 16) & 31);

    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sTransmitBuffer == NULL, error = OT_ERROR_BUSY);

    sTransmitBuffer = aBuf;
    sTransmitLength = aBufLength;

exit:
    return error;
}

void processReceive(void)
{
    // Copy tail to prevent multiple reads
    uint16_t tail = sReceive.mTail;

    // If the data wraps around, process the first part
    if (sReceive.mHead > tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, kReceiveBufferSize - sReceive.mHead);

        // Reset the buffer mHead back to zero.
        sReceive.mHead = 0;
    }

    // For any data remaining, process it
    if (sReceive.mHead != tail)
    {
        otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, tail - sReceive.mHead);

        // Set mHead to the local tail we have cached
        sReceive.mHead = tail;
    }
}

otError otPlatUartFlush(void)
{
    otEXPECT(sTransmitBuffer != NULL);

    for (; sTransmitLength > 0; sTransmitLength--)
    {
        while (HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF)
            ;

        HWREG(UART0_BASE + UART_O_DR) = *sTransmitBuffer++;
    }

    sTransmitBuffer = NULL;
    return OT_ERROR_NONE;

exit:
    return OT_ERROR_INVALID_STATE;
}

void processTransmit(void)
{
    otPlatUartFlush();
    otPlatUartSendDone();
}

void cc2538UartProcess(void)
{
    processReceive();
    processTransmit();
}

void UART0IntHandler(void)
{
    uint32_t mis;
    uint8_t  byte;

    mis                            = HWREG(UART0_BASE + UART_O_MIS);
    HWREG(UART0_BASE + UART_O_ICR) = mis;

    if (mis & (UART_IM_RXIM | UART_IM_RTIM))
    {
        while (!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE))
        {
            byte = HWREG(UART0_BASE + UART_O_DR);

            // We can only write if incrementing mTail doesn't equal mHead
            if (sReceive.mHead != (sReceive.mTail + 1) % kReceiveBufferSize)
            {
                sReceive.mBuffer[sReceive.mTail] = byte;
                sReceive.mTail                   = (sReceive.mTail + 1) % kReceiveBufferSize;
            }
        }
    }
}

#if OPENTHREAD_CONFIG_ENABLE_DEBUG_UART

int otPlatDebugUart_kbhit(void)
{
    uint32_t v;

    /* get flags */
    v = HWREG(UART1_BASE + UART_O_FR);

    /* if FIFO empty we have no data */
    return !(v & UART_FR_RXFE);
}

int otPlatDebugUart_getc(void)
{
    int v = 1;

    /* if nothing in fifo */
    if (!otPlatDebugUart_kbhit())
    {
        return -1;
    }

    /* fetch */
    v = (int)HWREG(UART0_BASE + UART_O_DR);
    v = (v & 0x0ff);
    return v;
}

void otPlatDebugUart_putchar_raw(int b)
{
    /* wait till not busy */
    while (HWREG(UART1_BASE + UART_O_FR) & UART_FR_TXFF)
        ;

    /* write byte */
    HWREG(UART1_BASE + UART_O_DR) = ((uint32_t)(b & 0x0ff));
}

void cc2538DebugUartInit(void)
{
    int32_t a, b;

    // clocks
    enable_uart_clocks();

    HWREG(UART1_BASE + UART_O_CC) = 0;

    // UART1 - tx pin
    // Using an RF06 Evaluation board
    // http://www.ti.com/tool/cc2538dk
    // PA3 => is jumper position RF1.14
    // To use these, you will require a "flying-lead" UART adapter
    HWREG(IOC_PA3_SEL)  = IOC_MUX_OUT_SEL_UART1_TXD;
    HWREG(IOC_PA3_OVER) = IOC_OVERRIDE_OE;
    HWREG(GPIO_A_BASE + GPIO_O_AFSEL) |= GPIO_PIN_3;

    // UART1 - rx pin we don't really use but we setup anyway
    // PA2 => is jumper position RF1.16
    HWREG(IOC_UARTRXD_UART1) = IOC_PAD_IN_SEL_PA2;
    HWREG(IOC_PA2_OVER)      = IOC_OVERRIDE_DIS;
    HWREG(GPIO_A_BASE + GPIO_O_AFSEL) |= GPIO_PIN_2;

    HWREG(UART1_BASE + UART_O_CC) = 0;

    // baud rate
    b = (((kPlatformClock * 8) / kBaudRate) + 1) / 2;
    a = b / 64;
    b = b % 64;

    HWREG(UART1_BASE + UART_O_IBRD) = a;
    HWREG(UART1_BASE + UART_O_FBRD) = b;
    HWREG(UART1_BASE + UART_O_LCRH) = UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE;

    /* NOTE:
     *  uart1 is not using IRQs it is tx only
     *  and we block when writing bytes
     */
    HWREG(UART1_BASE + UART_O_CTL) = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

#endif
