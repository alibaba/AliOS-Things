#ifndef SENSOR_DRV_CONF_H
#define SENSOR_DRV_CONF_H

typedef enum
{
    SENSOR_CLOSE = 0, /* Accelerometer */
    SENSOR_OPEN,
} sensor_ability;


typedef struct
{
    const char *   company;
    const char *   model;
    const char *   name;
    const char *   path;
    sensor_tag_e   tag;
    sensor_ability ability;
    unsigned char  slave;
    unsigned short addr;
    unsigned short
                   reg_cnt; /* register length in bytes, keep <= SNS_VALUE_BYTES_MAX */
    unsigned char  byte_reverse;  /* 0: high bytes first, 1: low bytes first */
    unsigned short response_time; /* slave response time in ms */
} modbus_sensor_t;

const modbus_sensor_t modbus_sensors[] = {
    // company                     sensor model          name device path tag
    // ability                            slave       addr             reg cnt
    // reverse      timeout
    { "MTMicrosystems", "MSTH1000", "temp", dev_temp_path, TAG_DEV_TEMP,
      SENSOR_CLOSE, 0x01, 0x0001, 1, 0, 50 },
    { "MTMicrosystems", "MSTH1000", "humi", dev_humi_path, TAG_DEV_HUMI,
      SENSOR_CLOSE, 0x01, 0x0000, 1, 0, 50 },
    { "MTMicrosystems", "MSN1000MB", "noise", dev_noise_path, TAG_DEV_NOISE,
      SENSOR_OPEN, 0x02, 0x0002, 1, 0, 50 },
    { "MTMicrosystems", "MSP1000MB", "baro", dev_baro_path, TAG_DEV_BARO,
      SENSOR_OPEN, 0x03, 0x0009, 2, 0, 50 },
    { "MTMicrosystems", "MSF1000MB", "hcho", dev_hcho_path, TAG_DEV_HCHO,
      SENSOR_CLOSE, 0x04, 0x0008, 1, 0, 50 },
    { "MTMicrosystems", "MSPM25MB", "pm2.5", dev_pm25_path, TAG_DEV_PM25,
      SENSOR_CLOSE, 0x05, 0x0005, 1, 0, 50 },
    { "MTMicrosystems", "MSPM25MB", "pm1.0", dev_pm1_path, TAG_DEV_PM1P0,
      SENSOR_OPEN, 0x05, 0x0006, 1, 0, 50 },
    { "MTMicrosystems", "MSPM25MB", "pm10", dev_pm10_path, TAG_DEV_PM10,
      SENSOR_OPEN, 0x05, 0x0007, 1, 0, 50 },
    { "MTMicrosystems", "MSCO2MB", "mtco2", dev_co2_path, TAG_DEV_CO2,
      SENSOR_CLOSE, 0x06, 0x0002, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "co2", dev_co2_path, TAG_DEV_CO2, SENSOR_OPEN,
      0x20, 0x0000, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "humi", dev_humi_path, TAG_DEV_HUMI, SENSOR_OPEN,
      0x20, 0x0001, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "pm2.5", dev_pm25_path, TAG_DEV_PM25, SENSOR_OPEN,
      0x20, 0x0002, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "temp", dev_temp_path, TAG_DEV_TEMP, SENSOR_OPEN,
      0x20, 0x0003, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "tvoc", dev_tvoc_path, TAG_DEV_TVOC, SENSOR_OPEN,
      0x20, 0x0004, 1, 0, 50 },
    { "Honeywell", "HAQEAW", "hcho", dev_hcho_path, TAG_DEV_HCHO, SENSOR_OPEN,
      0x20, 0x0005, 1, 0, 50 },
};


#endif
