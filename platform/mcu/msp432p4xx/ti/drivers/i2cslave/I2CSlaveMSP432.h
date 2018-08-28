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
 *  @file       I2CSlaveMSP432.h
 *
 *  @brief      I2CSlave driver implementation for the EUSCI controller on
 *  MSP432
 *
 *  This I2CSlave driver implementation is designed to operate on a EUCSI
 *  controller in I2CSlave mode.  The I2CSlaveMSP432 header file should be
 *  included in an application as follows:
 *  @code
 *  #include <ti/drivers/I2CSlave.h>
 *  #include <ti/drivers/I2CSlaveMSP432.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef ti_drivers_i2cslave_I2CSlaveMSP432__include
#define ti_drivers_i2cslave_I2CSlaveMSP432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/I2CSlave.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>

/*
 *  I2CSlave port/pin defines for pin configuration.  Ports P2, P3, and P7 are
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

/* Port 1 EUSCI B I2CSLAVE defines */
#define I2CSLAVEMSP432_P1_6_UCB0SDA   0x00000116  /* Primary, port 1, pin 6 */
#define I2CSLAVEMSP432_P1_7_UCB0SCL   0x00000117  /* Primary, port 1, pin 7 */

/* Port 2, pin 0 defines */
#define I2CSLAVEMSP432_P2_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x20)
#define I2CSLAVEMSP432_P2_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x20)
#define I2CSLAVEMSP432_P2_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x20)
#define I2CSLAVEMSP432_P2_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x20)

/* Port 2, pin 1 defines */
#define I2CSLAVEMSP432_P2_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x21)
#define I2CSLAVEMSP432_P2_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x21)
#define I2CSLAVEMSP432_P2_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x21)
#define I2CSLAVEMSP432_P2_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x21)

/* Port 2, pin 2 defines */
#define I2CSLAVEMSP432_P2_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x22)
#define I2CSLAVEMSP432_P2_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x22)
#define I2CSLAVEMSP432_P2_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x22)
#define I2CSLAVEMSP432_P2_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x22)

/* Port 2, pin 3 defines */
#define I2CSLAVEMSP432_P2_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x23)
#define I2CSLAVEMSP432_P2_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x23)
#define I2CSLAVEMSP432_P2_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x23)
#define I2CSLAVEMSP432_P2_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x23)

/* Port 2, pin 4 defines */
#define I2CSLAVEMSP432_P2_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x24)
#define I2CSLAVEMSP432_P2_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x24)
#define I2CSLAVEMSP432_P2_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x24)
#define I2CSLAVEMSP432_P2_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x24)

/* Port 2, pin 5 defines */
#define I2CSLAVEMSP432_P2_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x25)
#define I2CSLAVEMSP432_P2_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x25)
#define I2CSLAVEMSP432_P2_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x25)
#define I2CSLAVEMSP432_P2_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x25)

/* Port 2, pin 6 defines */
#define I2CSLAVEMSP432_P2_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x26)
#define I2CSLAVEMSP432_P2_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x26)
#define I2CSLAVEMSP432_P2_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x26)
#define I2CSLAVEMSP432_P2_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x26)

/* Port 2, pin 7 defines */
#define I2CSLAVEMSP432_P2_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x27)
#define I2CSLAVEMSP432_P2_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x27)
#define I2CSLAVEMSP432_P2_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x27)
#define I2CSLAVEMSP432_P2_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x27)

/* Port 3, pin 0 defines */
#define I2CSLAVEMSP432_P3_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x30)
#define I2CSLAVEMSP432_P3_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x30)
#define I2CSLAVEMSP432_P3_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x30)
#define I2CSLAVEMSP432_P3_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x30)

/* Port 3, pin 1 defines */
#define I2CSLAVEMSP432_P3_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x31)
#define I2CSLAVEMSP432_P3_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x31)
#define I2CSLAVEMSP432_P3_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x31)
#define I2CSLAVEMSP432_P3_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x31)

/* Port 3, pin 2 defines */
#define I2CSLAVEMSP432_P3_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x32)
#define I2CSLAVEMSP432_P3_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x32)
#define I2CSLAVEMSP432_P3_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x32)
#define I2CSLAVEMSP432_P3_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x32)

