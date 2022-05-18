/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file contains header for exit code utilities.
 */

#ifndef PLATFORM_EXIT_CODE_H_
#define PLATFORM_EXIT_CODE_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This enumeration represents exit codes used when OpenThread exits.
 *
 */
enum
{
    /**
     * Success.
     */
    OT_EXIT_SUCCESS = 0,

    /**
     * Generic failure.
     */
    OT_EXIT_FAILURE = 1,

    /**
     * Invalid arguments.
     */
    OT_EXIT_INVALID_ARGUMENTS = 2,

    /**
     * Incompatible radio spinel.
     */
    OT_EXIT_RADIO_SPINEL_INCOMPATIBLE = 3,

    /**
     * Unexpected radio spinel reset.
     */
    OT_EXIT_RADIO_SPINEL_RESET = 4,

    /**
     * System call or library function error.
     */
    OT_EXIT_ERROR_ERRNO = 5,

    /**
     * No response from radio spinel.
     */
    OT_EXIT_RADIO_SPINEL_NO_RESPONSE = 6,
};

/**
 * This function converts an exit code into a string.
 *
 * @param[in]  aExitCode  An exit code.
 *
 * @returns  A string representation of an exit code.
 *
 */
const char *otExitCodeToString(uint8_t aExitCode);

/**
 * This macro checks for the specified condition, which is expected to commonly be true,
 * and both records exit status and terminates the program if the condition is false.
 *
 * @param[in]   aCondition  The condition to verify
 * @param[in]   aExitCode   The exit code.
 *
 */
#define VerifyOrDie(aCondition, aExitCode)                                                                   \
    do                                                                                                       \
    {                                                                                                        \
        if (!(aCondition))                                                                                   \
        {                                                                                                    \
            otLogCritPlat("%s() at %s:%d: %s", __func__, __FILE__, __LINE__, otExitCodeToString(aExitCode)); \
            exit(aExitCode);                                                                                 \
        }                                                                                                    \
    } while (false)

/**
 * This macro checks for the specified error code, which is expected to commonly be successful,
 * and both records exit status and terminates the program if the error code is unsuccessful.
 *
 * @param[in]  aError  An error code to be evaluated against OT_ERROR_NONE.
 *
 */
#define SuccessOrDie(aError)             \
    VerifyOrDie(aError == OT_ERROR_NONE, \
                (aError == OT_ERROR_INVALID_ARGS ? OT_EXIT_INVALID_ARGUMENTS : OT_EXIT_FAILURE))

/**
 * This macro unconditionally both records exit status and terminates the program.
 *
 * @param[in]   aExitCode   The exit code.
 *
 */
#define DieNow(aExitCode) VerifyOrDie(false, aExitCode)

/**
 * This macro unconditionally both records exit status and exit message and terminates the program.
 *
 * @param[in]   aMessage    The exit message.
 * @param[in]   aExitCode   The exit code.
 *
 */
#define DieNowWithMessage(aMessage, aExitCode)                                                 \
    do                                                                                         \
    {                                                                                          \
        otLogCritPlat("exit(%d): %s line %d, %s, %s", aExitCode, __func__, __LINE__, aMessage, \
                      otExitCodeToString(aExitCode));                                          \
        exit(aExitCode);                                                                       \
    } while (false)

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_EXIT_CODE_H_
