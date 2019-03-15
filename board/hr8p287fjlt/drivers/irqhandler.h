/**
  ******************************************************************************
  * @file    irqhandler.h
  * @brief   Handler Head File.
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

#ifndef __IRQHANDLER_H__
#define __IRQHANDLER_H__

/* Includes -------------------------------------------------------------------*/
#include "HR8P296.h"

/* Private types --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private constants ----------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Public constants -----------------------------------------------------------*/
/* Public types ---------------------------------------------------------------*/

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif

/******************* (C) COPYRIGHT Eastsoft Microelectronics *****END OF FILE****/
