/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "pdu.h"

/**
 * Used for 122 format frame assemble: 1byte 2byte 2byte
 * The function code that conform to this format : 0x01 0x02 0x03 0x4 0x05 0x6
 */
mb_status_t pdu_type122_assemble(mb_handler_t *req_handler, uint8_t field0, uint16_t field1, uint16_t field2)
{
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];

    if (req_handler->slave_addr > SLAVE_ADDR_MAX) {
        status = MB_INVALID_SLAVE_ADDR;
        return status;
    }

    pdu_buf[0] = field0;
    pdu_buf[1] = htobe16(field1) & 0xFF;
    pdu_buf[2] = (htobe16(field1) >> 8) & 0xFF;
    pdu_buf[3] = htobe16(field2) & 0xFF;
    pdu_buf[4] = (htobe16(field2) >> 8) & 0xFF;

    req_handler->pdu_length = 5;

    return status;
}

/**
 * Used for 1221n format frame assemble: 1byte function_code:2byte start address:2byte quantity:1byte counts: n byte outputs data
 * The function code that conform to this format : 0x0F 0x10
 */
mb_status_t pdu_type1221n_assemble(mb_handler_t *req_handler, uint8_t function_code, uint16_t start_addr,
                                   uint16_t quantity, uint8_t byte_count, uint8_t *outputs_buf)
{
    uint8_t     i;
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];
    uint16_t   *register_data = outputs_buf;

    if (req_handler->slave_addr > SLAVE_ADDR_MAX) {
        status = MB_INVALID_SLAVE_ADDR;
        return status;
    }

    pdu_buf[0] = function_code;
    pdu_buf[1] = htobe16(start_addr) & 0xFF;
    pdu_buf[2] = (htobe16(start_addr) >> 8) & 0xFF;
    pdu_buf[3] = htobe16(quantity) & 0xFF;
    pdu_buf[4] = (htobe16(quantity) >> 8) & 0xFF;
    pdu_buf[5] = byte_count;

    if (function_code == FUNC_CODE_WRITE_MULTIPLE_REGISTERS) {
        for (i = 0; i < quantity; i++) {
            register_data[i] = htobe16(register_data[i]);
        }
    }
    memcpy(&pdu_buf[6], register_data, byte_count);

    req_handler->pdu_length = 6 + byte_count;

    return status;
}

/**
 * Used for 11n format frame disassemble : 1 byte function code:1 byte count:n byte data
 * when exception the format is: 1 byte error code:1 byte excepton code
 * The function code that conform to this format : 0x01 0x02 0x03 0x4
 */
mb_status_t pdu_type11n_disassemble(mb_handler_t *req_handler, uint8_t function_code, uint8_t *respond_buf,
                                    uint8_t *respond_count)
{
    uint32_t    i;
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];
    uint16_t   *register_data;

    if ((respond_buf == NULL) || (respond_count == NULL)) {
        status = MB_INVALID_PARAM;
        return status;
    }

    if ((pdu_buf[0] & 0x80) != 0) {
        respond_buf[0] = pdu_buf[1];
        *respond_count  = 1;

        status = MB_RESPOND_EXCEPTION;
        return status;
    }

    *respond_count = pdu_buf[1];

    /* pdu_length = (1 byte function code) + (1 byte count) + (n byte data) */
    if ((*respond_count + 2) != req_handler->pdu_length) {
        status = MB_RESPOND_LENGTH_ERR;
        return status;
    }

    register_data = &pdu_buf[2];

    if ((function_code == FUNC_CODE_READ_HOLDING_REGISTERS) || (function_code == FUNC_CODE_READ_INPUT_REGISTERS)) {
        for (i = 0; i < (*respond_count / 2); i++) {
            register_data[i] = betoh16(register_data[i]);
        }
    }

    memcpy(respond_buf, register_data, *respond_count);
    return status;
}

/**
 * Used for 112 format frame disassemble : 1 byte function code:2 byte data:2 byte data
 * when exception the format is: 1 byte error code:1 byte excepton code
 * The function code that conform to this format : 0x05 0x06
 */
mb_status_t pdu_type122_disassemble(mb_handler_t *req_handler, uint8_t function_code, uint16_t *data1,
                                    uint16_t *data2, uint8_t* exception_code)
{
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];
    uint16_t    revdata;

    if ((pdu_buf[0] & 0x80) != 0 ) {
        if (exception_code != NULL) {
           *exception_code = pdu_buf[1];
        }
        status = MB_RESPOND_EXCEPTION;
        return status;
    }

    /* For FUNC_CODE_WRITE_SINGLE_COIL, 0xff00 means ON and 0x0000 means OFF,
     * so conversion is also required */
    if (data1 != NULL) {
        revdata = pdu_buf[2];
        revdata = pdu_buf[1] | (revdata << 8);
        *data1  = betoh16(revdata);
    }

    /* For FUNC_CODE_WRITE_SINGLE_COIL, 0xff00 means ON and 0x0000 means OFF,
     * so conversion is also required */
    if (data2 != NULL) {
        revdata = pdu_buf[4];
        revdata = pdu_buf[3] | (revdata << 8);
        *data2  = betoh16(revdata);
    }

    return status;
}
