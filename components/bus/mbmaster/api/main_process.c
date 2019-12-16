/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "../pdu/pdu.h"
#include "main_process.h"

/* process function code 0x01 0x02 0x03 0x04, they have the same request parameters */
mb_status_t mbmaster_main_type1_process(mb_handler_t *req_handler, uint8_t function_code, uint8_t slave_addr,
                                        uint16_t start_addr, uint16_t quantity, uint8_t *respond_buf,
                                        uint8_t *respond_count)
{
    mb_status_t status = MB_SUCCESS;

    MB_MUTEX_LOCK(&req_handler->mb_mutex);

    req_handler->slave_addr = slave_addr;

    switch (function_code) {
        case FUNC_CODE_READ_COILS:
        case FUNC_CODE_READ_DISCRETE_INPUTS:
            /* The modubs protocol limits the maximum number of reads */
            if ((quantity < 1) || (quantity > QUANTITY_MAX_FUNC01)) {
                return MB_INVALID_PARAM;
            }
            status = pdu_type122_assemble(req_handler, function_code, start_addr, quantity);
            break;
        case FUNC_CODE_READ_HOLDING_REGISTERS:
        case FUNC_CODE_READ_INPUT_REGISTERS:
            /* The modubs protocol limits the maximum number of reads */
            if ((quantity < 1) || (quantity > QUANTITY_MAX_FUNC01)) {
                return MB_INVALID_PARAM;
            }
            status = pdu_type122_assemble(req_handler, function_code, start_addr, quantity);
            break;
        default:
            status = MB_FUNCTION_CODE_NOT_SUPPORT;
            LOGE(MODBUS_MOUDLE, "invalid funciton code!");
            break;
    }
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_assemble(req_handler);

    status = req_handler->frame_send(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

     /* wait slave respond */
    LOGD(MODBUS_MOUDLE, "waiting for respond");

    status = req_handler->frame_recv(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_disassemble(req_handler);

    switch (function_code) {
        case FUNC_CODE_READ_COILS:
        case FUNC_CODE_READ_DISCRETE_INPUTS:
        case FUNC_CODE_READ_HOLDING_REGISTERS:
        case FUNC_CODE_READ_INPUT_REGISTERS:
            status = pdu_type11n_disassemble(req_handler, function_code, respond_buf, respond_count);
            break;
        default:
            status = MB_FUNCTION_CODE_NOT_SUPPORT;
            LOGE(MODBUS_MOUDLE, "invalid funciton code!");
            break;
    }

    MB_MUTEX_UNLOCK(&req_handler->mb_mutex);

    return status;
}

/* process function code 0x05 0x06, they have the same request parameters */
mb_status_t mbmaster_main_type2_process(mb_handler_t *req_handler, uint8_t function_code, uint8_t slave_addr,
                                        uint16_t register_addr, uint16_t register_value, uint16_t *resp_addr,
                                        uint16_t *resp_value, uint8_t* exception_code)
{
    mb_status_t status = MB_SUCCESS;

    MB_MUTEX_LOCK(&req_handler->mb_mutex);

    req_handler->slave_addr = slave_addr;

    switch (function_code) {
        case FUNC_CODE_WRITE_SINGLE_COIL:
            if ((register_value != 0) && (register_value != 0xFF00)) {
                status = MB_INVALID_PARAM;
                return status;
            }
            status = pdu_type122_assemble(req_handler, function_code, register_addr, register_value);
            break;
        case FUNC_CODE_WRITE_SINGLE_REGISTERS:
            status = pdu_type122_assemble(req_handler, function_code, register_addr, register_value);
            break;
        default:
            status = MB_FUNCTION_CODE_NOT_SUPPORT;
            LOGE(MODBUS_MOUDLE, "invalid funciton code!");
            break;
    }
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_assemble(req_handler);

    status = req_handler->frame_send(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

     /* wait slave respond */
    LOGD(MODBUS_MOUDLE, "waiting for respond");

    status = req_handler->frame_recv(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_UNLOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_disassemble(req_handler);

    switch (function_code) {
        case FUNC_CODE_WRITE_SINGLE_COIL:
        case FUNC_CODE_WRITE_SINGLE_REGISTERS:
            status = pdu_type122_disassemble(req_handler, function_code, resp_addr, resp_value, exception_code);
            break;
        default:
            status = MB_FUNCTION_CODE_NOT_SUPPORT;
            LOGE(MODBUS_MOUDLE, "invalid funciton code!");
            break;
    }

    MB_MUTEX_UNLOCK(&req_handler->mb_mutex);

    return status;
}

