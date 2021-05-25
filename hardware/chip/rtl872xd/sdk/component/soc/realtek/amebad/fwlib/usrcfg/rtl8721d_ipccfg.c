/**
  ******************************************************************************
  * @file    rtl8721d_ipccfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides configurations for KM0 or KM4 IPC init
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

#if defined (ARM_CORE_CM4)
const IPC_INIT_TABLE  ipc_init_config[] =
{
	//USER_MSG_TYPE		IRQFUNC								IRQDATA
	{IPC_USER_DATA,		shell_switch_ipc_int,		(VOID*) NULL},//channel 0: IPC_INT_CHAN_SHELL_SWITCH
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 1: IPC_INT_CHAN_WIFI_FW
	{IPC_USER_DATA,		FLASH_Write_IPC_Int,		(VOID*) NULL},//channel 2: IPC_INT_CHAN_FLASHPG_REQ
	{IPC_USER_POINT,	NULL,						(VOID*) NULL},//channel 3: IPC_INT_KM4_TICKLESS_INDICATION
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 4: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 5: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 6: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 7: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 8: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 9: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 10: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 11: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 12: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 13: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 14: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 15: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 16: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 17: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 18: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 19: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 20: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 21: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 22: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 23: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 24: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 25: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 26: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 27: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 28: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 29: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 30: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 31: Reserved for Customer use

	{0xFFFFFFFF,		OFF,								OFF},	/* Table end */
};
#else
#if CONFIG_WIFI_FW_EN
extern void driver_fw_flow_ipc_int(VOID *Data, u32 IrqStatus, u32 ChanNum);
#define fw_flow_ipc_int driver_fw_flow_ipc_int
#else
#define fw_flow_ipc_int NULL
#endif
const IPC_INIT_TABLE  ipc_init_config[] =
{
	//USER_MSG_TYPE		IRQFUNC								IRQDATA
	{IPC_USER_DATA,		shell_switch_ipc_int,		(VOID*) NULL},//channel 0: IPC_INT_CHAN_SHELL_SWITCH
	{IPC_USER_DATA,		fw_flow_ipc_int,				(VOID*) IPCM4_DEV},//channel 1: IPC_INT_CHAN_WIFI_FW
	{IPC_USER_DATA,		FLASH_Write_IPC_Int,		(VOID*) NULL},//channel 2: IPC_INT_CHAN_FLASHPG_REQ
	{IPC_USER_POINT,	km4_tickless_ipc_int,		(VOID*) NULL},//channel 3: IPC_INT_KM4_TICKLESS_INDICATION
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 4: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 5: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 6: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 7: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 8: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 9: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 10: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 11: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 12: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 13: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 14: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 15: Reserved for Realtek use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 16: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 17: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 18: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 19: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 20: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 21: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 22: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 23: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 24: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 25: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 26: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 27: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 28: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 29: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 30: Reserved for Customer use
	{IPC_USER_DATA,		NULL,						(VOID*) NULL},//channel 31: Reserved for Customer use

	{0xFFFFFFFF,		OFF,								OFF},	/* Table end */
};
#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

