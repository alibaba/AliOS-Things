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
 * $Revision:    V2.4
 *
 * Project:      MCI Driver Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __MCI_LPC18XX_H
#define __MCI_LPC18XX_H

#include "Driver_MCI.h"

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "MCI_LPC18xx.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#include <string.h>

#if (defined(RTE_Drivers_MCI0) && !RTE_SDMMC)
#error "SDMMC not configured in RTE_Device.h!"
#endif
/* Driver flag definitions */
#define MCI_INIT            (1 << 0)    /* MCI initialized         */
#define MCI_POWER           (1 << 1)    /* MCI powered on          */
#define MCI_SETUP           (1 << 2)    /* MCI configured          */
#define MCI_WRITE           (1 << 3)    /* Write transfer          */
#define MCI_STREAM          (1 << 4)    /* Stream stransfer        */
#define MCI_RESP_LONG       (1 << 5)    /* Long response expected  */


#define MCI_RESPONSE_EXPECTED_Msk (ARM_MCI_RESPONSE_SHORT      | \
                                   ARM_MCI_RESPONSE_SHORT_BUSY | \
                                   ARM_MCI_RESPONSE_LONG)

#define MCI_TRANSFER_EVENT_Msk   (ARM_MCI_EVENT_TRANSFER_ERROR   | \
                                  ARM_MCI_EVENT_TRANSFER_TIMEOUT | \
                                  ARM_MCI_EVENT_TRANSFER_COMPLETE)

#define MCI_COMMAND_EVENT_Msk    (ARM_MCI_EVENT_COMMAND_ERROR   | \
                                  ARM_MCI_EVENT_COMMAND_TIMEOUT | \
                                  ARM_MCI_EVENT_COMMAND_COMPLETE)

#define MCI_CONTROL_EVENT_Msk    (ARM_MCI_EVENT_CARD_INSERTED | \
                                  ARM_MCI_EVENT_CARD_REMOVED  | \
                                  ARM_MCI_EVENT_SDIO_INTERRUPT)

#define SDMMC_CTRL_RESET_BITMASK (SDMMC_CTRL_CONTROLLER_RESET | \
                                  SDMMC_CTRL_FIFO_RESET       | \
                                  SDMMC_CTRL_DMA_RESET)

#define SDMMC_RINT_ERR_SDIO_Msk  (SDMMC_RINTSTS_RE            | \
                                  SDMMC_RINTSTS_RCRC          | \
                                  SDMMC_RINTSTS_DCRC          | \
                                  SDMMC_RINTSTS_RTO_BAR       | \
                                  SDMMC_RINTSTS_DRTO_BDS      | \
                                  SDMMC_RINTSTS_HLE           | \
                                  SDMMC_RINTSTS_SBE           | \
                                  SDMMC_RINTSTS_EBE           | \
                                  SDMMC_RINTSTS_SDIO_INTERRUPT)

/* Clock Control Unit register bits */
#define CCU_CLK_CFG_RUN   (1 << 0)
#define CCU_CLK_CFG_AUTO  (1 << 1)
#define CCU_CLK_STAT_RUN  (1 << 0)

/* Reset Generation Unit register bits */
#define RGU_RESET_CTRL0_SDIO_RST (1 << 20)

/* CGU BASE_SDIO_CLK CLK_SEL definition */
#define SDIO_CLK_SEL_PLL1 0x09

/* Number of DMA descriptors */
#define SDMMC_DMA_DESC_CNT 4

/* DMA descriptor bit definitions */
#define SDMMC_DMA_DESC_DIC (1U <<  1)   /* Disable Interrupt on Completion    */
#define SDMMC_DMA_DESC_LD  (1U <<  2)   /* Last Descriptor                    */
#define SDMMC_DMA_DESC_FS  (1U <<  3)   /* First Descriptor                   */
#define SDMMC_DMA_DESC_CH  (1U <<  4)   /* Second Address Chained             */
#define SDMMC_DMA_DESC_ER  (1U <<  5)   /* End of Ring                        */
#define SDMMC_DMA_DESC_CES (1U << 30)   /* Card Error Summary                 */
#define SDMMC_DMA_DESC_OWN (1U << 31)   /* Descriptor Ownership               */

/* SDMMC Internal DMA Descriptor Definition */
typedef struct {
  uint32_t CtrlStat;                    /* Control and Status Information     */
  uint32_t BufSize;                     /* Buffer Size                        */
  uint32_t BufAddr1;                    /* Address pointer to data buffer 1   */
  uint32_t BufAddr2;                    /* Address pointer to data buffer 2   */
} SDMMC_DMA_DESC;

