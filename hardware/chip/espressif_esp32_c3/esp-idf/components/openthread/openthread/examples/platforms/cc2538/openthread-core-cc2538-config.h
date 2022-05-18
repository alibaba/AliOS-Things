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
 *   This file includes cc2538 compile-time configuration constants for OpenThread.
 */

#ifndef OPENTHREAD_CORE_CC2538_CONFIG_H_
#define OPENTHREAD_CORE_CC2538_CONFIG_H_

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO "CC2538"

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE
 *
 * Define to 1 if you want to enable software ACK timeout logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_ACK_TIMEOUT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Define to 1 if you want to enable software retransmission logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Define to 1 if you want to enable software CSMA-CA backoff logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
 *
 * Define to 1 if you want to enable software transmission security logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE 0

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
 *
 * Define to 1 if you want to enable software energy scanning logic.
 *
 */
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable NCP HDLC support.
 *
 */
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_CC2538_USE_RADIO_RX_INTERRUPT
 *
 * Enable support for using interrupt-driven radio reception.  This allows
 * for a single frame to be received whilst the CPU is busy processing some
 * other code.
 *
 * To disable interrupts and just rely on polling, set this to 0.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2538_USE_RADIO_RX_INTERRUPT
#define OPENTHREAD_CONFIG_CC2538_USE_RADIO_RX_INTERRUPT 1
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2538_WITH_CC2592
 *
 * Enable support for the CC2592 range-extender front-end.
 *
 * This is a feature of the CC2538-CC2592 EM and other peripherals which
 * extends the range of the bare CC2538 to over a kilometre line-of-sight.
 * The CC2592 needs to be wired up to the RF port on the CC2538 in accordance
 * with application note 130 ("Using CC2592 Front End With CC2538", TI doc
 * SWRA447).
 *
 * If you have such a board, change this to 1.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2538_WITH_CC2592
#define OPENTHREAD_CONFIG_CC2538_WITH_CC2592 0
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_PA_EN_PIN
 *
 * Define the pin (on port C) that connects to the CC2592 PA_EN pin.
 *
 * One of the 3 observable channels on the CC2538 radio module will be
 * configured to take the "PA power down" signal from the radio module itself,
 * invert it, and emit it on this GPIO pin.  Due to hardware constraints, it
 * may only be connected to a pin on GPIO port C.
 *
 * The default (PC3) is as per TI recommendations in AN130.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_PA_EN_PIN
#define OPENTHREAD_CONFIG_CC2592_PA_EN_PIN 3
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_LNA_EN_PIN
 *
 * Define the pin (on port C) that connects to the CC2592 LNA_EN pin.
 *
 * One of the 3 observable channels on the CC2538 radio module will be
 * configured to take the "LNA power down" signal from the radio module itself,
 * invert it, and emit it on this GPIO pin.  Due to hardware constraints, it
 * may only be connected to a pin on GPIO port C.
 *
 * The default (PC2) is as per TI recommendations in AN130.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_LNA_EN_PIN
#define OPENTHREAD_CONFIG_CC2592_LNA_EN_PIN 2
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_USE_HGM
 *
 * Enable control of the high-gain mode signal.
 *
 * High-gain mode is enabled through the `HGM` pin on the CC2592, which may be
 * connected to any free GPIO pin for software control, or may be linked to
 * VDD or 0V to hard-wire it to a given state.
 *
 * Set this to 0 if you have wired this pin to a power rail, or have a
 * non-standard way of controlling it.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_USE_HGM
#define OPENTHREAD_CONFIG_CC2592_USE_HGM 1
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2538_RECEIVE_SENSITIVITY
 *
 * Set the CC2538 receive sensitivity.
 *
 * A bare CC2538 has a receive sensitivity of -88dBm.  The CC2592 changes this
 * to -85 or -81 depending on whether the HGM pin is high or low.  If
 * `OPENTHREAD_CONFIG_CC2592_USE_HGM` is 0, then this sets the receive
 * sensitivity.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2538_RECEIVE_SENSITIVITY
#define OPENTHREAD_CONFIG_CC2538_RECEIVE_SENSITIVITY -88
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2538_RSSI_OFFSET
 *
 * Set the CC2538 RSSI offset.  This calibrates the RSSI readings received from
 * the CC2538 radio module to give a reading in dBm.
 *
 * For a standard CC2538 (no front-end), the RSSI offset is 73.
 *
 * For a CC2592 hard-wired in high-gain mode, an offset of 85 should be used;
 * or for low-gain mode, 81.  If `OPENTHREAD_CONFIG_CC2592_USE_HGM` is 0, then
 * this calibrates the RSSI value accordingly.
 */
#ifndef OPENTHREAD_CONFIG_CC2538_RSSI_OFFSET
#define OPENTHREAD_CONFIG_CC2538_RSSI_OFFSET 73
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_HGM_PORT
 *
 * Define the GPIO port that the HGM pin is connected to.  It may be
 * connected to any available GPIO pin.
 *
 * The default (GPIO port D) is as per TI recommendations.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_HGM_PORT
#define OPENTHREAD_CONFIG_CC2592_HGM_PORT GPIO_D_BASE
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_HGM_PIN
 *
 * Define the pin on the GPIO port that the HGM pin is connected to.  It
 * may be connected to any available GPIO pin.
 *
 * The default (PD2) is as per TI recommendations.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_HGM_PIN
#define OPENTHREAD_CONFIG_CC2592_HGM_PIN 2
#endif

/**
 * @def OPENTHREAD_CONFIG_CC2592_HGM_DEFAULT_STATE
 *
 * Define the default state of the CC2592's HGM pin.
 *
 * The default is to turn high-gain mode on.
 *
 */
#ifndef OPENTHREAD_CONFIG_CC2592_HGM_DEFAULT_STATE
#define OPENTHREAD_CONFIG_CC2592_HGM_DEFAULT_STATE true
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * Define to 1 to enable otPlatFlash* APIs to support non-volatile storage.
 *
 * When defined to 1, the platform MUST implement the otPlatFlash* APIs instead of the otPlatSettings* APIs.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 1

#endif // OPENTHREAD_CORE_CC2538_CONFIG_H_
