/**
  ******************************************************************************
  * @file    sensors_data.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   Manage sensors of STM32L475 IoT board.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include "vl53l0x_proximity.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

static float    TEMPERATURE_Value;
static float    HUMIDITY_Value;
static float    PRESSURE_Value;
static int16_t  ACC_Value[3];
static float    GYR_Value[3];
static int16_t  MAG_Value[3];
static uint16_t PROXIMITY_Value;

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  init_sensors
  * @param  none
  * @retval 0 in case of success
  *         -1 in case of failure
  */
int init_sensors(void)
{
  int ret = 0;
  
  if (HSENSOR_OK != BSP_HSENSOR_Init()) 
  {
    printf("BSP_HSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }
  
  if (TSENSOR_OK != BSP_TSENSOR_Init())
  {  
    printf("BSP_TSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }
  
  if (PSENSOR_OK != BSP_PSENSOR_Init())
  {  
    printf("BSP_PSENSOR_Init() returns %d\n", ret);
    ret = -1;
  }
  
  if (MAGNETO_OK != BSP_MAGNETO_Init()) 
  {  
    printf("BSP_MAGNETO_Init() returns %d\n", ret);
    ret = -1;
  }

  if (GYRO_OK != BSP_GYRO_Init()) 
  {  
    printf("BSP_GYRO_Init() returns %d\n", ret);
    ret = -1;
  }
  
  if (ACCELERO_OK != BSP_ACCELERO_Init()) 
  {  
    printf("BSP_ACCELERO_Init() returns %d\n", ret);
    ret = -1;
  }
    
  VL53L0X_PROXIMITY_Init();
    
  return ret;
}

/**
  * @brief  fill the payload with the sensor values
  * @param  none
  * @param PayloadBuffer is the char pointer for the Payload buffer to be filled
  * @param PayloadSize size of the above buffer
  * @retval 0 in case of success
  *         -1 in case of failure
  */
int PrepareMqttPayload(char * PayloadBuffer, int PayloadSize)
{
  char * Buff = PayloadBuffer;
  int BuffSize = PayloadSize;
  int snprintfreturn = 0;

  TEMPERATURE_Value = BSP_TSENSOR_ReadTemp();

  HUMIDITY_Value = BSP_HSENSOR_ReadHumidity();

  PRESSURE_Value = BSP_PSENSOR_ReadPressure();
  
  PROXIMITY_Value = VL53L0X_PROXIMITY_GetDistance();
       
  BSP_ACCELERO_AccGetXYZ(ACC_Value);
     
  BSP_GYRO_GetXYZ(GYR_Value);
     
  BSP_MAGNETO_GetXYZ(MAG_Value);
  
  snprintfreturn = snprintf( Buff, BuffSize, "{\n \"state\": {\n  \"reported\": {\n"
           "   \"temperature\": %.2f,\n   \"humidity\": %.2f,\n   \"pressure\": %.2f,\n   \"proximity\": %d,\n"
           "   \"acc_x\": %d, \"acc_y\": %d, \"acc_z\": %d,\n"
           "   \"gyr_x\": %.0f, \"gyr_y\": %.0f, \"gyr_z\": %.0f,\n"
           "   \"mag_x\": %d, \"mag_y\": %d, \"mag_z\": %d\n"
           "  }\n }\n}",
           TEMPERATURE_Value, HUMIDITY_Value, PRESSURE_Value, PROXIMITY_Value,
           ACC_Value[0], ACC_Value[1], ACC_Value[2],
           GYR_Value[0], GYR_Value[1], GYR_Value[2],
           MAG_Value[0], MAG_Value[1], MAG_Value[2] );
  
  /* Check total size to be less than buffer size
            * if the return is >=0 and <n, then
            * the entire string was successfully formatted; if the return is
            * >=n, the string was truncated (but there is still a null char
            * at the end of what was written); if the return is <0, there was
            * an error.
            */
  if (snprintfreturn >= 0 && snprintfreturn < PayloadSize)
  {
      return 0;
  }
  else if(snprintfreturn >= PayloadSize)
  {
      printf("Data Pack truncated\n");
      return 0;
  }
  else
  {
      printf("Data Pack Error\n");
      return -1;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
