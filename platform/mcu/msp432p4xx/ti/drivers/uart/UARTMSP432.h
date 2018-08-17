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
 *  @file       UARTMSP432.h
 *
 *  @brief      UART driver implementation for a EUSCIA peripheral for MSP432
 *
 *  This UART driver implementation is designed to operate on a EUSCI controller
 *  in UART mode for MSP432 devices.
 *
 *  The UART header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/UART.h>
 *  #include <ti/drivers/uart/UARTMSP432.h>
 *  @endcode
 *
 *  Refer to @ref UART.h for a complete description of APIs and example of use.
 *
 *  # Device Specific Pin Mode Macros #
 *  This header file contains port/pin macros for pin configuration. These
 *  macros are used to select the pins used for UART TX and RX in the
 *  UARTMSP432_HWAttrsV1 structure.  The following is a description of the
 *  port/pin macro bit masks.
 *
 *  The port mapping controller on MSP432P401x MCUs allows reconfigurable
 *  mapping of digital functions on ports P2, P3, and P7.  For these ports,
 *  the pin function value is encoded in bits 10-15 of the port/pin
 *  macro.  This value ranges from 0 to 31.  (You can find the port mapping
 *  definitions in the device data sheet.)
 *
 *  The pin ranges from 0 - 7 and is encoded in bits 0 - 3 (bit 3 is not
 *  actually used).  Bits 4 - 7 of the port/pin macro are used to encode
 *  the port.  For ports P2, P3, and P7, bits 8 and 9 are not used.  The
 *  table below shows the bit mapping for P2, P3, and P7 port/pin macros.
 *
 *    15 - 10 | 9 | 8 | 7 - 4 | 3 - 0 |
 *  ---------- | - | - | ----- | ------ |
 *    VALUE   | X | X | PORT | PIN |
 *
 *  Some pins that are not on ports P2, P3, or P7, can be configured as
 *  UART TX or UART RX pins (e.g., P1.2 and P1.3).  The macros for these pins
 *  will have 0 for bits 20 - 12.  Bits 8 and 9 are used to hold the control
 *  bits, PnSEL1.x and PnSEL0.x, which determine the module function (PRIMARY,
 *  SECONDARY, or TERTIALRY).  The table below shows the bit mapping for
 *  non port-mapped port/pin macros for pins that can be configured as UART
 *  TX or UART RX pins.
 *
 *    15 - 10 | 9 | 8 | 7 - 4 | 3 - 0 |
 *  ---------- | - | - | ----- | ------ |
 *    X   | PnSEL1.x | PnSEL0.x | PORT | PIN |
 *
 *
 *  ============================================================================
 */

#ifndef ti_drivers_uart_UARTMSP432__include
#define ti_drivers_uart_UARTMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/devices/DeviceFamily.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/utils/RingBuf.h>

#include <ti/devices/msp432p4xx/inc/msp.h>

#define UARTMSP432_P1_2_UCA0RXD  0x00000112  /* Primary, port 1, pin 2 */
#define UARTMSP432_P1_3_UCA0TXD  0x00000113  /* Primary, port 1, pin 3 */

/* Port 2, pin 0 */
#define UARTMSP432_P2_0_UCA0RXD ((PMAP_UCA0RXD << 10) | 0x20)
#define UARTMSP432_P2_0_UCA0TXD ((PMAP_UCA0TXD << 10) | 0x20)
#define UARTMSP432_P2_0_UCA1RXD ((PMAP_UCA1RXD << 10) | 0x20)
#define UARTMSP432_P2_0_UCA1TXD ((PMAP_UCA1TXD << 10) | 0x20)
#define UARTMSP432_P2_0_UCA2RXD ((PMAP_UCA2RXD << 10) | 0x20)
#define UARTMSP432_P2_0_UCA2TXD ((PMAP_UCA2TXD << 10) | 0x20)

/* Port 2, pin 1 */
#define UARTMSP432_P2_1_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x21)
#define UARTMSP432_P2_1_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x21)
#define UARTMSP432_P2_1_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x21)
#define UARTMSP432_P2_1_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x21)
#define UARTMSP432_P2_1_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x21)
#define UARTMSP432_P2_1_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x21)

/* Port 2, pin 2 */
#define UARTMSP432_P2_2_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x22)
#define UARTMSP432_P2_2_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x22)
#define UARTMSP432_P2_2_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x22)
#define UARTMSP432_P2_2_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x22)
#define UARTMSP432_P2_2_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x22)
#define UARTMSP432_P2_2_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x22)

