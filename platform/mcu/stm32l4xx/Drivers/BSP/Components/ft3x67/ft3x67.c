/**
  ******************************************************************************
  * @file    ft3x67.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the FT3X67
  *          touch screen devices.
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
#include "ft3x67.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup FT3X67
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/** @defgroup FT3X67_Private_Types_Definitions
  * @{
  */

/* ft3x67 Handle definition. */
typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} ft3x67_handle_TypeDef;

/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @defgroup FT3X67_Private_Variables
  * @{
  */

/* Touch screen driver structure initialization */
TS_DrvTypeDef ft3x67_ts_drv =
{
  ft3x67_Init,
  ft3x67_ReadID,
  ft3x67_Reset,
  ft3x67_TS_Start,
  ft3x67_TS_DetectTouch,
  ft3x67_TS_GetXY,
  ft3x67_TS_EnableIT,
  ft3x67_TS_ClearIT,
  ft3x67_TS_ITStatus,
  ft3x67_TS_DisableIT
};

/* Global ft3x67 handle */
static ft3x67_handle_TypeDef ft3x67_handle = { FT3X67_I2C_NOT_INITIALIZED, 0U, 0U};

/**
  * @}
  */

/* Private functions prototypes-----------------------------------------------*/

/** @defgroup FT3X67_Private_Functions
  * @{
  */

static uint8_t  ft3x67_Get_I2C_InitializedStatus(void);
static void     ft3x67_I2C_InitializeIfRequired(void);
static uint32_t ft3x67_TS_Configure(uint16_t DeviceAddr);

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup FT3X67_Exported_Functions
  * @{
  */

/**
  * @brief  Initialize the ft3x67 communication bus
  *         from MCU to FT3X67 : ie I2C channel initialization (if required).
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @retval None
  */
void ft3x67_Init(uint16_t DeviceAddr)
{
  /* Initialize I2C link if needed */
  ft3x67_I2C_InitializeIfRequired();
}

/**
  * @brief  Software Reset the ft3x67.
  *         @note : Not applicable to FT3X67.
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @retval None
  */
void ft3x67_Reset(uint16_t DeviceAddr)
{
  /* Do nothing */
  /* No software reset sequence available in FT3X67 IC */
}

/**
  * @brief  Read the ft3x67 device ID, pre initialize I2C in case of need to be
  *         able to read the FT3X67 device ID, and verify this is a FT3X67.
  * @param  DeviceAddr: I2C FT3X67 Slave address.
  * @retval The Device ID (two bytes).
  */
uint16_t ft3x67_ReadID(uint16_t DeviceAddr)
{
  /* Initialize I2C link if needed */
  ft3x67_I2C_InitializeIfRequired();
  
  /* Return the device ID value */
  return(TS_IO_Read(DeviceAddr, FT3X67_CHIP_ID_REG));
}

/**
  * @brief  Configures the touch Screen IC device to start detecting touches
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
  * @retval None.
  */
void ft3x67_TS_Start(uint16_t DeviceAddr)
{
  /* Minimum static configuration of FT3X67 */
  ft3x67_TS_Configure(DeviceAddr);
  
  /* By default set FT3X67 IC in Polling mode : no INT generation on FT3X67 for new touch available */
  /* Note TS_INT is active low                                                                      */
  ft3x67_TS_DisableIT(DeviceAddr);
}

/**
  * @brief  Return if there is touches detected or not.
  *         Try to detect new touches and forget the old ones (reset internal global
  *         variables).
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval : Number of active touches detected (can be 0, 1 or 2).
  */
uint8_t ft3x67_TS_DetectTouch(uint16_t DeviceAddr)
{
  volatile uint8_t nbTouch = 0U;
  
  /* Read register FT3X67_TD_STAT_REG to check number of touches detection */
  nbTouch = TS_IO_Read(DeviceAddr, FT3X67_TD_STAT_REG);
  nbTouch &= FT3X67_TD_STAT_MASK;
  
  if(nbTouch > FT3X67_MAX_DETECTABLE_TOUCH)
  {
    /* If invalid number of touch detected, set it to zero */
    nbTouch = 0U;
  }
  
  /* Update ft3x67 driver internal global : current number of active touches */
  ft3x67_handle.currActiveTouchNb = nbTouch;
  
  /* Reset current active touch index on which to work on */
  ft3x67_handle.currActiveTouchIdx = 0U;
  
  return(nbTouch);
}

/**
  * @brief  Get the touch screen X and Y positions values
  *         Manage multi touch thanks to touch Index global
  *         variable 'ft3x67_handle.currActiveTouchIdx'.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  X: Pointer to X position value
  * @param  Y: Pointer to Y position value
  * @retval None.
  */
void ft3x67_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  uint8_t regAddress = 0U;
  uint8_t dataxy[4U];
  
  if(ft3x67_handle.currActiveTouchIdx < ft3x67_handle.currActiveTouchNb)
  {
    switch(ft3x67_handle.currActiveTouchIdx)
    {
    case 0U :
      regAddress = FT3X67_P1_XH_REG;
      break;
    
    case 1U :
      regAddress = FT3X67_P2_XH_REG;
      break;
    
    default :
      break;
    } /* end switch(ft3x67_handle.currActiveTouchIdx) */
    
    /* Read X and Y positions */
    TS_IO_ReadMultiple(DeviceAddr, regAddress, dataxy, sizeof(dataxy)); 
    
    /* Send back ready X position to caller */
    *X = ((dataxy[0U] & FT3X67_TOUCH_POS_MSB_MASK) << 8U) | dataxy[1U];
    
    /* Send back ready Y position to caller */
    *Y = ((dataxy[2U] & FT3X67_TOUCH_POS_MSB_MASK) << 8U) | dataxy[3U];
    
    /* Increment current touch index */
    ft3x67_handle.currActiveTouchIdx++;
  }
}

/**
  * @brief  Configure the FT3X67 device to generate IT on given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of FT3X67).
  * @retval None
  */
void ft3x67_TS_EnableIT(uint16_t DeviceAddr)
{
  /* Set interrupt trigger mode in FT3X67_GMODE_REG */
  TS_IO_Write(DeviceAddr, FT3X67_GMODE_REG, FT3X67_G_MODE_INTERRUPT_TRIGGER);
}

/**
  * @brief  Configure the FT3X67 device to stop generating IT on the given INT pin
  *         connected to MCU as EXTI.
  * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of FT3X67).
  * @retval None
  */
void ft3x67_TS_DisableIT(uint16_t DeviceAddr)
{
  /* Set interrupt polling mode in FT3X67_GMODE_REG */
  TS_IO_Write(DeviceAddr, FT3X67_GMODE_REG, FT3X67_G_MODE_INTERRUPT_POLLING);
}

/**
  * @brief  Get IT status from FT3X67 interrupt status registers
  *         Should be called Following an EXTI coming to the MCU to know the detailed
  *         reason of the interrupt.
  *         @note : This feature is not applicable to FT3X67.
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @retval TS interrupts status : always return 0 here
  */
uint8_t ft3x67_TS_ITStatus(uint16_t DeviceAddr)
{
  /* Always return 0 as feature not applicable to FT3X67 */
  return 0U;
}

/**
  * @brief  Clear IT status in FT3X67 interrupt status clear registers
  *         Should be called Following an EXTI coming to the MCU.
  *         @note : This feature is not applicable to FT3X67.
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @retval None
  */
void ft3x67_TS_ClearIT(uint16_t DeviceAddr)
{
  /* Nothing to be done here for FT3X67 */
}

/**
  * @brief  Configure gesture feature (enable/disable).
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @param  Activation : Enable or disable gesture feature. Possible values are
  *         FT3X67_GESTURE_DISABLE or FT3X67_GESTURE_ENABLE.
  * @retval None.
  */
void ft3x67_TS_GestureConfig(uint16_t DeviceAddr, uint32_t Activation)
{
  if(Activation == FT3X67_GESTURE_ENABLE)
  {
    /* Enable gesture feature. */
    TS_IO_Write(DeviceAddr, FT3X67_GESTURE_FLAG_REG, FT3X67_GEST_ALL_FLAGS_ENABLE);
    TS_IO_Write(DeviceAddr, FT3X67_GESTURE_ENABLE_REG, FT3X67_GESTURE_ENABLE);
  }
  else
  {
    /* Disable gesture feature. */
    TS_IO_Write(DeviceAddr, FT3X67_GESTURE_FLAG_REG, FT3X67_GEST_ALL_FLAGS_DISABLE);
    TS_IO_Write(DeviceAddr, FT3X67_GESTURE_ENABLE_REG, FT3X67_GESTURE_DISABLE);
  }
}

/**
  * @brief  Get the last touch gesture identification (zoom, move up/down...).
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @param  pGestureId : Pointer to get last touch gesture Identification.
  * @retval None.
  */
void ft3x67_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId)
{
  volatile uint8_t ucReadData = 0U;
  
  ucReadData = TS_IO_Read(DeviceAddr, FT3X67_GEST_ID_REG);
  
  *pGestureId = ucReadData;
}

