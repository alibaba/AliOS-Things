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
 *  This file defines the OpenThread Backbone Router API (Thread 1.2)
 */

#ifndef OPENTHREAD_BACKBONE_ROUTER_H_
#define OPENTHREAD_BACKBONE_ROUTER_H_

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-backbone-router
 *
 * @brief
 *  This module includes functions for the OpenThread Backbone Router Service.
 *
 * @{
 *
 */

/**
 * This structure represents Backbone Router configuration.
 *
 */
typedef struct otBackboneRouterConfig
{
    uint16_t mServer16;            ///< Only used when get Primary Backbone Router information in the Thread Network
    uint16_t mReregistrationDelay; ///< Reregistration Delay (in seconds)
    uint32_t mMlrTimeout;          ///< Multicast Listener Registration Timeout (in seconds)
    uint8_t  mSequenceNumber;      ///< Sequence Number
} otBackboneRouterConfig;

/**
 * This function gets the Primary Backbone Router information in the Thread Network.
 *
 * @param[in]   aInstance            A pointer to an OpenThread instance.
 * @param[out]  aConfig              A pointer to where to put Primary Backbone Router information.
 *
 * @retval OT_ERROR_NONE              Successfully got Primary Backbone Router information.
 * @retval OT_ERROR_NOT_FOUND         No Primary Backbone Router exists.
 *
 */
otError otBackboneRouterGetPrimary(otInstance *aInstance, otBackboneRouterConfig *aConfig);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_BACKBONE_ROUTER_H_
