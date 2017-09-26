/**
 ******************************************************************************
 * @file    hts221.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   This file provides a set of functions needed to manage the HTS221
 *          humidity and temperature devices
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hts221.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup HTS221 HTS221
  * @{
  */

/** @defgroup HTS221_Private_Variables HTS221 Private Variables
  * @{
  */
/* HTS221 Humidity Private Variables */
HSENSOR_DrvTypeDef HTS221_H_Drv =
{
  HTS221_H_Init,
  HTS221_H_ReadID,
  HTS221_H_ReadHumidity
};

/* HTS221_Temperature_Private_Variables */ 
TSENSOR_DrvTypeDef HTS221_T_Drv =
{
  HTS221_T_Init,
  0,
  0,
  HTS221_T_ReadTemp
};
/**
  * @}
  */ 

/** @defgroup HTS221_Humidity_Private_Functions HTS221 Humidity Private Functions
  * @{
  */
/**
  * @brief  Set HTS221 humidity sensor Initialization.
  */
void HTS221_H_Init(uint16_t DeviceAddr)
{
  uint8_t tmp;
  
  /* Read CTRL_REG1 */
  tmp = SENSOR_IO_Read(DeviceAddr, HTS221_CTRL_REG1);
  
  /* Enable BDU */
  tmp &= ~HTS221_BDU_MASK;
  tmp |= (1 << HTS221_BDU_BIT);
  
  /* Set default ODR */
  tmp &= ~HTS221_ODR_MASK;
  tmp |= (uint8_t)0x01; /* Set ODR to 1Hz */
  
  /* Activate the device */
  tmp |= HTS221_PD_MASK;
  
  /* Apply settings to CTRL_REG1 */
  SENSOR_IO_Write(DeviceAddr, HTS221_CTRL_REG1, tmp);
}

/**
  * @brief  Read HTS221 ID.
  * @retval ID 
  */
uint8_t HTS221_H_ReadID(uint16_t DeviceAddr)
{  
  uint8_t ctrl = 0x00;
 
  /* IO interface initialization */
  SENSOR_IO_Init(); 
  
  /* Read value at Who am I register address */
  ctrl = SENSOR_IO_Read(DeviceAddr, HTS221_WHO_AM_I_REG);
  
  return ctrl;
}

/**
  * @brief  Read humidity value of HTS221
  * @retval humidity value;
  */
float HTS221_H_ReadHumidity(uint16_t DeviceAddr)
{
  int16_t H0_T0_out, H1_T0_out, H_T_out;
  int16_t H0_rh, H1_rh;
  uint8_t buffer[2];
  float tmp_f;

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_H0_RH_X2 | 0x80), buffer, 2);

  H0_rh = buffer[0] >> 1;
  H1_rh = buffer[1] >> 1;

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_H0_T0_OUT_L | 0x80), buffer, 2);

  H0_T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_H1_T0_OUT_L | 0x80), buffer, 2);

  H1_T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_HR_OUT_L_REG | 0x80), buffer, 2);

  H_T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

  tmp_f = (float)(H_T_out - H0_T0_out) * (float)(H1_rh - H0_rh) / (float)(H1_T0_out - H0_T0_out)  +  H0_rh;
  tmp_f *= 10.0f;

  tmp_f = ( tmp_f > 1000.0f ) ? 1000.0f
        : ( tmp_f <    0.0f ) ?    0.0f
        : tmp_f;

  return (tmp_f / 10.0f);
}


/**
  * @}
  */ 

/** @defgroup HTS221_Temperature_Private_Functions HTS221 Temperature Private Functions
  * @{
  */

/**
  * @brief  Set HTS221 temperature sensor Initialization.
  * @param  DeviceAddr: I2C device address
  * @param  InitStruct: pointer to a TSENSOR_InitTypeDef structure 
  *         that contains the configuration setting for the HTS221.
  */
void HTS221_T_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct)
{  
  uint8_t tmp;
  
  /* Read CTRL_REG1 */
  tmp = SENSOR_IO_Read(DeviceAddr, HTS221_CTRL_REG1);
  
  /* Enable BDU */
  tmp &= ~HTS221_BDU_MASK;
  tmp |= (1 << HTS221_BDU_BIT);
  
  /* Set default ODR */
  tmp &= ~HTS221_ODR_MASK;
  tmp |= (uint8_t)0x01; /* Set ODR to 1Hz */
  
  /* Activate the device */
  tmp |= HTS221_PD_MASK;
  
  /* Apply settings to CTRL_REG1 */
  SENSOR_IO_Write(DeviceAddr, HTS221_CTRL_REG1, tmp);
}

/**
  * @brief  Read temperature value of HTS221
  * @param  DeviceAddr: I2C device address
  * @retval temperature value
  */
float HTS221_T_ReadTemp(uint16_t DeviceAddr)
{
  int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
  int16_t T0_degC, T1_degC;
  uint8_t buffer[4], tmp;
  float tmp_f;

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_T0_DEGC_X8 | 0x80), buffer, 2);
  tmp = SENSOR_IO_Read(DeviceAddr, HTS221_T0_T1_DEGC_H2);

  T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer[0]);
  T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer[1]);
  T0_degC = T0_degC_x8_u16 >> 3;
  T1_degC = T1_degC_x8_u16 >> 3;

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_T0_OUT_L | 0x80), buffer, 4);

  T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
  T1_out = (((uint16_t)buffer[3]) << 8) | (uint16_t)buffer[2];

  SENSOR_IO_ReadMultiple(DeviceAddr, (HTS221_TEMP_OUT_L_REG | 0x80), buffer, 2);

  T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

  tmp_f = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out - T0_out)  +  T0_degC;

  return tmp_f;
}

/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */
  
/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
