/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       SPIMSP432DMA.h
 *
 *  @brief      SPI driver implementation for a EUSCI peripheral on MSP432
 *              using the micro DMA controller.
 *
 *  The SPI header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/SPI.h>
 *  #include <ti/drivers/spi/SPIMSP432DMA.h>
 *  @endcode
 *
 *  Refer to @ref SPI.h for a complete description of APIs & example of use.
 *
 *  This SPI driver implementation is designed to operate on a EUCSI controller
 *  in SPI mode using a micro DMA controller.
 *
 *  ## Frame Formats #
 *  This SPI controller supports 4 phase & polarity formats. Refer to the device
 *  specific data sheets & technical reference manuals for specifics on each
 *  format.
 *
 *  ## SPI Chip Select #
 *
 *  The SPI driver can be used in 3-pin or 4-pin mode.  When in 4-pin mode the
 *  hardware manages a pin as the chip select.  In 3-pin mode it is the
 *  application's responsibility to assert and de-assert a GPIO pin for chip
 *  select purposes.
 *
 *  <table>
 *  <tr>
 *  <th>Chip select type</th>
 *  <th>SPI_MASTER mode</th>
 *  <th>SPI_SLAVE mode</th>
 *  </tr>
 *  <tr>
 *  <td>Hardware chip select</td>
 *  <td>No action is needed by the application to select the peripheral.</td>
 *  <td>See the device documentation on it's chip select requirements.</td>
 *  </tr>
 *  <tr>
 *  <td>Software chip select</td>
 *  <td>The application is responsible to ensure that correct SPI slave is
 *      selected before performing a SPI_transfer().</td>
 *  <td>Up to the application's implementation.</td>
 *  </tr>
 *  </table>
 *
 *  ## SPI data frames #
 *
 *  The EUSCI controller only supports 8-bit data frames.
 *
 *  dataSize  | buffer element size |
 *  --------  | ------------------- |
 *  8 bits    | uint8_t             |
 *
 *  ## DMA operation #
 *  DMA use in this driver varies based on the SPI_TransferMode set when the
 *  driver instance was opened.  If the driver was opened in SPI_MODE_CALLBACK,
 *  all transfers make use of the DMA regardless of the amount of data.
 *
 *  If the driver was opened in SPI_MODE_BLOCKING, it verifies the amount of
 *  data frames to be transfered exceeds the minDmaTransferSize before
 *  performing a transfer using the DMA.  minDmaTransferSize (in the
 *  SPIMSP432DMA_HWAttrs) allows users to set a minimum amount of data frames
 *  a transfer must have to perform a transfer using the DMA.  If the amount of
 *  data is less than minDmaTransferSize, the driver performs a polling
 *  transfer.  This feature is provided for situations where there is little
 *  data to be transfered & it is more efficient to simply perform a polling
 *  transfer instead of configuring the DMA & waiting until the task is
 *  unblocked.
 *
 *  ## DMA Interrupts #
 *  The MSP432 DMA controller has 4 interrupt vectors to handle all DMA
 *  related IRQ. Due to the "shared" nature of the DMA interrupts, this driver
 *  implementation requires each SPI instance to explicitly use a single DMA
 *  interrupt.  It is up to the application to ensure no two peripherals are
 *  configured to respond to a given DMA interrupt at any moment.
 *
 *  ## DMA transfer size limit #
 *  The DMA controller only supports data transfers of up to 1024
 *  data frames, so large amounts of data will be split & transfered
 *  accordingly.  Each SPI driver instance requires 2 DMA channels (Tx and Rx)
 *  to operate.
 *
 *  ## DMA accessible memory #
 *
 *  Ensure that the txBuf and rxBuf (in ::SPI_Transaction) point to memory that
 *  is accessible by the DMA.
 *
 *  ## Scratch Buffers #
 *  A uint8_t scratch buffer is used to allow SPI_transfers where txBuf or
 *  rxBuf are NULL. Rather than requiring txBuf or rxBuf to have a dummy buffer
 *  of size of the transfer count, a single DMA accessible uint8_t scratch
 *  buffer is used. When txBuf is NULL, an internal scratch buffer is
 *  initialized to the defaultTxBufValue so the DMA will send some known value.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_spi_SPIMSP432DMA__include
