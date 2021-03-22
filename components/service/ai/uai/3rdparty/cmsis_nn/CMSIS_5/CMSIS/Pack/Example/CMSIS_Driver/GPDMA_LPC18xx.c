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
 * $Revision:    V1.3
 *
 * Project:      GPDMA Driver for NXP LPC18xx
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.3
 *    - Corrected transfers bigger than 4k
 *  Version 1.2
 *    - Added GPDMA_ChannelGetCount function.
 *  Version 1.1
 *    - Updated Initialize and Uninitialize functions
 */

#include "LPC18xx.h"
#include "GPDMA_LPC18xx.h"

// GPDMA Channel register block structure
typedef struct {
  __IO uint32_t  SRCADDR;       // DMA Channel Source Address Register
  __IO uint32_t  DESTADDR;      // DMA Channel Destination Address Register
  __IO uint32_t  LLI;           // DMA Channel Linked List Item Register
  __IO uint32_t  CONTROL;       // DMA Channel Control Register
  __IO uint32_t  CONFIG;        // DMA Channel Configuration Register
  __I  uint32_t  RESERVED1[3];
} GPDMA_CHANNEL_REG;

typedef struct {
  uint32_t            SrcAddr;
  uint32_t            DestAddr;
  uint32_t            Size;
  uint32_t            Cnt;
  GPDMA_SignalEvent_t cb_event;
} GPDMA_Channel_Info;

static uint32_t Channel_active = 0U;
static uint32_t Init_cnt       = 0U;

static GPDMA_Channel_Info Channel_info[GPDMA_NUMBER_OF_CHANNELS] = { 0U };

#define GPDMA_CHANNEL(n)  ((GPDMA_CHANNEL_REG *) (&(LPC_GPDMA->C0SRCADDR) + (n * 8U)))


/**
  \fn          int32_t Set_Channel_active_flag (uint8_t ch)
  \brief       Protected set of channel active flag
  \param[in]   ch        Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
__inline static int32_t Set_Channel_active_flag (uint8_t ch) {
  uint32_t val;

  do {
    val = __LDREXW (&Channel_active);
    if (val & (1U << ch)) {
      __CLREX (); 
      return -1;
    }
  } while (__STREXW (val | (1U << ch), &Channel_active));

  return 0;
}

/**
  \fn          void Clear_Channel_active_flag (uint8_t ch)
  \brief       Protected clear of channel active flag
  \param[in]   ch        Channel number (0..7)
*/
__inline static void Clear_Channel_active_flag (uint8_t ch) {
  while(__STREXW((__LDREXW(&Channel_active) & ~(1U << ch)), &Channel_active));
}