/* Port 2, pin 3 */
#define UARTMSP432_P2_3_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x23)
#define UARTMSP432_P2_3_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x23)
#define UARTMSP432_P2_3_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x23)
#define UARTMSP432_P2_3_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x23)
#define UARTMSP432_P2_3_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x23)
#define UARTMSP432_P2_3_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x23)

/* Port 2, pin 4 */
#define UARTMSP432_P2_4_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x24)
#define UARTMSP432_P2_4_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x24)
#define UARTMSP432_P2_4_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x24)
#define UARTMSP432_P2_4_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x24)
#define UARTMSP432_P2_4_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x24)
#define UARTMSP432_P2_4_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x24)

/* Port 2, pin 5 */
#define UARTMSP432_P2_5_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x25)
#define UARTMSP432_P2_5_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x25)
#define UARTMSP432_P2_5_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x25)
#define UARTMSP432_P2_5_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x25)
#define UARTMSP432_P2_5_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x25)
#define UARTMSP432_P2_5_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x25)

/* Port 2, pin 6 */
#define UARTMSP432_P2_6_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x26)
#define UARTMSP432_P2_6_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x26)
#define UARTMSP432_P2_6_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x26)
#define UARTMSP432_P2_6_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x26)
#define UARTMSP432_P2_6_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x26)
#define UARTMSP432_P2_6_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x26)

/* Port 2, pin 7 */
#define UARTMSP432_P2_7_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x27)
#define UARTMSP432_P2_7_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x27)
#define UARTMSP432_P2_7_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x27)
#define UARTMSP432_P2_7_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x27)
#define UARTMSP432_P2_7_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x27)
#define UARTMSP432_P2_7_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x27)

/* Port 3, pin 0 */
#define UARTMSP432_P3_0_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x30)
#define UARTMSP432_P3_0_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x30)
#define UARTMSP432_P3_0_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x30)
#define UARTMSP432_P3_0_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x30)
#define UARTMSP432_P3_0_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x30)
#define UARTMSP432_P3_0_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x30)

/* Port 3, pin 1 */
#define UARTMSP432_P3_1_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x31)
#define UARTMSP432_P3_1_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x31)
#define UARTMSP432_P3_1_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x31)
#define UARTMSP432_P3_1_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x31)
#define UARTMSP432_P3_1_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x31)
#define UARTMSP432_P3_1_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x31)

/* Port 3, pin 2 */
#define UARTMSP432_P3_2_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x32)
#define UARTMSP432_P3_2_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x32)
#define UARTMSP432_P3_2_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x32)
#define UARTMSP432_P3_2_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x32)
#define UARTMSP432_P3_2_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x32)
#define UARTMSP432_P3_2_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x32)

/* Port 3, pin 3 */
#define UARTMSP432_P3_3_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x33)
#define UARTMSP432_P3_3_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x33)
#define UARTMSP432_P3_3_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x33)
#define UARTMSP432_P3_3_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x33)
#define UARTMSP432_P3_3_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x33)
#define UARTMSP432_P3_3_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x33)

/* Port 3, pin 4 */
#define UARTMSP432_P3_4_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x34)
#define UARTMSP432_P3_4_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x34)
#define UARTMSP432_P3_4_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x34)
#define UARTMSP432_P3_4_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x34)
#define UARTMSP432_P3_4_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x34)
#define UARTMSP432_P3_4_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x34)

/* Port 3, pin 5 */
#define UARTMSP432_P3_5_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x35)
#define UARTMSP432_P3_5_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x35)
#define UARTMSP432_P3_5_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x35)
#define UARTMSP432_P3_5_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x35)
#define UARTMSP432_P3_5_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x35)
#define UARTMSP432_P3_5_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x35)

/* Port 3, pin 6 */
#define UARTMSP432_P3_6_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x36)
#define UARTMSP432_P3_6_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x36)
#define UARTMSP432_P3_6_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x36)
#define UARTMSP432_P3_6_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x36)
#define UARTMSP432_P3_6_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x36)
#define UARTMSP432_P3_6_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x36)

/* Port 3, pin 7 */
#define UARTMSP432_P3_7_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x37)
#define UARTMSP432_P3_7_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x37)
#define UARTMSP432_P3_7_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x37)
#define UARTMSP432_P3_7_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x37)
#define UARTMSP432_P3_7_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x37)
#define UARTMSP432_P3_7_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x37)

