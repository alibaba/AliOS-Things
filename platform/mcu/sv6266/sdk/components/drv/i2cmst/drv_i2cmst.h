#ifndef I2CMST
#define I2CMST
#include "hal_i2cmst.h"

#define REPEAT_CNT (1000)


/**
 * Initializes I2C registers with initial values provided from the customizer.
 * 
 * @param   frequency       The number of data bits in each character. 
 *
 * @return  -1              Index error!! only support index 0:I2C1, 1:I2C2 
 * @return   0              The operation completed successfully.
 */
int8_t drv_i2c_mst_init(uint16_t frequency);



/**
 * Deinitialize I2C registers.
 *
 * @return  -1              Index error!! only support index 0:I2C1, 1:I2C2 
 * @return   0              The operation completed successfully.
 */
int8_t drv_i2c_mst_deinit(void);



/**
 * Writes the referenced data buffer to a specified slave address.
 * 
 * @param   slave_address   The I2C slave address for 7 bit.
 * @param   data            Pointer to the data buffer to be written by the master.
 * @param   length          Size of the write buffer exposed to the I2C master. 
 *
 * @return  -1              Slave has no ack. 
 * @return   0              The operation completed successfully.
 */
int8_t drv_i2c_mst_write(uint8_t slave_address, uint8_t *data, uint32_t length);



/**
 * Reads data from the specified slave address and places the data in the referenced buffer.
 * 
 * @param   slave_address   The I2C slave address for 7 bit.
 * @param   data            Pointer to the data buffer to be read by the master.
 * @param   length          Size of the write buffer exposed to the I2C master. 
 *
 * @return  -1              Slave has no ack. 
 * @return   0              The operation completed successfully.
 */
int8_t drv_i2c_mst_read(uint8_t slave_address, uint8_t *data, uint32_t length);

#endif /* end of include guard */

