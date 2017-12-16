/**
  ******************************************************************************
  * @file    stm32l496g_discovery_idd.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage the
  *          Idd measurement driver for STM32L496G-Discovery board.
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
#include "stm32l496g_discovery_idd.h"
#include "stm32l496g_discovery_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_IDD STM32L496G-DISCOVERY IDD
  * @brief This file includes the Idd driver for STM32L496G-DISCOVERY board.
  *        It allows user to measure MCU Idd current on board, especially in
  *        different low power modes.
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_IDD_Private_Defines Private Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup STM32L496G_DISCOVERY_IDD_Private_Variables Private Variables
  * @{
  */
static IDD_DrvTypeDef *IddDrv;

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_IDD_Private_Functions Private Functions
  * @{
  */

/**
  * @}
  */

/** @defgroup STM32L496G_DISCOVERY_IDD_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Configures IDD measurement component.
  * @retval IDD_OK if no problem during initialization
  */
uint8_t BSP_IDD_Init(void)
{
  IDD_ConfigTypeDef iddconfig = {0};
  uint8_t mfxstm32l152_id = 0;
  uint8_t ret = 0;

  /* wake up mfx component in case it went to standby mode */
  mfxstm32l152_idd_drv.WakeUp(IDD_I2C_ADDRESS);
  HAL_Delay(5);

  /* Read ID and verify if the MFX is ready */
  mfxstm32l152_id = mfxstm32l152_idd_drv.ReadID(IDD_I2C_ADDRESS);

  if ((mfxstm32l152_id == MFXSTM32L152_ID_1) || (mfxstm32l152_id == MFXSTM32L152_ID_2))
  {
    /* Initialize the Idd driver structure */
    IddDrv = &mfxstm32l152_idd_drv;

    /* Initialize the Idd driver */
    if (IddDrv->Init != NULL)
    {
      IddDrv->Init(IDD_I2C_ADDRESS);
    }

    /* Configure Idd component with default values */
    iddconfig.AmpliGain = DISCOVERY_IDD_AMPLI_GAIN;
    iddconfig.VddMin = DISCOVERY_IDD_VDD_MIN;
    iddconfig.Shunt0Value = DISCOVERY_IDD_SHUNT0_VALUE;
    iddconfig.Shunt1Value = DISCOVERY_IDD_SHUNT1_VALUE;
    iddconfig.Shunt2Value = DISCOVERY_IDD_SHUNT2_VALUE;
    iddconfig.Shunt3Value = 0;
    iddconfig.Shunt4Value = DISCOVERY_IDD_SHUNT4_VALUE;
    iddconfig.Shunt0StabDelay = DISCOVERY_IDD_SHUNT0_STABDELAY;
    iddconfig.Shunt1StabDelay = DISCOVERY_IDD_SHUNT1_STABDELAY;
    iddconfig.Shunt2StabDelay = DISCOVERY_IDD_SHUNT2_STABDELAY;
    iddconfig.Shunt3StabDelay = 0;
    iddconfig.Shunt4StabDelay = DISCOVERY_IDD_SHUNT4_STABDELAY;
    iddconfig.ShuntNbOnBoard = MFXSTM32L152_IDD_SHUNT_NB_4;
    iddconfig.ShuntNbUsed = MFXSTM32L152_IDD_SHUNT_NB_4;
    iddconfig.VrefMeasurement = MFXSTM32L152_IDD_VREF_AUTO_MEASUREMENT_ENABLE;
    iddconfig.Calibration = MFXSTM32L152_IDD_AUTO_CALIBRATION_ENABLE;
    iddconfig.PreDelayUnit = MFXSTM32L152_IDD_PREDELAY_20_MS;
    iddconfig.PreDelayValue = 0x7F;
    iddconfig.MeasureNb = 100;
    iddconfig.DeltaDelayUnit = MFXSTM32L152_IDD_DELTADELAY_0_5_MS;
    iddconfig.DeltaDelayValue = 10;
    BSP_IDD_Config(iddconfig);

    ret = IDD_OK;
  }
  else
  {
    ret = IDD_ERROR;
  }

  return ret;
}

/**
  * @brief  Unconfigures IDD measurement component.
  * @retval IDD_OK if no problem during deinitialization
  */
void BSP_IDD_DeInit(void)
{
  if (IddDrv->DeInit != NULL)
  {
    IddDrv->DeInit(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Reset Idd measurement component.
  * @retval None
  */
void BSP_IDD_Reset(void)
{
  if (IddDrv->Reset != NULL)
  {
    IddDrv->Reset(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Turn Idd measurement component in low power (standby/sleep) mode
  * @retval None
  */
void BSP_IDD_LowPower(void)
{
  if (IddDrv->LowPower != NULL)
  {
    IddDrv->LowPower(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Start Measurement campaign
  * @retval None
  */
void BSP_IDD_StartMeasure(void)
{

  /* Activate the OPAMP used ny the MFX to measure the current consumption */
  BSP_IO_ConfigPin(IDD_AMP_CONTROL_PIN, IO_MODE_OUTPUT);
  BSP_IO_WritePin(IDD_AMP_CONTROL_PIN, GPIO_PIN_RESET);

  if (IddDrv->Start != NULL)
  {
    IddDrv->Start(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Configure Idd component
  * @param  IddConfig: structure of idd parameters
  * @retval None
  */
void BSP_IDD_Config(IDD_ConfigTypeDef IddConfig)
{
  if (IddDrv->Config != NULL)
  {
    IddDrv->Config(IDD_I2C_ADDRESS, IddConfig);
  }
}

/**
  * @brief  Get Idd current value.
  * @param  IddValue: Pointer on u32 to store Idd. Value unit is 10 nA.
  * @retval None
  */
void BSP_IDD_GetValue(uint32_t *IddValue)
{
  /* De-activate the OPAMP used ny the MFX to measure the current consumption */
  BSP_IO_ConfigPin(IDD_AMP_CONTROL_PIN, IO_MODE_OUTPUT);
  BSP_IO_WritePin(IDD_AMP_CONTROL_PIN, GPIO_PIN_RESET);

  if (IddDrv->GetValue != NULL)
  {
    IddDrv->GetValue(IDD_I2C_ADDRESS, IddValue);
  }
}

/**
  * @brief  Enable Idd interrupt that warn end of measurement
  * @retval None
  */
void BSP_IDD_EnableIT(void)
{
  if (IddDrv->EnableIT != NULL)
  {
    IddDrv->EnableIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Clear Idd interrupt that warn end of measurement
  * @retval None
  */
void BSP_IDD_ClearIT(void)
{
  if (IddDrv->ClearIT != NULL)
  {
    IddDrv->ClearIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Get Idd interrupt status
  * @retval status
  */
uint8_t BSP_IDD_GetITStatus(void)
{
  if (IddDrv->GetITStatus != NULL)
  {
    return (IddDrv->GetITStatus(IDD_I2C_ADDRESS));
  }
  else
  {
    return IDD_ERROR;
  }
}

/**
  * @brief  Disable Idd interrupt that warn end of measurement
  * @retval None
  */
void BSP_IDD_DisableIT(void)
{
  if (IddDrv->DisableIT != NULL)
  {
    IddDrv->DisableIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Get Error Code .
  * @retval Error code or error status
  */
uint8_t BSP_IDD_ErrorGetCode(void)
{
  if (IddDrv->ErrorGetSrc != NULL)
  {
    if ((IddDrv->ErrorGetSrc(IDD_I2C_ADDRESS) & MFXSTM32L152_IDD_ERROR_SRC) != RESET)
    {
      if (IddDrv->ErrorGetCode != NULL)
      {
        return IddDrv->ErrorGetCode(IDD_I2C_ADDRESS);
      }
      else
      {
        return IDD_ERROR;
      }
    }
    else
    {
      return IDD_ERROR;
    }
  }
  else
  {
    return IDD_ERROR;
  }
}


/**
  * @brief  Enable error interrupt that warn end of measurement
  * @retval None
  */
void BSP_IDD_ErrorEnableIT(void)
{
  if (IddDrv->ErrorEnableIT != NULL)
  {
    IddDrv->ErrorEnableIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Clear Error interrupt that warn end of measurement
  * @retval None
  */
void BSP_IDD_ErrorClearIT(void)
{
  if (IddDrv->ErrorClearIT != NULL)
  {
    IddDrv->ErrorClearIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Get Error interrupt status
  * @retval Status
  */
uint8_t BSP_IDD_ErrorGetITStatus(void)
{
  if (IddDrv->ErrorGetITStatus != NULL)
  {
    return (IddDrv->ErrorGetITStatus(IDD_I2C_ADDRESS));
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Disable Error interrupt
  * @retval None
  */
void BSP_IDD_ErrorDisableIT(void)
{
  if (IddDrv->ErrorDisableIT != NULL)
  {
    IddDrv->ErrorDisableIT(IDD_I2C_ADDRESS);
  }
}

/**
  * @brief  Wake up Idd measurement component.
  * @retval None
  */
void BSP_IDD_WakeUp(void)
{
  if (IddDrv->WakeUp != NULL)
  {
    IddDrv->WakeUp(IDD_I2C_ADDRESS);
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

