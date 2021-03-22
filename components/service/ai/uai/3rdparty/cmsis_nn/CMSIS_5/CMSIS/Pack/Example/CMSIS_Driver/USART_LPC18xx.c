/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V2.9
 *
 * Driver:       Driver_USART0, Driver_USART1, Driver_USART2, Driver_USART3
 * Configured:   via RTE_Device.h configuration file
 * Project:      USART Driver for NXP LPC18xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                   Value   UART Interface
 *   ---------------------                   -----   --------------
 *   Connect to hardware via Driver_USART# = 0       use USART0
 *   Connect to hardware via Driver_USART# = 1       use UART1
 *   Connect to hardware via Driver_USART# = 2       use USART2
 *   Connect to hardware via Driver_USART# = 3       use USART3
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.9
 *    - Driver update to work with GPDMA_LPC18xx ver.: 1.3
 *  Version 2.8
 *    - Corrected PowerControl function for conditional Power full (driver must be initialized)
 *  Version 2.7
 *    - PowerControl for Power OFF and Uninitialize functions made unconditional.
 *    - Corrected status bit-field handling, to prevent race conditions.
 *  Version 2.6
 *    - Corrected disabling of receive DMA channel when aborting
 *      Receive (ARM_USART_ABORT_RECEIVE) or Transfer (ARM_USART_ABORT_TRANSFER)
 *  Version 2.5
 *    - fract_div_lookup_table moved from USART_LPC18xx.h to USART_LPC18xx.c
 *  Version 2.4
 *    - Improved baudrate calculation
 *  Version 2.3
 *    - Corrected RX Time-Out handling
 *    - Corrected USART clock configuration
 *    - Updated USART_Control function
 *    - Updated USART_Send function
 *    - GPDMA initialization and uninitialization
 *  Version 2.2
 *    - Corrected modem lines handling
 *  Version 2.1
 *    - Added DMA support
 *    - Other Improvements (status checking, USART_Control, ...)
 *  Version 2.0
 *    - Updated to CMSIS Driver API V2.00
 *  Version 1.1
 *    - Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.0
 *    - Initial release
 */
#include "USART_LPC18xx.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!RTE_USART0) && (!RTE_UART1) && (!RTE_USART2) && (!RTE_USART3))
#error "USART not enabled in RTE_Device.h!"
#endif

// Driver Version
static const ARM_DRIVER_VERSION usart_driver_version = { ARM_USART_API_VERSION, ARM_USART_DRV_VERSION };

// Trigger level definitions
// Can be user defined by C preprocessor
#ifndef USART0_TRIG_LVL
#define USART0_TRIG_LVL           USART_TRIG_LVL_1
#endif
#ifndef USART1_TRIG_LVL
#define USART1_TRIG_LVL           USART_TRIG_LVL_1
#endif
#ifndef USART2_TRIG_LVL
#define USART2_TRIG_LVL           USART_TRIG_LVL_1
#endif
#ifndef USART3_TRIG_LVL
#define USART3_TRIG_LVL           USART_TRIG_LVL_1
#endif

// SmartCard oversampling ratio
#ifndef USART0_SC_OVERSAMPLING_RATIO
#define USART0_SC_OVERSAMPLING_RATIO           372
#endif
#ifndef USART2_SC_OVERSAMPLING_RATIO
#define USART2_SC_OVERSAMPLING_RATIO           372
#endif
#ifndef USART3_SC_OVERSAMPLING_RATIO
#define USART3_SC_OVERSAMPLING_RATIO           372
#endif

// Fractional divider lookup table
static const FRACT_DIVIDER fract_div_lookup_table[] = {
  {(1 << 12), 0},
  FRACT_DIV(1,  15),
  FRACT_DIV(1,  14),
  FRACT_DIV(1,  13),
  FRACT_DIV(1,  12),
  FRACT_DIV(1,  11),
  FRACT_DIV(1,  10),
  FRACT_DIV(1,   9),
  FRACT_DIV(1,   8),
  FRACT_DIV(2,  15),
  FRACT_DIV(1,   7),
  FRACT_DIV(2,  13),
  FRACT_DIV(1,   6),
  FRACT_DIV(2,  11),
  FRACT_DIV(1,   5),
  FRACT_DIV(3,  14),
  FRACT_DIV(2,   9),
  FRACT_DIV(3,  13),
  FRACT_DIV(1,   4),
  FRACT_DIV(4,  15),
  FRACT_DIV(3,  11),
  FRACT_DIV(2,   7),
  FRACT_DIV(3,  10),
  FRACT_DIV(4,  13),
  FRACT_DIV(1,   3),
  FRACT_DIV(5,  14),
  FRACT_DIV(4,  11),
  FRACT_DIV(3,   8),
  FRACT_DIV(5,  13),
  FRACT_DIV(2,   5),
  FRACT_DIV(5,  12),
  FRACT_DIV(3,   7),
  FRACT_DIV(4,   9),
  FRACT_DIV(5,  11),
  FRACT_DIV(6,  13),
  FRACT_DIV(7,  15),
  FRACT_DIV(1,   2),
  FRACT_DIV(8,  15),
  FRACT_DIV(7,  13),
  FRACT_DIV(6,  11),
  FRACT_DIV(5,   9),
  FRACT_DIV(4,   7),
  FRACT_DIV(7,  12),
  FRACT_DIV(3,   5),
  FRACT_DIV(8,  13),
  FRACT_DIV(5,   8),
  FRACT_DIV(7,  11),
  FRACT_DIV(9,  14),
  FRACT_DIV(2,   3),
  FRACT_DIV(9,  13),
  FRACT_DIV(7,  10),
  FRACT_DIV(5,   7),
  FRACT_DIV(8,  11),
  FRACT_DIV(11, 15),
  FRACT_DIV(3,   4),
  FRACT_DIV(10, 13),
  FRACT_DIV(7,   9),
  FRACT_DIV(11, 14),
  FRACT_DIV(4,   5),
  FRACT_DIV(9,  11),
  FRACT_DIV(5,   6),
  FRACT_DIV(11, 13),
  FRACT_DIV(6,   7),
  FRACT_DIV(13, 15),
  FRACT_DIV(7,   8),
  FRACT_DIV(8,   9),
  FRACT_DIV(9,  10),
  FRACT_DIV(10, 11),
  FRACT_DIV(11, 12),
  FRACT_DIV(12, 13),
  FRACT_DIV(13, 14),
  FRACT_DIV(14, 15)
};

// Fractional divider lookup table size
#define FRACT_DIV_LOOKUP_TABLE_SZ  (sizeof(fract_div_lookup_table) / sizeof(fract_div_lookup_table[0]))

// USART0
#if (RTE_USART0)
static USART_INFO USART0_Info = {0};
static PIN_ID USART0_pin_tx  = { RTE_USART0_TX_PORT,   RTE_USART0_TX_BIT,   RTE_USART0_TX_FUNC };
static PIN_ID USART0_pin_rx  = { RTE_USART0_RX_PORT,   RTE_USART0_RX_BIT,   RTE_USART0_RX_FUNC };
#if (RTE_USART0_UCLK_PIN_EN == 1)
static PIN_ID USART0_pin_clk = { RTE_USART0_UCLK_PORT, RTE_USART0_UCLK_BIT, RTE_USART0_UCLK_FUNC };
#endif

#if (RTE_USART0_DMA_TX_EN == 1)
void USART0_GPDMA_Tx_Event (uint32_t event);
static USART_DMA USART0_DMA_Tx = {RTE_USART0_DMA_TX_CH,
                                  RTE_USART0_DMA_TX_PERI,
                                  RTE_USART0_DMA_TX_PERI_SEL,
                                  USART0_GPDMA_Tx_Event};
#endif
#if (RTE_USART0_DMA_RX_EN == 1)
void USART0_GPDMA_Rx_Event (uint32_t event);
static USART_DMA USART0_DMA_Rx = {RTE_USART0_DMA_RX_CH,
                                  RTE_USART0_DMA_RX_PERI,
                                  RTE_USART0_DMA_RX_PERI_SEL,
                                  USART0_GPDMA_Rx_Event};
#endif

static const USART_RESOURCES USART0_Resources = {
  {     // Capabilities
    1,  // supports UART (Asynchronous) mode
#if (RTE_USART0_UCLK_PIN_EN == 1)
    1,  // supports Synchronous Master mode
    1,  // supports Synchronous Slave mode
#else
    0,  // supports Synchronous Master mode
    0,  // supports Synchronous Slave mode
#endif
    1,  // supports UART Single-wire mode
    0,  // supports UART IrDA mode
    1,  // supports UART Smart Card mode
#if (RTE_USART0_UCLK_PIN_EN == 1)
    1,  // Smart Card Clock generator
#else
    0,
#endif
    0,  // RTS Flow Control available
    0,  // CTS Flow Control available
    0,  // Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
#if ((RTE_USART0_DMA_RX_EN == 1) || (USART0_TRIG_LVL == USART_TRIG_LVL_1))
    0,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#else
    1,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#endif
    0,  // RTS Line: 0=not available, 1=available
    0,  // CTS Line: 0=not available, 1=available
    0,  // DTR Line: 0=not available, 1=available
    0,  // DSR Line: 0=not available, 1=available
    0,  // DCD Line: 0=not available, 1=available
    0,  // RI Line: 0=not available, 1=available
    0,  // Signal CTS change event: \ref ARM_USART_EVENT_CTS
    0,  // Signal DSR change event: \ref ARM_USART_EVENT_DSR
    0,  // Signal DCD change event: \ref ARM_USART_EVENT_DCD
    0,  // Signal RI change event: \ref ARM_USART_EVENT_RI
  },
    LPC_USART0,
    NULL,
  {     // USART Pin Configuration
    &USART0_pin_tx,
    &USART0_pin_rx,
#if (RTE_USART0_UCLK_PIN_EN == 1)
    &USART0_pin_clk,
#else
    NULL,
#endif
    NULL, NULL, NULL, NULL, NULL, NULL,
  },
  {     // USART Clocks Configuration
    &LPC_CCU1->CLK_M3_USART0_CFG,
    &LPC_CCU1->CLK_M3_USART0_STAT,
    &LPC_CCU2->CLK_APB0_USART0_CFG,
    &LPC_CCU2->CLK_APB0_USART0_STAT,
    &LPC_CGU->BASE_UART0_CLK,
  },
  {    // USART Reset Configuration
    (1 << 12),
    &LPC_RGU->RESET_CTRL1,
    &LPC_RGU->RESET_ACTIVE_STATUS1,
  },
    USART0_IRQn,
    USART0_TRIG_LVL,
#if (RTE_USART0_DMA_TX_EN == 1)
    &USART0_DMA_Tx,
#else
    NULL,
#endif
#if (RTE_USART0_DMA_RX_EN == 1)
    &USART0_DMA_Rx,
#else
    NULL,
#endif
    &USART0_Info,
    USART0_SC_OVERSAMPLING_RATIO
};
#endif

// UART1
#if (RTE_UART1)
static USART_INFO USART1_Info = {0};
static PIN_ID USART1_pin_tx  = { RTE_UART1_TX_PORT,   RTE_UART1_TX_BIT,  RTE_UART1_TX_FUNC };
static PIN_ID USART1_pin_rx  = { RTE_UART1_RX_PORT,   RTE_UART1_RX_BIT,  RTE_UART1_RX_FUNC };
#if (RTE_UART1_CTS_PIN_EN == 1)
static PIN_ID USART1_pin_cts = { RTE_UART1_CTS_PORT,  RTE_UART1_CTS_BIT, RTE_UART1_CTS_FUNC };
#endif
#if (RTE_UART1_RTS_PIN_EN == 1)
static PIN_ID USART1_pin_rts = { RTE_UART1_RTS_PORT,  RTE_UART1_RTS_BIT, RTE_UART1_RTS_FUNC };
#endif
#if (RTE_UART1_DCD_PIN_EN == 1)
static PIN_ID USART1_pin_dcd = { RTE_UART1_DCD_PORT,  RTE_UART1_DCD_BIT, RTE_UART1_DCD_FUNC };
#endif
#if (RTE_UART1_DSR_PIN_EN == 1)
static PIN_ID USART1_pin_dsr = { RTE_UART1_DSR_PORT,  RTE_UART1_DSR_BIT, RTE_UART1_DSR_FUNC };
#endif
#if (RTE_UART1_DTR_PIN_EN == 1)
static PIN_ID USART1_pin_dtr = { RTE_UART1_DTR_PORT,  RTE_UART1_DTR_BIT, RTE_UART1_DTR_FUNC };
#endif
#if (RTE_UART1_RI_PIN_EN == 1)
static PIN_ID USART1_pin_ri  = { RTE_UART1_RI_PORT,   RTE_UART1_RI_BIT,  RTE_UART1_RI_FUNC };
#endif

