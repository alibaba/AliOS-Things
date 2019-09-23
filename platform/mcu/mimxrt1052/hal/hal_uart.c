/*
 * Copyright 2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * POSIX getopt for Windows
 * Code given out at the 1985 UNIFORUM conference in Dallas.
 *
 * From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
 * Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
 * Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
 * Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
 * From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
 * Newsgroups: mod.std.unix
 * Subject: public domain AT&T getopt source
 * Message-ID: <3352@ut-sally.UUCP>
 * Date: 3 Nov 85 19:34:15 GMT
 * Date-Received: 4 Nov 85 12:25:09 GMT
 * Organization: IEEE/P1003 Portable Operating System Environment Committee
 * Lines: 91
 * Approved: jsq@ut-sally.UUC
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include "aos/kernel.h"
#include <k_api.h>
#include "aos/hal/uart.h"
#include "fsl_clock.h"
#include "fsl_lpuart.h"
#include "board.h"

/* Then transfer byte larger than the threshold, use non-blocking method. */
#define UART_TX_INT_THRESHOLD 4

static const uint32_t s_uartBaseAddrs[8] = {(uint32_t)LPUART1_BASE, (uint32_t)LPUART2_BASE,
(uint32_t)LPUART3_BASE, (uint32_t)LPUART4_BASE, (uint32_t)LPUART5_BASE, (uint32_t)LPUART6_BASE, (uint32_t)LPUART7_BASE, 
(uint32_t)LPUART8_BASE};
static lpuart_handle_t s_handle[9];
static ksem_t s_uartSendSem[9];
static ksem_t s_uartReceiveSem[9];
static kmutex_t s_uartRxMutex[9];
static kmutex_t s_uartTxMutex[9];

