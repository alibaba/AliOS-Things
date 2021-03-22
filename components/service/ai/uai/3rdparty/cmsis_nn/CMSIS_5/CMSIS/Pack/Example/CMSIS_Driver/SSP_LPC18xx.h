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
 * $Revision:    V2.2
 *
 * Project:      SSP Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __SSP_LPC18XX_H
#define __SSP_LPC18XX_H

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "GPIO_LPC18xx.h"
#include "GPDMA_LPC18xx.h"

#include "Driver_SPI.h"

/* SSP Register Interface Definitions */
#define CGU_BASE_SSPx_CLK_PD              (0x01 << 0)       /*!< CGU BASE_SSPx_CLK: PD Mask              */
#define CGU_BASE_SSPx_CLK_AUTOBLOCK       (0x01 << 11)      /*!< CGU BASE_SSPx_CLK: AUTOBLOCK Mask       */
#define CGU_BASE_SSPx_CLK_CLK_SEL         (0x1F << 24)      /*!< CGU BASE_SSPx_CLK: CLK_SEL Mask         */
#define CCU1_CLK_M3_SSPx_CFG_RUN          (0x01 << 0)       /*!< CCU1 CLK_M3_SSPx_CFG: RUN Mask          */
#define CCU1_CLK_M3_SSPx_CFG_AUTO         (0x01 << 1)       /*!< CCU1 CLK_M3_SSPx_CFG: AUTO Mask         */
#define CCU1_CLK_M3_SSPx_CFG_WAKEUP       (0x01 << 2)       /*!< CCU1 CLK_M3_SSPx_CFG: WAKEUP Mask       */
#define CCU1_CLK_M3_SSPx_STAT_RUN         (0x01 << 0)       /*!< CCU1 CLK_M3_SSPx_STAT: RUN Mask         */
#define CCU1_CLK_M3_SSPx_STAT_AUTO        (0x01 << 1)       /*!< CCU1 CLK_M3_SSPx_STAT: AUTO Mask        */
#define CCU1_CLK_M3_SSPx_STAT_WAKEUP      (0x01 << 2)       /*!< CCU1 CLK_M3_SSPx_STAT: WAKEUP Mask      */
#define CCU2_BASE_STAT_BASE_SSP1_CLK      (0x01 << 5)       /*!< CCU2 BASE_STAT: BASE_SSP1_CLK Mask      */
#define CCU2_BASE_STAT_BASE_SSP0_CLK      (0x01 << 6)       /*!< CCU2 BASE_STAT: BASE_SSP0_CLK Mask      */
#define CCU2_CLK_APBn_SSPx_CFG_RUN        (0x01 << 0)       /*!< CCU2 CLK_APBn_SSPx_CFG: RUN Mask        */
#define CCU2_CLK_APBn_SSPx_CFG_AUTO       (0x01 << 1)       /*!< CCU2 CLK_APBn_SSPx_CFG: AUTO Mask       */
#define CCU2_CLK_APBn_SSPx_CFG_WAKEUP     (0x01 << 2)       /*!< CCU2 CLK_APBn_SSPx_CFG: WAKEUP Mask     */
#define CCU2_CLK_APBn_SSPx_STAT_RUN       (0x01 << 0)       /*!< CCU2 CLK_APBn_SSPx_STAT: RUN Mask       */
#define CCU2_CLK_APBn_SSPx_STAT_AUTO      (0x01 << 1)       /*!< CCU2 CLK_APBn_SSPx_STAT: AUTO Mask      */
#define CCU2_CLK_APBn_SSPx_STAT_WAKEUP    (0x01 << 2)       /*!< CCU2 CLK_APBn_SSPx_STAT: WAKEUP Mask    */
#define RGU_RESET_CTRL1_SSP0_RST          (0x01 << 18)      /*!< RGU RESET_CTRL1: SSP0_RST Mask          */
#define RGU_RESET_CTRL1_SSP1_RST          (0x01 << 19)      /*!< RGU RESET_CTRL1: SSP1_RST Mask          */
#define RGU_RESET_STATUS3_SSP0_RST        (0x03 << 4)       /*!< RGU RESET_STATUS3: SSP0_RST Mask        */
#define RGU_RESET_STATUS3_SSP1_RST        (0x03 << 6)       /*!< RGU RESET_STATUS3: SSP1_RST Mask        */
#define RGU_RESET_ACTIVE_STATUS1_SSP0_RST (0x01 << 18)      /*!< RGU RESET_ACTIVE_STATUS1: SSP0_RST Mask */
#define RGU_RESET_ACTIVE_STATUS1_SSP1_RST (0x01 << 19)      /*!< RGU RESET_ACTIVE_STATUS1: SSP1_RST Mask */

