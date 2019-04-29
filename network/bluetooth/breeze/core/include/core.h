/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_CORE_H
#define BZ_CORE_H

#include <stdint.h>

#include "common.h"
#include "ble_service.h"
#include "transport.h"
#include "auth.h"
#include "extcmd.h"
#include "bzopt.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ALI_COMPANY_ID 0x01A8
#define MAX_ADV_DATA_LEN 16

#define FMSK_BLUETOOTH_VER_Pos 0
#define FMSK_OTA_Pos 2
#define FMSK_SECURITY_Pos 3
#define FMSK_SECRET_TYPE_Pos 4
#define FMSK_SIGNED_ADV_Pos 5
#define MAC_ASCII_LEN 6

typedef struct {
    ali_event_handler_t event_handler;
    uint8_t adv_data[MAX_ADV_DATA_LEN];
    uint16_t adv_data_len;
} core_t;

ret_code_t core_init(ali_init_t const *p_init);
void core_reset(void);
ret_code_t get_bz_adv_data(uint8_t *p_data, uint16_t *length);
void event_notify(uint8_t evt_type, uint8_t *data, uint16_t length);
void core_handle_err(uint8_t src, uint8_t code);

#ifdef __cplusplus
}
#endif

#endif // BZ_CORE_H
