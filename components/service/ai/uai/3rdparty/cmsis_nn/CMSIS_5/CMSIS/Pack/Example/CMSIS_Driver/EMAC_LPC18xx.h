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
 * $Revision:    V2.6
 *
 * Project:      Ethernet Media Access (MAC) Definitions for NXP LPC18xx
 * -------------------------------------------------------------------------- */

#ifndef __EMAC_LPC18XX_H
#define __EMAC_LPC18XX_H

#include <string.h>

#include "Driver_ETH_MAC.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#include "cmsis_os.h"
#include "LPC18xx.h"

#include "SCU_LPC18xx.h"

#if (defined(RTE_Drivers_ETH_MAC0) && !RTE_ENET)
#error "Ethernet not configured in RTE_Device.h!"
#endif

#if (RTE_ENET_MII && RTE_ENET_RMII)
#error "Ethernet interface configuration in RTE_Device.h is invalid!"
#endif

#define ENET                LPC_ETHERNET

/* Clock Control Unit register */
#define CCU_CLK_CFG_RUN     (1 << 0)
#define CCU_CLK_CFG_AUTO    (1 << 1)
#define CCU_CLK_STAT_RUN    (1 << 0)

#define CLK_SRC_PLL1        0x09            // EMAC clock source

/* CREG6 Control Register */
#define EMAC_CREG6_ETH_MASK 0x00000007      // EMAC mode mask
#define EMAC_CREG6_ETH_MII  (0x0 << 0)      // EMAC mode MII
#define EMAC_CREG6_ETH_RMII (0x4 << 0)      // EMAC mode RMII

/* EMAC reset value for RGU */
#define RGU_RESET_EMAC      (1 << 22)       // EMAC reset

/* EMAC Driver state flags */
#define EMAC_FLAG_INIT      (1 << 0)        // Driver initialized
#define EMAC_FLAG_POWER     (1 << 1)        // Driver power on

/* TDES0 Descriptor TX Packet Control/Status */
#define EMAC_TDES0_OWN      0x80000000      // Own bit 1=DMA, 0=CPU
#define EMAC_TDES0_IC       0x40000000      // Interrupt on completition
#define EMAC_TDES0_LS       0x20000000      // Last segment
#define EMAC_TDES0_FS       0x10000000      // First segment
#define EMAC_TDES0_DC       0x08000000      // Disable CRC
#define EMAC_TDES0_DP       0x04000000      // Disable pad
#define EMAC_TDES0_TTSE     0x02000000      // Transmit time stamp enable
#define EMAC_TDES0_TER      0x00200000      // Transmit end of ring
#define EMAC_TDES0_TCH      0x00100000      // Second address chained
#define EMAC_TDES0_TTSS     0x00020000      // Transmit time stamp status
#define EMAC_TDES0_IHE      0x00010000      // IP header error
#define EMAC_TDES0_ES       0x00008000      // Error summary
#define EMAC_TDES0_JT       0x00004000      // Jabber timeout
#define EMAC_TDES0_FF       0x00002000      // Frame flushed
#define EMAC_TDES0_IPE      0x00001000      // IP payload error
#define EMAC_TDES0_LOC      0x00000800      // Loss of carrier
#define EMAC_TDES0_NC       0x00000400      // No carrier
#define EMAC_TDES0_LC       0x00000200      // Late collision
#define EMAC_TDES0_EC       0x00000100      // Excessive collision
#define EMAC_TDES0_VF       0x00000080      // VLAN frame
#define EMAC_TDES0_CC       0x00000078      // Collision count
#define EMAC_TDES0_ED       0x00000004      // Excessive deferral
#define EMAC_TDES0_UF       0x00000002      // Underflow error
#define EMAC_TDES0_DB       0x00000001      // Deferred bit

/* TDES1 Descriptor TX Packet Control */
#define EMAC_TDES1_TBS2     0x1FFF0000      // Transmit buffer 2 size
#define EMAC_TDES1_TBS1     0x00001FFF      // Transmit buffer 1 size

/* RDES0 Descriptor RX Packet Status */
#define EMAC_RDES0_OWN      0x80000000      // Own bit 1=DMA, 0=CPU
#define EMAC_RDES0_AFM      0x40000000      // Destination address filter fail
#define EMAC_RDES0_FL       0x3FFF0000      // Frame length mask
#define EMAC_RDES0_ES       0x00008000      // Error summary
#define EMAC_RDES0_DE       0x00004000      // Descriptor error
#define EMAC_RDES0_SAF      0x00002000      // Source address filter fail
#define EMAC_RDES0_LE       0x00001000      // Length error
#define EMAC_RDES0_OE       0x00000800      // Overflow error
#define EMAC_RDES0_VLAN     0x00000400      // VLAN tag
#define EMAC_RDES0_FS       0x00000200      // First descriptor
#define EMAC_RDES0_LS       0x00000100      // Last descriptor
#define EMAC_RDES0_TSA      0x00000080      // Timestamp available
#define EMAC_RDES0_LC       0x00000040      // Late collision
#define EMAC_RDES0_FT       0x00000020      // Frame type
#define EMAC_RDES0_RWT      0x00000010      // Receive watchdog timeout
#define EMAC_RDES0_RE       0x00000008      // Receive error
#define EMAC_RDES0_DBE      0x00000004      // Dribble bit error
#define EMAC_RDES0_CE       0x00000002      // CRC error
#define EMAC_RDES0_ESA      0x00000001      // Extended Status/Rx MAC address

