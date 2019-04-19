#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "sensor.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"
#include "gpio.h"

#define D6T44L06_I2C_SLAVE_ADDR (0x0A)

#define D6T44L06_I2C_ADDR_TRANS(n) ((n) << 1)
#define D6T44L06_I2C_ADDR D6T44L06_I2C_ADDR_TRANS(D6T44L06_I2C_SLAVE_ADDR)
#define D6T44L06_I2C_SPEED_STANDARD (100000)

/* TEMPERATURE REGISTER */
#define D6T44L06_DATA_BASE_ADDR_I2C (0x4C)
#define D6T44L06_DATA_BASE_ADDR D6T44L06_DATA_BASE_ADDR_I2C
#define D6T44L06_DATA_LEN (35)
#define D6T44L06_TEMP_DATA_LEN (16)

static int d6t44l06_io_read(uint8_t reg, uint8_t *data, uint16_t size);
static int d6t44l06_io_write(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_thermoarray_omron_d6t44l06_io_init(int io_port);
static int drv_thermoarray_omron_d6t44l06_i2c_read(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_thermoarray_omron_d6t44l06_i2c_write(uint8_t reg, uint8_t *data, uint16_t size);
int drv_thermoarray_omron_d6t44l06_init(void);
static int drv_thermoarray_omron_d6t44l06_open(void);
static int drv_thermoarray_omron_d6t44l06_close(void);
static void drv_thermoarray_omron_d6t44l06_irq_handle(void);
static int drv_thermoarray_omron_d6t44l06_read(void *buf, size_t len);
static int drv_thermoarray_omron_d6t44l06_ioctl(int cmd, unsigned long arg);

static uint8_t drv_thermoarray_omron_d6t44l06_calc_crc(uint8_t data);
static int drv_thermoarray_omron_d6t44l06_checkPEC(const uint8_t* buf, uint8_t pec_index);

i2c_dev_t d6t44l06_ctx_i2c = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = D6T44L06_I2C_SPEED_STANDARD,
    .config.dev_addr      = D6T44L06_I2C_ADDR,
};

sensor_io_dev_t d6t44l06_ctx;
sensor_io_func_st d6t44l06_api = { NULL, NULL };

/* read */
static int d6t44l06_io_read(uint8_t reg, uint8_t *data, uint16_t size)
{
    if (unlikely(NULL == d6t44l06_api.read)) {
        return -1;
    }

    return d6t44l06_api.read(reg, data, size);
}

/* write */
static int d6t44l06_io_write(uint8_t reg, uint8_t *data, uint16_t size)
{
    if (NULL == d6t44l06_api.write) {
        return -1;
    }

    return d6t44l06_api.write(reg, data, size);
}

/* Select I/O type */
static int drv_thermoarray_omron_d6t44l06_io_init(int io_port)
{
    memset(&d6t44l06_api, 0, sizeof(d6t44l06_api));
    memset(&d6t44l06_ctx, 0, sizeof(d6t44l06_ctx));
    if (io_port == I2C_PORT) {
        d6t44l06_ctx.io_port  = I2C_PORT;
        memcpy(&d6t44l06_ctx.dev_i2c,&d6t44l06_ctx_i2c,sizeof(d6t44l06_ctx.dev_i2c));
        d6t44l06_api.read  = drv_thermoarray_omron_d6t44l06_i2c_read;
        d6t44l06_api.write = drv_thermoarray_omron_d6t44l06_i2c_write;
        return 0;
    } 
    else if (io_port == SPI_PORT) {
        return -1;
    }
    return -1;
}

/* I2C read */
static int drv_thermoarray_omron_d6t44l06_i2c_read(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t addr = reg;
    return sensor_io_read(&d6t44l06_ctx, &addr, data, size, I2C_OP_RETRIES);
}

/* I2C write */
static int drv_thermoarray_omron_d6t44l06_i2c_write(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t addr = reg;
    return sensor_io_write(&d6t44l06_ctx, &addr, data, size, I2C_OP_RETRIES);
}

/* Sensor init */
int drv_thermoarray_omron_d6t44l06_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_THERMOARRAY;
    sensor.path       = dev_thermoarray_path;
    sensor.open       = drv_thermoarray_omron_d6t44l06_open;
    sensor.close      = drv_thermoarray_omron_d6t44l06_close;
    sensor.read       = drv_thermoarray_omron_d6t44l06_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_thermoarray_omron_d6t44l06_ioctl;
    sensor.irq_handle = drv_thermoarray_omron_d6t44l06_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_thermoarray_omron_d6t44l06_io_init(sensor.io_port);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

/* Driver APIs */
static int drv_thermoarray_omron_d6t44l06_open(void)
{
    return 0;
}

static int drv_thermoarray_omron_d6t44l06_close(void)
{
    return 0;
}

static void drv_thermoarray_omron_d6t44l06_irq_handle(void)
{
    /* no handle so far */
}

static int drv_thermoarray_omron_d6t44l06_read(void *buf, size_t len)
{
    int i;
    int ret = 0;
    size_t size = 0;
    uint8_t data[D6T44L06_DATA_LEN];

    thermoarray_data_t *pdata = (thermoarray_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(thermoarray_data_t);
    if (len < size) {
        return -1;
    }

    /* Read data */
    ret = d6t44l06_io_read(D6T44L06_DATA_BASE_ADDR, data, D6T44L06_DATA_LEN);
    if (unlikely(ret)) {
        return -1;
    }

    /* CRC Error */
    ret = drv_thermoarray_omron_d6t44l06_checkPEC(data, D6T44L06_DATA_LEN-1);
    if (unlikely(ret)) {
        return -1;
    }

    /* set data */
    pdata->ref_temp_data = (int16_t)((((uint16_t)data[1])<<8) | (uint16_t)data[0]);
    for (i = 0; i < D6T44L06_TEMP_DATA_LEN; i++) {
        pdata->temp_data[i] = (int16_t)((((uint16_t)data[2*i+3])<<8) | (uint16_t)data[2*i+2]);
    }
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_thermoarray_omron_d6t44l06_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            /* no such func */
            break;
        }
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->vendor = DEV_SENSOR_VENDOR_OMRON;
            info->model = "D6T44L06";
            info->unit = dCelsius;
            info->range_max = 500;
            info->range_min = 50;
            break;
        }
        case SENSOR_IOCTL_SET_POWER: {
            /* no such func */
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}

/* Local functions */
static uint8_t drv_thermoarray_omron_d6t44l06_calc_crc(uint8_t data)
{
    int i;
    uint8_t temp;

    for (i = 0; i < 8; i++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {
            data ^= 0x07;
        }
    }
    return data;
}

static int drv_thermoarray_omron_d6t44l06_checkPEC(const uint8_t* buf, uint8_t pec_index)
{
    int i;
    uint8_t crc;
    
    if (pec_index >= 1) {
        crc = drv_thermoarray_omron_d6t44l06_calc_crc(0x14);
        crc = drv_thermoarray_omron_d6t44l06_calc_crc(0x4C ^ crc);
        crc = drv_thermoarray_omron_d6t44l06_calc_crc(0x15 ^ crc);
    }
    else {  /* only PEC is read */
        crc = drv_thermoarray_omron_d6t44l06_calc_crc(0x15);
    }
    for (i = 0; i< pec_index; i++) {
        crc = drv_thermoarray_omron_d6t44l06_calc_crc(buf[i] ^ crc);
    }
    if (crc != buf[pec_index]) {
        return -1;
    }
    
    return 0;
}
