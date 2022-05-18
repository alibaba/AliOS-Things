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
 * @brief
 *   This file includes the abstraction for the platform OTNS utilities.
 *
 *   OTNS is an OpenThread Network Simulator that simulates Thread networks using OpenThread simulation instances and
 *   provides visualization and management of those simulated networks.
 *   Refer to https://github.com/openthread/ot-ns for more information about OTNS.
 */

#ifndef OPENTHREAD_PLATFORM_OTNS_H_
#define OPENTHREAD_PLATFORM_OTNS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-otns
 *
 * @brief
 *   This module includes the platform abstraction for OTNS.
 *
 * @{
 *
 */

/**
 * This function exports status information to OTNS.
 *
 * The status information is represented by a null-terminated string with format recognizable by OTNS.
 * Each call to `otPlatOtnsStatus` can send multiple statuses, separated by ';', e.x. "parid=577fbc37;lrid=5".
 * Each status contains key and value separated by '='.
 * Status value can be further separated into multiple fields using ',',
 * e.x. "ping_request=fdde:ad00:beef:0:459e:d7b4:b65e:5480,4,112000".
 *
 * New statuses should follow these conventions.
 *
 * Currently, OTNS only supports virtual time simulation.
 *
 * @param[in]  aStatus  The status string.
 *
 */
void otPlatOtnsStatus(const char *aStatus);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_OTNS_H_
