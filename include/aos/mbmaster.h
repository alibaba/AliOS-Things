/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_MBMASTER_H
#define AOS_MBMASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mbmaster_api.h>

int aos_mbmaster_rtu_init(mb_handler_t **handler, uint8_t port,
                          uint32_t baud_rate, mb_parity_t parity);

int aos_mbmaster_rtu_uninit(mb_handler_t *req_handler);

int aos_mbmaster_read_holding_register(mb_handler_t *req_handler,
                                       uint8_t slave_addr, uint16_t start_addr,
                                       uint16_t quantity, uint8_t *respond_buf,
                                       uint8_t *respond_count);

int aos_mbmaster_read_input_register(mb_handler_t *req_handler, uint8_t slave_addr,
                                     uint16_t start_addr, uint16_t quantity, uint8_t *respond_buf,
                                     uint8_t *respond_count);

int aos_mbmaster_write_single_register(mb_handler_t *req_handler, uint8_t slave_addr, uint16_t register_addr,
                                       uint16_t register_value, uint16_t *resp_addr, uint16_t *resp_value,
                                       uint8_t *exception_code);

#ifdef __cplusplus
}
#endif

#endif /* AOS_MBMASTER_H */

