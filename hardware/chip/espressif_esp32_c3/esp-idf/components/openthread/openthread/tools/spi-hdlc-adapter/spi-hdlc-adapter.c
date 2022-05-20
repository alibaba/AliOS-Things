/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE 1

#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H 0
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ucontext.h>

#include <linux/ioctl.h>
#include <linux/spi/spidev.h>

#ifndef HAVE_EXECINFO_H
#define HAVE_EXECINFO_H 0
#endif

#if HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifndef HAVE_PTY_H
#define HAVE_PTY_H 0
#endif

#if HAVE_PTY_H
#include <pty.h>
#endif

#ifndef HAVE_UTIL_H
#define HAVE_UTIL_H 0
#endif

#if HAVE_UTIL_H
#include <util.h>
#endif

#ifndef HAVE_OPENPTY
#define HAVE_OPENPTY 0
#endif

/* ------------------------------------------------------------------------- */
/* MARK: Macros and Constants */

#define SPI_HDLC_VERSION "0.07"

#define MAX_FRAME_SIZE 2048
#define HEADER_LEN 5
#define SPI_HEADER_RESET_FLAG 0x80
#define SPI_HEADER_CRC_FLAG 0x40
#define SPI_HEADER_PATTERN_VALUE 0x02
#define SPI_HEADER_PATTERN_MASK 0x03

#define EXIT_QUIT 65535

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC 1000
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC (USEC_PER_MSEC * MSEC_PER_SEC)
#endif

#define SPI_POLL_PERIOD_MSEC (MSEC_PER_SEC / 30)

#define IMMEDIATE_RETRY_COUNT 5
#define FAST_RETRY_COUNT 15

#define IMMEDIATE_RETRY_TIMEOUT_MSEC 1
#define FAST_RETRY_TIMEOUT_MSEC 10
#define SLOW_RETRY_TIMEOUT_MSEC 33

#define GPIO_INT_ASSERT_STATE 0 // I̅N̅T̅ is asserted low
#define GPIO_RES_ASSERT_STATE 0 // R̅E̅S̅ is asserted low

#define SPI_RX_ALIGN_ALLOWANCE_MAX 16

#define SOCKET_DEBUG_BYTES_PER_LINE 16

#ifndef AUTO_PRINT_BACKTRACE
#define AUTO_PRINT_BACKTRACE (HAVE_EXECINFO_H)
#endif

#define AUTO_PRINT_BACKTRACE_STACK_DEPTH 20

static const uint8_t  kHdlcResetSignal[] = {0x7E, 0x13, 0x11, 0x7E};
static const uint16_t kHdlcCrcCheckValue = 0xf0b8;
static const uint16_t kHdlcCrcResetValue = 0xffff;

enum
{
    MODE_STDIO = 0,
    MODE_PTY   = 1,
};

// Ignores return value from function 's'
#define IGNORE_RETURN_VALUE(s) \
    do                         \
    {                          \
        if (s)                 \
        {                      \
        }                      \
    } while (0)

/* ------------------------------------------------------------------------- */
/* MARK: Global State */

#if HAVE_OPENPTY
static int sMode = MODE_PTY;
#else
static int sMode = MODE_STDIO;
#endif

static int sLogLevel = LOG_WARNING;

static int sSpiDevFd       = -1;
static int sResGpioValueFd = -1;
static int sIntGpioValueFd = -1;

static int sHdlcInputFd  = -1;
static int sHdlcOutputFd = -1;

static int     sSpiSpeed      = 1000000; // in Hz (default: 1MHz)
static uint8_t sSpiMode       = 0;
static int     sSpiCsDelay    = 20; // in microseconds
static int     sSpiResetDelay = 0;  // in milliseconds

static uint16_t sSpiRxPayloadSize;
static uint8_t  sSpiRxFrameBuffer[MAX_FRAME_SIZE + SPI_RX_ALIGN_ALLOWANCE_MAX];

static uint16_t sSpiTxPayloadSize;
static bool     sSpiTxIsReady      = false;
static int      sSpiTxRefusedCount = 0;
static uint8_t  sSpiTxFrameBuffer[MAX_FRAME_SIZE + SPI_RX_ALIGN_ALLOWANCE_MAX];

static int sSpiRxAlignAllowance = 0;
static int sSpiSmallPacketSize  = 32; // in bytes

static bool sSlaveDidReset = false;

static int sCaughtSignal = -1;

// If sUseRawFrames is set to true, HDLC encoding/encoding
// is skipped and the raw frames are read-from/written-to
// the sHdlcInputFd/sHdlcOutputFd whole. See `--raw`.
static bool sUseRawFrames = false;

static int sMTU = MAX_FRAME_SIZE - HEADER_LEN;

static int sRet = 0;

static bool sDumpStats = false;

static sig_t sPreviousHandlerForSIGINT;
static sig_t sPreviousHandlerForSIGTERM;

/* ------------------------------------------------------------------------- */
/* MARK: Statistics */

static uint64_t sSlaveResetCount           = 0;
static uint64_t sSpiFrameCount             = 0;
static uint64_t sSpiValidFrameCount        = 0;
static uint64_t sSpiGarbageFrameCount      = 0;
static uint64_t sSpiDuplexFrameCount       = 0;
static uint64_t sSpiUnresponsiveFrameCount = 0;
static uint64_t sHdlcRxFrameByteCount      = 0;
static uint64_t sHdlcTxFrameByteCount      = 0;
static uint64_t sHdlcRxFrameCount          = 0;
static uint64_t sHdlcTxFrameCount          = 0;
static uint64_t sHdlcRxBadCrcCount         = 0;

/* ------------------------------------------------------------------------- */
/* MARK: Signal Handlers */

static void signal_SIGINT(int sig)
{
    static const char message[] = "\nCaught SIGINT!\n";

    sRet = EXIT_QUIT;

    // Can't use syslog() because it isn't async signal safe.
    // So we write to stderr
    IGNORE_RETURN_VALUE(write(STDERR_FILENO, message, sizeof(message) - 1));
    sCaughtSignal = sig;

    // Restore the previous handler so that if we end up getting
    // this signal again we perform the system default action.
    signal(SIGINT, sPreviousHandlerForSIGINT);
    sPreviousHandlerForSIGINT = NULL;

    // Ignore signal argument.
    (void)sig;
}

static void signal_SIGTERM(int sig)
{
    static const char message[] = "\nCaught SIGTERM!\n";

    sRet = EXIT_QUIT;

    // Can't use syslog() because it isn't async signal safe.
    // So we write to stderr
    IGNORE_RETURN_VALUE(write(STDERR_FILENO, message, sizeof(message) - 1));
    sCaughtSignal = sig;

    // Restore the previous handler so that if we end up getting
    // this signal again we perform the system default action.
    signal(SIGTERM, sPreviousHandlerForSIGTERM);
    sPreviousHandlerForSIGTERM = NULL;

    // Ignore signal argument.
    (void)sig;
}

static void signal_SIGHUP(int sig)
{
    static const char message[] = "\nCaught SIGHUP!\n";

    sRet = EXIT_FAILURE;

    // Can't use syslog() because it isn't async signal safe.
    // So we write to stderr
    IGNORE_RETURN_VALUE(write(STDERR_FILENO, message, sizeof(message) - 1));
    sCaughtSignal = sig;

    // We don't restore the "previous handler"
    // because we always want to let the main
    // loop decide what to do for hangups.

    // Ignore signal argument.
    (void)sig;
}

static void signal_dumpstats(int sig)
{
    sDumpStats = true;

    // Ignore signal argument.
    (void)sig;
}

static void signal_clearstats(int sig)
{
    sDumpStats                 = true;
    sSlaveResetCount           = 0;
    sSpiFrameCount             = 0;
    sSpiValidFrameCount        = 0;
    sSpiGarbageFrameCount      = 0;
    sSpiDuplexFrameCount       = 0;
    sSpiUnresponsiveFrameCount = 0;
    sHdlcRxFrameByteCount      = 0;
    sHdlcTxFrameByteCount      = 0;
    sHdlcRxFrameCount          = 0;
    sHdlcTxFrameCount          = 0;
    sHdlcRxBadCrcCount         = 0;

    // Ignore signal argument.
    (void)sig;
}

