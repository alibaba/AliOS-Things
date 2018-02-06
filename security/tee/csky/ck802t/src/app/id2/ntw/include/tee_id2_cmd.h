/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_ID2_CMD_H_
#define _TEE_ID2_CMD_H_
#include "tee_dbg.h"

#define TEE_ID2_GET_ID         (0x0)
#define TEE_ID2_RSA_SIGN       (0x1)
#define TEE_ID2_RSA_VERIFY     (0x2)
#define TEE_ID2_RSA_PUBK_ENC   (0x3)
#define TEE_ID2_RSA_PRVK_DEC   (0x4)
#define TEE_ID2_CIPHER         (0x5)

#define ID_LEN                 (24)
#define SIGN_LEN               (128)

#define ID2_SRV_UUID    {                               \
    0x5445455f, 0x4944, 0x325f,                         \
    { 0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x49, 0x44 }  \
}

#define ID2_DBG_E(_f, _a ...)  tee_dbg_print(ERR, _f, ##_a)
#define ID2_DBG_I(_f, _a ...)  tee_dbg_print(INF, _f, ##_a)

#define ID2_DBG_E_STRING(_str)  uart_print_string(_str)
#define ID2_DBG_I_STRING(_str)  uart_print_string(_str)

#define ID2_DBG_E_DATA(_data)       \
    do {                            \
        uart_print_uint32(_data);   \
        uart_print_string("\n");    \
    } while (0)

#define ID2_DBG_I_DATA(_data)       \
    do {                            \
        uart_print_uint32(_data);   \
        uart_print_string("\n");    \
    } while (0)

#define ID2_DBG_I_DUMP_DATA8(_data, _cnt)  \
    do {    \
            {   \
            int __tmp__i = 0;   \
            for (__tmp__i = 0; __tmp__i < (int)(_cnt); __tmp__i++) {    \
                uart_print_uint8(_data[__tmp__i]);  \
                    uart_print_string(" ");    \
                if (7 == (__tmp__i & 7)) {  \
                    uart_print_string("\n");    \
                }   \
            }   \
            uart_print_string("\n");    \
            }   \
    } while (0)

#endif /* _TEE_ID2_CMD_H_ */
