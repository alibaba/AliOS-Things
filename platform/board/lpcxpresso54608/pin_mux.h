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
#define IOCON_PIO_FUNC0 0x00u         /*!<@brief Selects pin function 0 */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_FUNC6 0x06u         /*!<@brief Selects pin function 6 */
#define IOCON_PIO_FUNC7 0x07u         /*!<@brief Selects pin function 7 */
#define IOCON_PIO_INPFILT_OFF 0x0200u /*!<@brief Input filter disabled */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_MODE_PULLUP 0x20u   /*!<@brief Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_FAST 0x0400u   /*!<@brief Fast mode, slew rate control is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */

/*! @name FC0_RXD_SDA_MOSI (coord B13), U24[13]/P0_29-ISP_FC0_RXD
  @{ */
#define BOARD_ISP_FC0_RXD_PERIPHERAL FLEXCOMM0              /*!<@brief Device name: FLEXCOMM0 */
#define BOARD_ISP_FC0_RXD_SIGNAL RXD_SDA_MOSI               /*!<@brief FLEXCOMM0 signal: RXD_SDA_MOSI */
#define BOARD_ISP_FC0_RXD_PIN_NAME FC0_RXD_SDA_MOSI         /*!<@brief Pin name */
#define BOARD_ISP_FC0_RXD_LABEL "U24[13]/P0_29-ISP_FC0_RXD" /*!<@brief Label */
#define BOARD_ISP_FC0_RXD_NAME "ISP_FC0_RXD"                /*!<@brief Identifier name */
                                                            /* @} */

/*! @name FC0_TXD_SCL_MISO (coord A2), U24[12]/P0_30-ISP_FC0_TXD
  @{ */
#define BOARD_ISP_FC0_TXD_PERIPHERAL FLEXCOMM0              /*!<@brief Device name: FLEXCOMM0 */
#define BOARD_ISP_FC0_TXD_SIGNAL TXD_SCL_MISO               /*!<@brief FLEXCOMM0 signal: TXD_SCL_MISO */
#define BOARD_ISP_FC0_TXD_PIN_NAME FC0_TXD_SCL_MISO         /*!<@brief Pin name */
#define BOARD_ISP_FC0_TXD_LABEL "U24[12]/P0_30-ISP_FC0_TXD" /*!<@brief Label */
#define BOARD_ISP_FC0_TXD_NAME "ISP_FC0_TXD"                /*!<@brief Identifier name */
                                                            /* @} */

/*! @name SWO (coord P2), U16[12]/SWO_TRGT
  @{ */
#define BOARD_SWO_TRGT_PERIPHERAL SWD           /*!<@brief Device name: SWD */
#define BOARD_SWO_TRGT_SIGNAL SWO               /*!<@brief SWD signal: SWO */
#define BOARD_SWO_TRGT_PIN_NAME SWO             /*!<@brief Pin name */
#define BOARD_SWO_TRGT_LABEL "U16[12]/SWO_TRGT" /*!<@brief Label */
#define BOARD_SWO_TRGT_NAME "SWO_TRGT"          /*!<@brief Identifier name */
                                                /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

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
