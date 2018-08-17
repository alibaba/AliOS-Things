/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  @file       SDSPIMSP432.h
 *
 *  @brief      SDSPI driver implementation using an EUSCI SPI peripheral for
 *              MSP432.
 *
 *  The SDSPI header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/SDSPI.h>
 *  #include <ti/drivers/sdspi/SDSPIMSP432.h>
 *  @endcode
 *
 *  Refer to @ref SDSPI.h for a complete description of APIs & example of use.
 *
 *  This SDSPI driver implementation is designed to operate on a EUSCI SPI
 *  controller in a simple polling method.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_sdspi_SDSPIMSP432__include
#define ti_drivers_sdspi_SDSPIMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <ti/drivers/SDSPI.h>

#include <third_party/fatfs/ff.h>
#include <third_party/fatfs/diskio.h>

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
#define SDSPIMSP432_P1_1_UCA0CLK   (0x0111)  /* Primary, port 1, pin 1 */
#define SDSPIMSP432_P1_2_UCA0SOMI  (0x0112)  /* Primary, port 1, pin 2 */
#define SDSPIMSP432_P1_3_UCA0SIMO  (0x0113)  /* Primary, port 1, pin 3 */

/* Port 1 EUSCI B0 defines */
#define SDSPIMSP432_P1_5_UCB0CLK   (0x0115)  /* Primary, port 1, pin 5 */
#define SDSPIMSP432_P1_6_UCB0SIMO  (0x0116)  /* Primary, port 1, pin 6 */
#define SDSPIMSP432_P1_7_UCB0SOMI  (0x0117)  /* Primary, port 1, pin 7 */

/* Port 2, pin 0 defines */
#define SDSPIMSP432_P2_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x20)
#define SDSPIMSP432_P2_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x20)

/* Port 2, pin 1 defines */
#define SDSPIMSP432_P2_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x21)
#define SDSPIMSP432_P2_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x21)

/* Port 2, pin 2 defines */
#define SDSPIMSP432_P2_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x22)
#define SDSPIMSP432_P2_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x22)

/* Port 2, pin 3 defines */
#define SDSPIMSP432_P2_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x23)
#define SDSPIMSP432_P2_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x23)

/* Port 2, pin 4 defines */
#define SDSPIMSP432_P2_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x24)
#define SDSPIMSP432_P2_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x24)

/* Port 2, pin 5 defines */
#define SDSPIMSP432_P2_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x25)
#define SDSPIMSP432_P2_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x25)

/* Port 2, pin 6 defines */
#define SDSPIMSP432_P2_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x26)
#define SDSPIMSP432_P2_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x26)

/* Port 2, pin 7 defines */
#define SDSPIMSP432_P2_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x27)
#define SDSPIMSP432_P2_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x27)

/* Port 3, pin 0 defines */
#define SDSPIMSP432_P3_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x30)
#define SDSPIMSP432_P3_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x30)

/* Port 3, pin 1 defines */
#define SDSPIMSP432_P3_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x31)
#define SDSPIMSP432_P3_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x31)

/* Port 3, pin 2 defines */
#define SDSPIMSP432_P3_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x32)
#define SDSPIMSP432_P3_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x32)

/* Port 3, pin 3 defines */
#define SDSPIMSP432_P3_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x33)
#define SDSPIMSP432_P3_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x33)

/* Port 3, pin 4 defines */
#define SDSPIMSP432_P3_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x34)
#define SDSPIMSP432_P3_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x34)

/* Port 3, pin 5 defines */
#define SDSPIMSP432_P3_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x35)
#define SDSPIMSP432_P3_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x35)

/* Port 3, pin 6 defines */
#define SDSPIMSP432_P3_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x36)
#define SDSPIMSP432_P3_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x36)

/* Port 3, pin 7 defines */
#define SDSPIMSP432_P3_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x37)
#define SDSPIMSP432_P3_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x37)

/* Port 6 EUSCI B1, B3 defines */
#define SDSPIMSP432_P6_3_UCB1CLK   (0x0163)  /* Primary, port 6, pin 3 */
#define SDSPIMSP432_P6_4_UCB1SIMO  (0x0164)  /* Primary, port 6, pin 4 */
#define SDSPIMSP432_P6_5_UCB1SOMI  (0x0165)  /* Primary, port 6, pin 5 */
#define SDSPIMSP432_P6_6_UCB3SIMO  (0x0266)  /* Secondary, port 6, pin 6 */
#define SDSPIMSP432_P6_7_UCB3SOMI  (0x0267)  /* Secondary, port 6, pin 7 */

