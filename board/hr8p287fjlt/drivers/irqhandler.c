/**
  ******************************************************************************
  * @file    irqhandler.c
  * @brief   Handler Source File.
  *
  * @version V0.01
  * @data    2018-7-4
  * @author  Eastsoft AE Team
  * @note
  * 
  *
  * Copyright (C) 2018 Shanghai Eastsoft Microelectronics Co., Ltd. ALL rights reserved.
  *******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "irqhandler.h"
#include "k_api.h"

/* Private constants ----------------------------------------------------------*/
/* Private types --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------*/
/* Public functions -----------------------------------------------------------*/
/**
  * @brief  NMI_IRQHandler.
  * @param  None
  * @retval None
  */
void NMI_IRQHandler(void)
{

}

/**
  * @brief  HardFault_IRQHandler.
  * @param  None
  * @retval None
  */
void HardFault_IRQHandler(void)
{
    while(1)
    {
    }
}

/**
  * @brief  SVC_IRQHandler.
  * @param  None
  * @retval None
  */
void SVC_IRQHandler(void)
{
    
}

/**
  * @brief  SysTick_IRQHandler.
  * @param  None
  * @retval None
  */
void SysTick_IRQHandler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();    
}

/******************* (C) COPYRIGHT Eastsoft Microelectronics *****END OF FILE****/
