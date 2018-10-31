/**
 ******************************************************************************
 * @file    lsm6dsl.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   This file provides a set of functions needed to manage the LSM6DSL
 *          accelero and gyro devices
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
#include "lsm6dsl.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup LSM6DSL LSM6DSL
  * @{
  */

/** @defgroup LSM6DSL_Private_Variables LSM6DSL Private Variables
  * @{
  */ 
ACCELERO_DrvTypeDef Lsm6dslAccDrv =
{
  LSM6DSL_AccInit,
  LSM6DSL_AccDeInit,
  LSM6DSL_AccReadID,
  0,
  LSM6DSL_AccLowPower,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  LSM6DSL_AccReadXYZ
};

GYRO_DrvTypeDef Lsm6dslGyroDrv =
{
  LSM6DSL_GyroInit,
  LSM6DSL_GyroDeInit,
  LSM6DSL_GyroReadID,
  0,
  LSM6DSL_GyroLowPower,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  LSM6DSL_GyroReadXYZAngRate
};
/**
  * @}
  */ 

/** @defgroup LSM6DSL_ACC_Private_Functions LSM6DSL ACC Private Functions
  * @{
  */
/**
  * @brief  Set LSM6DSL Accelerometer Initialization.
  * @param  InitStruct: Init parameters
  */
void LSM6DSL_AccInit(uint16_t InitStruct)
{  
  uint8_t ctrl = 0x00;
  uint8_t tmp;

  /* Read CTRL1_XL */
  tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);

  /* Write value to ACC MEMS CTRL1_XL register: FS and Data Rate */
  ctrl = (uint8_t) InitStruct;
  tmp &= ~(0xFC);
  tmp |= ctrl;
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, tmp);

  /* Read CTRL3_C */
  tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C);

  /* Write value to ACC MEMS CTRL3_C register: BDU and Auto-increment */
  ctrl = ((uint8_t) (InitStruct >> 8));
  tmp &= ~(0x44);
  tmp |= ctrl; 
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C, tmp);
}

/**
  * @brief  LSM6DSL Accelerometer De-initialization.
  */
void LSM6DSL_AccDeInit(void)
{
  uint8_t ctrl = 0x00;
  
  /* Read control register 1 value */
  ctrl = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);

  /* Clear ODR bits */
  ctrl &= ~(LSM6DSL_ODR_BITPOSITION);

  /* Set Power down */
  ctrl |= LSM6DSL_ODR_POWER_DOWN;
  
  /* write back control register */
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, ctrl);
}

/**
  * @brief  Read LSM6DSL ID.
  * @retval ID 
  */
uint8_t LSM6DSL_AccReadID(void)
{  
  /* IO interface initialization */
  SENSOR_IO_Init();
  /* Read value at Who am I register address */
  return (SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_WHO_AM_I_REG));
}

/**
  * @brief  Set/Unset Accelerometer in low power mode.
  * @param  status 0 means disable Low Power Mode, otherwise Low Power Mode is enabled
  */
void LSM6DSL_AccLowPower(uint16_t status)
{
  uint8_t ctrl = 0x00;
  
  /* Read CTRL6_C value */
  ctrl = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL6_C);

  /* Clear Low Power Mode bit */
  ctrl &= ~(0x10);

  /* Set Low Power Mode */
  if(status)
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_XL_ENABLED;
  }else
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_XL_DISABLED;
  }
  
  /* write back control register */
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL6_C, ctrl);
}

/**
  * @brief  Read X, Y & Z Acceleration values 
  * @param  pData: Data out pointer
  */
void LSM6DSL_AccReadXYZ(int16_t* pData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx= 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  float sensitivity = 0;
  
  /* Read the acceleration control register content */
  ctrlx = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);
  
  /* Read output register X, Y & Z acceleration */
  SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_XL, buffer, 6);
  
  for(i=0; i<3; i++)
  {
    pnRawData[i]=((((uint16_t)buffer[2*i+1]) << 8) + (uint16_t)buffer[2*i]);
  }
  
  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL1_XL */
  switch(ctrlx & 0x0C)
  {
  case LSM6DSL_ACC_FULLSCALE_2G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_2G;
    break;
  case LSM6DSL_ACC_FULLSCALE_4G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_4G;
    break;
  case LSM6DSL_ACC_FULLSCALE_8G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_8G;
    break;
  case LSM6DSL_ACC_FULLSCALE_16G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_16G;
    break;    
  }
  
  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pData[i]=( int16_t )(pnRawData[i] * sensitivity);
  }
}

