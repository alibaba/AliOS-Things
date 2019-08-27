#ifndef _LEGA_I2C_H_
#define _LEGA_I2C_H_
#include <stdint.h>

#define I2C_DEVICE0     0
#define I2C_DEVICE1     1
#define LEGA_I2C_NUM    2

#define I2C_ADDR_WIDTH_7BIT     7
#define I2C_ADDR_WIDTH_10BIT    10

#define I2C_SS_MAX_SCL_FREQ         100000 //100K
#define I2C_FS_MAX_SCL_FREQ         400000 //400K
#define I2C_FS_PLUS_MAX_SCL_FREQ    1000000 //1M
#define I2C_HS_MAX_SCL_FREQ         3400000 //3.4M

#define I2C_FIFO_DEPTH              32
#define I2C_BUF_QUEUE_SIZE          I2C_FIFO_DEPTH //byte

#define I2C_MODE_MASTER 1          /* i2c communication is master mode */
#define I2C_MODE_SLAVE  2          /* i2c communication is slave mode */

#define I2C_MEM_ADDR_SIZE_8BIT  1  /* i2c menory address size 8bit */
#define I2C_MEM_ADDR_SIZE_16BIT 2  /* i2c menory address size 16bit */

typedef void (*lega_i2c_slv_tx_callback_func)(void);
typedef void (*lega_i2c_slv_rx_callback_func)(uint8_t);
typedef struct
{
    lega_i2c_slv_tx_callback_func tx_func;
    lega_i2c_slv_rx_callback_func rx_func;
}lega_i2c_slv_callback_t;

typedef struct {
    uint32_t address_width;
    uint32_t freq;
    uint8_t  mode;
    uint16_t dev_addr;
} lega_i2c_config_t;

typedef struct {
    uint8_t      port;    /* i2c port */
    lega_i2c_config_t config;  /* i2c config */
    void        *priv;    /* priv data */
} lega_i2c_dev_t;

int32_t lega_i2c_reset(lega_i2c_dev_t *i2c);

/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_init(lega_i2c_dev_t *i2c);

/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  data      i2c send data
 * @param[in]  size      i2c send data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_master_send(lega_i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout);

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address
 * @param[out]  data      i2c receive data
 * @param[in]   size      i2c receive data size
 * @param[in]   timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_master_recv(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout);
/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  mem_addr_size  mem address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_mem_write(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout);

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[in]   mem_addr_size  mem address
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                             if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t lega_i2c_mem_read(lega_i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout);

int32_t lega_i2c_tx_data(lega_i2c_dev_t *i2c, uint8_t data);
/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t lega_i2c_finalize(lega_i2c_dev_t *i2c);

#endif //_LEGA_I2C_H_