/* Port 3, pin 3 defines */
#define I2CSLAVEMSP432_P3_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x33)
#define I2CSLAVEMSP432_P3_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x33)
#define I2CSLAVEMSP432_P3_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x33)
#define I2CSLAVEMSP432_P3_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x33)

/* Port 3, pin 4 defines */
#define I2CSLAVEMSP432_P3_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x34)
#define I2CSLAVEMSP432_P3_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x34)
#define I2CSLAVEMSP432_P3_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x34)
#define I2CSLAVEMSP432_P3_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x34)

/* Port 3, pin 5 defines */
#define I2CSLAVEMSP432_P3_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x35)
#define I2CSLAVEMSP432_P3_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x35)
#define I2CSLAVEMSP432_P3_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x35)
#define I2CSLAVEMSP432_P3_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x35)

/* Port 3, pin 6 defines */
#define I2CSLAVEMSP432_P3_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x36)
#define I2CSLAVEMSP432_P3_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x36)
#define I2CSLAVEMSP432_P3_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x36)
#define I2CSLAVEMSP432_P3_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x36)

/* Port 3, pin 7 defines */
#define I2CSLAVEMSP432_P3_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x37)
#define I2CSLAVEMSP432_P3_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x37)
#define I2CSLAVEMSP432_P3_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x37)
#define I2CSLAVEMSP432_P3_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x37)

/* Port 6 EUSCI B I2CSLAVE defines */
#define I2CSLAVEMSP432_P6_4_UCB1SDA   0x00000164  /* Primary, port 6, pin 4 */
#define I2CSLAVEMSP432_P6_5_UCB1SCL   0x00000165  /* Primary, port 6, pin 5 */
#define I2CSLAVEMSP432_P6_6_UCB3SDA   0x00000266  /* Secondary, port 6, pin 6 */
#define I2CSLAVEMSP432_P6_7_UCB3SCL   0x00000267  /* Secondary, port 6, pin 7 */

/* Port 7, pin 0 defines */
#define I2CSLAVEMSP432_P7_0_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x70)
#define I2CSLAVEMSP432_P7_0_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x70)
#define I2CSLAVEMSP432_P7_0_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x70)
#define I2CSLAVEMSP432_P7_0_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x70)

/* Port 7, pin 1 defines */
#define I2CSLAVEMSP432_P7_1_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x71)
#define I2CSLAVEMSP432_P7_1_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x71)
#define I2CSLAVEMSP432_P7_1_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x71)
#define I2CSLAVEMSP432_P7_1_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x71)

/* Port 7, pin 2 defines */
#define I2CSLAVEMSP432_P7_2_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x72)
#define I2CSLAVEMSP432_P7_2_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x72)
#define I2CSLAVEMSP432_P7_2_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x72)
#define I2CSLAVEMSP432_P7_2_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x72)

/* Port 7, pin 3 defines */
#define I2CSLAVEMSP432_P7_3_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x73)
#define I2CSLAVEMSP432_P7_3_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x73)
#define I2CSLAVEMSP432_P7_3_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x73)
#define I2CSLAVEMSP432_P7_3_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x73)

/* Port 7, pin 4 defines */
#define I2CSLAVEMSP432_P7_4_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x74)
#define I2CSLAVEMSP432_P7_4_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x74)
#define I2CSLAVEMSP432_P7_4_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x74)
#define I2CSLAVEMSP432_P7_4_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x74)

/* Port 7, pin 5 defines */
#define I2CSLAVEMSP432_P7_5_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x75)
#define I2CSLAVEMSP432_P7_5_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x75)
#define I2CSLAVEMSP432_P7_5_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x75)
#define I2CSLAVEMSP432_P7_5_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x75)

/* Port 7, pin 6 defines */
#define I2CSLAVEMSP432_P7_6_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x76)
#define I2CSLAVEMSP432_P7_6_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x76)
#define I2CSLAVEMSP432_P7_6_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x76)
#define I2CSLAVEMSP432_P7_6_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x76)

