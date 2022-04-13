/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifdef NEW_NV_RECORD_ENALBED

#ifndef __NVRECORD_EXTENSION_H__
#define __NVRECORD_EXTENSION_H__
#include "bluetooth.h"
#include "me_api.h"
#include "nvrecord_wifi.h"

// increase by 1 if the nvrecord's whole data structure is changed and the content needs to be rebuilt
#define NV_EXTENSION_MAJOR_VERSION 2
// increase by 1 if the new items are appended to the tail of the former nvrecord's data structure
#define NV_EXTENSION_MINOR_VERSION 1

#define NV_EXTENSION_SIZE 4096                              // one flash page
#define NV_EXTENSION_HEADER_SIZE sizeof(NVRECORD_HEADER_T)  // magic number and valid length
#define NV_EXTENSION_MAGIC_NUMBER 0x4E455854
#define NV_EXTENSION_VALID_LEN (sizeof(NV_EXTENSION_RECORD_T) - sizeof(NVRECORD_HEADER_T))

/* unused, just for backwards compatible */
#define section_name_ddbrec "ddbrec"

/* BT paired device info */
#define MAX_BT_PAIRED_DEVICE_COUNT 8

/* BLE paired device information */
#define BLE_RECORD_NUM 5

#define BLE_ADDR_SIZE 6
#define BLE_ENC_RANDOM_SIZE 8
#define BLE_LTK_SIZE 16
#define BLE_IRK_SIZE 16

#define BLE_STATIC_ADDR 0
#define BLE_RANDOM_ADDR 1

#ifdef GFPS_ENABLED
/* fast pair account key */
#define FP_ACCOUNT_KEY_RECORD_NUM 5
#define FP_ACCOUNT_KEY_SIZE 16
#define FP_MAX_NAME_LEN 64
#endif

// TODO: should be increased if NV_EXTENSION_MIRROR_RAM_SIZE exceeds this value

#define TILE_INFO_SIZE 400
#define BT_FREQENCY_RANGE_NUM   3
#define BT_IQ_INVALID_MAGIC_NUM 0xFFFFFFFF
#define BT_IQ_VALID_MAGIC_NUM   0x5a5a5a5a
typedef struct
{
    uint32_t validityMagicNum;
    uint16_t gain_cal_val[BT_FREQENCY_RANGE_NUM];
    uint16_t phase_cal_val[BT_FREQENCY_RANGE_NUM];
} BT_IQ_CALIBRATION_CONFIG_T;

/* nv record header data structure */
typedef struct
{
    uint32_t magicNumber;
    uint16_t majorVersion;  // should be NV_EXTENSION_MAJOR_VERSION
    uint16_t minorVersion;  // should be NV_EXTENSION_MINOR_VERSION
    uint32_t validLen;      // should be the valid content in this nv record version
    uint32_t crc32;         // crc32 of following valid values in the nv extention section
} NVRECORD_HEADER_T;

/* system information */
typedef struct {
    int8_t language;
} media_language_t;

#if defined(APP_LINEIN_A2DP_SOURCE) || defined(APP_I2S_A2DP_SOURCE)
typedef struct {
    int8_t src_snk_mode;
} src_snk_t;
#endif

typedef struct {
    uint32_t mode;
    btif_device_record_t record;
    bool tws_connect_success;
} ibrt_mode_t;

typedef struct {
    uint32_t status;
} factory_tester_status_t;

#ifdef IS_MULTI_AI_ENABLED
typedef struct {
    bool    voice_key_enable;
    uint8_t setedCurrentAi; //if false, set ai default mode
    uint8_t currentAiSpec;  //
    uint8_t aiStatusDisableFlag;        //all ai disable flag
    uint8_t amaAssistantEnableStatus;   //ama enable flag
} AI_MANAGER_INFO_T;
#endif

struct nvrecord_env_t {
    media_language_t media_language;
#if defined(APP_LINEIN_A2DP_SOURCE) || defined(APP_I2S_A2DP_SOURCE)
    src_snk_t src_snk_flag;
#endif
    ibrt_mode_t ibrt_mode;
    factory_tester_status_t factory_tester_status;
#if defined(__TENCENT_VOICE__)
    uint8_t flag_value[8];
#endif

#ifdef IS_MULTI_AI_ENABLED
    AI_MANAGER_INFO_T aiManagerInfo;
#endif
};

typedef struct btdevice_volume {
    int8_t a2dp_vol;
    int8_t hfp_vol;
} btdevice_volume;

typedef struct btdevice_profile {
    bool hfp_act;
    bool hsp_act;
    bool a2dp_act;
    uint8_t a2dp_codectype;
} btdevice_profile;

typedef struct {
    btif_device_record_t record;
    btdevice_volume device_vol;
    btdevice_profile device_plf;
} nvrec_btdevicerecord;

