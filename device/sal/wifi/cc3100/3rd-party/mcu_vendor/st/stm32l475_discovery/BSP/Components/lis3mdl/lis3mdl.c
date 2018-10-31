/**
 ******************************************************************************
 * @file    lis3mdl.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   This file provides a set of functions needed to manage the LIS3MDL
 *          magnetometer devices
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
#include "lis3mdl.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup LIS3MDL LIS3MDL
  * @{
  */

/** @defgroup LIS3MDL_Mag_Private_Variables LIS3MDL Mag Private Variables
  * @{
  */ 
MAGNETO_DrvTypeDef Lis3mdlMagDrv =
{
  LIS3MDL_MagInit,
  LIS3MDL_MagDeInit,
  LIS3MDL_MagReadID,
  0,
  LIS3MDL_MagLowPower,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  LIS3MDL_MagReadXYZ
};
/**
  * @}
  */ 


/** @defgroup LIS3MDL_Mag_Private_Functions LIS3MDL Mag Private Functions
  * @{
  */
/**
  * @brief  Set LIS3MDL Magnetometer Initialization.
  * @param  LIS3MDL_InitStruct: pointer to a LIS3MDL_MagInitTypeDef structure 
  *         that contains the configuration setting for the LIS3MDL.
  */
void LIS3MDL_MagInit(MAGNETO_InitTypeDef LIS3MDL_InitStruct)
{  
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG1, LIS3MDL_InitStruct.Register1);
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG2, LIS3MDL_InitStruct.Register2);
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3, LIS3MDL_InitStruct.Register3);
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG4, LIS3MDL_InitStruct.Register4);
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG5, LIS3MDL_InitStruct.Register5);
}

/**
  * @brief  LIS3MDL Magnetometer De-initialization.
  */
void LIS3MDL_MagDeInit(void)
{
  uint8_t ctrl = 0x00;
  
  /* Read control register 1 value */
  ctrl = SENSOR_IO_Read(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3);

  /* Clear Selection Mode bits */
  ctrl &= ~(LIS3MDL_MAG_SELECTION_MODE);

  /* Set Power down */
  ctrl |= LIS3MDL_MAG_POWERDOWN2_MODE;
  
  /* write back control register */
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3, ctrl);  
}

/**
  * @brief  Read LIS3MDL ID.
  * @retval ID 
  */
uint8_t LIS3MDL_MagReadID(void)
{
  /* IO interface initialization */
  SENSOR_IO_Init();  
  /* Read value at Who am I register address */
  return (SENSOR_IO_Read(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_WHO_AM_I_REG));
}

/**
  * @brief  Set/Unset Magnetometer in low power mode.
  * @param  status 0 means disable Low Power Mode, otherwise Low Power Mode is enabled
  */
void LIS3MDL_MagLowPower(uint16_t status)
{  
  uint8_t ctrl = 0;
  
  /* Read control register 1 value */
  ctrl = SENSOR_IO_Read(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3);

  /* Clear Low Power Mode bit */
  ctrl &= ~(0x20);

  /* Set Low Power Mode */
  if(status)
  {
    ctrl |= LIS3MDL_MAG_CONFIG_LOWPOWER_MODE;
  }else
  {
    ctrl |= LIS3MDL_MAG_CONFIG_NORMAL_MODE;
  }
  
  /* write back control register */
  SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3, ctrl);  
}

/**
  * @brief  Read X, Y & Z Magnetometer values 
  * @param  pData: Data out pointer
  */
void LIS3MDL_MagReadXYZ(int16_t* pData)
{
  int16_t pnRawData[3];
  uint8_t ctrlm= 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  float sensitivity = 0;
  
  /* Read the magnetometer control register content */
  ctrlm = SENSOR_IO_Read(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG2);
  
  /* Read output register X, Y & Z acceleration */
  SENSOR_IO_ReadMultiple(LIS3MDL_MAG_I2C_ADDRESS_HIGH, (LIS3MDL_MAG_OUTX_L | 0x80), buffer, 6);
  
  for(i=0; i<3; i++)
  {
    pnRawData[i]=((((uint16_t)buffer[2*i+1]) << 8) + (uint16_t)buffer[2*i]);
  }
  
  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL_REG2 */
  switch(ctrlm & 0x60)
  {
  case LIS3MDL_MAG_FS_4_GA:
    sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_4GA;
    break;
  case LIS3MDL_MAG_FS_8_GA:
    sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_8GA;
    break;
  case LIS3MDL_MAG_FS_12_GA:
    sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_12GA;
    break;
  case LIS3MDL_MAG_FS_16_GA:
    sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_16GA;
    break;    
  }
  
  /* Obtain the mGauss value for the three axis */
  for(i=0; i<3; i++)
  {
    pData[i]=( int16_t )(pnRawData[i] * sensitivity);
  }
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
  
