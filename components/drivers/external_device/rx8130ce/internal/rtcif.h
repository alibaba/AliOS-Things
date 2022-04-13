#ifndef RTCIF_H
#define RTCIF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===== System include ===== */
#include "aos/hal/i2c.h"
#include "aos/kernel.h"
#include "rx8130ce.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************
 * @fun    rtc_i2c_init
 * @breif  rtc i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rtc_i2c_init(i2c_dev_t *i2c);

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
                  uint32_t timeout);

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
                     uint32_t timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_PCA9544_H */