/* Port 7, pin 0 */
#define UARTMSP432_P7_0_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x70)
#define UARTMSP432_P7_0_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x70)
#define UARTMSP432_P7_0_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x70)
#define UARTMSP432_P7_0_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x70)
#define UARTMSP432_P7_0_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x70)
#define UARTMSP432_P7_0_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x70)

/* Port 7, pin 1 */
#define UARTMSP432_P7_1_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x71)
#define UARTMSP432_P7_1_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x71)
#define UARTMSP432_P7_1_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x71)
#define UARTMSP432_P7_1_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x71)
#define UARTMSP432_P7_1_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x71)
#define UARTMSP432_P7_1_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x71)

/* Port 7, pin 2 */
#define UARTMSP432_P7_2_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x72)
#define UARTMSP432_P7_2_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x72)
#define UARTMSP432_P7_2_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x72)
#define UARTMSP432_P7_2_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x72)
#define UARTMSP432_P7_2_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x72)
#define UARTMSP432_P7_2_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x72)

/* Port 7, pin 3 */
#define UARTMSP432_P7_3_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x73)
#define UARTMSP432_P7_3_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x73)
#define UARTMSP432_P7_3_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x73)
#define UARTMSP432_P7_3_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x73)
#define UARTMSP432_P7_3_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x73)
#define UARTMSP432_P7_3_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x73)

/* Port 7, pin 4 */
#define UARTMSP432_P7_4_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x74)
#define UARTMSP432_P7_4_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x74)
#define UARTMSP432_P7_4_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x74)
#define UARTMSP432_P7_4_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x74)
#define UARTMSP432_P7_4_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x74)
#define UARTMSP432_P7_4_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x74)

/* Port 7, pin 5 */
#define UARTMSP432_P7_5_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x75)
#define UARTMSP432_P7_5_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x75)
#define UARTMSP432_P7_5_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x75)
#define UARTMSP432_P7_5_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x75)
#define UARTMSP432_P7_5_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x75)
#define UARTMSP432_P7_5_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x75)

/* Port 7, pin 6 */
#define UARTMSP432_P7_6_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x76)
#define UARTMSP432_P7_6_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x76)
#define UARTMSP432_P7_6_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x76)
#define UARTMSP432_P7_6_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x76)
#define UARTMSP432_P7_6_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x76)
#define UARTMSP432_P7_6_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x76)

/* Port 7, pin 7 */
#define UARTMSP432_P7_7_UCA0RXD  ((PMAP_UCA0RXD << 10) | 0x77)
#define UARTMSP432_P7_7_UCA0TXD  ((PMAP_UCA0TXD << 10) | 0x77)
#define UARTMSP432_P7_7_UCA1RXD  ((PMAP_UCA1RXD << 10) | 0x77)
#define UARTMSP432_P7_7_UCA1TXD  ((PMAP_UCA1TXD << 10) | 0x77)
#define UARTMSP432_P7_7_UCA2RXD  ((PMAP_UCA2RXD << 10) | 0x77)
#define UARTMSP432_P7_7_UCA2TXD  ((PMAP_UCA2TXD << 10) | 0x77)

#define UARTMSP432_P9_6_UCA3RXD  0x00000196  /* Primary, port 9, pin 6 */
#define UARTMSP432_P9_7_UCA3TXD  0x00000197  /* Primeary, port 9, pin 7 */

/**
 *  @addtogroup UART_STATUS
 *  UARTMSP432_STATUS_* macros are command codes only defined in the
 *  UARTMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTMSP432.h>
 *  @endcode
 *  @{
 */

/* Add UARTMSP432_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup UART_CMD
 *  UARTMSP432_CMD_* macros are command codes only defined in the
 *  UARTMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTMSP432.h>
 *  @endcode
 *  @{
 */

/* Add UARTMSP432_CMD_* macros here */

/** @}*/

/* UART function table pointer */
extern const UART_FxnTable UARTMSP432_fxnTable;

/*!
 *  @brief      The definition of an optional callback function used by the UART
 *              driver to notify the application when a receive error (FIFO overrun,
 *              parity error, etc) occurs.
 *
 *  @param      UART_Handle             UART_Handle
 *
 *  @param      error                   The current value of the receive
 *                                      status register.  Please refer to the
 *                                      device data sheet to interpret this
 *                                      value.
 */
typedef void (*UARTMSP432_ErrorCallback) (UART_Handle handle,  uint32_t error);

