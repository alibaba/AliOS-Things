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

#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE

#include <openthread/config.h>
#include <openthread/openthread-system.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if defined(HAVE_LIBEDIT) || defined(HAVE_LIBREADLINE)

#if defined(HAVE_LIBEDIT)
#include <editline/readline.h>
#elif defined(HAVE_LIBREADLINE)
#include <readline/history.h>
#include <readline/readline.h>
#endif

#include <openthread/cli.h>

#include "cli/cli_config.h"
#include "common/code_utils.hpp"

#include "openthread-core-config.h"
#include "platform-posix.h"

static void InputCallback(char *aLine)
{
    if (aLine != nullptr)
    {
        if (aLine[0] != '\0')
        {
            add_history(aLine);
        }
        otCliInputLine(aLine);
        free(aLine);
    }
    else
    {
        exit(OT_EXIT_SUCCESS);
    }
}

static int OutputCallback(void *aContext, const char *aFormat, va_list aArguments)
{
    OT_UNUSED_VARIABLE(aContext);

    return vdprintf(STDOUT_FILENO, aFormat, aArguments);
}

extern "C" void otAppCliInit(otInstance *aInstance)
{
    rl_instream           = stdin;
    rl_outstream          = stdout;
    rl_inhibit_completion = true;

    rl_set_screen_size(0, OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH);

    rl_callback_handler_install("", InputCallback);
    rl_already_prompted = true;

    otCliInit(aInstance, OutputCallback, nullptr);
}

extern "C" void otAppCliDeinit(void)
{
    rl_callback_handler_remove();
}

extern "C" void otAppCliUpdate(otSysMainloopContext *aMainloop)
{
    FD_SET(STDIN_FILENO, &aMainloop->mReadFdSet);
    FD_SET(STDIN_FILENO, &aMainloop->mErrorFdSet);

    if (aMainloop->mMaxFd < STDIN_FILENO)
    {
        aMainloop->mMaxFd = STDIN_FILENO;
    }
}

extern "C" void otAppCliProcess(const otSysMainloopContext *aMainloop)
{
    if (FD_ISSET(STDIN_FILENO, &aMainloop->mErrorFdSet))
    {
        exit(OT_EXIT_FAILURE);
    }

    if (FD_ISSET(STDIN_FILENO, &aMainloop->mReadFdSet))
    {
        rl_callback_read_char();
    }
}

#endif // defined(HAVE_LIBEDIT) || defined(HAVE_LIBREADLINE)
#endif // !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
