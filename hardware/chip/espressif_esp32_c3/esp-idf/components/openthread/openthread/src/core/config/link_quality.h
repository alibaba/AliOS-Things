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
 *   This file includes compile-time configurations for the Link Quality service.
 *
 */

#ifndef CONFIG_LINK_QUALITY_H_
#define CONFIG_LINK_QUALITY_H_

/**
 * @def OPENTHREAD_CONFIG_CCA_FAILURE_RATE_AVERAGING_WINDOW
 *
 * OpenThread's MAC implementation maintains the average failure rate of CCA (Clear Channel Assessment) operation on
 * frame transmissions. This value specifies the window (in terms of number of transmissions or samples) over which the
 * average rate is maintained. Practically, the average value can be considered as the percentage of CCA failures in
 * (approximately) last AVERAGING_WINDOW frame transmissions.
 *
 */
#ifndef OPENTHREAD_CONFIG_CCA_FAILURE_RATE_AVERAGING_WINDOW
#define OPENTHREAD_CONFIG_CCA_FAILURE_RATE_AVERAGING_WINDOW 512
#endif

/**
 * @def OPENTHREAD_CONFIG_FRAME_TX_ERR_RATE_AVERAGING_WINDOW
 *
 * OpenThread's MAC implementation maintains the average error rate of MAC frame transmissions per neighbor. This
 * parameter specifies the window (in terms of number of frames/sample) over which the average error rate is maintained.
 * Practically, the average value can be considered as the percentage of failed (no ack) MAC frame transmissions  over
 * (approximately) last AVERAGING_WINDOW frame transmission attempts to a specific neighbor.
 *
 */
#ifndef OPENTHREAD_CONFIG_FRAME_TX_ERR_RATE_AVERAGING_WINDOW
#define OPENTHREAD_CONFIG_FRAME_TX_ERR_RATE_AVERAGING_WINDOW 128
#endif

/**
 * @def OPENTHREAD_CONFIG_IPV6_TX_ERR_RATE_AVERAGING_WINDOW
 *
 * OpenThread maintains the average error rate of IPv6 messages per neighbor. This parameter specifies the
 * window (in terms of number of messages) over which the average error rate is maintained. Practically, the average
 * value can be considered as the percentage of failed (no ack) messages over (approximately) last AVERAGING_WINDOW
 * IPv6 messages sent to a specific neighbor.
 *
 */
#ifndef OPENTHREAD_CONFIG_IPV6_TX_ERR_RATE_AVERAGING_WINDOW
#define OPENTHREAD_CONFIG_IPV6_TX_ERR_RATE_AVERAGING_WINDOW 128
#endif

#endif // CONFIG_LINK_QUALITY_H_
