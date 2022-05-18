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

#include "platform/openthread-posix-config.h"

#include "cli/cli_config.h"

#include <openthread/platform/toolchain.h>

#ifndef HAVE_LIBEDIT
#define HAVE_LIBEDIT 0
#endif

#ifndef HAVE_LIBREADLINE
#define HAVE_LIBREADLINE 0
#endif

#define OPENTHREAD_USE_READLINE (HAVE_LIBEDIT || HAVE_LIBREADLINE)

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#if HAVE_LIBEDIT
#include <editline/readline.h>
#elif HAVE_LIBREADLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif

#include "common/code_utils.hpp"

#include "platform-posix.h"

namespace {

struct Config
{
    const char *mNetifName;
};

enum
{
    kLineBufferSize = OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH,
};

static_assert(kLineBufferSize >= sizeof("> "), "kLineBufferSize is too small");
static_assert(kLineBufferSize >= sizeof("Done\r\n"), "kLineBufferSize is too small");
static_assert(kLineBufferSize >= sizeof("Error "), "kLineBufferSize is too small");

int sSessionFd = -1;

void QuitOnExit(const char *aBuffer)
{
    constexpr char kExit[] = "exit";

    while (*aBuffer == ' ' || *aBuffer == '\t')
    {
        ++aBuffer;
    }

    VerifyOrExit(strstr(aBuffer, kExit) == aBuffer);

    aBuffer += sizeof(kExit) - 1;

    while (*aBuffer == ' ' || *aBuffer == '\t')
    {
        ++aBuffer;
    }

    switch (*aBuffer)
    {
    case '\0':
    case '\r':
    case '\n':
        exit(OT_EXIT_SUCCESS);
        break;
    default:
        break;
    }

exit:
    return;
}

#if OPENTHREAD_USE_READLINE
void InputCallback(char *aLine)
{
    if (aLine != nullptr)
    {
        QuitOnExit(aLine);
        add_history(aLine);
        dprintf(sSessionFd, "%s\n", aLine);
        free(aLine);
    }
    else
    {
        exit(OT_EXIT_SUCCESS);
    }
}
#endif // OPENTHREAD_USE_READLINE

bool DoWrite(int aFile, const void *aBuffer, size_t aSize)
{
    bool ret = true;

    while (aSize)
    {
        ssize_t rval = write(aFile, aBuffer, aSize);
        if (rval <= 0)
        {
            VerifyOrExit((rval == -1) && (errno == EINTR), perror("write"); ret = false);
        }
        else
        {
            aBuffer = reinterpret_cast<const uint8_t *>(aBuffer) + rval;
            aSize -= static_cast<size_t>(rval);
        }
    }

exit:
    return ret;
}

int ConnectSession(const Config &aConfig)
{
    int ret;

    if (sSessionFd != -1)
    {
        close(sSessionFd);
    }

    sSessionFd = socket(AF_UNIX, SOCK_STREAM, 0);
    VerifyOrExit(sSessionFd != -1, ret = -1);

    {
        struct sockaddr_un sockname;

        memset(&sockname, 0, sizeof(struct sockaddr_un));
        sockname.sun_family = AF_UNIX;
        ret = snprintf(sockname.sun_path, sizeof(sockname.sun_path), OPENTHREAD_POSIX_DAEMON_SOCKET_NAME,
                       aConfig.mNetifName);
        VerifyOrExit(ret >= 0 && static_cast<size_t>(ret) < sizeof(sockname.sun_path), {
            errno = EINVAL;
            ret   = -1;
        });

        ret = connect(sSessionFd, reinterpret_cast<const struct sockaddr *>(&sockname), sizeof(struct sockaddr_un));
    }

exit:
    return ret;
}

bool ReconnectSession(Config &aConfig)
{
    bool     ok    = false;
    uint32_t delay = 0; // 100ms

    for (int i = 0; i < 6; i++) // delay for 3.1s in total
    {
        int rval;

        usleep(delay);
        delay = delay > 0 ? delay * 2 : 100000;

        rval = ConnectSession(aConfig);

        VerifyOrExit(rval == -1, ok = true);

        // Exit immediately if the sock file is not found
        VerifyOrExit(errno != ENOENT);
    }

exit:
    return ok;
}

enum
{
    kOptInterfaceName = 'I',
    kOptHelp          = 'h',
};

const struct option kOptions[] = {
    {"interface-name", required_argument, NULL, kOptInterfaceName},
    {"help", required_argument, NULL, kOptHelp},
};

void PrintUsage(const char *aProgramName, FILE *aStream, int aExitCode)
{
    fprintf(aStream,
            "Syntax:\n"
            "    %s [Options] [--] ...\n"
            "Options:\n"
            "    -h  --help                    Display this usage information.\n"
            "    -I  --interface-name name     Thread network interface name.\n",
            aProgramName);
    exit(aExitCode);
}

static bool ShouldEscape(char aChar)
{
    return (aChar == ' ') || (aChar == '\t') || (aChar == '\r') || (aChar == '\n') || (aChar == '\\');
}

Config ParseArg(int &aArgCount, char **&aArgVector)
{
    Config config = {"wpan0"};

    optind = 1;

    for (int index, option; (option = getopt_long(aArgCount, aArgVector, "+I:h", kOptions, &index)) != -1;)
    {
        switch (option)
        {
        case kOptInterfaceName:
            config.mNetifName = optarg;
            break;
        case kOptHelp:
            PrintUsage(aArgVector[0], stdout, OT_EXIT_SUCCESS);
            break;
        default:
            PrintUsage(aArgVector[0], stderr, OT_EXIT_FAILURE);
            break;
        }
    }

    aArgCount -= optind;
    aArgVector += optind;

    return config;
}

} // namespace