/*!
 *  @brief Complement set of read functions to be used by the UART ISR and
 *         UARTMSP432_read(). Internal use only.
 *
 *  These functions should not be used by the user and are solely intended for
 *  the UARTMSP432 driver.
 *  The UARTMSP432_FxnSet is a pair of complement functions that are design to
 *  operate with one another in a task context and in an ISR context. The
 *  readTaskFxn is called by UARTMSP432_read() to drain a circular buffer,
 *  whereas the readIsrFxn is used by the UARTMSP432_hwiIntFxn to fill up the
 *  circular buffer.
 *
 *  readTaskFxn:    Function called by UART read
 *                  These variables are set and available for use to the
 *                  readTaskFxn.
 *                  object->readBuf = buffer; //Pointer to a user buffer
 *                  object->readSize = size;  //Desired no. of bytes to read
 *                  object->readCount = size; //Remaining no. of bytes to read
 *
 *  readIsrFxn:     The required ISR counterpart to readTaskFxn
 */
typedef struct UARTMSP432_FxnSet {
    bool (*readIsrFxn)  (UART_Handle handle);
    int  (*readTaskFxn) (UART_Handle handle);
} UARTMSP432_FxnSet;

/*!
 *  @brief      UARTMSP432 Baudrate configuration
 *
 *  This structure is used to specifiy the EUSCI controller's clock divider
 *  settings to achieve the desired baudrate given the a clock input frequency.
 *  Divider values can be determined by referring to the MSP432 baudrate
 *  calculator.
 *  http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 *
 *  A sample structure is shown below:
 *  @code
 *  const UARTMSP432_BaudrateConfig uartBaudrates[] = {
 *      // {baudrate, input clock, prescalar, UCBRFx, UCBRSx, oversampling}
 *      {
 *          .outputBaudrate = 115200,
 *          .inputClockFreq = 12000000,
 *          .prescalar = 6,
 *          .hwRegUCBRFx = 8,
 *          .hwRegUCBRSx = 32,
 *          .oversampling = 1
 *      },
 *      {115200, 6000000,   3,  4,   2, 1},
 *      {115200, 3000000,   1, 10,   0, 1},
 *      {9600,   12000000, 78,  2,   0, 1},
 *      {9600,   6000000,  39,  1,   0, 1},
 *      {9600,   3000000,  19,  8,  85, 1},
 *      {9600,   32768,     3,  0, 146, 0}
 *  };
 *  @endcode
 */
typedef struct UARTMSP432_BaudrateConfig {
    uint32_t  outputBaudrate; /*!< Search criteria: desired baudrate */
    uint32_t  inputClockFreq; /*!< Search criteria: given this input clock frequency */

    uint16_t  prescalar;      /*!< Clock prescalar */
    uint8_t   hwRegUCBRFx;    /*!< UCBRFx lookup entry */
    uint8_t   hwRegUCBRSx;    /*!< UCBRSx lookup entry */
    uint8_t   oversampling;   /*!< Oversampling mode (1: True; 0: False) */
} UARTMSP432_BaudrateConfig;

/*!
 *  @brief      UARTMSP432 Hardware attributes
 *
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSP432Ware these definitions are found in:
 *      - uart.h
 *
 *  intPriority is the UART peripheral's interrupt priority, as defined by the
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
 *
 *  unsigned char uartMSP432RingBuffer[2][32];
 *
 *  const UARTMSP432_HWAttrsV1 uartMSP432HWAttrs[MSP_EXP432P401R_UARTCOUNT] = {
 *      {
 *          .baseAddr = EUSCI_A0_BASE,
 *          .intNum = INT_EUSCIA0,
 *          .intPriority = (~0),
 *          .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
 *          .bitOrder = EUSCI_A_UART_LSB_FIRST,
 *          .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
 *                                sizeof(UARTMSP432_BaudrateConfig),
 *          .baudrateLUT = uartMSP432Baudrates,
 *          .ringBufPtr  = uartMSP432RingBuffer[0],
 *          .ringBufSize = sizeof(uartMSP432RingBuffer[0]),
 *          .errorFxn = NULL
 *      },
 *      {
 *          .baseAddr = EUSCI_A2_BASE,
 *          .intNum = INT_EUSCIA2,
 *          .intPriority = (~0),
 *          .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
 *          .bitOrder = EUSCI_A_UART_LSB_FIRST,
 *          .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
 *                                sizeof(UARTMSP432_BaudrateConfig),
 *          .baudrateLUT = uartMSP432Baudrates
 *          .ringBufPtr  = uartMSP432RingBuffer[1],
 *          .ringBufSize = sizeof(uartMSP432RingBuffer[1]),
 *          .errorFxn = NULL
 *      }
 *  };
 *  @endcode
 */
