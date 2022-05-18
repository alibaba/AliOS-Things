/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes compile-time configurations for Dataset Updater.
 *
 */

#ifndef CONFIG_DATASET_UPDATER_H_
#define CONFIG_DATASET_UPDATER_H_

/**
 * @def OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE
 *
 * Define as 1 to enable Dataset Updater support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE
#define OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_DELAY
 *
 * Specifies the default delay (in ms) used by Dataset Updater when not included in Dataset already.
 *
 */
#ifndef OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_DELAY
#define OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_DELAY 30000
#endif

/**
 * @def OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_RETRY_WAIT_INTERVAL
 *
 * Specifies the default retry wait interval (in ms) for Dataset Updater to wait (in addition to Dataset delay) after
 * sending MGMT Set Pending Dataset command waiting for Active Dataset to be updated before retrying again before
 * retrying the MGMT Set Pending Dataset command.
 *
 */
#ifndef OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_RETRY_WAIT_INTERVAL
#define OPENTHREAD_CONFIG_DATASET_UPDATER_DEFAULT_RETRY_WAIT_INTERVAL 1500
#endif

#endif // CONFIG_DATASET_UPDATER_H_