int main(int argc, char *argv[])
{
    bool   isInteractive = true;
    bool   isFinished    = false;
    bool   isBeginOfLine = true;
    char   lineBuffer[kLineBufferSize];
    size_t lineBufferWritePos = 0;
    int    ret;
    Config config;

    config = ParseArg(argc, argv);

    VerifyOrExit(ConnectSession(config) != -1, perror("connect session failed"); ret = OT_EXIT_FAILURE);

    if (argc > 0)
    {
        char   buffer[kLineBufferSize];
        size_t count = 0;

        for (int i = 0; i < argc; i++)
        {
            for (const char *c = argv[i]; *c && count < sizeof(buffer);)
            {
                if (ShouldEscape(*c))
                {
                    buffer[count++] = '\\';

                    VerifyOrExit(count < sizeof(buffer), ret = OT_EXIT_INVALID_ARGUMENTS);
                }

                buffer[count++] = *c++;
            }

            VerifyOrExit(count < sizeof(buffer), ret = OT_EXIT_INVALID_ARGUMENTS);
            buffer[count++] = ' ';
        }

        // ignore the trailing space
        if (--count)
        {
            VerifyOrExit(DoWrite(sSessionFd, buffer, count), ret = OT_EXIT_FAILURE);
        }

        isInteractive = false;
    }
#if OPENTHREAD_USE_READLINE
    else
    {
        rl_instream           = stdin;
        rl_outstream          = stdout;
        rl_inhibit_completion = true;
        rl_callback_handler_install("> ", InputCallback);
        rl_already_prompted = 1;
    }
#endif

    while (!isFinished)
    {
        char   buffer[kLineBufferSize];
        fd_set readFdSet;
        int    maxFd = sSessionFd;

        FD_ZERO(&readFdSet);

        FD_SET(sSessionFd, &readFdSet);

        if (isInteractive)
        {
            FD_SET(STDIN_FILENO, &readFdSet);
            if (STDIN_FILENO > maxFd)
            {
                maxFd = STDIN_FILENO;
            }
        }

        ret = select(maxFd + 1, &readFdSet, nullptr, nullptr, nullptr);

        VerifyOrExit(ret != -1, perror("select"); ret = OT_EXIT_FAILURE);

        if (ret == 0)
        {
            ExitNow(ret = OT_EXIT_SUCCESS);
        }

        if (isInteractive && FD_ISSET(STDIN_FILENO, &readFdSet))
        {
#if OPENTHREAD_USE_READLINE
            rl_callback_read_char();
#else
            VerifyOrExit(fgets(buffer, sizeof(buffer), stdin) != nullptr, ret = OT_EXIT_FAILURE);

            QuitOnExit(buffer);
            VerifyOrExit(DoWrite(sSessionFd, buffer, strlen(buffer)), ret = OT_EXIT_FAILURE);
#endif
        }

        if (FD_ISSET(sSessionFd, &readFdSet))
        {
            ssize_t rval = read(sSessionFd, buffer, sizeof(buffer));
            VerifyOrExit(rval != -1, perror("read"); ret = OT_EXIT_FAILURE);

            if (rval == 0)
            {
                // daemon closed sSessionFd
                if (isInteractive && ReconnectSession(config))
                {
                    continue;
                }

                ExitNow(ret = isInteractive ? OT_EXIT_FAILURE : OT_EXIT_SUCCESS);
            }

            if (isInteractive)
            {
                VerifyOrExit(DoWrite(STDOUT_FILENO, buffer, static_cast<size_t>(rval)), ret = OT_EXIT_FAILURE);
            }
            else
            {
                for (ssize_t i = 0; i < rval; i++)
                {
                    char c = buffer[i];

                    lineBuffer[lineBufferWritePos++] = c;
                    if (c == '\n' || lineBufferWritePos >= sizeof(lineBuffer) - 1)
                    {
                        char * line = lineBuffer;
                        size_t len  = lineBufferWritePos;

                        // read one line successfully or line buffer is full
                        line[len] = '\0';

                        if (isBeginOfLine && strncmp("> ", lineBuffer, 2) == 0)
                        {
                            line += 2;
                            len -= 2;
                        }

                        VerifyOrExit(DoWrite(STDOUT_FILENO, line, len), ret = OT_EXIT_FAILURE);

                        if (isBeginOfLine && (strncmp("Done\n", line, 5) == 0 || strncmp("Done\r\n", line, 6) == 0 ||
                                              strncmp("Error ", line, 6) == 0))
                        {
                            isFinished = true;
                            ret        = OT_EXIT_SUCCESS;
                            break;
                        }

                        // reset for next line
                        lineBufferWritePos = 0;
                        isBeginOfLine      = c == '\n';
                    }
                }
            }
        }
    }

exit:
    if (sSessionFd != -1)
    {
#if OPENTHREAD_USE_READLINE
        if (isInteractive)
        {
            rl_callback_handler_remove();
        }
#endif
        close(sSessionFd);
    }

    return ret;
}
