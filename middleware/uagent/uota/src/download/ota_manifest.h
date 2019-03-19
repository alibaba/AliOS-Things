/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_MANIFEST_H_
#define OTA_MANIFEST_H_
#include <stdint.h>
#include "ota_hal_plat.h"
#include "ota_transport.h"
#include "ota_rsa_verify.h"

typedef int (*ota_write_cb_t)(int32_t writed_size, uint8_t *buf,
                              int32_t buf_len, int type);

typedef int (*ota_read_cb_t)(volatile uint32_t *read_size, uint8_t *buf,
                             int32_t buf_len, int type);

typedef int (*ota_finish_cb_t)(OTA_ENUM_RESULT_TYPE finish_result,
                               void                *updated_version);

int8_t ota_do_update_packet(ota_response_params *response_parmas,
                            ota_request_params  *request_parmas,
                            ota_write_cb_t wcb, ota_read_cb_t rcb,
                            ota_finish_cb_t fcb);

int8_t ota_cancel_update_packet(ota_response_params *response_parmas);

int8_t ota_post_version_msg(void);

const char *ota_get_resp_msg(void);

int ota_set_resp_msg(const char *value);
#endif /* OTA_MANIFEST_H_ */
