/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __AMP_MACHINE_CONFIG_H__
#define __AMP_MACHINE_CONFIG_H__

/* python engine root dir define */
#define MP_FS_ROOT_DIR                 "/data"
#define MP_FS_EXT_ROOT_DIR             "/sdcard"

#define AMP_FS_ROOT_DIR                MP_FS_ROOT_DIR "/pyamp"
#define AMP_FS_EXT_ROOT_DIR            MP_FS_EXT_ROOT_DIR "/pyamp"

#define MP_REPL_UART_PORT              0
#define MP_REPL_UART_BAUDRATE          1500000UL

#define MP_RECOVERY_UART_PORT          0
#define MP_RECOVERY_UART_PORT_BAUDRATE 1500000UL

#endif  // __AMP_MACHINE_CONFIG_H__