#if AUTO_PRINT_BACKTRACE
static void signal_critical(int sig, siginfo_t *info, void *ucontext)
{
    // This is the last hurah for this process.
    // We dump the stack, because that's all we can do.

    void *      stack_mem[AUTO_PRINT_BACKTRACE_STACK_DEPTH];
    void **     stack = stack_mem;
    char **     stack_symbols;
    int         stack_depth, i;
    ucontext_t *uc = (ucontext_t *)ucontext;

    // Shut up compiler warning.
    (void)uc;
    (void)info;

    // We call some functions here which aren't async-signal-safe,
    // but this function isn't really useful without those calls.
    // Since we are making a gamble (and we deadlock if we loose),
    // we are going to set up a two-second watchdog to make sure
    // we end up terminating like we should. The choice of a two
    // second timeout is entirely arbitrary, and may be changed
    // if needs warrant.
    alarm(2);
    signal(SIGALRM, SIG_DFL);

    fprintf(stderr, " *** FATAL ERROR: Caught signal %d (%s):\n", sig, strsignal(sig));

    stack_depth = backtrace(stack, AUTO_PRINT_BACKTRACE_STACK_DEPTH);

    // Here are are trying to update the pointer in the backtrace
    // to be the actual location of the fault.
#if defined(__x86_64__)
    stack[1] = (void *)uc->uc_mcontext.gregs[REG_RIP];
#elif defined(__i386__)
    stack[1] = (void *)uc->uc_mcontext.gregs[REG_EIP];
#elif defined(__arm__)
    stack[1] = (void *)uc->uc_mcontext.arm_ip;
#else
#warning TODO: Add this arch to signal_critical
#endif

    // Now dump the symbols to stderr, in case syslog barfs.
    backtrace_symbols_fd(stack, stack_depth, STDERR_FILENO);

    // Load up the symbols individually, so we can output to syslog, too.
    stack_symbols = backtrace_symbols(stack, stack_depth);

    syslog(LOG_CRIT, " *** FATAL ERROR: Caught signal %d (%s):", sig, strsignal(sig));

    for (i = 0; i != stack_depth; i++)
    {
        syslog(LOG_CRIT, "[BT] %2d: %s", i, stack_symbols[i]);
    }

    free(stack_symbols);

    exit(EXIT_FAILURE);
}
#endif // if AUTO_PRINT_BACKTRACE

static void log_debug_buffer(const char *desc, const uint8_t *buffer_ptr, int buffer_len, bool force)
{
    int i = 0;

    if (!force && (sLogLevel < LOG_DEBUG))
    {
        return;
    }

    while (i < buffer_len)
    {
        int  j;
        char dump_string[SOCKET_DEBUG_BYTES_PER_LINE * 3 + 1];

        for (j = 0; i < buffer_len && j < SOCKET_DEBUG_BYTES_PER_LINE; i++, j++)
        {
            sprintf(dump_string + j * 3, "%02X ", buffer_ptr[i]);
        }

        syslog(force ? LOG_WARNING : LOG_DEBUG, "%s: %s%s", desc, dump_string, (i < buffer_len) ? " ..." : "");
    }
}

/* ------------------------------------------------------------------------- */
/* MARK: SPI Transfer Functions */

static void spi_header_set_flag_byte(uint8_t *header, uint8_t value)
{
    header[0] = value;
}

static void spi_header_set_accept_len(uint8_t *header, uint16_t len)
{
    header[1] = ((len >> 0) & 0xFF);
    header[2] = ((len >> 8) & 0xFF);
}

static void spi_header_set_data_len(uint8_t *header, uint16_t len)
{
    header[3] = ((len >> 0) & 0xFF);
    header[4] = ((len >> 8) & 0xFF);
}

static uint8_t spi_header_get_flag_byte(const uint8_t *header)
{
    return header[0];
}

static uint16_t spi_header_get_accept_len(const uint8_t *header)
{
    return (header[1] + (uint16_t)(header[2] << 8));
}

static uint16_t spi_header_get_data_len(const uint8_t *header)
{
    return (header[3] + (uint16_t)(header[4] << 8));
}

static uint8_t *get_real_rx_frame_start(void)
{
    uint8_t *ret = sSpiRxFrameBuffer;
    int      i   = 0;

    for (i = 0; i < sSpiRxAlignAllowance; i++)
    {
        if (ret[0] != 0xFF)
        {
            break;
        }
        ret++;
    }

    return ret;
}

static int do_spi_xfer(int len)
{
    int ret;

    struct spi_ioc_transfer xfer[2] = {{
                                           // This part is the delay between C̅S̅ being
                                           // asserted and the SPI clock starting. This
                                           // is not supported by all Linux SPI drivers.
                                           .tx_buf        = 0,
                                           .rx_buf        = 0,
                                           .len           = 0,
                                           .delay_usecs   = (uint16_t)sSpiCsDelay,
                                           .speed_hz      = (uint32_t)sSpiSpeed,
                                           .bits_per_word = 8,
                                           .cs_change     = false,
                                       },
                                       {
                                           // This part is the actual SPI transfer.
                                           .tx_buf        = (unsigned long)sSpiTxFrameBuffer,
                                           .rx_buf        = (unsigned long)sSpiRxFrameBuffer,
                                           .len           = (uint32_t)(len + HEADER_LEN + sSpiRxAlignAllowance),
                                           .delay_usecs   = 0,
                                           .speed_hz      = (uint32_t)sSpiSpeed,
                                           .bits_per_word = 8,
                                           .cs_change     = false,
                                       }};

    if (sSpiCsDelay > 0)
    {
        // A C̅S̅ delay has been specified. Start transactions
        // with both parts.
        ret = ioctl(sSpiDevFd, SPI_IOC_MESSAGE(2), &xfer[0]);
    }
    else
    {
        // No C̅S̅ delay has been specified, so we skip the first
        // part because it causes some SPI drivers to croak.
        ret = ioctl(sSpiDevFd, SPI_IOC_MESSAGE(1), &xfer[1]);
    }

    if (ret != -1)
    {
        log_debug_buffer("SPI-TX", sSpiTxFrameBuffer, (int)xfer[1].len, false);
        log_debug_buffer("SPI-RX", sSpiRxFrameBuffer, (int)xfer[1].len, false);

        sSpiFrameCount++;
    }

    return ret;
}

static void debug_spi_header(const char *hint, bool force)
{
    if (force || (sLogLevel >= LOG_DEBUG))
    {
        const uint8_t *spiRxFrameBuffer = get_real_rx_frame_start();

        syslog(force ? LOG_WARNING : LOG_DEBUG, "%s-TX: H:%02X ACCEPT:%d DATA:%0d\n", hint,
               spi_header_get_flag_byte(sSpiTxFrameBuffer), spi_header_get_accept_len(sSpiTxFrameBuffer),
               spi_header_get_data_len(sSpiTxFrameBuffer));

        syslog(force ? LOG_WARNING : LOG_DEBUG, "%s-RX: H:%02X ACCEPT:%d DATA:%0d\n", hint,
               spi_header_get_flag_byte(spiRxFrameBuffer), spi_header_get_accept_len(spiRxFrameBuffer),
               spi_header_get_data_len(spiRxFrameBuffer));
    }
}

