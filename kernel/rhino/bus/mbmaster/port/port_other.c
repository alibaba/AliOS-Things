/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <mbmaster_api.h>
#include "port.h"

void mb_log( mb_log_level_t log_level, const int8_t * fmt_str, ... )
{
    va_list param;

    if(log_level <= MB_LOG_LEVEL)
    {
        va_start(param,fmt_str);
        vprintf(fmt_str,param);
        va_end(param);
    }
}

uint8_t* status_to_string(mb_exception_t status)
{
    const int8_t *status_string = NULL;
    switch(status) {
        case MB_EX_RESPOND_ERROR:
            status_string = "Transmission error";
            break;
        case MB_EX_RESPOND_TIMEOUT:
            status_string = "slave no respond";
            break;
        case MB_EX_RESPOND_EXCEPTION:
            status_string = "slave respond EXCEPTION";
            break;
        case MB_EX_MB_BUSY:
            status_string = "modbus is busy,try again later";
            break;
        case MB_EX_ILLEGAL_SLAVE_ADDRESS:
            status_string = "slave address is invalid";
            break;
        case MB_EX_INVALID_PARAM:
            status_string = "invalid parameter";
            break;
        case MB_EX_NONE:
            status_string = "ok";
            break;
    }
    return status_string;
}