/* RDES1 Descriptor RX Packet Control */
#define EMAC_RDES1_RBS2     0x1FFF0000      // Receive buffer 2 size
#define EMAC_RDES1_RER      0x00008000      // Receive end of ring
#define EMAC_RDES1_RCH      0x00004000      // Second address chained
#define EMAC_RDES1_RBS1     0x00001FFF      // Receive buffer 1 size

/* MAC Configuration Register */
#define EMAC_MCR_WD         0x00800000      // Watchdog disable
#define EMAC_MCR_JD         0x00400000      // Jabber disable
#define EMAC_MCR_JE         0x00100000      // Jumbo frame enable
#define EMAC_MCR_IFG        0x000E0000      // Inter-frame gap mask
#define EMAC_MCR_DCRS       0x00010000      // Disable carrier sense during Tx
#define EMAC_MCR_PS         0x00008000      // Port select
#define EMAC_MCR_FES        0x00004000      // Speed
#define EMAC_MCR_DO         0x00002000      // Disable receive own
#define EMAC_MCR_LM         0x00001000      // Loopback mode
#define EMAC_MCR_DM         0x00000800      // Duplex mode
#define EMAC_MCR_DR         0x00000200      // Disable Retry
#define EMAC_MCR_ACS        0x00000080      // Automatic pad/CRC stripping
#define EMAC_MCR_BL         0x00000060      // Back-off limit mask
#define EMAC_MCR_DF         0x00000010      // Deferral check
#define EMAC_MCR_TE         0x00000008      // Transmitter enable
#define EMAC_MCR_RE         0x00000004      // Receiver enable

/* MAC Frame Filter Register */
#define EMAC_MFFR_RA        0x80000000      // Receive all
#define EMAC_MFFR_HPF       0x00000400      // Hash or perfect filter
#define EMAC_MFFR_PCF       0x000000C0      // Pass control frames mask
#define EMAC_MFFR_DBF       0x00000020      // Disable broadcast frames
#define EMAC_MFFR_PAM       0x00000010      // Pass all multicast
#define EMAC_MFFR_DAIF      0x00000008      // Dest. address inverse filtering
#define EMAC_MFFR_HMC       0x00000004      // Hash multicast
#define EMAC_MFFR_HUC       0x00000002      // Hash unicast
#define EMAC_MFFR_PR        0x00000001      // Promiscuous mode

/* MAC MII Address Register */
#define EMAC_MMAR_PA        0x0000F800      // PHY address mask
#define EMAC_MMAR_GR        0x000007C0      // MII register address mask
#define EMAC_MMAR_CR        0x0000003C      // Clock range mask
#define EMAC_MMAR_W         0x00000002      // MII write
#define EMAC_MMAR_GB        0x00000001      // MII busy

/* MAC MII Address Register Clock Range */
#define EMAC_MMAR_CR_Div102 (0x4 << 2)      // Clk_M3_Ethernet: 150-250 MHz
#define EMAC_MMAR_CR_Div62  (0x1 << 2)      // Clk_M3_Ethernet: 100-150 MHz
#define EMAC_MMAR_CR_Div42  (0x0 << 2)      // Clk_M3_Ethernet:  60-100 MHz
#define EMAC_MMAR_CR_Div26  (0x3 << 2)      // Clk_M3_Ethernet:  35- 60 MHz
#define EMAC_MMAR_CR_Div16  (0x2 << 2)      // Clk_M3_Ethernet:  20- 35 MHz

/* MAC MII Data Register */
#define EMAC_MMDR_GD        0x0000FFFF      // MII 16-bit rw data

/* MAC Flow Control Register */
#define EMAC_MFCR_PT        0xFFFF0000      // Pause time mask
#define EMAC_MFCR_DZPQ      0x00000080      // Disable zero-quanta pause
#define EMAC_MFCR_PLT       0x00000030      // Pause low threshold
#define EMAC_MFCR_UP        0x00000008      // Unicaste pause frame detect
#define EMAC_MFCR_RFE       0x00000004      // Receive flow control enable
#define EMAC_MFCR_TFE       0x00000002      // Transmit flow control enable
#define EMAC_MFCR_FCB       0x00000001      // Flow ctrl busy/backpressure activate