typedef struct {
    uint32_t pairedDevNum;
    nvrec_btdevicerecord pairedBtDevInfo[MAX_BT_PAIRED_DEVICE_COUNT];
} NV_RECORD_PAIRED_BT_DEV_INFO_T;

typedef enum {
    section_usrdata_ddbrecord,
    section_none
} SECTIONS_ADP_ENUM;

#if defined(GSOUND_OTA_ENABLED)&&defined(VOICE_DATAPATH)
typedef enum {
    GSOUND_OTA_STATUS_NONE        = 0,
    GSOUND_OTA_STATUS_IN_PROGRESS = 1,
    GSOUND_OTA_STATUS_COMPLETE    = 2,
    GSOUND_OTA_STATUS_NUM,
} GSOUND_OTA_STATUS_E;
#endif

typedef struct {
    uint8_t ble_addr[BTIF_BD_ADDR_SIZE];
    uint8_t ble_irk[BLE_IRK_SIZE];
} BLE_BASIC_INFO_T;

typedef struct {
    uint8_t peer_bleAddr[BLE_ADDR_SIZE];
    uint16_t EDIV;
    uint8_t RANDOM[BLE_ENC_RANDOM_SIZE];
    uint8_t LTK[BLE_LTK_SIZE];
    uint8_t IRK[BLE_IRK_SIZE];
    uint8_t bonded;

} BleDeviceinfo;

typedef struct {
    uint32_t saved_list_num;
    BLE_BASIC_INFO_T self_info;
    BleDeviceinfo ble_nv[BLE_RECORD_NUM];
} NV_RECORD_PAIRED_BLE_DEV_INFO_T;

#ifdef TWS_SYSTEM_ENABLED
typedef struct {
    BLE_BASIC_INFO_T ble_info;
} TWS_INFO_T;
#endif  // #ifdef TWS_SYSTEM_ENABLED

#ifdef GFPS_ENABLED
typedef struct {
    uint8_t key[FP_ACCOUNT_KEY_SIZE];

} NV_FP_ACCOUNT_KEY_ENTRY_T;

typedef struct {
    uint32_t key_count;
    NV_FP_ACCOUNT_KEY_ENTRY_T accountKey[FP_ACCOUNT_KEY_RECORD_NUM];
    uint16_t nameLen;
    uint8_t name[FP_MAX_NAME_LEN];
} NV_FP_ACCOUNT_KEY_RECORD_T;
#endif  // #ifdef GFPS_ENABLED

#ifdef NVREC_BAIDU_DATA_SECTION
/*  DMA owned configuration information */
typedef struct {
    int32_t fmfreq;
    char rand[BAIDU_DATA_RAND_LEN + 1];

} NV_DMA_CONFIGURATION_T;
#endif  // #ifdef NVREC_BAIDU_DATA_SECTION

#ifdef TGENIE_MESH_DATA_SECTION
#define NETKEY_MAX_NUM 2
#define APPKEY_MAX_NUM 10
#define GROUP_MAX_CNT 10



#define GENIE_TIME_CNT 40
#define GENIE_TIME_SIZE 4

/*  MESH data information */

typedef struct{
    uint8_t def_trans;

#ifdef CONFIG_MESH_MODEL_GEN_ONOFF_SRV
    uint8_t default_onoff;
    uint8_t last_onoff;
#endif

    uint8_t range_status;

#ifdef CONFIG_MESH_MODEL_GEN_LEVEL_SRV
    int16_t default_level;
    int16_t last_level;
#endif

    uint16_t default_actual;
    uint16_t last_actual;
    uint16_t min_actual;
    uint16_t max_actual;

    //temp
    uint16_t default_temp;
    uint16_t last_temp;
    uint16_t min_temp;
    uint16_t max_temp;

    //uint16_t default_UV;
    //uint16_t last_UV;

#if 0
    //hue
    uint16_t default_hue;
    uint16_t last_hue;
    uint16_t min_hue;
    uint16_t max_hue;

    //sat
    uint16_t default_sat;
    uint16_t last_sat;
    uint16_t min_sat;
    uint16_t max_sat;
#endif
}  MESH_POWERUP;

typedef struct
{
    uint8_t appkey[APPKEY_MAX_NUM][16];
    uint8_t netkey[NETKEY_MAX_NUM][16];
    uint16_t group[GROUP_MAX_CNT];
}CONFIG_STATUS;

typedef struct
{
    uint32_t duration;
    uint32_t times;
}MESH_IND;

typedef struct
{
    uint16_t period_time;
    uint8_t retry_delay;
    uint8_t retry_times;
}TIME_ADJUST;

typedef struct
{
    uint32_t time;
    uint16_t attr_type;
    uint8_t attr_param;
    uint8_t rsv;
}TIMER_SETTING;