#define ti_drivers_spi_SPIMSP432DMA__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/dma/UDMAMSP432.h>

/*
 *  SPI port/pin defines for pin configuration.  Ports P2, P3, and P7 are
 *  configurable through the port mapping controller.
 *  Value specifies the pin function and ranges from 0 to 31
 *  pin range: 0 - 7, port range: 0 - 15
 *
 *
 *    15 - 10   9  8  7 - 4  3 - 0
 *  -------------------------------
 *  |  VALUE | X | X | PORT | PIN |
 *  -------------------------------
 *
 *  value = pinConfig >> 10
 *  port = (pinConfig >> 4) & 0xf
 *  pin = pinConfig & 0x7
 *
 *  pmap = port * 0x8;  // 2 -> 0x10, 3 -> 0x18, 7 -> 0x38
 *  portMapReconfigure = PMAP_ENABLE_RECONFIGURATION;
 *
 *  Code from pmap.c:
 *   //Get write-access to port mapping registers:
 *   PMAP->KEYID = PMAP_KEYID_VAL;
 *
 *   //Enable/Disable reconfiguration during runtime
 *   PMAP->CTL = (PMAP->CTL & ~PMAP_CTL_PRECFG) | portMapReconfigure;
 *   HWREG8(PMAP_BASE + pin + pmap) = value;
 *
 *  For non-configurable ports (bits 20 - 12 will be 0).
 *  Bits 8 and 9 hold the module function (PRIMARY, SECONDARY, or
 *  TERTIALRY).
 *
 *        9         8      7 - 4  3 - 0
 *  -----------------------------------
 *  | PnSEL1.x | PnSEL0.x | PORT | PIN |
 *  -----------------------------------
 *
 *  moduleFunction = (pinConfig >> 8) & 0x3
 *  port = (pinConfig >> 4) & 0xf
 *  pin = 1 << (pinConfig & 0xf)
 *
 *  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
 *       pin, moduleFunction);
 *  or:
 *  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port,
 *       pin, moduleFunction);
 *
 */

/* Port 1 EUSCI A0 defines */
#define SPIMSP432DMA_P1_0_UCA0STE   0x00000110  /* Primary, port 1, pin 0 */
#define SPIMSP432DMA_P1_1_UCA0CLK   0x00000111  /* Primary, port 1, pin 1 */
#define SPIMSP432DMA_P1_2_UCA0SOMI  0x00000112  /* Primary, port 1, pin 2 */
#define SPIMSP432DMA_P1_3_UCA0SIMO  0x00000113  /* Primary, port 1, pin 3 */

/* Port 1 EUSCI B0 defines */
#define SPIMSP432DMA_P1_4_UCB0STE   0x00000114  /* Primary, port 1, pin 4 */
#define SPIMSP432DMA_P1_5_UCB0CLK   0x00000115  /* Primary, port 1, pin 5 */
#define SPIMSP432DMA_P1_6_UCB0SIMO  0x00000116  /* Primary, port 1, pin 6 */
#define SPIMSP432DMA_P1_7_UCB0SOMI  0x00000117  /* Primary, port 1, pin 7 */

/* Port 2, pin 0 defines */
#define SPIMSP432DMA_P2_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA1STE   ((PMAP_UCA1STE << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCA2STE   ((PMAP_UCA2STE << 10) | 0x20)
#define SPIMSP432DMA_P2_0_UCB2STE   ((PMAP_UCB2STE << 10) | 0x20)

/* Port 2, pin 1 defines */
#define SPIMSP432DMA_P2_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA1STE   ((PMAP_UCA1STE << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCA2STE   ((PMAP_UCA2STE << 10) | 0x21)
#define SPIMSP432DMA_P2_1_UCB2STE   ((PMAP_UCB2STE << 10) | 0x21)

/* Port 2, pin 2 defines */
#define SPIMSP432DMA_P2_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA1STE   ((PMAP_UCA1STE << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCA2STE   ((PMAP_UCA2STE << 10) | 0x22)
#define SPIMSP432DMA_P2_2_UCB2STE   ((PMAP_UCB2STE << 10) | 0x22)

