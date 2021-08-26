#ifdef __INTERCONNECTION__
#ifndef __APP_INTERCONNECTION_LOGIC_PROTOCOL_H__
#define __APP_INTERCONNECTION_LOGIC_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MTU_MAX_LENGTH                                          (521)

#define SERVICE_ID_LENGTH                                       (1)
#define COMMAND_ID_LENGTH                                       (1)

#define CCMP_GENERAL_USED_ERROR_CODE_TYPE                       (127)
#define CCMP_GENERAL_USED_ERROR_CODE_NO_ERROR                   (0)
#define CCMP_GENERAL_USED_ERROR_CODE_LEN                        (4)

typedef union
{
    uint32_t f;
    char c[4];
}UINT32_CONV_U;

typedef union
{
    uint16_t f;
    char c[2];
}UINT16_CONV_U;

// sof value used for ccmp communication
#define CCMP_SOF_VALUE                                          0x5A
#define CCMP_CONTROL                                            0x00


#define CCMP_ERR_HEAD_BASE                                      (100000)
#define CCMP_ERR_HEAD                                           (1)
#define CCMP_ERR_MODULE_BASE                                    (1000)

typedef enum
{
    CCMP_MODULE_GENERAL = 0,
    CCMP_MODULE_DEVICE_MANAGEMENT = 1,
    CCMP_MODULE_NOTIFICATION = 2,
    CCMP_MODULE_MUSIC = 6,
    CCMP_MODULE_OTA = 9,
    CCMP_MODULE_MUSIC_MANAGEMENT = 17,
    CCMP_MODULE_BT_TEST = 19,
    CCMP_MODULE_MCU_TEST = 20,
    CCMP_MODULE_MIDWARE_SERVICE = 33,
} CCMP_MODULE_E;

/**
 * @brief
 *
 * error code----start
 *
 */
// general used error code
typedef enum
{
    CCMP_GEN_ERR_CODE_SUCCESS = 0,
    CCMP_GEN_ERR_CODE_UNKNOWN = 1,
    CCMP_GEN_ERR_CODE_SERVICE_UNSUPPORTED = 2,
    CCMP_GEN_ERR_CODE_COMMAND_UNSUPPORTED = 3,
    CCMP_GEN_ERR_CODE_AUTHORITY_LIMITED = 4,
    CCMP_GEN_ERR_CODE_SYS_BUSY = 5,
    CCMP_GEN_ERR_CODE_REQUEST_FORMAT_ERROR = 6,
    CCMP_GEN_ERR_CODE_PARAM_ERROR = 7,
    CCMP_GEN_ERR_CODE_MALLOC_FAIL = 8,
    CCMP_GEN_ERR_CODE_RESPONSE_TIMEOUT = 9,
    CCMP_GEN_ERR_CODE_NOT_ENCRYPTED = 10,
    CCMP_GEN_ERR_CODE_DECRYPTION_FORMAT_ERROR = 11,
    CCMP_GEN_ERR_CODE_ILLEGAL_ECRYPTED_PACKET_INDEX = 12,
    CCMP_GEN_ERR_CODE_ENCRYPTED_DATA_DAMAGED = 13,
    CCMP_GEN_ERR_CODE_DATA_LOST = 14,
} CCMP_GEN_ERR_CODE_E;

// device management error code
typedef enum
{
    CCMP_DEV_ERR_CODE_ILLEGAL_PARAM = 1,
    CCMP_DEV_ERR_CODE_LOW_POWER = 2,
    CCMP_DEV_ERR_CODE_DEVICE_UNAVAILABLE = 3,
} CCMP_DEV_ERR_CODE_E;

// notification error code
typedef enum
{
    CCMP_NOTI_ERR_CODE_ILLEGAL_PAPAM = 1,
} CCMP_NOTI_ERR_CODE_E;


// OTA error code
typedef enum
{
    CCMP_OTA_ERR_CODE_ILLEGAL_PARM = 1,
    CCMP_OTA_ERR_CODE_LOW_BATTERY = 2,
    CCMP_OTA_ERR_CODE_FIRMWARE_ABNORMAL = 3,
    CCMP_OTA_ERR_CODE_FLASH_PROCESS_FAILURE = 4,
    CCMP_OTA_ERR_CODE_UPDATE_DENIED = 5,
    CCMP_OTA_ERR_CODE_FILE_TRANSFER_FAILURE = 6,
    CCMP_OTA_ERR_CODE_BT_DOWNLOAD_FAILURE = 7,
    CCMP_OTA_ERR_CODE_BT_UPDATE_FAILURE = 8,
    CCMP_OTA_ERR_CODE_DATA_WRITE_FAILURE = 9,
    CCMP_OTA_ERR_CODE_DATA_VERIFY_FAILURE = 10,
    CCMP_OTA_ERR_CODE_UPDATE_PACKET_ABNORMAL = 11,
} CCMP_OTA_ERR_CODE_E;


// music error code
typedef enum
{
    CCMP_MUSIC_ERR_CODE_ILLEGAL_PARAM = 1,
} CCMP_MUSIC_ERR_CODE_E;

