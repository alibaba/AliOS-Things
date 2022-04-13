#ifdef __INTERCONNECTION__
#ifndef __APP_INTERCONN_OTA_H__
#define __APP_INTERCONN_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define APP_WAIT_TIMEOUT                    60//second
#define DEVICE_RESTART_TIMEOUT              60
#define OTA_UNIT_SIZE_SPP                   512
#define OTA_UNIT_SIZE_BLE                   256
#define INTERVAL                            10//ms
#define ACK_ENABLE


#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define OTA_DATA_BUFFER_SIZE_FOR_BURNING    FLASH_SECTOR_SIZE_IN_BYTES

#define OTA_START_MAGIC_CODE                (0x54534542)  // BEST
#define NORMAL_BOOT                         0xBE57EC1C
#define COPY_NEW_IMAGE                      0x5a5a5a5a

#ifndef DATA_ACK_FOR_SPP_DATAPATH_ENABLED
#define DATA_ACK_FOR_SPP_DATAPATH_ENABLED   0
#endif

#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET              (0x200000)
#endif

#ifndef __APP_IMAGE_FLASH_OFFSET__
#define __APP_IMAGE_FLASH_OFFSET__          (0x18000)
#endif

#define OTA_FLASH_LOGIC_ADDR                (FLASH_NC_BASE)
#define OTA_FLASH_OFFSET_OF_IMAGE           __APP_IMAGE_FLASH_OFFSET__
#define OTA_FLASH_ENTRY_OFFSET              (OTA_FLASH_LOGIC_ADDR + OTA_FLASH_OFFSET_OF_IMAGE)

#define BD_ADDR_LENGTH                      6
#define NAME_LENGTH                         32
#define OTA_BLE_DATA_PACKET_MAX_SIZE        257
#define OTA_BT_DATA_PACKET_MAX_SIZE         513

#define OTA_BOOT_INFO_FLASH_OFFSET          0x1000
// #define otaUpgradeLogInFlash                (*(FLASH_OTA_UPGRADE_LOG_FLASH_T *)(OTA_FLASH_LOGIC_ADDR+0x3000))
// #define otaUpgradeLog                       otaUpgradeLogInFlash

/**
 * @brief The format of the otaBootInfo
 *
 */
typedef struct
{
    uint32_t magicNumber;
    uint32_t imageSize;
    uint32_t imageCrc;
}__attribute__ ((__packed__)) FLASH_OTA_BOOT_INFO_T;

// typedef struct
// {
//     uint8_t     version_md5[32];
//     uint32_t    upgradeSize[(FLASH_SECTOR_SIZE_IN_BYTES - 32) / 4];
// }__attribute__ ((__packed__)) FLASH_OTA_UPGRADE_LOG_FLASH_T;


// typedef struct
// {
//     uint8_t version_md5[32];
// }__attribute__ ((__packed__)) OTA_RESUME_VERIFY_T;

// typedef struct
// {
//     bool breakpoint_flag;
//     uint32_t offset;
// }__attribute__ ((__packed__)) OTA_RESUME_VERIFY_RESPONSE_T;

typedef struct
{
    uint8_t*    dataBufferForBurning;   // temp buffer pointer for image data wite into flash
    uint8_t*    imageInfoPtr;           // pointer for image information
    uint32_t    dstFlashOffsetForNewImage;
    uint32_t    offsetInDataBufferForBurning;
    uint32_t    offsetInFlashToProgram;
    uint32_t    totalImageSize; // image size to apply
    uint32_t    imageRequestTime;
    uint32_t    imageLastRequestSize;
    uint8_t     imageLastRequestBitmapMask;
    uint32_t    alreadyReceivedDataSizeOfImage;
    uint32_t    addressOfStartApplyImage; // start applied data offset of the whole image
    uint32_t    offsetInFlashOfCurrentSegment;
    uint32_t    offsetOfImageOfCurrentSegment;
    uint32_t    crcOfImage;
    uint32_t    crcOfSegment;
    uint32_t    flasehOffsetOfUserDataPool;
    uint32_t    flasehOffsetOfFactoryDataPool;
    uint32_t    startLocationToWriteImage;
    uint32_t    endLocationToWriteImage;
    uint8_t     isOTAInProgress;
    uint8_t     isPendingForReboot;
    uint16_t    dataPacketSize;
    bool        isUpdateNecessary;
    bool        resume_at_breakpoint;
    uint32_t    breakPoint;
    uint32_t    i_log;
    uint8_t     otaWorkMode;
    uint8_t     componentSize;
    uint8_t     batteryThreshold;
    uint16_t    appWaitTimeout;
    uint16_t    deviceRestartTimeout;
    uint16_t    otaUnitSize;
    uint16_t    packetInterval;
    uint8_t     ackEnable;
    uint8_t     bitmap;
    bool        isImageInfoReceived;
    uint32_t    receivedImageInfoLength;
    bool        isLastPacketReceived;
    uint8_t     packetNum;
    bool        isAskAgain;
    uint8_t     askAgainPacketNum;
    uint8_t     askAgainPacketTotal;
    uint8_t     updateDataChannel;
    uint8_t     shaOfImg[32];
} __attribute__((__packed__)) INTERCONN_OTA_ENV_T;

#define  PACKETCRC_RESULT_TYPE  0x01
#define  PACKETCRC_RESULT_LENGHT  0x01

typedef struct
{
    uint16_t       funcCode;
    uint8_t        result_type;
    uint8_t        result_length;
    uint8_t        result_value;
}__attribute__ ((__packed__)) ota_packetCRC_report_t;


#define IMAGE_RECV_FLASH_CHECK       1  // It's best to turn it on durning development and not a big deal off in the release.
#define MAX_IMAGE_SIZE              ((uint32_t)(NEW_IMAGE_FLASH_OFFSET - __APP_IMAGE_FLASH_OFFSET__))
#define MIN_SEG_ALIGN               256

void app_interconnection_ota_set_update_flag(bool flag);

bool app_interconnection_ota_get_update_flag(void);

uint8_t app_interconnection_ota_get_battery_threshold(void);

uint32_t app_interconnection_ota_get_file_offset();

void app_interconnection_ota_set_ota_in_progress(uint8_t value);

uint8_t app_interconnection_ota_get_file_bitmap();

bool app_interconnection_ota_is_image_info_received(void);

void app_interconnection_ota_image_info_received_handler(uint8_t *ptrParam, uint32_t paramLen);

void app_interconnection_ota_fw_data_received_handler(uint8_t* ptrData, uint32_t DataLength, bool pass);

void app_interconn_ota_handler_init(void);

void app_interconnection_ota_finished_handler(void);

uint8_t app_interconn_get_ota_status();

void app_interconnection_ota_firmware_verify_handler(void);

void ota_segmemt_verify_handler(uint8_t* ptrParam, uint32_t paramLen);

INTERCONN_OTA_ENV_T *app_interconnection_ota_get_env_ptr();

uint8_t app_interconnection_ota_get_update_data_channel(void);

void app_interconnection_ota_set_update_data_channel(uint8_t channel);

#ifdef __cplusplus
}
#endif
#endif  // __APP_TENCENT_OTA_H__
#endif