/**
  ******************************************************************************
  * @file    rtl_trace.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions for log print and mask.
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

SRAM_NOCACHE_DATA_SECTION u32 log_buffer_start;
SRAM_NOCACHE_DATA_SECTION u32 log_buffer_end;
SRAM_NOCACHE_DATA_SECTION TaskHandle_t log_buffer_thread_handle;

#if defined (ARM_CORE_CM4)
u32 LOG_PRINTF_BUFFER(const char *fmt)
{
	/* To avoid gcc warnings */
	( void ) fmt;
	
	IPC_TypeDef *IPC0 = IPCM0_DEV;
	u32 write_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP];
	u32 read_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP];
	u32 write_pointer_next = 0;

	write_pointer_next = write_pointer + sizeof(log_buffer_t);
	if (write_pointer_next > log_buffer_end) {
		write_pointer_next = log_buffer_start;
	}
	
	/* write pointer catch up with read pointer, wait */
	while (1) {
		read_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP];
		
		if (write_pointer_next != read_pointer)
			break;
	}

	/* make sure write pointer buffer is empty */
	IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP] = write_pointer_next;	

	//DiagPrintfD("LOG_PRINTF_BUFFER: %x\n", IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP]);

	return (write_pointer);	
}

u32 LOG_PRINTF_BUFFER_INIT(u32 thread_init)
{
	/* To avoid gcc wanrings */
	( void ) thread_init;
	
	IPC_TypeDef *IPC0 = IPCM0_DEV;

	log_buffer_start = IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP];

	if (log_buffer_start == 0) {
		DBG_8195A("KM0 dont open logbuf \n");
		return 0;
	}
	
	log_buffer_end = log_buffer_start + (LOG_BUFFER_NUM - 1) * sizeof(log_buffer_t);
	ConfigDebugBufferGet = LOG_PRINTF_BUFFER;
	
	DBG_8195A("LOG_PRINTF_BUFFER_INIT KM4 %x %x\n", log_buffer_start, log_buffer_end);

	/* open/close in CmdLogBuf */	
	ConfigDebugBuffer = 1;
	
	return 0;
}
#else
/* reserve one to avoid memory overflow */
SRAM_NOCACHE_DATA_SECTION log_buffer_t log_buffer[LOG_BUFFER_NUM + 1];
static VOID LOG_BUFF_TASK(VOID *Data)
{
	/* To avoid gcc warnings */
	( void ) Data;

	IPC_TypeDef *IPC0 = IPCM0_DEV;
	u32 write_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP];
	u32 read_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP];
	
	do {
		//DBG_8195A("write_pointer:%x read_pointer:%x \n", IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP],
		//	IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP]);

		/* read pointer catch up with write pointer, wait */
		while (1) {
			write_pointer = IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP];

			if ((write_pointer != read_pointer) && (write_pointer != 0))
				break;
	
			taskYIELD();
		}
		DiagPrintf((const char *)read_pointer);

		read_pointer = (read_pointer + sizeof(log_buffer_t));
		if (read_pointer > log_buffer_end) {
			read_pointer = log_buffer_start;
		}

		IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP] = read_pointer;
	} while(1);
}

u32 LOG_PRINTF_BUFFER_INIT(u32 thread_init)
{
	IPC_TypeDef *IPC0 = IPCM0_DEV;

	log_buffer_thread_handle = NULL;
	
	return 0;
	
	/* init read/write pointer */	
	IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP] = (u32)&log_buffer[0]; /* write_pointer */
	IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP] = (u32)&log_buffer[0]; /* read_pointer */
	log_buffer_end = (u32)&log_buffer[LOG_BUFFER_NUM-1];
	log_buffer_start = (u32)&log_buffer[0];
	log_buffer[0].buffer[0]='\0';

	DBG_8195A("LOG_PRINTF_BUFFER_INIT %x %x %x\n", IPC0->IPCx_USR[IPC_USER_BUF_LOG_WP], \
		IPC0->IPCx_USR[IPC_USER_BUF_LOG_RP], log_buffer_end);

	if (!thread_init)
		return 0;

	/* init thread, low-priority */
	if (pdTRUE != xTaskCreate(LOG_BUFF_TASK, (const char * const)"LOGBUFF_TASK", 128, 
		NULL, tskIDLE_PRIORITY + 3 , &log_buffer_thread_handle))
	{
		DiagPrintf("CreateLOGBUFF_TASK Err!!\n");
	}

	return 0;
}

u32 LOG_BUFF_SUSPEND(void)
{
	if (log_buffer_thread_handle) {
		vTaskSuspend(log_buffer_thread_handle);
	}
	return TRUE;
}

u32 LOG_BUFF_RESUME(void)
{
	if (log_buffer_thread_handle) {
		vTaskResume(log_buffer_thread_handle);
	}
	return TRUE;
}
#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
