/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ALI_COMMON_H_
#define _ALI_COMMON_H_

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define VERIFY_PARAM_NOT_NULL(p)                                       \
    do {                                                               \
        if (p == NULL) {                                               \
            printf("Error: VERIFY_PARAM_NOT_NULL failed (%s %d).\r\n", \
                   __func__, __LINE__);                                \
            assert(0);                                                 \
        }                                                              \
    } while (0)

#define VERIFY_SUCCESS(e)                                           \
    do {                                                            \
        if (e) {                                                    \
            printf("Error: VERIFY_SUCCESS failed (e: %d).\r\n", e); \
            assert(0);                                              \
        }                                                           \
    } while (0)

#define VERIFY_PARAM_NOT_NULL_VOID(p)                                       \
    do {                                                                    \
        if (p == NULL) {                                                    \
            printf("Error: VERIFY_PARAM_NOT_NULL_VOID failed (%s %d).\r\n", \
                   __func__, __LINE__);                                     \
            assert(0);                                                      \
        }                                                                   \
    } while (0)

#define VERIFY_MODULE_INITIALIZED_VOID()                                    \
    do {                                                                    \
        printf("Info: %s %d VERIFY_MODULE_INITIALIZED_VOID.\r\n", __func__, \
               __LINE__);                                                   \
    } while (0)

#define VERIFY_MODULE_INITIALIZED()                                    \
    do {                                                               \
        printf("Info: %s %d VERIFY_MODULE_INITIALIZED.\r\n", __func__, \
               __LINE__);                                              \
    } while (0)

#define VERIFY_SUCCESS_VOID(e)                                        \
    do {                                                              \
        if (e) {                                                      \
            printf("Error: %s %d VERIFY_SUCCESS failed (e: %d).\r\n", \
                   __func__, __LINE__, e);                            \
            assert(0);                                                \
        }                                                             \
    } while (0)

#define APP_ERROR_CHECK(e)                                             \
    do {                                                               \
        if (e) {                                                       \
            printf("Error: %s %d APP_ERROR_CHECK failed (e: %d).\r\n", \
                   __func__, __LINE__, e);                             \
            assert(0);                                                 \
        }                                                              \
    } while (0)

#define GATT_MTU_SIZE_DEFAULT 23


/*CMD definition*/
#define ALI_CMD_TYPE_MASK           0xF0                            /**< Mask used to extract the command type. */

#define ALI_CMD_TYPE_GENERIC        0x00                            /**< Command type: generic */
#define ALI_CMD_CTRL                (ALI_CMD_TYPE_GENERIC | 0x0)    /**< Control command from the central. */
#define ALI_CMD_STATUS              (ALI_CMD_TYPE_GENERIC | 0x1)    /**< Status notification to the central. */
#define ALI_CMD_QUERY               (ALI_CMD_TYPE_GENERIC | 0x2)    /**< Query request, to be handled by application. */
#define ALI_CMD_REPLY               (ALI_CMD_TYPE_GENERIC | 0x3)    /**< Response to @ref ALI_CMD_QUERY, from the application. */
#define ALI_CMD_EXT_DOWN            (ALI_CMD_TYPE_GENERIC | 0xD)    /**< Extended query from the central. */
#define ALI_CMD_EXT_UP              (ALI_CMD_TYPE_GENERIC | 0xE)    /**< Response to @ref ALI_CMD_EXT_DOWN, to the central. */
#define ALI_CMD_ERROR               (ALI_CMD_TYPE_GENERIC | 0xF)    /**< Error notification from peripheral to central. */

#define ALI_CMD_TYPE_AUTH           0x10                            /**< Command type: authentication */
#define ALI_CMD_AUTH_RAND           (ALI_CMD_TYPE_AUTH | 0x1)       /**< 16-byte random number. */
#define ALI_CMD_AUTH_REQ            (ALI_CMD_TYPE_AUTH | 0x2)       /**< Authentication request from central, with payload "Hi,Server". */
#define ALI_CMD_AUTH_RSP            (ALI_CMD_TYPE_AUTH | 0x3)       /**< Authentication response from peripheral, with payload "Hi,Client". */
#define ALI_CMD_AUTH_CFM            (ALI_CMD_TYPE_AUTH | 0x4)       /**< Authentication confirmation from central, with payload "OK". */
#define ALI_CMD_AUTH_KEY            (ALI_CMD_TYPE_AUTH | 0x5)       /**< Key, aka Device identifier, from peripheral. */