typedef struct UARTMSP432_HWAttrsV1 {
    /*! UART Peripheral's base address */
    unsigned int    baseAddr;
    /*! UART Peripheral's interrupt vector */
    unsigned int    intNum;
    /*! UART Peripheral's interrupt priority */
    unsigned int    intPriority;
    /*! UART Clock source */
    uint8_t         clockSource;
    /*! UART Bit order */
    uint32_t        bitOrder;
    /*! Number of UARTMSP432_BaudrateConfig entries */
    uint8_t         numBaudrateEntries;
    /*! Pointer to a table of possible UARTMSP432_BaudrateConfig entries */
    UARTMSP432_BaudrateConfig const *baudrateLUT;
    /*! Pointer to a application buf queue */
    kbuf_queue_t   *bufQueuePtr;
    /*! Size of ringBufPtr */
    size_t          bufQueueSize;

    uint16_t        rxPin;
    uint16_t        txPin;
    /*! Application error function to be called on receive errors */
    UARTMSP432_ErrorCallback errorFxn;
} UARTMSP432_HWAttrsV1;

/*!
 *  @brief      UARTMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct UARTMSP432_Object {
    /* UART state variable */
    struct {
        bool             opened:1;         /* Has the obj been opened */
        UART_Mode        readMode:1;       /* Mode for all read calls */
        UART_Mode        writeMode:1;      /* Mode for all write calls */
        UART_DataMode    readDataMode:1;   /* Type of data being read */
        UART_DataMode    writeDataMode:1;  /* Type of data being written */
        UART_ReturnMode  readReturnMode:1; /* Receive return mode */
        UART_Echo        readEcho:1;       /* Echo received data back */
        bool             writeCR:1;        /* Write a return character */
        /*
         * Flag to determine if a timeout has occurred when the user called
         * UART_read(). This flag is set by the timeoutClk clock object.
         */
        bool             bufTimeout:1;
        /*
         * Flag to determine when an ISR needs to perform a callback; in both
         * UART_MODE_BLOCKING or UART_MODE_CALLBACK
         */
        bool             callCallback:1;
        /*
         * Flag to determine if the ISR is in control draining the ring buffer
         * when in UART_MODE_CALLBACK
         */
        bool             drainByISR:1;
        /* Flag to keep the state of the read Power constraints */
        bool             rxEnabled:1;
        /* Flag to keep the state of the write Power constraints */
        bool             txEnabled:1;

        /* Flags to prevent recursion in read callback mode */
        bool             inReadCallback:1;
        volatile bool    readCallbackPending:1;
    } state;

    HwiP_Handle          hwiHandle;        /* Hwi handle for interrupts */
    ClockP_Handle        timeoutClk;       /* Clock object to for timeouts */
    uint32_t             baudRate;         /* Baud rate for UART */
    UART_STOP            stopBits;         /* Stop bits for UART */
    UART_PAR             parityType;       /* Parity bit type for UART */
    UART_LEN             dataLength;       /* 7 or 8-bit data supported */

    /* UART read variables */
    kbuf_queue_t         *bufqueue;       /* local circular buffer object */
    /* A complement pair of read functions for both the ISR and UART_read() */
    UARTMSP432_FxnSet    readFxns;
    unsigned char       *readBuf;          /* Buffer data pointer */
    size_t               readSize;         /* Desired number of bytes to read */
    size_t               readCount;        /* Number of bytes left to read */
    SemaphoreP_Handle    readSem;          /* UART read semaphore */
    unsigned int         readTimeout;      /* Timeout for read semaphore */
    UART_Callback        readCallback;     /* Pointer to read callback */

    /* UART write variables */
    const unsigned char *writeBuf;         /* Buffer data pointer */
    size_t               writeSize;        /* Desired number of bytes to write*/
    size_t               writeCount;       /* Number of bytes left to write */
    SemaphoreP_Handle    writeSem;         /* UART write semaphore*/
    unsigned int         writeTimeout;     /* Timeout for write semaphore */
    UART_Callback        writeCallback;    /* Pointer to write callback */
    unsigned int         writeEmptyClkTimeout; /* TX FIFO timeout tick count */

    Power_NotifyObj      perfChangeNotify;
    uint32_t             perfConstraintMask;
} UARTMSP432_Object, *UARTMSP432_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_uart_UARTMSP432__include */