#if (RTE_UART1_DMA_TX_EN == 1)
void USART1_GPDMA_Tx_Event (uint32_t event);
static USART_DMA USART1_DMA_Tx = {RTE_UART1_DMA_TX_CH,
                                  RTE_UART1_DMA_TX_PERI,
                                  RTE_UART1_DMA_TX_PERI_SEL,
                                  USART1_GPDMA_Tx_Event};
#endif
#if (RTE_UART1_DMA_RX_EN == 1)
void USART1_GPDMA_Rx_Event (uint32_t event);
static USART_DMA USART1_DMA_Rx = {RTE_UART1_DMA_RX_CH,
                                  RTE_UART1_DMA_RX_PERI,
                                  RTE_UART1_DMA_RX_PERI_SEL,
                                  USART1_GPDMA_Rx_Event};
#endif

static const USART_RESOURCES USART1_Resources = {
  {     // Capabilities
    1,  // supports UART (Asynchronous) mode 
    0,  // supports Synchronous Master mode
    0,  // supports Synchronous Slave mode
    0,  // supports UART Single-wire mode
    0,  // supports UART IrDA mode
    0,  // supports UART Smart Card mode
    0,  // Smart Card Clock generator
#if (RTE_UART1_RTS_PIN_EN == 1)
    1,  // RTS Flow Control available
#else
    0,  // RTS Flow Control available
#endif
#if (RTE_UART1_CTS_PIN_EN == 1)
    1,  // CTS Flow Control available
#else
    0,  // CTS Flow Control available
#endif
    0,  // Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
#if ((RTE_UART1_DMA_RX_EN == 1) || (USART1_TRIG_LVL == USART_TRIG_LVL_1))
    0,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#else
    1,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#endif
#if (RTE_UART1_RTS_PIN_EN == 1)
    1,  // RTS Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_CTS_PIN_EN == 1)
    1,  // CTS Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_DTR_PIN_EN == 1)
    1,  // DTR Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_DSR_PIN_EN == 1)
    1,  // DSR Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_DCD_PIN_EN == 1)
    1,  // DCD Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_RI_PIN_EN == 1)
    1,  // RI Line: 0=not available, 1=available
#else
    0,
#endif
#if (RTE_UART1_CTS_PIN_EN == 1)
    1,  // Signal CTS change event: \ref ARM_USART_EVENT_CTS
#else
    0,
#endif
#if (RTE_UART1_DSR_PIN_EN == 1)
    1,  // Signal DSR change event: \ref ARM_USART_EVENT_DSR
#else
    0,
#endif
#if (RTE_UART1_DCD_PIN_EN == 1)
    1,  // Signal DCD change event: \ref ARM_USART_EVENT_DCD
#else
    0,
#endif
#if (RTE_UART1_RI_PIN_EN == 1)
    1,  // Signal RI change event: \ref ARM_USART_EVENT_RI
#else
    0,
#endif
  },
    (LPC_USARTn_Type *)LPC_UART1,
    LPC_UART1,
  {     // USART Pin Configuration
    &USART1_pin_tx,
    &USART1_pin_rx,
    NULL,
#if (RTE_UART1_CTS_PIN_EN == 1)
    &USART1_pin_cts,
#else
    NULL,
#endif
#if (RTE_UART1_RTS_PIN_EN == 1)
    &USART1_pin_rts,
#else
    NULL,
#endif
#if (RTE_UART1_DCD_PIN_EN == 1)
    &USART1_pin_dcd,
#else
    NULL,
#endif
#if (RTE_UART1_DSR_PIN_EN == 1)
    &USART1_pin_dsr,
#else
    NULL,
#endif
#if (RTE_UART1_DTR_PIN_EN == 1)
    &USART1_pin_dtr,
#else
    NULL,
#endif
#if (RTE_UART1_RI_PIN_EN == 1)
    &USART1_pin_ri,
#else
    NULL,
#endif
  },
  {     // USART Clocks Configuration
    &LPC_CCU1->CLK_M3_UART1_CFG,
    &LPC_CCU1->CLK_M3_UART1_STAT,
    &LPC_CCU2->CLK_APB0_UART1_CFG,
    &LPC_CCU2->CLK_APB0_UART1_STAT,
    &LPC_CGU->BASE_UART1_CLK,
  },
  {    // USART Reset Configuration
    (1 << 13),
    &LPC_RGU->RESET_CTRL1,
    &LPC_RGU->RESET_ACTIVE_STATUS1,
  },
    UART1_IRQn,
    USART1_TRIG_LVL,
#if (RTE_UART1_DMA_TX_EN == 1)
    &USART1_DMA_Tx,
#else
    NULL,
#endif
#if (RTE_UART1_DMA_RX_EN == 1)
    &USART1_DMA_Rx,
#else
    NULL,
#endif
    &USART1_Info,
    0
};
#endif

// USART2
#if (RTE_USART2)
static USART_INFO USART2_Info = {0};
static PIN_ID USART2_pin_tx  = { RTE_USART2_TX_PORT,   RTE_USART2_TX_BIT,   RTE_USART2_TX_FUNC };
static PIN_ID USART2_pin_rx  = { RTE_USART2_RX_PORT,   RTE_USART2_RX_BIT,   RTE_USART2_RX_FUNC };
#if (RTE_USART2_UCLK_PIN_EN == 1)
static PIN_ID USART2_pin_clk = { RTE_USART2_UCLK_PORT, RTE_USART2_UCLK_BIT, RTE_USART2_UCLK_FUNC };
#endif

#if (RTE_USART2_DMA_TX_EN == 1)
void USART2_GPDMA_Tx_Event (uint32_t event);
static USART_DMA USART2_DMA_Tx = {RTE_USART2_DMA_TX_CH,
                                  RTE_USART2_DMA_TX_PERI,
                                  RTE_USART2_DMA_TX_PERI_SEL,
                                  USART2_GPDMA_Tx_Event};
#endif
#if (RTE_USART2_DMA_RX_EN == 1)
void USART2_GPDMA_Rx_Event (uint32_t event);
static USART_DMA USART2_DMA_Rx = {RTE_USART2_DMA_RX_CH,
                                  RTE_USART2_DMA_RX_PERI,
                                  RTE_USART2_DMA_RX_PERI_SEL,
                                  USART2_GPDMA_Rx_Event};
#endif

static const USART_RESOURCES USART2_Resources = {
  {     // Capabilities
    1,  // supports UART (Asynchronous) mode
#if (RTE_USART1_UCLK_PIN_EN == 1)
    1,  // supports Synchronous Master mode
    1,  // supports Synchronous Slave mode
#else
    0,  // supports Synchronous Master mode
    0,  // supports Synchronous Slave mode
#endif
    1,  // supports UART Single-wire mode
    0,  // supports UART IrDA mode
    1,  // supports UART Smart Card mode
#if (RTE_USART2_UCLK_PIN_EN == 1)
    1,  // Smart Card Clock generator
#else
    0,
#endif
    0,  // RTS Flow Control available
    0,  // CTS Flow Control available
    0,  // Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
#if ((RTE_USART2_DMA_RX_EN == 1) || (USART2_TRIG_LVL == USART_TRIG_LVL_1))
    0,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#else
    1,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#endif
    0,  // RTS Line: 0=not available, 1=available
    0,  // CTS Line: 0=not available, 1=available
    0,  // DTR Line: 0=not available, 1=available
    0,  // DSR Line: 0=not available, 1=available
    0,  // DCD Line: 0=not available, 1=available
    0,  // RI Line: 0=not available, 1=available
    0,  // Signal CTS change event: \ref ARM_USART_EVENT_CTS
    0,  // Signal DSR change event: \ref ARM_USART_EVENT_DSR
    0,  // Signal DCD change event: \ref ARM_USART_EVENT_DCD
    0,  // Signal RI change event: \ref ARM_USART_EVENT_RI
  },
    LPC_USART2,
    NULL,
  {     // USART Pin Configuration
    &USART2_pin_tx,
    &USART2_pin_rx,
#if (RTE_USART2_UCLK_PIN_EN == 1)
    &USART2_pin_clk,
#else
    NULL,
#endif
    NULL, NULL, NULL, NULL, NULL, NULL,
  },
  {     // USART Clocks Configuration
    &LPC_CCU1->CLK_M3_USART2_CFG,
    &LPC_CCU1->CLK_M3_USART2_STAT,
    &LPC_CCU2->CLK_APB2_USART2_CFG,
    &LPC_CCU2->CLK_APB2_USART2_STAT,
    &LPC_CGU->BASE_UART2_CLK,
  },
  {    // USART Reset Configuration
    (1 << 14),
    &LPC_RGU->RESET_CTRL1,
    &LPC_RGU->RESET_ACTIVE_STATUS1,
  },
    USART2_IRQn,
    USART2_TRIG_LVL,
#if (RTE_USART2_DMA_TX_EN == 1)
    &USART2_DMA_Tx,
#else
    NULL,
#endif
#if (RTE_USART2_DMA_RX_EN == 1)
    &USART2_DMA_Rx,
#else
    NULL,
#endif
    &USART2_Info,
    USART2_SC_OVERSAMPLING_RATIO
};
#endif

// USART3
#if (RTE_USART3)
static USART_INFO USART3_Info = {0};
static PIN_ID USART3_pin_tx  = { RTE_USART3_TX_PORT,   RTE_USART3_TX_BIT,   RTE_USART3_TX_FUNC };
static PIN_ID USART3_pin_rx  = { RTE_USART3_RX_PORT,   RTE_USART3_RX_BIT,   RTE_USART3_RX_FUNC };
#if (RTE_USART3_UCLK_PIN_EN == 1)
static PIN_ID USART3_pin_clk = { RTE_USART3_UCLK_PORT, RTE_USART3_UCLK_BIT, RTE_USART3_UCLK_FUNC };
#endif

#if (RTE_USART3_DMA_TX_EN == 1)
void USART3_GPDMA_Tx_Event (uint32_t event);
static USART_DMA USART3_DMA_Tx = {RTE_USART3_DMA_TX_CH,
                                  RTE_USART3_DMA_TX_PERI,
                                  RTE_USART3_DMA_TX_PERI_SEL,
                                  USART3_GPDMA_Tx_Event};
#endif
#if (RTE_USART3_DMA_RX_EN == 1)
void USART3_GPDMA_Rx_Event (uint32_t event);
static USART_DMA USART3_DMA_Rx = {RTE_USART3_DMA_RX_CH,
                                  RTE_USART3_DMA_RX_PERI,
                                  RTE_USART3_DMA_RX_PERI_SEL,
                                  USART3_GPDMA_Rx_Event};
#endif