typedef struct
{
    uint16_t h24_time;
    uint8_t schedule;
    uint8_t rsv1;
    uint16_t attr_type;
    uint8_t attr_param;
    uint8_t rsv2;
}PERIOD_TIMER_SETTING;

typedef struct 
{
    union
    {
        TIMER_SETTING timer;
        PERIOD_TIMER_SETTING period_timer;
    }t;
    //bit1,bit0, timer type
    //00'b- not set;
    //01'b, timer set,
    //10'b, period timer set.
    //bit7, bit6,// use status
    //01'b, in use, not time out
    //10'b, in use, time out.
    uint8_t flag;
    uint32_t timer_cur_time;//system seconds.
    uint32_t timer_cur_unix_time;//unix seconds.
}VENDOR_TIMER;

typedef struct
{
    uint32_t unix_time;
    uint32_t cur_time;//system seconds.
    //bit 0, for unix time; 
    //bit 1, for time adjust set.
    //bit 2, for time zone.
    //bit 3, for time update
    uint16_t set_flag;
    int8_t time_zone;
    uint8_t used_timer_num;
    
    TIME_ADJUST time_adjust;
    VENDOR_TIMER timer[GENIE_TIME_CNT];
}MESH_TIME_SCENE;

typedef struct
{
    uint16_t addr;//unicast address
    uint8_t times;//valid onoff times, for mode (5 times for factory mode)
    uint8_t network_id;
    uint32_t seq_number;//this should not be removed after node reset.
    MESH_POWERUP light_st;
    MESH_TIME_SCENE time_scene;
    uint8_t mesh_data[600];//more than sizeof(struct bt_mesh_net);
} NV_MESH_DATA_T;


#endif

#if TILE_DATAPATH
typedef struct {
    uint8_t tileInfo[TILE_INFO_SIZE];
} NV_TILE_INFO_CONFIG_T;
#endif

#if defined(GSOUND_OTA_ENABLED)&&defined(VOICE_DATAPATH)
typedef struct {
    uint8_t isGsoundEnabled;
    uint8_t gsoundOtaStatus;
    uint32_t gsoundOtaOffset;
    uint32_t gsoundOtaImageSize;
    uint8_t gsoundOtaSessionString[16];
} NV_GSOUND_INFO_T;
#endif

typedef struct {
    NVRECORD_HEADER_T header;
    struct nvrecord_env_t system_info;
    NV_RECORD_PAIRED_BT_DEV_INFO_T bt_pair_info;
    NV_RECORD_PAIRED_BLE_DEV_INFO_T ble_pair_info;

#ifdef TWS_SYSTEM_ENABLED
    TWS_INFO_T tws_info;
#endif

#ifdef GFPS_ENABLED
    NV_FP_ACCOUNT_KEY_RECORD_T fp_account_key_rec;
#endif

#ifdef NVREC_BAIDU_DATA_SECTION
    NV_DMA_CONFIGURATION_T dma_config;
#endif

#ifdef TILE_DATAPATH
    NV_TILE_INFO_CONFIG_T tileConfig;
#endif

#if defined(GSOUND_OTA_ENABLED)&&defined(VOICE_DATAPATH)
    NV_GSOUND_INFO_T gsound_info;
#endif

#ifdef TX_IQ_CAL
    BT_IQ_CALIBRATION_CONFIG_T btIqCalConfig;
#endif

#ifdef TGENIE_MESH_DATA_SECTION
    NV_MESH_DATA_T                      mesh_config;
#endif

    nvrec_wifidevicerecord wifi_info;

    // TODO: If wanna OTA to update the nv record, two choices:
    // 1. Change above data structures and increase NV_EXTENSION_MAJOR_VERSION.
    //     Then the nv record will be rebuilt and the whole history information will be cleared
    // 2. Don't touch above data structures but just add new items here and increase NV_EXTENSION_MINOR_VERSION.
    //     Then the nv record will keep all the whole hisotry.
} NV_EXTENSION_RECORD_T;

#ifdef __cplusplus
extern "C" {
#endif

extern NV_EXTENSION_RECORD_T *nvrecord_extension_p;

int nv_record_env_init(void);

NV_EXTENSION_RECORD_T *nv_record_get_extension_entry_ptr(void);

void nv_record_extension_update(void);

void nv_extension_callback(void *param);

int nv_record_touch_cause_flush(void);

void nv_record_sector_clear(void);

void nv_record_flash_flush(void);

int nv_record_flash_flush_in_sleep(void);

void nv_record_update_runtime_userdata(void);

void nv_record_rebuild(void);

uint32_t nv_record_pre_write_operation(void);

void nv_record_post_write_operation(uint32_t lock);

bt_status_t nv_record_open(SECTIONS_ADP_ENUM section_id);

void nv_record_init(void);

#ifdef __cplusplus
}
#endif
#endif
#endif  //#if defined(NEW_NV_RECORD_ENALBED)
