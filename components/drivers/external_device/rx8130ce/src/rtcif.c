/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 *
 */
#include "aos/hal/i2c.h"
#include "aos/kernel.h"
#include "rx8130ce.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef I2C_ITF_SET
#include "pca9544.h"
#define I2C_BY_PCA9544
#define PCA9544_RTC_CHAN 5

PCA9544_DEV_CFG_T dev_cfg = {
    .dev_addr = PCA9544_BASE_ADDR,
    .pca9544_ch = PCA9544_RTC_CHAN,
    .subdev_addr = 0x32,
    .reg_addr = 0x10,
};

#endif

/*********************************************************
 * @fun    rtc_i2c_init
 * @breif  rtc i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rtc_i2c_init(i2c_dev_t *i2c)
{
  int32_t ret;

  if (i2c == NULL) {
    printf("i2c cfg is null\r\n");
    return -1;
  }

  ret = hal_i2c_init(i2c);
  if (ret) {
    printf("rtc i2c init fail \r\n");
    return -1;
  }

#ifdef I2C_BY_PCA9544

  ret = pca9544_init(i2c, &dev_cfg);
  if (ret) {
    printf("rtc i2c(pca9544) init fail\r\n");
    return -1;
  }

  ret = pca9544_set_chan(PCA9544_RTC_CHAN);
  if (ret) {
    printf("pca9544 set rtc channel fail\r\n");
    return -1;
  }
#endif
}

/*********************************************************
 * @fun    rtc_i2c_write
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  reg_addr       register address
 * @param[in]  reg_num        register num
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rtc_i2c_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t reg_addr,
                  uint16_t reg_num, const uint8_t *data, uint16_t size,
                  uint32_t timeout)
{
  int32_t ret = 0;

  if (i2c == NULL) {
    printf("i2c param is null\r\n");
    return -1;
  }
#ifndef I2C_BY_PCA9544
  ret =
      hal_i2c_mem_write(i2c, dev_addr, reg_addr, reg_num, data, size, timeout);
  if (ret) {
    printf("rtc_i2c reg write failed\r\n");
    return -1;
  }

#else
  /*printf("rtc i2c: by pca9544");*/
  ret = pca9544_raw_data_write(dev_addr, reg_addr, reg_num, data, size);
  if (ret) {
    printf("rtc reg write failed\r\n");
    return -1;
  }
#endif
}

/*********************************************************
 * @fun    rtc_i2c_read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   reg_addr       register address
 * @param[in]   reg_num        register number
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t rtc_i2c_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t reg_addr,
                     uint16_t reg_num, const uint8_t *data, uint16_t size,
                     uint32_t timeout)
{
  int32_t ret = 0;

  if (i2c == NULL) {
    printf("i2c param is null\r\n");
    return -1;
  }

#ifndef I2C_BY_PCA9544
  ret = hal_i2c_mem_read(i2c, dev_addr, reg_addr, reg_num, data, size, timeout);
  if (ret) {
    printf("rtc_i2c reg read failed\r\n");
    return -1;
  }
#else
  /*printf("rtc i2c: by pca9544");*/
  ret = pca9544_raw_data_read(dev_addr, reg_addr, reg_num, data, size);
  if (ret) {
    printf("rtc reg read failed\r\n");
    return -1;
  }
#endif
}
