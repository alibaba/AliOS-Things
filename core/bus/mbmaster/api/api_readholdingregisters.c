/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <mbmaster_api.h>
#include "../pdu/pdu.h"

#if (MBMASTER_CONFIG_READ_HOLDING > 0)
mb_status_t mbmaster_read_holding_reginster(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t start_addr,
                                            uint16_t quantity, uint8_t *respond_buf, uint8_t *respond_count)
{
    mb_status_t status = MB_SUCCESS;

    MB_MUTEX_LOCK(&req_handler->mb_mutex);

    req_handler->slave_addr = slave_addr;
    status = read_holding_reginster_assemble(req_handler, start_addr, quantity);
    if (status != MB_SUCCESS) {
        MB_MUTEX_LOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_assemble(req_handler);

    status = req_handler->frame_send(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_LOCK(&req_handler->mb_mutex);
        return status;
    }

     /* wait slave respond */
    mb_log(MB_LOG_DEBUG, "waiting for respond\n");

    status = req_handler->frame_recv(req_handler);
    if (status != MB_SUCCESS) {
        MB_MUTEX_LOCK(&req_handler->mb_mutex);
        return status;
    }

    req_handler->adu_disassemble(req_handler);

    status = read_holding_reginster_disassemble(req_handler, respond_buf, respond_count);
    if (status != MB_SUCCESS) {
        MB_MUTEX_LOCK(&req_handler->mb_mutex);
        return status;
    }

    MB_MUTEX_UNLOCK(&req_handler->mb_mutex);

    return status;
}
#endif /* MBMASTER_CONFIG_READ_HOLDING */