#define SSPx_CR0_DSS                      (0x0F << 0)       /*!< SSPx CR0: DSS Mask                      */
#define SSPx_CR0_FRF                      (0x03 << 4)       /*!< SSPx CR0: FRF Mask                      */
#define SSPx_CR0_CPOL                     (0x01 << 6)       /*!< SSPx CR0: CPOL Mask                     */
#define SSPx_CR0_CPHA                     (0x01 << 7)       /*!< SSPx CR0: CPHA Mask                     */
#define SSPx_CR0_SCR                      (0xFF << 8)       /*!< SSPx CR0: SCR Mask                      */
#define SSPx_CR1_LBM                      (0x01 << 0)       /*!< SSPx CR1: LBM Mask                      */
#define SSPx_CR1_SSE                      (0x01 << 1)       /*!< SSPx CR1: SSE Mask                      */
#define SSPx_CR1_MS                       (0x01 << 2)       /*!< SSPx CR1: MS Mask                       */
#define SSPx_CR1_SOD                      (0x01 << 3)       /*!< SSPx CR1: SOD Mask                      */
#define SSPx_DR_DATA                      (0xFFFF<<0)       /*!< SSPx DR: DATA Mask                      */
#define SSPx_SR_TFE                       (0x01 << 0)       /*!< SSPx SR: TFE Mask                       */
#define SSPx_SR_TNF                       (0x01 << 1)       /*!< SSPx SR: TNF Mask                       */
#define SSPx_SR_RNE                       (0x01 << 2)       /*!< SSPx SR: RNE Mask                       */
#define SSPx_SR_RFF                       (0x01 << 3)       /*!< SSPx SR: RFF Mask                       */
#define SSPx_SR_BSY                       (0x01 << 4)       /*!< SSPx SR: BSY Mask                       */
#define SSPx_CPSR_CPSDVSR                 (0xFF << 0)       /*!< SSPx CPSR: CPSDVSR Mask                 */
#define SSPx_IMSC_RORIM                   (0x01 << 0)       /*!< SSPx IMSC: RORIM Mask                   */
#define SSPx_IMSC_RTIM                    (0x01 << 1)       /*!< SSPx IMSC: RTIM Mask                    */
#define SSPx_IMSC_RXIM                    (0x01 << 2)       /*!< SSPx IMSC: RXIM Mask                    */
#define SSPx_IMSC_TXIM                    (0x01 << 3)       /*!< SSPx IMSC: TXIM Mask                    */
#define SSPx_RIS_RORRIS                   (0x01 << 0)       /*!< SSPx RIS: RORRIS Mask                   */
#define SSPx_RIS_RTRIS                    (0x01 << 1)       /*!< SSPx RIS: RTRIS Mask                    */
#define SSPx_RIS_RXRIS                    (0x01 << 2)       /*!< SSPx RIS: RXRIS Mask                    */
#define SSPx_RIS_TXRIS                    (0x01 << 3)       /*!< SSPx RIS: TXRIS Mask                    */
#define SSPx_MIS_RORMIS                   (0x01 << 0)       /*!< SSPx MIS: RORMIS Mask                   */
#define SSPx_MIS_RTMIS                    (0x01 << 1)       /*!< SSPx MIS: RTMIS Mask                    */
#define SSPx_MIS_RXMIS                    (0x01 << 2)       /*!< SSPx MIS: RXMIS Mask                    */
#define SSPx_MIS_TXMIS                    (0x01 << 3)       /*!< SSPx MIS: TXMIS Mask                    */
#define SSPx_ICR_RORIC                    (0x01 << 0)       /*!< SSPx ICR: RORIC Mask                    */
#define SSPx_ICR_RTIC                     (0x01 << 1)       /*!< SSPx ICR: RTIC Mask                     */
#define SSPx_DMACR_RXDMAE                 (0x01 << 0)       /*!< SSPx DMACR: RXDMAE Mask                 */
#define SSPx_DMACR_TXDMAE                 (0x01 << 1)       /*!< SSPx DMACR: TXDMAE Mask                 */

#define CLK_SRC_PLL1                       0x09             // SSP clock source

