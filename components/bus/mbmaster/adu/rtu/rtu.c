/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <mbmaster_api.h>
#include "rtu.h"
#include "../../pdu/pdu.h"
#include "mbcrc.h"

#if (MBMASTER_CONFIG_RTU_ENABLED > 0)
mb_status_t rtu_assemble(mb_handler_t *handler)
{
    uint16_t    crc_16;
    mb_status_t status = MB_SUCCESS;
    uint8_t    *send_buf;

    if (handler->slave_addr == SLAVE_ADDR_BROADCAST) {
        handler->respond_timeout = 1000;
    } else {
        handler->respond_timeout = 200;
    }

    send_buf = handler->mb_frame_buff;

    send_buf[ADU_SER_ADDR_OFF] = handler->slave_addr;
    handler->mb_frame_length   = handler->pdu_length + 1;
    crc_16 = mb_crc16((uint8_t *) send_buf, handler->mb_frame_length);

    send_buf[handler->mb_frame_length++] = (uint8_t)(crc_16 & 0xFF);
    send_buf[handler->mb_frame_length++] = (uint8_t)(crc_16 >> 8);

    mb_log(MB_LOG_DEBUG, "start to send data len=%u\n", (unsigned int)handler->mb_frame_length);
    for (int i = 0; i < handler->mb_frame_length; i++) {
        mb_log(MB_LOG_DEBUG, "0x%x ", send_buf[i]);
        if (i == handler->mb_frame_length - 1)
            mb_log(MB_LOG_DEBUG, "\n");
    }

    return status;
}

mb_status_t rtu_disassemble(mb_handler_t *handler)
{
    mb_status_t status = MB_SUCCESS;
    uint8_t    *recv_buf;

    recv_buf = handler->mb_frame_buff;

    mb_log(MB_LOG_DEBUG, "rev data len =%u ,data is :\n", (unsigned int)handler->mb_frame_length);
    for (int i = 0; i < handler->mb_frame_length; i++) {
        mb_log(MB_LOG_DEBUG, "0x%x ", recv_buf[i]);
        if (i == handler->mb_frame_length - 1)
            mb_log(MB_LOG_DEBUG, "\n");
    }

    if ((handler->mb_frame_length >= ADU_SER_LENGTH_MIN)
        && (mb_crc16(( uint8_t *) recv_buf, handler->mb_frame_length) == 0)) {
        handler->slave_addr = recv_buf[ADU_SER_ADDR_OFF];
        handler->pdu_length = handler->mb_frame_length - ADU_SER_PDU_OFF - ADU_SER_LENGTH_CRC;
        handler->pdu_offset = ADU_SER_PDU_OFF;
    } else {
        mb_log(MB_LOG_ERROR, "frame is too short or CRC error\n");
        status = MB_RESPOND_FRAME_ERR;
    }
    return status;
}
#endif /* MBMASTER_CONFIG_RTU_ENABLED */