/**
  * @brief  Get the touch detailed informations on touch number 'touchIdx' (0..1)
  *         This touch detailed information contains :
  *         - weight that was applied to this touch
  *         - sub-area of the touch in the touch panel
  *         - event of linked to the touch (press down, lift up, ...)
  * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT3X67).
  * @param  touchIdx : Passed index of the touch (0..1) on which we want to get the
  *                    detailed information.
  * @param  pWeight : Pointer to to get the weight information of 'touchIdx'.
  * @param  pArea   : Pointer to to get the sub-area information of 'touchIdx'.
  * @param  pEvent  : Pointer to to get the event information of 'touchIdx'.
  * @note   Area and Weight features are not supported by FT3X67. Return always 0 value.
  * @retval None.
  */
void ft3x67_TS_GetTouchInfo(uint16_t   DeviceAddr,
                            uint32_t   touchIdx,
                            uint32_t * pWeight,
                            uint32_t * pArea,
                            uint32_t * pEvent)
{
  volatile uint8_t ucReadData = 0U;
  uint8_t regAddressXHigh     = 0U;
  
  if(touchIdx < ft3x67_handle.currActiveTouchNb)
  {
    switch(touchIdx)
    {
    case 0U :
      regAddressXHigh   = FT3X67_P1_XH_REG;
      break;
    
    case 1U :
      regAddressXHigh   = FT3X67_P2_XH_REG;
      break;
    default :
      break;
    } /* end switch(touchIdx) */
    
    /* Read Event Id of touch index */
    ucReadData = TS_IO_Read(DeviceAddr, regAddressXHigh);
    *pEvent = (ucReadData & FT3X67_TOUCH_EVT_FLAG_MASK) >> FT3X67_TOUCH_EVT_FLAG_SHIFT;
    
    /* Weight and area of touch index not supported by FT3X67 */
    *pWeight = 0;
    *pArea   = 0;
  } /* of if(touchIdx < ft3x67_handle.currActiveTouchNb) */
}

