/**
  ******************************************************************************
  * @file    board.h
  * @brief   Hardware Initialization Head File.
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

#ifndef __BOARD_H
#define __BOARD_H

/* Includes -------------------------------------------------------------------*/ 
/* Private types --------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------*/
/* Private constants ----------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
/* Public constants -----------------------------------------------------------*/
/* Public types ---------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT Eastsoft Microelectronics *****END OF FILE****/
