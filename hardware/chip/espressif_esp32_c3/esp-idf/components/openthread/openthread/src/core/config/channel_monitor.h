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
 *   This file includes compile-time configurations for Channel Monitor.
 *
 */

#ifndef CONFIG_CHANNEL_MONITOR_H_
#define CONFIG_CHANNEL_MONITOR_H_

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
 *
 * Define to 1 to enable Channel Monitor support.
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_INTERVAL
 *
 * The sample interval in milliseconds used by Channel Monitoring feature.

 * When enabled, a zero-duration Energy Scan is performed, collecting a single RSSI sample per channel during each
 * interval.
 *
 * Applicable only if Channel Monitoring feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_INTERVAL
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_INTERVAL 41000
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MONITOR_RSSI_THRESHOLD
 *
 * The RSSI threshold in dBm used by Channel Monitoring feature.
 *
 * The RSSI samples are compared with the given threshold. Channel monitoring reports the average rate of RSSI samples
 * that are above this threshold within an observation window (per channel).
 *
 * It is recommended that this value is set to same value as the CCA threshold used by radio.
 *
 * Applicable only if Channel Monitoring feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MONITOR_RSSI_THRESHOLD
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_RSSI_THRESHOLD -75
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_WINDOW
 *
 * The averaging sample window length (in units of channel sample interval) used by Channel Monitoring feature.
 *
 * Channel monitoring will sample all channels every sample interval. It maintains the average rate of RSSI samples
 * that are above the RSSI threshold within (approximately) this sample window.
 *
 * Applicable only if Channel Monitoring feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_WINDOW
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_SAMPLE_WINDOW 960
#endif

#endif // CONFIG_CHANNEL_MONITOR_H_