/**
  * @}
  */

/* Private functions bodies---------------------------------------------------*/

/** @addtogroup FT3X67_Private_Functions
  * @{
  */

/**
  * @brief  Return the status of I2C was initialized or not.
  * @param  None.
  * @retval : I2C initialization status.
  */
static uint8_t ft3x67_Get_I2C_InitializedStatus(void)
{
  return(ft3x67_handle.i2cInitialized);
}

/**
  * @brief  I2C initialize if needed.
  * @param  None.
  * @retval : None.
  */
static void ft3x67_I2C_InitializeIfRequired(void)
{
  if(ft3x67_Get_I2C_InitializedStatus() == FT3X67_I2C_NOT_INITIALIZED)
  {
    /* Initialize TS IO BUS layer (I2C) */
    TS_IO_Init();
    
    /* Set state to initialized */
    ft3x67_handle.i2cInitialized = FT3X67_I2C_INITIALIZED;
  }
}

/**
  * @brief  Basic static configuration of TouchScreen
  * @param  DeviceAddr: FT3X67 Device address for communication on I2C Bus.
  * @retval Status FT3X67_STATUS_OK or FT3X67_STATUS_NOT_OK.
  */
static uint32_t ft3x67_TS_Configure(uint16_t DeviceAddr)
{
  uint32_t status = FT3X67_STATUS_OK;
  
  /* Disable gesture feature */
  TS_IO_Write(DeviceAddr, FT3X67_GESTURE_ENABLE_REG, FT3X67_GESTURE_DISABLE);
  
  return(status);
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