/* MAC Power Management Control and Status Register */
#define EMAC_PMTR_WFFRPR    0x80000000      // Wake-up frame filter register pointer reset
#define EMAC_PMTR_GU        0x00000200      // Global unicast wake-up enable
#define EMAC_PMTR_WFR       0x00000040      // Wake-up frame received status
#define EMAC_PMTR_MPR       0x00000020      // Magic packet received status
#define EMAC_PMTR_WFE       0x00000004      // Wake-up frame enable
#define EMAC_PMTR_MPE       0x00000002      // Magic packet enable
#define EMAC_PMTR_PD        0x00000001      // Power-down

/* MAC Interrupt Status Register */
#define EMAC_MISR_TS        0x00000200      // Timestamp interrupt status
#define EMAC_MISR_PMT       0x00000008      // PMT interrupt status

/* MAC Interrupt Mask Register */
#define EMAC_MIMR_TSIM      0x00000200      // Timestamp interrupt mask
#define EMAC_MIMR_PMTIM     0x00000008      // PMT interrupt mask

/* MAC Timestamp Control Register */
#define EMAC_MTCR_TSENMA    0x00040000      // Enable MAC address for PTP frame filtering
#define EMAC_MTCR_TSCLKT    0x00030000      // Select the type of clock node, see manual
#define EMAC_MTCR_TSMSTR    0x00008000      // Enable Snapshot for messages relevant to master
#define EMAC_MTCR_TSEVNT    0x00004000      // Enable TS snapshot for event messages
#define EMAC_MTCR_TSIPV4E   0x00002000      // Enable TS snapshot for IPv4 frames
#define EMAC_MTCR_TSIPV6E   0x00001000      // Enable TS snapshot for IPv6 frames
#define EMAC_MTCR_TSIPENA   0x00000800      // Enable TS snapshot for PTP over ethernet frames
#define EMAC_MTCR_TSVER2    0x00000400      // PTP packet snooping for version 2 format
#define EMAC_MTCR_TSCTRL    0x00000200      // Timestamp digital or binary rollover control
#define EMAC_MTCR_TSENAL    0x00000100      // Enable timestamp for all frames
#define EMAC_MTCR_TSADDR    0x00000020      // Addend register update
#define EMAC_MTCR_TSTRIG    0x00000010      // Timestamp interrupt trigger enable
#define EMAC_MTCR_TSUPDT    0x00000008      // Timestamp update
#define EMAC_MTCR_TSINIT    0x00000004      // Timestamp initialize
#define EMAC_MTCR_TSCFUP    0x00000002      // Timestamp fine or coarse update
#define EMAC_MTCR_TSENA     0x00000001      // Timestamp enable

/* MAC Timestamp Status Register */
#define EMAC_MTSR_TSTARGT   0x00000002      // Timestamp target/alarm reached
#define EMAC_MTSR_TSSOVF    0x00000001      // Timestamp seconds overflow

/* DMA Status Register */
#define EMAC_DSR_EB3        0x02000000      // Error bit 3
#define EMAC_DSR_EB2        0x01000000      // Error bit 2
#define EMAC_DSR_EB1        0x00800000      // Error bit 1
#define EMAC_DSR_TS         0x00700000      // Transmit process state
#define EMAC_DSR_RS         0x000E0000      // Receive process state
#define EMAC_DSR_NIS        0x00010000      // Normal interrupt summary
#define EMAC_DSR_AIS        0x00008000      // Abnormal interrupt summary
#define EMAC_DSR_ERI        0x00004000      // Early receive interrupt
#define EMAC_DSR_FBI        0x00002000      // Fatal bus error interrupt
#define EMAC_DSR_ETI        0x00000400      // Early transmit interrupt
#define EMAC_DSR_RWT        0x00000200      // Receive watchdog timeout
#define EMAC_DSR_RPS        0x00000100      // Receive process stopped
#define EMAC_DSR_RU         0x00000080      // Receive buffer unavailable
#define EMAC_DSR_RI         0x00000040      // Receive interrupt
#define EMAC_DSR_UNF        0x00000020      // Transmit underflow
#define EMAC_DSR_OVF        0x00000010      // Receive overflow
#define EMAC_DSR_TJT        0x00000008      // Transmit jabber timeout
#define EMAC_DSR_TU         0x00000004      // Transmit buffer unavailable
#define EMAC_DSR_TPS        0x00000002      // Transmit process stopped
#define EMAC_DSR_TI         0x00000001      // Transmit interrupt

