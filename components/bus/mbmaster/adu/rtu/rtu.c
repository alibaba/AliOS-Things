/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "rtu.h"
#include "../../pdu/pdu.h"
#include "mbcrc.h"

#if (MBMASTER_CONFIG_RTU_ENABLED > 0)
mb_status_t rtu_assemble(mb_handler_t *handler)
{
    uint16_t    crc_16;
    mb_status_t status = MB_SUCCESS;
    uint8_t    *send_buf;
#ifdef DEBUG
    uint8_t     debug_buf[100];
    uint8_t     debug_hex[14];
#endif

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

#ifdef DEBUG
    uint32_t debug_len = 0;
    LOGD(MODBUS_MOUDLE, "start to send data len=%u", (unsigned int)handler->mb_frame_length);
    memset(debug_buf, 0, sizeof(debug_buf));
    for (int i = 0; i < handler->mb_frame_length; i++) {
        debug_len += snprintf(debug_hex, sizeof(debug_hex),"0x%02x ", send_buf[i]);
        if (debug_len > (sizeof(debug_buf))) {
            LOGD(MODBUS_MOUDLE, "assemble debug buf is not enough");
            break;
        }
        strcat(debug_buf, debug_hex);
    }
    LOGD(MODBUS_MOUDLE, "%s", debug_buf);
#endif

    return status;
}

mb_status_t rtu_disassemble(mb_handler_t *handler)
{
    mb_status_t status = MB_SUCCESS;
    uint8_t    *recv_buf;
#ifdef DEBUG
    uint8_t     debug_buf[100];
    uint8_t     debug_hex[6];
#endif

    recv_buf = handler->mb_frame_buff;

#ifdef DEBUG
    uint32_t debug_len = 0;
    LOGD(MODBUS_MOUDLE, "rev data len =%u ,data is :", (unsigned int)handler->mb_frame_length);
    memset(debug_buf, 0, sizeof(debug_buf));
    for (int i = 0; i < handler->mb_frame_length; i++) {
        debug_len += snprintf(debug_hex, sizeof(debug_hex),"0x%02x ", recv_buf[i]);
        if (debug_len > (sizeof(debug_buf))) {
            LOGD(MODBUS_MOUDLE, "disassemble debug buf is not enough");
            break;
        }
        strcat(debug_buf, debug_hex);
    }
    LOGD(MODBUS_MOUDLE, "%s", debug_buf);
#endif

    if ((handler->mb_frame_length >= ADU_SER_LENGTH_MIN)
        && (mb_crc16(( uint8_t *) recv_buf, handler->mb_frame_length) == 0)) {
        handler->slave_addr = recv_buf[ADU_SER_ADDR_OFF];
        handler->pdu_length = handler->mb_frame_length - ADU_SER_PDU_OFF - ADU_SER_LENGTH_CRC;
        handler->pdu_offset = ADU_SER_PDU_OFF;
    } else {
        LOGE(MODBUS_MOUDLE, "frame is too short or CRC error");
        status = MB_RESPOND_FRAME_ERR;
    }
    return status;
}
#endif /* MBMASTER_CONFIG_RTU_ENABLED */
