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
 *   This file includes compile-time configurations for the DNS Client.
 *
 */

#ifndef CONFIG_ANNOUNCE_SENDER_H_
#define CONFIG_ANNOUNCE_SENDER_H_

/**
 * @def OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
 *
 * Define as 1 to enable `AnnounceSender` which will periodically send MLE Announce message on all channels.
 *
 * The list of channels is determined from the Operational Dataset's ChannelMask.
 *
 */
#ifndef OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
#define OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_ANNOUNCE_SENDER_INTERVAL
 *
 * Specifies the time interval (in milliseconds) between `AnnounceSender` transmit cycles.
 *
 * In a cycle, the `AnnounceSender` sends MLE Announcement on all channels in Active Operational Dataset's ChannelMask.
 * The transmissions on different channels happen uniformly over the given interval (i.e., if there are 16 channels,
 * there will be 16 MLE Announcement messages each on one channel with `interval / 16`  between two consecutive MLE
 * Announcement transmissions).
 *
 * Applicable only if `AnnounceSender` feature is enabled (see `OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE`).
 *
 */
#ifndef OPENTHREAD_CONFIG_ANNOUNCE_SENDER_INTERVAL
#define OPENTHREAD_CONFIG_ANNOUNCE_SENDER_INTERVAL 688000 // 688 seconds = 11 min and 28 sec.
#endif

/**
 * @def OPENTHREAD_CONFIG_ANNOUNCE_SENDER_REDUNDANCY_CONSTANT
 *
 * Specifies the number of MLE Announcement messages that the device must receive within a cycle interval to skip
 * sending the Announcement itself.
 *
 * This is used as the trickle timer redundancy constant in `AnnounceSender`.
 *
 * Applicable only if `AnnounceSender` feature is enabled (see `OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE`).
 *
 */
#ifndef OPENTHREAD_CONFIG_ANNOUNCE_SENDER_REDUNDANCY_CONSTANT
#define OPENTHREAD_CONFIG_ANNOUNCE_SENDER_REDUNDANCY_CONSTANT 8
#endif

/**
 *  @def OPENTHREAD_CONFIG_ANNOUNCE_SENDER_JITTER_INTERVAL
 *
 * Specifies the jitter interval (in milliseconds) used by `AnnounceSender`. A random jitter interval is applied to
 * the period between any two successive MLE Announcement transmissions (possibly) on different channels.
 *
 */
#ifndef OPENTHREAD_CONFIG_ANNOUNCE_SENDER_JITTER_INTERVAL
#define OPENTHREAD_CONFIG_ANNOUNCE_SENDER_JITTER_INTERVAL 500
#endif

#endif // CONFIG_ANNOUNCE_SENDER_H_
