/**
  ******************************************************************************
  * @file    stm32l475e_iot01_magneto.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-March-2017
  * @brief   This file provides a set of functions needed to manage the magnetometer sensor
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
#include "stm32l475e_iot01_magneto.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */
      
/** @defgroup STM32L475E_IOT01_MAGNETO MAGNETO
  * @{
  */ 

/** @defgroup STM32L475E_IOT01_MAGNETO_Private_Variables MAGNETO Private Variables
  * @{
  */
static MAGNETO_DrvTypeDef  *MagnetoDrv;
/**
  * @}
  */


/** @defgroup STM32L475E_IOT01_MAGNETO_Private_Functions MAGNETO Private Functions
  * @{
  */

/**
 * @brief Initialize a magnetometer sensor
 * @retval COMPONENT_ERROR in case of failure
 */
MAGNETO_StatusTypeDef BSP_MAGNETO_Init(void)
{
  MAGNETO_StatusTypeDef ret = MAGNETO_OK;
  MAGNETO_InitTypeDef LIS3MDL_InitStructureMag;

  if(Lis3mdlMagDrv.ReadID() != I_AM_LIS3MDL)
  {
    ret = MAGNETO_ERROR;
  }
  else
  {
    /* Initialize the MAGNETO magnetometer driver structure */
    MagnetoDrv = &Lis3mdlMagDrv;
    
    /* MEMS configuration ------------------------------------------------------*/
    /* Fill the MAGNETO magnetometer structure */
    LIS3MDL_InitStructureMag.Register1 = LIS3MDL_MAG_TEMPSENSOR_DISABLE | LIS3MDL_MAG_OM_XY_HIGH | LIS3MDL_MAG_ODR_40_HZ;
    LIS3MDL_InitStructureMag.Register2 = LIS3MDL_MAG_FS_4_GA | LIS3MDL_MAG_REBOOT_DEFAULT | LIS3MDL_MAG_SOFT_RESET_DEFAULT;
    LIS3MDL_InitStructureMag.Register3 = LIS3MDL_MAG_CONFIG_NORMAL_MODE | LIS3MDL_MAG_CONTINUOUS_MODE;
    LIS3MDL_InitStructureMag.Register4 = LIS3MDL_MAG_OM_Z_HIGH | LIS3MDL_MAG_BLE_LSB;
    LIS3MDL_InitStructureMag.Register5 = LIS3MDL_MAG_BDU_MSBLSB;
    /* Configure the MAGNETO magnetometer main parameters */
    MagnetoDrv->Init(LIS3MDL_InitStructureMag);
  } 

  return ret;  
}

/**
  * @brief  DeInitialize the MAGNETO.
  */
void BSP_MAGNETO_DeInit(void)
{
  /* DeInitialize the  magnetometer IO interfaces */
  if(MagnetoDrv != NULL)
  {
    if(MagnetoDrv->DeInit != NULL)
    {
      MagnetoDrv->DeInit();
    }
  }
}

/**
  * @brief  Set/Unset the MAGNETO in low power mode.
  */
void BSP_MAGNETO_LowPower(uint16_t status)
{
  /* Put the magnetometer in low power mode */
  if(MagnetoDrv != NULL)
  {
    if(MagnetoDrv->LowPower != NULL)
    {
      MagnetoDrv->LowPower(status);
    }
  }
}

/**
  * @brief  Get XYZ magnetometer values.
  * @param  pDataXYZ Pointer on 3 magnetometer values table with
  *                  pDataXYZ[0] = X axis, pDataXYZ[1] = Y axis, pDataXYZ[2] = Z axis 
  */
void BSP_MAGNETO_GetXYZ(int16_t *pDataXYZ)
{
  if(MagnetoDrv != NULL)
  {
    if(MagnetoDrv->GetXYZ != NULL)
    {   
      MagnetoDrv->GetXYZ(pDataXYZ);
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
