/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <mbmaster_api.h>
#include "pdu.h"

#define PDU_REQ_READ_ADDR_OFF   (PDU_DATA_FIELD_OFF + 0)
#define PDU_REQ_READ_REGCNT_OFF (PDU_DATA_FIELD_OFF + 2)
#define PDU_REQ_READ_SIZE       (5)
#define PDU_REQ_READ_REGCNT_MAX (0x007D)

#define PDU_RESP_READ_REGCNT_OFF (PDU_DATA_FIELD_OFF + 0)
#define PDU_RESP_READ_VALUE_OFF  (PDU_DATA_FIELD_OFF + 1)

#if (MBMASTER_CONFIG_READ_HOLDING > 0)
mb_status_t read_holding_reginster_assemble(mb_handler_t *req_handler, uint16_t start_addr, uint16_t quantity)
{
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];

    if (req_handler->slave_addr > SLAVE_ADDR_MAX) {
        status = MB_INVALID_SLAVE_ADDR;
        return status;
    }

    pdu_buf[PDU_FUNC_FIELD_OFF]        = FUNC_CODE_READ_HOLDING_REGISTERS;
    pdu_buf[PDU_REQ_READ_ADDR_OFF]     = htobe(start_addr);
    pdu_buf[PDU_REQ_READ_ADDR_OFF+1]   = htobe(start_addr) >> 8;
    pdu_buf[PDU_REQ_READ_REGCNT_OFF]   = htobe(quantity);
    pdu_buf[PDU_REQ_READ_REGCNT_OFF+1] = htobe(quantity) >> 8;

    req_handler->pdu_length = PDU_REQ_READ_SIZE;

    return status;
}

mb_status_t read_holding_reginster_disassemble(mb_handler_t *req_handler, uint8_t *respond_buf,
                                               uint8_t *respond_count)
{
    mb_status_t status  = MB_SUCCESS;
    uint8_t    *pdu_buf = &req_handler->mb_frame_buff[req_handler->pdu_offset];

    if ((respond_buf == NULL) || (respond_count == NULL)) {
        status = MB_INVALID_PARAM;
        return status;
    }

    if (pdu_buf[PDU_FUNC_FIELD_OFF] & 0x80) {
        respond_buf[0] = pdu_buf[PDU_RESP_READ_REGCNT_OFF];
        respond_count  = 1;

        status = MB_RESPOND_EXCEPTION;
        return status;
    }

    *respond_count = pdu_buf[PDU_RESP_READ_REGCNT_OFF];

    if ((*respond_count + PDU_RESP_READ_VALUE_OFF) != req_handler->pdu_length) {
        status = MB_RESPOND_LENGTH_ERR;
        return status;
    }

    memcpy(respond_buf, &pdu_buf[PDU_RESP_READ_VALUE_OFF], *respond_count);
    return status;
}

#endif /* MBMASTER_CONFIG_READ_HOLDING */