/* MCI Transfer Information Definition */
typedef struct _MCI_XFER {
  uint8_t *buf;                         /* Data buffer                        */
  uint32_t cnt;                         /* Data bytes to transfer             */
} MCI_XFER;

/* MCI Driver State Definition */
typedef struct _MCI_INFO {
  ARM_MCI_SignalEvent_t cb_event;       /* Driver event callback function     */
  ARM_MCI_STATUS        status;         /* Driver status                      */
  uint32_t             *response;       /* Pointer to response buffer         */
  MCI_XFER              xfer;           /* Data transfer description          */
  uint8_t               flags;          /* Driver state flags                 */
} MCI_INFO;

/* SDMMC CTRL Register Bitmask Definitions */
#define SDMMC_CTRL_CONTROLLER_RESET               (1U <<  0)
#define SDMMC_CTRL_FIFO_RESET                     (1U <<  1)
#define SDMMC_CTRL_DMA_RESET                      (1U <<  2)
#define SDMMC_CTRL_INT_ENABLE                     (1U <<  4)
#define SDMMC_CTRL_READ_WAIT                      (1U <<  6)
#define SDMMC_CTRL_SEND_IRQ_RESPONSE              (1U <<  7)
#define SDMMC_CTRL_ABORT_READ_DATA                (1U <<  8)
#define SDMMC_CTRL_SEND_CCSD                      (1U <<  9)
#define SDMMC_CTRL_SEND_AUTO_STOP_CCSD            (1U << 10)
#define SDMMC_CTRL_CEATA_DEVICE_INTERRUPT_STATUS  (1U << 11)
#define SDMMC_CTRL_USE_INTERNAL_DMAC              (1U << 25)

/* SDMMC PWREN Register Bitmask Definitions */
#define SDMMC_PWREN_POWER_ENABLE                  (1U <<  0)

/* SDMMC CLKDIV Register Bitmask Definitions */
#define SDMMC_CLKDIV_CLK_DIVIDER0(x)    (((x) & 0xFF) <<  0)
#define SDMMC_CLKDIV_CLK_DIVIDER1(x)    (((x) & 0xFF) <<  8)
#define SDMMC_CLKDIV_CLK_DIVIDER2(x)    (((x) & 0xFF) << 16)
#define SDMMC_CLKDIV_CLK_DIVIDER3(x)    (((x) & 0xFF) << 24)

/* SDMMC CLKSRC Register Bitmask Definitions */
#define SDMMC_CLKSRC_CLK_SOURCE(x)      (((x) & 0x03) <<  0)

/* SDMMC CLKENA Register Bitmask Definitions */
#define SDMMC_CLKENA_CCLK_ENABLE                  (1U <<  0)
#define SDMMC_CLKENA_CCLK_LOW_POWER               (1U << 16)

/* SDMMC TMOUT Register Bitmask Definitions */
#define SDMMC_TMOUT_RESPONSE_TIMEOUT(x) (((x) & 0xFF) <<  0)
#define SDMMC_TMOUT_DATA_TIMEOUT(x) (((x) & 0xFFFFFF) <<  8)

/* SDMMC CTYPE Register Bitmask Definitions */
#define SDMMC_CTYPE_CARD_WIDTH0                   (1U <<  0)
#define SDMMC_CTYPE_CARD_WIDTH1                   (1U << 16)

/* SDMMC BLKSIZ Register Bitmask Definitions */
#define SDMMC_BLKSIZ_BLOCK_SIZE(x)    (((x) & 0xFFFF) <<  0)

/* SDMMC BYTCNT Register Bitmask Definitions */
#define SDMMC_BYTCNT_BYTE_COUNT(x)               ((x) <<  0)

