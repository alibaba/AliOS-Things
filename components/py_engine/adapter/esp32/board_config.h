/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __AMP_MACHINE_CONFIG_H__
#define __AMP_MACHINE_CONFIG_H__

/* python engine root dir define */
#define MP_PY_PATH     "main.py"
#define MP_FS_IDF_DIR  "/"

#define MP_FS_ROOT_DIR "/data"
#define MP_FS_EXT_ROOT_DIR "/sdcard"

#define AMP_FS_ROOT_DIR MP_FS_ROOT_DIR "/pyamp"
#define AMP_FS_EXT_ROOT_DIR MP_FS_EXT_ROOT_DIR "/pyamp"

#define AMP_PY_ENTRY_HAAS  (AMP_FS_ROOT_DIR "/" MP_PY_PATH)
#define AMP_PY_ENTRY_ROOT  "/main.py"

#define MP_REPL_UART_PORT 0
#define MP_REPL_UART_BAUDRATE 115200UL

#define MP_RECOVERY_UART_PORT 0
#define MP_RECOVERY_UART_PORT_BAUDRATE 115200UL

#endif // __AMP_MACHINE_CONFIG_H__
