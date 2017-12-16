/**
 ******************************************************************************
 * @file    lis3mdl.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   LIS3MDL header driver file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIS3MDL__H
#define __LIS3MDL__H

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/
#include "../Common/magneto.h"
 
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup LIS3MDL
  * @{
  */

/** @defgroup LIS3MDL_Exported_Constants LIS3MDL Exported Constants
  * @{
  */
/************** I2C Address *****************/

#define LIS3MDL_MAG_I2C_ADDRESS_LOW    ((uint8_t)0x38)  // SAD[0] = 0
#define LIS3MDL_MAG_I2C_ADDRESS_HIGH   ((uint8_t)0x3C)  // SAD[0] = 1

/************** Who am I  *******************/

#define I_AM_LIS3MDL                        ((uint8_t)0x3D)

/************** Device Register  *******************/

#define LIS3MDL_MAG_WHO_AM_I_REG    0x0F
#define LIS3MDL_MAG_CTRL_REG1       0x20
#define LIS3MDL_MAG_CTRL_REG2       0x21
#define LIS3MDL_MAG_CTRL_REG3       0x22
#define LIS3MDL_MAG_CTRL_REG4       0x23
#define LIS3MDL_MAG_CTRL_REG5       0x24
#define LIS3MDL_MAG_STATUS_REG      0x27
#define LIS3MDL_MAG_OUTX_L          0x28
#define LIS3MDL_MAG_OUTX_H          0x29
#define LIS3MDL_MAG_OUTY_L          0x2A
#define LIS3MDL_MAG_OUTY_H          0x2B
#define LIS3MDL_MAG_OUTZ_L          0x2C
#define LIS3MDL_MAG_OUTZ_H          0x2D
#define LIS3MDL_MAG_TEMP_OUT_L      0x2E
#define LIS3MDL_MAG_TEMP_OUT_H      0x2F
#define LIS3MDL_MAG_INT_CFG         0x30
#define LIS3MDL_MAG_INT_SRC         0x31
#define LIS3MDL_MAG_INT_THS_L       0x32
#define LIS3MDL_MAG_INT_THS_H       0x33
  
/* Mag Temperature Sensor Control*/ 
#define LIS3MDL_MAG_TEMPSENSOR_ENABLE        ((uint8_t) 0x80)   /*!< Temp sensor Enable */
#define LIS3MDL_MAG_TEMPSENSOR_DISABLE       ((uint8_t) 0x00)   /*!< Temp sensor Disable */

/* Mag_XY-axis Operating Mode */ 
#define LIS3MDL_MAG_OM_XY_LOWPOWER           ((uint8_t) 0x00)
#define LIS3MDL_MAG_OM_XY_MEDIUM             ((uint8_t) 0x20)
#define LIS3MDL_MAG_OM_XY_HIGH               ((uint8_t) 0x40)
#define LIS3MDL_MAG_OM_XY_ULTRAHIGH          ((uint8_t) 0x60)
   
/* Mag Data Rate */ 
#define LIS3MDL_MAG_ODR_0_625_HZ             ((uint8_t) 0x00)  /*!< Output Data Rate = 0.625 Hz */
#define LIS3MDL_MAG_ODR_1_25_HZ              ((uint8_t) 0x04)  /*!< Output Data Rate = 1.25 Hz  */
#define LIS3MDL_MAG_ODR_2_5_HZ               ((uint8_t) 0x08)  /*!< Output Data Rate = 2.5 Hz   */
#define LIS3MDL_MAG_ODR_5_0_HZ               ((uint8_t) 0x0C)  /*!< Output Data Rate = 5.0 Hz   */
#define LIS3MDL_MAG_ODR_10_HZ                ((uint8_t) 0x10)  /*!< Output Data Rate = 10 Hz    */
#define LIS3MDL_MAG_ODR_20_HZ                ((uint8_t) 0x14)  /*!< Output Data Rate = 20 Hz    */
#define LIS3MDL_MAG_ODR_40_HZ                ((uint8_t) 0x18)  /*!< Output Data Rate = 40 Hz    */
#define LIS3MDL_MAG_ODR_80_HZ                ((uint8_t) 0x1C)  /*!< Output Data Rate = 80 Hz    */

/* Mag Data Rate */ 
#define LMS303C_MAG_SELFTEST_DISABLE         ((uint8_t 0x00)     
#define LMS303C_MAG_SELFTEST_ENABLE          ((uint8_t 0x01)
   
