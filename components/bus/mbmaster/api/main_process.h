/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef MAIN_PROCESS_H
#define MAIN_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

/* process function code 0x01 0x02 0x03 0x04, they have the same request parameters */
mb_status_t mbmaster_main_type1_process(mb_handler_t *req_handler, uint8_t function_code, uint8_t slave_addr,
                                        uint16_t start_addr, uint16_t quantity, uint8_t *respond_buf,
                                        uint8_t *respond_count);

/* process function code 0x05 0x06, they have the same request parameters */
mb_status_t mbmaster_main_type2_process(mb_handler_t *req_handler, uint8_t function_code, uint8_t slave_addr,
                                        uint16_t register_addr, uint16_t register_value, uint16_t *resp_addr,
                                        uint16_t *resp_value, uint8_t* exception_code);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_PROCESS_H */

