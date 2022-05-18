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
 *   This file includes platform abstractions for miscellaneous behaviors.
 */

#ifndef OPENTHREAD_PLATFORM_MISC_H_
#define OPENTHREAD_PLATFORM_MISC_H_

#include <stdint.h>

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-misc
 *
 * @brief
 *   This module includes platform abstractions for miscellaneous behaviors.
 *
 * @{
 *
 */

/**
 * This function performs a software reset on the platform, if supported.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 */
void otPlatReset(otInstance *aInstance);

/**
 * Enumeration of possible reset reason codes.
 *
 * These are in the same order as the Spinel reset reason codes.
 *
 */
typedef enum
{
    OT_PLAT_RESET_REASON_POWER_ON = 0,
    OT_PLAT_RESET_REASON_EXTERNAL = 1,
    OT_PLAT_RESET_REASON_SOFTWARE = 2,
    OT_PLAT_RESET_REASON_FAULT    = 3,
    OT_PLAT_RESET_REASON_CRASH    = 4,
    OT_PLAT_RESET_REASON_ASSERT   = 5,
    OT_PLAT_RESET_REASON_OTHER    = 6,
    OT_PLAT_RESET_REASON_UNKNOWN  = 7,
    OT_PLAT_RESET_REASON_WATCHDOG = 8,

    OT_PLAT_RESET_REASON_COUNT,
} otPlatResetReason;

/**
 * This function returns the reason for the last platform reset.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 *
 */
otPlatResetReason otPlatGetResetReason(otInstance *aInstance);

/**
 * This function provides a platform specific implementation for assert.
 *
 * @param[in] aFilename    The name of the file where the assert occurred.
 * @param[in] aLineNumber  The line number in the file where the assert occurred.
 *
 */
void otPlatAssertFail(const char *aFilename, int aLineNumber);

/**
 * This function performs a platform specific operation to wake the host MCU.
 * This is used only for NCP configurations.
 *
 */
void otPlatWakeHost(void);

/**
 * Enumeration of micro-controller's power states.
 *
 * These values are used for NCP configuration when `OPENTHREAD_CONFIG_NCP_ENABLE_MCU_POWER_STATE_CONTROL` is enabled.
 *
 * The power state specifies the desired power state of NCP's micro-controller (MCU) when the underlying platform's
 * operating system enters idle mode (i.e., all active tasks/events are processed and the MCU can potentially enter a
 * energy-saving power state).
 *
 * The power state primarily determines how the host should interact with the NCP and whether the host needs an
 * external trigger (a "poke") to NCP before it can communicate with the NCP or not.
 *
 * After a reset, the MCU power state MUST be `OT_PLAT_POWER_STATE_ON`.
 *
 */
typedef enum
{
    /**
     * NCP's MCU stays on and active all the time.
     *
     * When the NCP's desired power state is set to `ON`, host can send messages to NCP without requiring any "poke" or
     * external triggers.
     *
     * @note The `ON` power state only determines the MCU's power mode and is not related to radio's state.
     *
     */
    OT_PLAT_MCU_POWER_STATE_ON = 0,

    /**
     * NCP's MCU can enter low-power (energy-saving) state.
     *
     * When the NCP's desired power state is set to `LOW_POWER`, host is expected to "poke" the NCP (e.g., an external
     * trigger like an interrupt) before it can communicate with the NCP (send a message to the NCP). The "poke"
     * mechanism is determined by the platform code (based on NCP's interface to the host).
     *
     * While power state is set to `LOW_POWER`, NCP can still (at any time) send messages to host. Note that receiving
     * a message from the NCP does NOT indicate that the NCP's power state has changed, i.e., host is expected to
     * continue to "poke" when it wants to talk to the NCP until the power state is explicitly changed (by a successful
     * call to `otPlatSetMcuPowerState()` changing the state to `ON`).
     *
     * @note The `LOW_POWER` power state only determines the MCU's power mode and is not related to radio's state
     * (radio is managed by OpenThread core and device role, e.g., device being sleepy or not.
     *
     */
    OT_PLAT_MCU_POWER_STATE_LOW_POWER = 1,

    /**
     * NCP is fully off.
     *
     * An NCP hardware reset (via a RESET pin) is required to bring the NCP back to `SPINEL_MCU_POWER_STATE_ON`.
     * RAM is not retained after reset.
     *
     */
    OT_PLAT_MCU_POWER_STATE_OFF = 2,
} otPlatMcuPowerState;

/**
 * This function sets the desired MCU power state.
 *
 * This is only applicable and used for NCP configuration when `OPENTHREAD_CONFIG_NCP_ENABLE_MCU_POWER_STATE_CONTROL`
 * is enabled.
 *
 * @param[in] aInstance      A pointer to OpenThread instance.
 * @param[in] aState         The new MCU power state.
 *
 * @retval OT_ERROR_NONE     The power state updated successfully.
 * @retval OT_ERROR_FAILED   The given MCU power state is not supported by the platform.
 *
 */
otError otPlatSetMcuPowerState(otInstance *aInstance, otPlatMcuPowerState aState);

/**
 * This function gets the current desired MCU power state.
 *
 * This is only applicable and used for NCP configuration when `OPENTHREAD_CONFIG_NCP_ENABLE_MCU_POWER_STATE_CONTROL`
 * is enabled.
 *
 * After a reset, the power state MUST return `OT_PLAT_POWER_STATE_ON`. During operation, power state SHOULD only
 * change through an explicit successful call to `otPlatSetMcuPowerState()`.
 *
 * @param[in] aInstance  A pointer to OpenThread instance.
 *
 * @returns The current power state.
 *
 */
otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_MISC_H_
