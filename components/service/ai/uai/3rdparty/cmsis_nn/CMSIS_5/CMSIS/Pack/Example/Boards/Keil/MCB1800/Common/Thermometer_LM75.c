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
 * Name:    Thermometer_LM75.c
 * Purpose: Thermometer LM75 interface
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include "Driver_I2C.h"
#include "Board_Thermometer.h"

#ifndef TH_I2C_PORT
#define TH_I2C_PORT      0              /* I2C Port number                    */
#endif

#define TH_I2C_ADDR     0x48            /* LM 75 Thermometer I2C address      */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(TH_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(TH_I2C_PORT))

#define MODE_COMP   0                   /* Comparator mode                    */
#define MODE_INT    1                   /* Interrupt mode                     */

/* Register addresses */
#define REG_TEMP        0x00
#define REG_CONF        0x01
#define REG_THYS        0x02
#define REG_TOS         0x03

static uint8_t DeviceAddr;

/**
  \fn          int32_t TH_Read (uint8_t reg, uint8_t *val, uint32_t size)
  \brief       Read register value from Thermometer
  \param[in]   reg    Register address
  \param[in]   val    Pointer where value will be read from register
  \param[in]   size   Number of data bytes to read
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t TH_Read (uint8_t reg, uint8_t *val, uint32_t size) {
  uint8_t data[1];

  data[0] = reg;
  ptrI2C->MasterTransmit (DeviceAddr, data, 1, true);
  while (ptrI2C->GetStatus().busy);
  ptrI2C->MasterReceive (DeviceAddr, val, size, false);
  while (ptrI2C->GetStatus().busy);

  return 0;
}

/**
  \fn          int32_t Thermometer_Initialize (void)
  \brief       Initialize thermometer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Thermometer_Initialize (void) {
  uint8_t val[3];

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  ptrI2C->Control      (ARM_I2C_BUS_CLEAR, 0);

  /* Init L3G4200D device */
  DeviceAddr = TH_I2C_ADDR;
  
  /* Set default configuration */
  val[1] = 0;
  val[0] = REG_CONF;
  ptrI2C->MasterTransmit (DeviceAddr, val, 2, false);
  while (ptrI2C->GetStatus().busy);

  /* Set Hysteresis */
  val[2] = 75;
  val[1] =  0;
  val[0] =  REG_THYS;
  ptrI2C->MasterTransmit (DeviceAddr, val, 3, false);
  while (ptrI2C->GetStatus().busy);

  /* Set Tos */
  val[2] = 80;
  val[1] =  0;
  val[0] =  REG_TOS;
  ptrI2C->MasterTransmit (DeviceAddr, val, 3, false);
  while (ptrI2C->GetStatus().busy);

  return 0;
}

/**
  \fn          int32_t Thermometer_Uninitialize (void)
  \brief       De-initialize thermometer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Thermometer_Uninitialize (void) {
  return 0;
}

/**
  \fn          int32_t Thermometer_GetTemperature (void)
  \brief       Get last temperature measurement
  \returns     Temperature in 0.1C, or TEMPERATURE_INVALID in case of error
*/
int32_t Thermometer_GetTemperature (void) {
  uint8_t val[2];

  if (TH_Read(REG_TEMP, val, 2) == 0) {
    return (((val[0] << 1) | (val[1]) >> 7) * 5);
  }
  return TEMPERATURE_INVALID;
}
