/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     user_cmd.h
* @brief    Define user command.
* @details
* @author   jane
* @date     2016-02-18
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _USER_CMD_H_
#define _USER_CMD_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if CONFIG_BT_USER_COMMAND

#include <data_uart.h>
#include <user_cmd_parse.h>

extern const T_USER_CMD_TABLE_ENTRY user_cmd_table[];
extern T_USER_CMD_IF    user_cmd_if;

#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