static int push_pull_spi(void)
{
    int            ret;
    uint16_t       spi_xfer_bytes   = 0;
    const uint8_t *spiRxFrameBuffer = NULL;
    uint8_t        slave_header;
    uint16_t       slave_max_rx;
    int            successful_exchanges = 0;

    static uint16_t slave_data_len;

    // For now, sSpiRxPayloadSize must be zero
    // when entering this function. This may change
    // at some point, for now this makes things
    // much easier.
    assert(sSpiRxPayloadSize == 0);

    if (sSpiValidFrameCount == 0)
    {
        // Set the reset flag to indicate to our slave that we
        // are coming up from scratch.
        spi_header_set_flag_byte(sSpiTxFrameBuffer, SPI_HEADER_RESET_FLAG | SPI_HEADER_PATTERN_VALUE);
    }
    else
    {
        spi_header_set_flag_byte(sSpiTxFrameBuffer, SPI_HEADER_PATTERN_VALUE);
    }

    // Zero out our rx_accept and our data_len for now.
    spi_header_set_accept_len(sSpiTxFrameBuffer, 0);
    spi_header_set_data_len(sSpiTxFrameBuffer, 0);

    // Sanity check.
    if (slave_data_len > MAX_FRAME_SIZE)
    {
        slave_data_len = 0;
    }

    if (sSpiTxIsReady)
    {
        // Go ahead and try to immediately send a frame if we have it queued up.
        spi_header_set_data_len(sSpiTxFrameBuffer, sSpiTxPayloadSize);

        if (sSpiTxPayloadSize > spi_xfer_bytes)
        {
            spi_xfer_bytes = sSpiTxPayloadSize;
        }
    }

    if (sSpiRxPayloadSize == 0)
    {
        if (slave_data_len != 0)
        {
            // In a previous transaction the slave indicated
            // it had something to send us. Make sure our
            // transaction is large enough to handle it.
            if (slave_data_len > spi_xfer_bytes)
            {
                spi_xfer_bytes = slave_data_len;
            }
        }
        else
        {
            // Set up a minimum transfer size to allow small
            // frames the slave wants to send us to be handled
            // in a single transaction.
            if (sSpiSmallPacketSize > spi_xfer_bytes)
            {
                spi_xfer_bytes = (uint16_t)sSpiSmallPacketSize;
            }
        }

        spi_header_set_accept_len(sSpiTxFrameBuffer, spi_xfer_bytes);
    }

    // Perform the SPI transaction.
    ret = do_spi_xfer(spi_xfer_bytes);

    if (ret < 0)
    {
        perror("push_pull_spi:do_spi_xfer");
        syslog(LOG_ERR, "push_pull_spi:do_spi_xfer: errno=%d (%s)", errno, strerror(errno));

        // Print out a helpful error message for
        // a common error.
        if ((sSpiCsDelay != 0) && (errno == EINVAL))
        {
            syslog(LOG_ERR, "SPI ioctl failed with EINVAL. Try adding `--spi-cs-delay=0` to command line arguments.");
        }
        goto bail;
    }

    // Account for misalignment (0xFF bytes at the start)
    spiRxFrameBuffer = get_real_rx_frame_start();

    debug_spi_header("push_pull", false);

    slave_header = spi_header_get_flag_byte(spiRxFrameBuffer);

    if ((slave_header == 0xFF) || (slave_header == 0x00))
    {
        if ((slave_header == spiRxFrameBuffer[1]) && (slave_header == spiRxFrameBuffer[2]) &&
            (slave_header == spiRxFrameBuffer[3]) && (slave_header == spiRxFrameBuffer[4]))
        {
            // Device is off or in a bad state.
            // In some cases may be induced by flow control.
            syslog(slave_data_len == 0 ? LOG_DEBUG : LOG_WARNING,
                   "Slave did not respond to frame. (Header was all 0x%02X)", slave_header);
            sSpiUnresponsiveFrameCount++;
        }
        else
        {
            // Header is full of garbage
            syslog(LOG_WARNING, "Garbage in header : %02X %02X %02X %02X %02X", spiRxFrameBuffer[0],
                   spiRxFrameBuffer[1], spiRxFrameBuffer[2], spiRxFrameBuffer[3], spiRxFrameBuffer[4]);
            sSpiGarbageFrameCount++;
            if (sLogLevel < LOG_DEBUG)
            {
                log_debug_buffer("SPI-TX", sSpiTxFrameBuffer, (int)spi_xfer_bytes + HEADER_LEN + sSpiRxAlignAllowance,
                                 true);
                log_debug_buffer("SPI-RX", sSpiRxFrameBuffer, (int)spi_xfer_bytes + HEADER_LEN + sSpiRxAlignAllowance,
                                 true);
            }
        }
        sSpiTxRefusedCount++;
        goto bail;
    }

    slave_max_rx   = spi_header_get_accept_len(spiRxFrameBuffer);
    slave_data_len = spi_header_get_data_len(spiRxFrameBuffer);

    if (((slave_header & SPI_HEADER_PATTERN_MASK) != SPI_HEADER_PATTERN_VALUE) || (slave_max_rx > MAX_FRAME_SIZE) ||
        (slave_data_len > MAX_FRAME_SIZE))
    {
        sSpiGarbageFrameCount++;
        sSpiTxRefusedCount++;
        slave_data_len = 0;
        syslog(LOG_WARNING, "Garbage in header : %02X %02X %02X %02X %02X", spiRxFrameBuffer[0], spiRxFrameBuffer[1],
               spiRxFrameBuffer[2], spiRxFrameBuffer[3], spiRxFrameBuffer[4]);
        if (sLogLevel < LOG_DEBUG)
        {
            log_debug_buffer("SPI-TX", sSpiTxFrameBuffer, (int)spi_xfer_bytes + HEADER_LEN + sSpiRxAlignAllowance,
                             true);
            log_debug_buffer("SPI-RX", sSpiRxFrameBuffer, (int)spi_xfer_bytes + HEADER_LEN + sSpiRxAlignAllowance,
                             true);
        }
        goto bail;
    }

    sSpiValidFrameCount++;

    if ((slave_header & SPI_HEADER_RESET_FLAG) == SPI_HEADER_RESET_FLAG)
    {
        sSlaveResetCount++;
        syslog(LOG_NOTICE, "Slave did reset (%llu resets so far)", (unsigned long long)sSlaveResetCount);
        sSlaveDidReset = true;
        sDumpStats     = true;
    }

    // Handle received packet, if any.
    if ((sSpiRxPayloadSize == 0) && (slave_data_len != 0) &&
        (slave_data_len <= spi_header_get_accept_len(sSpiTxFrameBuffer)))
    {
        // We have received a packet. Set sSpiRxPayloadSize so that
        // the packet will eventually get queued up by push_hdlc().
        sSpiRxPayloadSize = slave_data_len;

        slave_data_len = 0;

        successful_exchanges++;
    }

    // Handle transmitted packet, if any.
    if (sSpiTxIsReady && (sSpiTxPayloadSize == spi_header_get_data_len(sSpiTxFrameBuffer)))
    {
        if (spi_header_get_data_len(sSpiTxFrameBuffer) <= slave_max_rx)
        {
            // Our outbound packet has been successfully transmitted. Clear
            // sSpiTxPayloadSize and sSpiTxIsReady so that pull_hdlc() can
            // pull another packet for us to send.
            sSpiTxIsReady      = false;
            sSpiTxPayloadSize  = 0;
            sSpiTxRefusedCount = 0;
            successful_exchanges++;
        }
        else
        {
            // The slave Wasn't ready for what we had to
            // send them. Incrementing this counter will
            // turn on rate limiting so that we
            // don't waste a ton of CPU bombarding them
            // with useless SPI transfers.
            sSpiTxRefusedCount++;
        }
    }

    if (!sSpiTxIsReady)
    {
        sSpiTxRefusedCount = 0;
    }

    if (successful_exchanges == 2)
    {
        sSpiDuplexFrameCount++;
    }
bail:
    return ret;
}

static bool check_and_clear_interrupt(void)
{
    if (sIntGpioValueFd >= 0)
    {
        char    value[5] = "";
        ssize_t len;

        lseek(sIntGpioValueFd, 0, SEEK_SET);

        len = read(sIntGpioValueFd, value, sizeof(value) - 1);

        if (len < 0)
        {
            perror("check_and_clear_interrupt");
            sRet = EXIT_FAILURE;
        }

        // The interrupt pin is active low.
        return GPIO_INT_ASSERT_STATE == atoi(value);
    }

    return true;
}

/* ------------------------------------------------------------------------- */
/* MARK: HDLC Transfer Functions */

