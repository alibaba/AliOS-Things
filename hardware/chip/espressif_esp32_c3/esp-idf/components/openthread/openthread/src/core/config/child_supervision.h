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
 *   This file includes compile-time configurations for Child Supervision.
 *
 */

#ifndef CONFIG_CHILD_SUPERVISION_H_
#define CONFIG_CHILD_SUPERVISION_H_

/**
 * @def OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
 *
 * Define to 1 to enable Child Supervision support.
 *
 */
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_CHILD_SUPERVISION_INTERVAL
 *
 * The default supervision interval in seconds used by parent. Set to zero to disable the supervision process on the
 * parent.
 *
 * Applicable only if child supervision feature is enabled (i.e.,
 * `OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE ` is set).
 *
 * Child supervision feature provides a mechanism for parent to ensure that a message is sent to each sleepy child
 * within the supervision interval. If there is no transmission to the child within the supervision interval, child
 * supervisor will enqueue and send a supervision message (a data message with empty payload) to the child.
 *
 */
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_INTERVAL
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_INTERVAL 129
#endif

/**
 * @def OPENTHREAD_CONFIG_CHILD_SUPERVISION_CHECK_TIMEOUT
 *
 * The default supervision check timeout interval (in seconds) used by a device in child state. Set to zero to disable
 * the supervision check process on the child.
 *
 * Applicable only if child supervision feature is enabled (i.e., `OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE` is set).
 *
 * If the sleepy child does not hear from its parent within the specified timeout interval, it initiates the re-attach
 * process (MLE Child Update Request/Response exchange with its parent).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_CHECK_TIMEOUT
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_CHECK_TIMEOUT 190
#endif

/**
 * @def OPENTHREAD_CONFIG_CHILD_SUPERVISION_MSG_NO_ACK_REQUEST
 *
 * Define as 1 to clear/disable 15.4 ack request in the MAC header of a supervision message.
 *
 * Applicable only if child supervision feature is enabled (i.e., `OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHILD_SUPERVISION_MSG_NO_ACK_REQUEST
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_MSG_NO_ACK_REQUEST 0
#endif

#endif // CONFIG_CHILD_SUPERVISION_H_