/* Port 2, pin 3 defines */
#define SPIMSP432DMA_P2_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA1STE   ((PMAP_UCA1STE << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCA2STE   ((PMAP_UCA2STE << 10) | 0x23)
#define SPIMSP432DMA_P2_3_UCB2STE   ((PMAP_UCB2STE << 10) | 0x23)

/* Port 2, pin 4 defines */
#define SPIMSP432DMA_P2_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA1STE   ((PMAP_UCA1STE << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCA2STE   ((PMAP_UCA2STE << 10) | 0x24)
#define SPIMSP432DMA_P2_4_UCB2STE   ((PMAP_UCB2STE << 10) | 0x24)

/* Port 2, pin 5 defines */
#define SPIMSP432DMA_P2_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA1STE   ((PMAP_UCA1STE << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCA2STE   ((PMAP_UCA2STE << 10) | 0x25)
#define SPIMSP432DMA_P2_5_UCB2STE   ((PMAP_UCB2STE << 10) | 0x25)

/* Port 2, pin 6 defines */
#define SPIMSP432DMA_P2_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA1STE   ((PMAP_UCA1STE << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCA2STE   ((PMAP_UCA2STE << 10) | 0x26)
#define SPIMSP432DMA_P2_6_UCB2STE   ((PMAP_UCB2STE << 10) | 0x26)

/* Port 2, pin 7 defines */
#define SPIMSP432DMA_P2_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA1STE   ((PMAP_UCA1STE << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCA2STE   ((PMAP_UCA2STE << 10) | 0x27)
#define SPIMSP432DMA_P2_7_UCB2STE   ((PMAP_UCB2STE << 10) | 0x27)

/* Port 3, pin 0 defines */
#define SPIMSP432DMA_P3_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA2STE   ((PMAP_UCA2STE << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCA1STE   ((PMAP_UCA1STE << 10) | 0x30)
#define SPIMSP432DMA_P3_0_UCB2STE   ((PMAP_UCB2STE << 10) | 0x30)

/* Port 3, pin 1 defines */
#define SPIMSP432DMA_P3_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA1STE   ((PMAP_UCA1STE << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCA2STE   ((PMAP_UCA2STE << 10) | 0x31)
#define SPIMSP432DMA_P3_1_UCB2STE   ((PMAP_UCB2STE << 10) | 0x31)

/* Port 3, pin 2 defines */
#define SPIMSP432DMA_P3_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA1STE   ((PMAP_UCA1STE << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCA2STE   ((PMAP_UCA2STE << 10) | 0x32)
#define SPIMSP432DMA_P3_2_UCB2STE   ((PMAP_UCB2STE << 10) | 0x32)

/* Port 3, pin 3 defines */
#define SPIMSP432DMA_P3_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA1STE   ((PMAP_UCA1STE << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCA2STE   ((PMAP_UCA2STE << 10) | 0x33)
#define SPIMSP432DMA_P3_3_UCB2STE   ((PMAP_UCB2STE << 10) | 0x33)

/* Port 3, pin 4 defines */
#define SPIMSP432DMA_P3_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB2STE   ((PMAP_UCB2STE << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA1STE   ((PMAP_UCA1STE << 10) | 0x34)
#define SPIMSP432DMA_P3_4_UCA2STE   ((PMAP_UCA2STE << 10) | 0x34)

/* Port 3, pin 5 defines */
#define SPIMSP432DMA_P3_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA1STE   ((PMAP_UCA1STE << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCA2STE   ((PMAP_UCA2STE << 10) | 0x35)
#define SPIMSP432DMA_P3_5_UCB2STE   ((PMAP_UCB2STE << 10) | 0x35)

/* Port 3, pin 6 defines */
#define SPIMSP432DMA_P3_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA1STE   ((PMAP_UCA1STE << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCA2STE   ((PMAP_UCA2STE << 10) | 0x36)
#define SPIMSP432DMA_P3_6_UCB2STE   ((PMAP_UCB2STE << 10) | 0x36)

/* Port 3, pin 7 defines */
#define SPIMSP432DMA_P3_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA1STE   ((PMAP_UCA1STE << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCA2STE   ((PMAP_UCA2STE << 10) | 0x37)
#define SPIMSP432DMA_P3_7_UCB2STE   ((PMAP_UCB2STE << 10) | 0x37)

