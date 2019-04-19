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

//#define SMPB02E_I2C
#define SMPB02E_SPI

#define SMPB02E_SPI_READ_FLAG (0x80)
#define SMPB02E_SPI_WRITE_FLAG (0x7F)

#define SMPB02E_I2C_SLAVE_ADDR_SDO_HI (0x56)
#define SMPB02E_I2C_SLAVE_ADDR_SDO_LO (0x70)
#define SMPB02E_I2C_SLAVE_ADDR SMPB02E_I2C_SLAVE_ADDR_SDO_HI

#define SMPB02E_I2C_ADDR_TRANS(n) ((n) << 1)
#define SMPB02E_I2C_ADDR SMPB02E_I2C_ADDR_TRANS(SMPB02E_I2C_SLAVE_ADDR)
#define SMPB02E_I2C_SPEED_STANDARD (100000)
#define SMPB02E_I2C_SPEED_FAST (400000)
/* CHIP ID REGISTER */
#define SMPB02E_CHIP_ID_ADDR_I2C (0xD1)
#define SMPB02E_CHIP_ID_ADDR_SPI (0x51)
#ifdef SMPB02E_I2C
#define SMPB02E_CHIP_ID_ADDR SMPB02E_CHIP_ID_ADDR_I2C
#else
#define SMPB02E_CHIP_ID_ADDR SMPB02E_CHIP_ID_ADDR_SPI
#endif
#define SMPB02E_CHIP_ID (0x5C)
/* IO SETUP REGISTER */
#define SMPB02E_IO_SETUP_ADDR_I2C (0xF5)
#define SMPB02E_IO_SETUP_ADDR_SPI (0x75)
#ifdef SMPB02E_I2C
#define SMPB02E_IO_SETUP_ADDR SMPB02E_IO_SETUP_ADDR_I2C
#else
#define SMPB02E_IO_SETUP_ADDR SMPB02E_IO_SETUP_ADDR_SPI
#endif
/* COE REGISTER */
#define SMPB02E_COE_BASE_ADDR_I2C (0xA0)
#define SMPB02E_COE_BASE_ADDR_SPI (0x20)
#ifdef SMPB02E_I2C
#define SMPB02E_COE_BASE_ADDR SMPB02E_COE_BASE_ADDR_I2C
#else
#define SMPB02E_COE_BASE_ADDR SMPB02E_COE_BASE_ADDR_SPI
#endif
#define SMPB02E_COE_NUM (25)
/* CTRL REGISTER */
#define SMPB02E_CTRL_MEAS_ADDR_I2C (0xF4)
#define SMPB02E_CTRL_MEAS_ADDR_SPI (0x74)
#ifdef SMPB02E_I2C
#define SMPB02E_CTRL_MEAS_ADDR SMPB02E_CTRL_MEAS_ADDR_I2C
#else
#define SMPB02E_CTRL_MEAS_ADDR SMPB02E_CTRL_MEAS_ADDR_SPI
#endif
/* TEMPERATURE AVERAGE TIMES */
typedef enum {
    SMPB02E_TEMP_AVG_SKIP = 0,
    SMPB02E_TEMP_AVG_1,
    SMPB02E_TEMP_AVG_2,
    SMPB02E_TEMP_AVG_4,
    SMPB02E_TEMP_AVG_8,
    SMPB02E_TEMP_AVG_16,
    SMPB02E_TEMP_AVG_32,
    SMPB02E_TEMP_AVG_64,
    SMPB02E_TEMP_AVG_MAX
} smpb02e_temp_avg_e;
/* PRESSURE AVERAGE TIMES */
typedef enum {
    SMPB02E_PRESS_AVG_SKIP = 0,
    SMPB02E_PRESS_AVG_1,
    SMPB02E_PRESS_AVG_2,
    SMPB02E_PRESS_AVG_4,
    SMPB02E_PRESS_AVG_8,
    SMPB02E_PRESS_AVG_16,
    SMPB02E_PRESS_AVG_32,
    SMPB02E_PRESS_AVG_64,
    SMPB02E_PRESS_AVG_MAX
} smpb02e_press_avg_e;
/* POWER MODE */
typedef enum {
    SMPB02E_POWER_MODE_SLEEP = 0,
    SMPB02E_POWER_MODE_FORCED,
    SMPB02E_POWER_MODE_FORCED_2,
    SMPB02E_POWER_MODE_NORMAL,
    SMPB02E_POWER_MODE_MAX
} smpb02e_power_mode_e;
/* IIR REGISTER */
#define SMPB02E_IIR_ADDR_I2C (0xF1)
#define SMPB02E_IIR_ADDR_SPI (0x71)
#ifdef SMPB02E_I2C
#define SMPB02E_IIR_ADDR SMPB02E_IIR_ADDR_I2C
#else
#define SMPB02E_IIR_ADDR SMPB02E_IIR_ADDR_SPI
#endif
/* IIR */
typedef enum {
    SMPB02E_IIR_OFF = 0,
    SMPB02E_IIR_2,
    SMPB02E_IIR_4,
    SMPB02E_IIR_8,
    SMPB02E_IIR_16,
    SMPB02E_IIR_32,
    SMPB02E_IIR_32_2,
    SMPB02E_IIR_32_3,
    SMPB02E_IIR_MAX
} smpb02e_iir_e;
/* RESET REGISTER */
#define SMPB02E_RESET_ADDR_I2C (0xE0)
#define SMPB02E_RESET_ADDR_SPI (0x60)
#ifdef SMPB02E_I2C
#define SMPB02E_RESET_ADDR SMPB02E_RESET_ADDR_I2C
#else
#define SMPB02E_RESET_ADDR SMPB02E_RESET_ADDR_SPI
#endif
#define SMPB02E_RESET_VAL (0xE6)
/* DEVICE STATUS REGISTER */
#define SMPB02E_DEVICE_STAT_ADDR_I2C (0xF3)
#define SMPB02E_DEVICE_STAT_ADDR_SPI (0x73)
#ifdef SMPB02E_I2C
#define SMPB02E_DEVICE_STAT_ADDR SMPB02E_DEVICE_STAT_ADDR_I2C
#else
#define SMPB02E_DEVICE_STAT_ADDR SMPB02E_DEVICE_STAT_ADDR_SPI
#endif
#define SMPB02E_DEVICE_STAT_MEAS_OK (0)
#define SMPB02E_DEVICE_STAT_MEAS_BUSY (1)
/* TEMPERATURE REGISTER */
#define SMPB02E_TEMP_BASE_ADDR_I2C (0xFA)
#define SMPB02E_TEMP_BASE_ADDR_SPI (0x7A)
#ifdef SMPB02E_I2C
#define SMPB02E_TEMP_BASE_ADDR SMPB02E_TEMP_BASE_ADDR_I2C
#else
#define SMPB02E_TEMP_BASE_ADDR SMPB02E_TEMP_BASE_ADDR_SPI
#endif
#define SMPB02E_TEMP_LEN (3)
/* PRESSURE REGISTER */
#define SMPB02E_PRESS_BASE_ADDR_I2C (0xF7)
#define SMPB02E_PRESS_BASE_ADDR_SPI (0x77)
#ifdef SMPB02E_I2C
#define SMPB02E_PRESS_BASE_ADDR SMPB02E_PRESS_BASE_ADDR_I2C
#else
#define SMPB02E_PRESS_BASE_ADDR SMPB02E_PRESS_BASE_ADDR_SPI
#endif
#define SMPB02E_PRESS_LEN (3)

