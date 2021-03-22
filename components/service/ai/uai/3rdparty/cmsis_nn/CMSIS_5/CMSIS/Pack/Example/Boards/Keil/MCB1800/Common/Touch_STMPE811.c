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
 * Name:    Touch_STMPE811.c
 * Purpose: Touchscreen STMPE811 interface
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "STMPE811.h"
#include "Driver_I2C.h"
#include "Board_Touch.h"

#ifndef TSC_I2C_PORT
#define TSC_I2C_PORT    0               /* I2C Port number                    */
#endif

#define TSC_I2C_ADDR    0x41            /* 7-bit I2C address                  */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(TSC_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(TSC_I2C_PORT))


/**
  \fn          int32_t Touch_ReadRegister (uint32_t reg, uint8_t *val)
  \brief       Read register value from Touchscreen controller
  \param[in]   reg    Register address
  \param[out]  val    Pointer where data will be read from register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static bool Touch_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];

  data[0] = reg;
  ptrI2C->MasterTransmit (TSC_I2C_ADDR, data, 1, true);
  while (ptrI2C->GetStatus().busy);
  ptrI2C->MasterReceive (TSC_I2C_ADDR, val, 1, false);
  while (ptrI2C->GetStatus().busy);

  return 0;
}

/**
  \fn          int32_t Touch_WriteData (uint32_t reg, const uint8_t *val)
  \brief       Write value to Touchscreen controller register
  \param[in]   reg    Register address
  \param[in]   val    Pointer with data to write to register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static bool Touch_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];

  data[0] = reg;
  data[1] = val;
  ptrI2C->MasterTransmit (TSC_I2C_ADDR, data, 2, false);
  while (ptrI2C->GetStatus().busy);

  return 0;
}

/**
  \fn          int32_t Touch_Initialize (void)
  \brief       Initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Initialize (void) {
  uint32_t tick;
  
  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  ptrI2C->Control      (ARM_I2C_BUS_CLEAR, 0);

  Touch_Write(STMPE811_SYS_CTRL1,      0x02); /* Reset Touch-screen controller */
  tick = osKernelSysTick();                   /* Wait 10ms                     */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

  Touch_Write(STMPE811_SYS_CTRL2,      0x0C); /* Enable TSC and ADC            */
  Touch_Write(STMPE811_INT_EN,         0x07); /* Enable Touch detect, FIFO     */
  Touch_Write(STMPE811_ADC_CTRL1,      0x68); /* Set sample time , 12-bit mode */
  tick = osKernelSysTick();                   /* Wait 500us                    */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(500));

  Touch_Write(STMPE811_ADC_CTRL2,      0x01); /* ADC frequency 3.25 MHz        */
  Touch_Write(STMPE811_TSC_CFG,        0xC2); /* Detect delay 10us,
                                                 Settle time 500us             */
  Touch_Write(STMPE811_FIFO_TH,        0x01); /* Threshold for FIFO            */
  Touch_Write(STMPE811_FIFO_STA,       0x01); /* FIFO reset                    */
  Touch_Write(STMPE811_FIFO_STA,       0x00); /* FIFO not reset                */
  Touch_Write(STMPE811_TSC_FRACTION_Z, 0x07); /* Fraction z                    */
  Touch_Write(STMPE811_TSC_I_DRIVE,    0x01); /* Drive 50 mA typical           */
  Touch_Write(STMPE811_GPIO_AF,        0x00); /* Pins are used for touchscreen */
  Touch_Write(STMPE811_TSC_CTRL,       0x01); /* Enable TSC                    */
  Touch_Write(STMPE811_INT_STA,        0xFF); /* Clear interrupt status        */

  return 0;
}

/**
  \fn          int32_t Touch_Uninitialize (void)
  \brief       De-initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Uninitialize (void) {
  Touch_Write(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller    */
  return 0;
}

/**
  \fn          int32_t Touch_GetState (TOUCH_STATE *pState)
  \brief       Get touchscreen state
  \param[out]  pState  pointer to TOUCH_STATE structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_GetState (TOUCH_STATE *pState) {
  uint8_t val;
  uint8_t num;
  uint8_t xyz[4];
  int32_t res;

  /* Read touch status */
  res = Touch_Read(STMPE811_TSC_CTRL, &val);
  if (res < 0) return -1;
  pState->pressed = (val & (1 << 7)) ? 1 : 0;
  
  if (pState->pressed) {
    val = STMPE811_TSC_DATA;

    /* If FIFO overflow, discard all samples except the last one */
    res = Touch_Read(STMPE811_FIFO_SIZE, &num);
    if (res < 0 || num == 0) return -1;

    while (num--) {
      ptrI2C->MasterTransmit (TSC_I2C_ADDR, &val, 1, true);
      while (ptrI2C->GetStatus().busy);
      ptrI2C->MasterReceive (TSC_I2C_ADDR, xyz, 4, false);
      while (ptrI2C->GetStatus().busy);
    }
    pState->x =  (xyz[0] << 4) | ((xyz[1] & 0xF0) >> 4);
    pState->y =   xyz[2]       | ((xyz[1] & 0x0F) << 8);
  }

  /* Clear interrupt flags */
  Touch_Write(STMPE811_INT_STA, 0x1F);

  return 0;
}
