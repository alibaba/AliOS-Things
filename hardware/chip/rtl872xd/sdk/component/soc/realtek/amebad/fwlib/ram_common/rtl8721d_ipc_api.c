/**
  ******************************************************************************
  * @file    rtl8721d_ipc_api.c
  * @author
  * @version V1.0.0
  * @date    2018-06-11
  * @brief   This file contains all the API for the IPC function
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

#include "ameba_soc.h"

/**
  * @brief  init ipc channel according to ipc_init_config[].
  * @retval   None
  */
void ipc_table_init(VOID)
{
	u32 i;

	for (i = 0;;) {
		/*  Check if search to end */
		if (ipc_init_config[i].USER_MSG_TYPE == 0xFFFFFFFF) {
			break;
		}

		if((ipc_init_config[i].func != NULL) || (ipc_init_config[i].IrqData != NULL)) {
			IPC_INTUserHandler(i, (void*)ipc_init_config[i].func, ipc_init_config[i].IrqData);
		}

		i++;
	}
}

/**
  * @brief  exchange messages between KM0 and KM4.
  * @param  IPC_ChNum: the IPC channel number.
   * @param  Message: pointer to the message to be exchanged,and should not stored in stack.
  * @retval   None
  */
void ipc_send_message(u8 IPC_ChNum, u32 Message)
{
	u32 CpuId = IPC_CPUID();
	IPC_TypeDef *IPC_DEV;
	
	if(CpuId == 1) 
		IPC_DEV = IPCM4_DEV;
	else if(CpuId == 0) 
		IPC_DEV = IPCM0_DEV;

	if(IPC_USER_POINT == ipc_init_config[IPC_ChNum].USER_MSG_TYPE) {
		/*message is shared between two cpu ,so it can't store in stack
		 * assume stack down growth, and 0x100 is an estimated value
		 */
		//alios todo
		//assert_param((Message > (u32)(vTaskStackAddr() + vTaskStackSize()*4)) || (Message < (u32)vTaskStackAddr()));
	}

	IPC_DEV->IPCx_USR[IPC_ChNum] = (u32)Message;
	IPC_INTRequest(IPC_DEV, IPC_ChNum);
}

/**
  * @brief  get ipc message.
  * @param  IPC_ChNum: the IPC channel number.
  * @retval  : pointer to the message to be exchanged.
  * @note for data massage, corresponding data cache should be invalidate before access.
  */
u32 ipc_get_message(u8 IPC_ChNum)
{
	u32 CpuId = IPC_CPUID();
	IPC_TypeDef *IPC_DEV;

	/*KM4 read mesaage for km0*/
	if(CpuId == 1) 
		IPC_DEV = IPCM0_DEV;
	else if(CpuId == 0) 
		IPC_DEV = IPCM4_DEV;

	return IPC_DEV->IPCx_USR[IPC_ChNum];
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
