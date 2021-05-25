/**
****************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file      ftl.h
* @brief     flash transport layer is used as abstraction layer for user application to save read/write
*            parameters in flash.
* @note      ftl is dedicate block in flash, only used for save read/write value, and offset here is
*            logical offset which is defined for whole ftl section.If value is only for one time read,
*            refer to fs_load_app_data_8 or other APIs in flash_device.h
* @author    Brenda_li
* @date      2016-12-27
* @version   v1.0
* **************************************************************************************************
*/

#ifndef _FTL_IF_H_
#define _FTL_IF_H_

#include <stdint.h>

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus

extern  uint32_t ftl_load_from_storage(void *pdata, uint16_t offset, uint16_t size);
extern  uint32_t ftl_save_to_storage(void *pdata, uint16_t offset, uint16_t size);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_IF_H_

