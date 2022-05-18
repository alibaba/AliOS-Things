/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *  This file defines the top-level ICMPv6 functions for the OpenThread library.
 */

#ifndef OPENTHREAD_ICMP6_H_
#define OPENTHREAD_ICMP6_H_

#include <openthread/ip6.h>
#include <openthread/message.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-icmp6
 *
 * @brief
 *   This module includes functions that control ICMPv6 communication.
 *
 * @{
 *
 */

/**
 * ICMPv6 Message Types
 *
 */
typedef enum otIcmp6Type
{
    OT_ICMP6_TYPE_DST_UNREACH       = 1,   ///< Destination Unreachable
    OT_ICMP6_TYPE_PACKET_TO_BIG     = 2,   ///< Packet To Big
    OT_ICMP6_TYPE_TIME_EXCEEDED     = 3,   ///< Time Exceeded
    OT_ICMP6_TYPE_PARAMETER_PROBLEM = 4,   ///< Parameter Problem
    OT_ICMP6_TYPE_ECHO_REQUEST      = 128, ///< Echo Request
    OT_ICMP6_TYPE_ECHO_REPLY        = 129, ///< Echo Reply
    OT_ICMP6_TYPE_ROUTER_SOLICIT    = 133, ///< Router Solicitation
    OT_ICMP6_TYPE_ROUTER_ADVERT     = 134, ///< Router Advertisement
} otIcmp6Type;

/**
 * ICMPv6 Message Codes
 *
 */
typedef enum otIcmp6Code
{
    OT_ICMP6_CODE_DST_UNREACH_NO_ROUTE = 0, ///< Destination Unreachable No Route
    OT_ICMP6_CODE_FRAGM_REAS_TIME_EX   = 1, ///< Fragment Reassembly Time Exceeded
} otIcmp6Code;

#define OT_ICMP6_HEADER_DATA_SIZE 4 ///< Size of an message specific data of ICMPv6 Header.

/**
 * @struct otIcmp6Header
 *
 * This structure represents an ICMPv6 header.
 *
 */
OT_TOOL_PACKED_BEGIN
struct otIcmp6Header
{
    uint8_t  mType;     ///< Type
    uint8_t  mCode;     ///< Code
    uint16_t mChecksum; ///< Checksum
    union OT_TOOL_PACKED_FIELD
    {
        uint8_t  m8[OT_ICMP6_HEADER_DATA_SIZE / sizeof(uint8_t)];
        uint16_t m16[OT_ICMP6_HEADER_DATA_SIZE / sizeof(uint16_t)];
        uint32_t m32[OT_ICMP6_HEADER_DATA_SIZE / sizeof(uint32_t)];
    } mData; ///< Message-specific data
} OT_TOOL_PACKED_END;

/**
 * This type represents an ICMPv6 header.
 *
 */
typedef struct otIcmp6Header otIcmp6Header;

/**
 * This callback allows OpenThread to inform the application of a received ICMPv6 message.
 *
 * @param[in]  aContext      A pointer to arbitrary context information.
 * @param[in]  aMessage      A pointer to the received message.
 * @param[in]  aMessageInfo  A pointer to message information associated with @p aMessage.
 * @param[in]  aIcmpHeader   A pointer to the received ICMPv6 header.
 *
 */
typedef void (*otIcmp6ReceiveCallback)(void *               aContext,
                                       otMessage *          aMessage,
                                       const otMessageInfo *aMessageInfo,
                                       const otIcmp6Header *aIcmpHeader);

/**
 * This structure implements ICMPv6 message handler.
 *
 */
typedef struct otIcmp6Handler
{
    otIcmp6ReceiveCallback mReceiveCallback; ///< The ICMPv6 received callback
    void *                 mContext;         ///< A pointer to arbitrary context information.
    struct otIcmp6Handler *mNext;            ///< A pointer to the next handler in the list.
} otIcmp6Handler;

/**
 * ICMPv6 Echo Reply Modes
 *
 */
typedef enum otIcmp6EchoMode
{
    OT_ICMP6_ECHO_HANDLER_DISABLED       = 0, ///< ICMPv6 Echo processing disabled
    OT_ICMP6_ECHO_HANDLER_UNICAST_ONLY   = 1, ///< ICMPv6 Echo processing enabled only for unicast requests only
    OT_ICMP6_ECHO_HANDLER_MULTICAST_ONLY = 2, ///< ICMPv6 Echo processing enabled only for multicast requests only
    OT_ICMP6_ECHO_HANDLER_ALL            = 3, ///< ICMPv6 Echo processing enabled for unicast and multicast requests
} otIcmp6EchoMode;

/**
 * This function indicates whether or not ICMPv6 Echo processing is enabled.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval OT_ICMP6_ECHO_HANDLER_DISABLED        ICMPv6 Echo processing is disabled.
 * @retval OT_ICMP6_ECHO_HANDLER_UNICAST_ONLY    ICMPv6 Echo processing enabled for unicast requests only
 * @retval OT_ICMP6_ECHO_HANDLER_MULTICAST_ONLY  ICMPv6 Echo processing enabled for multicast requests only
 * @retval OT_ICMP6_ECHO_HANDLER_ALL             ICMPv6 Echo processing enabled for unicast and multicast requests
 *
 */
otIcmp6EchoMode otIcmp6GetEchoMode(otInstance *aInstance);

/**
 * This function sets whether or not ICMPv6 Echo processing is enabled.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aMode     The ICMPv6 Echo processing mode.
 *
 */
void otIcmp6SetEchoMode(otInstance *aInstance, otIcmp6EchoMode aMode);

/**
 * This function registers a handler to provide received ICMPv6 messages.
 *
 * @note A handler structure @p aHandler has to be stored in persistent (static) memory.
 *       OpenThread does not make a copy of handler structure.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aHandler  A pointer to a handler containing callback that is called when
 *                       an ICMPv6 message is received.
 *
 */
otError otIcmp6RegisterHandler(otInstance *aInstance, otIcmp6Handler *aHandler);

/**
 * This function sends an ICMPv6 Echo Request via the Thread interface.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aMessage      A pointer to the message buffer containing the ICMPv6 payload.
 * @param[in]  aMessageInfo  A reference to message information associated with @p aMessage.
 * @param[in]  aIdentifier   An identifier to aid in matching Echo Replies to this Echo Request.
 *                           May be zero.
 *
 */
otError otIcmp6SendEchoRequest(otInstance *         aInstance,
                               otMessage *          aMessage,
                               const otMessageInfo *aMessageInfo,
                               uint16_t             aIdentifier);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_ICMP6_H_
