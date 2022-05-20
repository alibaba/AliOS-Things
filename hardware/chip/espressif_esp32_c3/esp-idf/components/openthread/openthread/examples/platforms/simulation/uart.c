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

#include "platform-simulation.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <openthread/platform/debug_uart.h>

#include "utils/code_utils.h"
#include "utils/uart.h"

#if OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART == 0

static uint8_t        s_receive_buffer[128];
static const uint8_t *s_write_buffer;
static uint16_t       s_write_length;
static int            s_in_fd;
static int            s_out_fd;

static struct termios original_stdin_termios;
static struct termios original_stdout_termios;

static void restore_stdin_termios(void)
{
    tcsetattr(s_in_fd, TCSAFLUSH, &original_stdin_termios);
}

static void restore_stdout_termios(void)
{
    tcsetattr(s_out_fd, TCSAFLUSH, &original_stdout_termios);
}

void platformUartRestore(void)
{
    restore_stdin_termios();
    restore_stdout_termios();
    dup2(s_out_fd, STDOUT_FILENO);
}

otError otPlatUartEnable(void)
{
    otError        error = OT_ERROR_NONE;
    struct termios termios;

    s_in_fd  = dup(STDIN_FILENO);
    s_out_fd = dup(STDOUT_FILENO);
    dup2(STDERR_FILENO, STDOUT_FILENO);

    // We need this signal to make sure that this
    // process terminates properly.
    signal(SIGPIPE, SIG_DFL);

    if (isatty(s_in_fd))
    {
        tcgetattr(s_in_fd, &original_stdin_termios);
        atexit(&restore_stdin_termios);
    }

    if (isatty(s_out_fd))
    {
        tcgetattr(s_out_fd, &original_stdout_termios);
        atexit(&restore_stdout_termios);
    }

    if (isatty(s_in_fd))
    {
        // get current configuration
        otEXPECT_ACTION(tcgetattr(s_in_fd, &termios) == 0, perror("tcgetattr"); error = OT_ERROR_GENERIC);

        // Set up the termios settings for raw mode. This turns
        // off input/output processing, line processing, and character processing.
        cfmakeraw(&termios);

        // Set up our cflags for local use. Turn on hangup-on-close.
        termios.c_cflag |= HUPCL | CREAD | CLOCAL;

        // "Minimum number of characters for noncanonical read"
        termios.c_cc[VMIN] = 1;

        // "Timeout in deciseconds for noncanonical read"
        termios.c_cc[VTIME] = 0;

        // configure baud rate
        otEXPECT_ACTION(cfsetispeed(&termios, OPENTHREAD_SIMULATION_UART_BAUDRATE) == 0, perror("cfsetispeed");
                        error = OT_ERROR_GENERIC);

        // set configuration
        otEXPECT_ACTION(tcsetattr(s_in_fd, TCSANOW, &termios) == 0, perror("tcsetattr"); error = OT_ERROR_GENERIC);
    }

    if (isatty(s_out_fd))
    {
        // get current configuration
        otEXPECT_ACTION(tcgetattr(s_out_fd, &termios) == 0, perror("tcgetattr"); error = OT_ERROR_GENERIC);

        // Set up the termios settings for raw mode. This turns
        // off input/output processing, line processing, and character processing.
        cfmakeraw(&termios);

        // Absolutely obliterate all output processing.
        termios.c_oflag = 0;

        // Set up our cflags for local use. Turn on hangup-on-close.
        termios.c_cflag |= HUPCL | CREAD | CLOCAL;

        // configure baud rate
        otEXPECT_ACTION(cfsetospeed(&termios, OPENTHREAD_SIMULATION_UART_BAUDRATE) == 0, perror("cfsetospeed");
                        error = OT_ERROR_GENERIC);

        // set configuration
        otEXPECT_ACTION(tcsetattr(s_out_fd, TCSANOW, &termios) == 0, perror("tcsetattr"); error = OT_ERROR_GENERIC);
    }

    return error;

exit:
    close(s_in_fd);
    close(s_out_fd);
    return error;
}

