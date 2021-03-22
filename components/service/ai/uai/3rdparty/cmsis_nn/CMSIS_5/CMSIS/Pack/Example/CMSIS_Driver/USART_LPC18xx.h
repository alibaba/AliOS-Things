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
 * $Revision:    V2.5
 *
 * Project:      USART Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __USART_LPC18XX_H
#define __USART_LPC18XX_H

#include "LPC18xx.h"
#include "Driver_USART.h"

#include "SCU_LPC18xx.h"
#include "GPDMA_LPC18xx.h"

// Clock Control Unit register
#define CCU_CLK_CFG_RUN     (1 << 0)
#define CCU_CLK_CFG_AUTO    (1 << 1)
#define CCU_CLK_STAT_RUN    (1 << 0)

// USART register interface definitions
// USART Divisor Latch register LSB
#define USART_DLL_DLLSB_POS          (     0U)
#define USART_DLL_DLLSB_MSK          (0xFFU << USART_DLL_DLLSB_POS)

// USART Divisor Latch register MSB
#define USART_DLM_DLMSB_POS          (     0U)
#define USART_DLM_DLMSB_MSK          (0xFFU << USART_DLM_DLMSB_POS)

// USART Interrupt enable register
#define USART_IER_RBRIE              (1U << 0)
#define USART_IER_THREIE             (1U << 1)
#define USART_IER_RXIE               (1U << 2)
#define UART_IER_MSIE                (1U << 3) // Only for UART1 - modem status interrupt enable
#define USART_IER_ABEOINTEN          (1U << 8)
#define USART_IER_ABTOINTEN          (1U << 9)

// USART Interrupt identification register
#define USART_IIR_INTSTATUS          (1U << 0)
#define USART_IIR_INTID_POS          (     1U)
#define USART_IIR_INTID_MSK          (7U << USART_IIR_INTID_POS)
#define USART_IIR_FIFOENABLE_POS     (     6U)
#define USART_IIR_FIFOENABLE_MSK     (3U << USART_IIR_FIFOENABLE_POS)
#define USART_IIR_ABEOINT            (1U << 8)
#define USART_IIR_ABTOINT            (1U << 9)

#define USART_IIR_INTID_RLS          (3U << USART_IIR_INTID_POS)
#define USART_IIR_INTID_RDA          (2U << USART_IIR_INTID_POS)
#define USART_IIR_INTID_CTI          (6U << USART_IIR_INTID_POS)
#define USART_IIR_INTID_THRE         (1U << USART_IIR_INTID_POS)
#define UART_IIR_INTID_MS            (0U << USART_IIR_INTID_POS) // UART1 only

// USART FIFO control register
#define USART_FCR_FIFOEN             (1U << 0)
#define USART_FCR_RXFIFORES          (1U << 1)
#define USART_FCR_TXFIFORES          (1U << 2)
#define USART_FCR_DMAMODE            (1U << 3)
#define USART_FCR_RXTRIGLVL_POS      (     6U)
#define USART_FCR_RXTRIGLVL_MSK      (3U << USART_FCR_RXTRIGLVL_POS)

// USART Line control register
#define USART_LCR_WLS_POS            (     0U)
#define USART_LCR_WLS_MSK            (3U << USART_LCR_WLS_POS)
#define USART_LCR_SBS                (1U << 2)
#define USART_LCR_PE                 (1U << 3)
#define USART_LCR_PS_POS             (     4U)
#define USART_LCR_PS_MSK             (3U << USART_LCR_PS_POS)
#define USART_LCR_BC                 (1U << 6)
#define USART_LCR_DLAB               (1U << 7)

// USART Line status register
#define USART_LSR_RDR                (1U << 0)
#define USART_LSR_OE                 (1U << 1)
#define USART_LSR_PE                 (1U << 2)
#define USART_LSR_FE                 (1U << 3)
#define USART_LSR_BI                 (1U << 4)
#define USART_LSR_THRE               (1U << 5)
#define USART_LSR_TEMT               (1U << 6)
#define USART_LSR_RXFE               (1U << 7)
#define USART_LSR_TXERR              (1U << 8)

#define USART_LSR_LINE_INT           (USART_LSR_OE | USART_LSR_PE | USART_LSR_FE | USART_LSR_BI)

// USART IrDA control register
#define USART_ICR_IRDAEN             (1U << 0)
#define USART_ICR_FIXPULSEEN         (1U << 1)
#define USART_ICR_IRDAINV            (1U << 2)
#define USART_ICR_PULSEDIV_POS       (     3U)
#define USART_ICR_PULSEDIV_MSK       (7U << USART_ICR_PULSEDIV_POS)