/* Port 6 EUSCI B1, B3 defines */
#define SPIMSP432DMA_P6_2_UCB1STE   0x00000162  /* Primary, port 6, pin 2 */
#define SPIMSP432DMA_P6_3_UCB1CLK   0x00000163  /* Primary, port 6, pin 3 */
#define SPIMSP432DMA_P6_4_UCB1SIMO  0x00000164  /* Primary, port 6, pin 4 */
#define SPIMSP432DMA_P6_5_UCB1SOMI  0x00000165  /* Primary, port 6, pin 5 */
#define SPIMSP432DMA_P6_6_UCB3SIMO  0x00000266  /* Secondary, port 6, pin 6 */
#define SPIMSP432DMA_P6_7_UCB3SOMI  0x00000267  /* Secondary, port 6, pin 7 */

/* Port 7, pin 0 defines */
#define SPIMSP432DMA_P7_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA1STE   ((PMAP_UCA1STE << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCA2STE   ((PMAP_UCA2STE << 10) | 0x70)
#define SPIMSP432DMA_P7_0_UCB2STE   ((PMAP_UCB2STE << 10) | 0x70)

/* Port 7, pin 1 defines */
#define SPIMSP432DMA_P7_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA1STE   ((PMAP_UCA1STE << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCA2STE   ((PMAP_UCA2STE << 10) | 0x71)
#define SPIMSP432DMA_P7_1_UCB2STE   ((PMAP_UCB2STE << 10) | 0x71)

/* Port 7, pin 2 defines */
#define SPIMSP432DMA_P7_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA1STE   ((PMAP_UCA1STE << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCA2STE   ((PMAP_UCA2STE << 10) | 0x72)
#define SPIMSP432DMA_P7_2_UCB2STE   ((PMAP_UCB2STE << 10) | 0x72)

/* Port 7, pin 3 defines */
#define SPIMSP432DMA_P7_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA1STE   ((PMAP_UCA1STE << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCA2STE   ((PMAP_UCA2STE << 10) | 0x73)
#define SPIMSP432DMA_P7_3_UCB2STE   ((PMAP_UCB2STE << 10) | 0x73)

/* Port 7, pin 4 defines */
#define SPIMSP432DMA_P7_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA1STE   ((PMAP_UCA1STE << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCA2STE   ((PMAP_UCA2STE << 10) | 0x74)
#define SPIMSP432DMA_P7_4_UCB2STE   ((PMAP_UCB2STE << 10) | 0x74)

/* Port 7, pin 5 defines */
#define SPIMSP432DMA_P7_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA1STE   ((PMAP_UCA1STE << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCA2STE   ((PMAP_UCA2STE << 10) | 0x75)
#define SPIMSP432DMA_P7_5_UCB2STE   ((PMAP_UCB2STE << 10) | 0x75)

/* Port 7, pin 6 defines */
#define SPIMSP432DMA_P7_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA1STE   ((PMAP_UCA1STE << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCA2STE   ((PMAP_UCA2STE << 10) | 0x76)
#define SPIMSP432DMA_P7_6_UCB2STE   ((PMAP_UCB2STE << 10) | 0x76)

/* Port 7, pin 7 defines */
#define SPIMSP432DMA_P7_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA1STE   ((PMAP_UCA1STE << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCA2STE   ((PMAP_UCA2STE << 10) | 0x77)
#define SPIMSP432DMA_P7_7_UCB2STE   ((PMAP_UCB2STE << 10) | 0x77)

/* Port 8 EUSCI B3 defines */
#define SPIMSP432DMA_P8_0_UCB3STE   0x00000180  /* Primary, port 8, pin 0 */
#define SPIMSP432DMA_P8_1_UCB3CLK   0x00000181  /* Primary, port 8, pin 1 */

/* Port 9 EUSCI A3 defines */
#define SPIMSP432DMA_P9_4_UCA3STE   0x00000194  /* Primary, port 9, pin 4 */
#define SPIMSP432DMA_P9_5_UCA3CLK   0x00000195  /* Primary, port 9, pin 5 */
#define SPIMSP432DMA_P9_6_UCA3SOMI  0x00000196  /* Primary, port 9, pin 6 */
#define SPIMSP432DMA_P9_7_UCA3SIMO  0x00000197  /* Primary, port 9, pin 7 */

