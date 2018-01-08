/*
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
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

#ifndef _FSL_RESET_H_
#define _FSL_RESET_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"

/*!
 * @addtogroup ksdk_common
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief reset driver version 2.0.0. */
#define FSL_RESET_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!
 * @brief Enumeration for peripheral reset control bits
 *
 * Defines the enumeration for peripheral reset control bits in PRESETCTRL/ASYNCPRESETCTRL registers
 */
typedef enum _SYSCON_RSTn
{
    kFLASH_RST_SHIFT_RSTn = 0 | 7U,  /**< Flash controller reset control */
    kFMC_RST_SHIFT_RSTn = 0 | 8U,    /**< Flash accelerator reset control */
    kMUX_RST_SHIFT_RSTn = 0 | 11U,   /**< Input mux reset control */
    kIOCON_RST_SHIFT_RSTn = 0 | 13U, /**< IOCON reset control */
    kGPIO0_RST_SHIFT_RSTn = 0 | 14U, /**< GPIO0 reset control */
    kGPIO1_RST_SHIFT_RSTn = 0 | 15U, /**< GPIO1 reset control */
    kPINT_RST_SHIFT_RSTn = 0 | 18U,  /**< Pin interrupt (PINT) reset control */
    kGINT_RST_SHIFT_RSTn = 0 | 19U,  /**< Grouped interrupt (PINT) reset control. */
    kDMA_RST_SHIFT_RSTn = 0 | 20U,   /**< DMA reset control */
    kCRC_RST_SHIFT_RSTn = 0 | 21U,   /**< CRC reset control */
    kWWDT_RST_SHIFT_RSTn = 0 | 22U,  /**< Watchdog timer reset control */
    kADC0_RST_SHIFT_RSTn = 0 | 27U,  /**< ADC0 reset control */

    kMRT_RST_SHIFT_RSTn = 65536 | 0U,     /**< Multi-rate timer (MRT) reset control */
    kRIT_RST_SHIFT_RSTn = 65536 | 1U,     /**< Repetitive interrupt timer (RIT) reset control */
    kSCT0_RST_SHIFT_RSTn = 65536 | 2U,    /**< SCTimer/PWM 0 (SCT0) reset control */
    kFIFO_RST_SHIFT_RSTn = 65536 | 9U,    /**< System FIFO reset control */
    kUTICK_RST_SHIFT_RSTn = 65536 | 10U,  /**< Micro-tick timer reset control */
    kCT32B2_RST_SHIFT_RSTn = 65536 | 22U, /**< CT32B2 reset control */
    kCT32B3_RST_SHIFT_RSTn = 65536 | 26U, /**< CT32B3 reset control */
    kCT32B4_RST_SHIFT_RSTn = 65536 | 27U, /**< CT32B4 reset control */

    kUSART0_RST_SHIFT_RSTn = 67108864 | 1U, /**< USART0 reset control */
    kUSART1_RST_SHIFT_RSTn = 67108864 | 2U, /**< USART1 reset control */
    kUSART2_RST_SHIFT_RSTn = 67108864 | 3U, /**< USART2 reset control */
    kUSART3_RST_SHIFT_RSTn = 67108864 | 4U, /**< USART3 reset control */

    kI2C0_RST_SHIFT_RSTn = 67108864 | 5U, /**< I2C0 reset control */
    kI2C1_RST_SHIFT_RSTn = 67108864 | 6U, /**< I2C1 reset control */
    kI2C2_RST_SHIFT_RSTn = 67108864 | 7U, /**< I2C2 reset control */

    kSPI0_RST_SHIFT_RSTn = 67108864 | 9U,  /**< SPI0 reset control */
    kSPI1_RST_SHIFT_RSTn = 67108864 | 10U, /**< SPI1 reset control */

    kCT32B0_RST_SHIFT_RSTn = 67108864 | 13U, /**< CT32B0 reset control */
    kCT32B1_RST_SHIFT_RSTn = 67108864 | 14U, /**< CT32B1 reset control */
    kFRG0_RST_SHIFT_RSTn = 67108864 | 15U,   /**< FRG0 reset control */

} SYSCON_RSTn_t;

/** Array initializers with peripheral reset bits **/
#define ADC_RSTS             \
    {                        \
        kADC0_RST_SHIFT_RSTn \
    } /* Reset bits for ADC peripheral */
#define CRC_RSTS            \
    {                       \
        kCRC_RST_SHIFT_RSTn \
    } /* Reset bits for CRC peripheral */
#define DMA_RSTS            \
    {                       \
        kDMA_RST_SHIFT_RSTn \
    } /* Reset bits for DMA peripheral */