// USART Fractional divider register
#define USART_FDR_DIVADDVAL_POS      (      0U)
#define USART_FDR_DIVADDVAL_MSK      (0x0FU << USART_FDR_DIVADDVAL_POS)
#define USART_FDR_MULVAL_POS         (      4U)
#define USART_FDR_MULVAL_MSK         (0x0FU << USART_FDR_MULVAL_POS)

// USART oversampling register
#define USART_OSR_OSFRAC_POS         (      1U)
#define USART_OSR_OSFRAC_MSK         (7U    << USART_OSR_OSFRAC_POS)
#define USART_OSR_OSINT_POS          (      4U)
#define USART_OSR_OSINT_MSK          (0x0FU << USART_OSR_OSINT_POS)
#define USART_OSR_FDINT_POS          (      8U)
#define USART_OSR_FDINT_MSK          (0x7FU << USART_OSR_FDINT_MSK)

// USART Half duplex enable register
#define USART_HDEN_HDEN              (1U << 0U)

// USART SmartCard interface control register
#define USART_SCICTRL_SCIEN          (1U << 0)
#define USART_SCICTRL_NACKDIS        (1U << 1)
#define USART_SCICTRL_PROTSEL        (1U << 2)
#define USART_SCICTRL_TXRETRY_POS    (     5U)
#define USART_SCICTRL_TXRETRY_MSK    (7U << USART_SCICTRL_TXRETRY_POS)
#define USART_SCICTRL_GUARDTIME_POS  (     8U)
#define USART_SCICTRL_GUARDTIME_MSK  (0xFFU << USART_SCICTRL_GUARDTIME_POS)

// USART Synchronous mode control register
#define USART_SYNCCTRL_SYNC          (1U << 0)
#define USART_SYNCCTRL_CSRC          (1U << 1)
#define USART_SYNCCTRL_FES           (1U << 2)
#define USART_SYNCCTRL_TSBYPASS      (1U << 3)
#define USART_SYNCCTRL_CSCEN         (1U << 4)
#define USART_SYNCCTRL_SSSDIS        (1U << 5)
#define USART_SYNCCTRL_CCCLR         (1U << 6)

// UART Modem control register
#define UART_MCR_DTRCTRL             (1U << 0)
#define UART_MCR_RTSCTRL             (1U << 1)
#define UART_MCR_LMS                 (1U << 4)
#define UART_MCR_RTSEN               (1U << 6)
#define UART_MCR_CTSEN               (1U << 7)

// UART Modem status register
#define UART_MSR_DCTS                (1U << 0)
#define UART_MSR_DDSR                (1U << 1)
#define UART_MSR_TERI                (1U << 2)
#define UART_MSR_DDCD                (1U << 3)
#define UART_MSR_CTS                 (1U << 4)
#define UART_MSR_DSR                 (1U << 5)
#define UART_MSR_RI                  (1U << 6)
#define UART_MSR_DCD                 (1U << 7)

// USART RS485 control register
#define USART_RS485CTRL_NMMEN        (1U << 0)
#define USART_RS485CTRL_RXDIS        (1U << 1)
#define USART_RS485CTRL_AADEN        (1U << 2)
#define USART_RS485CTRL_DCTRL        (1U << 4)
#define USART_RS485CTRL_OINV         (1U << 5)

// USART Transmitter enable register
#define USART_TER_TXEN               (1U << 0)


// USART flags
#define USART_FLAG_INITIALIZED       (1U << 0)
#define USART_FLAG_POWERED           (1U << 1)
#define USART_FLAG_CONFIGURED        (1U << 2)
#define USART_FLAG_TX_ENABLED        (1U << 3)
#define USART_FLAG_RX_ENABLED        (1U << 4)
#define USART_FLAG_SEND_ACTIVE       (1U << 5)

// USART synchronous xfer modes
#define USART_SYNC_MODE_TX           ( 1U )
#define USART_SYNC_MODE_RX           ( 2U )
#define USART_SYNC_MODE_TX_RX        (USART_SYNC_MODE_TX | \
                                      USART_SYNC_MODE_RX)


#define FRACT_BITS                   ( 12U  )
#define FRACT_MASK                   (0XFFFU)

#define FIXED_OVERSAMPLING_DIVIDER_LIMIT   (51U  << FRACT_BITS)
#define INTEGER_OVERSAMPLING_DIVIDER_LIMIT ((12U << FRACT_BITS) + (8 << FRACT_BITS) / 10)

// Baudrate accepted error
#define USART_MAX_BAUDRATE_ERROR     ( 3U )
#define USART_MAX_DIVIDER_ERROR      ( 3U )

// USART TX FIFO trigger level
#define USART_TRIG_LVL_1             (0x00U)
#define USART_TRIG_LVL_4             (0x40U)
#define USART_TRIG_LVL_8             (0x80U)
#define USART_TRIG_LVL_14            (0xC0U)

