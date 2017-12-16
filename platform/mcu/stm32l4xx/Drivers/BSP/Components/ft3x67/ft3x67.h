/**
  ******************************************************************************
  * @file    ft3x67.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          ft3x67.c touch screen driver.
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
#ifndef __FT3X67_H
#define __FT3X67_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../Common/ts.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup FT3X67
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup FT3X67_Exported_Constants
  * @{
  */

/* Maximum border values of the touchscreen pad */
#define FT3X67_MAX_WIDTH                    390U     /* Touchscreen pad max width   */
#define FT3X67_MAX_HEIGHT                   390U     /* Touchscreen pad max height  */

/* Possible values of driver functions return status */
#define FT3X67_STATUS_OK                    0x00U
#define FT3X67_STATUS_NOT_OK                0x01U

/* Possible values of global variable 'TS_I2C_Initialized' */
#define FT3X67_I2C_NOT_INITIALIZED          0x00U
#define FT3X67_I2C_INITIALIZED              0x01U

/* Max detectable simultaneous touches */
#define FT3X67_MAX_DETECTABLE_TOUCH         0x02U

/* Definitions for FT3X67 registers */

/* Current mode register of the FT3X67 (R/W) */
#define FT3X67_DEV_MODE_REG                 0x00U
/* Possible values of FT3X67_DEV_MODE_REG */
#define FT3X67_DEV_MODE_WORKING             0x00U
#define FT3X67_DEV_MODE_FACTORY             0x40U

/* Touch Data Status register : gives number of active touch points (0..2) */
#define FT3X67_TD_STAT_REG                  0x02U
/* Values related to FT3X67_TD_STAT_REG */
#define FT3X67_TD_STAT_MASK                 0x0FU

/* Values Pn_XH and Pn_YH related */
#define FT3X67_TOUCH_EVT_FLAG_PRESS_DOWN    0x00U
#define FT3X67_TOUCH_EVT_FLAG_LIFT_UP       0x01U
#define FT3X67_TOUCH_EVT_FLAG_CONTACT       0x02U
#define FT3X67_TOUCH_EVT_FLAG_NO_EVENT      0x03U
#define FT3X67_TOUCH_EVT_FLAG_SHIFT         0x06U
#define FT3X67_TOUCH_EVT_FLAG_MASK          (3U << FT3X67_TOUCH_EVT_FLAG_SHIFT)
#define FT3X67_TOUCH_POS_MSB_MASK           0x0FU

/* Point 1 registers */
#define FT3X67_P1_XH_REG                    0x03U
#define FT3X67_P1_XL_REG                    0x04U
#define FT3X67_P1_YH_REG                    0x05U
#define FT3X67_P1_YL_REG                    0x06U
#define FT3X67_P1_WEIGHT_REG                0x07U
#define FT3X67_P1_MISC_REG                  0x08U

/* Point 2 registers */
#define FT3X67_P2_XH_REG                    0x09U
#define FT3X67_P2_XL_REG                    0x0AU
#define FT3X67_P2_YH_REG                    0x0BU
#define FT3X67_P2_YL_REG                    0x0CU
#define FT3X67_P2_WEIGHT_REG                0x0DU
#define FT3X67_P2_MISC_REG                  0x0EU

/* Values related to Pn_MISC register */
#define FT3X67_TOUCH_AREA_MASK              (0xFU << FT3X67_TOUCH_AREA_SHIFT)
#define FT3X67_TOUCH_AREA_SHIFT             0x04U

/* Threshold for touch detection register */
#define FT3X67_TH_GROUP_REG                 0x80U

/* Filter function coefficients register */
#define FT3X67_TH_DIFF_REG                  0x85U

/* Control register */
#define FT3X67_CTRL_REG                     0x86U
/* Values related to FT3X67_CTRL_REG */
#define FT3X67_CTRL_KEEP_ACTIVE_MODE               0x00U
#define FT3X67_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE  0x01U

/* The time period of switching from Active mode to Monitor mode when there is no touching */
#define FT3X67_TIMEENTERMONITOR_REG         0x87U

/* Report rate in Active mode */
#define FT3X67_PERIODACTIVE_REG             0x88U

/* Report rate in Monitor mode */
#define FT3X67_PERIODMONITOR_REG            0x89U

/* High 8-bit of LIB Version info */
#define FT3X67_LIB_VER_H_REG                0xA1U

/* Low 8-bit of LIB Version info */
#define FT3X67_LIB_VER_L_REG                0xA2U

/* Chip Selecting */
#define FT3X67_CIPHER_REG                   0xA3U