static const USART_RESOURCES USART3_Resources = {
  {     // Capabilities
    1,  // supports UART (Asynchronous) mode 
#if (RTE_USART3_UCLK_PIN_EN == 1)
    1,  // supports Synchronous Master mode
    1,  // supports Synchronous Slave mode
#else
    0,  // supports Synchronous Master mode
    0,  // supports Synchronous Slave mode
#endif
    1,  // supports UART Single-wire mode
    1,  // supports UART IrDA mode
    1,  // supports UART Smart Card mode
#if (RTE_USART3_UCLK_PIN_EN == 1)
    1,  // Smart Card Clock generator
#else
    0,
#endif
    0,  // RTS Flow Control available
    0,  // CTS Flow Control available
    0,  // Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
#if ((RTE_USART3_DMA_RX_EN == 1) || (USART3_TRIG_LVL == USART_TRIG_LVL_1))
    0,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#else
    1,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
#endif
    0,  // RTS Line: 0=not available, 1=available
    0,  // CTS Line: 0=not available, 1=available
    0,  // DTR Line: 0=not available, 1=available
    0,  // DSR Line: 0=not available, 1=available
    0,  // DCD Line: 0=not available, 1=available
    0,  // RI Line: 0=not available, 1=available
    0,  // Signal CTS change event: \ref ARM_USART_EVENT_CTS
    0,  // Signal DSR change event: \ref ARM_USART_EVENT_DSR
    0,  // Signal DCD change event: \ref ARM_USART_EVENT_DCD
    0,  // Signal RI change event: \ref ARM_USART_EVENT_RI
  },
    LPC_USART3,
    NULL,
  {     // USART Pin Configuration
    &USART3_pin_tx,
    &USART3_pin_rx,
#if (RTE_USART3_UCLK_PIN_EN == 1)
    &USART3_pin_clk,
#else
    NULL,
#endif
    NULL, NULL, NULL, NULL, NULL, NULL,
  },
  {     // USART Clocks Configuration
    &LPC_CCU1->CLK_M3_USART3_CFG,
    &LPC_CCU1->CLK_M3_USART3_STAT,
    &LPC_CCU2->CLK_APB2_USART3_CFG,
    &LPC_CCU2->CLK_APB2_USART3_STAT,
    &LPC_CGU->BASE_UART3_CLK,
  },
  {    // USART Reset Configuration
    (1 << 15),
    &LPC_RGU->RESET_CTRL1,
    &LPC_RGU->RESET_ACTIVE_STATUS1,
  },
    USART3_IRQn,
    USART3_TRIG_LVL,
#if (RTE_USART3_DMA_TX_EN == 1)
    &USART3_DMA_Tx,
#else
    NULL,
#endif
#if (RTE_USART3_DMA_RX_EN == 1)
    &USART3_DMA_Rx,
#else
    NULL,
#endif
    &USART3_Info,
    USART3_SC_OVERSAMPLING_RATIO
};
#endif


// Extern Function
extern uint32_t GetClockFreq (uint32_t clk_src);

// Local Function
/**
  \fn          int32_t USART_SetBaudrate (uint32_t         baudrate,
                                          USART_RESOURCES *usart)
  \brief       Set baudrate dividers
  \param[in]   baudrate  Usart baudrate
  \param[in]   usart     Pointer to USART resources)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t USART_SetBaudrate (uint32_t         baudrate,
                           USART_RESOURCES *usart) {
  uint8_t  add, mul, add_mul_best, oversampling_fract_best;
  uint16_t latch_div_best, oversampling, oversampling_best;
  uint32_t i, j, pclk, div, tmp_div, latch_div, delta, delta_best, val;

  pclk = GetClockFreq ((*usart->clk.base_clk >> 24) & 0x1FU);

  // Calculate fixed point divider (12 LSBs are fractional part)
  div       = (uint32_t)(((uint64_t)pclk << FRACT_BITS) / (uint64_t)baudrate);

  delta_best              = 0xFFFFFFFFU;
  oversampling_fract_best = 0U;

  // SmartCard mode
  if (usart->info->mode == ARM_USART_MODE_SMART_CARD) {
    oversampling_best = usart->sc_oversamp;
    for (i = 0; i < FRACT_DIV_LOOKUP_TABLE_SZ; i++) {
      // Calculate latch divider (latch_div = div / (fract_div * oversampling(16)))
      latch_div = ((div / fract_div_lookup_table[i].val) / oversampling_best);

      if (latch_div > 65535U) { continue; }

      for (j = 0U; j < 2U; j++) {
        // Which latch divider value is more appropriate:
        //    latch_div or latch_div + 1 (rounded up)

        if (latch_div < 3U) { latch_div++; continue; }

        // Calculate actual divider (temp_div = latch_div * fract_div * oversampling(16))
        tmp_div   = (latch_div * fract_div_lookup_table[i].val) / oversampling_best;

        // Calculate delta
        if (div > tmp_div) { delta = div - tmp_div; }
        else               { delta = tmp_div - div; }

        // Check if delta is better than best delta
        if (delta < delta_best) {
          delta_best        = delta;
          add_mul_best      = fract_div_lookup_table[i].add_mul;
          latch_div_best    = latch_div;
        }
        latch_div++;
      }
    }
  } else {

    // Oversampling is fixed to 16
    // divider = oversampling * latch divider * fractional divider = 16 * latch_div * fract_div
    if (div >= FIXED_OVERSAMPLING_DIVIDER_LIMIT) {
      latch_div = div >> (FRACT_BITS + 4U);
      if ((div == (latch_div << (FRACT_BITS + 4U))) && ((latch_div >> 4) <= 0xFFFFU)) {
        // Fractional part of divider is 0
        delta_best        = 0U;
        add_mul_best      = 0U;
        latch_div_best    = latch_div;
        oversampling_best = 16U;
      } else {
        // Divider larger than 48, can be accomplished with configurable
        // latch and fractional divider, and fixed oversampling to 16

        for (i = 0U; i < FRACT_DIV_LOOKUP_TABLE_SZ; i++) {
          // Calculate latch divider (latch_div = div / (fract_div * oversampling(16)))
          latch_div = ((div / fract_div_lookup_table[i].val) >> 4);

          if (latch_div > 65535U) { continue; }

          for (j = 0U; j < 2U; j++) {
            // Which latch divider value is more appropriate: 
            //    latch_div or latch_div + 1 (rounded up)

            if (latch_div < 3U) { latch_div++; continue; }

            // Calculate actual divider (temp_div = latch_div * fract_div * oversampling(16))
            tmp_div   = (latch_div * fract_div_lookup_table[i].val) << 4;

            // Calculate delta
            if (div > tmp_div) { delta = div - tmp_div; }
            else               { delta = tmp_div - div; }

            // Check if delta is better than best delta
            if (delta < delta_best) {
              delta_best        = delta;
              add_mul_best      = fract_div_lookup_table[i].add_mul;
              latch_div_best    = latch_div;
              oversampling_best = 16U;
            }
            latch_div++;
          }
        }
      }
    } else {
      // Check if oversampling register is available
      if (usart->uart_reg != NULL) {return - 1; }

      if (div > INTEGER_OVERSAMPLING_DIVIDER_LIMIT) {
        // Oversampling ratio is integer value

        // Set oversampling
        if      (div > (48U << 12)) { oversampling = 15U; }
        else if (div > (45U << 12)) { oversampling = 14U; }
        else if (div > (42U << 12)) { oversampling = 13U; }
        else if (div > (38U << 12)) { oversampling = 12U; }
        else if (div > (35U << 12)) { oversampling = 11U; }
        else if (div > (32U << 12)) { oversampling = 10U; }
        else if (div > (29U << 12)) { oversampling =  9U; }
        else if (div > (26U << 12)) { oversampling =  8U; }
        else if (div > (23U << 12)) { oversampling =  7U; }
        else if (div > (19U << 12)) { oversampling =  6U; }
        else if (div > (16U << 12)) { oversampling =  5U; }
        else                        { oversampling =  4U; }

        // Check if divider is integer value
        tmp_div   = (div / oversampling);
        if ((tmp_div & FRACT_MASK) == 0U) {
          // Fractional part of divider is 0
          delta_best        = 0U;
          add_mul_best      = 0U;
          latch_div_best    = tmp_div >> FRACT_BITS;
          oversampling_best = oversampling;
        } else {
          // Fractional part of divider is not 0

          latch_div = 3U;

          for (i = 0U; i < FRACT_DIV_LOOKUP_TABLE_SZ; i++) {

            // Calculate actual divider (temp_div = latch_div * fract_div * oversampling)
            tmp_div   = latch_div * fract_div_lookup_table[i].val * oversampling;

            // Calculate delta
            if (div > tmp_div) { delta = div - tmp_div; }
            else               { delta = tmp_div - div; }

            // Check if delta is better than best delta
            if (delta < delta_best) {
              delta_best        = delta;
              add_mul_best      = fract_div_lookup_table[i].add_mul;
              latch_div_best    = latch_div;
              oversampling_best = oversampling;
            }
          }
        }

        //tmp_div = latch_div_best * fract_best * oversampling_best;
        add = add_mul_best & 0x0FU;
        mul = add_mul_best >> 4;
        tmp_div = ((latch_div_best * (mul + add) * oversampling_best) << 12) / mul;
        if ((tmp_div & FRACT_MASK) == 0U) {
          // If best possible divider is integer value, make sure
          // fractional divider is 0 and max oversampling is used

          oversampling = 16U;
          do {
            if (((tmp_div / oversampling) & FRACT_MASK) == 0U) {
              // Fractional part of divider is 0

              tmp_div /= oversampling;
              add_mul_best      = 0U;
              latch_div_best    = tmp_div >> FRACT_BITS;
              oversampling_best = oversampling;
              break;
            }
            oversampling--;
          } while (oversampling >= 4U);
        }
      } else {
        // Oversampling ratio can be fractional,
        // latch divider is 1 and fractional divider is not used

        // Oversampling step
        val = (125U << FRACT_BITS) / 1000U;
        oversampling = 13U << FRACT_BITS;
        do {
          // Calculate delta
          if (div > oversampling) { delta = div - oversampling; }
          else                    { delta = oversampling - div; }

          // Check if delta is better than best delta
          if (delta < delta_best) {
            delta_best        = delta;
            add_mul_best      = 0U;
            latch_div_best    = 1U;
            oversampling_best = oversampling;
          }

          oversampling -= val;
        } while (oversampling >= (4U << FRACT_BITS));

        oversampling_fract_best = ((oversampling_best & FRACT_MASK) << 3) >> FRACT_BITS;
        oversampling_best       =   oversampling_best >> FRACT_BITS;
      }
    }
  }

  if (((delta_best * 100U) / div) > USART_MAX_BAUDRATE_ERROR) { return -1; }

  usart->reg->LCR |= USART_LCR_DLAB;
  usart->reg->DLM  = ((latch_div_best >> 8) & 0xFFU) << USART_DLM_DLMSB_POS;
  usart->reg->DLL  = (latch_div_best & USART_DLL_DLLSB_MSK) << USART_DLL_DLLSB_POS;
  // Reset DLAB bit
  usart->reg->LCR &= (~USART_LCR_DLAB);
  usart->reg->FDR  = ((add_mul_best & USART_FDR_MULVAL_MSK)  |
                      (add_mul_best & USART_FDR_DIVADDVAL_MSK));

  // Check if oversampling register is available
  if (usart->uart_reg == NULL) {
    usart->reg->OSR = ((oversampling_best - 1)  << USART_OSR_OSINT_POS) |
                      ( oversampling_fract_best << USART_OSR_OSFRAC_POS);
  }

  usart->info->baudrate = baudrate;

  return 0;
}

/**
  \fn          uint32_t USART_RxLineIntHandler (USART_RESOURCES *usart)
  \brief       Receive line interrupt handler
  \param[in]   usart     Pointer to USART resources
  \return      Rx Line event mask
*/
static uint32_t USART_RxLineIntHandler (USART_RESOURCES *usart) {
  uint32_t lsr, event;

  event = 0U;
  lsr   = usart->reg->LSR  & USART_LSR_LINE_INT;

  // OverRun error
  if (lsr & USART_LSR_OE) {
    usart->info->rx_status.rx_overflow = 1U;
    event |= ARM_USART_EVENT_RX_OVERFLOW;

    // Sync Slave mode: If Transmitter enabled, signal TX underflow
    if (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE) {
      if (usart->info->xfer.send_active != 0U) {
        event |= ARM_USART_EVENT_TX_UNDERFLOW;
      }
    }
  }

  // Parity error
  if (lsr & USART_LSR_PE) {
    usart->info->rx_status.rx_parity_error = 1U;
    event |= ARM_USART_EVENT_RX_PARITY_ERROR;
  }

  // Break detected
  if (lsr & USART_LSR_BI) {
    usart->info->rx_status.rx_break = 1U;
    event |= ARM_USART_EVENT_RX_BREAK;
  }

  // Framing error
  else {
    if(lsr & USART_LSR_FE) {
      usart->info->rx_status.rx_framing_error = 1U;
      event |= ARM_USART_EVENT_RX_FRAMING_ERROR;
    }
  }

  return event;
}