/* Port 7, pin 0 defines */
#define SDSPIMSP432_P7_0_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x70)
#define SDSPIMSP432_P7_0_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x70)

/* Port 7, pin 1 defines */
#define SDSPIMSP432_P7_1_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x71)
#define SDSPIMSP432_P7_1_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x71)

/* Port 7, pin 2 defines */
#define SDSPIMSP432_P7_2_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x72)
#define SDSPIMSP432_P7_2_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x72)

/* Port 7, pin 3 defines */
#define SDSPIMSP432_P7_3_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x73)
#define SDSPIMSP432_P7_3_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x73)

/* Port 7, pin 4 defines */
#define SDSPIMSP432_P7_4_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x74)
#define SDSPIMSP432_P7_4_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x74)

/* Port 7, pin 5 defines */
#define SDSPIMSP432_P7_5_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x75)
#define SDSPIMSP432_P7_5_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x75)

/* Port 7, pin 6 defines */
#define SDSPIMSP432_P7_6_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x76)
#define SDSPIMSP432_P7_6_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x76)

/* Port 7, pin 7 defines */
#define SDSPIMSP432_P7_7_UCA0CLK   ((PMAP_UCA0CLK << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA0SIMO  ((PMAP_UCA0SIMO << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA0SOMI  ((PMAP_UCA0SOMI << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA1CLK   ((PMAP_UCA1CLK << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA1SIMO  ((PMAP_UCA1SIMO << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA1SOMI  ((PMAP_UCA1SOMI << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA2CLK   ((PMAP_UCA2CLK << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA2SIMO  ((PMAP_UCA2SIMO << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCA2SOMI  ((PMAP_UCA2SOMI << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB0CLK   ((PMAP_UCB0CLK << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB0SIMO  ((PMAP_UCB0SIMO << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB0SOMI  ((PMAP_UCB0SOMI << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB2CLK   ((PMAP_UCB2CLK << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB2SIMO  ((PMAP_UCB2SIMO << 10) | 0x77)
#define SDSPIMSP432_P7_7_UCB2SOMI  ((PMAP_UCB2SOMI << 10) | 0x77)

/* Port 8 EUSCI B3 defines */
#define SDSPIMSP432_P8_1_UCB3CLK   (0x0181)  /* Primary, port 8, pin 1 */

/* Port 9 EUSCI A3 defines */
#define SDSPIMSP432_P9_5_UCA3CLK   (0x0195)  /* Primary, port 9, pin 5 */
#define SDSPIMSP432_P9_6_UCA3SOMI  (0x0196)  /* Primary, port 9, pin 6 */
#define SDSPIMSP432_P9_7_UCA3SIMO  (0x0197)  /* Primary, port 9, pin 7 */

/* Port 10 EUSCI B3 defines */
#define SDSPIMSP432_P10_1_UCB3CLK  (0x01A1)  /* Primary, port 10, pin 1 */
#define SDSPIMSP432_P10_2_UCB3SIMO (0x01A2)  /* Primary, port 10, pin 2 */
#define SDSPIMSP432_P10_3_UCB3SOMI (0x01A3)  /* Primary, port 10, pin 3 */


/*
 *  Pin for chip select.  This pin will be configured as GPIO output.
 */
/* Port 1 */
#define SDSPIMSP432_P1_0_CS      (0x0010)
#define SDSPIMSP432_P1_1_CS      (0x0011)
#define SDSPIMSP432_P1_2_CS      (0x0012)
#define SDSPIMSP432_P1_3_CS      (0x0013)
#define SDSPIMSP432_P1_4_CS      (0x0014)
#define SDSPIMSP432_P1_5_CS      (0x0015)
#define SDSPIMSP432_P1_6_CS      (0x0016)
#define SDSPIMSP432_P1_7_CS      (0x0017)

/* Port 2 */
#define SDSPIMSP432_P2_0_CS      (0x0020)
#define SDSPIMSP432_P2_1_CS      (0x0021)
#define SDSPIMSP432_P2_2_CS      (0x0022)
#define SDSPIMSP432_P2_3_CS      (0x0023)
#define SDSPIMSP432_P2_4_CS      (0x0024)
#define SDSPIMSP432_P2_5_CS      (0x0025)
#define SDSPIMSP432_P2_6_CS      (0x0026)
#define SDSPIMSP432_P2_7_CS      (0x0027)

