/**
  ******************************************************************************
  * @file    rtl8711b_flashclk.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the Flash Clock firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_FLASH_CLK_H
#define _RTL8710B_FLASH_CLK_H

/* Other definations --------------------------------------------------------*/
typedef struct {
	u8 phase_int;
	u8 phase_frac;
} FLASH_CLK_Phase;

typedef struct {
	u16 div_sel;
	u8 div_int;
	u8 div_frac;
} FLASH_CLK_Div;

extern u8 NEW_CALIBREATION_END[];
#endif  //_RTL8710B_FLASH_CLK_H
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
