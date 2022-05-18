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
 *   This file includes compile-time configurations for CoAP.
 *
 */

#ifndef CONFIG_COAP_H_
#define CONFIG_COAP_H_

/**
 * @def OPENTHREAD_CONFIG_COAP_SERVER_MAX_CACHED_RESPONSES
 *
 * Maximum number of cached responses for CoAP Confirmable messages.
 *
 * Cached responses are used for message deduplication.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_SERVER_MAX_CACHED_RESPONSES
#define OPENTHREAD_CONFIG_COAP_SERVER_MAX_CACHED_RESPONSES 10
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_API_ENABLE
 *
 * Define to 1 to enable the CoAP API.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_API_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
 *
 * Define to 1 to enable the CoAP Observe (RFC7641) API.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
 *
 * Define to 1 to enable the CoAP Block-Wise Transfer.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
#define OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_MAX_BLOCK_LENGTH
 *
 * This setting configures the maximum length of one block during a CoAP Block-Wise Transfer.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_MAX_BLOCK_LENGTH
#define OPENTHREAD_CONFIG_COAP_MAX_BLOCK_LENGTH 1024
#endif

/**
 * @def OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
 *
 * Define to 1 to enable the CoAP Secure API.
 *
 */
#ifndef OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
#define OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE 0
#endif

#endif // CONFIG_COAP_H_