/* Mag Full Scale */ 
#define LIS3MDL_MAG_FS_DEFAULT               ((uint8_t) 0x00)
#define LIS3MDL_MAG_FS_4_GA                  ((uint8_t) 0x00)  
#define LIS3MDL_MAG_FS_8_GA                  ((uint8_t) 0x20)
#define LIS3MDL_MAG_FS_12_GA                 ((uint8_t) 0x40)  
#define LIS3MDL_MAG_FS_16_GA                 ((uint8_t) 0x60)  /*!< Full scale = ±16 Gauss */

/* Mag_Reboot */ 
#define LIS3MDL_MAG_REBOOT_DEFAULT           ((uint8_t) 0x00)
#define LIS3MDL_MAG_REBOOT_ENABLE            ((uint8_t) 0x08)
   
/* Mag Soft reset */ 
#define LIS3MDL_MAG_SOFT_RESET_DEFAULT       ((uint8_t) 0x00)
#define LIS3MDL_MAG_SOFT_RESET_ENABLE        ((uint8_t) 0x04)
   
/* Mag_Communication_Mode */ 
#define LIS3MDL_MAG_SIM_4_WIRE               ((uint8_t) 0x00)
#define LIS3MDL_MAG_SIM_3_WIRE               ((uint8_t) 0x04)
   
/* Mag Lowpower mode config */ 
#define LIS3MDL_MAG_CONFIG_NORMAL_MODE       ((uint8_t) 0x00)
#define LIS3MDL_MAG_CONFIG_LOWPOWER_MODE     ((uint8_t) 0x20)
   
/* Mag Operation Mode */ 
#define LIS3MDL_MAG_SELECTION_MODE           ((uint8_t) 0x03) /* CTRL_REG3 */
#define LIS3MDL_MAG_CONTINUOUS_MODE          ((uint8_t) 0x00)
#define LIS3MDL_MAG_SINGLE_MODE              ((uint8_t) 0x01)
#define LIS3MDL_MAG_POWERDOWN1_MODE          ((uint8_t) 0x02)
#define LIS3MDL_MAG_POWERDOWN2_MODE          ((uint8_t) 0x03)

/* Mag_Z-axis Operation Mode */ 
#define LIS3MDL_MAG_OM_Z_LOWPOWER            ((uint8_t) 0x00)
#define LIS3MDL_MAG_OM_Z_MEDIUM              ((uint8_t) 0x04)
#define LIS3MDL_MAG_OM_Z_HIGH                ((uint8_t) 0x08)
#define LIS3MDL_MAG_OM_Z_ULTRAHIGH           ((uint8_t) 0x0C)   

/* Mag Big little-endian selection */ 
#define LIS3MDL_MAG_BLE_LSB                  ((uint8_t) 0x00)
#define LIS3MDL_MAG_BLE_MSB                  ((uint8_t) 0x02)


/* Mag_Bloc_update_magnetic_data */ 
#define LIS3MDL_MAG_BDU_CONTINUOUS           ((uint8_t) 0x00)
#define LIS3MDL_MAG_BDU_MSBLSB               ((uint8_t) 0x40)
   
   
/* Magnetometer_Sensitivity */
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_4GA   ((float)0.14f)  /**< Sensitivity value for 4 gauss full scale  [mgauss/LSB] */
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_8GA   ((float)0.29f)  /**< Sensitivity value for 8 gauss full scale  [mgauss/LSB] */
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_12GA  ((float)0.43f)  /**< Sensitivity value for 12 gauss full scale [mgauss/LSB] */
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_16GA  ((float)0.58f)  /**< Sensitivity value for 16 gauss full scale [mgauss/LSB] */

/**
  * @}
  */

  
/** @defgroup LIS3MDL_Exported_Functions LIS3MDL Exported Functions
  * @{
  */
  
void LIS3MDL_MagInit(MAGNETO_InitTypeDef LIS3MDL_InitStruct);
void LIS3MDL_MagDeInit(void);
uint8_t LIS3MDL_MagReadID(void);
void LIS3MDL_MagLowPower(uint16_t status);
void LIS3MDL_MagReadXYZ(int16_t* pData);

/**
  * @}
  */


/** @defgroup LIS3MDL_Imported_Functions LIS3MDL Imported Functions
 * @{
 */
/* IO functions */
extern void     SENSOR_IO_Init(void);
extern void     SENSOR_IO_DeInit(void);
extern void     SENSOR_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  SENSOR_IO_Read(uint8_t Addr, uint8_t Reg);
extern uint16_t SENSOR_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
extern void     SENSOR_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
/**
  * @}
  */

/** @defgroup LIS3MDL_Imported_Globals  Imported Globals
  * @{
  */
/* MAG driver structure */
extern MAGNETO_DrvTypeDef  Lis3mdlMagDrv;
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __LIS3MDL__H */

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