/* Current driver status flag definition */
#define SSP_INITIALIZED                   (1    << 0)       // SSP initialized
#define SSP_POWERED                       (1    << 1)       // SSP powered on
#define SSP_CONFIGURED                    (1    << 2)       // SSP configured
#define SSP_DATA_LOST                     (1    << 3)       // SSP data lost occurred
#define SSP_MODE_FAULT                    (1    << 4)       // SSP mode fault occurred

/* SSP Pins Configuration */
typedef const struct _SSP_PINS {
  PIN_ID              *sck;              // SCK pin
  PIN_ID              *miso;             // MISO pin
  PIN_ID              *mosi;             // MOSI pin
  PIN_ID              *ssel;             // SSEL pin
  GPIO_ID             *gpio_ssel;        // SSEL gpio
  uint8_t              gpio_ssel_af;     // SSEL gpio alternate function
} SSP_PINS;

/* Clocks Configuration */
typedef const struct _SSP_CLOCK {
  uint32_t              reg_cfg_val;    // SSP register interface clock configuration register value
  volatile uint32_t    *reg_cfg;        // SSP register interface clock configuration register
  uint32_t              peri_cfg_val;   // SSP peripheral clock configuration register value
  volatile uint32_t    *peri_cfg;       // SSP peripheral clock configuration register
  uint32_t              peri_stat_val;  // SSP peripheral clock status register value
  const volatile uint32_t *peri_stat;   // SSP peripheral clock status register
} SSP_CLOCKS;

/* Reset Configuration */
typedef const struct _SSP_RESET {
  uint32_t              reg_cfg_val;    // SSP register interface clock configuration register value
  volatile uint32_t    *reg_cfg;        // SSP register interface clock configuration register
  uint32_t              reg_stat_val;   // SSP peripheral clock status register value
  const volatile uint32_t *reg_stat;    // SSP peripheral clock status register
} SSP_RESET;

/* DMA Configuration */
typedef const struct _SSP_DMA {
  uint8_t               tx_en;          // Transmit channel enabled
  uint8_t               tx_ch;          // Transmit channel number
  uint8_t               tx_peri;        // Transmit peripheral
  uint8_t               tx_peri_sel;    // Transmit peripheral mux selection
  void                (*tx_callback)(uint32_t event); // Transmit callback
  uint8_t               rx_en;          // Receive channel enabled
  uint8_t               rx_ch;          // Receive channel number
  uint8_t               rx_peri;        // Receive peripheral
  uint8_t               rx_peri_sel;    // Receive peripheral mux selection
  void                (*rx_callback)(uint32_t event); // Receive callback
} SSP_DMA;

/* SSP status */
typedef struct _SSP_STATUS {
  uint8_t busy;                         // Transmitter/Receiver busy flag
  uint8_t data_lost;                    // Data lost: Receive overflow / Transmit underflow (cleared on start of transfer operation)
  uint8_t mode_fault;                   // Mode fault detected; optional (cleared on start of transfer operation)
} SSP_STATUS;

/* SSP Information (Run-time) */
typedef struct _SSP_INFO {
  ARM_SPI_SignalEvent_t cb_event;       // Event Callback
  SSP_STATUS            status;         // Status flags
  uint8_t               state;          // Current SSP state
  uint32_t              mode;           // Current SSP mode
} SSP_INFO;

/* SSP Transfer Information (Run-Time) */
typedef struct _SSP_TRANSFER_INFO {
  uint32_t              num;            // Total number of transfers
  uint8_t              *rx_buf;         // Pointer to in data buffer
  uint8_t              *tx_buf;         // Pointer to out data buffer
  uint32_t              rx_cnt;         // Number of data received
  uint32_t              tx_cnt;         // Number of data sent
  uint32_t              dump_val;       // Variable for dumping DMA data
  uint16_t              def_val;        // Default transfer value
} SSP_TRANSFER_INFO;

/* SSP Resources */
typedef struct {
  LPC_SSPn_Type        *reg;            // SSP peripheral register interface
  SSP_PINS              pin;            // SSP pins configuration
  SSP_CLOCKS            clk;            // SSP clocks configuration
  SSP_RESET             rst;            // SSP reset configuration
  SSP_DMA               dma;            // SSP DMA configuration
  IRQn_Type             irq_num;        // SSP IRQ number
  SSP_INFO             *info;           // SSP Run-time information
  SSP_TRANSFER_INFO    *xfer;           // SSP transfer information
} const SSP_RESOURCES;

#endif /* __SSP_LPC18XX_H */
