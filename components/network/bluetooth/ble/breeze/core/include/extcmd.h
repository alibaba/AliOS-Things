/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_EXTCMD_H
#define BZ_EXTCMD_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ALI_EXT_MAX_TLV_01_RSP_LEN 26

#define ALI_EXT_TX_BUFF_LEN 255 /**< Tx buffer size. */

typedef struct extcmd_s {
    tx_func_t tx_func;
    uint8_t tlv_01_rsp[ALI_EXT_MAX_TLV_01_RSP_LEN];
    uint8_t tlv_01_rsp_len;
    uint32_t model_id;
    uint8_t*p_product_key;
    uint8_t product_key_len;
    uint8_t *p_device_name;
    uint8_t device_name_len;
    uint8_t tx_buff[ALI_EXT_TX_BUFF_LEN];
} extcmd_t;

ret_code_t extcmd_init(ali_init_t const *p_init, tx_func_t tx_func);
void extcmd_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // BZ_EXTCMD_H