/* Port 7, pin 7 defines */
#define I2CSLAVEMSP432_P7_7_UCB0SDA   ((PMAP_UCB0SDA << 10) | 0x77)
#define I2CSLAVEMSP432_P7_7_UCB0SCL   ((PMAP_UCB0SCL << 10) | 0x77)
#define I2CSLAVEMSP432_P7_7_UCB2SDA   ((PMAP_UCB2SDA << 10) | 0x77)
#define I2CSLAVEMSP432_P7_7_UCB2SCL   ((PMAP_UCB2SCL << 10) | 0x77)

/* Port 10 EUSCI B I2CSLAVE defines */
#define I2CSLAVEMSP432_P10_2_UCB3SDA   0x000001A2  /* Primary, port 10, pin 2 */
#define I2CSLAVEMSP432_P10_3_UCB3SCL   0x000001A3  /* Primary, port 10, pin 3 */

/**
 *  @addtogroup I2CSLAVE_STATUS
 *  I2CSLAVEMSP432_STATUS_* macros are command codes only defined in the
 *  I2CSlaveMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2cslave/I2CSlaveMSP432.h>
 *  @endcode
 *  @{
 */

/* Add I2CSLAVEMSP432_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup I2CSLAVE_CMD
 *  I2CSLAVEMSP432_CMD_* macros are command codes only defined in the
 *  I2CSlaveMSP432.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2cslave/I2CSlaveMSP432.h>
 *  @endcode
 *  @{
 */

/* Add I2CSLAVEMSP432_CMD_* macros here */

/** @}*/

/* I2CSlave function table pointer */
extern const I2CSlave_FxnTable I2CSlaveMSP432_fxnTable;

/*!
 *  @brief  I2CSlaveMSP432 Hardware attributes
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSP432 driverlib these definitions are
 *  found in:
 *      - i2c.h
 *
 *  A sample structure is shown below:
 *  @code
 *  const I2CSlaveMSP432_HWAttrs i2cslaveMSP432HWAttrs[] = {
 *      {
 *          .baseAddr = EUSCI_B0_BASE,
 *          .intNum = INT_EUSCIB0,
 *          .intPriority = ~0,
 *          .slaveAddress = 0x48
 *      }
 *  };
 *  @endcode
 */

 typedef struct I2CSlaveMSP432_HWAttrs {
    uint32_t baseAddr;       /*!< EUSCI_B_I2C Peripheral's base address */
    uint32_t intNum;         /*!< EUSCI_B_I2C Peripheral's interrupt vector */
    uint32_t intPriority;    /*!< EUSCI_B_I2C Peripheral's interrupt priority */
    uint32_t slaveAddress;   /*!< EUSCI_B_I2C Peripheral's slave address */
    uint16_t clkPin;         /*!< EUSCI_B_I2C clock pin configuration */
    uint16_t dataPin;        /*!< EUSCI_B_I2C data pin configuration */

} I2CSlaveMSP432_HWAttrs;

/*!
 *  @brief  I2CSlaveMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct I2CSlaveMSP432_Object {
    SemaphoreP_Handle       mutex;               /* Grants exclusive access */
     /* Notify finished I2C transfer */
    SemaphoreP_Handle       transferComplete;

    HwiP_Handle             hwiHandle;

    I2CSlave_CallbackFxn    transferCallbackFxn; /* Callback function pointer */
    /* Internal inc. writeBuf index */
    const uint8_t           *writeBufferIdx;
    /* Internal inc. readBuf index */
    uint8_t                 *readBufferIdx;
    size_t                  countIdx;            /* Internal dec. writeCounter */

    volatile I2CSlave_Mode  mode;                /* Stores the I2CSlave state */
    I2CSlave_TransferMode   transferMode;        /* Blocking or Callback mode */

    /* To determine if the I2CSlave is open */
    bool                    isOpen;
    bool                    transferInProgress;
} I2CSlaveMSP432_Object;


I2CSlave_Handle I2CSlaveMSP432_open(I2CSlave_Handle handle,
                                    I2CSlave_Params *params);
bool I2CSlaveMSP432_write(I2CSlave_Handle handle, const void *buffer,
                          size_t size);
bool I2CSlaveMSP432_read(I2CSlave_Handle handle,void *buffer,
                         size_t size);
void I2CSlaveMSP432_close(I2CSlave_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2cslave_I2CMSP432__include */
