#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#include "esp8266/pin_mux_register.h"
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO2_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_GPIO4_U
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 4
#define I2C_MASTER_SDA_FUNC FUNC_GPIO2
#define I2C_MASTER_SCL_FUNC FUNC_GPIO4

//#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO2_U
//#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_GPIO0_U
//#define I2C_MASTER_SDA_GPIO 2
//#define I2C_MASTER_SCL_GPIO 0
//#define I2C_MASTER_SDA_FUNC FUNC_GPIO2
//#define I2C_MASTER_SCL_FUNC FUNC_GPIO0

#if 0
#define I2C_MASTER_GPIO_SET(pin)  \
    gpio_output_set(1<<pin,0,1<<pin,0)

#define I2C_MASTER_GPIO_CLR(pin) \
    gpio_output_set(0,1<<pin,1<<pin,0)

#define I2C_MASTER_GPIO_OUT(pin,val) \
    if(val) I2C_MASTER_GPIO_SET(pin);\
    else I2C_MASTER_GPIO_CLR(pin)
#endif

#define I2C_MASTER_SDA_HIGH_SCL_HIGH()  \
    gpio_output_set(1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_HIGH_SCL_LOW()  \
    gpio_output_set(1<<I2C_MASTER_SDA_GPIO, 1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_LOW_SCL_HIGH()  \
    gpio_output_set(1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_LOW_SCL_LOW()  \
    gpio_output_set(0, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup I2C_Driver_APIs I2C_MASTER Driver APIs
  * @brief UART driver APIs
  */

/** @addtogroup I2C_MASTER_Driver_APIs
  * @{
  */

/**  
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  * 
  * @param   null
  *  
  * @return  null
  */
void i2c_master_gpio_init(void);

/**  
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  * 
  * @param  initilize I2C bus to enable i2c operations.
  *  
  * @return  null
  */
void i2c_master_init(void);

#define i2c_master_wait    os_delay_us


/**  
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  * 
  * @param  set i2c to stop sending state.
  *  
  * @return  null
  */
void i2c_master_stop(void);

/**  
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  * 
  * @param  set i2c to start sending state.
  *  
  * @return  null
  */
void i2c_master_start(void);

/**  
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  * 
  * @param  set ack to i2c bus as level value.
  *  
  * @return  null
  */
void i2c_master_setAck(uint8 level);

/**  
  * @brief   confirm if peer send ack.
  * 
  * @param null
  *  
  * @return  null
  */
uint8 i2c_master_getAck(void);

/**  
  * @brief   read Byte from i2c bus.
  * 
  * @param null
  *  
  * @return  the byte which read from i2c bus.
  */
uint8 i2c_master_readByte(void);

/**  
  * @brief   write wrdata value(one byte) into i2c.
  * 
  * @param uint8 wrdata:write value
  *  
  * @return  null
  */
void i2c_master_writeByte(uint8 wrdata);

/**  
  * @brief   i2c_master_checkAck.
  * 
  * @param null
  *  
  * @return  the result of check ack
  */
bool i2c_master_checkAck(void);

/**  
  * @brief   i2c master send Ack.
  * 
  * @param null
  *  
  * @return  null
  */
void i2c_master_send_ack(void);

/**  
  * @brief   i2c master send Nack.
  * 
  * @param null
  *  
  * @return  null
  */
void i2c_master_send_nack(void);

/**
  * @}
  */

/**
  * @}
  */

#endif
