/**
  ******************************************************************************
  * @file    stm32l475e_iot01_accelero.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   This file provides a set of functions needed to manage the accelerometer sensor
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_accelero.h"
/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_ACCELERO ACCELERO
  * @{
  */

/** @defgroup STM32L475E_IOT01_ACCELERO_Private_Variables ACCELERO Private Variables 
  * @{
  */    
static ACCELERO_DrvTypeDef *AccelerometerDrv;  
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_ACCELERO_Private_Functions ACCELERO Private Functions
  * @{
  */ 
/**
  * @brief  Initialize the ACCELERO.
  * @retval ACCELERO_OK or ACCELERO_ERROR
  */
ACCELERO_StatusTypeDef BSP_ACCELERO_Init(void)
{  
  ACCELERO_StatusTypeDef ret = ACCELERO_OK;
  uint16_t ctrl = 0x0000;
  ACCELERO_InitTypeDef LSM6DSL_InitStructure;

  if(Lsm6dslAccDrv.ReadID() != LSM6DSL_ACC_GYRO_WHO_AM_I)
  {
    ret = ACCELERO_ERROR;
  }
  else
  {
    /* Initialize the ACCELERO accelerometer driver structure */
    AccelerometerDrv = &Lsm6dslAccDrv;
  
    /* MEMS configuration ------------------------------------------------------*/
    /* Fill the ACCELERO accelerometer structure */
    LSM6DSL_InitStructure.AccOutput_DataRate = LSM6DSL_ODR_52Hz;
    LSM6DSL_InitStructure.Axes_Enable = 0;
    LSM6DSL_InitStructure.AccFull_Scale = LSM6DSL_ACC_FULLSCALE_2G;
    LSM6DSL_InitStructure.BlockData_Update = LSM6DSL_BDU_BLOCK_UPDATE;
    LSM6DSL_InitStructure.High_Resolution = 0;
    LSM6DSL_InitStructure.Communication_Mode = 0;
        
    /* Configure MEMS: data rate, full scale  */
    ctrl =  (LSM6DSL_InitStructure.AccOutput_DataRate | LSM6DSL_InitStructure.AccFull_Scale);
    
    /* Configure MEMS: BDU and Auto-increment for multi read/write */
    ctrl |= ((LSM6DSL_InitStructure.BlockData_Update | LSM6DSL_ACC_GYRO_IF_INC_ENABLED) << 8);

    /* Configure the ACCELERO accelerometer main parameters */
    AccelerometerDrv->Init(ctrl);
  }  

  return ret;
}

/**
  * @brief  DeInitialize the ACCELERO.
  * @retval None.
  */
void BSP_ACCELERO_DeInit(void)
{
  /* DeInitialize the accelerometer IO interfaces */
  if(AccelerometerDrv != NULL)
  {
    if(AccelerometerDrv->DeInit != NULL)
    {
      AccelerometerDrv->DeInit();
    }
  }
}

/**
  * @brief  Set/Unset the ACCELERO in low power mode.
  * @param  status 0 means disable Low Power Mode, otherwise Low Power Mode is enabled
  * @retval None
  */
void BSP_ACCELERO_LowPower(uint16_t status)
{
  /* Set/Unset the ACCELERO in low power mode */
  if(AccelerometerDrv != NULL)
  {
    if(AccelerometerDrv->LowPower != NULL)
    {
      AccelerometerDrv->LowPower(status);
    }
  }
}

/**
  * @brief  Get XYZ acceleration values.
  * @param  pDataXYZ Pointer on 3 angular accelerations table with  
  *                  pDataXYZ[0] = X axis, pDataXYZ[1] = Y axis, pDataXYZ[2] = Z axis
  * @retval None
  */
void BSP_ACCELERO_AccGetXYZ(int16_t *pDataXYZ)
{
  if(AccelerometerDrv != NULL)
  {
    if(AccelerometerDrv->GetXYZ != NULL)
    {   
      AccelerometerDrv->GetXYZ(pDataXYZ);
    }
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