/**
  * @}
  */ 

/** @defgroup LSM6DSL_GYRO_Private_Functions LSM6DSL GYRO Private Functions
  * @{
  */

/**
  * @brief  Set LSM6DSL Gyroscope Initialization.
  * @param  InitStruct: pointer to a LSM6DSL_InitTypeDef structure 
  *         that contains the configuration setting for the LSM6DSL.
  */
void LSM6DSL_GyroInit(uint16_t InitStruct)
{  
  uint8_t ctrl = 0x00;
  uint8_t tmp;

  /* Read CTRL2_G */
  tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);

  /* Write value to GYRO MEMS CTRL2_G register: FS and Data Rate */
  ctrl = (uint8_t) InitStruct;
  tmp &= ~(0xFC);
  tmp |= ctrl;
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G, tmp);

  /* Read CTRL3_C */
  tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C);

  /* Write value to GYRO MEMS CTRL3_C register: BDU and Auto-increment */
  ctrl = ((uint8_t) (InitStruct >> 8));
  tmp &= ~(0x44);
  tmp |= ctrl; 
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C, tmp);
}


/**
  * @brief LSM6DSL Gyroscope De-initialization
  */
void LSM6DSL_GyroDeInit(void)
{
  uint8_t ctrl = 0x00;
  
  /* Read control register 1 value */
  ctrl = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);

  /* Clear ODR bits */
  ctrl &= ~(LSM6DSL_ODR_BITPOSITION);

  /* Set Power down */
  ctrl |= LSM6DSL_ODR_POWER_DOWN;
  
  /* write back control register */
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G, ctrl);
}

/**
  * @brief  Read ID address of LSM6DSL
  * @retval ID 
  */
uint8_t LSM6DSL_GyroReadID(void)
{
  /* IO interface initialization */
  SENSOR_IO_Init();  
  /* Read value at Who am I register address */
  return SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_WHO_AM_I_REG);
}

/**
  * @brief Set/Unset LSM6DSL Gyroscope in low power mode
  * @param  status 0 means disable Low Power Mode, otherwise Low Power Mode is enabled 
  */
void LSM6DSL_GyroLowPower(uint16_t status)
{  
  uint8_t ctrl = 0x00;
  
  /* Read CTRL7_G value */
  ctrl = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL7_G);

  /* Clear Low Power Mode bit */
  ctrl &= ~(0x80);

  /* Set Low Power Mode */
  if(status)
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_G_ENABLED;
  }else
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_G_DISABLED;
  }
  
  /* write back control register */
  SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL7_G, ctrl);
}

/**
* @brief  Calculate the LSM6DSL angular data.
* @param  pfData: Data out pointer
*/
void LSM6DSL_GyroReadXYZAngRate(float *pfData)
{
  int16_t pnRawData[3];
  uint8_t ctrlg= 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  float sensitivity = 0;
  
  /* Read the gyro control register content */
  ctrlg = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);
  
  /* Read output register X, Y & Z acceleration */
  SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_G, buffer, 6);
  
  for(i=0; i<3; i++)
  {
    pnRawData[i]=((((uint16_t)buffer[2*i+1]) << 8) + (uint16_t)buffer[2*i]);
  }
  
  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL2_G */
  switch(ctrlg & 0x0C)
  {
  case LSM6DSL_GYRO_FS_245:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_245DPS;
    break;
  case LSM6DSL_GYRO_FS_500:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_500DPS;
    break;
  case LSM6DSL_GYRO_FS_1000:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_1000DPS;
    break;
  case LSM6DSL_GYRO_FS_2000:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_2000DPS;
    break;    
  }
  
  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pfData[i]=( float )(pnRawData[i] * sensitivity);
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

