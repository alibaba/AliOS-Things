/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes the OpenThread API for ping sender module.
 */

#ifndef OPENTHREAD_PING_SENDER_H_
#define OPENTHREAD_PING_SENDER_H_

#include <stdint.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-ping-sender
 *
 * @brief
 *   This file includes the OpenThread API for the ping sender module.
 *
 * @{
 *
 */

/**
 * This structure represents a ping reply.
 *
 */
typedef struct otPingSenderReply
{
    otIp6Address mSenderAddress;  ///< Sender IPv6 address (address from which ping reply was received).
    uint16_t     mRoundTripTime;  ///< Round trip time in msec.
    uint16_t     mSize;           ///< Data size (number of bytes) in reply (excluding IPv6 and ICMP6 headers).
    uint16_t     mSequenceNumber; ///< Sequence number.
    uint8_t      mHopLimit;       ///< Hop limit.
} otPingSenderReply;

/**
 * This structure represents statistics of a ping request.
 *
 */
typedef struct otPingSenderStatistics
{
    uint16_t mSentCount;          ///< The number of ping requests already sent.
    uint16_t mReceivedCount;      ///< The number of ping replies received.
    uint32_t mTotalRoundTripTime; ///< The total round trip time of ping requests.
    uint16_t mMinRoundTripTime;   ///< The min round trip time among ping requests.
    uint16_t mMaxRoundTripTime;   ///< The max round trip time among ping requests.
    bool     mIsMulticast;        ///< Whether this is a multicast ping request.
} otPingSenderStatistics;

/**
 * This function pointer type specifies the callback to notify receipt of a ping reply.
 *
 * @param[in] aReply      A pointer to a `otPingSenderReply` containing info about the received ping reply.
 * @param[in] aContext    A pointer to application-specific context.
 *
 */
typedef void (*otPingSenderReplyCallback)(const otPingSenderReply *aReply, void *aContext);

/**
 * This function pointer type specifies the callback to report the ping statistics.
 *
 * @param[in] aStatistics      A pointer to a `otPingSenderStatistics` containing info about the received ping
 *                             statistics.
 * @param[in] aContext         A pointer to application-specific context.
 *
 */
typedef void (*otPingSenderStatisticsCallback)(const otPingSenderStatistics *aStatistics, void *aContext);

/**
 * This structure represents a ping request configuration.
 *
 */
typedef struct otPingSenderConfig
{
    otIp6Address              mSource;        ///< Source address of the ping.
    otIp6Address              mDestination;   ///< Destination address to ping.
    otPingSenderReplyCallback mReplyCallback; ///< Callback function to report replies (can be NULL if not needed).
    otPingSenderStatisticsCallback
             mStatisticsCallback; ///< Callback function to report statistics (can be NULL if not needed).
    void *   mCallbackContext;    ///< A pointer to the callback application-specific context.
    uint16_t mSize;               ///< Data size (# of bytes) excludes IPv6/ICMPv6 header. Zero for default.
    uint16_t mCount;              ///< Number of ping messages to send. Zero to use default.
    uint32_t mInterval;           ///< Ping tx interval in milliseconds. Zero to use default.
    uint16_t mTimeout;            ///< Time in milliseconds to wait for final reply after sending final request.
                                  ///< Zero to use default.
    uint8_t mHopLimit;            ///< Hop limit (used if `mAllowZeroHopLimit` is false). Zero for default.
    bool    mAllowZeroHopLimit;   ///< Indicates whether hop limit is zero.
} otPingSenderConfig;

/**
 * This function starts a ping.
 *
 * @param[in] aInstance            A pointer to an OpenThread instance.
 * @param[in] aConfig              The ping config to use.
 *
 * @retval OT_ERROR_NONE           The ping started successfully.
 * @retval OT_ERROR_BUSY           Could not start since busy with a previous ongoing ping request.
 * @retval OT_ERROR_INVALID_ARGS   The @p aConfig contains invalid parameters (e.g., ping interval is too long).

 *
 */
otError otPingSenderPing(otInstance *aInstance, const otPingSenderConfig *aConfig);

/**
 * This function stops an ongoing ping.
 *
 * @param[in] aInstance            A pointer to an OpenThread instance.
 *
 */
void otPingSenderStop(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PING_SENDER_H_