#define ALI_CMD_TYPE_FW_UPGRADE     0x20                            /**< Command type: firmware upgrade */
#define ALI_CMD_FW_VERSION_REQ      (ALI_CMD_TYPE_FW_UPGRADE | 0x0) /**< Query on firmware version, sent from central. */
#define ALI_CMD_FW_VERSION_RSP      (ALI_CMD_TYPE_FW_UPGRADE | 0x1) /**< Response to @ref ALI_CMD_FW_VERSION_REQ. */
#define ALI_CMD_FW_UPGRADE_REQ      (ALI_CMD_TYPE_FW_UPGRADE | 0x2) /**< Request to start firmware upgrade, sent from central. */
#define ALI_CMD_FW_UPGRADE_RSP      (ALI_CMD_TYPE_FW_UPGRADE | 0x3) /**< Response to @ref ALI_CMD_FW_UPGRADE_REQ. */
#define ALI_CMD_FW_BYTES_RECEIVED   (ALI_CMD_TYPE_FW_UPGRADE | 0x4) /**< Response to @ref ALI_CMD_FW_GET_INITIAL_SIZE or @ref ALI_CMD_FW_DATA on the progress of firmware download, conveying the last known valid frame sequence and data offset. */
#define ALI_CMD_FW_CHECK_RESULT     (ALI_CMD_TYPE_FW_UPGRADE | 0x5) /**< Result of firmware check after the whole firmware image has been received. */
#define ALI_CMD_FW_UPDATE_PROCESS   (ALI_CMD_TYPE_FW_UPGRADE | 0x6) /**< Notification from peripheral on the progress of firmware upgrade. */
#define ALI_CMD_FW_GET_INIT_FW_SIZE (ALI_CMD_TYPE_FW_UPGRADE | 0x7) /**< Request to get the desired offset of firmware image. */
#define ALI_CMD_FW_XFER_FINISH      (ALI_CMD_TYPE_FW_UPGRADE | 0x8) /**< Message from the central to indicate the end of firmware transfer. */
#define ALI_CMD_FW_DATA             (ALI_CMD_TYPE_FW_UPGRADE | 0xF) /**< Speciial packet for firmware data. */

typedef uint16_t ret_code_t;

/* Error codes. */
#define BREEZE_SUCCESS                          (0)
#define BREEZE_ERROR_INVALID_PARAM              (1)
#define BREEZE_ERROR_DATA_SIZE                  (2)
#define BREEZE_ERROR_INVALID_STATE              (3)
#define BREEZE_ERROR_GATT_NOTIFY                (4)
#define BREEZE_ERROR_GATT_INDICATE              (5)
#define BREEZE_ERROR_BT_ENABLE                  (6)
#define BREEZE_ERROR_BT_ADV                     (7)
#define BREEZE_ERROR_TIMEOUT                    (8)
#define BREEZE_ERROR_BUSY                       (9)
#define BREEZE_ERROR_INVALID_DATA               (10)
#define BREEZE_ERROR_INTERNAL                   (12)
#define BREEZE_ERROR_INVALID_ADDR               (13)
#define BREEZE_ERROR_NOT_SUPPORTED              (14)
#define BREEZE_ERROR_NO_MEM                     (15)
#define BREEZE_ERROR_FORBIDDEN                  (16)
#define BREEZE_ERROR_NULL                       (17)
#define BREEZE_ERROR_INVALID_LENGTH             (18)
#define BREEZE_ERROR_TRANSPORT_TX_FAILURE       (19)
#define BREEZE_ERROR_FLASH_STORE_FAIL           (20)
#define BREEZE_ERROR_FLASH_ERASE_FAIL           (21)
#define BREEZE_ERROR_SETTINGS_FAIL              (22)
#define BREEZE_ERROR_INVALID_BIN_TYPE           (23)
#define BREEZE_ERROR_BIN_UPGRADE_NOT_SUPPORTED  (24)
#define BREEZE_ERROR_BINS_UPGRADE_NOT_SUPPORTED (25)
#define BREEZE_ERROR_INVALID_TLVDATA            (26)
#define BREEZE_ERROR_GET_SEQ_FAIL               (27)
#define BREEZE_ERROR_GET_SIGN_FAIL              (28)

