/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,        /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,       /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif


#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_FUNC2 0x02u         /*!<@brief Selects pin function 2 */
#define IOCON_PIO_INPFILT_OFF 0x0200u /*!<@brief Input filter disabled */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_MODE_PULLUP 0x20u   /*!<@brief Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO122_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO122_FUNC_ALT0 0x00u
/*!
 * @brief Input polarity.: Disabled. Input function is not inverted. */
#define PIO122_INVERT_DISABLED 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO122_MODE_PULL_UP 0x02u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO320_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO320_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO321_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO321_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO322_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO322_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO330_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO330_FUNC_ALT1 0x01u
/*!
 * @brief Select Analog/Digital mode.: Digital mode. */
#define PIO47_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO47_FUNC_ALT0 0x00u
/*!
 * @brief Input polarity.: Disabled. Input function is not inverted. */
#define PIO47_INVERT_DISABLED 0x00u
/*!
 * @brief
 * Selects function mode (on-chip pull-up/pull-down resistor control).
 * : Pull-down.
 * Pull-down resistor enabled.
 */
#define PIO47_MODE_PULL_DOWN 0x01u

/*! @name PIO4_7 (coord A14), PWRON
  @{ */
#define BOARD_INITGT202SHIELD_PWRON_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITGT202SHIELD_PWRON_SIGNAL PIO4                        /*!<@brief GPIO signal: PIO4 */
#define BOARD_INITGT202SHIELD_PWRON_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_PWRON_GPIO_PIN 7U                        /*!<@brief PIO4 pin index: 7 */
#define BOARD_INITGT202SHIELD_PWRON_PORT 4U                            /*!<@brief PORT device name: 4U */
#define BOARD_INITGT202SHIELD_PWRON_PIN 7U                             /*!<@brief 4U pin index: 7 */
#define BOARD_INITGT202SHIELD_PWRON_CHANNEL 7                          /*!<@brief GPIO PIO4 channel: 7 */
#define BOARD_INITGT202SHIELD_PWRON_PIN_NAME PIO4_7                    /*!<@brief Pin name */
#define BOARD_INITGT202SHIELD_PWRON_LABEL "PWRON"                      /*!<@brief Label */
#define BOARD_INITGT202SHIELD_PWRON_NAME "PWRON"                       /*!<@brief Identifier name */
#define BOARD_INITGT202SHIELD_PWRON_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                       /* @} */

/*! @name PIO1_22 (coord P11), IRQ
  @{ */
#define BOARD_INITGT202SHIELD_IRQ_PERIPHERAL GPIO                   /*!<@brief Device name: GPIO */
#define BOARD_INITGT202SHIELD_IRQ_SIGNAL PIO1                       /*!<@brief GPIO signal: PIO1 */
#define BOARD_INITGT202SHIELD_IRQ_GPIO GPIO                         /*!<@brief GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_IRQ_GPIO_PIN 22U                      /*!<@brief PIO1 pin index: 22 */
#define BOARD_INITGT202SHIELD_IRQ_PORT 1U                           /*!<@brief PORT device name: 1U */
#define BOARD_INITGT202SHIELD_IRQ_PIN 22U                           /*!<@brief 1U pin index: 22 */
#define BOARD_INITGT202SHIELD_IRQ_CHANNEL 22                        /*!<@brief GPIO PIO1 channel: 22 */
#define BOARD_INITGT202SHIELD_IRQ_PIN_NAME PIO1_22                  /*!<@brief Pin name */
#define BOARD_INITGT202SHIELD_IRQ_LABEL "IRQ"                       /*!<@brief Label */
#define BOARD_INITGT202SHIELD_IRQ_NAME "IRQ"                        /*!<@brief Identifier name */
#define BOARD_INITGT202SHIELD_IRQ_DIRECTION kPIN_MUX_DirectionInput /*!<@brief Direction */
                                                                    /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitGT202Shield(void); /* Function assigned for the Cortex-M4F */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
