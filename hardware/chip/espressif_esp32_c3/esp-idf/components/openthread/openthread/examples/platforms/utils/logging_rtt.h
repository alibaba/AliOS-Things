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

/**
 * @file
 * @brief
 *   This file defines the logging rtt interfaces and default constants used by logging_rtt.c.
 */

#ifndef UTILS_LOGGING_RTT_H
#define UTILS_LOGGING_RTT_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "openthread-core-config.h"
#include <openthread/config.h>
#include <openthread/platform/logging.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def LOG_RTT_BUFFER_INDEX
 *
 * RTT's buffer index.
 *
 */
#ifndef LOG_RTT_BUFFER_INDEX
#define LOG_RTT_BUFFER_INDEX 0
#endif

/**
 * @def LOG_RTT_BUFFER_NAME
 *
 * RTT's name. Only used if LOG_RTT_BUFFER_INDEX is not 0. Otherwise,
 * the buffer name is fixed to "Terminal".
 *
 */
#ifndef LOG_RTT_BUFFER_NAME
#define LOG_RTT_BUFFER_NAME "Terminal"
#endif

/**
 * @def LOG_RTT_BUFFER_SIZE
 *
 * LOG RTT's buffer size. Only used if LOG_RTT_BUFFER_INDEX is not 0. To
 * configure buffer #0 size, check the BUFFER_SIZE_UP definition in
 * SEGGER_RTT_Conf.h
 *
 */
#ifndef LOG_RTT_BUFFER_SIZE
#define LOG_RTT_BUFFER_SIZE 256
#endif

/**
 * @def LOG_RTT_COLOR_ENABLE
 *
 * Enable colors on RTT Viewer.
 *
 */
#ifndef LOG_RTT_COLOR_ENABLE
#define LOG_RTT_COLOR_ENABLE 1
#endif

/**
 * @def LOG_PARSE_BUFFER_SIZE
 *
 * LOG buffer used to parse print format. It will be locally allocated on the
 * stack.
 *
 */
#ifndef LOG_PARSE_BUFFER_SIZE
#define LOG_PARSE_BUFFER_SIZE 128
#endif

/**
 * @def LOG_TIMESTAMP_ENABLE
 *
 * Enable timestamp in the logs.
 *
 */
#ifndef LOG_TIMESTAMP_ENABLE
#define LOG_TIMESTAMP_ENABLE 1
#endif

/**
 * Initialization of Logger driver.
 *
 */
void utilsLogRttInit(void);

/**
 * Deinitialization of Logger driver.
 *
 */
void utilsLogRttDeinit(void);

/**
 * This function outputs logs to SEGGER RTT.
 *
 * @param[in]  aLogLevel   The log level.
 * @param[in]  aLogRegion  The log region.
 * @param[in]  aFormat     A pointer to the format string.
 * @param[in]  ap          va_list matching information for aFormat
 *
 */
void utilsLogRttOutput(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, va_list ap);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // UTILS_LOGGING_RTT_H
