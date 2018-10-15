/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BZ_COMMON_H
#define BZ_COMMON_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "bzopt.h"

#define VERIFY_PARAM_NOT_NULL(p)                                       \
    do {                                                               \
        if (p == NULL) {                                               \
            BREEZE_LOG_ERR("Error: VERIFY_PARAM_NOT_NULL failed (%s %d).\r\n", \
                   __func__, __LINE__);                                \
            assert(0);                                                 \
        }                                                              \
    } while (0)

#define VERIFY_SUCCESS(e)                                           \
    do {                                                            \
        if (e) {                                                    \
            BREEZE_LOG_ERR("Error: VERIFY_SUCCESS failed (e: %d).\r\n", e); \
            assert(0);                                              \
        }                                                           \
    } while (0)

#define VERIFY_PARAM_NOT_NULL_VOID(p)                                       \
    do {                                                                    \
        if (p == NULL) {                                                    \
            BREEZE_LOG_ERR("Error: VERIFY_PARAM_NOT_NULL_VOID failed (%s %d).\r\n", \
                   __func__, __LINE__);                                     \
            assert(0);                                                      \
        }                                                                   \
    } while (0)

#define VERIFY_SUCCESS_VOID(e)                                        \
    do {                                                              \
        if (e) {                                                      \
            BREEZE_LOG_ERR("Error: %s %d VERIFY_SUCCESS failed (e: %d).\r\n", \
                   __func__, __LINE__, e);                            \
            assert(0);                                                \
        }                                                             \
    } while (0)

#define APP_ERROR_CHECK(e)                                             \
    do {                                                               \
        if (e) {                                                       \
            BREEZE_LOG_ERR("Error: %s %d APP_ERROR_CHECK failed (e: %d).\r\n", \
                   __func__, __LINE__, e);                             \
            assert(0);                                                 \
        }                                                              \
    } while (0)

#define BZ_CMD_TYPE_MASK 0xf0

#define BZ_CMD_CTRL 0x0
#define BZ_CMD_STATUS 0x1
#define BZ_CMD_QUERY 0x2
#define BZ_CMD_REPLY 0x3
#define BZ_CMD_EXT_DOWN 0xd
#define BZ_CMD_EXT_UP 0xd
#define BZ_CMD_ERR 0xf

#define BZ_CMD_AUTH 0x10
#define BZ_CMD_AUTH_RAND 0x11
#define BZ_CMD_AUTH_REQ 0x12
#define BZ_CMD_AUTH_RSP 0x13
#define BZ_CMD_AUTH_CFM 0x14
#define BZ_CMD_AUTH_KEY 0x15

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

#define ALI_ERROR_SRC_TYPE_TRANSPORT            0x00020000                          /**< Error source group: Transport */
#define ALI_ERROR_SRC_TRANSPORT_TX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 0)  /**< Transport: Tx timer. */
#define ALI_ERROR_SRC_TRANSPORT_RX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 1)  /**< Transport: Rx timer. */
#define ALI_ERROR_SRC_TRANSPORT_1ST_FRAME       (ALI_ERROR_SRC_TYPE_TRANSPORT | 2)  /**< Transport: 1st frame. */
#define ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES    (ALI_ERROR_SRC_TYPE_TRANSPORT | 3)  /**< Transport: 2nd frame, and beyond. */
#define ALI_ERROR_SRC_TRANSPORT_ENCRYPTED       (ALI_ERROR_SRC_TYPE_TRANSPORT | 4)
#define ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE    (ALI_ERROR_SRC_TYPE_TRANSPORT | 5)  /**< Transport: Received packet larger than buffer size. */
#define ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT    (ALI_ERROR_SRC_TYPE_TRANSPORT | 6)  /**< Transport: Number of packets sent more than expected. */
#define ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC    (ALI_ERROR_SRC_TYPE_TRANSPORT | 7)  /**< Transport: 2nd frame, and beyond. Sequence number discontinuous. */
#define ALI_ERROR_SRC_TRANSPORT_SET_KEY         (ALI_ERROR_SRC_TYPE_TRANSPORT | 8)  /**< Transport: Tx timer. */
#define ALI_ERROR_SRC_TRANSPORT_SEND         (ALI_ERROR_SRC_TYPE_TRANSPORT | 9)  /**< Transport: Tx timer. */

