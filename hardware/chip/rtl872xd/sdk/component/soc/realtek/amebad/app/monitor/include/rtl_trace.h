/**
  ******************************************************************************
  * @file    rtl_trace.h
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

u32 LOG_PRINTF_BUFFER(const char *fmt);
u32 LOG_PRINTF_BUFFER_INIT(u32 thread_init);
u32 LOG_BUFF_SUSPEND(void);
u32 LOG_BUFF_RESUME(void);

#define LOG_BUFFER_NUM					6
#define LOG_BUFFER_SIZE				512
typedef struct {
	char buffer[LOG_BUFFER_SIZE];
	/* please define member after buffer */
} log_buffer_t;
extern log_buffer_t log_buffer[];

typedef u32 (*DIAG_PRINT_BUF_FUNC)(const char *fmt);

extern DIAG_PRINT_BUF_FUNC ConfigDebugBufferGet;
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
