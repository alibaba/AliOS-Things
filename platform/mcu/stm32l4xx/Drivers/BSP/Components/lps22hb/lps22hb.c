/**
 ******************************************************************************
 * @file    lps22hb.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   This file provides a set of functions needed to manage the LPS22HB
 *          pressure and temperature devices
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
#include "lps22hb.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup LPS22HB LPS22HB
  * @{
  */

/** @defgroup LPS22HB_Private_FunctionsPrototypes LPS22HB Private Functions Prototypes
  * @{
  */
static void LPS22HB_Init(uint16_t DeviceAddr);
/**
  * @}
  */

/** @defgroup LPS22HB_Private_Variables LPS22HB Private Variables
  * @{
  */
/* Pressure Private Variables */
PSENSOR_DrvTypeDef LPS22HB_P_Drv =
{
  LPS22HB_P_Init,
  LPS22HB_P_ReadID,
  LPS22HB_P_ReadPressure
};

/* Temperature Private Variables */ 
TSENSOR_DrvTypeDef LPS22HB_T_Drv =
{
  LPS22HB_T_Init,
  0,
  0,
  LPS22HB_T_ReadTemp
};
/**
  * @}
  */ 

/** @defgroup LPS22HB_Pressure_Private_Functions LPS22HB Pressure Private Functions
  * @{
  */
/**
  * @brief  Set LPS22HB pressure sensor Initialization.
  */
void LPS22HB_P_Init(uint16_t DeviceAddr)
{
  LPS22HB_Init(DeviceAddr);
}

/**
  * @brief  Read LPS22HB ID.
  * @retval ID 
  */
uint8_t LPS22HB_P_ReadID(uint16_t DeviceAddr)
{  
  uint8_t ctrl = 0x00;

  /* IO interface initialization */
  SENSOR_IO_Init();  
  
  /* Read value at Who am I register address */
  ctrl = SENSOR_IO_Read(DeviceAddr, LPS22HB_WHO_AM_I_REG);
  
  return ctrl;
}

/**
  * @brief  Read pressure value of LPS22HB
  * @retval pressure value
  */
float LPS22HB_P_ReadPressure(uint16_t DeviceAddr)
{
  int32_t raw_press;
  uint8_t buffer[3];
  uint32_t tmp = 0;
  uint8_t i;

  for(i = 0; i < 3; i++)
  {
    buffer[i] = SENSOR_IO_Read(DeviceAddr, (LPS22HB_PRESS_OUT_XL_REG + i));
  }

  /* Build the raw data */
  for(i = 0; i < 3; i++)
    tmp |= (((uint32_t)buffer[i]) << (8 * i));

  /* convert the 2's complement 24 bit to 2's complement 32 bit */
  if(tmp & 0x00800000)
    tmp |= 0xFF000000;

  raw_press = ((int32_t)tmp);

  raw_press = (raw_press * 100) / 4096;

  return (float)((float)raw_press / 100.0f);
}


/**
  * @}
  */ 

/** @defgroup LPS22HB_Temperature_Private_Functions LPS22HB Temperature Private Functions
  * @{
  */

/**
  * @brief  Set LPS22HB temperature sensor Initialization.
  * @param  DeviceAddr: I2C device address
  * @param  InitStruct: pointer to a TSENSOR_InitTypeDef structure 
  *         that contains the configuration setting for the HTS221.
  * @retval None
  */
void LPS22HB_T_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct)
{  
  LPS22HB_Init(DeviceAddr);
}

/**
  * @brief  Read temperature value of LPS22HB
  * @param  DeviceAddr: I2C device address
  * @retval temperature value
  */
float LPS22HB_T_ReadTemp(uint16_t DeviceAddr)
{
  int16_t raw_data;
  uint8_t buffer[2];
  uint16_t tmp;
  uint8_t i;

  for(i = 0; i < 2; i++)
  {
    buffer[i] = SENSOR_IO_Read(DeviceAddr, (LPS22HB_TEMP_OUT_L_REG + i));
  }

  /* Build the raw tmp */
  tmp = (((uint16_t)buffer[1]) << 8) + (uint16_t)buffer[0];

  raw_data = (tmp * 10) / 100;
  
  return ((float)(raw_data / 10.0f));
}

/**
  * @}
  */

/** @addtogroup LPS22HB_Private_Functions LPS22HB Private functions
 * @{
 */

/**
  * @brief  Set LPS22HB Initialization.
  * @param  DeviceAddr: I2C device address
  * @retval None
  */
static void LPS22HB_Init(uint16_t DeviceAddr)
{
  uint8_t tmp;

  /* Set Power mode */
  tmp = SENSOR_IO_Read(DeviceAddr, LPS22HB_RES_CONF_REG);

  tmp &= ~LPS22HB_LCEN_MASK;
  tmp |= (uint8_t)0x01; /* Set low current mode */

  SENSOR_IO_Write(DeviceAddr, LPS22HB_RES_CONF_REG, tmp);

  /* Read CTRL_REG1 */
  tmp = SENSOR_IO_Read(DeviceAddr, LPS22HB_CTRL_REG1);

  /* Set default ODR */
  tmp &= ~LPS22HB_ODR_MASK;
  tmp |= (uint8_t)0x30; /* Set ODR to 25Hz */

  /* Enable BDU */
  tmp &= ~LPS22HB_BDU_MASK;
  tmp |= ((uint8_t)0x02);

  /* Apply settings to CTRL_REG1 */
  SENSOR_IO_Write(DeviceAddr, LPS22HB_CTRL_REG1, tmp);
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