/* SDMMC INTMASK Register Bitmask Definitions */
#define SDMMC_INTMASK_CDET                        (1U <<  0)
#define SDMMC_INTMASK_RE                          (1U <<  1)
#define SDMMC_INTMASK_CDONE                       (1U <<  2)
#define SDMMC_INTMASK_DTO                         (1U <<  3)
#define SDMMC_INTMASK_TXDR                        (1U <<  4)
#define SDMMC_INTMASK_RXDR                        (1U <<  5)
#define SDMMC_INTMASK_RCRC                        (1U <<  6)
#define SDMMC_INTMASK_DCRC                        (1U <<  7)
#define SDMMC_INTMASK_RTO                         (1U <<  8)
#define SDMMC_INTMASK_DRTO                        (1U <<  9)
#define SDMMC_INTMASK_HTO                         (1U << 10)
#define SDMMC_INTMASK_FRUN                        (1U << 11)
#define SDMMC_INTMASK_HLE                         (1U << 12)
#define SDMMC_INTMASK_SBE                         (1U << 13)
#define SDMMC_INTMASK_ACD                         (1U << 14)
#define SDMMC_INTMASK_EBE                         (1U << 15)
#define SDMMC_INTMASK_SDIO_INT_MASK               (1U << 16)

/* SDMMC CMDARG Register Bitmask Definitions */
#define SDMMC_CMDARG_CMD_ARG(x)                  ((x) <<  0)

/* SDMMC CMD Register Bitmask Definitions */
#define SDMMC_CMD_CMD_INDEX(x)          (((x) & 0x3F) <<  0)
#define SDMMC_CMD_RESPONSE_EXPECT                 (1U <<  6)
#define SDMMC_CMD_RESPONSE_LENGTH                 (1U <<  7)
#define SDMMC_CMD_CHECK_RESPONSE_CRC              (1U <<  8)
#define SDMMC_CMD_DATA_EXPECTED                   (1U <<  9)
#define SDMMC_CMD_READ_WRITE                      (1U << 10)
#define SDMMC_CMD_TRANSFER_MODE                   (1U << 11)
#define SDMMC_CMD_SEND_AUTO_STOP                  (1U << 12)
#define SDMMC_CMD_WAIT_PRVDATA_COMPLETE           (1U << 13)
#define SDMMC_CMD_STOP_ABORT_CMD                  (1U << 14)
#define SDMMC_CMD_SEND_INITIALIZATION             (1U << 15)
#define SDMMC_CMD_UPDATE_CLOCK_REGISTERS_ONLY     (1U << 21)
#define SDMMC_CMD_READ_CEATA_DEVICE               (1U << 22)
#define SDMMC_CMD_CCS_EXPECTED                    (1U << 23)
#define SDMMC_CMD_ENABLE_BOOT                     (1U << 24)
#define SDMMC_CMD_EXPECT_BOOT_ACK                 (1U << 25)
#define SDMMC_CMD_DISABLE_BOOT                    (1U << 26)
#define SDMMC_CMD_BOOT_MODE                       (1U << 27)
#define SDMMC_CMD_VOLT_SWITCH                     (1U << 28)
#define SDMMC_CMD_START_CMD                       (1U << 31)

/* SDMMC MINTSTS Register Bitmask Definitions */
#define SDMMC_MINTSTS_CDET                        (1U <<  0)
#define SDMMC_MINTSTS_RE                          (1U <<  1)
#define SDMMC_MINTSTS_CDONE                       (1U <<  2)
#define SDMMC_MINTSTS_DTO                         (1U <<  3)
#define SDMMC_MINTSTS_TXDR                        (1U <<  4)
#define SDMMC_MINTSTS_RXDR                        (1U <<  5)
#define SDMMC_MINTSTS_RCRC                        (1U <<  6)
#define SDMMC_MINTSTS_DCRC                        (1U <<  7)
#define SDMMC_MINTSTS_RTO                         (1U <<  8)
#define SDMMC_MINTSTS_DRTO                        (1U <<  9)
#define SDMMC_MINTSTS_HTO                         (1U << 10)
#define SDMMC_MINTSTS_FRUN                        (1U << 11)
#define SDMMC_MINTSTS_HLE                         (1U << 12)
#define SDMMC_MINTSTS_SBE                         (1U << 13)
#define SDMMC_MINTSTS_ACD                         (1U << 14)
#define SDMMC_MINTSTS_EBE                         (1U << 15)
#define SDMMC_MINTSTS_SDIO_INTERRUPT              (1U << 16)

