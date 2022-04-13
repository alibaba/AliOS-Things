/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "hal2vfs/io_i2c.h"
#include "drivers/ddkc_log.h"


#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "si7006.h"

static i2c_dev_t i2c_dev;
static int g_i2c_fd = -1;
/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

int _i2c_open_channel(unsigned int id) {
    int fd = -1;
    char name[16] = {0};

    snprintf(name, sizeof(name), "/dev/i2c%d", id);
    fd = open(name, 0);
    ddkc_info("open %s %s, fd:%d\r\n", name, fd >= 0 ? "success" : "fail", fd);

    return fd;
}

int _hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
    uint16_t size, uint32_t timeout) {
    int ret = -1;
    io_i2c_data_t d;

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(g_i2c_fd, IOC_I2C_MASTER_TX, (unsigned long)&d);

    ddkc_info("ioctl on IOC_I2C_MASTER_TX %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
    return ret;
}

int32_t _hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout) {
    int ret = -1;
    io_i2c_data_t d;

    d.addr = dev_addr;
    d.data = data;
    d.length = size;
    d.maddr = 0;
    d.mlength = 0;
    d.timeout = timeout;

    ret = ioctl(g_i2c_fd, IOC_I2C_MASTER_RX, (unsigned long)&d);

    ddkc_info("ioctl on IOC_I2C_MASTER_RX %s, ret:%d\r\n", ret ? "fail" : "succeed", ret);
    return ret;
}

void si7006_init(void)
{

    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_400K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    i2c_dev.config.dev_addr      = Si7006_ADDRESS;
#if 0
    hal_i2c_init(&i2c_dev);
#else
    int ret = 0;
    int channel = 1;
    io_i2c_control_u c;

    g_i2c_fd = _i2c_open_channel(1);

    if (g_i2c_fd < 0) {
        ddkc_err("open channel %d failed, fd:%d, ignore ap3216c ops\r\n", channel, g_i2c_fd);
        return;
    }

    c.c.addr = Si7006_ADDRESS;  /* sensor's address */
    c.c.addr_width = 0;         /* 7-bit address */
    c.c.role = 1;               /* master mode */
    ret = ioctl(g_i2c_fd, IOC_I2C_SET_CONFIG, (unsigned long)&c);

    c.freq = 400000;
    ret = ioctl(g_i2c_fd, IOC_I2C_SET_FREQ, (unsigned long)&c);

    ddkc_info("ioctl on IOC_I2C_SET_FREQ %s ret:%d\r\n", ret ? "fail" : "succeed", ret);

#endif
}


int si7006_deinit(void) {

    if (g_i2c_fd > 0) {
        close(g_i2c_fd);
        g_i2c_fd = -1;
    }
    return 0;
}

uint8_t si7006_getVer(void)
{
    uint8_t reg[2]  = {Si7006_READ_Firmware_Revision_0,Si7006_READ_Firmware_Revision_1};
    uint8_t version = 0;

    _hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, reg, 2, 1000);
    aos_msleep(30);
    _hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &version, 1, 1000);
    //ddkc_info("APP", "ver:0x%2x \n",version);
    return version;
}

void si7006_getID(uint8_t *id_buf)
{
    uint8_t reg[4]  = {Si7006_READ_ID_LOW_0,Si7006_READ_ID_LOW_1,Si7006_READ_ID_HIGH_0,Si7006_READ_ID_HIGH_1};

    _hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, reg, 2, 1000); 
    aos_msleep(30);
    _hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, id_buf, 4, 1000);

    _hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg[2], 2, 1000); 
    aos_msleep(30);
    _hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &id_buf[4], 4, 1000);

    return;
}

bool si7006_getTemperature(float *temperature)
{
    uint8_t reg  = Si7006_MEAS_TEMP_NO_MASTER_MODE;
    uint8_t read_data[2] = {0};
    unsigned int value;

    _hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000); 
    aos_msleep(30);
    _hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    ddkc_info("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC)
    {
        *temperature = (175.72f * (float)value) / 65536.0f - 46.85f;
        ddkc_info("APP", "temperature: %2f \n", *temperature);
    }
    else
    {
        ddkc_info("APP", "Error on temp\n");
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

    _hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000);

    aos_msleep(30);

    _hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    ddkc_info("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    if (value & 0xFFFE)
    {
        *humidity = ((125.0f * (float)value ) / 65535.0f) - 6.0f;
        ddkc_info("APP", "humidity: %f \n", *humidity);
    }
    else
    {
        ddkc_info("APP", "Error on humidity\n");
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

int si7006_test(void) {
    int id = 0;
    float humidity= 0;
    float temperature = 0;

    ddkc_info("si7006 init\r\n");
    si7006_init();
    ddkc_info("si7006 init done\r\n");

    ddkc_info("si7006 getID\r\n");
    si7006_getID(&id);
    ddkc_info("si7006 id:%x\r\n", id);

    si7006_getTempHumidity(&humidity, &temperature);
    ddkc_info("si7006 humidity:%f, temperature:%f\r\n", humidity, temperature);

    si7006_deinit();
    ddkc_info("si7006 deinit\r\n");
    return 0;
}
