#ifdef __INTERACTION__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define APP_WAIT_TIMEOUT                    60//second
#define DEVICE_RESTART_TIMEOUT              60
#define OTA_UNIT_SIZE_SPP                   512
#define INTERVAL                            10//ms
#define ACK_ENABLE


#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define OTA_DATA_BUFFER_SIZE_FOR_BURNING    FLASH_SECTOR_SIZE_IN_BYTES
#define INTERACTION_OTA_NORFLASH_BUFFER_LEN    (FLASH_SECTOR_SIZE_IN_BYTES*2)

#define OTA_START_MAGIC_CODE                (0x54534542)  // BEST
#define NORMAL_BOOT                         0xBE57EC1C
#define COPY_NEW_IMAGE                      0x5a5a5a5a


#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET              (0x200000)
#endif

#ifndef __APP_IMAGE_FLASH_OFFSET__
#define __APP_IMAGE_FLASH_OFFSET__          (0x18000)
#endif

#define OTA_FLASH_LOGIC_ADDR                (FLASH_NC_BASE)
#define OTA_FLASH_OFFSET_OF_IMAGE           __APP_IMAGE_FLASH_OFFSET__
#define OTA_FLASH_ENTRY_OFFSET              (OTA_FLASH_LOGIC_ADDR + OTA_FLASH_OFFSET_OF_IMAGE)
#define OTA_FLASH_ENTRY2_OFFSET             (OTA_FLASH_LOGIC_ADDR + NEW_IMAGE_FLASH_OFFSET)

#define BD_ADDR_LENGTH                      6
#define NAME_LENGTH                         32
#define OTA_BLE_DATA_PACKET_MAX_SIZE        257
#define OTA_BT_DATA_PACKET_MAX_SIZE         513

#define OTA_BOOT_INFO_FLASH_OFFSET          0x1000

/**
 * @brief The format of the otaBootInfo
 *
 */

typedef struct
{
	uint8_t version_md5[4];
}__attribute__ ((__packed__)) INTERACTION_OTA_RESUME_VERIFY_T;

typedef struct
{
    uint32_t magicNumber;
    uint32_t imageSize;
    uint32_t imageCrc;
}__attribute__ ((__packed__)) FLASH_OTA_BOOT_INFO_T;

typedef struct
{
    uint8_t     version_md5[4];
    uint32_t    upgradeDone;
    uint32_t    upgradeSize[(FLASH_SECTOR_SIZE_IN_BYTES - 8) / 4];
}__attribute__ ((__packed__)) FLASH_OTA_UPGRADE_LOG_FLASH_T;


typedef struct
{
    uint8_t    isToRenameBT;
    uint8_t    newBTName[32];
    uint8_t    isToRenameBLE;
    uint8_t    newBLEName[32];
    uint8_t    isToUpdateBTAddr;
    uint8_t    newBTAddr[6];
    uint8_t    isToUpdateBLEAddr;
    uint8_t    newBLEAddr[6];
    uint32_t    startLocationToWriteImage;    // the offset of the flash to start writing the image
}__attribute__ ((__packed__))INTERACTION_OTA_CONFIGURATION_T;

typedef struct
{
  uint8_t   dataBufferForBurning[OTA_DATA_BUFFER_SIZE_FOR_BURNING];
  uint32_t  dstFlashOffsetForNewImage;
  uint32_t  offsetInDataBufferForBurning;
  uint32_t  offsetInFlashToProgram;
  uint32_t  totalImageSize;
  uint32_t  alreadyReceivedDataSizeOfImage;
  uint32_t  offsetInFlashOfCurrentSegment;
  uint32_t  offsetOfImageOfCurrentSegment;
  uint32_t  crc32OfImage;
  uint32_t  crc32OfSegment;
  uint32_t  flashOffsetOfUserDataPool;
  uint32_t  flasehOffsetOfFactoryDataPool;
  uint8_t   version_md5[4];

  INTERACTION_OTA_CONFIGURATION_T configuration;

  uint8_t 	 isOTAInProgress;
  uint8_t    isReadyToApply;

 // uint8_t     resumeType;
  bool        resume_at_breakpoint;
  uint32_t    breakPoint;
  uint32_t    i_log;
  
} __attribute__((__packed__)) INTERACTION_OTA_ENV_T;


typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t		    idenfifier[4];    //is the MD5 of file;
		uint8_t         type;
} __attribute__((__packed__)) INTERACTION_OTA_SYNC_REQ_T;

typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t			status;
		uint8_t			type;
		uint8_t			resumePointTpye;
		uint8_t			blockCheck;
		uint32_t		blockSize;
		uint16_t		transferSize;
		uint8_t		    idenfifier[4]; 
} __attribute__((__packed__)) INTERACTION_OTA_SYNC_CFM_T;


typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t			type;
		uint8_t			resumePoint;
		uint32_t		imageSize;
} __attribute__((__packed__)) INTERACTION_OTA_START_REQ_T;

typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t			status;
		uint8_t			type;
		uint8_t			resumePoint;
		uint32_t		startOffset;
} __attribute__((__packed__)) INTERACTION_OTA_START_CFM_T;

typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint32_t		startOffset;
		uint8_t         *data;
} __attribute__((__packed__)) INTERACTION_OTA_DATA_REQ_T;

typedef struct
{
		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t		    status;
		uint16_t        delayTime;
} __attribute__((__packed__)) INTERACTION_OTA_DATA_CFM_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint32_t		startOffset   ;
		uint32_t        crc;
} __attribute__((__packed__)) INTERACTION_OTA_BLOCK_VERIFY_REQ_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t		    status;
} __attribute__((__packed__)) INTERACTION_OTA_BLOCK_VERIFY_CFM_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
} __attribute__((__packed__)) INTERACTION_OTA_VALIDATION_DONE_REQ_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t		    status;
		uint16_t        delayTime;
} __attribute__((__packed__)) INTERACTION_OTA_VALIDATION_DONE_CFM_T;


typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t        status;
} __attribute__((__packed__)) INTERACTION_OTA_TRANSFER_COMPELETE_REQ_T;
typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t         status;
} __attribute__((__packed__)) INTERACTION_OTA_TRANSFER_COMPELETE_CFM_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
} __attribute__((__packed__)) INTERACTION_OTA_COMMIT_REQ_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t         status;
} __attribute__((__packed__)) INTERACTION_OTA_COMMIT_CFM_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
} __attribute__((__packed__)) INTERACTION_OTA_ABORT_REQ_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t         status;
} __attribute__((__packed__)) INTERACTION_OTA_ABORT_CFM_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t			errorCode;
} __attribute__((__packed__)) INTERACTION_OTA_ERROR_REQ_T;

typedef struct
{
  		uint16_t		cmdID;
		uint8_t			transferID;
		uint16_t		paramLen;
		uint8_t			starus;
} __attribute__((__packed__)) INTERACTION_OTA_ERROR_CFM_T;


typedef enum
{
    OTA_BLOCK_NO_CHECK = 0x00,
	OTA_BLOCK_CHECK,
	
}INTERACTION_OTA_UPGRADE_BLOCK_CHECK_T;


typedef enum 
{
    OTA_UPGRADE_RESUME_POINT_START=0x00,
	OTA_UPGRADE_RESUME_POINT_PRE_VALIDATE,
	OTA_UPGRADE_RESUME_POINT_PRE_UPGRADE,
	OTA_UPGRADE_RESUME_POINT_POST_UPGRADE,
} INTERACTION_OTA_UPGRADE_STATUS_T;

typedef enum
{
	OTA_BIN_OF_EARPHONE=0x01,
	OTA_BIN_OF_BOX,
}INTERACTION_OTA_UPGRADE_BIN_TYPE_T;


#define  PACKETCRC_RESULT_TYPE  0x01
#define  PACKETCRC_RESULT_LENGHT  0x01

#define IMAGE_RECV_FLASH_CHECK       1  // It's best to turn it on durning development and not a big deal off in the release.
#define MAX_IMAGE_SIZE              ((uint32_t)(NEW_IMAGE_FLASH_OFFSET - __APP_IMAGE_FLASH_OFFSET__))
#define MIN_SEG_ALIGN               256

void ota_breakpoint_log(uint32_t breakpoint); 

void app_interaction_ota_set_update_flag(bool flag);

bool app_interaction_ota_get_update_flag(void);

uint8_t app_interaction_ota_get_battery_threshold(void);

uint32_t app_interaction_ota_get_file_offset();

void app_interaction_ota_set_ota_in_progress(uint8_t value);

uint8_t app_interaction_ota_get_file_bitmap();

bool app_interaction_ota_is_image_info_received(void);

void app_interaction_ota_image_info_received_handler(uint8_t *ptrParam, uint32_t paramLen);

void app_interaction_ota_fw_data_received_handler(uint8_t* ptrData, uint32_t DataLength, bool pass);

void app_interaction_ota_handler_init(void);

void app_interaction_ota_finished_handler(void);

uint8_t app_interaction_get_ota_status();

void app_interaction_ota_firmware_verify_handler(void);

void ota_segmemt_verify_handler(uint8_t* ptrParam, uint32_t paramLen);

INTERACTION_OTA_ENV_T *app_interaction_ota_get_env_ptr();


void app_interaction_ota_upgrade_is_validation_done_rsp();
void app_interaction_ota_upgrade_sys_rsp();


void app_interaction_ota_upgrade_sys_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_upgrade_start_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_upgrade_data_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_upgrade_block_verify_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_upgrade_is_validation_done_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_finished_handler(void);
void app_interaction_ota_upgrade_is_complete_hanlder(void);
void app_interaction_ota_upgrade_commit_hanlder(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_ota_upgrade_abort_handler(uint8_t* ptrData, uint32_t dataLength);


#ifdef __cplusplus
}
#endif
 
#endif