#define HDLC_BYTE_FLAG 0x7E
#define HDLC_BYTE_ESC 0x7D
#define HDLC_BYTE_XON 0x11
#define HDLC_BYTE_XOFF 0x13
#define HDLC_BYTE_SPECIAL 0xF8
#define HDLC_ESCAPE_XFORM 0x20

static uint16_t hdlc_crc16(uint16_t aFcs, uint8_t aByte)
{
#if 1
    // CRC-16/CCITT, CRC-16/CCITT-TRUE, CRC-CCITT
    // width=16 poly=0x1021 init=0x0000 refin=true refout=true xorout=0x0000 check=0x2189 name="KERMIT"
    // http://reveng.sourceforge.net/crc-catalogue/16.htm#crc.cat.kermit
    static const uint16_t sFcsTable[256] = {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5,
        0xe97e, 0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52,
        0xdaed, 0xcb64, 0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3,
        0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9,
        0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285, 0x430c, 0x7197, 0x601e,
        0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 0x6306, 0x728f,
        0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862,
        0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb,
        0x4e64, 0x5fed, 0x6d76, 0x7cff, 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1, 0x0948,
        0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226,
        0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 0xc60c, 0xd785, 0xe51e, 0xf497,
        0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704,
        0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb,
        0x0e70, 0x1ff9, 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c,
        0x3de3, 0x2c6a, 0x1ef1, 0x0f78};
    return (aFcs >> 8) ^ sFcsTable[(aFcs ^ aByte) & 0xff];
#else
    // CRC-16/CCITT-FALSE, same CRC as 802.15.4
    // width=16 poly=0x1021 init=0xffff refin=false refout=false xorout=0x0000 check=0x29b1 name="CRC-16/CCITT-FALSE"
    // http://reveng.sourceforge.net/crc-catalogue/16.htm#crc.cat.crc-16-ccitt-false
    aFcs = (uint16_t)((aFcs >> 8) | (aFcs << 8));
    aFcs ^= aByte;
    aFcs ^= ((aFcs & 0xff) >> 4);
    aFcs ^= (aFcs << 12);
    aFcs ^= ((aFcs & 0xff) << 5);
    return aFcs;
#endif
}

static bool hdlc_byte_needs_escape(uint8_t byte)
{
    switch (byte)
    {
    case HDLC_BYTE_SPECIAL:
    case HDLC_BYTE_ESC:
    case HDLC_BYTE_FLAG:
    case HDLC_BYTE_XOFF:
    case HDLC_BYTE_XON:
        return true;

    default:
        return false;
    }
}

static int push_hdlc(void)
{
    int             ret              = 0;
    const uint8_t * spiRxFrameBuffer = get_real_rx_frame_start();
    static uint8_t  escaped_frame_buffer[MAX_FRAME_SIZE * 2];
    static uint16_t unescaped_frame_len;
    static uint16_t escaped_frame_len;
    static uint16_t escaped_frame_sent;

    if (escaped_frame_len == 0)
    {
        if (sSlaveDidReset)
        {
            // Indicate an MCU reset.
            memcpy(escaped_frame_buffer, kHdlcResetSignal, sizeof(kHdlcResetSignal));
            escaped_frame_len = sizeof(kHdlcResetSignal);
            sSlaveDidReset    = false;

            // Set this to zero, since this isn't a real frame.
            unescaped_frame_len = 0;
        }
        else if (sSpiRxPayloadSize != 0)
        {
            // Escape the frame.
            uint8_t  c;
            uint16_t fcs = kHdlcCrcResetValue;
            uint16_t i;

            unescaped_frame_len = sSpiRxPayloadSize;

            for (i = 0; i < sSpiRxPayloadSize; i++)
            {
                c   = spiRxFrameBuffer[i + HEADER_LEN];
                fcs = hdlc_crc16(fcs, c);
                if (hdlc_byte_needs_escape(c))
                {
                    escaped_frame_buffer[escaped_frame_len++] = HDLC_BYTE_ESC;
                    escaped_frame_buffer[escaped_frame_len++] = c ^ HDLC_ESCAPE_XFORM;
                }
                else
                {
                    escaped_frame_buffer[escaped_frame_len++] = c;
                }
            }

            fcs ^= 0xFFFF;

            c = fcs & 0xFF;
            if (hdlc_byte_needs_escape(c))
            {
                escaped_frame_buffer[escaped_frame_len++] = HDLC_BYTE_ESC;
                escaped_frame_buffer[escaped_frame_len++] = c ^ HDLC_ESCAPE_XFORM;
            }
            else
            {
                escaped_frame_buffer[escaped_frame_len++] = c;
            }

            c = (fcs >> 8) & 0xFF;
            if (hdlc_byte_needs_escape(c))
            {
                escaped_frame_buffer[escaped_frame_len++] = HDLC_BYTE_ESC;
                escaped_frame_buffer[escaped_frame_len++] = c ^ HDLC_ESCAPE_XFORM;
            }
            else
            {
                escaped_frame_buffer[escaped_frame_len++] = c;
            }

            escaped_frame_buffer[escaped_frame_len++] = HDLC_BYTE_FLAG;
            escaped_frame_sent                        = 0;
            sSpiRxPayloadSize                         = 0;
        }
        else
        {
            // Nothing to do.
            goto bail;
        }
    }

    ret = (int)write(sHdlcOutputFd, escaped_frame_buffer + escaped_frame_sent, escaped_frame_len - escaped_frame_sent);

    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            ret = 0;
        }
        else
        {
            perror("push_hdlc:write");
            syslog(LOG_ERR, "push_hdlc:write: errno=%d (%s)", errno, strerror(errno));
        }
        goto bail;
    }

    escaped_frame_sent += ret;

    // Reset state once we have sent the entire frame.
    if (escaped_frame_len == escaped_frame_sent)
    {
        escaped_frame_len = escaped_frame_sent = 0;

        // Increment counter for statistics
        sHdlcTxFrameCount++;
        sHdlcTxFrameByteCount += unescaped_frame_len;
    }

    ret = 0;

bail:
    return ret;
}

static int pull_hdlc(void)
{
    int             ret = 0;
    static uint16_t fcs;
    static bool     unescape_next_byte = false;

    if (!sSpiTxIsReady)
    {
        uint8_t byte;
        while ((ret = (int)read(sHdlcInputFd, &byte, 1)) == 1)
        {
            if (sSpiTxPayloadSize >= (MAX_FRAME_SIZE - HEADER_LEN))
            {
                syslog(LOG_WARNING, "HDLC frame was too big");
                unescape_next_byte = false;
                sSpiTxPayloadSize  = 0;
                fcs                = kHdlcCrcResetValue;
            }
            else if (byte == HDLC_BYTE_FLAG)
            {
                if (sSpiTxPayloadSize <= 2)
                {
                    unescape_next_byte = false;
                    sSpiTxPayloadSize  = 0;
                    fcs                = kHdlcCrcResetValue;
                    continue;
                }
                else if (fcs != kHdlcCrcCheckValue)
                {
                    syslog(LOG_WARNING, "HDLC frame with bad CRC (LEN:%d, FCS:0x%04X)", sSpiTxPayloadSize, fcs);
                    sHdlcRxBadCrcCount++;
                    unescape_next_byte = false;
                    sSpiTxPayloadSize  = 0;
                    fcs                = kHdlcCrcResetValue;
                    continue;
                }

                // Clip off the CRC
                sSpiTxPayloadSize -= 2;

                // Indicate that a frame is ready to go out
                sSpiTxIsReady = true;

                // Increment counters for statistics
                sHdlcRxFrameCount++;
                sHdlcRxFrameByteCount += sSpiTxPayloadSize;

                // Clean up for the next frame
                unescape_next_byte = false;
                fcs                = kHdlcCrcResetValue;
                break;
            }
            else if (byte == HDLC_BYTE_ESC)
            {
                unescape_next_byte = true;
                continue;
            }
            else if (hdlc_byte_needs_escape(byte))
            {
                // Skip all other control codes.
                continue;
            }
            else if (unescape_next_byte)
            {
                byte               = byte ^ HDLC_ESCAPE_XFORM;
                unescape_next_byte = false;
            }

            fcs                                                 = hdlc_crc16(fcs, byte);
            sSpiTxFrameBuffer[HEADER_LEN + sSpiTxPayloadSize++] = byte;
        }
    }

    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            ret = 0;
        }
        else
        {
            perror("pull_hdlc:read");
            syslog(LOG_ERR, "pull_hdlc:read: errno=%d (%s)", errno, strerror(errno));
        }
    }

    return ret < 0 ? ret : 0;
}