// BT factory test error code
typedef enum
{
    CCMP_BT_TEST_ERR_CODE_ILLEGAL_PARAM = 1,
} CCMP_BT_TEST_ERR_CODE_E;


// MCU factory test error code
typedef enum
{
    CCMP_FACTORY_TEST_ERR_CODE_ILLEGAL_PARAM = 1,
} CCMP_FACTORY_TEST_ERR_CODE_E;


// midware service error code
typedef enum
{
    CCMP_MIDWARE_ERR_CODE_NO_HUAWEI_MUSIC = 1,
    CCMP_MIDWARE_ERR_CODE_USER_NOT_SIGNED = 2,
    CCMP_MIDWARE_ERR_CODE_NO_NETWORK = 3,
    CCMP_MIDWARE_ERR_CODE_ADD_FAILURE = 4,
    CCMP_MIDWARE_ERR_CODE_NO_MUSIC = 5,
    CCMP_MIDWARE_ERR_CODE_PLAYING_FAVORITE = 6,
    CCMP_MIDWARE_ERR_CODE_PLAYING_FAILURE = 7,
} CCMP_MIDWARE_ERR_CODE_E;
/**
 * @brief
 *
 * error code----end
 *
 */
typedef enum
{
    CCMP_GENERAL_COMMAND_ID_CONNECT_CCMP = 1,
} CCMP_GENERAL_COMMAND_ID_E;

// command id of service device management
typedef enum
{
    CCMP_DEVICE_COMMAND_ID_LINK_PARAM = 1,
    CCMP_DEVICE_COMMAND_ID_SERVICE_ABILITY = 2,
    CCMP_DEVICE_COMMAND_ID_COMMAND_ABILITY = 3,
    CCMP_DEVICE_COMMAND_ID_TIME_FORMAT = 4,
    CCMP_DEVICE_COMMAND_ID_SET_DEVICE_TIME = 5,
    CCMP_DEVICE_COMMAND_ID_GET_DEVICE_TIME = 6,
    CCMP_DEVICE_COMMAND_ID_DEVICE_INFO = 7,
    CCMP_DEVICE_COMMAND_ID_GET_MOBILE_BATT = 8,
} CCMP_DEVICE_COMMAND_ID_E;


// type id used for command CCMP_DEVICE_COMMAND_ID_LINK_PARAM
typedef enum
{
    TYPE_PROTOCOL_VERSION = 1,
    TYPE_MAX_FRAME_SIZE = 2,
    TYPE_MAX_TRANSIMISSION_UNIT = 3,
    TYPE_LINK_INTERVAL = 4,
    TYPE_RANDOM_VALUE = 5,
} LINK_PARAM_TYPE_E;


// type id used for command CCMP_DEVICE_COMMAND_ID_SERVICE_ABILITY
typedef enum
{
    TYPE_SUPPORT_SERVICE_REQUEST = 1,
    TYPE_SUPPORT_SERVICE_RESPONSE = 2,
} SERVICE_ABILITY_SUPPORT_TYPE_E;


// type id used for command CCMP_DEVICE_COMMAND_ID_DEVICE_INFO
typedef enum
{
    TYPE_BT_VERSION = 1,
    TYPE_DEVICE_TYPE = 2,
    TYPE_DEVICE_VERSION = 3,
    TYPE_DEVICE_PHONE_NUMBER = 4,
    TYPE_DEVICE_BT_MAC = 5,
    TYPE_DEVICE_IMEI = 6,
    TYPE_DEVICE_SOFTWARE_VERSION = 7,
    TYPE_DEVICE_OPEN_SOURCE_VERSION = 8,
    TYPE_DEVICE_SN = 9,
    TYPE_DEVICE_MODEL_ID = 10,
    TYPE_DEVICE_EMMC_ID = 11,
    TYPE_DEVICE_NAME = 12,
    TYPE_DEVICE_SUPPORT_HEALTH_APP = 13,
    TYPE_DEVICE_ID_TO_SERVER_TYPE = 14,
} DEVICE_INFO_TYPE_E;


// command id of service midware
typedef enum
{
    CCMP_MIDWARE_COMMAND_ID_HUAWEI_MUSIC = 3,
} CCMP_MIDWARE_COMMAND_ID_E;


// type id used for command CCMP_MIDWARE_COMMAND_ID_HUAWEI_MUSIC
typedef enum
{
    TYPE_FAVORITE_INFO = 1,
}HUAWEI_MUSIC_TYPE_E;


// event id used for type TYPE_FAVORITE_INFO
typedef enum
{
    HUAWEI_MUSIC_RESPONSE_ADD_FAVORITE = 1,
    HUAWEI_MUSIC_RESPONSE_PLAY_FAVORITE = 2,
} HUAWEI_MUSIC_RESPONSE_E;


