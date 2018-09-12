#ifndef _OTA_BREEZE_H
#define _OTA_BREEZE_H

#include "ota_breeze_export.h"

#ifndef true
#define true    (1)
#endif

#ifndef false
#define false   (0)
#endif

#ifndef NULL
#define NULL 0
#endif

#define OTA_BREEZE_BIN_TYPE_INFO_OFFSET 0x28

#define OTA_BREEZE_BIN_TYPE_MAGIC_APP    0xabababab
#define OTA_BREEZE_BIN_TYPE_MAGIC_KERNEL 0xcdcdcdcd
#define OTA_BREEZE_BIN_TYPE_MAGIC_SINGLE 0xefefefef

typedef enum
{
    OTA_BREEZE_BIN_TYPE_APP = 0,
    OTA_BREEZE_BIN_TYPE_KERNEL,
    OTA_BREEZE_BIN_TYPE_SINGLE,
    OTA_BREEZE_BIN_TYPE_MAX,
    OTA_BREEZE_BIN_TYPE_INVALID = 0xff
} ota_breeze_bin_type_t;

typedef enum {
    OTA_BREEZE_FLASH_CODE_SUCCESS,
    OTA_BREEZE_FLASH_CODE_ERROR
} ota_breeze_flash_err_t;


#define OTA_BREEZE_CMD_TYPE_MASK           0xF0 

/* Error codes. */
#define OTA_BREEZE_SUCCESS                              (0)
#define OTA_BREEZE_ERROR_INVALID_PARAM                  (1)
#define OTA_BREEZE_ERROR_DATA_SIZE                      (2)
#define OTA_BREEZE_ERROR_INVALID_STATE                  (3)
#define OTA_BREEZE_ERROR_TIMEOUT                        (4)
#define OTA_BREEZE_ERROR_BUSY                           (5)
#define OTA_BREEZE_ERROR_INVALID_DATA                   (6)
#define OTA_BREEZE_ERROR_INTERNAL                       (7)
#define OTA_BREEZE_ERROR_INVALID_ADDR                   (8)
#define OTA_BREEZE_ERROR_NOT_SUPPORTED                  (9)
#define OTA_BREEZE_ERROR_NO_MEM                         (10)
#define OTA_BREEZE_ERROR_FORBIDDEN                      (11)
#define OTA_BREEZE_ERROR_NULL                           (12)
#define OTA_BREEZE_ERROR_INVALID_LENGTH                 (13)
#define OTA_BREEZE_ERROR_TRANSPORT_TX_FAILURE           (14)
#define OTA_BREEZE_ERROR_FLASH_STORE_FAIL               (15)
#define OTA_BREEZE_ERROR_FLASH_ERASE_FAIL               (16)
#define OTA_BREEZE_ERROR_SETTINGS_FAIL                  (17)
#define OTA_BREEZE_ERROR_INVALID_BIN_TYPE               (18)
#define OTA_BREEZE_ERROR_BIN_UPGRADE_NOT_SUPPORTED      (19)
#define OTA_BREEZE_ERROR_BINS_UPGRADE_NOT_SUPPORTED     (20)

#define OTA_BREEZE_CMD_TYPE_GENERIC                     0x00                /**< Command type: generic */
#define OTA_BREEZE_CMD_ERROR         (OTA_BREEZE_CMD_TYPE_GENERIC | 0xF)    /**< Error notification from peripheral to central. */


