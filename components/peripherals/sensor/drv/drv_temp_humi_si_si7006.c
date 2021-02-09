/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <math.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "si7006.h"

static i2c_dev_t i2c_dev;
/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

void si7006_init(void)
{
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_400K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    i2c_dev.config.dev_addr      = Si7006_ADDRESS;

    hal_i2c_init(&i2c_dev);
}

uint8_t si7006_getVer(void)
{
    uint8_t reg[2]  = {Si7006_READ_Firmware_Revision_0,Si7006_READ_Firmware_Revision_1};
    uint8_t version = 0;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, reg, 2, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &version, 1, 1000);
    //LOGI("APP", "ver:0x%2x \n",version);
    return version;
}

void si7006_getID(uint8_t *id_buf)
{
    uint8_t reg[4]  = {Si7006_READ_ID_LOW_0,Si7006_READ_ID_LOW_1,Si7006_READ_ID_HIGH_0,Si7006_READ_ID_HIGH_1};

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, reg, 2, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, id_buf, 4, 1000);

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg[2], 2, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &id_buf[4], 4, 1000);

    return;
}

bool si7006_getTemperature(float *temperature)
{
    uint8_t reg  = Si7006_MEAS_TEMP_NO_MASTER_MODE;
    uint8_t read_data[2] = {0};
    unsigned int value;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC)
    {
    	*temperature = (175.72f * (float)value) / 65536.0f - 46.85f;
        LOGI("APP", "temperature: %2f \n", *temperature);
    }
    else
    {
        LOGI("APP", "Error on temp\n");
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
    uint8_t reg  = Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE;
    uint8_t read_data[3] = {0};
    unsigned int value;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000);

    aos_msleep(30);

    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    if (value & 0xFFFE)
    {
    	*humidity = ((125.0f * (float)value ) / 65535.0f) - 6.0f;
        LOGI("APP", "humidity: %f \n", *humidity);
    }
    else
    {
        LOGI("APP", "Error on humidity\n");
        return 1;
    }
    return 0;
}

//get temp and humidity
void si7006_getTempHumidity(float *humidity, float *temperature)
{
    si7006_getTemperature(temperature);
    si7006_getHumidity(humidity);
}
