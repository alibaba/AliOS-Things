/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "drv_temp_humi_si_si7006.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include <math.h>
#include <stdlib.h>

/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/
#define SI7006_I2C_PORT 1

int32_t si7006_i2c_master_send(const uint8_t *data, uint16_t size, uint32_t timeout) {
    return sensor_i2c_master_send(SI7006_I2C_PORT, Si7006_ADDRESS, data, size, timeout);
}

int32_t si7006_i2c_master_recv(uint8_t *data, uint16_t size, uint32_t timeout) {
    return sensor_i2c_master_recv(SI7006_I2C_PORT, Si7006_ADDRESS, data, size, timeout);
}

void si7006_init(void)
{

    int32_t ret = sensor_i2c_open (SI7006_I2C_PORT, Si7006_ADDRESS, I2C_BUS_BIT_RATES_400K, 0);
    if (ret) {
        printf("sensor i2c open failed, ret:%d\n", ret);
        return;
    }

}

uint8_t si7006_getVer(void)
{
    uint8_t reg[2]  = {Si7006_READ_Firmware_Revision_0,
                      Si7006_READ_Firmware_Revision_1};
    uint8_t version = 0;

    si7006_i2c_master_send(reg, 2, 1000);
    aos_msleep(30);
    si7006_i2c_master_recv(&version, 1, 1000);
    //LOGI("SENSOR", "ver:0x%2x \n",version);
    return version;
}

void si7006_getID(uint8_t *id_buf)
{
    uint8_t reg[4] = {Si7006_READ_ID_LOW_0, Si7006_READ_ID_LOW_1,
                      Si7006_READ_ID_HIGH_0, Si7006_READ_ID_HIGH_1};

    si7006_i2c_master_send(reg, 2, 1000);
    aos_msleep(30);
    si7006_i2c_master_recv(id_buf, 4, 1000);

    si7006_i2c_master_send(&reg[2], 2, 1000);
    aos_msleep(30);
    si7006_i2c_master_recv(&id_buf[4], 4, 1000);

    return;
}

bool si7006_getTemperature(float *temperature)
{
    uint8_t      reg          = Si7006_MEAS_TEMP_NO_MASTER_MODE;
    uint8_t      read_data[2] = {0};
    unsigned int value;

    si7006_i2c_master_send(&reg, 1, 1000);
    aos_msleep(30);
    si7006_i2c_master_recv(read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("SENSOR", "%0x -- %0x -->0x%x\n", read_data[0], read_data[1], value);
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC) {
        *temperature = (175.72f * (float)value) / 65536.0f - 46.85f;
        LOGI("SENDOR", "temperature: %2f\n", *temperature);
    } else {
        LOGE("SENDOR", "Error on temp\n");
        return 1;
    }
    return 0;
}

/*
i2c – the i2c device
dev_addr – device address
mem_addr – mem address
mem_addr_size – mem address
data – i2c master send data
size – i2c master send data size
*/
bool si7006_getHumidity(float *humidity)
{
    uint8_t      reg          = Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE;
    uint8_t      read_data[3] = {0};
    unsigned int value;

    si7006_i2c_master_send(&reg, 1, 1000);

    aos_msleep(30);

    si7006_i2c_master_recv(read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("SENSOR", "%0x -- %0x -->0x%x\n", read_data[0], read_data[1], value);
    if (value & 0xFFFE) {
        *humidity = ((125.0f * (float)value) / 65535.0f) - 6.0f;
        LOGI("SENDOR", "humidity: %f\n", *humidity);
    } else {
        LOGE("SENDOR", "Error on humidity\n");
        return 1;
    }
    return 0;
}

// get temp and humidity
void si7006_getTempHumidity(float *humidity, float *temperature)
{
    si7006_getTemperature(temperature);
    si7006_getHumidity(humidity);
}


void si7006_deinit(void)
{
    int32_t ret = sensor_i2c_close(SI7006_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
    }

    return;
}