#define OTA_BREEZE_CMD_TYPE_FW_UPGRADE     0x20                            /**< Command type: firmware upgrade */
#define OTA_BREEZE_CMD_FW_VERSION_REQ      (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x0) /**< Query on firmware version, sent from central. */
#define OTA_BREEZE_CMD_FW_VERSION_RSP      (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x1) /**< Response to @ref ALI_CMD_FW_VERSION_REQ. */
#define OTA_BREEZE_CMD_FW_UPGRADE_REQ      (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x2) /**< Request to start firmware upgrade, sent from central. */
#define OTA_BREEZE_CMD_FW_UPGRADE_RSP      (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x3) /**< Response to @ref ALI_CMD_FW_UPGRADE_REQ. */
#define OTA_BREEZE_CMD_FW_BYTES_RECEIVED   (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x4) /**< Response to @ref ALI_CMD_FW_GET_INITIAL_SIZE or @ref ALI_CMD_FW_DATA on the progress of firmware download, conveying the last known valid frame sequence and data offset. */
#define OTA_BREEZE_CMD_FW_CHECK_RESULT     (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x5) /**< Result of firmware check after the whole firmware image has been received. */
#define OTA_BREEZE_CMD_FW_UPDATE_PROCESS   (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x6) /**< Notification from peripheral on the progress of firmware upgrade. */
#define OTA_BREEZE_CMD_FW_GET_INIT_FW_SIZE (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x7) /**< Request to get the desired offset of firmware image. */
#define OTA_BREEZE_CMD_FW_XFER_FINISH      (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0x8) /**< Message from the central to indicate the end of firmware transfer. */
#define OTA_BREEZE_CMD_FW_DATA             (OTA_BREEZE_CMD_TYPE_FW_UPGRADE | 0xF) /**< Speciial packet for firmware data. */

#define OTA_BREEZE_RECEIVE_BUF_NUMB        (3)
#define OTA_BREEZE_REC_PER_FRAME_LEN       (256)

#define OTA_BREEZE_EVENT_AUTH       (0x01)
#define OTA_BREEZE_EVENT_TX_DONE    (0x02)

typedef struct _ota_breeze_rec_data_{
    uint8_t cmd;
    uint8_t num_frames;
    uint16_t length;
    uint8_t rec_buf[OTA_BREEZE_REC_PER_FRAME_LEN];
}ota_breeze_rec_t;


typedef enum
{
    OTA_BREEZE_STATE_OFF,              /**< Off, expects authentication results. */
    OTA_BREEZE_STATE_IDLE,             /**< Idle, expects commands 0x20 (version query) or 0x22 (update request) */
    OTA_BREEZE_STATE_RECEIVE,          /**< Receive firmware, expects commands 0x27 (progress query) or 0x2F (Firmware data). */
    OTA_BREEZE_STATE_RECEIVE_ERR,      /**< Receive firmware, but frame discontinuity happened, waiting for error (cmd=0x0F) sent. */
    OTA_BREEZE_STATE_WRITE,            /**< Flash write, expects flash completion event. */
    OTA_BREEZE_STATE_WRITE_SETTINGS,   /**< Settings write after flash write, expects bootloader_settings completion event. */
    OTA_BREEZE_STATE_FW_CHECK,         /**< Flash check, expects command 0x28 (Firmware check) to make sure the peer will not send anything in between. */
    OTA_BREEZE_STATE_RESET_PREPARE,    /**< Reset prepare, expects Tx-done event to make sure commands 0x25 (firmware check result) is sent. */
    OTA_BREEZE_STATE_UPGRADE_REPORT,   /**< Upgrade report, expects auth-done event to trigger the sending of command 0x26 (update result). */
} ota_breeze_state_t;

typedef struct _ota_ble_global_dat{
    ota_breeze_version_t verison;
    volatile uint8_t feature_enable;
    volatile uint8_t ota_breeze_task_active_ctrl;
    volatile uint8_t ota_breeze_task_active_flag;
    volatile uint8_t ota_breeze_status;
    uint32_t rx_fw_size;                /**< Size of firmware to be received. */
    uint32_t bytes_recvd;               /**< Size of firmware received. */
    uint16_t frames_recvd;              /**< Number of frames of firmware received. */
    uint16_t crc;
    uint32_t bank_1_addr;
}_ota_ble_global_dat_t;

typedef struct {
    ota_breeze_bin_type_t type;
    uint32_t magic;
} ota_breeze_bin_info_t;

_ota_ble_global_dat_t* ota_breeze_get_global_data_center(void);
void ota_breeze_get_data(uint8_t ota_cmd, uint8_t num_frame, uint8_t *buffer, uint32_t length);
void ota_breeze_relate_event(uint8_t event_type, uint8_t sub_status);
void ota_breeze_set_task_active_flag(volatile uint8_t flag);
volatile uint8_t ota_breeze_get_task_active_flag(void);
void ota_breeze_set_task_active_ctrl(volatile uint8_t is_enable);
volatile uint8_t ota_breeze_get_task_active_ctrl_status(void);
void ota_breeze_on_fw_data(uint8_t *buffer, uint32_t length, uint8_t num_frames);

#endif