otError otPlatUartDisable(void)
{
    otError error = OT_ERROR_NONE;

    close(s_in_fd);
    close(s_out_fd);

    return error;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(s_write_length == 0, error = OT_ERROR_BUSY);

    s_write_buffer = aBuf;
    s_write_length = aBufLength;

exit:
    return error;
}

void platformUartUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, fd_set *aErrorFdSet, int *aMaxFd)
{
    if (aReadFdSet != NULL)
    {
        FD_SET(s_in_fd, aReadFdSet);

        if (aErrorFdSet != NULL)
        {
            FD_SET(s_in_fd, aErrorFdSet);
        }

        if (aMaxFd != NULL && *aMaxFd < s_in_fd)
        {
            *aMaxFd = s_in_fd;
        }
    }

    if ((aWriteFdSet != NULL) && (s_write_length > 0))
    {
        FD_SET(s_out_fd, aWriteFdSet);

        if (aErrorFdSet != NULL)
        {
            FD_SET(s_out_fd, aErrorFdSet);
        }

        if (aMaxFd != NULL && *aMaxFd < s_out_fd)
        {
            *aMaxFd = s_out_fd;
        }
    }
}

otError otPlatUartFlush(void)
{
    otError error = OT_ERROR_NONE;
    ssize_t count;

    otEXPECT_ACTION(s_write_buffer != NULL && s_write_length > 0, error = OT_ERROR_INVALID_STATE);

    while ((count = write(s_out_fd, s_write_buffer, s_write_length)) > 0 && (s_write_length -= count) > 0)
    {
        s_write_buffer += count;
    }

    if (count != -1)
    {
        assert(s_write_length == 0);
        s_write_buffer = NULL;
    }
    else
    {
        perror("write(UART)");
        exit(EXIT_FAILURE);
    }

exit:
    return error;
}

void platformUartProcess(void)
{
    ssize_t       rval;
    const int     error_flags = POLLERR | POLLNVAL | POLLHUP;
    struct pollfd pollfd[]    = {
        {s_in_fd, POLLIN | error_flags, 0},
        {s_out_fd, POLLOUT | error_flags, 0},
    };

    errno = 0;

    rval = poll(pollfd, sizeof(pollfd) / sizeof(*pollfd), 0);

    if (rval < 0)
    {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    if (rval > 0)
    {
        if ((pollfd[0].revents & error_flags) != 0)
        {
            perror("s_in_fd");
            exit(EXIT_FAILURE);
        }

        if ((pollfd[1].revents & error_flags) != 0)
        {
            perror("s_out_fd");
            exit(EXIT_FAILURE);
        }

        if (pollfd[0].revents & POLLIN)
        {
            rval = read(s_in_fd, s_receive_buffer, sizeof(s_receive_buffer));

            if (rval <= 0)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            otPlatUartReceived(s_receive_buffer, (uint16_t)rval);
        }

        if ((s_write_length > 0) && (pollfd[1].revents & POLLOUT))
        {
            rval = write(s_out_fd, s_write_buffer, s_write_length);

            if (rval >= 0)
            {
                s_write_buffer += (uint16_t)rval;
                s_write_length -= (uint16_t)rval;

                if (s_write_length == 0)
                {
                    otPlatUartSendDone();
                }
            }
            else if (errno != EINTR)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    }
}
#endif // OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART == 0

#if OPENTHREAD_CONFIG_ENABLE_DEBUG_UART && (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART)

static FILE *posix_logfile;

otError otPlatDebugUart_logfile(const char *filename)
{
    posix_logfile = fopen(filename, "wt");

    return posix_logfile ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

void otPlatDebugUart_putchar_raw(int c)
{
    FILE *fp;

    /* note: log file will have a mix of cr/lf and
     * in some/many cases duplicate cr because in
     * some cases the log function {ie: Mbed} already
     * includes the CR or LF... but other log functions
     * do not include cr/lf and expect it appended
     */
    fp = posix_logfile;

    if (fp != NULL)
    {
        /* log is lost ... until a file is setup */
        fputc(c, fp);
        /* we could "fflush" but will not */
    }
}

int otPlatDebugUart_kbhit(void)
{
    /* not supported */
    return 0;
}

int otPlatDebugUart_getc(void)
{
    /* not supported */
    return -1;
}

#endif
