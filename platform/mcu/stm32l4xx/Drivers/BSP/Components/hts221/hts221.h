/**
 ******************************************************************************
 * @file    hts221.h
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    14-February-2017
 * @brief   HTS221 header driver file
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
#ifndef __HTS221__H
#define __HTS221__H

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/
#include "../Common/hsensor.h"
#include "../Common/tsensor.h"  

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup HTS221 
  * @{
  */

/** @defgroup HTS221_Exported_Constants HTS221 Exported Constants
  * @{
  */

/**
  * @brief  Bitfield positioning.
  */
#define HTS221_BIT(x) ((uint8_t)x)


/**
  * @brief Device Identification register.
  *        Read
  *        Default value: 0xBC
  *        7:0 This read-only register contains the device identifier for HTS221.
  */
#define HTS221_WHO_AM_I_REG          (uint8_t)0x0F

/**
  * @brief Device Identification value.
  */
#define HTS221_WHO_AM_I_VAL         (uint8_t)0xBC


/**
  * @brief Humidity and temperature average mode register.
  *        Read/write
  *        Default value: 0x1B
  *        7:6 Reserved.
  *        5:3 AVGT2-AVGT1-AVGT0: Select the temperature internal average.
  *        
  *             AVGT2 | AVGT1 | AVGT0 | Nr. Internal Average
  *          ----------------------------------------------------
  *              0    |   0   |   0   |    2
  *              0    |   0   |   1   |    4
  *              0    |   1   |   0   |    8
  *              0    |   1   |   1   |    16
  *              1    |   0   |   0   |    32
  *              1    |   0   |   1   |    64
  *              1    |   1   |   0   |    128
  *              1    |   1   |   1   |    256
  *        
  *        2:0 AVGH2-AVGH1-AVGH0: Select humidity internal average.
  *             AVGH2 | AVGH1 |  AVGH0 | Nr. Internal Average
  *          ------------------------------------------------------
  *              0    |   0   |   0   |    4
  *              0    |   0   |   1   |    8
  *              0    |   1   |   0   |    16
  *              0    |   1   |   1   |    32
  *              1    |   0   |   0   |    64
  *              1    |   0   |   1   |    128
  *              1    |   1   |   0   |    256
  *              1    |   1   |   1   |    512
  *
  */
#define HTS221_AV_CONF_REG        (uint8_t)0x10

#define HTS221_AVGT_BIT           HTS221_BIT(3)
#define HTS221_AVGH_BIT           HTS221_BIT(0)

#define HTS221_AVGH_MASK          (uint8_t)0x07
#define HTS221_AVGT_MASK          (uint8_t)0x38

/**
  * @brief Control register 1.
  *        Read/write
  *        Default value: 0x00
  *        7 PD: power down control. 0 - power down mode; 1 - active mode.
  *        6:3 Reserved.
  *        2 BDU: block data update. 0 - continuous update
  *                                  1 - output registers not updated until MSB and LSB reading.
  *        1:0 ODR1, ODR0: output data rate selection.
  *       
  *          ODR1  | ODR0  | Humidity output data-rate(Hz)  | Pressure output data-rate(Hz)
  *          ----------------------------------------------------------------------------------
  *            0   |   0   |         one shot               |         one shot
  *            0   |   1   |            1                   |            1
  *            1   |   0   |            7                   |            7
  *            1   |   1   |           12.5                 |           12.5
  *       
  */
#define HTS221_CTRL_REG1      (uint8_t)0x20

#define HTS221_PD_BIT          HTS221_BIT(7)
#define HTS221_BDU_BIT         HTS221_BIT(2)
#define HTS221_ODR_BIT         HTS221_BIT(0)

#define HTS221_PD_MASK        (uint8_t)0x80
#define HTS221_BDU_MASK       (uint8_t)0x04
#define HTS221_ODR_MASK       (uint8_t)0x03

/**
  * @brief Control register 2.
  *        Read/write
  *        Default value: 0x00
  *        7 BOOT:  Reboot memory content. 0: normal mode 
  *                                        1: reboot memory content. Self-cleared upon completation.
  *        6:2 Reserved.
  *        1 HEATHER: 0: heater enable; 1: heater disable.
  *        0 ONE_SHOT: 0: waiting for start of conversion 
  *                    1: start for a new dataset. Self-cleared upon completation.
  */
#define HTS221_CTRL_REG2      (uint8_t)0x21

#define HTS221_BOOT_BIT        HTS221_BIT(7)
#define HTS221_HEATHER_BIT     HTS221_BIT(1)
#define HTS221_ONESHOT_BIT     HTS221_BIT(0)

#define HTS221_BOOT_MASK      (uint8_t)0x80
#define HTS221_HEATHER_MASK   (uint8_t)0x02
#define HTS221_ONE_SHOT_MASK  (uint8_t)0x01

