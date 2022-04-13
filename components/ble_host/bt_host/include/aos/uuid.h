/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __UUID_H__
#define __UUID_H__

#ifdef __cplusplus
extern "C" {
#endif

//#pragma pack(1)

#define BLE_BIT(n)          (1UL << (n))

typedef enum {
    UUID_TYPE_16,
    UUID_TYPE_32,
    UUID_TYPE_128,
} uuid_type_en;

typedef struct _uuid_t {
    uint8_t type;
} uuid_t;


struct ut_uuid_16 {
    uuid_t uuid;
    uint16_t val;
};

struct ut_uuid_32 {
    uuid_t uuid;
    uint32_t val;
};

struct ut_uuid_128 {
    uuid_t uuid;
    uint8_t val[16];
};

#define UUID16_INIT(value)      \
    {                         \
        .uuid.type = UUID_TYPE_16, \
                     .val = (value) \
    }

#define UUID32_INIT(value)      \
    {                   \
        .uuid.type = UUID_TYPE_32, \
                     .val = (value)\
    }

#define UUID128_INIT(value...)  \
    {                   \
        .uuid.type = UUID_TYPE_128, \
                     .val = {value},\
    }

#define UUID16_DECLARE(value) \
    ((uuid_t *) (&((struct ut_uuid_16)UUID16_INIT(value))))
#define UUID32_DECLARE(value) \
    ((uuid_t *) (&((struct ut_uuid_32)UUID32_INIT(value))))
#define UUID128_DECLARE(value...) \
    ((uuid_t *) (&((struct ut_uuid_128)UUID128_INIT(value))))

#define UUID16(__u)  (((struct ut_uuid_16 *)(__u))->val)
#define UUID32(__u)  (((struct ut_uuid_32 *)(__u))->val)
#define UUID128(__u) (((struct ut_uuid_128 *)(__u))->val)



int uuid_compare(const uuid_t *uuid1, const uuid_t *uuid2);
uint8_t *get_uuid_val(uuid_t *uuid);
uint8_t UUID_EQUAL(uuid_t *uuid1 , uuid_t *uuid2);

#define UUID_TYPE(__u) ((__u)->type)
#define UUID_LEN(__u) (UUID_TYPE(__u) == UUID_TYPE_16?2: (UUID_TYPE(__u) == UUID_TYPE_32?4:16))

/** @def UUID_GAP
 *  @brief Generic Access
 */
#define UUID_GAP                       UUID16_DECLARE(0x1800)
/** @def UUID_GATT
 *  @brief Generic Attribute
 */
#define UUID_GATT                      UUID16_DECLARE(0x1801)
/** @def UUID_CTS
 *  @brief Current Time Service
 */
#define UUID_CTS                       UUID16_DECLARE(0x1805)
/** @def UUID_DIS
 *  @brief Device Information Service
 */
#define UUID_DIS                       UUID16_DECLARE(0x180a)
/** @def UUID_HRS
 *  @brief Heart Rate Service
 */
#define UUID_HRS                       UUID16_DECLARE(0x180d)
/** @def UUID_BAS
 *  @brief Battery Service
 */
#define UUID_BAS                       UUID16_DECLARE(0x180f)
/** @def UUID_HIDS
 *  @brief HID Service
 */
#define UUID_HIDS                      UUID16_DECLARE(0x1812)
/** @def UUID_CSC
 *  @brief Cycling Speed and Cadence Service
 */
#define UUID_CSC                       UUID16_DECLARE(0x1816)
/** @def UUID_ESS
 *  @brief Environmental Sensing Service
 */
#define UUID_ESS                       UUID16_DECLARE(0x181a)
/** @def UUID_IPSS
 *  @brief IP Support Service
 */
#define UUID_IPSS                      UUID16_DECLARE(0x1820)
/** @def UUID_MESH_PROV
 *  @brief Mesh Provisioning Service
 */
#define UUID_MESH_PROV                 UUID16_DECLARE(0x1827)
/** @def UUID_MESH_PROXY
 *  @brief Mesh Proxy Service
 */
#define UUID_MESH_PROXY                UUID16_DECLARE(0x1828)
/** @def UUID_GATT_PRIMARY
 *  @brief GATT Primary Service
 */
#define UUID_GATT_PRIMARY              UUID16_DECLARE(0x2800)
/** @def UUID_GATT_SECONDARY
 *  @brief GATT Secondary Service
 */
#define UUID_GATT_SECONDARY            UUID16_DECLARE(0x2801)
/** @def UUID_GATT_INCLUDE
 *  @brief GATT Include Service
 */
#define UUID_GATT_INCLUDE              UUID16_DECLARE(0x2802)
/** @def UUID_GATT_CHRC
 *  @brief GATT Characteristic
 */
#define UUID_GATT_CHRC                 UUID16_DECLARE(0x2803)
/** @def UUID_GATT_CEP
 *  @brief GATT Characteristic Extended Properties
 */
#define UUID_GATT_CEP                  UUID16_DECLARE(0x2900)
/** @def UUID_GATT_CUD
 *  @brief GATT Characteristic User Description
 */
#define UUID_GATT_CUD                  UUID16_DECLARE(0x2901)
/** @def UUID_GATT_CCC
 *  @brief GATT Client Characteristic Configuration
 */
#define UUID_GATT_CCC                  UUID16_DECLARE(0x2902)
/** @def UUID_GATT_SCC
 *  @brief GATT Server Characteristic Configuration
 */
#define UUID_GATT_SCC                  UUID16_DECLARE(0x2903)
/** @def UUID_GATT_CPF
 *  @brief GATT Characteristic Presentation Format
 */
#define UUID_GATT_CPF                  UUID16_DECLARE(0x2904)
/** @def UUID_VALID_RANGE
 *  @brief Valid Range Descriptor
 */
#define UUID_VALID_RANGE               UUID16_DECLARE(0x2906)
/** @def UUID_HIDS_EXT_REPORT
 *  @brief HID External Report Descriptor
 */
#define UUID_HIDS_EXT_REPORT           UUID16_DECLARE(0x2907)
/** @def UUID_HIDS_REPORT_REF
 *  @brief HID Report Reference Descriptor
 */
#define UUID_HIDS_REPORT_REF           UUID16_DECLARE(0x2908)
/** @def UUID_ES_CONFIGURATION
 *  @brief Environmental Sensing Configuration Descriptor
 */
#define UUID_ES_CONFIGURATION          UUID16_DECLARE(0x290b)
/** @def UUID_ES_MEASUREMENT
 *  @brief Environmental Sensing Measurement Descriptor
 */
#define UUID_ES_MEASUREMENT            UUID16_DECLARE(0x290c)
/** @def UUID_ES_TRIGGER_SETTING
 *  @brief Environmental Sensing Trigger Setting Descriptor
 */
#define UUID_ES_TRIGGER_SETTING        UUID16_DECLARE(0x290d)
/** @def UUID_GAP_DEVICE_NAME
 *  @brief GAP Characteristic Device Name
 */
#define UUID_GAP_DEVICE_NAME           UUID16_DECLARE(0x2a00)
/** @def UUID_GAP_APPEARANCE
 *  @brief GAP Characteristic Appearance
 */
#define UUID_GAP_APPEARANCE            UUID16_DECLARE(0x2a01)
/** @def UUID_GAP_PPCP
 *  @brief GAP Characteristic Peripheral Preferred Connection Parameters
 */
#define UUID_GAP_PPCP                  UUID16_DECLARE(0x2a04)
/** @def UUID_GATT_SC
 *  @brief GATT Characteristic Service Changed
 */
#define UUID_GATT_SC                   UUID16_DECLARE(0x2a05)
/** @def UUID_BAS_BATTERY_LEVEL
 *  @brief BAS Characteristic Battery Level
 */
#define UUID_BAS_BATTERY_LEVEL         UUID16_DECLARE(0x2a19)
/** @def UUID_HIDS_BOOT_KB_IN_REPORT
 *  @brief HID Characteristic Boot Keyboard Input Report
 */
#define UUID_HIDS_BOOT_KB_IN_REPORT    UUID16_DECLARE(0x2a22)
/** @def UUID_DIS_SYSTEM_ID
 *  @brief DIS Characteristic System ID
 */
#define UUID_DIS_SYSTEM_ID             UUID16_DECLARE(0x2a23)
/** @def UUID_DIS_MODEL_NUMBER
 *  @brief DIS Characteristic Model Number String
 */
#define UUID_DIS_MODEL_NUMBER          UUID16_DECLARE(0x2a24)
/** @def UUID_DIS_SERIAL_NUMBER
 *  @brief DIS Characteristic Serial Number String
 */
#define UUID_DIS_SERIAL_NUMBER         UUID16_DECLARE(0x2a25)
/** @def UUID_DIS_FIRMWARE_REVISION
 *  @brief DIS Characteristic Firmware Revision String
 */
#define UUID_DIS_FIRMWARE_REVISION     UUID16_DECLARE(0x2a26)
/** @def UUID_DIS_HARDWARE_REVISION
 *  @brief DIS Characteristic Hardware Revision String
 */
#define UUID_DIS_HARDWARE_REVISION     UUID16_DECLARE(0x2a27)
/** @def UUID_DIS_SOFTWARE_REVISION
 *  @brief DIS Characteristic Software Revision String
 */
#define UUID_DIS_SOFTWARE_REVISION     UUID16_DECLARE(0x2a28)
/** @def UUID_DIS_MANUFACTURER_NAME
 *  @brief DIS Characteristic Manufacturer Name String
 */
#define UUID_DIS_MANUFACTURER_NAME     UUID16_DECLARE(0x2a29)
/** @def UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST
 *  @brief IEEE Regulatory Certification Data List characteristic UUID.
 */
#define UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST UUID16_DECLARE(0x2a2a)
/** @def UUID_DIS_PNP_ID
 *  @brief DIS Characteristic PnP ID
 */
#define UUID_DIS_PNP_ID                UUID16_DECLARE(0x2a50)
/** @def UUID_CTS_CURRENT_TIME
 *  @brief CTS Characteristic Current Time
 */
#define UUID_CTS_CURRENT_TIME          UUID16_DECLARE(0x2a2b)
/** @def UUID_MAGN_DECLINATION
 *  @brief Magnetic Declination Characteristic
 */
#define UUID_MAGN_DECLINATION          UUID16_DECLARE(0x2a2c)
/** @def UUID_HIDS_BOOT_KB_OUT_REPORT
 *  @brief HID Boot Keyboard Output Report Characteristic
 */
#define UUID_HIDS_BOOT_KB_OUT_REPORT   UUID16_DECLARE(0x2a32)
/** @def UUID_HIDS_BOOT_MOUSE_IN_REPORT
 *  @brief HID Boot Mouse Input Report Characteristic
 */
#define UUID_HIDS_BOOT_MOUSE_IN_REPORT UUID16_DECLARE(0x2a33)
/** @def UUID_HRS_MEASUREMENT
 *  @brief HRS Characteristic Measurement Interval
 */
#define UUID_HRS_MEASUREMENT           UUID16_DECLARE(0x2a37)
/** @def UUID_HRS_BODY_SENSOR
 *  @brief HRS Characteristic Body Sensor Location
 */
#define UUID_HRS_BODY_SENSOR           UUID16_DECLARE(0x2a38)
/** @def UUID_HRS_CONTROL_POINT
 *  @brief HRS Characteristic Control Point
 */
#define UUID_HRS_CONTROL_POINT         UUID16_DECLARE(0x2a39)
/** @def UUID_HIDS_INFO
 *  @brief HID Information Characteristic
 */
#define UUID_HIDS_INFO                 UUID16_DECLARE(0x2a4a)
/** @def UUID_HIDS_REPORT_MAP
 *  @brief HID Report Map Characteristic
 */
#define UUID_HIDS_REPORT_MAP           UUID16_DECLARE(0x2a4b)
/** @def UUID_HIDS_CTRL_POINT
 *  @brief HID Control Point Characteristic
 */
#define UUID_HIDS_CTRL_POINT           UUID16_DECLARE(0x2a4c)
/** @def UUID_HIDS_REPORT
 *  @brief HID Report Characteristic
 */
#define UUID_HIDS_REPORT               UUID16_DECLARE(0x2a4d)
/** @def UUID_HIDS_PROTOCOL_MODE
 *  @brief HID Protocol Mode Characteristic
 */
#define UUID_HIDS_PROTOCOL_MODE        UUID16_DECLARE(0x2a4e)
/** @def UUID_CSC_MEASUREMENT
 *  @brief CSC Measurement Characteristic
 */
#define UUID_CSC_MEASUREMENT           UUID16_DECLARE(0x2a5b)
/** @def UUID_CSC_FEATURE
 *  @brief CSC Feature Characteristic
 */
#define UUID_CSC_FEATURE               UUID16_DECLARE(0x2a5c)
/** @def UUID_SENSOR_LOCATION
 *  @brief Sensor Location Characteristic
 */
#define UUID_SENSOR_LOCATION           UUID16_DECLARE(0x2a5d)
/** @def UUID_SC_CONTROL_POINT
 *  @brief SC Control Point Characteristic
 */
#define UUID_SC_CONTROL_POINT          UUID16_DECLARE(0x2a55)
/** @def UUID_ELEVATION
 *  @brief Elevation Characteristic
 */
#define UUID_ELEVATION                 UUID16_DECLARE(0x2a6c)
/** @def UUID_PRESSURE
 *  @brief Pressure Characteristic
 */
#define UUID_PRESSURE                  UUID16_DECLARE(0x2a6d)
/** @def UUID_TEMPERATURE
 *  @brief Temperature Characteristic
 */
#define UUID_TEMPERATURE               UUID16_DECLARE(0x2a6e)
/** @def UUID_HUMIDITY
 *  @brief Humidity Characteristic
 */
#define UUID_HUMIDITY                  UUID16_DECLARE(0x2a6f)
/** @def UUID_TRUE_WIND_SPEED
 *  @brief True Wind Speed Characteristic
 */
#define UUID_TRUE_WIND_SPEED           UUID16_DECLARE(0x2a70)
/** @def UUID_TRUE_WIND_DIR
 *  @brief True Wind Direction Characteristic
 */
#define UUID_TRUE_WIND_DIR             UUID16_DECLARE(0x2a71)
/** @def UUID_APPARENT_WIND_SPEED
 *  @brief Apparent Wind Speed Characteristic
 */
#define UUID_APPARENT_WIND_SPEED       UUID16_DECLARE(0x2a72)
/** @def UUID_APPARENT_WIND_DIR
 *  @brief Apparent Wind Direction Characteristic
 */
#define UUID_APPARENT_WIND_DIR         UUID16_DECLARE(0x2a73)
/** @def UUID_GUST_FACTOR
 *  @brief Gust Factor Characteristic
 */
#define UUID_GUST_FACTOR               UUID16_DECLARE(0x2a74)
/** @def UUID_POLLEN_CONCENTRATION
 *  @brief Pollen Concentration Characteristic
 */
#define UUID_POLLEN_CONCENTRATION      UUID16_DECLARE(0x2a75)
/** @def UUID_UV_INDEX
 *  @brief UV Index Characteristic
 */
#define UUID_UV_INDEX                  UUID16_DECLARE(0x2a76)
/** @def UUID_IRRADIANCE
 *  @brief Irradiance Characteristic
 */
#define UUID_IRRADIANCE                UUID16_DECLARE(0x2a77)
/** @def UUID_RAINFALL
 *  @brief Rainfall Characteristic
 */
#define UUID_RAINFALL                  UUID16_DECLARE(0x2a78)
/** @def UUID_WIND_CHILL
 *  @brief Wind Chill Characteristic
 */
#define UUID_WIND_CHILL                UUID16_DECLARE(0x2a79)
/** @def UUID_HEAT_INDEX
 *  @brief Heat Index Characteristic
 */
#define UUID_HEAT_INDEX                UUID16_DECLARE(0x2a7a)
/** @def UUID_DEW_POINT
 *  @brief Dew Point Characteristic
 */
#define UUID_DEW_POINT                 UUID16_DECLARE(0x2a7b)
/** @def UUID_DESC_VALUE_CHANGED
 *  @brief Descriptor Value Changed Characteristic
 */
#define UUID_DESC_VALUE_CHANGED        UUID16_DECLARE(0x2a7d)
/** @def UUID_MAGN_FLUX_DENSITY_2D
 *  @brief Magnetic Flux Density - 2D Characteristic
 */
#define UUID_MAGN_FLUX_DENSITY_2D      UUID16_DECLARE(0x2aa0)
/** @def UUID_MAGN_FLUX_DENSITY_3D
 *  @brief Magnetic Flux Density - 3D Characteristic
 */
#define UUID_MAGN_FLUX_DENSITY_3D      UUID16_DECLARE(0x2aa1)
/** @def UUID_BAR_PRESSURE_TREND
 *  @brief Barometric Pressure Trend Characteristic
 */
#define UUID_BAR_PRESSURE_TREND        UUID16_DECLARE(0x2aa3)
/** @def UUID_CENTRAL_ADDR_RES
 *  @brief Central Address Resolution Characteristic
 */
#define UUID_CENTRAL_ADDR_RES          UUID16_DECLARE(0x2aa6)
/** @def UUID_MESH_PROV_DATA_IN
 *  @brief Mesh Provisioning Data In
 */
#define UUID_MESH_PROV_DATA_IN         UUID16_DECLARE(0x2adb)
/** @def UUID_MESH_PROV_DATA_OUT
 *  @brief Mesh Provisioning Data Out
 */
#define UUID_MESH_PROV_DATA_OUT        UUID16_DECLARE(0x2adc)
/** @def UUID_MESH_PROXY_DATA_IN
 *  @brief Mesh Proxy Data In
 */
#define UUID_MESH_PROXY_DATA_IN        UUID16_DECLARE(0x2add)
/** @def UUID_MESH_PROXY_DATA_OUT
 *  @brief Mesh Proxy Data Out
 */
#define UUID_MESH_PROXY_DATA_OUT       UUID16_DECLARE(0x2ade)

/*
 * Protocol UUIDs
 */
#define UUID_SDP                       UUID16_DECLARE(0x0001)
#define UUID_UDP                       UUID16_DECLARE(0x0002)
#define UUID_RFCOMM                    UUID16_DECLARE(0x0003)
#define UUID_TCP                       UUID16_DECLARE(0x0004)
#define UUID_TCS_BIN                   UUID16_DECLARE(0x0005)
#define UUID_TCS_AT                    UUID16_DECLARE(0x0006)
#define UUID_ATT                       UUID16_DECLARE(0x0007)
#define UUID_OBEX                      UUID16_DECLARE(0x0008)
#define UUID_IP                        UUID16_DECLARE(0x0009)
#define UUID_FTP                       UUID16_DECLARE(0x000a)
#define UUID_HTTP                      UUID16_DECLARE(0x000c)
#define UUID_BNEP                      UUID16_DECLARE(0x000f)
#define UUID_UPNP                      UUID16_DECLARE(0x0010)
#define UUID_HIDP                      UUID16_DECLARE(0x0011)
#define UUID_HCRP_CTRL                 UUID16_DECLARE(0x0012)
#define UUID_HCRP_DATA                 UUID16_DECLARE(0x0014)
#define UUID_HCRP_NOTE                 UUID16_DECLARE(0x0016)
#define UUID_AVCTP                     UUID16_DECLARE(0x0017)
#define UUID_AVDTP                     UUID16_DECLARE(0x0019)
#define UUID_CMTP                      UUID16_DECLARE(0x001b)
#define UUID_UDI                       UUID16_DECLARE(0x001d)
#define UUID_MCAP_CTRL                 UUID16_DECLARE(0x001e)
#define UUID_MCAP_DATA                 UUID16_DECLARE(0x001f)
#define UUID_L2CAP                     UUID16_DECLARE(0x0100)

#ifdef __cplusplus
}
#endif

#endif  /* __UUID_H__ */