/* Port 3 */
#define SDSPIMSP432_P3_0_CS      (0x0030)
#define SDSPIMSP432_P3_1_CS      (0x0031)
#define SDSPIMSP432_P3_2_CS      (0x0032)
#define SDSPIMSP432_P3_3_CS      (0x0033)
#define SDSPIMSP432_P3_4_CS      (0x0034)
#define SDSPIMSP432_P3_5_CS      (0x0035)
#define SDSPIMSP432_P3_6_CS      (0x0036)
#define SDSPIMSP432_P3_7_CS      (0x0037)

/* Port 4 */
#define SDSPIMSP432_P4_0_CS      (0x0040)
#define SDSPIMSP432_P4_1_CS      (0x0041)
#define SDSPIMSP432_P4_2_CS      (0x0042)
#define SDSPIMSP432_P4_3_CS      (0x0043)
#define SDSPIMSP432_P4_4_CS      (0x0044)
#define SDSPIMSP432_P4_5_CS      (0x0045)
#define SDSPIMSP432_P4_6_CS      (0x0046)
#define SDSPIMSP432_P4_7_CS      (0x0047)

/* Port 5 */
#define SDSPIMSP432_P5_0_CS      (0x0050)
#define SDSPIMSP432_P5_1_CS      (0x0051)
#define SDSPIMSP432_P5_2_CS      (0x0052)
#define SDSPIMSP432_P5_3_CS      (0x0053)
#define SDSPIMSP432_P5_4_CS      (0x0054)
#define SDSPIMSP432_P5_5_CS      (0x0055)
#define SDSPIMSP432_P5_6_CS      (0x0056)
#define SDSPIMSP432_P5_7_CS      (0x0057)

/* Port 6 */
#define SDSPIMSP432_P6_0_CS      (0x0060)
#define SDSPIMSP432_P6_1_CS      (0x0061)
#define SDSPIMSP432_P6_2_CS      (0x0062)
#define SDSPIMSP432_P6_3_CS      (0x0063)
#define SDSPIMSP432_P6_4_CS      (0x0064)
#define SDSPIMSP432_P6_5_CS      (0x0065)
#define SDSPIMSP432_P6_6_CS      (0x0066)
#define SDSPIMSP432_P6_7_CS      (0x0067)

/* Port 7 */
#define SDSPIMSP432_P7_0_CS      (0x0070)
#define SDSPIMSP432_P7_1_CS      (0x0071)
#define SDSPIMSP432_P7_2_CS      (0x0072)
#define SDSPIMSP432_P7_3_CS      (0x0073)
#define SDSPIMSP432_P7_4_CS      (0x0074)
#define SDSPIMSP432_P7_5_CS      (0x0075)
#define SDSPIMSP432_P7_6_CS      (0x0076)
#define SDSPIMSP432_P7_7_CS      (0x0077)

/* Port 8 */
#define SDSPIMSP432_P8_0_CS      (0x0080)
#define SDSPIMSP432_P8_1_CS      (0x0081)
#define SDSPIMSP432_P8_2_CS      (0x0082)
#define SDSPIMSP432_P8_3_CS      (0x0083)
#define SDSPIMSP432_P8_4_CS      (0x0084)
#define SDSPIMSP432_P8_5_CS      (0x0085)
#define SDSPIMSP432_P8_6_CS      (0x0086)
#define SDSPIMSP432_P8_7_CS      (0x0087)

/* Port 9 */
#define SDSPIMSP432_P9_0_CS      (0x0090)
#define SDSPIMSP432_P9_1_CS      (0x0091)
#define SDSPIMSP432_P9_2_CS      (0x0092)
#define SDSPIMSP432_P9_3_CS      (0x0093)
#define SDSPIMSP432_P9_4_CS      (0x0094)
#define SDSPIMSP432_P9_5_CS      (0x0095)
#define SDSPIMSP432_P9_6_CS      (0x0096)
#define SDSPIMSP432_P9_7_CS      (0x0097)

/* Port 10 */
#define SDSPIMSP432_P10_0_CS     (0x00A0)
#define SDSPIMSP432_P10_1_CS     (0x00A1)
#define SDSPIMSP432_P10_2_CS     (0x00A2)
#define SDSPIMSP432_P10_3_CS     (0x00A3)
#define SDSPIMSP432_P10_4_CS     (0x00A4)
#define SDSPIMSP432_P10_5_CS     (0x00A5)
#define SDSPIMSP432_P10_6_CS     (0x00A6)
#define SDSPIMSP432_P10_7_CS     (0x00A7)