// command id of service ota
typedef enum
{
    CCMP_OTA_COMMAND_ID_UPDATE_PERMISSION = 1,
    CCMP_OTA_COMMAND_ID_PARAM_NEGOTIATION = 2,
    CCMP_OTA_COMMAND_ID_DEVICE_REPORT = 3,
    CCMP_OTA_COMMAND_ID_FIRMWARE_DATA = 4,
    CCMP_OTA_COMMAND_ID_PACKAGE_SIZE = 5,
    CCMP_OTA_COMMAND_ID_PACKAGE_VALIDITY = 6,
    CCMP_OTA_COMMAND_ID_UPDATE_STATUS_REPORT = 7,
    CCMP_OTA_COMMAND_ID_UPDATE_CANCEL = 8,
    CCMP_OTA_COMMAND_ID_UPDATE_STATUS_NOTIFY = 9,
    CCMP_OTA_COMMAND_ID_UPDATE_REQUEST = 10,
} CCMP_OTA_COMMAND_ID_E;


// type id for command CCMP_OTA_COMMAND_ID_UPDATE_PERMISSION
typedef enum
{
    TYPE_PACKET_VERSION = 1,
    TYPE_COMPONENT_SIZE = 2,
    TYPE_OTA_WORK_MODE = 3,
    TYPE_BATTERY_THRESHOLD = 4,
} UPDATE_PERMISSION_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_PARAM_NEGOTIATION
typedef enum
{
    TYPE_APP_WAIT_TIMEOUT = 1,
    TYPE_RESTART_TIMEOUT = 2,
    TYPE_OTA_UNIT_SIZE = 3,
    TYPE_INTERVAL = 4,
    TYPE_ACK_ENABLE = 5,
} PARAM_NEGOTIATION_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_DEVICE_REPORT
typedef enum
{
    TYPE_FILE_OFFSET = 1,
    TYPE_FILE_LENGTH = 2,
    TYPE_FILE_BITMAP = 3,
} DEVICE_REPORT_TYPE_E;
#define FILE_OFFSET_LENGTH                                      4
#define FILE_LENGTH                                             4
#define FILE_BITMAP_LENGTH                                      1


// type id for command CCMP_OTA_COMMAND_ID_PACKAGE_SIZE
typedef enum
{
    TYPE_VALID_SIZE = 1,
    TYPE_FILE_SIZE = 2,
} PACKAGE_SIZE_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_PACKAGE_VALIDITY
typedef enum
{
    PACKAGE_VALIDITY = 1,
} PACKAGE_VALIDITY_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_UPDATE_STATUS_REPORT
typedef enum
{
    UPDATE_STATUS_REPORT = 1,
} UPDATE_STATUS_REPORT_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_UPDATE_CANCEL
typedef enum
{
    UPDATE_CANCEL = 1,
} UPDATE_CANCEL_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_UPDATE_STATUS_NOTIFY
typedef enum
{
    UPDATE_STATUS_NOTIFY = 1,
} UPDATE_STATUS_NOTIFY_TYPE_E;


// type id for command CCMP_OTA_COMMAND_ID_UPDATE_REQUEST
typedef enum
{
    UPDATE_REQUEST = 1,
} UPDATE_REQUEST_TYPE_E;

typedef enum
{
    UPDATE_PARAM_RECEIVE_FAIL = 0,
    UPDATE_PARAM_RECEIVE_SUCCESS = 1,
} UPDATE_PARAM_RECEIVE_STATUS_E;


typedef struct
{
    uint8_t sof;
    uint16_t length;
    uint8_t ccmpControl;
    uint8_t serviceID;
    uint8_t commandID;
    uint8_t item[1];
}__attribute__((__packed__))CCMP_COMMUNICATION_HEAD_T;
#define CCMP_PAYLOAD_OFFSET (OFFSETOF(CCMP_COMMUNICATION_HEAD_T, ccmpControl))
#define CCMP_ITEM_OFFSET    (OFFSETOF(CCMP_COMMUNICATION_HEAD_T, item))

uint32_t uint32_convert_endian(uint32_t data);

uint16_t uint16_convert_endian(uint16_t data);

uint8_t app_interconnection_ccmp_creat_error_code(uint8_t module, uint8_t errType, uint32_t* dataPtr);

void app_interconnection_spp_client_send_ccmp_init_command();

void app_interconnection_package_size_report_handler(uint32_t packageSize, uint32_t receivedSize);

void app_interconnection_ota_package_validity_response(bool validity);

void ota_update_request(void);

void app_interconnection_ota_apply_update_data(uint32_t fileOffset, uint8_t fileBitmap, uint32_t fileLength);

uint8_t app_interconnection_spp_client_data_received_callback(uint8_t* ptrData, uint32_t dataLength);

void app_interconnection_handle_favorite_music_through_ccmp(uint8_t process);

uint8_t app_interconnection_mbb_data_received_callback(uint8_t* ptrData, uint16_t dataLength);

void app_interconnection_ccmp_negotiation_data_send(uint8_t *param, uint16_t paramLen);

void app_interconnection_ccmp_data_send(uint8_t *param, uint8_t paramLen);

void app_interconnection_spp_server_data_send(uint8_t *param, uint16_t paramLen);

void app_interconnection_response_error_code(uint8_t module, uint8_t err, uint8_t serviceId, uint8_t commandId);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __APP_INTERCONNECTION_LOGIC_PROTOCOL_H__
#endif // #ifdef __INTERCONNECTION__