/* DMA Bus Mode Register */
#define EMAC_DBMR_TXPR      0x08000000      // DMA Tx priority
#define EMAC_DBMR_MB        0x04000000      // Mixed burst
#define EMAC_DBMR_AAL       0x02000000      // Address-aligned beats
#define EMAC_DBMR_PBL8X     0x01000000      // 8 x PBL mode
#define EMAC_DBMR_USP       0x00800000      // Use separate PBL
#define EMAC_DBMR_RPBL      0x007E0000      // Rx DMA PBL mask
#define EMAC_DBMR_FB        0x00010000      // Fixed burst
#define EMAC_DBMR_PR        0x0000C000      // Rx-to-Tx priority ratio
#define EMAC_DBMR_PBL       0x00003F00      // Programmable burst length mask
#define EMAC_DBMR_ATDS      0x00000080      // Alternate descriptor size
#define EMAC_DBMR_DSL       0x0000007C      // Descriptor skip length
#define EMAC_DBMR_DA        0x00000002      // DMA arbitration scheme
#define EMAC_DBMR_SWR       0x00000001      // Software reset

/* DMA Operation Mode Register */
#define EMAC_DOMR_DFF       0x01000000      // Disable flushing of received frames
#define EMAC_DOMR_FTF       0x00100000      // Flush transmit FIFO
#define EMAC_DOMR_TTC       0x0001C000      // Transmit treshold control mask
#define EMAC_DOMR_ST        0x00002000      // Start/stop transmission
#define EMAC_DOMR_FEF       0x00000080      // Forward error frames
#define EMAC_DOMR_FUF       0x00000040      // Forward undersized good frames
#define EMAC_DOMR_RTC       0x00000018      // Receive threshold control mask
#define EMAC_DOMR_OSF       0x00000004      // Operate on second frame
#define EMAC_DOMR_SR        0x00000002      // Start/stop receive

/* DMA Interrupt Enable Register */
#define EMAC_DIER_NIE       0x00010000      // Normal interrupt summary enable
#define EMAC_DIER_AIE       0x00008000      // Abnormal interrupt summary enable
#define EMAC_DIER_ERE       0x00004000      // Early receive interrupt enable
#define ENET_DIER_FBE       0x00002000      // Fatal bus error enable
#define EMAC_DIER_ETE       0x00000400      // Early transmit interrupt enable
#define EMAC_DIER_RWE       0x00000200      // Receive watchdog timeout enable
#define EMAC_DIER_RSE       0x00000100      // Receive stopped enable
#define EMAC_DIER_RUE       0x00000080      // Receive buffer unavailable enable
#define EMAC_DIER_RIE       0x00000040      // Receive interrupt enable
#define EMAC_DIER_UNE       0x00000020      // Underflow interrupt enable
#define EMAC_DIER_OVE       0x00000010      // Overflow interrupt enable
#define EMAC_DIER_TJE       0x00000008      // Transmit jabber timeout enable
#define EMAC_DIER_TUE       0x00000004      // Transmit buffer unavailable enable
#define EMAC_DIER_TSE       0x00000002      // Transmit stopped enable
#define EMAC_DIER_TIE       0x00000001      // Transmit interrupt enable

/* DMA RX Descriptor */
typedef struct rx_desc {
  uint32_t volatile Stat;                   // Frame status
  uint32_t          Ctrl;                   // Frame control
  uint8_t  const   *Addr;                   // Data buffer address
  struct rx_desc   *Next;                   // Next Rx descriptor address
#if (EMAC_TIME_STAMP)
  uint32_t          ExtStat;                // Extended status
  uint32_t          Reserved[1];            // Not used
  uint32_t          TimeLo;                 // Receive Timestamp low
  uint32_t          TimeHi;                 // Receive Timestamp high
#endif
} RX_Desc;

/* DMA TX Descriptor */
typedef struct tx_desc {
  uint32_t volatile CtrlStat;               // Frame control and status
  uint32_t          Size;                   // Frame size
  uint8_t          *Addr;                   // Data buffer address
  struct tx_desc   *Next;                   // Next Tx descriptor address
#if (EMAC_TIME_STAMP)
  uint32_t          Reserved[2];            // Not used
  uint32_t          TimeLo;                 // Transmit Timestamp low
  uint32_t          TimeHi;                 // Transmit Timestamp high
#endif
} TX_Desc;

/* EMAC Driver Control Information */
typedef struct {
  ARM_ETH_MAC_SignalEvent_t cb_event;       // Event callback
  uint8_t           flags;                  // Control and state flags
  uint8_t           tx_index;               // Transmit descriptor index
  uint8_t           rx_index;               // Receive descriptor index
#if (EMAC_TIME_STAMP)
  uint8_t           tx_ts_index;            // Transmit Timestamp descriptor index
#endif
  uint8_t           mmar_cr_val;            // MII Address register Clock Range 
  uint8_t          *frame_end;              // End of assembled frame fragments
} EMAC_CTRL;

#endif /* __EMAC_LPC18XX_H */
