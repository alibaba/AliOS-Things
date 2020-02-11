/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_COMMON_H
#define BZ_COMMON_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef BREEZE_LOG_DEBUG
#define BREEZE_LOG_DEBUG printf
#endif

#ifndef BREEZE_LOG_ERR
#define BREEZE_LOG_ERR printf
#endif

#ifndef BREEZE_LOG_INFO
#define BREEZE_LOG_INFO printf
#endif

#define BZ_CMD_TYPE_MASK 0xf0

#define BZ_CMD_CTRL 0x0
#define BZ_CMD_STATUS 0x1
#define BZ_CMD_QUERY 0x2
#define BZ_CMD_REPLY 0x3
#define BZ_CMD_EXT_DOWN 0xd
#define BZ_CMD_EXT_UP 0xe
#define BZ_CMD_ERR 0xf

#define BZ_CMD_AUTH 0x10
#define BZ_CMD_AUTH_RAND 0x11
#define BZ_CMD_AUTH_REQ 0x12
#define BZ_CMD_AUTH_RSP 0x13
#define BZ_CMD_AUTH_CFM 0x14
#define BZ_CMD_AUTH_KEY 0x15
#define BZ_CMD_AUTH_REKEY 0x16
#define BZ_CMD_AUTH_REKEY_RSP 0x17

#define BZ_CMD_TYPE_OTA 0x20
#define BZ_CMD_OTA_VER_REQ 0x20
#define BZ_CMD_OTA_VER_RSP 0x21
#define BZ_CMD_OTA_REQ 0x22
#define BZ_CMD_OTA_RSP 0x23
#define BZ_CMD_OTA_PUB_SIZE 0x24
#define BZ_CMD_OTA_CHECK_RESULT 0x25
#define BZ_CMD_OTA_UPDATE_PROCESS 0x26
#define BZ_CMD_OTA_SIZE 0x27
#define BZ_CMD_OTA_DONE 0x28
#define BZ_CMD_OTA_DATA 0x2f

typedef uint8_t ret_code_t;

/* Error codes. */
#define BZ_SUCCESS 0
#define BZ_EINVALIDPARAM 1
#define BZ_EDATASIZE 2
#define BZ_EINVALIDSTATE 3
#define BZ_EGATTNOTIFY 4
#define BZ_EGATTINDICATE 5
#define BZ_ETIMEOUT 6
#define BZ_EBUSY 7
#define BZ_EINVALIDDATA 8
#define BZ_EINTERNAL 9
#define BZ_EINVALIDADDR 10
#define BZ_ENOTSUPPORTED 11
#define BZ_ENOMEM 12
#define BZ_EFORBIDDEN 13
#define BZ_ENULL 14
#define BZ_EINVALIDLEN 15
#define BZ_EINVALIDTLV 16

#define BZ_ERR_MASK 0xf0
#define BZ_TRANS_ERR 0x10
#define ALI_ERROR_SRC_TRANSPORT_TX_TIMER 0x10
#define ALI_ERROR_SRC_TRANSPORT_RX_TIMER 0x11
#define ALI_ERROR_SRC_TRANSPORT_1ST_FRAME 0x12
#define ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES 0x13
#define ALI_ERROR_SRC_TRANSPORT_ENCRYPTED 0x14
#define ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE 0x15
#define ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT 0x16
#define ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC 0x17
#define ALI_ERROR_SRC_TRANSPORT_SEND 0x18

#define BZ_AUTH_ERR 0x20
#define ALI_ERROR_SRC_AUTH_SEND_RSP 0x20
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_0 0x21
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_1 0x22
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_2 0x23
#define ALI_ERROR_SRC_AUTH_SVC_ENABLED 0x24
#define ALI_ERROR_SRC_AUTH_SEND_ERROR 0x25
#define ALI_ERROR_SRC_AUTH_SEND_KEY 0x26

#define BZ_EXTCMD_ERR 0x30
#define ALI_ERROR_SRC_TYPE_EXT 0x30
#define ALI_ERROR_SRC_EXT_SEND_RSP 0x30

#define BLE_CONN_HANDLE_INVALID 0xffff
#define BLE_CONN_HANDLE_MAGIC   0x1234

#define BLE_MAC_LEN 6

enum {
    BZ_EVENT_CONNECTED,
    BZ_EVENT_DISCONNECTED,
    BZ_EVENT_AUTHENTICATED,
    BZ_EVENT_TX_DONE,
    BZ_EVENT_APINFO,
    BZ_EVENT_ERR_DISCONT,
    BZ_CMD_CTX_INFO,
};

typedef struct {
    uint8_t *p_data;
    uint16_t length;
} ali_data_t;

typedef struct {
    uint8_t type;
    ali_data_t rx_data;
} ali_event_t;

typedef void (*ali_event_handler_t)(ali_event_t *p_event);

typedef struct {
    ali_event_handler_t event_handler;
    uint32_t   model_id;
    ali_data_t secret;   // secret 16 to 40 bytes
    ali_data_t product_secret; // secret 16 to 40 bytes
    ali_data_t product_key; // PK 11 to 20 bytes). */
    ali_data_t device_key;  // DN 20 to 32 bytes
    uint32_t transport_timeout; /**< Timeout of Tx/Rx, in number of ms. 0 if not used. */
    uint16_t max_mtu;           /**< Maximum MTU. */
    uint8_t  *user_adv_data;    /**< User's adv data, if any. */
    uint32_t user_adv_len;      /**< User's adv data length */
} ali_init_t;

typedef struct {
    uint8_t src;
    uint8_t code;
} bz_err_event_t;

typedef uint32_t (*tx_func_t)(uint8_t cmd, uint8_t *p_data, uint16_t length);

#endif  // BZ_COMMON_H