/**
  \fn          int32_t GPDMA_Initialize (void)
  \brief       Initialize GPDMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_Initialize (void) {
  uint32_t ch_num;

  Init_cnt++;

  // Check if already initialized
  if (Init_cnt > 1U) { return 0; }

  // Enable DMA clock
  LPC_CCU1->CLK_M3_DMA_CFG |= 1U;
  while ((LPC_CCU1->CLK_M3_DMA_STAT & 1U) == 0U);

  // Reset DMA
  LPC_RGU->RESET_CTRL0 = (1U << 19);

  // Reset all DMA channels
  for (ch_num = 0U; ch_num < GPDMA_NUMBER_OF_CHANNELS; ch_num++) {
    GPDMA_CHANNEL(ch_num)->CONFIG = 0U;
    Channel_info[ch_num].SrcAddr  = 0U;
    Channel_info[ch_num].DestAddr = 0U;
    Channel_info[ch_num].Size     = 0U;
    Channel_info[ch_num].Cnt      = 0U;
  }

  // Clear all DMA interrupt flags
  LPC_GPDMA->INTTCCLEAR = 0xFF;
  LPC_GPDMA->INTERRCLR = 0xFF;

  // Clear and Enable DMA IRQ
  NVIC_ClearPendingIRQ(DMA_IRQn);
  NVIC_EnableIRQ(DMA_IRQn);

  return 0;
}

/**
  \fn          int32_t GPDMA_Uninitialize (void)
  \brief       De-initialize GPDMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_Uninitialize (void) {

  // Check if DMA is initialized
  if (Init_cnt == 0U) { return -1; }

  Init_cnt--;
  if (Init_cnt != 0U) { return 0; }

  // Disable DMA clock
  LPC_CCU1->CLK_M3_DMA_CFG &= ~1U;

  // Disable and Clear DMA IRQ
  NVIC_DisableIRQ(DMA_IRQn);
  NVIC_ClearPendingIRQ(DMA_IRQn);

  return 0;
}

/**
  \fn          int32_t GPDMA_PeripheralSelect (uint8_t peri, uint8_t sel)
  \brief       Selects GPDMA requests
  \param[in]   peri GPDMA peripheral
  \param[in]   sel  Selects the DMA request for GPDMA input (0..3)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_PeripheralSelect (uint8_t peri, uint8_t sel) {

  if ((peri > 15U) || (sel > 3U)) { return -1; }

  LPC_CREG->DMAMUX = (LPC_CREG->DMAMUX & ~(3U   << (2U * peri))) | (sel  << (2U * peri));

  return 0;
}

/**
  \fn          int32_t GPDMA_ChannelConfigure (uint8_t              ch,
                                               uint32_t             src_addr,
                                               uint32_t             dest_addr,
                                               uint32_t             size,
                                               uint32_t             control,
                                               uint32_t             config,
                                               GPDMA_SignalEvent_t  cb_event)
  \brief       Configure GPDMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dest_addr Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   config    Channel configuration
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_ChannelConfigure (uint8_t              ch,
                                uint32_t             src_addr,
                                uint32_t             dest_addr,
                                uint32_t             size,
                                uint32_t             control,
                                uint32_t             config,
                                GPDMA_SignalEvent_t  cb_event) {
  GPDMA_CHANNEL_REG * dma_ch;

  // Check if channel is valid
  if (ch >= GPDMA_NUMBER_OF_CHANNELS)     { return -1; }

  // Set Channel active flag
  if (Set_Channel_active_flag (ch) == -1) { return -1; }

  // Save callback pointer
  Channel_info[ch].cb_event = cb_event;

  dma_ch = GPDMA_CHANNEL(ch);

  // Reset DMA Channel configuration
  dma_ch->CONFIG  = 0U;
  dma_ch->CONTROL = 0U;

  // Clear DMA interrupts
  LPC_GPDMA->INTTCCLEAR = (1U << ch);
  LPC_GPDMA->INTERRCLR  = (1U << ch);

  // Link list not supported
  dma_ch->LLI = 0U;

  // Enable DMA Channels, little endian
  LPC_GPDMA->CONFIG = GPDMA_CONFIG_E;
  while ((LPC_GPDMA->CONFIG & GPDMA_CONFIG_E) == 0U);

  Channel_info[ch].Size = size;
  if (size > 0x0FFFU) {
    // Max DMA transfer size = 4k
    size = 0x0FFFU;
  }

  control = (control & ~GPDMA_CH_CONTROL_TRANSFERSIZE_MSK) | GPDMA_CH_CONTROL_TRANSFERSIZE(size);

  // Set Source and destination address
  dma_ch->SRCADDR  = src_addr;
  dma_ch->DESTADDR = dest_addr;

  if (control & GPDMA_CH_CONTROL_SI) {
    // Source address increment
    src_addr += (size << ((control & GPDMA_CH_CONTROL_SWIDTH_MSK) >> GPDMA_CH_CONTROL_SWIDTH_POS));
  }
  if (control & GPDMA_CH_CONTROL_DI) {
    // Destination address increment
    dest_addr += (size << ((control & GPDMA_CH_CONTROL_DWIDTH_MSK) >> GPDMA_CH_CONTROL_DWIDTH_POS));
  }

  // Save channel information
  Channel_info[ch].SrcAddr  = src_addr;
  Channel_info[ch].DestAddr = dest_addr;
  Channel_info[ch].Cnt      = size;

  dma_ch->CONTROL = control;
  dma_ch->CONFIG  = config;

  if ((config & GPDMA_CONFIG_E) == 0U) {
    // Clear Channel active flag
    Clear_Channel_active_flag (ch);
  }

  return 0;
}

/**
  \fn          int32_t GPDMA_ChannelEnable (uint8_t ch)
  \brief       Enable GPDMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_ChannelEnable (uint8_t ch) {

  // Check if channel is valid
  if (ch >= GPDMA_NUMBER_OF_CHANNELS)     { return -1; }

  // Set Channel active flag
  if (Set_Channel_active_flag (ch) == -1) { return -1; }

  GPDMA_CHANNEL(ch)->CONFIG |= GPDMA_CH_CONFIG_E;
  return 0;
}

/**
  \fn          int32_t GPDMA_ChannelDisable (uint8_t ch)
  \brief       Disable GPDMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GPDMA_ChannelDisable (uint8_t ch) {

  // Check if channel is valid
  if (ch >= GPDMA_NUMBER_OF_CHANNELS) { return -1; }

  // Clear Channel active flag
  Clear_Channel_active_flag (ch);

  GPDMA_CHANNEL(ch)->CONFIG &= ~GPDMA_CH_CONFIG_E;

  return 0;
}

/**
  \fn          uint32_t GPDMA_ChannelGetStatus (uint8_t ch)
  \brief       Check if GPDMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*/