/* ------------------------------------------------------------------------- */
/* MARK: Raw Transfer Functions */

static int push_raw(void)
{
    int             ret              = 0;
    const uint8_t * spiRxFrameBuffer = get_real_rx_frame_start();
    static uint8_t  raw_frame_buffer[MAX_FRAME_SIZE];
    static uint16_t raw_frame_len;
    static uint16_t raw_frame_sent;

    if (raw_frame_len == 0)
    {
        if (sSlaveDidReset)
        {
            // Indicates an MCU reset.
            // We don't have anything to do here because
            // raw mode doesn't have any way to signal
            // resets out-of-band.
            sSlaveDidReset = false;
        }
        else if (sSpiRxPayloadSize > 0)
        {
            // Read the frame into raw_frame_buffer
            assert(sSpiRxPayloadSize <= sizeof(raw_frame_buffer));
            memcpy(raw_frame_buffer, &spiRxFrameBuffer[HEADER_LEN], sSpiRxPayloadSize);
            raw_frame_len     = sSpiRxPayloadSize;
            raw_frame_sent    = 0;
            sSpiRxPayloadSize = 0;
        }
        else
        {
            // Nothing to do.
            goto bail;
        }
    }

    ret = (int)write(sHdlcOutputFd, raw_frame_buffer + raw_frame_sent, raw_frame_len - raw_frame_sent);

    if (ret < 0)
    {
        if (errno == EAGAIN)
        {
            ret = 0;
        }
        else
        {
            perror("push_raw:write");
            syslog(LOG_ERR, "push_raw:write: errno=%d (%s)", errno, strerror(errno));
        }
        goto bail;
    }

    raw_frame_sent += ret;

    // Reset state once we have sent the entire frame.
    if (raw_frame_len == raw_frame_sent)
    {
        // Increment counter for statistics
        sHdlcTxFrameCount++;
        sHdlcTxFrameByteCount += raw_frame_len;

        raw_frame_len = raw_frame_sent = 0;
    }

    ret = 0;

bail:
    return ret;
}

static int pull_raw(void)
{
    int ret = 0;

    if (!sSpiTxIsReady)
    {
        ret = (int)read(sHdlcInputFd, &sSpiTxFrameBuffer[HEADER_LEN], (size_t)sMTU);

        if (ret < 0)
        {
            if (errno == EAGAIN)
            {
                ret = 0;
            }
            else
            {
                perror("pull_raw:read");
                syslog(LOG_ERR, "pull_raw:read: errno=%d (%s)", errno, strerror(errno));
            }
        }
        else if (ret > 0)
        {
            sSpiTxPayloadSize = (uint16_t)ret;
            sSpiTxIsReady     = true;

            // Increment counters for statistics
            sHdlcRxFrameCount++;
            sHdlcRxFrameByteCount += sSpiTxPayloadSize;
        }
    }

    return ret < 0 ? ret : 0;
}

/* ------------------------------------------------------------------------- */
/* MARK: Setup Functions */

static bool update_spi_mode(int x)
{
    sSpiMode = (uint8_t)x;

    if ((sSpiDevFd >= 0) && (ioctl(sSpiDevFd, SPI_IOC_WR_MODE, &sSpiMode) < 0))
    {
        perror("ioctl(SPI_IOC_WR_MODE)");
        return false;
    }

    return true;
}

static bool update_spi_speed(int x)
{
    sSpiSpeed = x;

    if ((sSpiDevFd >= 0) && (ioctl(sSpiDevFd, SPI_IOC_WR_MAX_SPEED_HZ, &sSpiSpeed) < 0))
    {
        perror("ioctl(SPI_IOC_WR_MAX_SPEED_HZ)");
        return false;
    }

    return true;
}

static bool setup_spi_dev(const char *path)
{
    int           fd            = -1;
    const uint8_t spi_word_bits = 8;
    int           ret;

    syslog(LOG_DEBUG, "SPI device path: %s", path);

    fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0)
    {
        perror("open");
        goto bail;
    }

    // Set the SPI mode.
    ret = ioctl(fd, SPI_IOC_WR_MODE, &sSpiMode);
    if (ret < 0)
    {
        perror("ioctl(SPI_IOC_WR_MODE)");
        goto bail;
    }

    // Set the SPI clock speed.
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &sSpiSpeed);
    if (ret < 0)
    {
        perror("ioctl(SPI_IOC_WR_MAX_SPEED_HZ)");
        goto bail;
    }

    // Set the SPI word size.
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_word_bits);
    if (ret < 0)
    {
        perror("ioctl(SPI_IOC_WR_BITS_PER_WORD)");
        goto bail;
    }

    // Lock the file descriptor
    if (flock(fd, LOCK_EX | LOCK_NB) < 0)
    {
        perror("flock");
        goto bail;
    }

    sSpiDevFd = fd;
    fd        = -1;

bail:
    if (fd >= 0)
    {
        close(fd);
    }
    return sSpiDevFd >= 0;
}

static bool setup_res_gpio(const char *path)
{
    int   setup_fd   = -1;
    char *dir_path   = NULL;
    char *value_path = NULL;
    int   len;

    syslog(LOG_DEBUG, "Reset gpio path: %s", path);

    len = asprintf(&dir_path, "%s/direction", path);

    if (len < 0)
    {
        perror("asprintf");
        goto bail;
    }

    len = asprintf(&value_path, "%s/value", path);

    if (len < 0)
    {
        perror("asprintf");
        goto bail;
    }

    setup_fd = open(dir_path, O_WRONLY | O_CLOEXEC);

    if (setup_fd >= 0)
    {
        if (-1 == write(setup_fd, "high\n", 5))
        {
            perror("set_res_direction");
            goto bail;
        }
    }

    sResGpioValueFd = open(value_path, O_WRONLY | O_CLOEXEC);

bail:

    if (setup_fd >= 0)
    {
        close(setup_fd);
    }

    if (dir_path)
    {
        free(dir_path);
    }

    if (value_path)
    {
        free(value_path);
    }

    return sResGpioValueFd >= 0;
}

static void trigger_reset(void)
{
    if (sResGpioValueFd >= 0)
    {
        char str[] = {'0' + GPIO_RES_ASSERT_STATE, '\n'};

        lseek(sResGpioValueFd, 0, SEEK_SET);
        if (write(sResGpioValueFd, str, sizeof(str)) == -1)
        {
            syslog(LOG_ERR, "trigger_reset(): error on write: %d (%s)", errno, strerror(errno));
        }

        usleep(10 * USEC_PER_MSEC);

        // Set the string to switch to the not-asserted state.
        str[0] = '0' + !GPIO_RES_ASSERT_STATE;

        lseek(sResGpioValueFd, 0, SEEK_SET);
        if (write(sResGpioValueFd, str, sizeof(str)) == -1)
        {
            syslog(LOG_ERR, "trigger_reset(): error on write: %d (%s)", errno, strerror(errno));
        }

        syslog(LOG_NOTICE, "Triggered hardware reset");
    }
}