/**
 *  @addtogroup SDSPI_STATUS
 *  SDSPIMSP432_STATUS_* macros are command codes only defined in the
 *  SDSPIMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/sdspi/SDSPIMSP432.h>
 *  @endcode
 *  @{
 */

/* Add SDSPIMSP432_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup SDSPI_CMD
 *  SDSPIMSP432_CMD_* macros are command codes only defined in the
 *  SDSPIMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/sdspi/SDSPIMSP432.h>
 *  @endcode
 *  @{
 */

/* Add SDSPIMSP432_CMD_* macros here */

/** @}*/

/* SDSPI function table */
extern const SDSPI_FxnTable SDSPIMSP432_fxnTable;

/*!
 *  @brief  SD Card type inserted
 */
typedef enum SDSPIMSP432_CardType {
    SDSPIMSP432_NOCARD = 0, /*!< Unrecognized Card */
    SDSPIMSP432_MMC = 1,    /*!< Multi-media Memory Card (MMC) */
    SDSPIMSP432_SDSC = 2,   /*!< Standard SDCard (SDSC) */
    SDSPIMSP432_SDHC = 3    /*!< High Capacity SDCard (SDHC) */
} SDSPIMSP432_CardType;

/*!
 *  @brief  SDSPIMSP432 Hardware attributes
 *
 *  The SDSPIMSP432 HWAttrs configuration structure contains SPI peripheral and
 *  GPIO Pin details required by the SDSPIMSP432 driver implementation.
 *
 *  The SDSPIMSP432 driver uses this information to:
 *  - configure and reconfigure specific ports/pins to initialize the SD Card
 *    for SPI mode
 *  - identify which SPI peripheral is used for data communications
 *  - identify which GPIO port and pin is used for the SPI chip select
 *    mechanism
 *  - identify which GPIO port and pin is concurrently located on the SPI's MOSI
 *    (TX) pin.
 *
 *  @remark
 *  To initialize the SD Card into SPI mode, the SDSPI driver changes the SPI's
 *  MOSI pin into a GPIO pin so it can kept driven HIGH while the SPI SCK pin
 *  can toggle. After the initialization, the TX pin is reverted back to the SPI
 *  MOSI mode.
 *
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSP432 driverlib these definitions are
 *  found in:
 *      - gpio.h
 *      - spi.h
 *
 *  @struct SDSPIMSP432_HWAttrs
 *  An example configuration structure could look as the following:
 *  @code
 *  const SDSPIMSP432_HWAttrsV1 sdspiMSP432HWAttrs[MSP_EXP432P401R_SDSPICOUNT] = {
 *      {
 *          .baseAddr = EUSCI_B0_BASE,
 *          .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
 *
 *          // CLK, MOSI & MISO ports & pins
 *          .sckPin = SDSPIMSP432_P1_5_UCB0CLK,
 *          .somiPin = SDSPIMSP432_P1_7_UCB0SOMI,
 *          .simoPin = SDSPIMSP432_P1_6_UCB0SIMO,
 *
 *          // Chip select port & pin
 *          .csPin = SDSPIMSP432_P4_6_CS,
 *      }
 *  };
 *  @endcode
 */
typedef struct SDSPIMSP432_HWAttrsV1 {
    uint32_t   baseAddr;     /*!< SPI Peripheral's base address */
    uint8_t    clockSource;  /*!< SPIUSCI Clock source */

    uint16_t   sckPin;       /*!< Pin configuration for SPI SCK */
    uint16_t   somiPin;      /*!< Pin configuration for the MISO pin */
    uint16_t   simoPin;      /*!< Pin configuration for the MOSI pin */
    uint16_t   csPin;        /*!< Pin configuration for chip select */
} SDSPIMSP432_HWAttrsV1;

/*!
 *  @brief  SDSPIMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct SDSPIMSP432_Object {
    uint16_t             driveNumber;   /*!< Drive number used by FatFs */
    DSTATUS              diskState;     /*!< Disk status */
    SDSPIMSP432_CardType cardType;      /*!< SDCard Card Command Class (CCC) */
    uint32_t             bitRate;       /*!< SPI bus bit rate (Hz) */
    FATFS                filesystem;    /*!< FATFS data object */

    Power_NotifyObj      perfChangeNotify;
    uint32_t             perfConstraintMask;
} SDSPIMSP432_Object, *SDSPIMSP432_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_sdspi_SDSPIMSP432__include */