uint32_t GPDMA_ChannelGetStatus (uint8_t ch) {

  // Check if channel is valid
  if (ch >= GPDMA_NUMBER_OF_CHANNELS) { return 0U; };

  if (Channel_active & (1 << ch)) { return 1U; }
  else                            { return 0U; }
}

/**
  \fn          uint32_t GPDMA_ChannelGetCount (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*/
uint32_t GPDMA_ChannelGetCount (uint8_t ch) {
  // Check if channel is valid
  if (ch >= GPDMA_NUMBER_OF_CHANNELS) return 0;

  return (Channel_info[ch].Cnt - (GPDMA_CHANNEL(ch)->CONTROL & GPDMA_CH_CONTROL_TRANSFERSIZE_MSK));
}

/**
  \fn          void DMA_IRQHandler (void)
  \brief       DMA interrupt handler
*/
void DMA_IRQHandler (void) {
  uint32_t ch, size;
  GPDMA_CHANNEL_REG * dma_ch;

  for (ch = 0; ch < GPDMA_NUMBER_OF_CHANNELS; ch++) {
    if (LPC_GPDMA->INTSTAT & (1U << ch)) {
      dma_ch = GPDMA_CHANNEL(ch);

      // Terminal count request interrupt
      if (LPC_GPDMA->INTTCSTAT & (1U << ch)) {
        // Clear interrupt flag
        LPC_GPDMA->INTTCCLEAR = (1U << ch);

        if (Channel_info[ch].Cnt != Channel_info[ch].Size) {
          // Data waiting to transfer

          size = Channel_info[ch].Size - Channel_info[ch].Cnt;
          // Max DMA transfer size = 4k
          if (size > 0x0FFFU) { size = 0x0FFFU; }

          Channel_info[ch].Cnt += size;

          if (dma_ch->CONTROL & GPDMA_CH_CONTROL_SI) {
            // Source Address Increment
            dma_ch->SRCADDR = Channel_info[ch].SrcAddr;
            Channel_info[ch].SrcAddr += (size << ((dma_ch->CONTROL & GPDMA_CH_CONTROL_SWIDTH_MSK) >> GPDMA_CH_CONTROL_SWIDTH_POS));
          }
          if (dma_ch->CONTROL & GPDMA_CH_CONTROL_DI) {
            // Destination address increment
            dma_ch->DESTADDR = Channel_info[ch].DestAddr;
            Channel_info[ch].DestAddr += (size << ((dma_ch->CONTROL & GPDMA_CH_CONTROL_DWIDTH_MSK) >> GPDMA_CH_CONTROL_DWIDTH_POS));
          }

          // Set transfer size
          dma_ch->CONTROL = (dma_ch->CONTROL & ~GPDMA_CH_CONTROL_TRANSFERSIZE_MSK) | GPDMA_CH_CONTROL_TRANSFERSIZE(size);

          // Enable DMA Channel
          dma_ch->CONFIG |= GPDMA_CH_CONFIG_E;
        } else {
          // All Data has been transferred

          // Clear Channel active flag
          Clear_Channel_active_flag (ch);

          // Signal Event
          if (Channel_info[ch].cb_event) {
            Channel_info[ch].cb_event(GPDMA_EVENT_TERMINAL_COUNT_REQUEST);
          }
        }
      } else {
        // DMA error interrupt
        if (LPC_GPDMA->INTERRSTAT & (1U << ch)) {
          dma_ch->CONFIG  = 0U;
          dma_ch->CONTROL = 0U;

          // Clear Channel active flag
          Clear_Channel_active_flag (ch);

          // Clear interrupt flag
          LPC_GPDMA->INTERRCLR = (1U << ch);

          // Signal Event
          if (Channel_info[ch].cb_event) {
            Channel_info[ch].cb_event(GPDMA_EVENT_ERROR);
          }
        }
      }
    }
  }
}