/* SDMMC RINTSTS Register Bitmask Definitions */
#define SDMMC_RINTSTS_CDET                        (1U <<  0)
#define SDMMC_RINTSTS_RE                          (1U <<  1)
#define SDMMC_RINTSTS_CDONE                       (1U <<  2)
#define SDMMC_RINTSTS_DTO                         (1U <<  3)
#define SDMMC_RINTSTS_TXDR                        (1U <<  4)
#define SDMMC_RINTSTS_RXDR                        (1U <<  5)
#define SDMMC_RINTSTS_RCRC                        (1U <<  6)
#define SDMMC_RINTSTS_DCRC                        (1U <<  7)
#define SDMMC_RINTSTS_RTO_BAR                     (1U <<  8)
#define SDMMC_RINTSTS_DRTO_BDS                    (1U <<  9)
#define SDMMC_RINTSTS_HTO                         (1U << 10)
#define SDMMC_RINTSTS_FRUN                        (1U << 11)
#define SDMMC_RINTSTS_HLE                         (1U << 12)
#define SDMMC_RINTSTS_SBE                         (1U << 13)
#define SDMMC_RINTSTS_ACD                         (1U << 14)
#define SDMMC_RINTSTS_EBE                         (1U << 15)
#define SDMMC_RINTSTS_SDIO_INTERRUPT              (1U << 16)

/* SDMMC STATUS Register Bitmask Definitions */
#define SDMMC_STATUS_FIFO_RX_WATERMARK            (1U <<  0)
#define SDMMC_STATUS_FIFO_TX_WATERMARK            (1U <<  1)
#define SDMMC_STATUS_FIFO_EMPTY                   (1U <<  2)
#define SDMMC_STATUS_FIFO_FULL                    (1U <<  3)
#define SDMMC_STATUS_DATA_3_STATUS                (1U <<  8)
#define SDMMC_STATUS_DATA_BUSY                    (1U <<  9)
#define SDMMC_STATUS_DATA_STATE_MC_BUSY           (1U << 10)
#define SDMMC_STATUS_DMA_ACK                      (1U << 30)
#define SDMMC_STATUS_DMA_REQ                      (1U << 31)

/* SDMMC FIFOTH Register Bitmask Definitions */
#define SDMMC_FIFOTH_TX_WMARK(x)       (((x) & 0x7FF) <<  0)
#define SDMMC_FIFOTH_RX_WMARK(x)       (((x) & 0x7FF) << 16)
#define SDMMC_FIFOTH_DMA_MTS(x)        (((x) & 0x007) << 28)

/* SDMMC CDETECT Register Bitmask Definitions */
#define SDMMC_CDETECT_CARD_DETECT                 (1U <<  0)

/* SDMMC WRTPRT Register Bitmask Definitions */
#define SDMMC_WRTPRT_WRITE_PROTECT                (1U <<  0)

/* SDMMC DEBNCE Register Bitmask Definitions */
#define SDMMC_DEBNCE_DEBOUNCE_COUNT(x) (((x)&0xFFFFFF)<<  0)

/* SDMMC RST_N Register Bitmask Definitions */
#define SDMMC_RST_N_CARD_RESET                    (1U <<  0)

/* SDMMC BMOD Register Bitmask Definitions */
#define SDMMC_BMOD_SWR                            (1U <<  0)
#define SDMMC_BMOD_FB                             (1U <<  1)
#define SDMMC_BMOD_DSL(x)               (((x) & 0x1F) <<  2)
#define SDMMC_BMOD_DE                             (1U <<  7)
#define SDMMC_BMOD_PBL(x)               (((x) & 0x07) <<  8)

/* SDMMC PLDMND Register Bitmask Definitions */
#define SDMMC_PLDMND_PD(x)                       ((x) <<  0)

/* SDMMC IDSTS Register Bitmask Definitions */
#define SDMMC_IDSTS_TI                            (1U <<  0)
#define SDMMC_IDSTS_RI                            (1U <<  1)
#define SDMMC_IDSTS_FBE                           (1U <<  2)
#define SDMMC_IDSTS_DU                            (1U <<  4)
#define SDMMC_IDSTS_CES                           (1U <<  5)
#define SDMMC_IDSTS_NIS                           (1U <<  8)
#define SDMMC_IDSTS_AIS                           (1U <<  9)

/* SDMMC IDSTS Register Bitmask Definitions */
#define SDMMC_IDINTEN_TI                          (1U <<  0)
#define SDMMC_IDINTEN_RI                          (1U <<  1)
#define SDMMC_IDINTEN_FBE                         (1U <<  2)
#define SDMMC_IDINTEN_DU                          (1U <<  4)
#define SDMMC_IDINTEN_CES                         (1U <<  5)
#define SDMMC_IDINTEN_NIS                         (1U <<  8)
#define SDMMC_IDINTEN_AIS                         (1U <<  9)

#endif /* __MCI_LPC18XX_H */