#define ALI_ERROR_SRC_TYPE_AUTH                 0x00030000                          /**< Error source group: Authentication */
#define ALI_ERROR_SRC_AUTH_SEND_RSP             (ALI_ERROR_SRC_TYPE_AUTH | 0)       /**< Authentication: when sending response. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_0         (ALI_ERROR_SRC_TYPE_AUTH | 1)       /**< Authentication: Procedure timer after connection. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_1         (ALI_ERROR_SRC_TYPE_AUTH | 2)       /**< Authentication: Procedure timer after receiving authetication request. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_2         (ALI_ERROR_SRC_TYPE_AUTH | 3)       /**< Authentication: Procedure timer timeout. */
#define ALI_ERROR_SRC_AUTH_SVC_ENABLED          (ALI_ERROR_SRC_TYPE_AUTH | 4)       /**< Authentication: when service has just been enabled. */
#define ALI_ERROR_SRC_AUTH_SEND_ERROR           (ALI_ERROR_SRC_TYPE_AUTH | 5)       /**< Authentication: when sending error (cmd=0x0F). */
#define ALI_ERROR_SRC_AUTH_SEND_KEY             (ALI_ERROR_SRC_TYPE_AUTH | 6)       /**< Authentication: when sending key (cmd=0x15). */

#define ALI_ERROR_SRC_TYPE_EXT                  0x00060000                          /**< Error source group: Extended command */
#define ALI_ERROR_SRC_EXT_SEND_RSP              (ALI_ERROR_SRC_TYPE_EXT | 0)        /**< Extended command: when sending response (cmd=0x0E). */

#define BLE_CONN_HANDLE_INVALID 0xffff
#define BLE_CONN_HANDLE_MAGIC   0x1234

#define BLE_MAC_LEN 6

typedef enum {
    BZ_EVENT_CONNECTED,
    BZ_EVENT_DISCONNECTED,
    BZ_EVENT_AUTHENTICATED,
    BZ_EVENT_TX_DONE,
    BZ_EVENT_RX_CTRL,
    BZ_EVENT_RX_QUERY,
    BZ_EVENT_APINFO,
    BZ_EVENT_OTA_CMD,
    BZ_EVENT_ERR,
};

typedef struct {
    uint8_t *p_data;
    uint16_t length;
} ali_data_t;

typedef ali_data_t ali_rx_data_evt_t;

typedef struct {
    uint32_t source;   /**< The location which caused the error. */
    uint32_t err_code; /**< Error code which has been raised. */
} ali_error_evt_t;

typedef struct {
    uint8_t type; /**< Event type. */
    union {
        ali_rx_data_evt_t rx_data; /**< Data provided for rx-data event. */
        ali_error_evt_t   error;   /**< Data provided for error event. */
    } data;
} ali_event_t;

typedef void (*ali_event_handler_t)(void *p_context, ali_event_t *p_event);

/**@brief Structure for core module configuration. */
typedef struct {
    uint16_t context_size;  // Size of context
    ali_event_handler_t event_handler; // Pointer to event handler
    void *p_evt_context;
    uint32_t   model_id;
    ali_data_t mac;  // mac address
    ali_data_t secret;   // secret 16 to 40 bytes
    ali_data_t product_secret; // secret 16 to 40 bytes
    ali_data_t product_key; // PK 11 to 20 bytes). */
    ali_data_t device_key;  // DN 20 to 32 bytes
    ali_data_t sw_ver;  // Software version
    uint32_t timer_prescaler; /**< Prescaler of timers. */
    uint32_t transport_timeout; /**< Timeout of Tx/Rx, in number of ms. Fill
                                   0 if not used. */
    bool     enable_ota;        /**< Enable OTA firmware upgrade. */
    uint16_t max_mtu;           /**< Maximum MTU. */
    uint8_t  *user_adv_data;    /**< User's adv data, if any. */
    uint32_t user_adv_len;      /**< User's adv data length */
} ali_init_t;

typedef uint32_t (*tx_func_t)(uint8_t cmd, uint8_t *p_data, uint16_t length);

#endif  // BZ_COMMON_H
