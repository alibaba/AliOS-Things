/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __APP_INTERCONNECTION_BLE_H__
#define __APP_INTERCONNECTION_BLE_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/

/******************************macro defination*****************************/
#define INTERCONNECTION_BLE_FAST_ADVERTISING_INTERVAL (32)  // 20ms (32*0.625ms)
#define INTERCONNECTION_BLE_ADVERTISING_INTERVAL (160)      // 100ms (160*0.625ms)
#define APP_INTERCONNECTION_FAST_ADV_TIMEOUT_IN_MS 60000    // 1MIN
#define APP_INTERCONNECTION_SLOW_ADV_TIMEOUT_IN_MS 120000   // 2MIN
#define APP_INTERCONNECTION_DISAPPEAR_ADV_IN_MS 5000        // 5sec
#define INTERCONNECTION_DISCOVERABLE_YES (0)
#define INTERCONNECTION_DISCOVERABLE_NOT (1)

#define CUSTOMIZED_ADV_TYPE                         (0x16)
#define UUID_LEN                                    (2)
#define UUID_VALUE                                  "\xee\xfd"

#define MODEL_ID                                    "\x00\x00\x01"
#define MODEL_ID_LEN                                (3)

// for debug
#define SUB_MODEL_ID                                (0x01)

// data length before extersible data is 15 byte according to spec from huawei
// data structure is defined as CUSTOMIZED_ADV_DATA_T
#define FIXED_CUSTOMIZED_ADV_DATA_LEN               (15)
#define EXTENSIBLE_DATA_MAX_LEN                     (ADV_DATA_MAX_LEN-FIXED_CUSTOMIZED_ADV_DATA_LEN)

// 2 byte to describe paired device
#define PAIRED_DEVICE_ID_LEN                        (2)

// 2 byte to describe dual model device
#define DUAL_MODEL_DEVICE_ID_LEN                    (2)

// max length of per extensible service is 3 byte
#define EXTENSIBLE_SERVICE_MAX_LEN                  (3)

/******************************type defination******************************/

typedef enum
{
    SERVICE_ID_NEARBY                               = 0x01,
    SERVICE_ID_ICONNECT                             = 0x01,
    SERVICE_ID_RSSI_REFERENCE_VALUE                 = 0x02,
    SERVICE_ID_MODEL_ID                             = 0x03,
    SERVICE_ID_SUB_MODEL_ID                         = 0x04,
    SERVICE_ID_PAIRED_DEVICE_ID                     = 0x05,
    SERVICE_ID_CONNECTED_DEVICE_NUM                 = 0x06,
    SERVICE_ID_PAIRED_DEVICE_NUM                    = 0x07,
    SERVICE_ID_MAX_CONNECT_DEVICE_NUM               = 0x08,
    SERVICE_ID_MAX_PAIR_DEVICE_NUM                  = 0x09,
    SERVICE_ID_DUAL_MODE_DEVICE_IDENTIFICATION      = 0x0a,
    SERVICE_ID_TOTAL_BATTERY                        = 0x0b,
    SERVICE_ID_LEFT_EAR_BATTERY                     = 0x0c,
    SERVICE_ID_RIGHT_EAR_BATTERY                    = 0x0d,
    SERVICE_ID_CHARGER_BOX_BATTERY                  = 0x0e,
} NEARBY_SERIVCE_ID_E;

typedef enum
{
    RECONNECTABLE_NOT_DISCOVERABLE_NOT              = 0x00,
    RECONNECTABLE_NOT_DISCOVERABLE_YES              = 0x01,
    RECONNECTABLE_YES_DISCOVERABLE_NOT              = 0x02,
    RECONNECTABLE_YES_DISCOVERABLE_YES              = 0X03,
}ICONNECT_STATUS_E;

typedef struct
{
    uint8_t             advLength;                      // 1 byte
    uint8_t             advType;                        // 1 byte
    uint8_t             uuidValue[UUID_LEN];            // 2 byte
    NEARBY_SERIVCE_ID_E nearbyServiceID;                // 1 byte
    NEARBY_SERIVCE_ID_E iconnectServiceID;              // 1 byte
    ICONNECT_STATUS_E   iconnectStatus;                 // 1 byte
    NEARBY_SERIVCE_ID_E rssiReferenceValueServiceID;    // 1 byte
    int8_t              rssiReferenceValue;             // 1 byte
    NEARBY_SERIVCE_ID_E modelIDServiceID;               // 1 byte
    uint8_t             modelID[MODEL_ID_LEN];          // 3 byte
    NEARBY_SERIVCE_ID_E subModelServiceID;              // 1 byte
    uint8_t             subModelID;                     // 1 byte
    // total length of data above is 15 byte
}__attribute__((__packed__)) CUSTOMIZED_ADV_DATA_HEAD_T;

typedef struct 
{
    uint8_t isCharging : 1;
    uint8_t batteryLevel : 7;
}BATTERY_INFO_T;

/****************************function declearation**************************/
void clear_discoverable_adv_timeout_flag(void);

void app_interconnection_start_disappear_adv(uint32_t advInterval, uint32_t advDuration);

void app_interceonnection_start_discoverable_adv(uint32_t advInterval, uint32_t advDuration);

void app_interconnection_ble_data_fill_handler(void* param);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_INTERCONNECTION_BLE_H__ */