/*Error event group and subcode*/
#define ALI_ERROR_SRC_TYPE_CORE                 0x00010000                          /**< Error source group: Core */
#define ALI_ERROR_SRC_CORE_CANNOT_SET_KEY       (ALI_ERROR_SRC_TYPE_CORE | 0)       /**< Core: key has been generated but refused by transport. */
#define ALI_ERROR_SRC_CORE_SEND_ERR             (ALI_ERROR_SRC_TYPE_CORE | 1)       /**< Core: when sending error (cmd=0x0F) to peer. */

#define ALI_ERROR_SRC_TYPE_TRANSPORT            0x00020000                          /**< Error source group: Transport */
#define ALI_ERROR_SRC_TRANSPORT_TX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 0)  /**< Transport: Tx timer. */
#define ALI_ERROR_SRC_TRANSPORT_RX_TIMER        (ALI_ERROR_SRC_TYPE_TRANSPORT | 1)  /**< Transport: Rx timer. */
#define ALI_ERROR_SRC_TRANSPORT_1ST_FRAME       (ALI_ERROR_SRC_TYPE_TRANSPORT | 2)  /**< Transport: 1st frame. */
#define ALI_ERROR_SRC_TRANSPORT_OTHER_FRAMES    (ALI_ERROR_SRC_TYPE_TRANSPORT | 3)  /**< Transport: 2nd frame, and beyond. */
#define ALI_ERROR_SRC_TRANSPORT_ENCRYPTED       (ALI_ERROR_SRC_TYPE_TRANSPORT | 4)  /**< Transport: encrypted packet, but either length is not aligned to 16 (BREEZE_ERROR_INVALID_DATA) or key is absent (BREEZE_ERROR_FORBIDDEN). */
#define ALI_ERROR_SRC_TRANSPORT_RX_BUFF_SIZE    (ALI_ERROR_SRC_TYPE_TRANSPORT | 5)  /**< Transport: Received packet larger than buffer size. */
#define ALI_ERROR_SRC_TRANSPORT_PKT_CFM_SENT    (ALI_ERROR_SRC_TYPE_TRANSPORT | 6)  /**< Transport: Number of packets sent more than expected. */
#define ALI_ERROR_SRC_TRANSPORT_FW_DATA_DISC    (ALI_ERROR_SRC_TYPE_TRANSPORT | 7)  /**< Transport: 2nd frame, and beyond. Sequence number discontinuous. */

#define ALI_ERROR_SRC_TYPE_AUTH                 0x00030000                          /**< Error source group: Authentication */
#define ALI_ERROR_SRC_AUTH_SEND_RSP             (ALI_ERROR_SRC_TYPE_AUTH | 0)       /**< Authentication: when sending response. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_0         (ALI_ERROR_SRC_TYPE_AUTH | 1)       /**< Authentication: Procedure timer after connection. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_1         (ALI_ERROR_SRC_TYPE_AUTH | 2)       /**< Authentication: Procedure timer after receiving authetication request. */
#define ALI_ERROR_SRC_AUTH_PROC_TIMER_2         (ALI_ERROR_SRC_TYPE_AUTH | 3)       /**< Authentication: Procedure timer timeout. */
#define ALI_ERROR_SRC_AUTH_SVC_ENABLED          (ALI_ERROR_SRC_TYPE_AUTH | 4)       /**< Authentication: when service has just been enabled. */
#define ALI_ERROR_SRC_AUTH_SEND_ERROR           (ALI_ERROR_SRC_TYPE_AUTH | 5)       /**< Authentication: when sending error (cmd=0x0F). */
#define ALI_ERROR_SRC_AUTH_SEND_KEY             (ALI_ERROR_SRC_TYPE_AUTH | 6)       /**< Authentication: when sending key (cmd=0x15). */

#define ALI_ERROR_SRC_TYPE_GAP                  0x00040000                          /**< Error source group: Generic Access */
#define ALI_ERROR_SRC_GAP_CMD_RECEIVED          (ALI_ERROR_SRC_TYPE_GAP | 0)        /**< GAP: Message has arrived. */
#define ALI_ERROR_SRC_GAP_TX_DONE               (ALI_ERROR_SRC_TYPE_GAP | 1)        /**< GAP: Message has been sent. */

