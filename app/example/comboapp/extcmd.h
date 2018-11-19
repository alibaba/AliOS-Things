/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_EXTCMD_H
#define BZ_EXTCMD_H

#include <stdint.h>
#include "breeze_export.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define EXT_VERSION "2.0.5"

#define ALI_EXT_MAX_TLV_01_RSP_LEN 26
#define ALI_EXT_TX_BUFF_LEN 255 /**< Tx buffer size. */

#define EXT_SUCCESS 0
#define EXT_EDATASIZE 2
#define EXT_EINVALIDDATA 8
#define EXT_EINVALIDADDR 10
#define EXT_ENOTSUPPORTED 11
#define EXT_ENOMEM 12
#define EXT_ENULL 14
#define EXT_EINVALIDLEN 15
#define EXT_EINVALIDTLV 16

#define BZ_EINVALIDPARAM 1
#define BZ_EINVALIDSTATE 3
#define BZ_EINTERNAL 9
#define BZ_EFORBIDDEN 13

#define CMD_EXT_DOWN 0xd
#define CMD_EXT_UP 0xe
#define CMD_EXT_ERR 0xf

typedef uint8_t ret_code_t;

typedef struct {
    char    ssid[32 + 1];
    char    pw[64 + 1];
    uint8_t bssid[6];
} breeze_apinfo_t;

typedef struct extcmd_s {
    uint8_t tlv_01_rsp[ALI_EXT_MAX_TLV_01_RSP_LEN];
    uint8_t tlv_01_rsp_len;
    uint32_t model_id;
    uint8_t p_product_key[STR_PROD_KEY_LEN];
    uint8_t product_key_len;
    uint8_t p_secret[STR_PROD_SEC_LEN];
    uint8_t secret_len;
    uint8_t p_device_name[STR_DEV_KEY_LEN];
    uint8_t device_name_len;
    uint8_t tx_buff[ALI_EXT_TX_BUFF_LEN];
} extcmd_t;

ret_code_t extcmd_init(breeze_dev_info_t *p_dev_info);
void extcmd_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // BZ_EXTCMD_H
