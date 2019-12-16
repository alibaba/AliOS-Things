/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <modbus/mbmaster.h>
#include "../physical/serial.h"
#include "../adu/rtu/rtu.h"
#include "../pdu/pdu.h"

#if (MBMASTER_CONFIG_RTU_ENABLED > 0)
mb_status_t mbmaster_rtu_init(mb_handler_t **req_handler, uint8_t port, uint32_t baud_rate, mb_parity_t parity)
{
    mb_handler_t *handler;
    mb_status_t   status = MB_SUCCESS;

    handler = mb_alloc_handler();
    if (handler == NULL) {
        return MB_CANNOT_GET_HANDLER;
    }

    status = mb_serial_init(handler, port, baud_rate, 8, parity);
    if (status != MB_SUCCESS) {
        return status;
    }

    MB_MUTEX_CREATE(&handler->mb_mutex);

    handler->mode            = MB_RTU;
    handler->frame_recv      = mb_serial_frame_recv;
    handler->frame_send      = mb_serial_frame_send;
    handler->adu_assemble    = rtu_assemble;
    handler->adu_disassemble = rtu_disassemble;
    handler->respond_timeout = 200;
    handler->pdu_offset      = ADU_SER_PDU_OFF;

    *req_handler = handler;

    return status;
}

mb_status_t mbmaster_rtu_uninit(mb_handler_t *req_handler)
{
    mb_status_t status = MB_SUCCESS;

    MB_MUTEX_LOCK(&req_handler->mb_mutex);
    MB_MUTEX_DEL(&req_handler->mb_mutex);
    mb_serial_finalize(req_handler);
    mb_free_handler(req_handler);

    return status;
}
#endif /* MBMASTER_CONFIG_RTU_ENABLED */
