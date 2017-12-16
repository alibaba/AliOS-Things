/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSOR_H
#define __SENSOR_H

/* Defines -------------------------------------------------------------------*/
/* #define USE_LPS22HB_TEMP */
/* Defines -------------------------------------------------------------------*/
#define PROXIMITY_I2C_ADDRESS         ((uint16_t)0x0052)
#define VL53L0X_ID                    ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_qspi.h"

#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
//#include "vl53l0x_tof.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void QSPI_demo(void);
void QSPI_MemoryMapped_demo(void);
void Temperature_Test(void);
void Humidity_Test(void);
void Pressure_Test(void);
void Gyro_Test(void);
void Magneto_Test(void);
void Accelero_Test(void);
void Proximity_Test(void);

uint32_t Serial_Scanf(uint32_t value);
void Error_Handler(void);

#endif /* __SENSOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