/* Global Variable for VFS DEBUG Output */
uart_dev_t uart_0 = {
    .port = 0,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_1 = {
    .port = 1,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};
uart_dev_t uart_2 = {
    .port = 2,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_3 = {
    .port = 3,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_4 = {
    .port = 4,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_5 = {
    .port = 5,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_6 = {
    .port = 6,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

uart_dev_t uart_7 = {
    .port = 8,                                                                          /* uart port */
    .config = {115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED}, /* uart config */
    .priv = NULL                                                                        /* priv data */
};

static void uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    uart_dev_t *uart = (uart_dev_t *)userData;

    if (kStatus_LPUART_TxIdle == status)
    {
        krhino_sem_give(&s_uartSendSem[uart->port]);
    }

    if (kStatus_LPUART_RxIdle == status)
    {
        krhino_sem_give(&s_uartReceiveSem[uart->port]);
    }
}

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
    lpuart_config_t config = {0};
    status_t status;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = uart->config.baud_rate;
    config.enableTx = true;
    config.enableRx = true;

    switch (uart->config.parity)
    {
        case NO_PARITY:
            config.parityMode = kLPUART_ParityDisabled;
            break;
        case ODD_PARITY:
            config.parityMode = kLPUART_ParityOdd;
            break;
        case EVEN_PARITY:
            config.parityMode = kLPUART_ParityEven;
            break;
        default:
            return EIO;
    }

    switch (uart->config.data_width)
    {
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
        case DATA_WIDTH_7BIT:
            config.dataBitsCount = kLPUART_SevenDataBits;
            break;
#endif
        case DATA_WIDTH_8BIT:
            config.dataBitsCount = kLPUART_EightDataBits;
            break;
        default:
            return EIO;
    }

    switch (uart->config.stop_bits)
    {
        case STOP_BITS_1:
            config.stopBitCount = kLPUART_OneStopBit;
            break;
        case STOP_BITS_2:
            config.stopBitCount = kLPUART_TwoStopBit;
            break;
        default:
            return EIO;
    }
    
    /* TODO: Need to have array to save clock source for all uart instances */
    uint32_t freq = 0;
    freq = BOARD_DebugConsoleSrcFreq();
    status = LPUART_Init((LPUART_Type *)s_uartBaseAddrs[uart->port], &config, freq);

    if (kStatus_Success != status)
        return EIO;
    
    /* Create semaphore  TODO: Add dynamic create support*/
    if (krhino_sem_create(&s_uartSendSem[uart->port], "uartSend", 0) != RHINO_SUCCESS)
    {
        return EIO;
    }

    if (krhino_sem_create(&s_uartReceiveSem[uart->port], "uartReceive", 0) != RHINO_SUCCESS)
    {
        return EIO;
    }

    if (krhino_mutex_create(&s_uartRxMutex[uart->port], "uartRxMutex") != RHINO_SUCCESS)
    {
        return EIO;
    }

    if (krhino_mutex_create(&s_uartTxMutex[uart->port], "uartTxMutex") != RHINO_SUCCESS)
    {
        return EIO;
    }    

    /* Create handle for LPUART */
    LPUART_TransferCreateHandle((LPUART_Type *)s_uartBaseAddrs[uart->port], &s_handle[uart->port],
                                uart_callback, uart);

    return 0;
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 * @param[in]  timeout Systick number for timeout
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    lpuart_transfer_t xfer;
    kstat_t stat = RHINO_SUCCESS;
    LPUART_Type *uartBase = (LPUART_Type *)s_uartBaseAddrs[uart->port];
    lpuart_handle_t *handle = &s_handle[uart->port];

    if (timeout == 0)
    {
        timeout = 1000;
    }

    /* Wait for Lock */
    stat = krhino_mutex_lock(&s_uartTxMutex[uart->port], timeout);
    if (stat != RHINO_SUCCESS)
    {
        return stat;
    }

    if (size < UART_TX_INT_THRESHOLD)
    {
        LPUART_WriteBlocking(uartBase, (uint8_t *)data, size);
    }
    else
    {
        xfer.data = (uint8_t *)data;
        xfer.dataSize = size;
        LPUART_TransferSendNonBlocking(uartBase, handle, &xfer);
        /* Wait for transfer finish */
        stat = krhino_sem_take(&s_uartSendSem[uart->port], timeout);

        if (stat != RHINO_SUCCESS)
        {
            LPUART_TransferAbortSend(uartBase, handle);

            /* Clear the semaphore in case it is set after timeout. */
            krhino_sem_count_set(&s_uartSendSem[uart->port], 0);

            stat = EIO;
        }
    }

    krhino_mutex_unlock(&s_uartTxMutex[uart->port]);

    return stat;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    lpuart_transfer_t xfer;
    kstat_t stat = RHINO_SUCCESS;
    LPUART_Type *uartBase = (LPUART_Type *)s_uartBaseAddrs[uart->port];
    lpuart_handle_t *handle = &s_handle[uart->port];

    if (timeout == 0)
    {
        timeout = 1000;
    }

    /* Wait for Lock */
    stat = krhino_mutex_lock(&s_uartRxMutex[uart->port], timeout);
    if (stat != RHINO_SUCCESS)
    {
        return stat;
    }

    xfer.data = (uint8_t *)data;
    xfer.dataSize = expect_size;
    if (kStatus_Success != LPUART_TransferReceiveNonBlocking(uartBase, handle, &xfer, NULL))
    {
        krhino_mutex_unlock(&s_uartRxMutex[uart->port]);
        return EIO;
    }

    /* Wait for transfer finish */
    stat = krhino_sem_take(&s_uartReceiveSem[uart->port], timeout);  

    if (stat != RHINO_SUCCESS)
    {
        LPUART_TransferAbortReceive(uartBase, handle);

        /* Clear the semaphore in case it is set after timeout. */
        krhino_sem_count_set(&s_uartReceiveSem[uart->port], 0);

        *recv_size = 0;

        stat = EIO;
    }
    else
    {
        *recv_size = expect_size;
    }

    krhino_mutex_unlock(&s_uartRxMutex[uart->port]);

    return stat;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    LPUART_Deinit((LPUART_Type *)s_uartBaseAddrs[uart->port]);
    /* Delete semaphore */
    krhino_sem_del(&s_uartSendSem[uart->port]);
    krhino_sem_del(&s_uartReceiveSem[uart->port]);
    krhino_mutex_del(&s_uartRxMutex[uart->port]);
    krhino_mutex_del(&s_uartTxMutex[uart->port]);
    return 0;
}