// Function Prototypes
static int32_t USART_Receive (void            *data,
                              uint32_t         num,
                              USART_RESOURCES *usart);


// USART Driver functions

/**
  \fn          ARM_DRIVER_VERSION USARTx_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USARTx_GetVersion (void) {
  return usart_driver_version;
}

/**
  \fn          ARM_USART_CAPABILITIES USART_GetCapabilities (USART_RESOURCES *usart)
  \brief       Get driver capabilities
  \param[in]   usart     Pointer to USART resources
  \return      \ref ARM_USART_CAPABILITIES
*/
static ARM_USART_CAPABILITIES USART_GetCapabilities (USART_RESOURCES *usart) {
  return usart->capabilities;
}

/**
  \fn          int32_t USART_Initialize (ARM_USART_SignalEvent_t  cb_event
                                         USART_RESOURCES         *usart)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref ARM_USART_SignalEvent
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Initialize (ARM_USART_SignalEvent_t  cb_event,
                                 USART_RESOURCES         *usart) {

  if (usart->info->flags & USART_FLAG_INITIALIZED) {
    // Driver is already initialized
    return ARM_DRIVER_OK;
  }

  // Initialize USART Run-time Resources
  usart->info->cb_event = cb_event;

  usart->info->rx_status.rx_busy          = 0U;
  usart->info->rx_status.rx_overflow      = 0U;
  usart->info->rx_status.rx_break         = 0U;
  usart->info->rx_status.rx_framing_error = 0U;
  usart->info->rx_status.rx_parity_error  = 0U;

  usart->info->xfer.send_active           = 0U;
  usart->info->xfer.tx_def_val            = 0U;

  // Configure CTS pin
  if (usart->capabilities.cts) {
    SCU_PinConfigure(usart->pins.cts->port, usart->pins.cts->num,
                     SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN |
                     SCU_PIN_CFG_MODE(usart->pins.cts->config_val));
  }

  // Configure RTS pin
  if (usart->capabilities.rts) {
    SCU_PinConfigure(usart->pins.rts->port, usart->pins.rts->num,
                     SCU_PIN_CFG_MODE(usart->pins.rts->config_val));
  }

  // Configure DCD pin
  if (usart->capabilities.dcd) {
    SCU_PinConfigure(usart->pins.dcd->port, usart->pins.dcd->num,
                     SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN |
                     SCU_PIN_CFG_MODE(usart->pins.dcd->config_val));
  }

  // Configure DSR pin
  if (usart->capabilities.dsr) {
    SCU_PinConfigure(usart->pins.dsr->port, usart->pins.dsr->num,
                     SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN |
                     SCU_PIN_CFG_MODE(usart->pins.dsr->config_val));
  }

  // Configure DTR pin
  if (usart->capabilities.dtr) {
    SCU_PinConfigure(usart->pins.dtr->port, usart->pins.dtr->num,
                     SCU_PIN_CFG_MODE(usart->pins.dtr->config_val));
  }

  // Configure RI pin
  if (usart->capabilities.ri) {
    SCU_PinConfigure(usart->pins.ri->port, usart->pins.ri->num,
                     SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN |
                     SCU_PIN_CFG_MODE(usart->pins.ri->config_val));
  }

  // DMA Initialize
  if (usart->dma_tx || usart->dma_rx) { GPDMA_Initialize (); }

  usart->info->flags = USART_FLAG_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Uninitialize (USART_RESOURCES *usart)
  \brief       De-initialize USART Interface.
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Uninitialize (USART_RESOURCES *usart) {

  // Reset TX pin configuration
  SCU_PinConfigure(usart->pins.tx->port, usart->pins.tx->num  , 0U);

  // Reset RX pin configuration
  SCU_PinConfigure(usart->pins.rx->port, usart->pins.rx->num  , 0U);

  // Reset CLK pin configuration
  if (usart->pins.clk) {
    SCU_PinConfigure(usart->pins.clk->port, usart->pins.clk->num, 0U);
  }

  // Reset CTS pin configuration
  if (usart->capabilities.cts) {
    SCU_PinConfigure(usart->pins.cts->port, usart->pins.cts->num, 0U);
  }

  // Reset RTS pin configuration
  if (usart->capabilities.rts) {
    SCU_PinConfigure(usart->pins.rts->port, usart->pins.rts->num, 0U);
  }

  // Configure DCD pin configuration
  if (usart->capabilities.dcd) {
    SCU_PinConfigure(usart->pins.dcd->port, usart->pins.dcd->num, 0U);
  }

  // Reset DSR pin configuration
  if (usart->capabilities.dsr) {
    SCU_PinConfigure(usart->pins.dsr->port, usart->pins.dsr->num, 0U);
  }

  // Reset DTR pin configuration
  if (usart->capabilities.dtr) {
    SCU_PinConfigure(usart->pins.dtr->port, usart->pins.dtr->num, 0U);
  }

  // Reset RI pin configuration
  if (usart->capabilities.ri) {
    SCU_PinConfigure(usart->pins.ri->port, usart->pins.ri->num, 0U);
  }

  // DMA Uninitialize
  if (usart->dma_tx || usart->dma_rx) { GPDMA_Uninitialize (); }

  // Reset USART status flags
  usart->info->flags = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_PowerControl (ARM_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \param[in]   usart  Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_PowerControl (ARM_POWER_STATE  state,
                                   USART_RESOURCES *usart) {
  uint32_t val;

  switch (state) {
    case ARM_POWER_OFF:
      // Disable USART IRQ
      NVIC_DisableIRQ(usart->irq_num);

      // If DMA mode - disable TX DMA channel
      if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
        GPDMA_ChannelDisable (usart->dma_tx->channel);
      }

      // If DMA mode - disable DMA channel
      if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
        GPDMA_ChannelDisable (usart->dma_rx->channel);
      }

      // Reset USART peripheral
      *usart->rst.reg_cfg = usart->rst.reg_cfg_val;
      while ((*(usart->rst.reg_stat) & usart->rst.reg_cfg_val) == 0U);

      // Disable USART peripheral clock
      *usart->clk.peri_cfg &= ~1U;
      while (*usart->clk.peri_cfg & 1U);
        
      // Disable USART register interface clock
      *usart->clk.reg_cfg &= ~1U;
      while (*usart->clk.reg_cfg & 1U);

      // Clear pending USART interrupts in NVIC
      NVIC_ClearPendingIRQ(usart->irq_num);

      // Clear driver variables
      usart->info->rx_status.rx_busy          = 0U;
      usart->info->rx_status.rx_overflow      = 0U;
      usart->info->rx_status.rx_break         = 0U;
      usart->info->rx_status.rx_framing_error = 0U;
      usart->info->rx_status.rx_parity_error  = 0U;
      usart->info->xfer.send_active           = 0U;

      usart->info->flags &= ~USART_FLAG_POWERED;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((usart->info->flags & USART_FLAG_INITIALIZED) == 0U) { return ARM_DRIVER_ERROR; }
      if ((usart->info->flags & USART_FLAG_POWERED)     != 0U) { return ARM_DRIVER_OK; }

      // Connect USART base clock to PLL1
      *usart->clk.base_clk = (1U    << 11) |
                             (0x09U << 24) ;

      // Enable USART register interface clock
      *usart->clk.reg_cfg |= CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while ((*usart->clk.reg_cfg & CCU_CLK_CFG_RUN) == 0U);

      // Enable USART peripheral clock
      *usart->clk.peri_cfg |= CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
      while (( *usart->clk.peri_cfg & CCU_CLK_CFG_RUN) == 0U);

      // Reset USART peripheral
      *usart->rst.reg_cfg = usart->rst.reg_cfg_val;
      while ((*(usart->rst.reg_stat) & usart->rst.reg_cfg_val) == 0U);

      // Disable transmitter
      usart->reg->TER &= ~USART_TER_TXEN;

      // Disable receiver
      usart->reg->RS485CTRL |= USART_RS485CTRL_RXDIS;

      // Disable interrupts
      usart->reg->IER = 0U;

      // Configure FIFO Control register
      // Set trigger level
      val = (usart->trig_lvl & USART_FCR_RXTRIGLVL_MSK) | USART_FCR_FIFOEN;

      if (usart->dma_rx || usart->dma_tx) {
        val |= USART_FCR_DMAMODE;
      }

      usart->reg->FCR = val;

#if (RTE_UART1)
      // Enable modem lines status interrupts (only UART1)
      if (usart->uart_reg) {
        if (usart->capabilities.cts || usart->capabilities.dcd ||
            usart->capabilities.dsr || usart->capabilities.ri) {
          usart->uart_reg->IER |= UART_IER_MSIE;
        }
      }
#endif

      // Clear driver variables
      usart->info->rx_status.rx_busy          = 0U;
      usart->info->rx_status.rx_overflow      = 0U;
      usart->info->rx_status.rx_break         = 0U;
      usart->info->rx_status.rx_framing_error = 0U;
      usart->info->rx_status.rx_parity_error  = 0U;

      usart->info->mode                       = 0U;
      usart->info->flags                      = 0U;
      usart->info->xfer.send_active           = 0U;

      usart->info->flags = USART_FLAG_POWERED | USART_FLAG_INITIALIZED;

      // Clear and Enable USART IRQ
      NVIC_ClearPendingIRQ(usart->irq_num);
      NVIC_EnableIRQ(usart->irq_num);

      break;

    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Send (const void            *data,
                                         uint32_t         num,
                                         USART_RESOURCES *usart)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Send (const void            *data,
                                 uint32_t         num,
                                 USART_RESOURCES *usart) {
  int32_t stat, source_inc, val;

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  if (usart->info->xfer.send_active != 0U) {
    // Send is not completed yet
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set Send active flag
  usart->info->xfer.send_active = 1U;

  // For DMA mode: source increment
  source_inc = GPDMA_CH_CONTROL_SI;

  // Synchronous mode
  if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
      (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
    if (usart->info->xfer.sync_mode == 0U) {
      usart->info->xfer.sync_mode = USART_SYNC_MODE_TX;
      // Start dummy reads
      stat = USART_Receive (&usart->info->xfer.rx_dump_val, num, usart);
      if (stat != ARM_DRIVER_OK) { return ARM_DRIVER_ERROR; }

    } else {
      if (usart->info->xfer.sync_mode == USART_SYNC_MODE_RX) {
        // Dummy DMA writes (do not increment source address)
        source_inc = 0U;
      }
    }
  }

  // Save transmit buffer info
  usart->info->xfer.tx_buf = (uint8_t *)data;
  usart->info->xfer.tx_num = num;
  usart->info->xfer.tx_cnt = 0U;

  // DMA mode
  if (usart->dma_tx) {

    // Configure DMA mux
    GPDMA_PeripheralSelect (usart->dma_tx->peripheral, usart->dma_tx->peripheral_sel);

    // Configure DMA channel
    stat = GPDMA_ChannelConfigure (usart->dma_tx->channel,
                                   (uint32_t)data,
                                   (uint32_t)(&(usart->reg->THR)),
                                   num,
                                   GPDMA_CH_CONTROL_SBSIZE(GPDMA_BSIZE_1)                   |
                                   GPDMA_CH_CONTROL_DBSIZE(GPDMA_BSIZE_1)                   |
                                   GPDMA_CH_CONTROL_SWIDTH(GPDMA_WIDTH_BYTE)                |
                                   GPDMA_CH_CONTROL_DWIDTH(GPDMA_WIDTH_BYTE)                |
                                   GPDMA_CH_CONTROL_S                                       |
                                   GPDMA_CH_CONTROL_D                                       |
                                   GPDMA_CH_CONTROL_I                                       |
                                   source_inc,
                                   GPDMA_CH_CONFIG_DEST_PERI(usart->dma_tx->peripheral) |
                                   GPDMA_CH_CONFIG_FLOWCNTRL(GPDMA_TRANSFER_M2P_CTRL_DMA)   |
                                   GPDMA_CH_CONFIG_IE                                       |
                                   GPDMA_CH_CONFIG_ITC                                      |
                                   GPDMA_CH_CONFIG_E,
                                   usart->dma_tx->cb_event);
  if (stat == -1) { return ARM_DRIVER_ERROR; }

  // Interrupt mode
  } else {
    // Fill TX FIFO
    if (usart->reg->LSR & USART_LSR_THRE) {
      val = 16U;
      while ((val--) && (usart->info->xfer.tx_cnt != usart->info->xfer.tx_num)) {
        usart->reg->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt++];
      }
    }

    // Enable transmit holding register empty interrupt
    usart->reg->IER |= USART_IER_THREIE;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Receive (void            *data,
                                      uint32_t         num,
                                      USART_RESOURCES *usart)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Receive (void            *data,
                              uint32_t         num,
                              USART_RESOURCES *usart) {

  int32_t stat, dest_inc;

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  // Check if receiver is busy
  if (usart->info->rx_status.rx_busy == 1U) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set RX busy flag
  usart->info->rx_status.rx_busy = 1U;

  dest_inc = GPDMA_CH_CONTROL_DI;

  // Save number of data to be received
  usart->info->xfer.rx_num = num;

  // Clear RX statuses
  usart->info->rx_status.rx_break          = 0U;
  usart->info->rx_status.rx_framing_error  = 0U;
  usart->info->rx_status.rx_overflow       = 0U;
  usart->info->rx_status.rx_parity_error   = 0U;

  // Save receive buffer info
  usart->info->xfer.rx_buf = (uint8_t *)data;
  usart->info->xfer.rx_cnt =                0U;

  // Synchronous mode
  if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
      (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
    if (usart->info->xfer.sync_mode == USART_SYNC_MODE_TX) {
      // Dummy DMA reads (do not increment destination address)
      dest_inc = 0U;
    }
  }

  // DMA mode
  if (usart->dma_rx) {

    GPDMA_PeripheralSelect (usart->dma_rx->peripheral, usart->dma_rx->peripheral_sel);
    stat = GPDMA_ChannelConfigure (usart->dma_rx->channel,
                                   (uint32_t)&usart->reg->RBR,
                                   (uint32_t)data,
                                   num,
                                   GPDMA_CH_CONTROL_SBSIZE(GPDMA_BSIZE_1)                   |
                                   GPDMA_CH_CONTROL_DBSIZE(GPDMA_BSIZE_1)                   |
                                   GPDMA_CH_CONTROL_SWIDTH(GPDMA_WIDTH_BYTE)                |
                                   GPDMA_CH_CONTROL_DWIDTH(GPDMA_WIDTH_BYTE)                |
                                   GPDMA_CH_CONTROL_S                                       |
                                   GPDMA_CH_CONTROL_D                                       |
                                   GPDMA_CH_CONTROL_I                                       |
                                   dest_inc,
                                   GPDMA_CH_CONFIG_SRC_PERI(usart->dma_rx->peripheral)      |
                                   GPDMA_CH_CONFIG_FLOWCNTRL(GPDMA_TRANSFER_P2M_CTRL_DMA)   |
                                   GPDMA_CH_CONFIG_IE                                       |
                                   GPDMA_CH_CONFIG_ITC                                      |
                                   GPDMA_CH_CONFIG_E,
                                   usart->dma_rx->cb_event);
  if (stat == -1) { return ARM_DRIVER_ERROR; }

  // Interrupt mode
  } else {
    // Enable receive data available interrupt
    usart->reg->IER |= USART_IER_RBRIE;
  }

  // Synchronous mode
  if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
      (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
    if (usart->info->xfer.sync_mode == 0U) {
      usart->info->xfer.sync_mode = USART_SYNC_MODE_RX;
      // Send dummy data
      stat = USART_Send (&usart->info->xfer.tx_def_val, num, usart);
      if (stat != ARM_DRIVER_OK) { return ARM_DRIVER_ERROR; }
    }
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Transfer (const void             *data_out,
                                             void             *data_in,
                                             uint32_t          num,
                                             USART_RESOURCES  *usart)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Transfer (const void             *data_out,
                                     void             *data_in,
                                     uint32_t          num,
                                     USART_RESOURCES  *usart) {
  int32_t status;

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured
    return ARM_DRIVER_ERROR;
  }

  if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
      (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {

    // Set xfer mode
    usart->info->xfer.sync_mode = USART_SYNC_MODE_TX_RX;

    // Receive
    status = USART_Receive (data_in, num, usart);
    if (status != ARM_DRIVER_OK) { return status; }

    // Send
    status = USART_Send (data_out, num, usart);
    if (status != ARM_DRIVER_OK) { return status; }

  } else {
    // Only in synchronous mode
    return ARM_DRIVER_ERROR;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t USART_GetTxCount (USART_RESOURCES *usart)
  \brief       Get transmitted data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items transmitted
*/
static uint32_t USART_GetTxCount (USART_RESOURCES *usart) {
  uint32_t cnt;

  if (usart->dma_tx) {
    cnt = GPDMA_ChannelGetCount (usart->dma_tx->channel);
  } else {
    cnt = usart->info->xfer.tx_cnt;
  }

  return cnt;
}

