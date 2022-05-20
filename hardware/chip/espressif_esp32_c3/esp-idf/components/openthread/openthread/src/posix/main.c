/*
 *  Copyright (c) 2018, The OpenThread Authors.
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

#include <openthread/config.h>

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/prctl.h>
#endif

#ifndef HAVE_LIBEDIT
#define HAVE_LIBEDIT 0
#endif

#ifndef HAVE_LIBREADLINE
#define HAVE_LIBREADLINE 0
#endif

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/logging.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/platform/radio.h>
#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
#include <openthread/cli.h>
#include "cli/cli_config.h"
#endif
#include <common/code_utils.hpp>
#include <common/logging.hpp>
#include <lib/platform/exit_code.h>
#include <openthread/openthread-system.h>
#include <openthread/platform/misc.h>

#ifndef OPENTHREAD_ENABLE_COVERAGE
#define OPENTHREAD_ENABLE_COVERAGE 0
#endif

/**
 * This function initializes NCP app.
 *
 * @param[in]  aInstance    A pointer to the OpenThread instance.
 *
 */
void otAppNcpInit(otInstance *aInstance);

/**
 * This function deinitializes NCP app.
 *
 */
void otAppNcpUpdate(otSysMainloopContext *aContext);

/**
 * This function updates the file descriptor sets with file descriptors used by console.
 *
 * @param[inout]    aMainloop   A pointer to the mainloop context.
 *
 */
void otAppNcpProcess(const otSysMainloopContext *aContext);

/**
 * This function initializes CLI app.
 *
 * @param[in]  aInstance    A pointer to the OpenThread instance.
 *
 */
void otAppCliInit(otInstance *aInstance);

/**
 * This function deinitializes CLI app.
 *
 */
void otAppCliDeinit(void);

/**
 * This function updates the file descriptor sets with file descriptors used by console.
 *
 * @param[inout]    aMainloop   A pointer to the mainloop context.
 *
 */
void otAppCliUpdate(otSysMainloopContext *aMainloop);

/**
 * This function performs console driver processing.
 *
 * @param[in]    aMainloop      A pointer to the mainloop context.
 *
 */
void otAppCliProcess(const otSysMainloopContext *aMainloop);

typedef struct PosixConfig
{
    otPlatformConfig mPlatformConfig;    ///< Platform configuration.
    otLogLevel       mLogLevel;          ///< Debug level of logging.
    bool             mIsDryRun;          ///< Dry run.
    bool             mPrintRadioVersion; ///< Whether to print radio firmware version.
    bool             mIsVerbose;         ///< Whether to print log to stderr.
} PosixConfig;

static jmp_buf gResetJump;

void __gcov_flush();

/**
 * This enumeration defines the argument return values.
 *
 */
enum
{
    OT_POSIX_OPT_BACKBONE_INTERFACE_NAME = 'B',
    OT_POSIX_OPT_DEBUG_LEVEL             = 'd',
    OT_POSIX_OPT_DRY_RUN                 = 'n',
    OT_POSIX_OPT_HELP                    = 'h',
    OT_POSIX_OPT_INTERFACE_NAME          = 'I',
    OT_POSIX_OPT_TIME_SPEED              = 's',
    OT_POSIX_OPT_VERBOSE                 = 'v',

    OT_POSIX_OPT_SHORT_MAX = 128,

    OT_POSIX_OPT_RADIO_VERSION,
    OT_POSIX_OPT_REAL_TIME_SIGNAL,
};

static const struct option kOptions[] = {
    {"backbone-interface-name", required_argument, NULL, OT_POSIX_OPT_BACKBONE_INTERFACE_NAME},
    {"debug-level", required_argument, NULL, OT_POSIX_OPT_DEBUG_LEVEL},
    {"dry-run", no_argument, NULL, OT_POSIX_OPT_DRY_RUN},
    {"help", no_argument, NULL, OT_POSIX_OPT_HELP},
    {"interface-name", required_argument, NULL, OT_POSIX_OPT_INTERFACE_NAME},
    {"radio-version", no_argument, NULL, OT_POSIX_OPT_RADIO_VERSION},
    {"real-time-signal", required_argument, NULL, OT_POSIX_OPT_REAL_TIME_SIGNAL},
    {"time-speed", required_argument, NULL, OT_POSIX_OPT_TIME_SPEED},
    {"verbose", no_argument, NULL, OT_POSIX_OPT_VERBOSE},
    {0, 0, 0, 0}};

