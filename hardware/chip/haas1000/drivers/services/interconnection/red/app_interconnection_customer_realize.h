#ifdef __INTERCONNECTION__
#ifndef __APP_INTERCONNECTION_CUSTOMER_REALIZE_H__
#define __APP_INTERCONNECTION_CUSTOMER_REALIZE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_interconnection.h"
// #define H2_FOR_TEST                 1

#define BT_VERSION                  "NA"
#define DEVICE_HARDWARE_VERSION     "\x00\x00\x00\x00\x00\x00\x00\x00"
#define PHONE_NUMBER                "NA"
#define DEVICE_BT_MAC               "NA"
#define DEVICE_IMEI                 "NA"
#define SOFTWARE_VERSION            "1.0.1.223"
#define DEVICE_OPEN_SOURCE_VERSION  "NA"
#define DEVICE_SN                   "\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01"
#ifdef H2_FOR_TEST
#define DEVICE_MODEL_ID             "BTFCMH2S-000016"
#define DEVICE_NAME                 "CM-H2S"
#else
#define DEVICE_MODEL_ID             "BTFCM70-00001B"
#define DEVICE_NAME                 "CM-70"
#endif
#define DEVICE_EMMC                 "NA"

#define CCMP_SUMMARY_VALUE          "\xe5\xef\xcb\x85\xeb\x43\x77\xa5\x18\x09\x0e\x9d\xcd\x77\xb9\xbf\xb4\x3e\xa0\xeb\xf4\xb8\x6a\x81\x1f\xd9\x46\x2a\x34\x92\x68\x12"
#define BUSINESS_TAG                "iconnect"
#define FIXED_IMAGE_INFO_HEADER     0x5A5A
#define OTA_COMPONENT_ID_HEAD_SET   1
#define FORCE_UPGRADE               1

#define OTA_FLASH_SIZE              0x180000 // 1.5M flash


typedef struct
{
    uint8_t serviceID;
    uint8_t support;
} __attribute__((__packed__)) SUPPORT_SERVICE_T;

#ifdef H2_FOR_TEST
// used for h2
typedef struct
{
    uint16_t h_header;
    uint16_t h_head_lens;
    uint8_t  h_upgrade_mode;
    uint8_t	 h_image_version[19];
    uint8_t  h_headset_firmware_flag;
    uint32_t h_headset_addr_offset;
    uint32_t h_headset_data_lens;
    uint8_t  h_headset_version[19];
    uint8_t  h_headset_figure_signature[256];
    uint32_t h_headset_crc;
    uint8_t  h_hinttone_firmware_flag;
    uint32_t h_hinttone_addr_offset;
    uint32_t h_hinttone_data_lens;
    uint8_t  h_hinttone_version[19];
    uint8_t  h_hinttone_figure_signature[256];
    uint32_t h_hinttone_crc;
    uint8_t  h_charger_firmware_flag;
    uint32_t h_charger_addr_offset;
    uint32_t h_charger_data_lens;
    uint8_t  h_charger_version[19];
    uint8_t  h_charger_figure_signature[256];
    uint32_t h_charger_crc;
    uint32_t h_data_crc;
} __attribute__ ((__packed__)) REC_HEAD_PACKET_T;
#else
// used for amazon
typedef struct
{
    uint16_t    h_header;
    uint16_t    h_head_lens;
    uint8_t     h_upgrade_mode;
    uint8_t     h_image_version[19];
    uint8_t     h_headset_firmware_flag;
    uint32_t    h_headset_addr_offset;
    uint32_t    h_headset_data_lens;
    uint8_t     h_headset_version[19];
    uint8_t     h_headset_figure_signature[256];
    uint32_t    h_headset_crc;
    uint32_t    h_data_crc;
} __attribute__ ((__packed__)) REC_HEAD_PACKET_T;
#endif

#define IMAGE_INFO_SIZE                 sizeof(REC_HEAD_PACKET_T)

uint32_t crc32_image(uint32_t crc_val, uint8_t *input, uint32_t len);

uint8_t *get_link_param_protocol_version(void);

bool is_service_support(uint8_t serviceID);

char *get_bt_version_ptr(void);

uint8_t *get_device_type_ptr(void);

char *get_hardware_version_ptr(void);

char *get_phone_number_ptr(void);

char *get_device_bt_mac_ptr(void);

char *get_device_imei_ptr(void);

char *get_software_version_ptr(void);

char *get_open_source_version_ptr(void);

char *get_sn_ptr(void);

char *get_model_id_ptr(void);

char *get_device_emmc_ptr(void);

char *get_device_name_ptr(void);

char *get_summary_value_ptr(void);

char *get_business_tag_ptr(void);

uint32_t get_update_permission_error_code(void);

uint8_t customer_realize_image_info_receive_finished_hook();

#ifdef __cplusplus
}
#endif
#endif
#endif