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
 * Name:    EEPROM_24LC128.c
 * Purpose: EEPROM 24LC128 interface
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "Driver_I2C.h"
#include "Board_EEPROM.h"

#ifndef EEPROM_I2C_PORT
#define EEPROM_I2C_PORT       0         /* I2C Port number                    */
#endif

#define EEPROM_I2C_ADDR       0x51      /* 24LC128 EEPROM I2C address         */

/* I2C Driver */
#define _I2C_Driver_(n)       Driver_I2C##n
#define  I2C_Driver_(n)      _I2C_Driver_(n)
extern ARM_DRIVER_I2C         I2C_Driver_(EEPROM_I2C_PORT);
#define ptrI2C              (&I2C_Driver_(EEPROM_I2C_PORT))

#define EEPROM_MAX_ADDR       16384     /* Max memory locations available     */
#define EEPROM_MAX_WRITE      64        /* Max bytes to write in one step     */

#define A_WR                  0         /* Master will write to the I2C       */
#define A_RD                  1         /* Master will read from the I2C      */

static uint8_t DeviceAddr;
static uint8_t wr_buf[EEPROM_MAX_WRITE + 2];


static int32_t EEPROM_WriteBuf (uint16_t addr, const uint8_t *buf, uint32_t len) {

  wr_buf[0] = (uint8_t)(addr >> 8);
  wr_buf[1] = (uint8_t)(addr & 0xFF);

  memcpy (&wr_buf[2], &buf[0], len);

  ptrI2C->MasterTransmit (DeviceAddr, wr_buf, len + 2, false);
  while (ptrI2C->GetStatus().busy);
  if (ptrI2C->GetDataCount () != (len + 2)) return -1;

  /* Acknowledge polling */
  do {
    ptrI2C->MasterReceive (DeviceAddr, &wr_buf[0], 1, false);
    while (ptrI2C->GetStatus().busy);
  } while (ptrI2C->GetDataCount () < 0);

  return 0;
}

static int32_t EEPROM_ReadBuf (uint16_t addr, uint8_t *buf, uint32_t len) {
  uint8_t a[2];

  a[0] = (uint8_t)(addr >> 8);
  a[1] = (uint8_t)(addr & 0xFF);

  ptrI2C->MasterTransmit (DeviceAddr, a, 2, true);
  while (ptrI2C->GetStatus().busy);
  ptrI2C->MasterReceive (DeviceAddr, buf, len, false);
  while (ptrI2C->GetStatus().busy);
  if (ptrI2C->GetDataCount () != len) return -1;

  return 0;
}


/**
  \fn          int32_t EEPROM_Initialize (void)
  \brief       Initialize EEPROM
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t EEPROM_Initialize (void) {
  uint8_t val;

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  ptrI2C->Control      (ARM_I2C_BUS_CLEAR, 0);

  /* Init 24LC128 EEPROM device */
  DeviceAddr = EEPROM_I2C_ADDR;

  /* Read min and max address */
  if (EEPROM_ReadBuf (0x00, &val, 1) == 0) {
    return (EEPROM_ReadBuf (EEPROM_MAX_ADDR-1, &val, 1));
  }
  return -1;
}

/**
  \fn          int32_t EEPROM_Uninitialize (void)
  \brief       De-initialize EEPROM
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t EEPROM_Uninitialize (void) {
  return 0;
}

/**
  \fn          uint32_t EEPROM_GetSize (void)
  \brief       Get EEPROM memory size in bytes
  \returns     Memory size in bytes
*/
uint32_t EEPROM_GetSize (void) {
  return EEPROM_MAX_ADDR;
}

/**
  \fn          int32_t EEPROM_ReadData (uint32_t addr, uint8_t *buf, uint32_t len)
  \brief       Read data from EEPROM
  \param[in]   addr   EEPROM address
  \param[in]   buf    Pointer where data will be read from EEPROM
  \param[in]   len    Number of data bytes to read
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t EEPROM_ReadData (uint32_t addr, uint8_t *buf, uint32_t len) {
  if (addr < EEPROM_MAX_ADDR) {
    return (EEPROM_ReadBuf ((uint16_t)addr, buf, len));
  }
  return -1;
}

/**
  \fn          int32_t EEPROM_WriteData (uint32_t addr, const uint8_t *buf, uint32_t len)
  \brief       Write data to EEPROM
  \param[in]   addr   EEPROM address
  \param[in]   buf    Pointer with data to write to EEPROM
  \param[in]   len    Number of data bytes to write
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t EEPROM_WriteData (uint32_t addr, const uint8_t *buf, uint32_t len) {
  uint32_t len_cur;

  if (addr < EEPROM_MAX_ADDR) {
    while (len) {
      len_cur = ((len > EEPROM_MAX_WRITE) ? EEPROM_MAX_WRITE : len);
      if (EEPROM_WriteBuf ((uint16_t)addr, buf, len_cur) == -1) return -1;
      len  -= len_cur;
      addr += len_cur;
    }
    return 0;
  }
  return -1;
}
