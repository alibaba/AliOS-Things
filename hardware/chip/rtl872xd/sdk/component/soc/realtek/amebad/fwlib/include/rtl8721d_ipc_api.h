/**
  ******************************************************************************
  * @file    rtl8721d_ipc_api.h
  * @author
  * @version V1.0.0
  * @date    2018-06-11
  * @brief   This file contains all the functions prototypes for the IPC API function
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

#ifndef _RTL8721D_IPC_API_H
#define _RTL8721D_IPC_API_H

void ipc_table_init(VOID);
void ipc_send_message(u8 IPC_ChNum, u32 Message);
u32 ipc_get_message(u8 IPC_ChNum);

#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
