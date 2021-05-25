/** mbed Microcontroller Library
  ******************************************************************************
  * @file    timer_api.h
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


#include "device.h"

#ifndef __RTK_DCT_H__
#define __RTK_DCT_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dct       DCT
 *  @ingroup    hal
 *  @brief      dct functions
 *  @{
 */

///@name Ameba Common 
///@{

 enum{
  DCT_SUCCESS = 0,     
  DCT_ERR = -1,       
  DCT_ERR_CRC = -2,     
  DCT_ERR_NO_SPACE = -3,   
  DCT_ERR_NO_MEMORY = -4,   
  DCT_ERR_FLASH_RW = -5,    
  DCT_ERR_NOT_FIND = -6,    
  DCT_ERR_INVALID = -7,   
  DCT_ERR_SIZE_OVER = -8,   
  DCT_ERR_MODULE_BUSY = -9,
};

enum{
  DCT_MODULE_STATE_INIT     = 0xFFFFFFFF,
  DCT_MODULE_STATE_VALID      = 0xFFFFFFFE,
  DCT_MODULE_STATE_DELETING   = 0xFFFFFFFC, 
  DCT_MODULE_STATE_DELETED    = 0xFFFFFFF8,
};




/**
  * @brief  Initialize device configuration table.
  * @param  none
  * @retval 32 bit 
  */
int32_t dct_init(void);

/**
 * @brief      Deinitialize device configuration table.
 * @retval     none
 */
void dct_deinit(void);

/**
 * @brief      Register module in DCT.
 * @param      module_name : module name
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_register_module(char *module_name);

/**
 * @brief  Unregister and delete module in DCT.
 * @param  module_name : module name
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_unregister_module(char *module_name);

/**
 * @brief  Open module in DCT.
 * @param  dct_handle : setup module informations in dct handler
 * @param  module_name : module name 
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_open_module(dct_handle_t *dct_handle, char *module_name);

/**
 * @brief  Close module in DCT.
 * @param  dct_handle : dct handler
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_close_module(dct_handle_t *dct_handle);

/**
 * @brief  Write variable name and value in opened module.
 * @param  dct_handle : dct handler
 * @param  variable_name : variable name which you want to store in module
 * @param  variable_value : variable value which you want to store in module
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_set_variable(dct_handle_t *dct_handle, char *variable_name, char *variable_value);

/**
 * @brief  Read value of variable name in opened module.
 * @param  dct_handle : dct handler
 * @param  variable_name : variable name which you want to get from module
 * @param  buffer : read variable value
 * @param  buffer_size : the buffer size
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_get_variable(dct_handle_t *dct_handle, char *variable_name, char *buffer, uint16_t buffer_size);

/**
 * @brief  Delete variable name and value in opened module.
 * @param  dct_handle : dct handler
 * @param  variable_name : variable name which you want to delete in module
 * @retval     0  : SUCCESS
 * @retval     <0 : ERROR
 */
int32_t dct_delete_variable(dct_handle_t *dct_handle, char *variable_name);




#ifdef __cplusplus
}
#endif

#endif/* MBED_TIMER_API_H */



