static bool setup_int_gpio(const char *path)
{
    char *  edge_path  = NULL;
    char *  dir_path   = NULL;
    char *  value_path = NULL;
    ssize_t len;
    int     setup_fd = -1;

    sIntGpioValueFd = -1;

    syslog(LOG_DEBUG, "Interrupt gpio path: %s", path);

    len = asprintf(&dir_path, "%s/direction", path);

    if (len < 0)
    {
        perror("asprintf");
        goto bail;
    }

    len = asprintf(&edge_path, "%s/edge", path);

    if (len < 0)
    {
        perror("asprintf");
        goto bail;
    }

    len = asprintf(&value_path, "%s/value", path);

    if (len < 0)
    {
        perror("asprintf");
        goto bail;
    }

    setup_fd = open(dir_path, O_WRONLY | O_CLOEXEC);

    if (setup_fd >= 0)
    {
        len = write(setup_fd, "in", 2);
        if (len < 0)
        {
            perror("write");
            goto bail;
        }

        close(setup_fd);
    }

    setup_fd = open(edge_path, O_WRONLY | O_CLOEXEC);

    if (setup_fd >= 0)
    {
        len = write(setup_fd, "falling", 7);

        if (len < 0)
        {
            perror("write");
            goto bail;
        }

        close(setup_fd);

        setup_fd = -1;
    }

    sIntGpioValueFd = open(value_path, O_RDONLY | O_CLOEXEC);

bail:

    if (setup_fd >= 0)
    {
        close(setup_fd);
    }

    if (edge_path)
    {
        free(edge_path);
    }

    if (dir_path)
    {
        free(dir_path);
    }

    if (value_path)
    {
        free(value_path);
    }

    return sIntGpioValueFd >= 0;
}

/* ------------------------------------------------------------------------- */
/* MARK: Help */

static void print_version(void)
{
    printf("spi-hdlc-adapter " SPI_HDLC_VERSION " (" __TIME__ " " __DATE__ ")\n");
    printf("Copyright (c) 2017 The OpenThread Authors, All Rights Reserved\n");
}

static void print_help(void)
{
    print_version();
    const char *help = "\n"
                       "Syntax:\n"
                       "\n"
                       "    spi-hdlc [options] <spi-device-path>\n"
                       "\n"
                       "Options:\n"
                       "\n"
                       "    --stdio ...................... Use `stdin` and `stdout` for HDLC input and\n"
                       "                                   output. Useful when directly started by the\n"
                       "                                   program that will be using it.\n"
#if HAVE_OPENPTY
                       "    --pty ........................ Create a pseudoterminal for HDLC input and\n"
                       "                                   output. The path of the newly-created PTY\n"
                       "                                   will be written to `stdout`, followed by a\n"
                       "                                   newline.\n"
#endif // HAVE_OPENPTY
                       "    --raw ........................ Do not encode/decode packets using HDLC.\n"
                       "                                   Instead, write whole, raw frames to the\n"
                       "                                   specified input and output FDs. This is useful\n"
                       "                                   for emulating a serial port, or when datagram-\n"
                       "                                   based sockets are supplied for stdin and\n"
                       "                                   stdout` (when used with --stdio).\n"
                       "    --mtu=[MTU] .................. Specify the MTU. Currently only used in raw mode.\n"
                       "                                   Default and maximum value is 2043.\n"
                       "    -i/--gpio-int[=gpio-path] .... Specify a path to the Linux sysfs-exported\n"
                       "                                   GPIO directory for the `I̅N̅T̅` pin. If not\n"
                       "                                   specified, `spi-hdlc` will fall back to\n"
                       "                                   polling, which is inefficient.\n"
                       "    -r/--gpio-reset[=gpio-path] .. Specify a path to the Linux sysfs-exported\n"
                       "                                   GPIO directory for the `R̅E̅S̅` pin.\n"
                       "    --spi-mode[=mode] ............ Specify the SPI mode to use (0-3).\n"
                       "    --spi-speed[=hertz] .......... Specify the SPI speed in hertz.\n"
                       "    --spi-cs-delay[=usec] ........ Specify the delay after C̅S̅ assertion, in µsec\n"
                       "    --spi-reset-delay[=ms] ....... Specify the delay after R̅E̅S̅E̅T̅ assertion, in miliseconds\n"
                       "    --spi-align-allowance[=n] .... Specify the maximum number of 0xFF bytes to\n"
                       "                                   clip from start of MISO frame. Max value is 16.\n"
                       "    --spi-small-packet=[n] ....... Specify the smallest packet we can receive\n"
                       "                                   in a single transaction(larger packets will\n"
                       "                                   require two transactions). Default value is 32.\n"
                       "    -v/--verbose[=num] ............Change log verbosity level. (Repeatable)\n"
                       "                                   num argument is optional and value 1 is default\n"
                       "                                   when not specified. Every instance of this option\n"
                       "                                   will increment or decrement (when num is negative)\n"
                       "                                   the syslog log level accordingly. Starting default\n"
                       "                                   log level is LOG_NOTICE (5).\n"
                       "    -h/-?/--help ................. Print out usage information and exit.\n"
                       "\n";

    printf("%s", help);
}

static const char *log_level_to_str(int log_level)
{
    const char *str;

    switch (log_level)
    {
    case LOG_EMERG:
        str = "EMERG";
        break;
    case LOG_ALERT:
        str = "ALERT";
        break;
    case LOG_CRIT:
        str = "CRIT";
        break;
    case LOG_ERR:
        str = "ERR";
        break;
    case LOG_WARNING:
        str = "WARNING";
        break;
    case LOG_NOTICE:
        str = "NOTICE";
        break;
    case LOG_INFO:
        str = "INFO";
        break;
    case LOG_DEBUG:
        str = "DEBUG";
        break;
    default:
        str = "-unknown-";
        break;
    }

    return str;
}

/* ------------------------------------------------------------------------- */
/* MARK: Main Loop */

int main(int argc, char *argv[])
{
    int            i = 0;
    char           prog[32];
    static fd_set  read_set;
    static fd_set  write_set;
    static fd_set  error_set;
    struct timeval timeout;
    int            max_fd                   = -1;
    bool           did_print_rate_limit_log = false;

#if AUTO_PRINT_BACKTRACE
    struct sigaction sigact;
#endif // if AUTO_PRINT_BACKTRACE

    enum
    {
        ARG_SPI_MODE            = 1001,
        ARG_SPI_SPEED           = 1002,
        ARG_VERBOSE             = 1003,
        ARG_SPI_CS_DELAY        = 1004,
        ARG_SPI_ALIGN_ALLOWANCE = 1005,
        ARG_RAW                 = 1006,
        ARG_MTU                 = 1007,
        ARG_SPI_SMALL_PACKET    = 1008,
        ARG_SPI_RESET_DELAY     = 1009,
    };

    static struct option options[] = {
        {"stdio", no_argument, &sMode, MODE_STDIO},
        {"pty", no_argument, &sMode, MODE_PTY},
        {"gpio-int", required_argument, NULL, 'i'},
        {"gpio-res", required_argument, NULL, 'r'},
        {"verbose", optional_argument, NULL, ARG_VERBOSE},
        {"version", no_argument, NULL, 'V'},
        {"raw", no_argument, NULL, ARG_RAW},
        {"mtu", required_argument, NULL, ARG_MTU},
        {"help", no_argument, NULL, 'h'},
        {"spi-mode", required_argument, NULL, ARG_SPI_MODE},
        {"spi-speed", required_argument, NULL, ARG_SPI_SPEED},
        {"spi-cs-delay", required_argument, NULL, ARG_SPI_CS_DELAY},
        {"spi-align-allowance", required_argument, NULL, ARG_SPI_ALIGN_ALLOWANCE},
        {"spi-small-packet", required_argument, NULL, ARG_SPI_SMALL_PACKET},
        {"spi-reset-delay", required_argument, NULL, ARG_SPI_RESET_DELAY},
        {NULL, 0, NULL, 0},
    };

    strncpy(prog, argv[0], sizeof(prog) - 1);
    prog[sizeof(prog) - 1] = 0;

    if (argc < 2)
    {
        print_help();
        exit(EXIT_FAILURE);
    }

    // ========================================================================
    // INITIALIZATION

    sPreviousHandlerForSIGINT  = signal(SIGINT, &signal_SIGINT);
    sPreviousHandlerForSIGTERM = signal(SIGTERM, &signal_SIGTERM);
    signal(SIGHUP, &signal_SIGHUP);
    signal(SIGUSR1, &signal_dumpstats);
    signal(SIGUSR2, &signal_clearstats);

#if AUTO_PRINT_BACKTRACE
    sigact.sa_sigaction = &signal_critical;
    sigact.sa_flags     = SA_RESTART | SA_SIGINFO | SA_NOCLDWAIT;

    sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL);
    sigaction(SIGBUS, &sigact, (struct sigaction *)NULL);
    sigaction(SIGILL, &sigact, (struct sigaction *)NULL);
    sigaction(SIGABRT, &sigact, (struct sigaction *)NULL);