/**
  * @brief Control register 3.
  *        Read/write
  *        Default value: 0x00
  *        7 DRDY_H_L: Interrupt edge. 0: active high, 1: active low.
  *        6 PP_OD: Push-Pull/OpenDrain selection on interrupt pads. 0: push-pull 
  *                                                                  1: open drain.
  *        5:3 Reserved.
  *        2 DRDY: interrupt config. 0: disable, 1: enable.
  */
#define HTS221_CTRL_REG3      (uint8_t)0x22

#define HTS221_DRDY_H_L_BIT    HTS221_BIT(7)
#define HTS221_PP_OD_BIT       HTS221_BIT(6)
#define HTS221_DRDY_BIT        HTS221_BIT(2)

#define HTS221_DRDY_H_L_MASK  (uint8_t)0x80
#define HTS221_PP_OD_MASK     (uint8_t)0x40
#define HTS221_DRDY_MASK      (uint8_t)0x04

/**
  * @brief  Status register.
  *         Read
  *         Default value: 0x00
  *         7:2 Reserved.
  *         1 H_DA: Humidity data available. 0: new data for humidity is not yet available 
  *                                          1: new data for humidity is available.
  *         0 T_DA: Temperature data available. 0: new data for temperature is not yet available
  *                                             1: new data for temperature is available.
  */
#define HTS221_STATUS_REG    (uint8_t)0x27

#define HTS221_H_DA_BIT       HTS221_BIT(1)
#define HTS221_T_DA_BIT       HTS221_BIT(0)

#define HTS221_HDA_MASK      (uint8_t)0x02
#define HTS221_TDA_MASK      (uint8_t)0x01

/**
  * @brief  Humidity data (LSB).
  *         Read
  *         Default value: 0x00.
  *         HOUT7 - HOUT0: Humidity data LSB (2's complement).
  */
#define HTS221_HR_OUT_L_REG        (uint8_t)0x28

/**
  * @brief  Humidity data (MSB).
  *         Read
  *         Default value: 0x00.
  *         HOUT15 - HOUT8: Humidity data MSB (2's complement).
  */
#define HTS221_HR_OUT_H_REG        (uint8_t)0x29

/**
  * @brief  Temperature data (LSB).
  *         Read
  *         Default value: 0x00.
  *         TOUT7 - TOUT0: temperature data LSB.
  */
#define HTS221_TEMP_OUT_L_REG         (uint8_t)0x2A

/**
  * @brief  Temperature data (MSB).
  *         Read
  *         Default value: 0x00.
  *         TOUT15 - TOUT8: temperature data MSB.
  */
#define HTS221_TEMP_OUT_H_REG         (uint8_t)0x2B

/**
  * @brief  Calibration registers.
  * Read
  */
#define HTS221_H0_RH_X2        (uint8_t)0x30
#define HTS221_H1_RH_X2        (uint8_t)0x31
#define HTS221_T0_DEGC_X8      (uint8_t)0x32
#define HTS221_T1_DEGC_X8      (uint8_t)0x33
#define HTS221_T0_T1_DEGC_H2   (uint8_t)0x35
#define HTS221_H0_T0_OUT_L     (uint8_t)0x36
#define HTS221_H0_T0_OUT_H     (uint8_t)0x37
#define HTS221_H1_T0_OUT_L     (uint8_t)0x3A
#define HTS221_H1_T0_OUT_H     (uint8_t)0x3B
#define HTS221_T0_OUT_L        (uint8_t)0x3C
#define HTS221_T0_OUT_H        (uint8_t)0x3D
#define HTS221_T1_OUT_L        (uint8_t)0x3E
#define HTS221_T1_OUT_H        (uint8_t)0x3F

/**
* @}
*/


/** @defgroup HTS221_Humidity_Exported_Functions HTS221 Humidity Exported Functions
  * @{
  */  
/* HUMIDITY functions */
void HTS221_H_Init(uint16_t DeviceAddr);
uint8_t HTS221_H_ReadID(uint16_t DeviceAddr);
float HTS221_H_ReadHumidity(uint16_t DeviceAddr);
/**
  * @}
  */

/** @defgroup HTS221_HumImported_Globals  Humidity Imported Globals
  * @{
  */
/* Humidity driver structure */
extern HSENSOR_DrvTypeDef HTS221_H_Drv;
/**
  * @}
  */

/** @defgroup HTS221_Temperature_Exported_Functions HTS221 Temperature Exported Functions
  * @{
  */
/* TEMPERATURE functions */
void HTS221_T_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct);
float HTS221_T_ReadTemp(uint16_t DeviceAddr);
/**
  * @}
  */
  
/** @defgroup HTS221_TempImported_Globals  Temperature Imported Globals
  * @{
  */
/* Temperature driver structure */
extern TSENSOR_DrvTypeDef HTS221_T_Drv;

/**
  * @}
  */

/** @defgroup HTS221_Imported_Functions HTS221 Imported Functions
  * @{
  */
/* IO functions */
extern void     SENSOR_IO_Init(void);
extern void     SENSOR_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  SENSOR_IO_Read(uint8_t Addr, uint8_t Reg);
extern uint16_t SENSOR_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
extern void     SENSOR_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __HTS221__H */

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