#define ALI_ERROR_SRC_TYPE_OTA                  0x00050000                          /**< Error source group: OTA firmware upgrade */
#define ALI_ERROR_SRC_OTA_SEND_VER_RSP          (ALI_ERROR_SRC_TYPE_OTA | 0)        /**< OTA: when sending version response (cmd=0x21). */
#define ALI_ERROR_SRC_OTA_SEND_UPGRADE_RSP      (ALI_ERROR_SRC_TYPE_OTA | 1)        /**< OTA: when sending upgrade response (cmd=0x23). */
#define ALI_ERROR_SRC_OTA_SEND_BYTES_RECVD      (ALI_ERROR_SRC_TYPE_OTA | 2)        /**< OTA: when sending bytes-received message (cmd=0x24). */
#define ALI_ERROR_SRC_OTA_SEND_CRC_RESULT       (ALI_ERROR_SRC_TYPE_OTA | 3)        /**< OTA: when sending CRC result (cmd=0x25). */
#define ALI_ERROR_SRC_OTA_SEND_ERROR            (ALI_ERROR_SRC_TYPE_OTA | 4)        /**< OTA: when sending error (cmd=0x0F). */
#define ALI_ERROR_SRC_OTA_SETTINGS_WRITE        (ALI_ERROR_SRC_TYPE_OTA | 5)        /**< OTA: when writing bootloader settings on boot. */
#define ALI_ERROR_SRC_OTA_FLASH_ERASE           (ALI_ERROR_SRC_TYPE_OTA | 6)        /**< OTA: when erasing flash on receive of firmware upgrade request (cmd=0x22). */
#define ALI_ERROR_SRC_OTA_FLASH_WRITE           (ALI_ERROR_SRC_TYPE_OTA | 7)        /**< OTA: when writing flash on receive of firmware data (cmd=0x2F). */
#define ALI_ERROR_SRC_OTA_FWUP_REQ              (ALI_ERROR_SRC_TYPE_OTA | 8)        /**< OTA: when erasing flash on receive of firmware upgrade request (cmd=0x22). */
#define ALI_ERROR_SRC_OTA_FW_DATA               (ALI_ERROR_SRC_TYPE_OTA | 9)        /**< OTA: when erasing flash on receive of firmware data (cmd=0x2F). */
#define ALI_ERROR_SRC_OTA_XFER_FINISHED         (ALI_ERROR_SRC_TYPE_OTA | 10)       /**< OTA: when writing bootloader settings on receive of firmware check (cmd=0x28). */
#define ALI_ERROR_SRC_OTA_NOTIFY_NEW_FW         (ALI_ERROR_SRC_TYPE_OTA | 11)       /**< OTA: when writing bootloader settings on new firmware is swapped. */
#define ALI_ERROR_SRC_OTA_SEND_FWUP_SUCCESS     (ALI_ERROR_SRC_TYPE_OTA | 12)       /**< OTA: when sending firmware upgrade process after firmware update succeed (cmd=0x26). */
#define ALI_ERROR_SRC_OTA_AUTH_TIMING           (ALI_ERROR_SRC_TYPE_OTA | 13)       /**< OTA: something has happened before authentication. */
#define ALI_ERROR_SRC_OTA_FW_RX_WRITE_SETTINGS  (ALI_ERROR_SRC_TYPE_OTA | 14)       /**< OTA: when writing bootloader settings after writing firmware data. */
#define ALI_ERROR_SRC_OTA_FW_RX_WR_SETTINGS_CB  (ALI_ERROR_SRC_TYPE_OTA | 15)       /**< OTA: in callback when writing bootloader settings after writing firmware data. */

#define ALI_ERROR_SRC_TYPE_EXT                  0x00060000                          /**< Error source group: Extended command */
#define ALI_ERROR_SRC_EXT_SEND_RSP              (ALI_ERROR_SRC_TYPE_EXT | 0)        /**< Extended command: when sending response (cmd=0x0E). */

#define BLE_CONN_HANDLE_INVALID 0xffff
#define BLE_CONN_HANDLE_MAGIC   0x1234

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#endif
