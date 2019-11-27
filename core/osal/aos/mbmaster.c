/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/mbmaster.h"

#if (MBMASTER_CONFIG_RTU_ENABLED > 0)
int aos_mbmaster_rtu_init(mb_handler_t **handler, uint8_t port, uint32_t baud_rate, mb_parity_t parity)
{
    mb_status_t status;

    if (handler == NULL) {
        return -1;
    }

    status = mbmaster_rtu_init(handler, port, baud_rate, parity);
    if (status != MB_SUCCESS) {
        return -1;
    }

    return 0;
}

int aos_mbmaster_rtu_uninit(mb_handler_t *req_handler)
{
    mb_status_t status;

    if (req_handler == NULL) {
        return -1;
    }

    status = mbmaster_rtu_uninit(req_handler);
    if (status != MB_SUCCESS) {
        return -1;
    }

    return 0;
}
#endif /* MBMASTER_CONFIG_RTU_ENABLED */

int aos_mbmaster_read_holding_register(mb_handler_t *req_handler, uint8_t slave_addr,
                                       uint16_t start_addr, uint16_t quantity, uint8_t *respond_buf,
                                       uint8_t *respond_count)
{
    mb_status_t status;

    if (req_handler == NULL) {
        return -1;
    }

    status = mbmaster_read_holding_register(req_handler, slave_addr, start_addr,
                                                 quantity, respond_buf, respond_count);
    if (status != MB_SUCCESS) {
        printf("%s\n", status_to_string(status));
        return -1;
    }

    return 0;
}

int aos_mbmaster_read_input_register(mb_handler_t *req_handler, uint8_t slave_addr,
                                     uint16_t start_addr, uint16_t quantity, uint8_t *respond_buf,
                                     uint8_t *respond_count)
{
    mb_status_t status;

    if (req_handler == NULL) {
        return -1;
    }

    status = mbmaster_read_input_register(req_handler, slave_addr, start_addr,
                                          quantity, respond_buf, respond_count);
    if (status != MB_SUCCESS) {
        printf("%s\n", status_to_string(status));
        return -1;
    }

    return 0;
}

int aos_mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                       uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value,
                                       uint8_t *exception_code)
{
    mb_status_t status;

    if (req_handler == NULL) {
        return -1;
    }

    status = mbmaster_write_single_register(req_handler, slave_addr, register_addr, register_value,
                                            resp_addr, resp_value, exception_code);
    if (status != MB_SUCCESS) {
        printf("%s\n", status_to_string(status));
        return -1;
    }

    return 0;
}

