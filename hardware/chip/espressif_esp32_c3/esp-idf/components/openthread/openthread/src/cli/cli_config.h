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

/**
 * @file
 *   This file includes compile-time configurations for the CLI service.
 *
 */

#ifndef CONFIG_CLI_H_
#define CONFIG_CLI_H_

#include "openthread-core-config.h"

#include <openthread/tcp.h>

#ifndef OPENTHREAD_POSIX
#if defined(__ANDROID__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__linux__) || defined(__NetBSD__) || \
    defined(__unix__)
#define OPENTHREAD_POSIX 1
#else
#define OPENTHREAD_POSIX 0
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH
 *
 * The maximum size of the CLI line in bytes including the null terminator.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH 384
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_TCP_ENABLE
 *
 * Indicates whether TCP should be enabled in the CLI tool.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_TCP_ENABLE
#define OPENTHREAD_CONFIG_CLI_TCP_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_TCP_DEFAULT_BENCHMARK_SIZE
 *
 * The number of bytes to transfer for the TCP benchmark in the CLI.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_TCP_DEFAULT_BENCHMARK_SIZE
#define OPENTHREAD_CONFIG_CLI_TCP_DEFAULT_BENCHMARK_SIZE (72 << 10)
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_TCP_RECEIVE_BUFFER_SIZE
 *
 * The size of memory used for the TCP receive buffer, in bytes.
 */
#ifndef OPENTHREAD_CONFIG_CLI_TCP_RECEIVE_BUFFER_SIZE
#define OPENTHREAD_CONFIG_CLI_TCP_RECEIVE_BUFFER_SIZE OT_TCP_RECEIVE_BUFFER_SIZE_FEW_HOPS
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
 *
 * Define as 1 for CLI to emit its command input string and the resulting output to the logs.
 *
 * By default this is enabled on any POSIX based platform (`OPENTHREAD_POSIX`) and only when CLI itself is not being
 * used for logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE
#define OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE \
    (OPENTHREAD_POSIX && (OPENTHREAD_CONFIG_LOG_OUTPUT != OPENTHREAD_CONFIG_LOG_OUTPUT_APP))
#endif

/**
 * @def OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_LOG_STRING_SIZE
 *
 * The log string buffer size (in bytes).
 *
 * This is only used when `OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_ENABLE` is enabled.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_LOG_STRING_SIZE
#define OPENTHREAD_CONFIG_CLI_LOG_INPUT_OUTPUT_LOG_STRING_SIZE OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH
#endif

#endif // CONFIG_CLI_H_