static void PrintUsage(const char *aProgramName, FILE *aStream, int aExitCode)
{
    fprintf(aStream,
            "Syntax:\n"
            "    %s [Options] RadioURL [RadioURL]\n"
            "Options:\n"
            "    -B  --backbone-interface-name Backbone network interface name.\n"
            "    -d  --debug-level             Debug level of logging.\n"
            "    -h  --help                    Display this usage information.\n"
            "    -I  --interface-name name     Thread network interface name.\n"
            "    -n  --dry-run                 Just verify if arguments is valid and radio spinel is compatible.\n"
            "        --radio-version           Print radio firmware version.\n"
            "    -s  --time-speed factor       Time speed up factor.\n"
            "    -v  --verbose                 Also log to stderr.\n",
            aProgramName);
#ifdef __linux__
    fprintf(aStream,
            "        --real-time-signal        (Linux only) The real-time signal number for microsecond timer.\n"
            "                                  Use +N for relative value to SIGRTMIN, and use N for absolute value.\n");

#endif
    fprintf(aStream, "%s", otSysGetRadioUrlHelpString());
    exit(aExitCode);
}

static void ParseArg(int aArgCount, char *aArgVector[], PosixConfig *aConfig)
{
    memset(aConfig, 0, sizeof(*aConfig));

    aConfig->mPlatformConfig.mSpeedUpFactor = 1;
    aConfig->mLogLevel                      = OT_LOG_LEVEL_CRIT;
#ifdef __linux__
    aConfig->mPlatformConfig.mRealTimeSignal = SIGRTMIN;
#endif

    optind = 1;

    while (true)
    {
        int index  = 0;
        int option = getopt_long(aArgCount, aArgVector, "B:d:hI:ns:v", kOptions, &index);

        if (option == -1)
        {
            break;
        }

        switch (option)
        {
        case OT_POSIX_OPT_DEBUG_LEVEL:
            aConfig->mLogLevel = (otLogLevel)atoi(optarg);
            break;
        case OT_POSIX_OPT_HELP:
            PrintUsage(aArgVector[0], stdout, OT_EXIT_SUCCESS);
            break;
        case OT_POSIX_OPT_INTERFACE_NAME:
            aConfig->mPlatformConfig.mInterfaceName = optarg;
            break;
        case OT_POSIX_OPT_BACKBONE_INTERFACE_NAME:
            aConfig->mPlatformConfig.mBackboneInterfaceName = optarg;
            break;
        case OT_POSIX_OPT_DRY_RUN:
            aConfig->mIsDryRun = true;
            break;
        case OT_POSIX_OPT_TIME_SPEED:
        {
            char *endptr = NULL;

            aConfig->mPlatformConfig.mSpeedUpFactor = (uint32_t)strtol(optarg, &endptr, 0);

            if (*endptr != '\0' || aConfig->mPlatformConfig.mSpeedUpFactor == 0)
            {
                fprintf(stderr, "Invalid value for TimerSpeedUpFactor: %s\n", optarg);
                exit(OT_EXIT_INVALID_ARGUMENTS);
            }
            break;
        }
        case OT_POSIX_OPT_VERBOSE:
            aConfig->mIsVerbose = true;
            break;
        case OT_POSIX_OPT_RADIO_VERSION:
            aConfig->mPrintRadioVersion = true;
            break;
#ifdef __linux__
        case OT_POSIX_OPT_REAL_TIME_SIGNAL:
            if (optarg[0] == '+')
            {
                aConfig->mPlatformConfig.mRealTimeSignal = SIGRTMIN + atoi(&optarg[1]);
            }
            else
            {
                aConfig->mPlatformConfig.mRealTimeSignal = atoi(optarg);
            }
            break;
#endif // __linux__
        case '?':
            PrintUsage(aArgVector[0], stderr, OT_EXIT_INVALID_ARGUMENTS);
            break;
        default:
            assert(false);
            break;
        }
    }

    for (; optind < aArgCount; optind++)
    {
        VerifyOrDie(aConfig->mPlatformConfig.mRadioUrlNum < OT_ARRAY_LENGTH(aConfig->mPlatformConfig.mRadioUrls),
                    OT_EXIT_INVALID_ARGUMENTS);
        aConfig->mPlatformConfig.mRadioUrls[aConfig->mPlatformConfig.mRadioUrlNum++] = aArgVector[optind];
    }

    if (aConfig->mPlatformConfig.mRadioUrlNum == 0)
    {
        PrintUsage(aArgVector[0], stderr, OT_EXIT_INVALID_ARGUMENTS);
    }
}