/* SPI Mode */
#define SMPB02E_SPI_MODE_MASTER HAL_SPI_MODE_MASTER
#define SMPB02E_SPI_MODE_SLAVE HAL_SPI_MODE_SLAVE

/* Calibration data */
typedef struct _smpb02e_cali_table_t
{
    double a1;
    double a2;
    double bt1;
    double bt2;
    double bp1;
    double b11;
    double bp2;
    double b12;
    double b21;
    double bp3;
    double a0;
    double b00;
} smpb02e_cali_table_t;
static smpb02e_cali_table_t smpb02e_cali_table;
static uint8_t* smpb02e_coe;

/* APPLICATION MODE */
typedef enum {
    SMPB02E_HIGH_SPEED_MODE = 0,
    SMPB02E_LOW_POWER_MODE,
    SMPB02E_STANDARD_MODE,
    SMPB02E_HIGH_ACCURACY_MODE,
    SMPB02E_ULTRA_HIGH_ACCURACY_MODE,
    SMPB02E_APL_MODE_MAX
} smpb02e_apl_mode_e;

/* TIME STANDBY */
typedef enum {
    SMPB02E_T_STANDBY_1MS = 0,
    SMPB02E_T_STANDBY_5MS,
    SMPB02E_T_STANDBY_50MS,
    SMPB02E_T_STANDBY_250MS,
    SMPB02E_T_STANDBY_500MS,
    SMPB02E_T_STANDBY_1000MS,
    SMPB02E_T_STANDBY_2000MS,
    SMPB02E_T_STANDBY_4000MS,
    SMPB02E_T_STANDBY_MAX
} smpb02e_t_standby_e;

