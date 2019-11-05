/*
* Copyright (C) 2018 Sensirion Inc.
* Author: Johannes Winkelmann, jwi@sensirion.com
*
* Driver for SCD30
* Based on SGP30 driver, and arduino-scd library
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define POLYNOMIAL  0x131

#define SCD30_I2C_SLAVE_ADDR                            0x61

#define SCD30_ADDR_TRANS(n)                             ((n) << 1)
#define SCD30_I2C_ADDR                                  SCD30_ADDR_TRANS(SCD30_I2C_SLAVE_ADDR)

#define CMD_CONT_MEASUREMENT_LEN 5
static  uint8_t CMD_CONT_MEASUREMENT[CMD_CONT_MEASUREMENT_LEN] = {
    0x00, 0x10, 0x00, 0x00, 0x81
};

// Note: in this library, we're hardcoding the interval to 2 seconds
#define CMD_SET_INTERVAL_LEN 5
static  uint8_t CMD_SET_INTERVAL[CMD_SET_INTERVAL_LEN] = { 0x46, 0x00,
                                                                0x00, 0x01,
                                                                0xE3 };
#define CMD_STOP_MEASUREMENT_LEN 2
static const uint8_t CMD_STOP_MEASUREMENT[CMD_STOP_MEASUREMENT_LEN] = { 0x01,
                                                                        0x04 };

#define CMD_READ_MEASUREMENT_LEN 2
#define CMD_READ_MEASUREMENT_RESULT_LEN 18
static uint8_t CMD_READ_MEASUREMENT[CMD_READ_MEASUREMENT_LEN] = { 0x03,
                                                                        0x00 };

#define CMD_DATA_READY_LEN 2
#define CMD_DATA_READY_RESULT_LEN 4
static uint8_t CMD_DATA_READY[CMD_DATA_READY_LEN] = { 0x02, 0x02 };

/*
 * default port = 3
 * Use "GLOBAL_DEFINES += SENSIRION_SCD30_PORT=2" in a Makefile to override
*/
#ifndef SENSIRION_SCD30_PORT
#define SENSIRION_SCD30_PORT 3
#endif /* SENSIRION_SCD30_PORT */

i2c_dev_t scd30_ctx = {
    .port = SENSIRION_SCD30_PORT,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = SCD30_I2C_ADDR,
};

static float g_scd30_most_recent_value = 0;

static uint8_t SCD30_CalcCrc(uint8_t data[], uint8_t nbrOfBytes)
{
  uint8_t bit;        // bit mask
  uint8_t crc = 0xFF; // calculated checksum
  uint8_t byteCtr;    // byte counter
  
  // calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  
  return crc;
}
static float scd30_convert_to_float(uint8_t *data)
{
    void* addr;
    uint32_t val = (((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                    ((uint32_t)data[3] << 8) | ((uint32_t)data[4]));
    addr = (void*)(&val);
    return *(float *)addr;
}

static void scd30_delay_ms(uint32_t delay_time)
{
    // TODO: krhino_task_sleep is currently blocking; therefore, I implemented
    // a busy wait loop
    //
    // krhino_task_sleep(krhino_ms_to_ticks(delay_time));
    uint32_t start_ms = aos_now_ms();
    while (aos_now_ms() - start_ms < delay_time) {
        // idle
    }
}

static int drv_scd30_cmd_write(i2c_dev_t* drv, uint8_t* cmd, uint8_t len)
{
    return hal_i2c_master_send(drv, drv->config.dev_addr, cmd, len, AOS_WAIT_FOREVER);
}

static int drv_scd30_result_read(i2c_dev_t* drv, uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0)
        return -1;

    return hal_i2c_master_recv(drv, drv->config.dev_addr, data, size, AOS_WAIT_FOREVER);
}

static int scd30_check_data_ready(i2c_dev_t* drv)
{
  bool dataReady = false;
  int ret = drv_scd30_cmd_write(drv, CMD_DATA_READY, CMD_DATA_READY_LEN);

  if (ret != 0) {
    return 1;
  } else {
    uint8_t buf[CMD_DATA_READY_RESULT_LEN] = { 0 };
    ret = drv_scd30_result_read(drv, buf, CMD_DATA_READY_RESULT_LEN);
    if (ret != 0) {
      return 2;
    } else {
      dataReady = (buf[1] == 1);
    }
  }
  return dataReady;
}

static int drv_scd30_read_raw_data(i2c_dev_t *drv, integer_data_t *pdata)
{
    int ret = 0;
    uint8_t data[CMD_READ_MEASUREMENT_RESULT_LEN] = {0};

    ret = scd30_check_data_ready(drv);
    if (ret) {
        ret = drv_scd30_cmd_write(drv, CMD_READ_MEASUREMENT, CMD_READ_MEASUREMENT_LEN);
        if (unlikely(ret)) {
            return ret;
        }
        ret = drv_scd30_result_read(drv, data, CMD_READ_MEASUREMENT_RESULT_LEN);
        if (unlikely(ret)) {
            return ret;
        }
        g_scd30_most_recent_value = scd30_convert_to_float(data);
    }

    pdata->data = g_scd30_most_recent_value;

    return ret;
}

static int drv_scd30_init_sensor(i2c_dev_t* drv)
{
    int ret = 0;
    
    CMD_SET_INTERVAL[4] = SCD30_CalcCrc(&CMD_SET_INTERVAL[2],2);
    ret = drv_scd30_cmd_write(drv, CMD_SET_INTERVAL, CMD_SET_INTERVAL_LEN);
    if (unlikely(ret)) {
        return ret;
    }
    // workaround for firmware bug in early samples
    scd30_delay_ms(100);
    CMD_CONT_MEASUREMENT[4] = SCD30_CalcCrc(&CMD_CONT_MEASUREMENT[2],2);
    return drv_scd30_cmd_write(drv, CMD_CONT_MEASUREMENT, CMD_CONT_MEASUREMENT_LEN);
}



static void drv_co2_sensirion_scd30_irq_handle(void)
{
    /* no handle so far */
}

static int drv_co2_sensirion_scd30_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_co2_sensirion_scd30_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_co2_sensirion_scd30_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size  = sizeof(integer_data_t);
    integer_data_t * pdata = (integer_data_t *)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }
    
    ret = drv_scd30_read_raw_data(&scd30_ctx, pdata);
    if (ret != 0)
        return -1;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_co2_sensirion_scd30_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_co2_sensirion_scd30_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "SCD30";
            //info->unit = ppm;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


int drv_co2_sensirion_scd30_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_temp;
    memset(&sensor_temp, 0, sizeof(sensor_temp));

    ret = drv_scd30_init_sensor(&scd30_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    /* fill the sensor_temp obj parameters here */
    sensor_temp.tag = TAG_DEV_CO2;
    sensor_temp.path = dev_co2_path;
    sensor_temp.io_port = I2C_PORT;
    sensor_temp.open = drv_co2_sensirion_scd30_open;
    sensor_temp.close = drv_co2_sensirion_scd30_close;
    sensor_temp.read = drv_co2_sensirion_scd30_read;
    sensor_temp.write = drv_co2_sensirion_scd30_write;
    sensor_temp.ioctl = drv_co2_sensirion_scd30_ioctl;
    sensor_temp.irq_handle = drv_co2_sensirion_scd30_irq_handle;

    ret = sensor_create_obj(&sensor_temp);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_co2_sensirion_scd30_init);
