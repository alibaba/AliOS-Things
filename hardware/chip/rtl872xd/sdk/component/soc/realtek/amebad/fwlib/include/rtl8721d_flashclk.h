/**
  ******************************************************************************
  * @file    rtl8721d_flashclk.h
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
typedef enum {
	FlashClass1 = 0,
	FlashClass2,
	FlashClass3,
	FlashClass4,
	FlashClass5,
	FlashClass6,
	FlashClassUser = 0xFE,
	FlashClassNone = 0xFF,
} FlashClass;

typedef struct {
	u32 flash_id;
	u32 id_mask;
	u8 flash_class;
	u32 sta_mask;
	VOID (*FlashInitHandler) (VOID);
} FlashInfo_TypeDef;

typedef struct {
	u8 phase_int;
	u8 phase_frac;
} FLASH_CLK_Phase;

typedef struct {
	u8 div_int;
	u8 div_frac;
} FLASH_CLK_Div;

extern u8 NEW_CALIBREATION_END[];
#endif  //_RTL8710B_FLASH_CLK_H
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