#define GINT_RSTS                                  \
    {                                              \
        kGINT_RST_SHIFT_RSTn, kGINT_RST_SHIFT_RSTn \
    } /* Reset bits for GINT peripheral. GINT0 & GINT1 share same slot */
#define GPIO_RSTS                                    \
    {                                                \
        kGPIO0_RST_SHIFT_RSTn, kGPIO1_RST_SHIFT_RSTn \
    } /* Reset bits for GPIO peripheral */
#define INPUTMUX_RSTS       \
    {                       \
        kMUX_RST_SHIFT_RSTn \
    } /* Reset bits for INPUTMUX peripheral */
#define IOCON_RSTS            \
    {                         \
        kIOCON_RST_SHIFT_RSTn \
    } /* Reset bits for IOCON peripheral */
#define FLASH_RSTS                                 \
    {                                              \
        kFLASH_RST_SHIFT_RSTn, kFMC_RST_SHIFT_RSTn \
    } /* Reset bits for Flash peripheral */
#define MRT_RSTS            \
    {                       \
        kMRT_RST_SHIFT_RSTn \
    } /* Reset bits for MRT peripheral */
#define RIT_RSTS            \
    {                       \
        kRIT_RST_SHIFT_RSTn \
    } /* Reset bits for RIT peripheral */
#define PINT_RSTS            \
    {                        \
        kPINT_RST_SHIFT_RSTn \
    } /* Reset bits for PINT peripheral */
#define SCT_RSTS             \
    {                        \
        kSCT0_RST_SHIFT_RSTn \
    } /* Reset bits for SCT peripheral */
#define FIFO_RSTS            \
    {                        \
        kFIFO_RST_SHIFT_RSTn \
    } /* Reset bits for FIFO peripheral */
#define CTIMER_RSTS                                                                                     \
    {                                                                                                   \
        kCT32B0_RST_SHIFT_RSTn, kCT32B1_RST_SHIFT_RSTn, kCT32B2_RST_SHIFT_RSTn, kCT32B3_RST_SHIFT_RSTn, \
            kCT32B4_RST_SHIFT_RSTn                                                                      \
    } /* Reset bits for TIMER peripheral */
#define UTICK_RSTS            \
    {                         \
        kUTICK_RST_SHIFT_RSTn \
    } /* Reset bits for UTICK peripheral */
#define WWDT_RSTS            \
    {                        \
        kWWDT_RST_SHIFT_RSTn \
    } /* Reset bits for WWDT peripheral */
#define USART_RSTS                                                                                    \
    {                                                                                                 \
        kUSART0_RST_SHIFT_RSTn, kUSART1_RST_SHIFT_RSTn, kUSART2_RST_SHIFT_RSTn kUSART3_RST_SHIFT_RSTn \
    } /* Reset bits for USART peripheral */
#define I2C_RSTS                                                         \
    {                                                                    \
        kI2C0_RST_SHIFT_RSTn, kI2C1_RST_SHIFT_RSTn, kI2C2_RST_SHIFT_RSTn \
    } /* Reset bits for I2C peripheral */
#define SPI_RSTS                                    \
    {                                               \
        kSPI0_RST_SHIFT_RSTn, kSPI0_RST_SHIFT_RSTn, \
    } /* Reset bits for SPI peripheral */
#define FRG_RSTS             \
    {                        \
        kFRG0_RST_SHIFT_RSTn \
    } /* Reset bits for WWDT peripheral */
typedef SYSCON_RSTn_t reset_ip_name_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Assert reset to peripheral.
 *
 * Asserts reset signal to specified peripheral module.
 *
 * @param peripheral Assert reset to this peripheral. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_SetPeripheralReset(reset_ip_name_t peripheral);

/*!
 * @brief Clear reset to peripheral.
 *
 * Clears reset signal to specified peripheral module, allows it to operate.
 *
 * @param peripheral Clear reset to this peripheral. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_ClearPeripheralReset(reset_ip_name_t peripheral);

/*!
 * @brief Reset peripheral module.
 *
 * Reset peripheral module.
 *
 * @param peripheral Peripheral to reset. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_PeripheralReset(reset_ip_name_t peripheral);

/*!
 * @brief Set slave core to reset state and hold.
 */
void RESET_SetSlaveCoreReset(void);

/*!
 * @brief Release slave core from reset state.
 */
void RESET_ClearSlaveCoreReset(void);

/*!
 * @brief Reset slave core with the boot entry.
 */
void RESET_SlaveCoreReset(uint32_t bootAddr, uint32_t bootStackPointer);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_RESET_H_ */