#endif // if AUTO_PRINT_BACKTRACE

    // ========================================================================
    // ARGUMENT PARSING

    openlog(basename(prog), LOG_PERROR | LOG_PID | LOG_CONS, LOG_DAEMON);

    setlogmask(LOG_UPTO(sLogLevel));

    while (1)
    {
        int c = getopt_long(argc, argv, "i:r:vVh?", options, NULL);
        if (c == -1)
        {
            break;
        }
        else
        {
            switch (c)
            {
            case 'i':
                if (!setup_int_gpio(optarg))
                {
                    syslog(LOG_ERR, "Unable to setup INT GPIO \"%s\", %s", optarg, strerror(errno));
                    exit(EXIT_FAILURE);
                }
                break;

            case ARG_SPI_ALIGN_ALLOWANCE:
                assert(optarg);

                errno                = 0;
                sSpiRxAlignAllowance = atoi(optarg);

                if (errno != 0 || (sSpiRxAlignAllowance < 0))
                {
                    syslog(LOG_ERR, "Invalid SPI RX Align Allowance \"%s\"", optarg);
                    exit(EXIT_FAILURE);
                }

                if (sSpiRxAlignAllowance > SPI_RX_ALIGN_ALLOWANCE_MAX)
                {
                    syslog(LOG_WARNING, "Reducing SPI RX Align Allowance from %s to %d", optarg,
                           SPI_RX_ALIGN_ALLOWANCE_MAX);
                    sSpiRxAlignAllowance = SPI_RX_ALIGN_ALLOWANCE_MAX;
                }

                break;

            case ARG_SPI_MODE:
                assert(optarg);

                if (!update_spi_mode(atoi(optarg)))
                {
                    syslog(LOG_ERR, "Unable to set SPI mode to \"%s\", %s", optarg, strerror(errno));
                    exit(EXIT_FAILURE);
                }
                break;

            case ARG_SPI_SPEED:
                assert(optarg);

                if (!update_spi_speed(atoi(optarg)))
                {
                    syslog(LOG_ERR, "Unable to set SPI speed to \"%s\", %s", optarg, strerror(errno));
                    exit(EXIT_FAILURE);
                }
                break;

            case ARG_SPI_SMALL_PACKET:
                assert(optarg);

                sSpiSmallPacketSize = atoi(optarg);
                if (sSpiSmallPacketSize > MAX_FRAME_SIZE - HEADER_LEN)
                {
                    syslog(LOG_WARNING, "Reducing SPI small-packet size from %s to %d", optarg,
                           MAX_FRAME_SIZE - HEADER_LEN);
                    sSpiSmallPacketSize = MAX_FRAME_SIZE - HEADER_LEN;
                }
                if (sSpiSmallPacketSize < 0)
                {
                    syslog(LOG_ERR, "The argument to --spi-small-packet cannot be negative. (Given: \"%s\")", optarg);
                    exit(EXIT_FAILURE);
                }
                syslog(LOG_NOTICE, "SPI small-packet size set to %d bytes.", sSpiSmallPacketSize);
                break;

            case ARG_SPI_CS_DELAY:
                assert(optarg);

                sSpiCsDelay = atoi(optarg);
                if (sSpiCsDelay < 0)
                {
                    syslog(LOG_ERR, "Negative values (%d) for --spi-cs-delay are invalid.", sSpiCsDelay);
                    exit(EXIT_FAILURE);
                }
                syslog(LOG_NOTICE, "SPI CS Delay set to %d usec", sSpiCsDelay);
                break;

            case ARG_SPI_RESET_DELAY:
                assert(optarg);

                sSpiResetDelay = atoi(optarg);
                if (sSpiResetDelay < 0)
                {
                    syslog(LOG_ERR, "Negative value (%d) for --spi-reset-delay is invalid.", sSpiResetDelay);
                    exit(EXIT_FAILURE);
                }
                syslog(LOG_NOTICE, "SPI RESET Delay set to %d ms", sSpiResetDelay);
                break;

            case ARG_RAW:
                sUseRawFrames = true;
                syslog(LOG_NOTICE, "HDLC encoding/decoding disabled. Will use raw frames for input/output.");
                break;

            case ARG_MTU:
                assert(optarg);

                sMTU = atoi(optarg);
                if (sMTU > MAX_FRAME_SIZE - HEADER_LEN)
                {
                    syslog(LOG_ERR, "Specified MTU of %d is too large, maximum is %d bytes.", sMTU,
                           MAX_FRAME_SIZE - HEADER_LEN);
                    exit(EXIT_FAILURE);
                }
                if (sMTU < 1)
                {
                    syslog(LOG_ERR, "Specified MTU of %d is too small, minimum is 1 byte.", sMTU);
                    exit(EXIT_FAILURE);
                }
                syslog(LOG_NOTICE, "MTU set to %d bytes", sMTU);
                break;

            case 'r':
                if (!setup_res_gpio(optarg))
                {
                    syslog(LOG_ERR, "Unable to setup RES GPIO \"%s\", %s", optarg, strerror(errno));
                    exit(EXIT_FAILURE);
                }
                break;

            case 'v':
            case ARG_VERBOSE:
                sLogLevel += (optarg != NULL) ? atoi(optarg) : 1;

                if (sLogLevel > LOG_DEBUG)
                {
                    sLogLevel = LOG_DEBUG;
                }

                if (sLogLevel < LOG_EMERG)
                {
                    sLogLevel = LOG_EMERG;
                }

                setlogmask(LOG_UPTO(sLogLevel));
                syslog(sLogLevel, "Verbosity set to log level %s (%d)", log_level_to_str(sLogLevel), sLogLevel);
                break;

            case 'V':
                print_version();
                exit(EXIT_SUCCESS);
                break;

            case 'h':
            case '?':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            }
        }
    }

    syslog(LOG_NOTICE, "spi-hdlc-adapter " SPI_HDLC_VERSION " (" __TIME__ " " __DATE__ ")\n");

    if (optind == argc)
    {
        fprintf(stderr, "%s: Missing SPI device path\n", prog);
        exit(EXIT_FAILURE);
    }
    else if (optind + 1 == argc)
    {
        if (!setup_spi_dev(argv[optind]))
        {
            char spi_path[64];

            strncpy(spi_path, argv[optind], sizeof(spi_path) - 1);
            spi_path[sizeof(spi_path) - 1] = 0;
            syslog(LOG_ERR, "%s: Unable to open SPI device \"%s\", %s", prog, spi_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "%s: Unexpected argument \"%s\"\n", prog, argv[optind + 1]);
        exit(EXIT_FAILURE);
    }

    if (sMode == MODE_STDIO)
    {
        sHdlcInputFd  = dup(STDIN_FILENO);
        sHdlcOutputFd = dup(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
    }
    else if (sMode == MODE_PTY)
    {
#if HAVE_OPENPTY

        static int pty_slave_fd = -1;
        char       pty_name[1024];
        sRet = openpty(&sHdlcInputFd, &pty_slave_fd, pty_name, NULL, NULL);

        if (sRet != 0)
        {
            perror("openpty");
            goto bail;
        }

        sHdlcOutputFd = dup(sHdlcInputFd);

        printf("%s\n", pty_name);

        close(STDOUT_FILENO);

#else // if HAVE_OPENPTY

        syslog(LOG_ERR, "Not built with support for `--pty`.");
        sRet = EXIT_FAILURE;
        goto bail;

#endif // else HAVE_OPENPTY
    }
    else
    {
        sRet = EXIT_FAILURE;
        goto bail;
    }

    if ((sHdlcInputFd < 0) || (sHdlcOutputFd < 0))
    {
        sRet = EXIT_FAILURE;
        goto bail;
    }

    // Set up sHdlcInputFd for non-blocking I/O
    if (-1 == (i = fcntl(sHdlcInputFd, F_GETFL, 0)))
    {
        i = 0;
    }
    IGNORE_RETURN_VALUE(fcntl(sHdlcInputFd, F_SETFL, i | O_NONBLOCK));

    // Since there are so few file descriptors in
    // this program, we calculate `max_fd` once
    // instead of trying to optimize its value
    // at every iteration.
    max_fd = sHdlcInputFd;

    if (max_fd < sHdlcOutputFd)
    {
        max_fd = sHdlcOutputFd;
    }

    if (max_fd < sIntGpioValueFd)
    {
        max_fd = sIntGpioValueFd;
    }

    if (sIntGpioValueFd < 0)
    {
        syslog(LOG_WARNING, "Interrupt pin was not set, must poll SPI. Performance will suffer.");
    }

    trigger_reset();

    usleep((useconds_t)sSpiResetDelay * USEC_PER_MSEC);

    // ========================================================================
    // MAIN LOOP

    while (sRet == 0)
    {
        int timeout_ms = MSEC_PER_SEC * 60 * 60 * 24; // 24 hours

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_ZERO(&error_set);

        if (!sSpiTxIsReady)
        {
            FD_SET(sHdlcInputFd, &read_set);
        }
        else
        {
            // We have data to send to the slave.
            timeout_ms = 0;
        }

        if (sSpiRxPayloadSize != 0)
        {
            // We have data that we are waiting to send out
            // of the HDLC descriptor, so we need to wait
            // for that to clear out before we can do anything
            // else.
            FD_SET(sHdlcOutputFd, &write_set);
        }
        else if (sIntGpioValueFd >= 0)
        {
            if (check_and_clear_interrupt())
            {
                // Interrupt pin is asserted,
                // set the timeout to be 0.
                timeout_ms = 0;

                syslog(LOG_DEBUG, "Interrupt.");
            }
            else
            {
                // The interrupt pin was not asserted,
                // so we wait for the interrupt pin to
                // be asserted by adding it to the error
                // set.
                FD_SET(sIntGpioValueFd, &error_set);
            }
        }
        else if (timeout_ms > SPI_POLL_PERIOD_MSEC)
        {
            // In this case we don't have an interrupt, so
            // we revert to SPI polling.
            timeout_ms = SPI_POLL_PERIOD_MSEC;
        }

        if (sDumpStats)
        {
            timeout_ms = 0;
        }

        if (sSpiTxRefusedCount)
        {
            int min_timeout = 0;

            // We are being rate-limited by the slave. This is
            // fairly normal behavior. Based on number of times
            // slave has refused a transmission, we apply a
            // minimum timeout.

            if (sSpiTxRefusedCount < IMMEDIATE_RETRY_COUNT)
            {
                min_timeout = IMMEDIATE_RETRY_TIMEOUT_MSEC;
            }
            else if (sSpiTxRefusedCount < FAST_RETRY_COUNT)
            {
                min_timeout = FAST_RETRY_TIMEOUT_MSEC;
            }
            else
            {
                min_timeout = SLOW_RETRY_TIMEOUT_MSEC;
            }

            if (timeout_ms < min_timeout)
            {
                timeout_ms = min_timeout;
            }

            if (sSpiTxIsReady && !did_print_rate_limit_log && (sSpiTxRefusedCount > 1))
            {
                // To avoid printing out this message over and over,
                // we only print it out once the refused count is at
                // two or higher when we actually have something to
                // send the slave. And then, we only print it once.
                syslog(LOG_INFO, "Slave is rate limiting transactions");

                did_print_rate_limit_log = true;
            }

            if (sSpiTxRefusedCount == 30)
            {
                // Ua-oh. The slave hasn't given us a chance to send
                // it anything for over thirty frames. If this ever
                // happens, print out a warning to the logs.
                syslog(LOG_WARNING, "Slave seems stuck.");
            }

            if (sSpiTxRefusedCount == 100)
            {
                // Double ua-oh. The slave hasn't given us a chance
                // to send it anything for over a hundred frames.
                // This almost certainly means that the slave has
                // locked up or gotten into an unrecoverable state.
                // It is not spi-hdlc-adapter's job to identify and
                // reset misbehaving devices (that is handled at a
                // higher level), but we go ahead and log the condition
                // for debugging purposes.
                syslog(LOG_ERR, "Slave seems REALLY stuck.");
            }
        }
        else
        {
            did_print_rate_limit_log = false;
        }

        // Calculate the timeout value.
        timeout.tv_sec  = timeout_ms / MSEC_PER_SEC;
        timeout.tv_usec = (timeout_ms % MSEC_PER_SEC) * USEC_PER_MSEC;

        // Wait for something to happen.
        IGNORE_RETURN_VALUE(select(max_fd + 1, &read_set, &write_set, &error_set, &timeout));

        if (sDumpStats || sRet != 0)
        {
            sDumpStats = false;
            syslog(LOG_NOTICE, "INFO: sSlaveResetCount=%llu", (unsigned long long)sSlaveResetCount);
            syslog(LOG_NOTICE, "INFO: sSpiFrameCount=%llu", (unsigned long long)sSpiFrameCount);
            syslog(LOG_NOTICE, "INFO: sSpiValidFrameCount=%llu", (unsigned long long)sSpiValidFrameCount);
            syslog(LOG_NOTICE, "INFO: sSpiDuplexFrameCount=%llu", (unsigned long long)sSpiDuplexFrameCount);
            syslog(LOG_NOTICE, "INFO: sSpiUnresponsiveFrameCount=%llu", (unsigned long long)sSpiUnresponsiveFrameCount);
            syslog(LOG_NOTICE, "INFO: sSpiGarbageFrameCount=%llu", (unsigned long long)sSpiGarbageFrameCount);
            syslog(LOG_NOTICE, "INFO: sHdlcTxFrameCount=%llu", (unsigned long long)sHdlcTxFrameCount);
            syslog(LOG_NOTICE, "INFO: sHdlcTxFrameByteCount=%llu", (unsigned long long)sHdlcTxFrameByteCount);
            syslog(LOG_NOTICE, "INFO: sHdlcRxFrameCount=%llu", (unsigned long long)sHdlcRxFrameCount);
            syslog(LOG_NOTICE, "INFO: sHdlcRxFrameByteCount=%llu", (unsigned long long)sHdlcRxFrameByteCount);
            syslog(LOG_NOTICE, "INFO: sHdlcRxBadCrcCount=%llu", (unsigned long long)sHdlcRxBadCrcCount);
        }

        // Handle serial input.
        if (FD_ISSET(sHdlcInputFd, &read_set))
        {
            // Read in the data.
            if ((sUseRawFrames ? pull_raw() : pull_hdlc()) < 0)
            {
                sRet = EXIT_FAILURE;
                break;
            }
        }

        // Handle serial output.
        if (FD_ISSET(sHdlcOutputFd, &write_set))
        {
            // Write out the data.
            if ((sUseRawFrames ? push_raw() : push_hdlc()) < 0)
            {
                sRet = EXIT_FAILURE;
                break;
            }

            continue;
        }

        // Service the SPI port if we can receive
        // a packet or we have a packet to be sent.
        if ((sSpiRxPayloadSize == 0) && (sSpiTxIsReady || check_and_clear_interrupt()))
        {
            // We guard this with the above check because we don't
            // want to overwrite any previously received (but not
            // yet pushed out) frames.
            if (push_pull_spi() < 0)
            {
                sRet = EXIT_FAILURE;
            }
        }
    }

    // ========================================================================
    // SHUTDOWN

bail:
    if (sCaughtSignal != -1)
    {
        syslog(LOG_ERR, "Caught %s", strsignal(sCaughtSignal));
    }

    syslog(LOG_NOTICE, "Shutdown. (sRet = %d)", sRet);

    syslog(LOG_NOTICE, "Reset NCP/RCP");
    trigger_reset();

    if (sRet == EXIT_QUIT)
    {
        sRet = EXIT_SUCCESS;
    }
    else if (sRet == -1)
    {
        sRet = EXIT_FAILURE;
    }

    return sRet;
}
