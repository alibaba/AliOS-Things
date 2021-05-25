/** mbed Microcontroller Library
  ******************************************************************************
  * @file    i2c_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed I2C API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ****************************************************************************** 
  */
#ifndef MBED_I2C_API_H
#define MBED_I2C_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup i2c       I2C
 *  @ingroup    hal
 *  @brief      I2C functions
 *  @{
 */

///@name Ameba Common 
///@{
typedef struct i2c_s i2c_t;

enum {
  I2C_ERROR_NO_SLAVE = -1,
  I2C_ERROR_BUS_BUSY = -2
};

/**
  * @brief  Initializes the I2C device, include clock/function/I2C registers.
  * @param  obj: i2c object define in application software.
  * @param  sda: SDA PinName according to pinmux spec.
  * @param  scl: SCL PinName according to pinmux spec.
  * @retval none  
  */
void i2c_init(i2c_t *obj, PinName sda, PinName scl);

/**
  * @brief  Set i2c frequency.
  * @param  obj: i2c object define in application software.
  * @param  hz: i2c clock(unit is Hz).
  * @retval none  
  */
void i2c_frequency(i2c_t *obj, int hz);

/**
  * @brief  Start i2c device.
  * @param  obj: i2c object define in application software.
  * @retval 0  
  */
int i2c_start(i2c_t *obj);

/**
  * @brief  Stop i2c device.
  * @param  obj: i2c object define in application software.
  * @retval 0  
  */
int i2c_stop(i2c_t *obj);

/**
  * @brief  Deinitializes the I2C device
  * @param  obj: i2c object define in application software.
  * @retval none  
  */
void i2c_reset(i2c_t *obj);

/**
  * @brief  I2C master receive single byte.
  * @param  obj: i2c object define in application software.
  * @param  last: hold the received data.
  * @retval : the received data. 
  */
int i2c_byte_read(i2c_t *obj, int last);

/**
  * @brief  I2C master send single byte.
  * @param  obj: i2c object define in application software.
  * @param  data: the data to be sent.
  * @retval : result. 
  */
int i2c_byte_write(i2c_t *obj, int data);

/**
  * @brief  Set i2c device to be slave.
  * @param  obj: i2c object define in application software.
  * @param  enable_slave: enable slave function, this parameter can be one of the following values:
  *		@arg 0 disable
  *		@arg 1 enable
  * @retval none  
  */
void i2c_slave_mode(i2c_t *obj, int enable_slave);

/**
  * @brief  Get i2c slave state.
  * @param  obj: i2c object define in application software.
  * @retval : the state of i2c slave.
  */
int i2c_slave_receive(i2c_t *obj);

/**
  * @brief  Set i2c slave address.
  * @param  obj: i2c object define in application software.
  * @param  idx: i2c index, this parameter can be one of the following values:
  *		@arg 0 I2C0 Device
  *		@arg 1 I2C1 Device 
  * @param  address: slave address.
  * @param  mask: the mask of address  
  * @retval none  
  */
void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask);
/**
  * @brief  I2C master read in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the buffer to hold the received data.
  * @param  length: the length of data that to be received.
  * @param  stop: specifies whether a STOP is issued after all the bytes are received.
  * @retval : the length of data received. 
  */
int i2c_read(i2c_t *obj, int address, char *data, int length, int stop);

/**
  * @brief  I2C master write in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the data to be sent.
  * @param  length: the length of data that to be sent.
  * @param  stop: specifies whether a STOP is issued after all the bytes are sent.
  * @retval : the length of data send. 
  */
int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop);

/**
  * @brief  I2C slave read in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  data: point to the buffer to hold the received data.
  * @param  length: the length of data that to be received.
  * @retval : the length of data received. 
  */
int i2c_slave_read(i2c_t *obj, char *data, int length);

/**
  * @brief  I2C slave write in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  data: point to the data to be sent.
  * @param  length: the length of data that to be sent.
  * @retval 0: FAIL 
  * @retval 1: SUCCESS
  */
int i2c_slave_write(i2c_t *obj, const char *data, int length);

/**
  * @brief  Set/clear i2c slave RD_REQ interrupt mask.
  * @param  obj: i2c object define in application software.
  * @param  set: set or clear for read request.
  * @retval 1: SUCCESS 
  */
int i2c_slave_set_for_rd_req(i2c_t *obj, int set);

/**
  * @brief  Set/clear i2c slave NAK or ACK data part in transfer.
  * @param  obj: i2c object define in application software.
  * @param  set_nak: set or clear for data NAK.
  * @retval 1: SUCCESS  
  */
int i2c_slave_set_for_data_nak(i2c_t *obj, int set_nak);
///@}

#if ((defined(CONFIG_PLATFORM_8711B) && (CONFIG_PLATFORM_8711B == 1)) || (defined (CONFIG_PLATFORM_8721D) && (CONFIG_PLATFORM_8721D == 1)))
///@name AmebaZ and AmebaD
///@{
/**
  * @brief  I2C master send data and read data in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  pWriteBuf: point to the data to be sent.
  * @param  Writelen: the length of data that to be sent.
  * @param  pReadBuf: point to the buffer to hold the received data.
  * @param  Readlen: the length of data that to be received.  
  * @retval the length of data received. 
  */
int i2c_repeatread(i2c_t *obj, int address, u8 *pWriteBuf, int Writelen, u8 *pReadBuf, int Readlen) ;
///@}
#endif //(CONFIG_PLATFORM_8711B||CONFIG_PLATFORM_8721D)

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif/* MBED_I2C_API_H */