/**
  \fn          uint32_t USART_GetRxCount (USART_RESOURCES *usart)
  \brief       Get received data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items received
*/
static uint32_t USART_GetRxCount (USART_RESOURCES *usart) {
  uint32_t cnt;

  if (usart->dma_rx) {
    cnt = GPDMA_ChannelGetCount (usart->dma_rx->channel);
  } else {
    cnt = usart->info->xfer.rx_cnt;
  }

  return cnt;
}

/**
  \fn          int32_t USART_Control (uint32_t          control,
                                      uint32_t          arg,
                                      USART_RESOURCES  *usart)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   usart    Pointer to USART resources
  \return      common \ref execution_status and driver specific \ref usart_execution_status
*/
static int32_t USART_Control (uint32_t          control,
                              uint32_t          arg,
                              USART_RESOURCES  *usart) {
  uint32_t val, mode;
  uint32_t syncctrl, hden, icr, scictrl, lcr, mcr;

  if ((usart->info->flags & USART_FLAG_POWERED) == 0U) {
    // USART not powered
    return ARM_DRIVER_ERROR;
  }

  syncctrl = 0U;
  hden     = 0U;
  icr      = 0U;
  scictrl  = 0U;
  lcr      = 0U;

  switch (control & ARM_USART_CONTROL_Msk) {
    // Control TX
    case ARM_USART_CONTROL_TX:
      // Check if TX line available
      if (usart->pins.tx == NULL) { return ARM_DRIVER_ERROR; }
      if (arg) {
        if (usart->info->mode != ARM_USART_MODE_SMART_CARD) {
          // USART TX pin function selected
          SCU_PinConfigure(usart->pins.tx->port, usart->pins.tx->num, SCU_PIN_CFG_INPUT_FILTER_DIS |
                           SCU_PIN_CFG_MODE(usart->pins.tx->config_val));
        }
        usart->info->flags |= USART_FLAG_TX_ENABLED;
        usart->reg->TER    |= USART_TER_TXEN;
      } else {
        usart->info->flags &= ~USART_FLAG_TX_ENABLED;
        usart->reg->TER &= ~USART_TER_TXEN;
        if (usart->info->mode != ARM_USART_MODE_SMART_CARD) {
          // GPIO pin function selected
          SCU_PinConfigure(usart->pins.tx->port, usart->pins.tx->num, SCU_PIN_CFG_INPUT_FILTER_DIS |
                           SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0));
        }
      }
      return ARM_DRIVER_OK;

    // Control RX
    case ARM_USART_CONTROL_RX:
      if (usart->pins.rx == NULL) { return ARM_DRIVER_ERROR; }
      // RX Line interrupt enable (overrun, framing, parity error, break)
      if (arg) {
        if ((usart->info->mode != ARM_USART_MODE_SMART_CARD)   &&
            (usart->info->mode != ARM_USART_MODE_SINGLE_WIRE )) {
          // USART RX pin function selected
          SCU_PinConfigure(usart->pins.rx->port, usart->pins.rx->num,
                           SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_INPUT_FILTER_DIS |
                           SCU_PIN_CFG_MODE(usart->pins.rx->config_val));
        }
        usart->info->flags |= USART_FLAG_RX_ENABLED;
        usart->reg->RS485CTRL &= ~USART_RS485CTRL_RXDIS;
        usart->reg->IER |= USART_IER_RXIE;
      } else {
        usart->info->flags &= ~USART_FLAG_RX_ENABLED;
        usart->reg->RS485CTRL |= USART_RS485CTRL_RXDIS;
        usart->reg->IER &= ~USART_IER_RXIE;
        if ((usart->info->mode != ARM_USART_MODE_SMART_CARD)   &&
            (usart->info->mode != ARM_USART_MODE_SINGLE_WIRE )) {
          // GPIO pin function selected
          SCU_PinConfigure(usart->pins.rx->port, usart->pins.rx->num, SCU_PIN_CFG_INPUT_FILTER_DIS |
                           SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0));
        }
      }
      return ARM_DRIVER_OK;

    // Control break
    case ARM_USART_CONTROL_BREAK:
      if (arg) {
        if (usart->info->xfer.send_active != 0U) { return ARM_DRIVER_ERROR_BUSY; }

        usart->reg->LCR |= USART_LCR_BC;

        // Set Send active flag
        usart->info->xfer.send_active = 1U;
      }
      else {
        usart->reg->LCR &= ~USART_LCR_BC;

        // Clear Send active flag
        usart->info->xfer.send_active = 0U;
      }
      return ARM_DRIVER_OK;

    // Abort Send
    case ARM_USART_ABORT_SEND:
      // Disable transmit holding register empty interrupt
      usart->reg->IER &= ~USART_IER_THREIE;

      // Set trigger level
      val  = (usart->trig_lvl & USART_FCR_RXTRIGLVL_MSK) | USART_FCR_FIFOEN;
      if (usart->dma_rx || usart->dma_tx) {
        val |= USART_FCR_DMAMODE;
      }

      // Transmit FIFO reset
      val |= USART_FCR_TXFIFORES;
      usart->reg->FCR = val;

      // If DMA mode - disable DMA channel
      if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
        GPDMA_ChannelDisable (usart->dma_tx->channel);
      }

      // Clear Send active flag
      usart->info->xfer.send_active = 0U;
      return ARM_DRIVER_OK;

    // Abort receive
    case ARM_USART_ABORT_RECEIVE:
      // Disable receive data available interrupt
      usart->reg->IER &= ~USART_IER_RBRIE;

      // Set trigger level
      val  = (usart->trig_lvl & USART_FCR_RXTRIGLVL_MSK) |
              USART_FCR_FIFOEN;
      if (usart->dma_rx || usart->dma_tx) {
        val |= USART_FCR_DMAMODE;
      }

      // Receive FIFO reset
      val |= USART_FCR_RXFIFORES;
      usart->reg->FCR = val;

      // If DMA mode - disable DMA channel
      if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
        GPDMA_ChannelDisable (usart->dma_rx->channel);
      }

      // Clear RX busy status
      usart->info->rx_status.rx_busy = 0U;
      return ARM_DRIVER_OK;

    // Abort transfer
    case ARM_USART_ABORT_TRANSFER:
      // Disable transmit holding register empty and 
      // receive data available interrupts
      usart->reg->IER &= ~(USART_IER_THREIE | USART_IER_RBRIE);

      // If DMA mode - disable DMA channel
      if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
        GPDMA_ChannelDisable (usart->dma_tx->channel);
      }
      if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
        GPDMA_ChannelDisable (usart->dma_rx->channel);
      }

      // Set trigger level
      val  = (usart->trig_lvl & USART_FCR_RXTRIGLVL_MSK) | USART_FCR_FIFOEN;
      if (usart->dma_rx || usart->dma_tx) {
        val |= USART_FCR_DMAMODE;
      }

      // Transmit and receive FIFO reset
      val |= USART_FCR_TXFIFORES | USART_FCR_RXFIFORES;
      usart->reg->FCR = val;

      // Clear busy statuses
      usart->info->rx_status.rx_busy = 0U;
      usart->info->xfer.send_active  = 0U;
      return ARM_DRIVER_OK;

    default: break;
  }

  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS:
      mode = ARM_USART_MODE_ASYNCHRONOUS;
      break;
    case ARM_USART_MODE_SYNCHRONOUS_MASTER:
      if (usart->capabilities.synchronous_master) {
        // Enable synchronous master (SCLK out) mode
        syncctrl = USART_SYNCCTRL_SYNC | USART_SYNCCTRL_CSRC;
      } else { return ARM_USART_ERROR_MODE; }
      mode = ARM_USART_MODE_SYNCHRONOUS_MASTER;
      break;
    case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
      if (usart->capabilities.synchronous_slave) {
        // Enable synchronous slave (SCLK in) mode
        syncctrl = USART_SYNCCTRL_SYNC;
      } else { return ARM_USART_ERROR_MODE; }
      mode = ARM_USART_MODE_SYNCHRONOUS_SLAVE;
      break;
    case ARM_USART_MODE_SINGLE_WIRE:
      // Enable Half duplex
      hden = USART_HDEN_HDEN;
      mode = ARM_USART_MODE_SINGLE_WIRE;
      break;
    case ARM_USART_MODE_IRDA:
      if (usart->capabilities.irda) {
        // Enable IrDA mode
        icr = USART_ICR_IRDAEN;
      } else { return ARM_USART_ERROR_MODE; }
      mode = ARM_USART_MODE_IRDA;
      break;
    case ARM_USART_MODE_SMART_CARD:
      if (usart->capabilities.smart_card) {
        // Enable Smart card mode
        scictrl = USART_SCICTRL_SCIEN;
      } else { return ARM_USART_ERROR_MODE; }
      mode = ARM_USART_MODE_SMART_CARD;
      break;

    // Default TX value
    case ARM_USART_SET_DEFAULT_TX_VALUE:
      usart->info->xfer.tx_def_val = arg;
      return ARM_DRIVER_OK;

    // IrDA pulse
    case ARM_USART_SET_IRDA_PULSE:
      if (usart->capabilities.irda) {
        if (arg == 0U) {
          usart->reg->ICR &= ~(USART_ICR_FIXPULSEEN);
        } else {
          val = 1000000000U / (GetClockFreq (((*usart->clk.base_clk >> 24) & 0x1FU)));
          icr = usart->reg->ICR & ~USART_ICR_PULSEDIV_MSK;
          if      (arg <= (2U   * val)) { icr |= (0U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (4U   * val)) { icr |= (1U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (8U   * val)) { icr |= (2U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (16U  * val)) { icr |= (3U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (32U  * val)) { icr |= (4U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (64U  * val)) { icr |= (5U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (128U * val)) { icr |= (6U << USART_ICR_PULSEDIV_POS); }
          else if (arg <= (256U * val)) { icr |= (7U << USART_ICR_PULSEDIV_POS); }
          else { return ARM_DRIVER_ERROR; }
          usart->reg->ICR = icr | USART_ICR_FIXPULSEEN;
        }
      } else { return ARM_DRIVER_ERROR; }
      return ARM_DRIVER_OK;

    // SmartCard guard time
    case ARM_USART_SET_SMART_CARD_GUARD_TIME:
      if (usart->capabilities.smart_card) {
        if (arg > 0xFF) { return ARM_DRIVER_ERROR; }
        usart->reg->SCICTRL &= ~USART_SCICTRL_GUARDTIME_MSK;
        usart->reg->SCICTRL |= (arg << USART_SCICTRL_GUARDTIME_POS);
      } else { return ARM_DRIVER_ERROR; }
      return ARM_DRIVER_OK;

    // SmartCard clock
    case ARM_USART_SET_SMART_CARD_CLOCK:
      if (usart->capabilities.smart_card == 0U) { return ARM_DRIVER_ERROR; }
      if (arg == 0U)                            { return ARM_DRIVER_OK;    }
      if (usart->capabilities.smart_card_clock) {
        if ((usart->info->baudrate * usart->sc_oversamp) != arg) {
          return ARM_DRIVER_ERROR;
        }
      } else { return ARM_DRIVER_ERROR; }
      return ARM_DRIVER_OK;

     // SmartCard NACK
    case ARM_USART_CONTROL_SMART_CARD_NACK:
      if (usart->capabilities.smart_card) {
        if (arg) { usart->reg->SCICTRL &= ~USART_SCICTRL_NACKDIS; }
        else     { usart->reg->SCICTRL |=  USART_SCICTRL_NACKDIS; }
      } else return ARM_DRIVER_ERROR;
      return ARM_DRIVER_OK;

    // Unsupported command
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // Check if Receiver/Transmitter is busy
  if ( usart->info->rx_status.rx_busy ||
      (usart->info->xfer.send_active != 0U)) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  // USART Data bits
  switch (control & ARM_USART_DATA_BITS_Msk) {
    case ARM_USART_DATA_BITS_5: lcr |= (0U << USART_LCR_WLS_POS); break;
    case ARM_USART_DATA_BITS_6: lcr |= (1U << USART_LCR_WLS_POS); break;
    case ARM_USART_DATA_BITS_7: lcr |= (2U << USART_LCR_WLS_POS); break;
    case ARM_USART_DATA_BITS_8: lcr |= (3U << USART_LCR_WLS_POS); break;
    default: return ARM_USART_ERROR_DATA_BITS;
  }

  // USART Parity
  switch (control & ARM_USART_PARITY_Msk) {
    case ARM_USART_PARITY_NONE:                                  break;
    case ARM_USART_PARITY_EVEN: lcr |= (1U << USART_LCR_PS_POS) |
                                              USART_LCR_PE;      break;
    case ARM_USART_PARITY_ODD:  lcr |= USART_LCR_PE;             break;
    default: return (ARM_USART_ERROR_PARITY);
  }

  // USART Stop bits
  switch (control & ARM_USART_STOP_BITS_Msk) {
    case ARM_USART_STOP_BITS_1:                       break;
    case ARM_USART_STOP_BITS_2: lcr |= USART_LCR_SBS; break;
    default: return ARM_USART_ERROR_STOP_BITS;
  }

  // USART Flow control (RTS and CTS lines are only available on USART1)
  if (usart->uart_reg != NULL) {
    mcr = usart->uart_reg->MCR & ~(UART_MCR_RTSEN | UART_MCR_CTSEN);
    switch (control & ARM_USART_FLOW_CONTROL_Msk) {
      case ARM_USART_FLOW_CONTROL_NONE:
        break;
      case ARM_USART_FLOW_CONTROL_RTS:
        if (usart->capabilities.flow_control_rts) {
          mcr |= UART_MCR_RTSEN;
        }
        else  { return ARM_USART_ERROR_FLOW_CONTROL; }
        break;
      case ARM_USART_FLOW_CONTROL_CTS:
        if (usart->capabilities.flow_control_cts) {
          mcr |= UART_MCR_CTSEN;
        } else { 
          return ARM_USART_ERROR_FLOW_CONTROL;
        }
        break;
      case ARM_USART_FLOW_CONTROL_RTS_CTS:
        if (usart->capabilities.flow_control_rts && 
            usart->capabilities.flow_control_cts) {
          mcr |= (UART_MCR_RTSEN | UART_MCR_CTSEN);
        } else {
          return ARM_USART_ERROR_FLOW_CONTROL;
        }
        break;
      default: { return ARM_USART_ERROR_FLOW_CONTROL; }
    }
  }

  // Clock setting for synchronous mode
  if ((mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
      (mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {

    // Only CPOL0 - CPHA1 combination available

    // USART clock polarity
    if ((control & ARM_USART_CPOL_Msk) != ARM_USART_CPOL0) {
      return ARM_USART_ERROR_CPOL;
    }

    // USART clock phase
    if ((control & ARM_USART_CPHA_Msk) != ARM_USART_CPHA1) {
      return ARM_USART_ERROR_CPHA;
    }
  }

  // USART Baudrate
  if (USART_SetBaudrate (arg, usart) == -1) {
    return ARM_USART_ERROR_BAUDRATE;
  }

  // Configuration is OK - Mode is valid
  usart->info->mode = mode;

  // Configure TX pin regarding mode and transmitter state
  val = SCU_PIN_CFG_INPUT_FILTER_DIS;
  switch (usart->info->mode) {
    case ARM_USART_MODE_SMART_CARD:
      // Pin function = USART TX
      val |= SCU_PIN_CFG_MODE(usart->pins.tx->config_val);
      break;
    default:
      // Synchronous master/slave, asynchronous, single-wire and IrDA mode
      if (usart->info->flags & USART_FLAG_TX_ENABLED) {
        // Pin function = USART TX
        val |= SCU_PIN_CFG_MODE(usart->pins.tx->config_val);
      } else {
        // Pin function = GPIO
        val |= SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0);
      }
  }
  SCU_PinConfigure(usart->pins.tx->port, usart->pins.tx->num, val);

  // Configure RX pin regarding mode and receiver state
  val = SCU_PIN_CFG_INPUT_FILTER_DIS;
  switch (usart->info->mode) {
    case ARM_USART_MODE_SINGLE_WIRE:
    case ARM_USART_MODE_SMART_CARD:
      // Pin function = GPIO
      val |= SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0);
      break;
    default:
      // Synchronous master/slave, asynchronous and  IrDA mode
      if (usart->info->flags & USART_FLAG_RX_ENABLED) {
        // Pin function = USART RX
        val |= SCU_PIN_CFG_INPUT_BUFFER_EN | 
               SCU_PIN_CFG_MODE(usart->pins.rx->config_val);
      } else {
        // Pin function = GPIO
        val |= SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0);
      }
      break;
  }
  SCU_PinConfigure(usart->pins.rx->port, usart->pins.rx->num, val);

  // Configure CLK pin regarding mode
  if (usart->pins.clk) {
    val = SCU_PIN_CFG_INPUT_FILTER_DIS;
    switch (usart->info->mode) {
      case ARM_USART_MODE_SMART_CARD:
      case ARM_USART_MODE_SYNCHRONOUS_MASTER:
        // Pin function = USART UCLK (output)
        val |= SCU_PIN_CFG_MODE(usart->pins.clk->config_val);
        break;
      case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
        // Pin function = USART UCLK (input)
        val |= SCU_PIN_CFG_INPUT_BUFFER_EN |
               SCU_PIN_CFG_MODE(usart->pins.clk->config_val);
        break;
      default:
        // Asynchronous, Single-wire and IrDA mode
        // Pin function = GPIO
        val |= SCU_PIN_CFG_INPUT_BUFFER_EN |
               SCU_PIN_CFG_MODE(SCU_CFG_MODE_FUNC0);
    }
    SCU_PinConfigure(usart->pins.clk->port, usart->pins.clk->num, val);
  }

  // Configure SYNCCRTL register (only in synchronous mode)
  if (usart->capabilities.synchronous_master ||
      usart->capabilities.synchronous_slave) {
    usart->reg->SYNCCTRL = USART_SYNCCTRL_FES    |
                           USART_SYNCCTRL_SSSDIS |
                           syncctrl;
  }

  // Configure HDEN register (only in single wire mode)
  if (usart->capabilities.single_wire) {
    usart->reg->HDEN = hden;
  }

  // Configure ICR register (only in IrDA mode)
  if (usart->capabilities.irda) {
    usart->reg->ICR = (usart->reg->ICR & ~USART_ICR_IRDAEN) | icr;
  }

  // Configure SCICTRL register (only in Smart Card mode)
  if (usart->capabilities.smart_card) {
    usart->reg->SCICTRL = (usart->reg->SCICTRL & ~USART_SCICTRL_SCIEN) |
                           scictrl;
  }

  // Configure MCR register (modem line for USART1)
  if (usart->uart_reg) {
    usart->uart_reg->MCR = ((usart->uart_reg->MCR & ~(UART_MCR_RTSEN |
                             UART_MCR_CTSEN))) | mcr;
  }

  // Configure Line control register
  usart->reg->LCR = ((usart->reg->LCR & (USART_LCR_BC | USART_LCR_DLAB)) | lcr);

  // Set configured flag
  usart->info->flags |= USART_FLAG_CONFIGURED;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USART_STATUS USART_GetStatus (USART_RESOURCES *usart)
  \brief       Get USART status.
  \param[in]   usart     Pointer to USART resources
  \return      USART status \ref ARM_USART_STATUS
*/
static ARM_USART_STATUS USART_GetStatus (USART_RESOURCES *usart) {
  ARM_USART_STATUS stat;

  stat.tx_busy          = (usart->reg->LSR & USART_LSR_TEMT ? (0U) : (1U));
  stat.rx_busy          = usart->info->rx_status.rx_busy;
  stat.tx_underflow     = 0U;
  stat.rx_overflow      = usart->info->rx_status.rx_overflow;
  stat.rx_break         = usart->info->rx_status.rx_break;
  stat.rx_framing_error = usart->info->rx_status.rx_framing_error;
  stat.rx_parity_error  = usart->info->rx_status.rx_parity_error;
  return stat;
}

/**
  \fn          int32_t USART_SetModemControl (ARM_USART_MODEM_CONTROL  control,
                                              USART_RESOURCES         *usart)
  \brief       Set USART Modem Control line state.
  \param[in]   control   \ref ARM_USART_MODEM_CONTROL
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_SetModemControl (ARM_USART_MODEM_CONTROL  control,
                                      USART_RESOURCES         *usart) {

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured
    return ARM_DRIVER_ERROR;
  }

  // Only UART1 supports modem lines
  if (usart->uart_reg == NULL) { return ARM_DRIVER_ERROR_UNSUPPORTED; }

  if (control == ARM_USART_RTS_CLEAR) {
    if (usart->capabilities.rts) { usart->uart_reg->MCR &= ~UART_MCR_RTSCTRL; }
    else                         { return ARM_DRIVER_ERROR_UNSUPPORTED;       }
  }
  if (control == ARM_USART_RTS_SET) {
    if (usart->capabilities.rts) { usart->uart_reg->MCR |=  UART_MCR_RTSCTRL; }
    else                         {return ARM_DRIVER_ERROR_UNSUPPORTED;        }
  }
  if (control == ARM_USART_DTR_CLEAR) {
    if (usart->capabilities.dtr) { usart->uart_reg->MCR &= ~UART_MCR_DTRCTRL; }
    else                         { return ARM_DRIVER_ERROR_UNSUPPORTED;       }
  }
  if (control == ARM_USART_DTR_SET) {
    if (usart->capabilities.dtr) { usart->uart_reg->MCR |=  UART_MCR_DTRCTRL; }
    else                         { return ARM_DRIVER_ERROR_UNSUPPORTED;       }
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USART_MODEM_STATUS USART_GetModemStatus (USART_RESOURCES *usart)
  \brief       Get USART Modem Status lines state.
  \param[in]   usart     Pointer to USART resources
  \return      modem status \ref ARM_USART_MODEM_STATUS
*/
static ARM_USART_MODEM_STATUS USART_GetModemStatus (USART_RESOURCES *usart) {
  ARM_USART_MODEM_STATUS modem_status;
  uint32_t msr;

  if (usart->uart_reg &&
     (usart->info->flags & USART_FLAG_CONFIGURED)) {

    msr = usart->uart_reg->MSR;

    if (usart->capabilities.cts) { modem_status.cts = (msr & UART_MSR_CTS ? (1U) : (0U)); }
    else                         { modem_status.cts = 0U; }
    if (usart->capabilities.dsr) { modem_status.dsr = (msr & UART_MSR_DSR ? (1U) : (0U)); }
    else                         { modem_status.dsr = 0U; }
    if (usart->capabilities.ri ) { modem_status.ri  = (msr & UART_MSR_RI  ? (1U) : (0U)); }
    else                         { modem_status.ri  = 0U; }
    if (usart->capabilities.dcd) { modem_status.dcd = (msr & UART_MSR_DCD ? (1U) : (0U)); }
    else                         { modem_status.dcd = 0U; }
  } else {
     modem_status.cts = 0U;
     modem_status.dsr = 0U;
     modem_status.ri  = 0U;
     modem_status.dcd = 0U;
  }

  return modem_status;
}

/**
  \fn          void USART_IRQHandler (UART_RESOURCES *usart)
  \brief       USART Interrupt handler.
  \param[in]   usart     Pointer to USART resources
*/
static void USART_IRQHandler (USART_RESOURCES *usart) {
  uint32_t iir, event, val, i;

  event = 0U;
  iir   = usart->reg->IIR;

  if ((iir & USART_IIR_INTSTATUS) == 0U) {

    // Transmit holding register empty
    if ((iir & USART_IIR_INTID_MSK) == USART_IIR_INTID_THRE) {
      val = 16U;
      while ((val --) && (usart->info->xfer.tx_num != usart->info->xfer.tx_cnt)) {
        if (((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER)  ||
             (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) &&
             (usart->info->xfer.sync_mode == USART_SYNC_MODE_RX)) {
          // Dummy write in synchronous receive only mode
          usart->reg->THR = usart->info->xfer.tx_def_val;
        } else {
          // Write data to Tx FIFO
          usart->reg->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt];
        }
        usart->info->xfer.tx_cnt++;
      }

      // Check if all data is transmitted
      if (usart->info->xfer.tx_num == usart->info->xfer.tx_cnt) {
        // Disable THRE interrupt
        usart->reg->IER &= ~USART_IER_THREIE;

        // Clear TX busy flag
        usart->info->xfer.send_active = 0U;

        // Set send complete event
        if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
            (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
          if ((usart->info->xfer.sync_mode == USART_SYNC_MODE_TX)    &&
              ((usart->info->flags & USART_FLAG_RX_ENABLED) == 0U)) {
            event |= ARM_USART_EVENT_SEND_COMPLETE;
          }
        } else {
          event |= ARM_USART_EVENT_SEND_COMPLETE;
        }
      }
    }

    // Receive line status
    if ((iir & USART_IIR_INTID_MSK) == USART_IIR_INTID_RLS) {
      event |= USART_RxLineIntHandler(usart);
    }

    // Receive data available and Character time-out indicator interrupt
    if (((iir & USART_IIR_INTID_MSK) == USART_IIR_INTID_RDA)  ||
        ((iir & USART_IIR_INTID_MSK) == USART_IIR_INTID_CTI)) {

      switch (usart->trig_lvl) {
        case USART_TRIG_LVL_1:  i = 1U;  break;
        case USART_TRIG_LVL_4:  i = 3U;  break;
        case USART_TRIG_LVL_8:  i = 7U;  break;
        case USART_TRIG_LVL_14: i = 13U; break;
      }

      // Get available data from RX FIFO
      while ((usart->reg->LSR & USART_LSR_RDR) && (i--)) {
        // Check RX line interrupt for errors
        event |= USART_RxLineIntHandler (usart);

        if (((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER)  ||
             (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) &&
             (usart->info->xfer.sync_mode == USART_SYNC_MODE_TX)) {
          // Dummy read in synchronous transmit only mode
          usart->reg->RBR;
        } else {
          // Read data from RX FIFO into receive buffer
          usart->info->xfer.rx_buf[usart->info->xfer.rx_cnt] = usart->reg->RBR;
        }

        usart->info->xfer.rx_cnt++;

        // Check if requested amount of data is received
        if (usart->info->xfer.rx_cnt == usart->info->xfer.rx_num) {
          // Disable RDA interrupt
          usart->reg->IER &= ~USART_IER_RBRIE;

          // Clear RX busy flag and set receive transfer complete event
          usart->info->rx_status.rx_busy = 0U;
          if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
              (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
            val = usart->info->xfer.sync_mode;
            usart->info->xfer.sync_mode = 0U;
            switch (val) {
              case USART_SYNC_MODE_TX:
                event |= ARM_USART_EVENT_SEND_COMPLETE;
                break;
              case USART_SYNC_MODE_RX:
                event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
                break;
              case USART_SYNC_MODE_TX_RX:
                event |= ARM_USART_EVENT_TRANSFER_COMPLETE;
                break;
              default: break;
            }
          } else {
            event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
          }
          break;
        }
      }
    }

    // Character time-out indicator
    if ((iir & USART_IIR_INTID_MSK) == USART_IIR_INTID_CTI) {
      if ((usart->info->mode != ARM_USART_MODE_SYNCHRONOUS_MASTER) &&
          (usart->info->mode != ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
        // Signal RX Time-out event, if not all requested data received
        if (usart->info->xfer.rx_cnt != usart->info->xfer.rx_num) {
          event |= ARM_USART_EVENT_RX_TIMEOUT;
        }
      }
    }

    // Modem interrupt (UART1 only)
#if (RTE_UART1)
    if (usart->uart_reg) {
      if ((iir & USART_IIR_INTID_MSK) == UART_IIR_INTID_MS) {
        // Save modem status register
        val = usart->uart_reg->MSR;
      
        // CTS state changed
        if ((usart->capabilities.cts) && (val & UART_MSR_DCTS)) {
          event |= ARM_USART_EVENT_CTS;
        }
        // DSR state changed
        if ((usart->capabilities.dsr) && (val & UART_MSR_DDSR)) {
          event |= ARM_USART_EVENT_DSR;
        }
        // Ring indicator
        if ((usart->capabilities.ri)  && (val & UART_MSR_TERI)) {
          event |= ARM_USART_EVENT_RI;
        }
        // DCD state changed
        if ((usart->capabilities.dcd) && (val & UART_MSR_DDCD)) {
          event |= ARM_USART_EVENT_DCD;
        }
      }
    }
#endif
  }
  if ((usart->info->cb_event != NULL) && (event != 0U)) {
    usart->info->cb_event (event);
  }
}

#if (((RTE_USART0) && (RTE_USART0_DMA_TX_EN == 1)) || \
     ((RTE_UART1)  && (RTE_UART1_DMA_TX_EN  == 1)) || \
     ((RTE_USART2) && (RTE_USART2_DMA_TX_EN == 1)) || \
     ((RTE_USART3) && (RTE_USART3_DMA_TX_EN == 1)))
/**
  \fn          void USART_GPDMA_Tx_Event (uint32_t event, USART_RESOURCES *usart)
  \brief       UART Interrupt handler.
  \param[in]   usart     Pointer to USART resources
  \param[in]   event     GPDMA_EVENT_TERMINAL_COUNT_REQUEST / GPDMA_EVENT_ERROR
*/
static void USART_GPDMA_Tx_Event (uint32_t event, USART_RESOURCES *usart) {
  switch (event) {
    case GPDMA_EVENT_TERMINAL_COUNT_REQUEST:
      usart->info->xfer.tx_cnt = usart->info->xfer.tx_num;
      // Clear TX busy flag
      usart->info->xfer.send_active = 0U;

      // Set Send Complete event for asynchronous transfers
      if ((usart->info->mode != ARM_USART_MODE_SYNCHRONOUS_MASTER) &&
          (usart->info->mode != ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
        if (usart->info->cb_event) {
          usart->info->cb_event (ARM_USART_EVENT_SEND_COMPLETE);
        }
      }
      break;
    case GPDMA_EVENT_ERROR:
    default:
      break;
  }
}
#endif

#if (((RTE_USART0) && (RTE_USART0_DMA_RX_EN == 1)) || \
     ((RTE_UART1)  && (RTE_UART1_DMA_RX_EN  == 1)) || \
     ((RTE_USART2) && (RTE_USART2_DMA_RX_EN == 1)) || \
     ((RTE_USART3) && (RTE_USART3_DMA_RX_EN == 1)))
/**
  \fn          void USART_GPDMA_Rx_Event (uint32_t event, USART_RESOURCES *usart)
  \brief       UART Interrupt handler.
  \param[in]   event     GPDMA_EVENT_TERMINAL_COUNT_REQUEST / GPDMA_EVENT_ERROR
  \param[in]   usart     Pointer to USART resources
*/
static void USART_GPDMA_Rx_Event (uint32_t event, USART_RESOURCES *usart) {
  uint32_t val, evt;

  evt = 0U;

  switch (event) {
    case GPDMA_EVENT_TERMINAL_COUNT_REQUEST:
      usart->info->xfer.rx_cnt    = usart->info->xfer.rx_num; 
      usart->info->rx_status.rx_busy = 0U;

      if ((usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_MASTER) ||
          (usart->info->mode == ARM_USART_MODE_SYNCHRONOUS_SLAVE )) {
        val = usart->info->xfer.sync_mode;
        usart->info->xfer.sync_mode = 0U;
        switch (val) {
          case USART_SYNC_MODE_TX:
            evt |= ARM_USART_EVENT_SEND_COMPLETE;
            break;
          case USART_SYNC_MODE_RX:
            evt |= ARM_USART_EVENT_RECEIVE_COMPLETE;
            break;
          case USART_SYNC_MODE_TX_RX:
            evt |= ARM_USART_EVENT_TRANSFER_COMPLETE;
             break;
          default: break;
        }
      } else {
        evt |= ARM_USART_EVENT_RECEIVE_COMPLETE;
      }
      if ((usart->info->cb_event != NULL) && (evt != 0U)) {
        usart->info->cb_event (evt);
      }
      break;
    case GPDMA_EVENT_ERROR:
    default:
      break;
  }
}
#endif


#if (RTE_USART0)
// USART0 Driver Wrapper functions
static ARM_USART_CAPABILITIES USART0_GetCapabilities (void) {
  return USART_GetCapabilities (&USART0_Resources);
}
static int32_t USART0_Initialize (ARM_USART_SignalEvent_t cb_event) {
  return USART_Initialize (cb_event, &USART0_Resources);
}
static int32_t USART0_Uninitialize (void) {
  return USART_Uninitialize(&USART0_Resources);
}
static int32_t USART0_PowerControl (ARM_POWER_STATE state) {
  return USART_PowerControl (state, &USART0_Resources);
}
static int32_t USART0_Send (const void *data, uint32_t num) {
  return USART_Send (data, num, &USART0_Resources);
}
static int32_t USART0_Receive (void *data, uint32_t num) {
  return USART_Receive (data, num, &USART0_Resources);
}
static int32_t USART0_Transfer (const void      *data_out,
                                      void      *data_in,
                                      uint32_t   num) {
  return USART_Transfer (data_out, data_in, num, &USART0_Resources);
}
static uint32_t USART0_GetTxCount (void) {
  return USART_GetTxCount (&USART0_Resources);
}
static uint32_t USART0_GetRxCount (void) {
  return USART_GetRxCount (&USART0_Resources); 
}
static int32_t USART0_Control (uint32_t control, uint32_t arg) {
  return USART_Control (control, arg, &USART0_Resources);
}
static ARM_USART_STATUS USART0_GetStatus (void) {
  return USART_GetStatus (&USART0_Resources);
}
static int32_t USART0_SetModemControl (ARM_USART_MODEM_CONTROL control) {
  return USART_SetModemControl (control, &USART0_Resources);
}
static ARM_USART_MODEM_STATUS USART0_GetModemStatus (void) {
  return USART_GetModemStatus (&USART0_Resources);
}
void UART0_IRQHandler (void) {
  USART_IRQHandler (&USART0_Resources);
}
#if (RTE_USART0_DMA_TX_EN == 1)
void USART0_GPDMA_Tx_Event (uint32_t event) {
  USART_GPDMA_Tx_Event(event, &USART0_Resources);
}
#endif
#if (RTE_USART0_DMA_RX_EN == 1)
void USART0_GPDMA_Rx_Event (uint32_t event) {
  USART_GPDMA_Rx_Event(event, &USART0_Resources);
}
#endif

// USART0 Driver Control Block
ARM_DRIVER_USART Driver_USART0 = {
    USARTx_GetVersion,
    USART0_GetCapabilities,
    USART0_Initialize,
    USART0_Uninitialize,
    USART0_PowerControl,
    USART0_Send, 
    USART0_Receive,
    USART0_Transfer,
    USART0_GetTxCount,
    USART0_GetRxCount,
    USART0_Control,
    USART0_GetStatus,
    USART0_SetModemControl,
    USART0_GetModemStatus
};
#endif

#if (RTE_UART1)
// USART1 Driver Wrapper functions
static ARM_USART_CAPABILITIES USART1_GetCapabilities (void) {
  return USART_GetCapabilities (&USART1_Resources);
}
static int32_t USART1_Initialize (ARM_USART_SignalEvent_t cb_event) {
  return USART_Initialize (cb_event, &USART1_Resources);
}
static int32_t USART1_Uninitialize (void) {
  return USART_Uninitialize(&USART1_Resources);
}
static int32_t USART1_PowerControl (ARM_POWER_STATE state) {
  return USART_PowerControl (state, &USART1_Resources);
}
static int32_t USART1_Send (const void *data, uint32_t num) {
  return USART_Send (data, num, &USART1_Resources);
}
static int32_t USART1_Receive (void *data, uint32_t num) {
  return USART_Receive (data, num, &USART1_Resources);
}
static int32_t USART1_Transfer (const void      *data_out,
                                      void      *data_in,
                                      uint32_t   num) {
  return USART_Transfer (data_out, data_in, num, &USART1_Resources);
}
static uint32_t USART1_GetTxCount (void) {
  return USART_GetTxCount (&USART1_Resources);
}
static uint32_t USART1_GetRxCount (void) {
  return USART_GetRxCount (&USART1_Resources); 
}
static int32_t USART1_Control (uint32_t control, uint32_t arg) {
  return USART_Control (control, arg, &USART1_Resources);
}
static ARM_USART_STATUS USART1_GetStatus (void) {
  return USART_GetStatus (&USART1_Resources);
}
static int32_t USART1_SetModemControl (ARM_USART_MODEM_CONTROL control) {
  return USART_SetModemControl (control, &USART1_Resources);
}
static ARM_USART_MODEM_STATUS USART1_GetModemStatus (void) {
  return USART_GetModemStatus (&USART1_Resources);
}
void UART1_IRQHandler (void) {
  USART_IRQHandler (&USART1_Resources);
}
#if (RTE_UART1_DMA_TX_EN == 1)
void USART1_GPDMA_Tx_Event (uint32_t event) {
  USART_GPDMA_Tx_Event(event, &USART1_Resources);
}
#endif
#if (RTE_UART1_DMA_RX_EN == 1)
void USART1_GPDMA_Rx_Event (uint32_t event) {
  USART_GPDMA_Rx_Event(event, &USART1_Resources);
}
#endif

// USART1 Driver Control Block
ARM_DRIVER_USART Driver_USART1 = {
    USARTx_GetVersion,
    USART1_GetCapabilities,
    USART1_Initialize,
    USART1_Uninitialize,
    USART1_PowerControl,
    USART1_Send, 
    USART1_Receive,
    USART1_Transfer,
    USART1_GetTxCount,
    USART1_GetRxCount,
    USART1_Control,
    USART1_GetStatus,
    USART1_SetModemControl,
    USART1_GetModemStatus
};
#endif

#if (RTE_USART2)
// USART2 Driver Wrapper functions
static ARM_USART_CAPABILITIES USART2_GetCapabilities (void) {
  return USART_GetCapabilities (&USART2_Resources);
}
static int32_t USART2_Initialize (ARM_USART_SignalEvent_t cb_event) {
  return USART_Initialize (cb_event, &USART2_Resources);
}
static int32_t USART2_Uninitialize (void) {
  return USART_Uninitialize(&USART2_Resources);
}
static int32_t USART2_PowerControl (ARM_POWER_STATE state) {
  return USART_PowerControl (state, &USART2_Resources);
}
static int32_t USART2_Send (const void *data, uint32_t num) {
  return USART_Send (data, num, &USART2_Resources);
}
static int32_t USART2_Receive (void *data, uint32_t num) {
  return USART_Receive (data, num, &USART2_Resources);
}
static int32_t USART2_Transfer (const void      *data_out,
                                      void      *data_in,
                                      uint32_t   num) {
  return USART_Transfer (data_out, data_in, num, &USART2_Resources);
}
static uint32_t USART2_GetTxCount (void) {
  return USART_GetTxCount (&USART2_Resources);
}
static uint32_t USART2_GetRxCount (void) {
  return USART_GetRxCount (&USART2_Resources); 
}
static int32_t USART2_Control (uint32_t control, uint32_t arg) {
  return USART_Control (control, arg, &USART2_Resources);
}
static ARM_USART_STATUS USART2_GetStatus (void) {
  return USART_GetStatus (&USART2_Resources);
}
static int32_t USART2_SetModemControl (ARM_USART_MODEM_CONTROL control) {
  return USART_SetModemControl (control, &USART2_Resources);
}
static ARM_USART_MODEM_STATUS USART2_GetModemStatus (void) {
  return USART_GetModemStatus (&USART2_Resources);
}
void UART2_IRQHandler (void) {
  USART_IRQHandler (&USART2_Resources);
}
#if (RTE_USART2_DMA_TX_EN == 1)
void USART2_GPDMA_Tx_Event (uint32_t event) {
  USART_GPDMA_Tx_Event(event, &USART2_Resources);
}
#endif
#if (RTE_USART2_DMA_RX_EN == 1)
void USART2_GPDMA_Rx_Event (uint32_t event) {
  USART_GPDMA_Rx_Event(event, &USART2_Resources);
}
#endif

// USART2 Driver Control Block
ARM_DRIVER_USART Driver_USART2 = {
    USARTx_GetVersion,
    USART2_GetCapabilities,
    USART2_Initialize,
    USART2_Uninitialize,
    USART2_PowerControl,
    USART2_Send, 
    USART2_Receive,
    USART2_Transfer,
    USART2_GetTxCount,
    USART2_GetRxCount,
    USART2_Control,
    USART2_GetStatus,
    USART2_SetModemControl,
    USART2_GetModemStatus
};
#endif

#if (RTE_USART3)
// USART3 Driver Wrapper functions
static ARM_USART_CAPABILITIES USART3_GetCapabilities (void) {
  return USART_GetCapabilities (&USART3_Resources);
}
static int32_t USART3_Initialize (ARM_USART_SignalEvent_t cb_event) {
  return USART_Initialize (cb_event, &USART3_Resources);
}
static int32_t USART3_Uninitialize (void) {
  return USART_Uninitialize(&USART3_Resources);
}
static int32_t USART3_PowerControl (ARM_POWER_STATE state) {
  return USART_PowerControl (state, &USART3_Resources);
}
static int32_t USART3_Send (const void *data, uint32_t num) {
  return USART_Send (data, num, &USART3_Resources);
}
static int32_t USART3_Receive (void *data, uint32_t num) {
  return USART_Receive (data, num, &USART3_Resources);
}
static int32_t USART3_Transfer (const void      *data_out,
                                      void      *data_in,
                                      uint32_t   num) {
  return USART_Transfer (data_out, data_in, num, &USART3_Resources);
}
static uint32_t USART3_GetTxCount (void) {
  return USART_GetTxCount (&USART3_Resources);
}
static uint32_t USART3_GetRxCount (void) {
  return USART_GetRxCount (&USART3_Resources); 
}
static int32_t USART3_Control (uint32_t control, uint32_t arg) {
  return USART_Control (control, arg, &USART3_Resources);
}
static ARM_USART_STATUS USART3_GetStatus (void) {
  return USART_GetStatus (&USART3_Resources);
}
static int32_t USART3_SetModemControl (ARM_USART_MODEM_CONTROL control) {
  return USART_SetModemControl (control, &USART3_Resources);
}
static ARM_USART_MODEM_STATUS USART3_GetModemStatus (void) {
  return USART_GetModemStatus (&USART3_Resources);
}
void UART3_IRQHandler (void) {
  USART_IRQHandler (&USART3_Resources);
}
#if (RTE_USART3_DMA_TX_EN == 1)
void USART3_GPDMA_Tx_Event (uint32_t event) {
  USART_GPDMA_Tx_Event(event, &USART3_Resources);
}
#endif
#if (RTE_USART3_DMA_RX_EN == 1)
void USART3_GPDMA_Rx_Event (uint32_t event) {
  USART_GPDMA_Rx_Event(event, &USART3_Resources);
}
#endif

// USART3 Driver Control Block
ARM_DRIVER_USART Driver_USART3 = {
    USARTx_GetVersion,
    USART3_GetCapabilities,
    USART3_Initialize,
    USART3_Uninitialize,
    USART3_PowerControl,
    USART3_Send, 
    USART3_Receive,
    USART3_Transfer,
    USART3_GetTxCount,
    USART3_GetRxCount,
    USART3_Control,
    USART3_GetStatus,
    USART3_SetModemControl,
    USART3_GetModemStatus
};
#endif