/* Port 10 EUSCI B3 defines */
#define SPIMSP432DMA_P10_0_UCB3STE   0x000001A0  /* Primary, port 10, pin 0 */
#define SPIMSP432DMA_P10_1_UCB3CLK   0x000001A1  /* Primary, port 10, pin 1 */
#define SPIMSP432DMA_P10_2_UCB3SIMO  0x000001A2  /* Primary, port 10, pin 2 */
#define SPIMSP432DMA_P10_3_UCB3SOMI  0x000001A3  /* Primary, port 10, pin 3 */

/*!
 * @brief SPIMSP432DMA_PIN_NO_CONFIG can be used to inform the SPIMSP432DMA
 * driver that a pin should not be configured for use in the SPI bus.
 * If the simoPin, somiPin or stePin is set to SPIMSP432DMA_PIN_NO_CONFIG in the
 * SPIMSP432DMA_HWAttrs, the pin is not configured to SPI functionality during
 * SPI_open() and the pin can be used for another function.  The clkPin cannot
 * be set to SPIMSP432DMA_PIN_NO_CONFIG; the clock signal is always required
 * during communication & must be driven by the SPI bus master.
 *
 * Setting pins to SPIMSP432DMA_PIN_NO_CONFIG can be useful in the following
 * situations:
 *   1.  simoPin = SPIMSP432DMA_PIN_NO_CONFIG:
 *       Useful in situations where the master will not transmit meaningful data
 *       but is interested in receiving data from slaves.  Slaves must ignore
 *       incoming data from master.
 *
 *   2.  somiPin = SPIMSP432DMA_PIN_NO_CONFIG:
 *       Useful in situations where the SPI bus master will transmit data to the
 *       slaves & is not interested in the data returned by the slaves.
 *
 *   3.  stePin = SPIMSP432DMA_PIN_NO_CONFIG:
 *       Useful in situations the ste (chip select) pin will be handled by the
 *       application instead of automatically by the hardware.  This is useful
 *       when the SPI master has to communicate with multiple slaves.
 *
 */
#define SPIMSP432DMA_PIN_NO_CONFIG    (0x0000FFFF)

/**
 *  @addtogroup SPI_STATUS
 *  SPIMSP432DMA_STATUS_* macros are command codes only defined in the
 *  SPIMSP432DMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/spi/SPIMSP432DMA.h>
 *  @endcode
 *  @{
 */

/* Add SPIMSP432DMA_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup SPI_CMD
 *  SPIMSP432DMA_CMD_* macros are command codes only defined in the
 *  SPIMSP432DMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/spi/SPIMSP432DMA.h>
 *  @endcode
 *  @{
 */

/* Add SPIMSP432DMA_CMD_* macros here */

/** @}*/

/* SPI function table pointer */
extern const SPI_FxnTable SPIMSP432DMA_fxnTable;

