#ifndef PCA9544_I2C_DRV_H
#define PCA9544_I2C_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===== System include ===== */
#include "aos/hal/i2c.h"

/* ===== API Lists ===== */
/**********************************************************
 * @fun    pca9544_i2c_init
 * @breif  pca9544 i2c initialization
 * @param  i2c:the pointer for i2c configuration
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_init(i2c_dev_t *i2c);

/********************************************************
 * @fun    pca9544_i2c_master_send
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data, uint16_t size, uint32_t timeout);

/********************************************************
 * @fun    pca9544_i2c_master_recv
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  data           i2c master recv data
 * @param[in]  size           i2c master recv data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data, uint16_t size, uint32_t timeout);

/*********************************************************
 * @fun    pca9544_i2c_mem_write
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       register address
 * @param[in]  mem_addr_size  register address size
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t* data, uint16_t size, uint32_t timeout);

/*********************************************************
 * @fun    pca9544_i2c_mem_read
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       register address
 * @param[in]  mem_addr_size  register address size
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to
 *HAL_WAIT_FOREVER if you want to wait forever
 *
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t* data, uint16_t size, uint32_t timeout);

/********************************************************
 * @fun    pca9544_i2c_finalize
 * @param[in]  i2c            the i2c device
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int32_t pca9544_i2c_finalize(i2c_dev_t *i2c);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCA9544_I2C_DRV_H */
