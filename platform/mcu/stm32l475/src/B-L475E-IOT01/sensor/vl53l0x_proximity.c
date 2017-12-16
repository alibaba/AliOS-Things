/**
  ******************************************************************************
  * @file    Cloud/AWS/Src/vl53l0x_proximity.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   High level API for VL53L0x proximity/time-of-flight sensor.
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
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "vl53l0x_proximity.h"
#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define PROXIMITY_I2C_ADDRESS         ((uint16_t)0x0052)
#define VL53L0X_ID                    ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hI2cHandler;
VL53L0X_Dev_t Dev =
{
  .I2cHandle = &hI2cHandler,
  .I2cDevAddr = PROXIMITY_I2C_ADDRESS
};

/* Private function prototypes -----------------------------------------------*/
void SENSOR_IO_Init(void);
void SetupSingleShot(VL53L0X_Dev_t Dev);

/**
  * @brief  VL53L0X proximity sensor Initialization.
  */
void VL53L0X_PROXIMITY_Init(void)
{
  uint16_t vl53l0x_id = 0; 
  VL53L0X_DeviceInfo_t VL53L0X_DeviceInfo;
  
  /* Initialize IO interface */
  SENSOR_IO_Init();
  VL53L0X_PROXIMITY_MspInit();
  
  memset(&VL53L0X_DeviceInfo, 0, sizeof(VL53L0X_DeviceInfo_t));
  
  if (VL53L0X_ERROR_NONE == VL53L0X_GetDeviceInfo(&Dev, &VL53L0X_DeviceInfo))
  {  
    if (VL53L0X_ERROR_NONE == VL53L0X_RdWord(&Dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, (uint16_t *) &vl53l0x_id))
    {
      if (vl53l0x_id == VL53L0X_ID)
      {
        if (VL53L0X_ERROR_NONE == VL53L0X_DataInit(&Dev))
        {
          SetupSingleShot(Dev);
        }
      }
    }
    else
    {
      printf("VL53L0X Time of Flight Failed to Initialize!\n");
    }
  }
  else
  {
    printf("VL53L0X Time of Flight Failed to get infos!\n");
  }  
}

/**
  * @brief  Get distance from VL53L0X proximity sensor.
  * @retval Distance in mm
  */
uint16_t VL53L0X_PROXIMITY_GetDistance(void)
{
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  
  VL53L0X_PerformSingleRangingMeasurement(&Dev, &RangingMeasurementData);
  
  return RangingMeasurementData.RangeMilliMeter;  
}

/**
  * @brief  VL53L0X proximity sensor Msp Initialization.
  */
void VL53L0X_PROXIMITY_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /*Configure GPIO pin : VL53L0X_XSHUT_Pin */
  GPIO_InitStruct.Pin = VL53L0X_XSHUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(VL53L0X_XSHUT_GPIO_Port, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(VL53L0X_XSHUT_GPIO_Port, VL53L0X_XSHUT_Pin, GPIO_PIN_SET);
  
  HAL_Delay(1000);  
}

/**
 *  Setup all detected sensors for single shot mode and setup ranging configuration
 */
void SetupSingleShot(VL53L0X_Dev_t Dev)
{
  int status;
  uint8_t VhvSettings;
  uint8_t PhaseCal;
  uint32_t refSpadCount;
	uint8_t isApertureSpads;
	FixPoint1616_t signalLimit = (FixPoint1616_t)(0.25*65536);
	FixPoint1616_t sigmaLimit = (FixPoint1616_t)(18*65536);
	uint32_t timingBudget = 33000;
	uint8_t preRangeVcselPeriod = 14;
	uint8_t finalRangeVcselPeriod = 10;

                          
  if( Dev.Present){
    status=VL53L0X_StaticInit(&Dev);
    if( status ){
      printf("VL53L0X_StaticInit failed\n");
    }
    
    
    status = VL53L0X_PerformRefCalibration(&Dev, &VhvSettings, &PhaseCal);
    if( status ){
      printf("VL53L0X_PerformRefCalibration failed\n");
    }
    
    status = VL53L0X_PerformRefSpadManagement(&Dev, &refSpadCount, &isApertureSpads);
    if( status ){
      printf("VL53L0X_PerformRefSpadManagement failed\n");
    }
    
    status = VL53L0X_SetDeviceMode(&Dev, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
    if( status ){
      printf("VL53L0X_SetDeviceMode failed\n");
    }
    
    status = VL53L0X_SetLimitCheckEnable(&Dev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
    if( status ){
      printf("VL53L0X_SetLimitCheckEnable failed\n");
    }
    
    status = VL53L0X_SetLimitCheckEnable(&Dev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
    if( status ){
      printf("VL53L0X_SetLimitCheckEnable failed\n");
    }
    
    /* Ranging configuration */
    signalLimit = (FixPoint1616_t)(0.1*65536);
    sigmaLimit = (FixPoint1616_t)(60*65536);
    timingBudget = 33000;
    preRangeVcselPeriod = 18;
    finalRangeVcselPeriod = 14;
    
    status = VL53L0X_SetLimitCheckValue(&Dev,  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
    
    if( status ){
      printf("VL53L0X_SetLimitCheckValue failed\n");
    }
    
    status = VL53L0X_SetLimitCheckValue(&Dev,  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
    if( status ){
      printf("VL53L0X_SetLimitCheckValue failed\n");
    }
    
    status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&Dev,  timingBudget);
    if( status ){
      printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
    }
    
    status = VL53L0X_SetVcselPulsePeriod(&Dev,  VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
    if( status ){
      printf("VL53L0X_SetVcselPulsePeriod failed\n");
    }
    
    status = VL53L0X_SetVcselPulsePeriod(&Dev,  VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
    if( status ){
      printf("VL53L0X_SetVcselPulsePeriod failed\n");
    }
    
    Dev.LeakyFirst=1;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
