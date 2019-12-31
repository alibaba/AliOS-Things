#ifndef OTA_BLE_IN_H
#define OTA_BLE_IN_H
#include "ota_log.h"

#define OTA_IMAGE_MAGIC_OFFSET        0x00
#define OTA_IMAGE_SIZE_OFFSET         0x04
#define OTA_IMAGE_MD5_OFFSET          0x08

typedef enum
{
    OTA_BIN_TYPE_APP = 0,
    OTA_BIN_TYPE_KERNEL,
    OTA_BIN_TYPE_SINGLE,
    OTA_BIN_TYPE_APP_KERNEL,
    OTA_BIN_TYPE_MAX,
    OTA_BIN_TYPE_INVALID = 0xff
} ota_ble_bin_type_t;

typedef enum {
    OTA_BLE_FLASH_CODE_SUCCESS,
    OTA_BLE_FLASH_CODE_ERROR
} ota_ble_flash_err_t;

#define OTA_BLE_CMD_TYPE_MASK           0xF0

#define OTA_BLE_CMD_TYPE_GENERIC                     0x00                /**< Command type: generic */
#define OTA_BLE_CMD_ERROR         (OTA_BLE_CMD_TYPE_GENERIC | 0xF)    /**< Error notification from peripheral to central. */

#define OTA_BLE_CMD_TYPE_FW_UPGRADE     0x20                            /**< Command type: firmware upgrade */
#define OTA_BLE_CMD_FW_VERSION_REQ      (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x0) /**< Query on firmware version, sent from central. */
#define OTA_BLE_CMD_FW_VERSION_RSP      (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x1) /**< Response to @ref ALI_CMD_FW_VERSION_REQ. */
#define OTA_BLE_CMD_FW_UPGRADE_REQ      (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x2) /**< Request to start firmware upgrade, sent from central. */
#define OTA_BLE_CMD_FW_UPGRADE_RSP      (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x3) /**< Response to @ref ALI_CMD_FW_UPGRADE_REQ. */
#define OTA_BLE_CMD_FW_BYTES_RECEIVED   (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x4) /**< Response to @ref ALI_CMD_FW_GET_INITIAL_SIZE or @ref ALI_CMD_FW_DATA on the progress of firmware download, conveying the last known valid frame sequence and data offset. */
#define OTA_BLE_CMD_FW_CHECK_RESULT     (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x5) /**< Result of firmware check after the whole firmware image has been received. */
#define OTA_BLE_CMD_FW_UPDATE_PROCESS   (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x6) /**< Notification from peripheral on the progress of firmware upgrade. */
#define OTA_BLE_CMD_FW_GET_INIT_FW_SIZE (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x7) /**< Request to get the desired offset of firmware image. */
#define OTA_BLE_CMD_FW_XFER_FINISH      (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0x8) /**< Message from the central to indicate the end of firmware transfer. */
#define OTA_BLE_CMD_FW_DATA             (OTA_BLE_CMD_TYPE_FW_UPGRADE | 0xF) /**< Speciial packet for firmware data. */

#define OTA_BLE_RECEIVE_BUF_NUMB        (3)
#define OTA_BLE_REC_PER_FRAME_LEN       (256)

#define OTA_BLE_EVENT_AUTH              (0x01)
#define OTA_BLE_EVENT_TX_DONE           (0x02)

typedef struct _ota_ble_receive_data_{
    unsigned char cmd;
    unsigned char num_frames;
    unsigned short len;
    unsigned char rec_buf[OTA_BLE_REC_PER_FRAME_LEN];
} ota_ble_receive_t;

typedef enum
{
    OTA_BLE_STATE_OFF,              /**< Off, expects authentication results. */
    OTA_BLE_STATE_IDLE,             /**< Idle, expects commands 0x20 (version query) or 0x22 (update request) */
    OTA_BLE_STATE_RECEIVER_HEADER,  /**< Receive firmware header**/
    OTA_BLE_STATE_RECEIVE,          /**< Receive firmware, expects commands 0x27 (progress query) or 0x2F (Firmware data). */
    OTA_BLE_STATE_RECEIVE_ERR,      /**< Receive firmware, but frame discontinuity happened, waiting for error (cmd=0x0F) sent. */
    OTA_BLE_STATE_WRITE_SETTINGS,   /**< Settings write after flash write, expects bootloader_settings completion event. */
    OTA_BLE_STATE_FW_CHECK,         /**< Flash check, expects command 0x28 (Firmware check) to make sure the peer will not send anything in between. */
    OTA_BLE_STATE_RESET_PREPARE,    /**< Reset prepare, expects Tx-done event to make sure commands 0x25 (firmware check result) is sent. */
} ota_ble_state_t;

typedef struct _ota_ble_global_dat{
    unsigned char task_ctrl;
    unsigned char task_flag;
    unsigned char status;
    unsigned int  rx_fw_size;                  /**< Size of firmware to be received. */
    unsigned int  valid_fw_size;               /**< After removing image info size*/
    unsigned int  bytes_recvd;                 /**< Size of firmware received. */
    unsigned int  valid_bytes_recvd;           /**< Don't include image info FM size*/
    unsigned short frames_recvd;               /**< Number of frames of firmware received. */
    unsigned short crc;
} ota_ble_global_dat_t;

int ota_ble_send_bytes_received(void);
void ota_ble_md5_update(const unsigned char *src, unsigned int size);
void ota_ble_calculate_image_crc16(unsigned char *data, unsigned int len);
int ota_ble_download(unsigned char ota_cmd, unsigned char num_frame, unsigned char *buf, unsigned int len);
#endif /*OTA_BLE_IN_H*/

