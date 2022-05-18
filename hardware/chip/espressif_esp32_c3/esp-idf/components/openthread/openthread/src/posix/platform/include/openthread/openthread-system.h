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
 *   This file defines the platform-specific functions needed by OpenThread's example applications.
 */

#ifndef OPENTHREAD_SYSTEM_H_
#define OPENTHREAD_SYSTEM_H_

#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/platform/misc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This enumeration represents default parameters for the SPI interface.
 *
 */
enum
{
    OT_PLATFORM_CONFIG_SPI_DEFAULT_MODE           = 0,       ///< Default SPI Mode: CPOL=0, CPHA=0.
    OT_PLATFORM_CONFIG_SPI_DEFAULT_SPEED_HZ       = 1000000, ///< Default SPI speed in hertz.
    OT_PLATFORM_CONFIG_SPI_DEFAULT_CS_DELAY_US    = 20,      ///< Default delay after SPI C̅S̅ assertion, in µsec.
    OT_PLATFORM_CONFIG_SPI_DEFAULT_RESET_DELAY_MS = 0, ///< Default delay after R̅E̅S̅E̅T̅ assertion, in miliseconds.
    OT_PLATFORM_CONFIG_SPI_DEFAULT_ALIGN_ALLOWANCE =
        16, ///< Default maximum number of 0xFF bytes to clip from start of MISO frame.
    OT_PLATFORM_CONFIG_SPI_DEFAULT_SMALL_PACKET_SIZE =
        32,                                ///< Default smallest SPI packet size we can receive in a single transaction.
    OT_PLATFORM_CONFIG_MAX_RADIO_URLS = 2, ///< Max number of Radio URLs.
};

/**
 * This structure represents platform specific configurations.
 *
 */
typedef struct otPlatformConfig
{
    const char *mBackboneInterfaceName;                        ///< Backbone network interface name.
    const char *mInterfaceName;                                ///< Thread network interface name.
    const char *mRadioUrls[OT_PLATFORM_CONFIG_MAX_RADIO_URLS]; ///< Radio URLs.
    uint8_t     mRadioUrlNum;                                  ///< Number of Radio URLs.
    int         mRealTimeSignal;                               ///< The real-time signal for microsecond timer.
    uint32_t    mSpeedUpFactor;                                ///< Speed up factor.
} otPlatformConfig;

/**
 * This function performs all platform-specific initialization of OpenThread's drivers and initializes the OpenThread
 * instance.
 *
 * @note This function is not called by the OpenThread library. Instead, the system/RTOS should call this function
 *       when initialization of OpenThread's drivers is most appropriate.
 *
 * @param[in]  aPlatformConfig  Platform configuration structure.
 *
 * @returns A pointer to the OpenThread instance.
 *
 */
otInstance *otSysInit(otPlatformConfig *aPlatformConfig);

/**
 * This function finalizes the OpenThread instance and performs all platform-specific deinitialization for OpenThread's
 * drivers.
 *
 * @note This function is not called by the OpenThread library. Instead, the system/RTOS should call this function
 *       when deinitialization of OpenThread's drivers is most appropriate.
 *
 */
void otSysDeinit(void);

/**
 * This structure represents a context for a select() based mainloop.
 *
 */
typedef struct otSysMainloopContext
{
    fd_set         mReadFdSet;  ///< The read file descriptors.
    fd_set         mWriteFdSet; ///< The write file descriptors.
    fd_set         mErrorFdSet; ///< The error file descriptors.
    int            mMaxFd;      ///< The max file descriptor.
    struct timeval mTimeout;    ///< The timeout.
} otSysMainloopContext;

/**
 * This function updates the file descriptor sets with file descriptors used by OpenThread drivers.
 *
 * @param[in]       aInstance   The OpenThread instance structure.
 * @param[inout]    aMainloop   A pointer to the mainloop context.
 *
 */
void otSysMainloopUpdate(otInstance *aInstance, otSysMainloopContext *aMainloop);

/**
 * This function polls OpenThread's mainloop.
 *
 * @param[inout]    aMainloop   A pointer to the mainloop context.
 *
 * @returns value returned from select().
 *
 */
int otSysMainloopPoll(otSysMainloopContext *aMainloop);

/**
 * This function performs all platform-specific processing for OpenThread's example applications.
 *
 * @note This function is not called by the OpenThread library. Instead, the system/RTOS should call this function
 *       in the main loop when processing OpenThread's drivers is most appropriate.
 *
 * @param[in]   aInstance   The OpenThread instance structure.
 * @param[in]   aMainloop   A pointer to the mainloop context.
 *
 */
void otSysMainloopProcess(otInstance *aInstance, const otSysMainloopContext *aMainloop);

/**
 * This method returns the radio url help string.
 *
 * @returns the radio url help string.
 *
 */
const char *otSysGetRadioUrlHelpString(void);

extern otPlatResetReason gPlatResetReason;

/**
 * This method returns the Thread network interface name.
 *
 * @returns The Thread network interface name.
 *
 */
const char *otSysGetThreadNetifName(void);

/**
 * This method returns the Thread network interface index.
 *
 * @returns The Thread network interface index.
 *
 */
unsigned int otSysGetThreadNetifIndex(void);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OPENTHREAD_SYSTEM_H_
