/*
 *    Copyright (c) 2020, The OpenThread Authors.
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

/**
 * @file
 *   This file implements exit code utilities.
 */

#include "exit_code.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

const char *otExitCodeToString(uint8_t aExitCode)
{
    const char *retval = NULL;

    switch (aExitCode)
    {
    case OT_EXIT_SUCCESS:
        retval = "Success";
        break;

    case OT_EXIT_FAILURE:
        retval = "Failure";
        break;

    case OT_EXIT_INVALID_ARGUMENTS:
        retval = "InvalidArgument";
        break;

    case OT_EXIT_RADIO_SPINEL_INCOMPATIBLE:
        retval = "RadioSpinelIncompatible";
        break;

    case OT_EXIT_RADIO_SPINEL_RESET:
        retval = "RadioSpinelReset";
        break;

    case OT_EXIT_RADIO_SPINEL_NO_RESPONSE:
        retval = "RadioSpinelNoResponse";
        break;

    case OT_EXIT_ERROR_ERRNO:
#ifdef errno
        retval = strerror(errno);
#else
        retval = "ErrorNo";
#endif
        break;

    default:
        assert(false);
        retval = "UnknownExitCode";
        break;
    }

    return retval;
}