static otInstance *InitInstance(PosixConfig *aConfig)
{
    otInstance *instance = NULL;

    syslog(LOG_INFO, "Running %s", otGetVersionString());
    syslog(LOG_INFO, "Thread version: %hu", otThreadGetVersion());
    IgnoreError(otLoggingSetLevel(aConfig->mLogLevel));

    instance = otSysInit(&aConfig->mPlatformConfig);
    VerifyOrDie(instance != NULL, OT_EXIT_FAILURE);
    syslog(LOG_INFO, "Thread interface: %s", otSysGetThreadNetifName());

    if (aConfig->mPrintRadioVersion)
    {
        printf("%s\n", otPlatRadioGetVersionString(instance));
    }
    else
    {
        syslog(LOG_INFO, "RCP version: %s", otPlatRadioGetVersionString(instance));
    }

    if (aConfig->mIsDryRun)
    {
        exit(OT_EXIT_SUCCESS);
    }

    return instance;
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatReset(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    gPlatResetReason = OT_PLAT_RESET_REASON_SOFTWARE;

    otSysDeinit();

    longjmp(gResetJump, 1);
    assert(false);
}

static void ProcessNetif(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aArgsLength);
    OT_UNUSED_VARIABLE(aArgs);

    otCliOutputFormat("%s:%u\r\n", otSysGetThreadNetifName(), otSysGetThreadNetifIndex());
}

#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
static void ProcessExit(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aArgsLength);
    OT_UNUSED_VARIABLE(aArgs);

    exit(EXIT_SUCCESS);
}
#endif

static const otCliCommand kCommands[] = {
#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
    {"exit", ProcessExit},
#endif
    {"netif", ProcessNetif},
};

int main(int argc, char *argv[])
{
    otInstance *instance;
    int         rval = 0;
    PosixConfig config;

#ifdef __linux__
    // Ensure we terminate this process if the
    // parent process dies.
    prctl(PR_SET_PDEATHSIG, SIGHUP);
#endif

    if (setjmp(gResetJump))
    {
        alarm(0);
#if OPENTHREAD_ENABLE_COVERAGE
        __gcov_flush();
#endif
        execvp(argv[0], argv);
    }

    ParseArg(argc, argv, &config);
    openlog(argv[0], LOG_PID | (config.mIsVerbose ? LOG_PERROR : 0), LOG_DAEMON);
    setlogmask(setlogmask(0) & LOG_UPTO(LOG_DEBUG));
    instance = InitInstance(&config);

#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
    otAppCliInit(instance);
#endif
    otCliSetUserCommands(kCommands, OT_ARRAY_LENGTH(kCommands), instance);

    while (true)
    {
        otSysMainloopContext mainloop;

        otTaskletsProcess(instance);

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        mainloop.mMaxFd           = -1;
        mainloop.mTimeout.tv_sec  = 10;
        mainloop.mTimeout.tv_usec = 0;

#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
        otAppCliUpdate(&mainloop);
#endif

        otSysMainloopUpdate(instance, &mainloop);

        if (otSysMainloopPoll(&mainloop) >= 0)
        {
            otSysMainloopProcess(instance, &mainloop);
#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
            otAppCliProcess(&mainloop);
#endif
        }
        else if (errno != EINTR)
        {
            perror("select");
            ExitNow(rval = OT_EXIT_FAILURE);
        }
    }

#if !OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
    otAppCliDeinit();
#endif

exit:
    otSysDeinit();

    return rval;
}