#define FRACT_DIV(add, mul)      { ((uint16_t)((1U << 12) + (((uint32_t)(add << 24) / (mul)) >> 12))), ((uint8_t) (((mul) << 4) | add)), }

typedef struct _FRACT_DIV {
  uint16_t val;
  uint8_t  add_mul;
} FRACT_DIVIDER;

// USART Transfer Information (Run-Time)
typedef struct _USART_TRANSFER_INFO {
  uint32_t                rx_num;        // Total number of data to be received
  uint32_t                tx_num;        // Total number of data to be send
  uint8_t                *rx_buf;        // Pointer to in data buffer
  uint8_t                *tx_buf;        // Pointer to out data buffer
  uint32_t                rx_cnt;        // Number of data received
  uint32_t                tx_cnt;        // Number of data sent
  uint8_t                 tx_def_val;    // Transmit default value (used in USART_SYNC_MASTER_MODE_RX)
  uint8_t                 rx_dump_val;   // Receive dump value (used in USART_SYNC_MASTER_MODE_TX)
  uint8_t                 send_active;   // Send active flag
  uint8_t                 sync_mode;     // Synchronous mode
} USART_TRANSFER_INFO;

typedef struct _USART_RX_STATUS {
  uint8_t rx_busy;                       // Receiver busy flag
  uint8_t rx_overflow;                   // Receive data overflow detected (cleared on start of next receive operation)
  uint8_t rx_break;                      // Break detected on receive (cleared on start of next receive operation)
  uint8_t rx_framing_error;              // Framing error detected on receive (cleared on start of next receive operation)
  uint8_t rx_parity_error;               // Parity error detected on receive (cleared on start of next receive operation)
} USART_RX_STATUS;

// USART Information (Run-Time)
typedef struct _USART_INFO {
  ARM_USART_SignalEvent_t cb_event;      // Event callback
  USART_RX_STATUS         rx_status;     // Receive status flags
  USART_TRANSFER_INFO     xfer;          // Transfer information
  uint8_t                 mode;          // USART mode
  uint8_t                 flags;         // USART driver flags
  uint32_t                baudrate;      // Baudrate
} USART_INFO;

// USART DMA
typedef const struct _USART_DMA {
  uint8_t                 channel;       // DMA Channel
  uint8_t                 peripheral;    // DMA mux
  uint8_t                 peripheral_sel;// DMA mux selection
  GPDMA_SignalEvent_t     cb_event;      // DMA Event callback
} USART_DMA;

// USART Pin Configuration
typedef const struct _USART_PINS {
  PIN_ID                 *tx;            // TX  Pin identifier
  PIN_ID                 *rx;            // RX  Pin identifier
  PIN_ID                 *clk;           // CLK  Pin identifier
  PIN_ID                 *cts;           // CTS Pin identifier
  PIN_ID                 *rts;           // RTS Pin identifier
  PIN_ID                 *dcd;           // DCD Pin identifier
  PIN_ID                 *dsr;           // DSR Pin identifier
  PIN_ID                 *dtr;           // DTR Pin identifier
  PIN_ID                 *ri;            // RI  Pin identifier
} USART_PINS;

// USART Clocks Configuration
typedef const struct _USART_CLOCK {
  __IO uint32_t          *reg_cfg;       // USART register interface clock configuration register
  __I  uint32_t          *reg_stat;      // USART register interface clock status register
  __IO uint32_t          *peri_cfg;      // USART peripheral clock configuration register
  __I  uint32_t          *peri_stat;     // USART peripheral clock status register
  __IO uint32_t          *base_clk;      // USART base clock
} USART_CLOCKS;

// USART Reset Configuration
typedef const struct _USART_RESET {
       uint32_t           reg_cfg_val;   // USART reset bit 
  __IO uint32_t          *reg_cfg;       // USART reset control register
  __I  uint32_t          *reg_stat;      // USART reset active status register
} USART_RESET;

// USART Resources definitions
typedef struct {
  ARM_USART_CAPABILITIES  capabilities;  // Capabilities
  LPC_USARTn_Type        *reg;           // Pointer to USART peripheral
  LPC_UART1_Type         *uart_reg;      // Pointer to UART peripheral
  USART_PINS              pins;          // USART pins configuration
  USART_CLOCKS            clk;           // USART clocks configuration
  USART_RESET             rst;           // USART reset configuration
  IRQn_Type               irq_num;       // USART IRQ Number
  uint32_t                trig_lvl;      // FIFO Trigger level
  USART_DMA              *dma_tx;
  USART_DMA              *dma_rx;
  USART_INFO             *info;          // Run-Time Information
  float                   sc_oversamp;   // SmartCard oversampling ratio
} const USART_RESOURCES;

#endif /* __USART_LPC18XX_H */
