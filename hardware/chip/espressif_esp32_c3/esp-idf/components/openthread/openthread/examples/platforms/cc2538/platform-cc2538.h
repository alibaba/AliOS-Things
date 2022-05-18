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
 *   This file includes the platform-specific initializers.
 *
 */

#ifndef PLATFORM_CC2538_H_
#define PLATFORM_CC2538_H_

#include <openthread-core-config.h>
#include <stdint.h>
#include <openthread/config.h>
#include <openthread/instance.h>

#include "cc2538-reg.h"

// Global OpenThread instance structure
extern otInstance *sInstance;

/**
 * Initialize the debug uart
 */
void cc2538DebugUartInit(void);

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void cc2538AlarmInit(void);

/**
 * This function performs alarm driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void cc2538AlarmProcess(otInstance *aInstance);

/**
 * This function initializes the radio service used by OpenThread.
 *
 */
void cc2538RadioInit(void);

/**
 * This function performs radio driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void cc2538RadioProcess(otInstance *aInstance);

/**
 * This function initializes the random number service used by OpenThread.
 *
 */
void cc2538RandomInit(void);

/**
 * This function performs UART driver processing.
 *
 */
void cc2538UartProcess(void);

#if OPENTHREAD_CONFIG_CC2538_WITH_CC2592 && OPENTHREAD_CONFIG_CC2592_USE_HGM
/**
 * Change the state of the CC2592 HGM pin.
 *
 * @param	aState	Whether or not to enable HGM
 */
void cc2538RadioSetHgm(bool aState);

/**
 * Retrieve the state of the CC2592 HGM pin.
 */
bool cc2538RadioGetHgm(void);
#endif // OPENTHREAD_CONFIG_CC2538_WITH_CC2592 && OPENTHREAD_CONFIG_CC2592_USE_HGM

typedef enum
{
    OT_CC2538_TIMER_ENERGY_SCAN, ///< Internal timer for energy scan
    OT_CC2538_TIMERS_COUNT,      ///< Number of internal timers
} otCC2538Timer;

/**
 * This function sets the internal timer.
 *
 * @param[in]   aTimer  The timer identifier.
 * @param[in]   aDelay  The delay to trigger the timer, and must be no more than `INT32_MAX`.
 *
 */
void cc2538SetTimer(otCC2538Timer aTimer, uint32_t aDelay);

#endif // PLATFORM_CC2538_H_