/*!
 *  @brief  SPIMSP432DMA Hardware attributes
 *  These fields, with the exception of intPriority,
 *  are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSP432 driverlib these definitions are
 *  found in:
 *      - dma.h
 *      - spi.h
 *
 *  intPriority is the SPI peripheral's interrupt priority, as defined by the
 *  underlying OS.  It is passed unmodified to the underlying OS's interrupt
 *  handler creation code, so you need to refer to the OS documentation
 *  for usage.  For example, for SYS/BIOS applications, refer to the
 *  ti.sysbios.family.arm.m3.Hwi documentation for SYS/BIOS usage of
 *  interrupt priorities.  If the driver uses the ti.dpl interface
 *  instead of making OS calls directly, then the HwiP port handles the
 *  interrupt priority in an OS specific way.  In the case of the SYS/BIOS
 *  port, intPriority is passed unmodified to Hwi_create().
 *
 *  A sample structure is shown below:
 *  @code
 *  const SPIMSP432DMA_HWAttrsV1 spiMSP432DMAHWAttrs[] = {
 *      {
 *          .baseAddr = EUSCI_B0_BASE,
 *          .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
 *          .bitOrder = EUSCI_B_SPI_MSB_FIRST,
 *
 *          .defaultTxBufValue = 0,
 *
 *          .dmaIntNum = INT_DMA_INT1,
 *          .intPriority = (~0),
 *          .rxDMAChannelIndex = DMA_CH1_EUSCIB0RX0,
 *          .txDMAChannelIndex = DMA_CH0_EUSCIB0TX0,
 *          .clkPin  = SPIMSP432DMA_P1_5_UCB0CLK,
 *          .simoPin = SPIMSP432DMA_P1_6_UCB0SIMO,
 *          .somiPin = SPIMSP432DMA_P1_7_UCB0SOMI,
 *          .stePin  = SPIMSP432DMA_P1_4_UCB0STE,
 *          .pinMode  = EUSCI_SPI_3PIN,
 *          .minDmaTransferSize = 10
 *      },
 *      {
 *          .baseAddr = EUSCI_B2_BASE,
 *          .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
 *          .bitOrder = EUSCI_B_SPI_MSB_FIRST,
 *
 *          .defaultTxBufValue = 0,
 *
 *          .dmaIntNum = INT_DMA_INT2,
 *          .intPriority = (~0),
 *          .rxDMAChannelIndex = DMA_CH5_EUSCIB2RX0,
 *          .txDMAChannelIndex = DMA_CH4_EUSCIB2TX0,
 *          .clkPin  = SPIMSP432DMA_P3_5_UCB2CLK,
 *          .simoPin = SPIMSP432DMA_P3_6_UCB2SIMO,
 *          .somiPin = SPIMSP432DMA_P3_7_UCB2SOMI,
 *          .stePin  = SPIMSP432DMA_P3_4_UCB2STE,
 *          .pinMode  = EUSCI_SPI_3PIN,
 *          .minDmaTransferSize = 10
 *      }
 *  };
 *  @endcode
 */
typedef struct SPIMSP432DMA_HWAttrsV1 {
    uint32_t baseAddr;           /*!< EUSCI_B_SPI Peripheral's base address */
    uint16_t bitOrder;           /*!< EUSCI_B_SPI Bit order */
    uint8_t  clockSource;        /*!< EUSCI_B_SPI Clock source */

    uint8_t  defaultTxBufValue;  /*!< Default TX value if txBuf == NULL */

    uint8_t  dmaIntNum;          /*!< DMA interrupt number */
    uint32_t intPriority;        /*!< DMA interrupt priority */
    uint32_t rxDMAChannelIndex;  /*!< DMA rxDMAChannel for Rx data */
    uint32_t txDMAChannelIndex;  /*!< DMA txDMAChannel for Tx data */

    uint16_t simoPin;            /*!< Port/pin for SIMO (MOSI) */
    uint16_t somiPin;            /*!< Port/pin for SOMI (MISO) */
    uint16_t clkPin;             /*!< Port/pin for SPI CLK */
    uint16_t stePin;             /*!< Port/pin for STE */
    uint16_t pinMode;            /*!< 3-pin or 4-pin mode */

    uint16_t minDmaTransferSize; /*!< Minimum amount of data for DMA transfer */
} SPIMSP432DMA_HWAttrsV1;

/*!
 *  @brief  SPIMSP432DMA Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct SPIMSP432DMA_Object {
    HwiP_Handle        hwiHandle;
    Power_NotifyObj    perfChangeNotify;
    SemaphoreP_Handle  transferComplete;
    SPI_CallbackFxn    transferCallbackFxn;
    SPI_Transaction   *transaction;
    UDMAMSP432_Handle  dmaHandle;

    size_t             amtDataXferred;
    size_t             currentXferAmt;
    uint32_t           bitRate;
    uint32_t           perfConstraintMask;
    uint32_t           transferTimeout;
    uint16_t           clockPolarity;
    uint16_t           clockPhase;

    SPI_Mode           spiMode;
    SPI_TransferMode   transferMode;

    bool               cancelInProgress;
    bool               isOpen;
    uint8_t            scratchBuffer;
} SPIMSP432DMA_Object, *SPIMSP432DMA_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_spi_SPIMSP432DMA__include */
