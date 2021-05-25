/**
  ******************************************************************************
  * @file    rtl8721dlp_flashcfg.c
  * @author
  * @version V1.0.0
  * @date    2018-09-12
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities:
  *           - flash function config
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

void flash_init_userdef(void);

/**
* @brif Indicate the flash baudrate. It can be one of the following value:
*	0xFFFF: 80MHz
*	0x7FFF: 100MHz
*	0x3FFF: 67MHz
*	0x1FFF: 57MHz
*	0x0FFF: 50MHz
*/
const u16 Flash_Speed = 0xFFFF;

/**
* @brif Indicate the flash read I/O mode. It can be one of the following value:
*	0xFFFF: Read quad IO, Address & Data 4 bits mode
*	0x7FFF: Read quad O, Just data 4 bits mode
*	0x3FFF: Read dual IO, Address & Data 2 bits mode
*	0x1FFF: Read dual O, Just data 2 bits mode
*	0x0FFF: 1 bit mode
* @note If the configured read mode is not supported, other modes would be searched until find the appropriate mode.
*/
const u16 Flash_ReadMode = 0xFFFF;


/**
* @brif Flash_AVL maintains the flash IC supported by SDK.
*	If users want to adpot new flash, add item in the following AVL.
*	Note that, if new flash can be classfied as one of the Realtek-defined categories according to Classification SPEC, 
*	filling in the defined class index is necessary. Otherwise, FlashClassUser can be used to indicate new class.
*/
const FlashInfo_TypeDef Flash_AVL[] = {
		/*flash_id,		flash_id_mask,	flash_class,		status_mask,	FlashInitHandler */
		
		/* case1: Realtek defined class, any modification is not allowed */
		{0xEF,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* Winbond: MANUFACTURER_ID_WINBOND */
		{0xA1,			0x000000FF,		FlashClass1,		0x0000FFFC,		NULL}, 	/* Fudan Micro: MANUFACTURER_ID_FM */
		{0x0B,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX */
		{0x0E,			0x000000FF,		FlashClass1,		0x000043FC,		NULL},	/* XTX(FT) */
		{0xC8,			0x000000FF,		FlashClass2,		0x000043FC,		NULL},	/* GD normal: MANUFACTURER_ID_GD */
		{0x28C2,		0x0000FFFF,		FlashClass6,		0x000200FC,		NULL},	/* MXIC wide-range VCC: MANUFACTURER_ID_MXIC */
		{0xC2,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* MXIC normal: MANUFACTURER_ID_BOHONG */
		{0x68,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* Hua Hong */
		{0x51,			0x000000FF,		FlashClass3,		0x000000FC,		NULL},	/* GD MD serial */
		{0x1C,			0x000000FF,		FlashClass4,		0x000000FC,		NULL},	/* ESMT: MANUFACTURER_ID_EON */
		{0x20,			0x000000FF,		FlashClass5,		0x000000FC,		NULL},	/* Microm: MANUFACTURER_ID_MICRON */

		/* case2: new flash, ID is not included in case1 list, but specification is compatible with FlashClass1~FlashClass6 */
		//{0xXX,			0x0000XXXX,		FlashClassX,		0x0000XXXX,		NULL},
		
		/* case3: new flash, ID is not included in case1 list, and specification is not compatible with FlashClass1~FlashClass6 */
		{0x00,			0x000000FF,		FlashClassUser,		0xFFFFFFFF,		&flash_init_userdef},

		/* End */
		{0xFF,			0xFFFFFFFF,		FlashClassNone,		0xFFFFFFFF,		NULL},
};

/**
 * @brief  To adopt user defined class, need to initialize the parameters in the function below
 *       with its default values according to flash Spec, and add additional config code if need.
 * @param none.
 * @retval none
 * @note	The following parameter setting is an example of MXIC flash. For users reference.
 */
void flash_init_userdef(void)
{
	FLASH_InitTypeDef* FLASH_InitStruct = &flash_init_para;

	/*1. Initialize CMD parameters */
	
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;

	/*1.1 status bit define */
	FLASH_InitStruct->FLASH_QuadEn_bit = BIT(6);
	FLASH_InitStruct->FLASH_Busy_bit = BIT(0);
	FLASH_InitStruct->FLASH_WLE_bit = BIT(1);
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/*1.2 dummy cycle */
	FLASH_InitStruct->FLASH_rd_dummy_cyle[0] = 0;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[1] = 0x04;
	FLASH_InitStruct->FLASH_rd_dummy_cyle[2] = 0x06;

	/*1.3 set 2bit mode cmd */
	FLASH_InitStruct->FLASH_rd_dual_io = 0xBB;
	FLASH_InitStruct->FLASH_rd_dual_o = 0x3B;

	/*1.4 set 4bit mode cmd */
	FLASH_InitStruct->FLASH_rd_quad_io = 0xEB;
	FLASH_InitStruct->FLASH_rd_quad_o = 0x6B;

	/*1.5 other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = 0x06;
	FLASH_InitStruct->FLASH_cmd_rd_id = 0x9F;
	FLASH_InitStruct->FLASH_cmd_rd_status = 0x05;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = 0x01;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0x60;
	FLASH_InitStruct->FLASH_cmd_block_e = 0xD8;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0x20;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0xAB;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0xB9;

	/*2. Add other configuration code here if needed for flash */
	//TODO
	
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
