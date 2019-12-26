/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "../pdu/pdu.h"
#include "main_process.h"

/* function code 0x01 */
mb_status_t mbmaster_read_coils(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count, uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_COILS, slave_addr, start_addr, quantity,
                                         respond_buf, respond_count, timeout);
    return status;
}

/* function code 0x02 */
mb_status_t mbmaster_read_discrete_inputs(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                          uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count,
                                          uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_DISCRETE_INPUTS, slave_addr, start_addr,
                                         quantity, respond_buf, respond_count, timeout);
    return status;
}

/* function code 0x03 */
mb_status_t mbmaster_read_holding_registers(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                            uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count,
                                            uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_HOLDING_REGISTERS, slave_addr, start_addr,
                                         quantity, respond_buf, respond_count, timeout);
    return status;
}

/* function code 0x04 */
mb_status_t mbmaster_read_input_registers(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                          uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count,
                                          uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type1_process(req_handler, FUNC_CODE_READ_INPUT_REGISTERS, slave_addr, start_addr,
                                         quantity, respond_buf, respond_count, timeout);
    return status;
}

/* function code 0x05 */
mb_status_t mbmaster_write_single_coil(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t coil_addr,
                                       uint16_t coil_value, uint16_t *resp_addr, uint16_t *resp_value,
                                       uint8_t *exception_code, uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type2_process(req_handler, FUNC_CODE_WRITE_SINGLE_COIL, slave_addr, coil_addr,
                                         coil_value, resp_addr, resp_value, exception_code, timeout);
    return status;
}

/* function code 0x06 */
mb_status_t mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                           uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value,
                                           uint8_t *exception_code, uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type2_process(req_handler, FUNC_CODE_WRITE_SINGLE_REGISTERS, slave_addr, register_addr,
                                         register_value, resp_addr, resp_value, exception_code, timeout);
    return status;
}

/* function code 0x0F */
mb_status_t mbmaster_write_multiple_coils(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                          uint16_t quantity, uint8_t *outputs_buf, uint16_t *resp_addr,
                                          uint16_t *resp_quantity, uint8_t *exception_code, uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type3_process(req_handler, FUNC_CODE_WRITE_MULTIPLE_COILS, slave_addr, start_addr,
                                         quantity, outputs_buf, resp_addr, resp_quantity, exception_code, timeout);
    return status;
}

/* function code 0x10 */
mb_status_t mbmaster_write_multiple_registers(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                              uint16_t quantity, uint8_t *outputs_buf, uint16_t *resp_addr,
                                              uint16_t *resp_quantity, uint8_t *exception_code, uint32_t timeout)
{
    mb_status_t status = MB_SUCCESS;
    status = mbmaster_main_type3_process(req_handler, FUNC_CODE_WRITE_MULTIPLE_REGISTERS, slave_addr, start_addr,
                                         quantity, outputs_buf, resp_addr, resp_quantity, exception_code, timeout);
    return status;
}