static int smpb02e_io_read(uint8_t reg, uint8_t *data, uint16_t size);
static int smpb02e_io_write(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_baro_omron_smpb02e_io_init(int io_port);
static int drv_baro_omron_smpb02e_spi_read(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_baro_omron_smpb02e_spi_write(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_baro_omron_smpb02e_i2c_read(uint8_t reg, uint8_t *data, uint16_t size);
static int drv_baro_omron_smpb02e_i2c_write(uint8_t reg, uint8_t *data, uint16_t size);
int drv_baro_omron_smpb02e_init(void);
static int drv_baro_omron_smpb02e_open(void);
static int drv_baro_omron_smpb02e_close(void);
static void drv_baro_omron_smpb02e_irq_handle(void);
static int drv_baro_omron_smpb02e_read(void *buf, size_t len);
static int drv_baro_omron_smpb02e_ioctl(int cmd, unsigned long arg);

static int drv_baro_omron_smpb02e_soft_reset(void);
static int drv_baro_omron_smpb02e_validate_id(uint8_t id_value);
static int drv_baro_omron_smpb02e_set_default_config(void);
static int drv_baro_omron_smpb02e_get_cali_parm(void);
static int drv_baro_omron_smpb02e_set_power_mode(smpb02e_power_mode_e mode);
static int drv_baro_omron_smpb02e_set_temp_avg(smpb02e_temp_avg_e avg);
static int drv_baro_omron_smpb02e_set_press_avg(smpb02e_press_avg_e avg);
static int drv_baro_omron_smpb02e_set_iir(smpb02e_iir_e iir);
static double drv_baro_omron_smpb02e_calcCoef1(double a, double s, uint16_t otp);
static double drv_baro_omron_smpb02e_calcCoef2(uint32_t otp);
static double drv_baro_omron_smpb02e_caliTemp(uint8_t* temp);
static double drv_baro_omron_smpb02e_caliPress(uint8_t* press, double Tr);
static smpb02e_t_standby_e drv_baro_omron_smpb02e_hz2odr(int hz);
static int drv_baro_omron_smpb02e_set_odr(smpb02e_t_standby_e t_stb);


i2c_dev_t smpb02e_ctx_i2c = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = SMPB02E_I2C_SPEED_FAST,
    .config.dev_addr      = SMPB02E_I2C_ADDR,
};

spi_dev_t smpb02e_ctx_spi = {
    .port                 = 2,
    .config.mode          = SMPB02E_SPI_MODE_MASTER,
    .config.freq          = 10000000,
};

sensor_io_dev_t smpb02e_ctx;
sensor_io_func_st smpb02e_api = { NULL, NULL };

/* read */
static int smpb02e_io_read(uint8_t reg, uint8_t *data, uint16_t size)
{
    if (unlikely(NULL == smpb02e_api.read)) {
        return -1;
    }

    return smpb02e_api.read(reg, data, size);
}

/* write */
static int smpb02e_io_write(uint8_t reg, uint8_t *data, uint16_t size)
{
    if (NULL == smpb02e_api.write) {
        return -1;
    }

    return smpb02e_api.write(reg, data, size);
}

/* Select I/O type */
static int drv_baro_omron_smpb02e_io_init(int io_port)
{
    memset(&smpb02e_api, 0, sizeof(smpb02e_api));
    memset(&smpb02e_ctx, 0, sizeof(smpb02e_ctx));
    if (io_port == I2C_PORT) {
        smpb02e_ctx.io_port  = I2C_PORT;
        memcpy(&smpb02e_ctx.dev_i2c,&smpb02e_ctx_i2c,sizeof(smpb02e_ctx.dev_i2c));
        smpb02e_api.read  = drv_baro_omron_smpb02e_i2c_read;
        smpb02e_api.write = drv_baro_omron_smpb02e_i2c_write;
        return 0;
    } 
    else if (io_port == SPI_PORT) {
        smpb02e_ctx.io_port  = SPI_PORT;
        memcpy(&smpb02e_ctx.dev_spi,&smpb02e_ctx_spi,sizeof(smpb02e_ctx.dev_spi));
        smpb02e_api.read  = drv_baro_omron_smpb02e_spi_read;
        smpb02e_api.write = drv_baro_omron_smpb02e_spi_write;
        return 0;
    }
    return -1;
}

/* SPI read */
static int drv_baro_omron_smpb02e_spi_read(uint8_t reg, uint8_t *data, uint16_t size)
{
    int i;
    int ret;

    /* allocate address array */
    uint8_t *tx_data = malloc(size+1);
    if (NULL == tx_data) {
        return -1;
    }
    /* allocate receive data array */
    uint8_t *rx_data = malloc(size+1);
    if (NULL == rx_data) {
        free(tx_data);
        return -1;
    }
    /* set addr & init receive data  */
    for (i = 0; i < size+1; i++) {
        tx_data[i] = (uint8_t)0x00;
        rx_data[i] = (uint8_t)0xFF;
    }
    tx_data[0] = (uint8_t)reg | SMPB02E_SPI_READ_FLAG;

    /* read data */
    ret = sensor_io_read(&smpb02e_ctx, tx_data, rx_data, size+1, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        free(tx_data);
        free(rx_data);
        return -1;
    }
    /* set receive data to buffer */
    for (i = 0; i < size; i++) {
        data[i] = rx_data[i+1];
    }
    /* free address array and receive data array */
    free(tx_data);
    free(rx_data);

    return ret;
}

/* SPI write */
static int drv_baro_omron_smpb02e_spi_write(uint8_t reg, uint8_t *data, uint16_t size)
{
    int i;
    int ret;

    /* allocate send data array */
    uint8_t *tx_data = malloc(size * 2);
    if (NULL == tx_data) {
        return -1;
    }
    /* set addr & data to be sent */
    for (i = 0; i < size; i++) {
        tx_data[2*i] = (uint8_t)(reg + i) & SMPB02E_SPI_WRITE_FLAG;
        tx_data[2*i+1] = (uint8_t)data[i];
    }
    
    /* send data */
    ret = sensor_io_write(&smpb02e_ctx, NULL, tx_data, size*2, I2C_OP_RETRIES);

    free(tx_data);

    return ret;
}

/* I2C read */
static int drv_baro_omron_smpb02e_i2c_read(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t addr = reg;
    return sensor_io_read(&smpb02e_ctx, &addr, data, size, I2C_OP_RETRIES);
}

/* I2C write */
static int drv_baro_omron_smpb02e_i2c_write(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t addr = reg;
    return sensor_io_write(&smpb02e_ctx, &addr, data, size, I2C_OP_RETRIES);
}

/* Sensor init */
int drv_baro_omron_smpb02e_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
#ifdef SMPB02E_I2C
    sensor.io_port    = I2C_PORT;
#else
    sensor.io_port    = SPI_PORT;
#endif
    sensor.tag        = TAG_DEV_BARO;
    sensor.path       = dev_baro_path;
    sensor.open       = drv_baro_omron_smpb02e_open;
    sensor.close      = drv_baro_omron_smpb02e_close;
    sensor.read       = drv_baro_omron_smpb02e_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_baro_omron_smpb02e_ioctl;
    sensor.irq_handle = drv_baro_omron_smpb02e_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_baro_omron_smpb02e_io_init(sensor.io_port);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_baro_omron_smpb02e_soft_reset();
    if (unlikely(ret)) {
        return -1;
    }

    /* Wait 10ms */
    aos_msleep(10);

    ret = drv_baro_omron_smpb02e_set_default_config();
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_baro_omron_smpb02e_validate_id(SMPB02E_CHIP_ID);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_baro_omron_smpb02e_get_cali_parm();
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

/* Driver APIs */
static int drv_baro_omron_smpb02e_open(void)
{
    int ret = 0;

    ret = drv_baro_omron_smpb02e_set_power_mode(SMPB02E_POWER_MODE_NORMAL);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_close(void)
{
    int ret = 0;
    
    ret = drv_baro_omron_smpb02e_set_power_mode(SMPB02E_POWER_MODE_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static void drv_baro_omron_smpb02e_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_omron_smpb02e_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size = 0;
    uint8_t temp_raw[SMPB02E_TEMP_LEN];
    uint8_t press_raw[SMPB02E_PRESS_LEN];
    double temperature = 0.0;
    double pressure = 0.0;

    barometer_data_t *pdata = (barometer_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(barometer_data_t);
    if (len < size) {
        return -1;
    }

    /* Read raw temp data */
    ret = smpb02e_io_read(SMPB02E_TEMP_BASE_ADDR, temp_raw, SMPB02E_TEMP_LEN);
    if (unlikely(ret)) {
        return -1;
    }
    /* Read raw press data */
    ret = smpb02e_io_read(SMPB02E_PRESS_BASE_ADDR, press_raw, SMPB02E_PRESS_LEN);
    if (unlikely(ret)) {
        return -1;
    }
    /* temp calibration */
    temperature = drv_baro_omron_smpb02e_caliTemp(temp_raw);

    /* press calibration */
    pressure = drv_baro_omron_smpb02e_caliPress(press_raw, temperature);

    /* set data */
    pdata->p = (uint32_t)pressure;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_baro_omron_smpb02e_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: { /* set avg times for temperature/pressure */
            smpb02e_t_standby_e odr = drv_baro_omron_smpb02e_hz2odr((int)arg);
            ret = drv_baro_omron_smpb02e_set_odr(odr);
            if (unlikely(ret)) {
                return -1;
            }
            break;
        }
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->vendor = DEV_SENSOR_VENDOR_OMRON;
            info->model = "SMPB02E";
            info->unit = pa;
            info->range_max = 110000;
            info->range_min = 30000;
            break;
        }
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_baro_omron_smpb02e_set_power_mode((smpb02e_power_mode_e)arg);
            if (unlikely(ret)) {
                return -1;
            }
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}

/* Local functions */
static int drv_baro_omron_smpb02e_soft_reset(void)
{
    int ret = 0;
    uint8_t value;

    value = SMPB02E_RESET_VAL;
    ret = smpb02e_io_write(SMPB02E_RESET_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_validate_id(uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

     /* read chip ID */
    ret = smpb02e_io_read(SMPB02E_CHIP_ID_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_set_default_config(void)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    /* I/O setup */
    ret = smpb02e_io_write(SMPB02E_IO_SETUP_ADDR, &value, 1);   /* t_standby = 1ms */
    if (unlikely(ret)) {
        return ret;
    }

    /* To sleep mode */
    ret = drv_baro_omron_smpb02e_set_power_mode(SMPB02E_POWER_MODE_SLEEP);
    if (unlikely(ret)) {
        return ret;
    }

    /* Set temperature average times */
    ret = drv_baro_omron_smpb02e_set_temp_avg(SMPB02E_TEMP_AVG_1);  /* stadard mode */
    if (unlikely(ret)) {
        return ret;
    }
    /* Set press average times */
    ret = drv_baro_omron_smpb02e_set_press_avg(SMPB02E_PRESS_AVG_8);  /* stadard mode */
    if (unlikely(ret)) {
        return ret;
    }
    /* Set IIR */
    ret = drv_baro_omron_smpb02e_set_iir(SMPB02E_IIR_OFF);  /* IIR OFF */
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_get_cali_parm(void)
{
    int i;
    int ret = 0;

    smpb02e_coe = malloc(SMPB02E_COE_NUM);
    if (NULL == smpb02e_coe) {
        return -1;
    }
    
    /* read calibration param */
    ret = smpb02e_io_read(SMPB02E_COE_BASE_ADDR, smpb02e_coe, SMPB02E_COE_NUM);
    if (unlikely(ret)) {
        return ret;
    }

    smpb02e_cali_table.a1 = drv_baro_omron_smpb02e_calcCoef1(-6.3E-03, 4.3E-04, ((((uint16_t)smpb02e_coe[20])<<8) | (uint16_t)smpb02e_coe[21]));
    smpb02e_cali_table.a2 = drv_baro_omron_smpb02e_calcCoef1(-1.9E-11, 1.2E-10, ((((uint16_t)smpb02e_coe[22])<<8) | (uint16_t)smpb02e_coe[23]));
    smpb02e_cali_table.bt1 = drv_baro_omron_smpb02e_calcCoef1(1.0E-01, 9.1E-02, ((((uint16_t)smpb02e_coe[2])<<8) | (uint16_t)smpb02e_coe[3]));
    smpb02e_cali_table.bt2 = drv_baro_omron_smpb02e_calcCoef1(1.2E-08, 1.2E-06, ((((uint16_t)smpb02e_coe[4])<<8) | (uint16_t)smpb02e_coe[5]));
    smpb02e_cali_table.bp1 = drv_baro_omron_smpb02e_calcCoef1(3.3E-02, 1.9E-02, ((((uint16_t)smpb02e_coe[6])<<8) | (uint16_t)smpb02e_coe[7]));
    smpb02e_cali_table.b11 = drv_baro_omron_smpb02e_calcCoef1(2.1E-07, 1.4E-07, ((((uint16_t)smpb02e_coe[8])<<8) | (uint16_t)smpb02e_coe[9]));
    smpb02e_cali_table.bp2 = drv_baro_omron_smpb02e_calcCoef1(-6.3E-10, 3.5E-10, ((((uint16_t)smpb02e_coe[10])<<8) | (uint16_t)smpb02e_coe[11]));
    smpb02e_cali_table.b12 = drv_baro_omron_smpb02e_calcCoef1(2.9E-13, 7.6E-13, ((((uint16_t)smpb02e_coe[12])<<8) | (uint16_t)smpb02e_coe[13]));
    smpb02e_cali_table.b21 = drv_baro_omron_smpb02e_calcCoef1(2.1E-15, 1.2E-14, ((((uint16_t)smpb02e_coe[14])<<8) | (uint16_t)smpb02e_coe[15]));
    smpb02e_cali_table.bp3 = drv_baro_omron_smpb02e_calcCoef1(1.3E-16, 7.9E-17, ((((uint16_t)smpb02e_coe[16])<<8) | (uint16_t)smpb02e_coe[17]));
    smpb02e_cali_table.a0 = drv_baro_omron_smpb02e_calcCoef2((((uint32_t)smpb02e_coe[18])<<12) | (((uint32_t)smpb02e_coe[19])<<4) | (uint32_t)(smpb02e_coe[24] & 0x0F));
    smpb02e_cali_table.b00 = drv_baro_omron_smpb02e_calcCoef2((((uint32_t)smpb02e_coe[0])<<12) | (((uint32_t)smpb02e_coe[1])<<4) | (uint32_t)((smpb02e_coe[24] & 0xF0)>>4));

    return 0;
}

static int drv_baro_omron_smpb02e_set_power_mode(smpb02e_power_mode_e mode)
{
    uint8_t value;
    int ret = 0;

    if (SMPB02E_POWER_MODE_MAX <= mode) {
        return -1;
    }
    ret = smpb02e_io_read(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }
    value = (value & 0xFC) | (mode & 0x03);
    ret = smpb02e_io_write(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_set_temp_avg(smpb02e_temp_avg_e avg)
{
    uint8_t value;
    int ret = 0;

    if (SMPB02E_TEMP_AVG_MAX <= avg) {
        return -1;
    }
    ret = smpb02e_io_read(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }
    value = (value & 0x1F) | ((avg & 0x07) << 5);
    ret = smpb02e_io_write(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_set_press_avg(smpb02e_press_avg_e avg)
{
    uint8_t value;
    int ret = 0;

    if (SMPB02E_PRESS_AVG_MAX <= avg) {
        return -1;
    }
    ret = smpb02e_io_read(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }
    value = (value & 0xE3) | ((avg & 0x07) << 2);
    ret = smpb02e_io_write(SMPB02E_CTRL_MEAS_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_baro_omron_smpb02e_set_iir(smpb02e_iir_e iir)
{
    uint8_t value;
    int ret = 0;

    if (SMPB02E_IIR_MAX <= iir) {
        return -1;
    }
    value = iir & 0x07;
    ret = smpb02e_io_write(SMPB02E_IIR_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static double drv_baro_omron_smpb02e_calcCoef1(double a, double s, uint16_t otp)
{
    int16_t data;
    double result;
    
    if ((otp & 0x8000) != 0) {
        data = (int16_t)((int32_t)otp - 0x00010000L);
    }
    else {
        data = (int16_t)otp;
    }
    result = a + (s* data) / 32767.0;    /* k = a + (s * otp) / 32767 */
    return result;
}

static double drv_baro_omron_smpb02e_calcCoef2(uint32_t otp)
{
    int32_t data;
    double result;
    
    if ((otp & 0x00080000) != 0) {
        data = (int32_t)otp - 0x00100000L;
    }
    else {
        data = (int32_t)otp;
    }
    result = data / 16.0;    /* k = otp / 16 */
    return result;
}

static double drv_baro_omron_smpb02e_caliTemp(uint8_t* temp)
{
    int32_t Dt;
    double Tr;
    
    Dt = (int32_t)((((uint32_t)temp[0])<<16) | (((uint32_t)temp[1])<<8) | ((uint32_t)temp[2])) - 0x00800000L;
    Tr = smpb02e_cali_table.a0 + smpb02e_cali_table.a1 * Dt + smpb02e_cali_table.a2 * Dt * Dt;

    return Tr;
}

static double drv_baro_omron_smpb02e_caliPress(uint8_t* press, double Tr)
{
    int32_t Dp;
    double Pr;
    
    Dp = (int32_t)((((uint32_t)press[0])<<16) | (((uint32_t)press[1])<<8) | ((uint32_t)press[2])) - 0x00800000L;
    Pr = smpb02e_cali_table.b00 + smpb02e_cali_table.bt1 * Tr + smpb02e_cali_table.bp1 * Dp + smpb02e_cali_table.b11 * Dp * Tr + smpb02e_cali_table.bt2 * Tr * Tr + smpb02e_cali_table.bp2 * Dp * Dp + smpb02e_cali_table.b12 * Dp * Tr * Tr + smpb02e_cali_table.b21 * Dp * Dp * Tr + smpb02e_cali_table.bp3 * Dp * Dp * Dp;

    return Pr;
}

static smpb02e_t_standby_e drv_baro_omron_smpb02e_hz2odr(int hz)
{
    if (hz > 64)
        return SMPB02E_T_STANDBY_1MS;
    else if (hz > 16)
        return SMPB02E_T_STANDBY_5MS;
    else if (hz > 3)
        return SMPB02E_T_STANDBY_50MS;
    else if (hz > 2)
        return SMPB02E_T_STANDBY_250MS;
    else if (hz > 1)
        return SMPB02E_T_STANDBY_500MS;
    else
        return SMPB02E_T_STANDBY_1000MS;
}

static int drv_baro_omron_smpb02e_set_odr(smpb02e_t_standby_e t_stb)
{
    int ret = 0;
    uint8_t value;
    uint8_t temp;

    value = (uint8_t)t_stb;
    if (value >= SMPB02E_T_STANDBY_MAX) {
        return -1;
    }
    ret = smpb02e_io_read(SMPB02E_IO_SETUP_ADDR, &temp, 1);
    if (unlikely(ret)) {
        return ret;
    }
    value = (temp & 0x1F) | (value << 5);
    ret = smpb02e_io_write(SMPB02E_IO_SETUP_ADDR, &value, 1);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}
