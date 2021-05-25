/**
  ******************************************************************************
  * @file    rtl8721d_bootcfg.c
  * @author
  * @version V1.0.0
  * @date    2018-09-12
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities:
  *           - memory layout config
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

/*
* @brif	MMU Configuration. 
*	There are 8 MMU entries totally. Entry 0 & Entry 1 are already used by OTA, Entry 2~7 can be used by Users.
*/
BOOT_RAM_DATA_SECTION
MMU_ConfDef Flash_MMU_Config[] = {
	/*VAddrStart,	VAddrEnd,			PAddrStart,		PAddrEnd*/
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 2
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 3
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 4
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 5
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 6
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},	//Entry 7
	{0xFFFFFFFF,	0xFFFFFFFF,			0xFFFFFFFF,		0xFFFFFFFF},
};

/*
* @brif	OTA start address. Because KM0 & KM4 IMG2 are combined, users only need to set the start address
*	of KM0 IMG2.
*/
BOOT_RAM_DATA_SECTION
u32 OTA_Region[2] = {
	0x08006000,		/* OTA1 region start address */ 
	0x08106000,		/* OTA2 region start address */
};

/*
* @brif	RSIP Mask Configuration.  
*	There are 4 RSIP mask entries totally. Entry 0 is already used by System Data, Entry 3 is reserved by Realtek. 
*	Only Entry 1 & Entry 2 can be used by Users.
*	MaskAddr: start address for RSIP Mask, should be 4KB aligned
*	MaskSize: size of the mask area, unit is 4KB
*/
BOOT_RAM_DATA_SECTION
RSIP_MaskDef RSIP_Mask_Config[] = {
	/*MaskAddr,		MaskSize*/
	{0x08001000,	3},  		//Entry 0: 4K system data & 4K backup & DPK

	/* customer can set here */
	{0xFFFFFFFF,	0xFFFF},  	//Entry 1: can be used by users
	{0xFFFFFFFF,	0xFFFF}, 	//Entry 2: can be used by users
	{0xFFFFFFFF, 	0xFFFF}, 	//Entry 3: Reserved by Realtek. If RDP is not used, this entry can be used by users.

	/* End */
	{0xFFFFFFFF, 	0xFFFF},
};

/**
* @brif  GPIO force OTA1 as image2. 0xFF means force OTA1 trigger is disabled.
* 	BIT[7]: active level, 0 is low level active, 1 is high level active, 
*	BIT[6:5]: port, 0 is PORT_A, 1 is PORT_B 
*	BIT[4:0]: pin num 0~31
*/
BOOT_RAM_DATA_SECTION
u8 Force_OTA1_GPIO = 0xFF;

/**
* @brif  boot log enable or disable.
* 	FALSE: disable 
*	TRUE: enable 
*/
BOOT_RAM_DATA_SECTION
u8 Boot_Log_En = FALSE;


/**
* @brif  Firmware verify callback handler.
*	If users need to verify checksum/hash for image2, implement the function and assign the address
*	to this function pointer.
* @param  None
* @retval 1: Firmware verify successfully, 0: verify failed
*/
BOOT_RAM_DATA_SECTION
FuncPtr FwCheckCallback = NULL;

/**
* @brif  Firmware select hook.
*	If users need to implement own OTA select method, implement the function and assign the address
*	to this function pointer.
* @param  None
* @retval 0: both firmwares are invalid, select none, 1: boot from OTA1, 2: boot from OTA2
*/
BOOT_RAM_DATA_SECTION
FuncPtr OTASelectHook = NULL;


