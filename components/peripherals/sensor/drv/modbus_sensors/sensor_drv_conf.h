#ifndef SENSOR_DRV_CONF_H
#define SENSOR_DRV_CONF_H
#include "sensor_hal.h"
typedef enum
{
    SENSOR_CLOSE = 0, /* sensor disable */
    SENSOR_OPEN,	/* sensor enable*/
} sensor_ability;


typedef struct
{
    const char *   company;
    const char *   model;
    const char *   name;
    char *   path;
    sensor_tag_e   tag;
    uint8_t        instance;
    sensor_ability ability;
    unsigned char  slave;
    unsigned short addr;
    unsigned short
                   reg_cnt; /* register length in bytes, keep <= SNS_VALUE_BYTES_MAX */
    unsigned char  byte_reverse;  /* 0: high bytes first, 1: low bytes first */
} modbus_sensor_t;

modbus_sensor_t modbus_sensors[] = {
    /* company           model       name      path     tag                  instance ability            slave  addr reg_cnt reverse */
#if AOS_SENSOR_TEMP_MTM_MSTH1000
    { "MTMicrosystems", "MSTH1000", "temp", dev_temp_path,  TAG_DEV_TEMP,         0,    SENSOR_OPEN,   0x01, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_HUMI_MTM_MSTH1000
    { "MTMicrosystems", "MSTH1000", "humi", dev_humi_path, TAG_DEV_HUMI,          0,    SENSOR_OPEN,   0x01, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_NOISE_MTM_MSN1000MB
    { "MTMicrosystems", "MSN1000MB", "noise", dev_noise_path, TAG_DEV_NOISE,      0,    SENSOR_OPEN,   0x02, 0x0002, 1,    0},
#endif
#if AOS_SENSOR_BARO_MTM_MSP1000MB
    { "MTMicrosystems", "MSP1000MB", "baro", dev_baro_path, TAG_DEV_BARO,         0,    SENSOR_OPEN,   0x03, 0x0009, 2,    0},
#endif
#if AOS_SENSOR_HCHO_MTM_MSF1000MB
    { "MTMicrosystems", "MSF1000MB", "hcho", dev_hcho_path, TAG_DEV_HCHO,         0,    SENSOR_OPEN,   0x04, 0x0008, 1,    0},
#endif
#if AOS_SENSOR_PM25_MTM_MSPM25MB
    { "MTMicrosystems", "MSPM25MB", "pm2.5", dev_pm25_path, TAG_DEV_PM25,         0,    SENSOR_OPEN,   0x05, 0x0005, 1,    0},
#endif
#if AOS_SENSOR_PM1_MTM_MSPM25MB
    { "MTMicrosystems", "MSPM25MB", "pm1.0", dev_pm1_path, TAG_DEV_PM1P0,         0,    SENSOR_OPEN,   0x05, 0x0006, 1,    0},
#endif
#if AOS_SENSOR_PM10_MTM_MSPM25MB
    { "MTMicrosystems", "MSPM25MB", "pm10", dev_pm10_path, TAG_DEV_PM10,          0,    SENSOR_OPEN,   0x05, 0x0007, 1,    0},
#endif

#if AOS_SENSOR_CO2_MTM_MSCO2MB
    { "MTMicrosystems", "MSCO2MB", "mtco2", dev_co2_path, TAG_DEV_CO2,            0,    SENSOR_OPEN,   0x06, 0x0002, 1,    0},
#endif
#if AOS_SENSOR_PH_MTM_MSPH10
    { "MTMicrosystems", "MSPH10", "ph", dev_ph_path, TAG_DEV_PH,                  0,    SENSOR_OPEN,   0x01, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_TEMP_MTM_MSPH10
    { "MTMicrosystems", "MSPH10", "temp", dev_temp_path, TAG_DEV_TEMP,            0,    SENSOR_OPEN,   0x01, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_ALS_MTM_MSLT100MB
    { "MTMicrosystems", "MSLT100MB", "als", dev_als_path, TAG_DEV_ALS,            0,    SENSOR_OPEN,   0x02, 0x0004, 1,    0},
#endif
#if AOS_SENSOR_VWC_MTM_MSSM10
    { "MTMicrosystems", "MSSM10", "vwc", dev_vwc_path, TAG_DEV_VWC,               0,    SENSOR_OPEN,   0x03, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_TEMP_MTM_MSEC10
    { "MTMicrosystems", "MSEC10", "temp", dev_temp_path, TAG_DEV_TEMP,            0,    SENSOR_OPEN,   0x04, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_EC_MTM_MSEC10
    { "MTMicrosystems", "MSEC10", "ec", dev_ec_path, TAG_DEV_EC,                  0,    SENSOR_OPEN,   0x04, 0x0002, 1,    0},
#endif
#if AOS_SENSOR_SALINITY_MTM_MSEC10
    { "MTMicrosystems", "MSEC10", "salinity", dev_salinity_path, TAG_DEV_SALINITY,0,    SENSOR_OPEN,   0x04, 0x0003, 1,    0},
#endif
#if AOS_SENSOR_TDS_MTM_MSEC10
    { "MTMicrosystems", "MSEC10", "tds", dev_tds_path, TAG_DEV_TDS,               0,    SENSOR_OPEN,   0x04, 0x0004, 1,    0},
#endif
#if AOS_SENSOR_TEMP_MTM_MLWS10
    { "MTMicrosystems", "MLWS10", "leaf  temp", dev_temp_path, TAG_DEV_TEMP,      0,    SENSOR_OPEN,   0X05, 0x0000, 1,    0},
#endif

#if AOS_SENSOR_VWC_MTM_MLWS10
    { "MTMicrosystems", "MLWS10", "leaf  wet", dev_vwc_path, TAG_DEV_VWC,         0,    SENSOR_OPEN,   0X05, 0x0001, 1,    0},
#endif

#if AOS_SENSOR_WINDSPEED_KUNLUN_JHFSW1
    { "KunLunHaiAn", "JHFS-W1", "wind speed", dev_windspeed_path, TAG_DEV_WINDSPD,0,    SENSOR_OPEN,   0X30, 0x002A, 1,   0},
#endif

#if AOS_SENSOR_WINDDIRECTION_KUNLUN_JHFXW1
    { "KunLunHaiAn", "JHFX-W1", "wdir", dev_winddirection_path, TAG_DEV_WINDDIR,  0,    SENSOR_OPEN,   0X31, 0x002A, 1,    0},
#endif
#if AOS_SENSOR_CURRENTRAINFALL_KUNLUN_JHYLW1
    { "KunLunHaiAn", "JHYL-W1", "current rain", dev_rainfall_path, TAG_DEV_RAIN,  0,    SENSOR_OPEN,   0X32, 0x0002, 1,    0},
#endif
#if AOS_SENSOR_DAILYRAINFALL_KUNLUN_JHYLW1
    { "KunLunHaiAn", "JHYL-W1", "today rain", dev_rainfall_path, TAG_DEV_RAIN,    0,    SENSOR_OPEN,   0X32, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_TEMP_KUNLUN_JWSKV
    { "KunLunHaiAn", "JWSK-V", "temp", dev_temp_path, TAG_DEV_TEMP,               0,    SENSOR_OPEN,   0X33, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_HUMI_KUNLUN_JWSKV
    { "KunLunHaiAn", "JWSK-V", "humi", dev_humi_path, TAG_DEV_HUMI,               0,    SENSOR_OPEN,   0X33, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_ALS_KUNLUN_ZD6
    { "KunLunHaiAn", "ZD-6", "als", dev_als_path, TAG_DEV_ALS,                    0,    SENSOR_OPEN,   0X34, 0x0000, 2,    0},
#endif
#if AOS_SENSOR_TEMP_KUNLUN_JTWSAC
    { "KunLunHaiAn", "JTWS-AC", "soiltemp", dev_temp_path, TAG_DEV_TEMP,          0,    SENSOR_OPEN,   0X35, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_VWC_KUNLUN_JTWSAC
    { "KunLunHaiAn", "JTWS-AC", "soilvwc", dev_vwc_path, TAG_DEV_VWC,             0,    SENSOR_OPEN,   0X35, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_BARO_KUNLUN_JQYB
    { "KunLunHaiAn", "JQYB", "baro", dev_baro_path, TAG_DEV_BARO,                 0,    SENSOR_OPEN,   0X36, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_CO2_KUNLUN_JQAW6W1V
    { "KunLunHaiAn", "JQAW-6W1-V", "co2", dev_co2_path, TAG_DEV_CO2,              0,    SENSOR_OPEN,   0X37, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_CO2_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "co2", dev_co2_path, TAG_DEV_CO2,                    0,    SENSOR_OPEN,   0x20, 0x0000, 1,    0},
#endif
#if AOS_SENSOR_HUMI_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "humi", dev_humi_path, TAG_DEV_HUMI,                 0,    SENSOR_OPEN,   0x20, 0x0001, 1,    0},
#endif
#if AOS_SENSOR_PM25_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "pm2.5", dev_pm25_path, TAG_DEV_PM25,                0,    SENSOR_OPEN,   0x20, 0x0002, 1,    0},
#endif
#if AOS_SENSOR_TEMP_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "temp", dev_temp_path, TAG_DEV_TEMP,                 0,    SENSOR_OPEN,   0x20, 0x0003, 1,    0},
#endif

#if AOS_SENSOR_TVOC_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "tvoc", dev_tvoc_path, TAG_DEV_TVOC,                 0,    SENSOR_OPEN,   0x20, 0x0004, 1,    0},
#endif

#if AOS_SENSOR_HCHO_HONEYWELL_HAQEAW
    { "Honeywell", "HAQEAW", "hcho", dev_hcho_path, TAG_DEV_HCHO,                 0,    SENSOR_OPEN,   0x20, 0x0005, 1,    0},
#endif
};

#endif

