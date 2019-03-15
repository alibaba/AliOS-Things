/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <mbmaster_api.h>

void mb_log(mb_log_level_t log_level, const char *fmt_str, ...)
{
    va_list param;

    if (log_level <= MBMASTER_CONFIG_LOG_LEVEL) {
        va_start(param,fmt_str);
        vprintf(fmt_str,param);
        va_end(param);
    }
}

uint8_t* status_to_string(mb_status_t status)
{
    const char *status_string = NULL;
    switch (status) {
        case MB_SUCCESS:
            status_string = "ok";
            break;
        case MB_INVALID_SLAVE_ADDR:
            status_string = "slave address is invalid";
            break;
        case MB_INVALID_PARAM:
            status_string = "invalid parameter";
            break;
        case MB_RESPOND_EXCEPTION:
            status_string = "slave respond EXCEPTION";
            break;
        case MB_RESPOND_LENGTH_ERR:
            status_string = "recv frame length error";
            break;
        case MB_RESPOND_FRAME_ERR:
            status_string = "recv frame error";
            break;
        case MB_RESPOND_TIMEOUT:
            status_string = "slave no respond";
            break;
        case MB_CANNOT_GET_HANDLER:
            status_string = "can't get handler";
            break;
        case MB_SLAVE_NO_RESPOND:
            status_string = "slave no respond";
            break;
        case MB_FRAME_SEND_ERR:
            status_string = "frame send error";
            break;
        case MB_SERIAL_INIT_FAILED:
            status_string = "serial init failed";
            break;
        default:
            status_string = "unknow reson";
            break;
    }
    return (uint8_t*)status_string;
}
