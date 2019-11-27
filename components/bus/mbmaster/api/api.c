/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <mbmaster_api.h>
#include "../pdu/pdu.h"
#include "main_process.h"

/* function code 0x03 */
mb_status_t mbmaster_read_holding_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                           uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_HOLDING_REGISTERS, slave_addr, start_addr, quantity, respond_buf, respond_count);
    return status;
}

/* function code 0x04 */
mb_status_t mbmaster_read_input_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                         uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_INPUT_REGISTERS, slave_addr, start_addr, quantity, respond_buf, respond_count);
    return status;
}

/* function code 0x06 */
mb_status_t mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                           uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value, uint8_t *exception_code)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type2_process(req_handler, FUNC_CODE_WRITE_SINGLE_REGISTERS, slave_addr, register_addr,
                                         register_value, resp_addr, resp_value, exception_code);
    return status;
}

