/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _MODBUSM_H
#define _MODBUSM_H

#include <stdio.h>
#include <k_api.h>
#include <aos/aos.h>
#include "mbm_default_config.h"

typedef enum {
    MB_PAR_NONE,                /*!< No parity. */
    MB_PAR_ODD,                 /*!< Odd parity. */
    MB_PAR_EVEN                 /*!< Even parity. */
} mb_parity_t;

typedef enum {
    MB_EX_NONE                  = 0x00,
    MB_EX_ILLEGAL_FUNCTION      = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS  = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE    = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE  = 0x04,
    MB_EX_ACKNOWLEDGE           = 0x05,
    MB_EX_SLAVE_BUSY            = 0x06,
    MB_EX_MEMORY_PARITY_ERROR   = 0x08,
    MB_EX_GATEWAY_PATH_FAILED   = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED    = 0x0B,

    MB_EX_MB_BUSY               = 0x10,
    MB_EX_INVALID_PARAM         = 0X11,
    MB_EX_ILLEGAL_SLAVE_ADDRESS = 0x12,
    MB_EX_RESPOND_TIMEOUT       = 0x13,
    MB_EX_RESPOND_ERROR         = 0x14,
    MB_EX_RESPOND_EXCEPTION     = 0X15,
    MB_EX_UNCERTAIN_REASON       = 0X16
} mb_exception_t;

typedef enum {
    MB_RTU,                     /*!< RTU transmission mode. */
    MB_ASCII,                   /*!< ASCII transmission mode. */
    MB_TCP                      /*!< TCP mode. */
} mb_mode_t;

typedef enum
{
    MB_ENOERR,                  /*!< no error. */
    MB_ENOREG,                  /*!< illegal register address. */
    MB_EINVAL,                  /*!< illegal argument. */
    MB_EPORTERR,                /*!< porting layer error. */
    MB_ENORES,                  /*!< insufficient resources. */
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT                /*!< timeout error occurred. */
} mb_error_code_t;

typedef enum
{
    MB_LOG_ERROR = 0,
    MB_LOG_DEBUG = 1
} mb_log_level_t;

mb_error_code_t mb_init( mb_mode_t mode, uint8_t port, uint32_t baud_rate, mb_parity_t parity);
mb_error_code_t mb_enable( void );
mb_error_code_t mb_poll( void );

mb_exception_t  mb_read_holding_reginster(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity,
                                       uint8_t *respond_buf, uint8_t *respond_count);

void            mb_log( mb_log_level_t log_level, const int8_t * fmt_str, ... );
uint8_t        *status_to_string(mb_exception_t status);

extern bool(*mb_frame_byte_received_func)(void);
extern bool(*mb_frame_byte_send_func)(void);

#endif
