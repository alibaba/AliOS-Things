/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MBMASTER_API_H
#define MBMASTER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "k_api.h"

#include "mbmaster_default_config.h"
#include "../adu/adu.h"
#include "../auxiliary/auxiliary.h"

typedef enum {
    MB_PAR_NONE,
    MB_PAR_ODD,
    MB_PAR_EVEN
} mb_parity_t;

typedef enum mb_status {
    MB_SUCCESS = 0u,
    MB_MUTEX_ERROR,
    MB_INVALID_SLAVE_ADDR,
    MB_INVALID_PARAM,
    MB_RESPOND_EXCEPTION,
    MB_RESPOND_LENGTH_ERR,
    MB_RESPOND_FRAME_ERR,
    MB_RESPOND_TIMEOUT,
    MB_CANNOT_GET_HANDLER,
    MB_SLAVE_NO_RESPOND,
    MB_FRAME_SEND_ERR,
    MB_SERIAL_INIT_FAILED,
    MB_SERIAL_UNINIT_FAILED,
    MB_FUNCTION_CODE_NOT_SUPPORT,
} mb_status_t;

typedef enum {
    MB_RTU,
    MB_ASCII,
    MB_TCP
} mb_mode_t;

/* forward declaration */
typedef struct mb_handler mb_handler_t;

typedef mb_status_t (*frame_send_func_t)(mb_handler_t *);
typedef mb_status_t (*frame_recv_func_t)(mb_handler_t *);
typedef mb_status_t (*adu_assemble_func_t)(mb_handler_t *);
typedef mb_status_t (*adu_disassemble_func_t)(mb_handler_t *);

typedef struct mb_handler {
    uint8_t used;
    uint8_t slave_addr;
    uint8_t mode;

    uint8_t  mb_frame_buff[ADU_BUF_MAX_LENGTH];
    uint8_t  pdu_offset;
    uint8_t  pdu_length;
    uint32_t mb_frame_length;

    uint32_t respond_timeout;

    frame_send_func_t      frame_send;
    frame_recv_func_t      frame_recv;
    adu_assemble_func_t    adu_assemble;
    adu_disassemble_func_t adu_disassemble;

    MB_MUTEX_T mb_mutex;

    void *private;
} mb_handler_t;

mb_status_t mbmaster_rtu_init(mb_handler_t **handler, uint8_t port, uint32_t baud_rate, mb_parity_t parity);

mb_status_t mbmaster_rtu_uninit(mb_handler_t *req_handler);

/* function code 0x03 */
mb_status_t mbmaster_read_holding_register(mb_handler_t *req_handler, uint8_t slave_addr,
                                           uint16_t start_addr, uint16_t quantity,
                                           uint8_t *respond_buf, uint8_t *respond_count);

/* function code 0x04 */
mb_status_t mbmaster_read_input_register(mb_handler_t *req_handler, uint8_t slave_addr,
                                         uint16_t start_addr, uint16_t quantity,
                                         uint8_t *respond_buf, uint8_t *respond_count);

/* function code 0x06 */
mb_status_t mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                           uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value, uint8_t *exception_code);

#ifdef __cplusplus
}
#endif

#endif /* MBMASTER_API_H */