/* Interrupt mode register (used when in interrupt mode) */
#define FT3X67_GMODE_REG                    0xA4U
/* Possible values of FT3X67_GMODE_REG */
#define FT3X67_G_MODE_INTERRUPT_POLLING     0x00U
#define FT3X67_G_MODE_INTERRUPT_TRIGGER     0x01U

/* Current power mode */
#define FT3X67_PWR_MODE_REG                 0xA5U

/* Firmware version */
#define FT3X67_FIRMID_REG                   0xA6U

/* Chip identification register */
#define FT3X67_CHIP_ID_REG                  0xA8U
/* Possible values of FT3X67_CHIP_ID_REG */
#define FT3X67_ID_VALUE                     0x11U

/* Release code version */
#define FT3X67_RELEASE_CODE_ID_REG          0xAFU

/* Current operating mode register */
#define FT3X67_STATE_REG                    0xBCU
/* Possible values of FT3X67_STATE_REG */
#define FT3X67_STATE_INFO_MODE              0x00U
#define FT3X67_STATE_NORMAL_MODE            0x01U
#define FT3X67_STATE_FACTORY_MODE           0x03U
#define FT3X67_STATE_AUTO_CALIB_MODE        0x04U

/* Gesture enable register */
#define FT3X67_GESTURE_ENABLE_REG           0xD0U
/* Possible values of FT3X67_GESTURE_ENABLE_REG */
#define FT3X67_GESTURE_DISABLE              0x00U
#define FT3X67_GESTURE_ENABLE               0x01U

/* Gesture flag register */
#define FT3X67_GESTURE_FLAG_REG             0xD1U
/* Possible values of FT3X67_GESTURE_FLAG_REG can be any combination of following values */
#define FT3X67_GEST_LINE_RIGHT_TO_LEFT_ENABLE  0x01U
#define FT3X67_GEST_LINE_LEFT_TO_RIGHT_ENABLE  0x02U
#define FT3X67_GEST_LINE_DOWN_TO_UP_ENABLE     0x04U
#define FT3X67_GEST_LINE_UP_TO_DOWN_ENABLE     0x08U
#define FT3X67_GEST_DOUBLE_TAP_ENABLE          0x10U
#define FT3X67_GEST_ALL_FLAGS_ENABLE           0x1FU
#define FT3X67_GEST_ALL_FLAGS_DISABLE          0x00U

/* Gesture ID register */
#define FT3X67_GEST_ID_REG                  0xD3U
/* Possible values of FT3X67_GEST_ID_REG */
#define FT3X67_GEST_ID_NO_GESTURE           0x00U
#define FT3X67_GEST_ID_MOVE_UP              0x22U
#define FT3X67_GEST_ID_MOVE_RIGHT           0x21U
#define FT3X67_GEST_ID_MOVE_DOWN            0x23U
#define FT3X67_GEST_ID_MOVE_LEFT            0x20U
#define FT3X67_GEST_ID_DOUBLE_CLICK         0x24U

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup FT3X67_Exported_Functions
  * @{
  */

void     ft3x67_Init(uint16_t DeviceAddr);
void     ft3x67_Reset(uint16_t DeviceAddr);
uint16_t ft3x67_ReadID(uint16_t DeviceAddr);
void     ft3x67_TS_Start(uint16_t DeviceAddr);
uint8_t  ft3x67_TS_DetectTouch(uint16_t DeviceAddr);
void     ft3x67_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
void     ft3x67_TS_EnableIT(uint16_t DeviceAddr);
void     ft3x67_TS_DisableIT(uint16_t DeviceAddr);
uint8_t  ft3x67_TS_ITStatus (uint16_t DeviceAddr);
void     ft3x67_TS_ClearIT (uint16_t DeviceAddr);
void     ft3x67_TS_GestureConfig(uint16_t DeviceAddr, uint32_t Activation);
void     ft3x67_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);
void     ft3x67_TS_GetTouchInfo(uint16_t   DeviceAddr,
                                uint32_t   touchIdx,
                                uint32_t * pWeight,
                                uint32_t * pArea,
                                uint32_t * pEvent);

/**
  * @}
  */

/* Imported TS IO functions --------------------------------------------------------*/

/** @defgroup FT3X67_Imported_Functions
  * @{
  */

/* TouchScreen (TS) external IO functions */
extern void     TS_IO_Init(void);
extern void     TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg);
extern uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
extern void     TS_IO_Delay(uint32_t Delay);

/**
  * @}
  */

/* Imported global variables --------------------------------------------------------*/

/** @defgroup FT3X67_Imported_Globals
  * @{
  */

/* Touch screen driver structure */
extern TS_DrvTypeDef ft3x67_ts_drv;

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

#ifdef __cplusplus
}
#endif
#endif /* __FT3X67_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
