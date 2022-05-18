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
 *   This file includes compile-time configurations for the Time Sync service.
 *
 */

#ifndef CONFIG_TIME_SYNC_H_
#define CONFIG_TIME_SYNC_H_

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
 *
 * Define as 1 to enable the time synchronization service feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#define OPENTHREAD_CONFIG_TIME_SYNC_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED
 *
 * Define as 1 to require time synchronization when attaching to a network. If the device is router capable
 * and cannot find a neighboring router supporting time synchronization, the device will form a new partition.
 * If the device is not router capable, the device will remain an orphan.
 *
 * Applicable only if time synchronization service feature is enabled (i.e., OPENTHREAD_CONFIG_TIME_SYNC_ENABLE is set)
 *
 */
#ifndef OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED
#define OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED 0
#endif

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_PERIOD
 *
 * Specifies the default period of time synchronization, in seconds.
 *
 * Applicable only if time synchronization service feature is enabled (i.e., OPENTHREAD_CONFIG_TIME_SYNC_ENABLE is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_TIME_SYNC_PERIOD
#define OPENTHREAD_CONFIG_TIME_SYNC_PERIOD 30
#endif

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_XTAL_THRESHOLD
 *
 * Specifies the default XTAL threshold for a device to become Router in time synchronization enabled network, in PPM.
 *
 * Applicable only if time synchronization service feature is enabled (i.e., OPENTHREAD_CONFIG_TIME_SYNC_ENABLE is set)
 *
 */
#ifndef OPENTHREAD_CONFIG_TIME_SYNC_XTAL_THRESHOLD
#define OPENTHREAD_CONFIG_TIME_SYNC_XTAL_THRESHOLD 300
#endif

/**
 * @def OPENTHREAD_CONFIG_TIME_SYNC_JUMP_NOTIF_MIN_US
 *
 * This setting sets the minimum amount of time (in microseconds) that the network time must jump due to
 * a time sync event for listeners to be notified of the new network time.
 *
 */
#ifndef OPENTHREAD_CONFIG_TIME_SYNC_JUMP_NOTIF_MIN_US
#define OPENTHREAD_CONFIG_TIME_SYNC_JUMP_NOTIF_MIN_US 10000
#endif

#endif // CONFIG_TIME_SYNC_H_
