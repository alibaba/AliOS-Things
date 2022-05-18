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
 *   This file includes functions for the Thread Border Agent role.
 */

#ifndef OPENTHREAD_BORDER_AGENT_H_
#define OPENTHREAD_BORDER_AGENT_H_

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-border-agent
 *
 * @brief
 *   This module includes functions for the Thread Border Agent role.
 *
 * @{
 *
 */

/**
 * This enumeration defines the Border Agent state.
 *
 */
typedef enum otBorderAgentState
{
    OT_BORDER_AGENT_STATE_STOPPED = 0, ///< Border agent role is disabled.
    OT_BORDER_AGENT_STATE_STARTED = 1, ///< Border agent is started.
    OT_BORDER_AGENT_STATE_ACTIVE  = 2, ///< Border agent is connected with external commissioner.
} otBorderAgentState;

/**
 * This function gets the state of Thread Border Agent role.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 *
 * @returns State of the Border Agent.
 *
 */
otBorderAgentState otBorderAgentGetState(otInstance *aInstance);

/**
 * This function gets the UDP port of Thread Border Agent service.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 *
 * @returns UDP port of the Border Agent.
 *
 */
uint16_t otBorderAgentGetUdpPort(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OPENTHREAD_BORDER_AGENT_H_
