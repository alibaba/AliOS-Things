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

#ifndef _FTL_H_
#define _FTL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus

/** @defgroup  FTL    Flash Transport Layer
    * @brief simple implementation of file system for flash
    * @{
    */
/** @defgroup FTL_Exported_Macros FTL Exported Macros
    * @brief
    * @{
    */
#define FTL_WRITE_SUCCESS                   (0x00)
#define FTL_WRITE_ERROR_INVALID_PARAMETER   (0x51)

#define FTL_READ_SUCCESS                    (0x00)
#define FTL_READ_ERROR_INVALID_PARAMETER    (0x51)

/** End of FTL_Exported_Macros
    * @}
    */
/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup FTL_Exported_Functions Flash Transport Layer Exported Functions
    * @brief
    * @{
    */
/**
    * @brief    Specified start offset of ftl
    *
    * The BT Lib will use FTL to save stack information and bond information.
    * The default value of start offset is the size of BT Lib used.
    *
    * @param    offset specify FTL start offset to store
    * @return   Configure result
    * @retval   true  Configure successful
    * @retval   false Configure fail
    */
bool ftl_config_start_offset(uint16_t offset);

/**
    * @brief    Save specified value to specified ftl offset
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to store
    *     @arg  Min: 0
    *     @arg  Max: 1020
    * @param    size   size to store
    *     @arg  Min: 4
    *     @arg  Max: 1024
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no conflict
    */
uint32_t ftl_save(void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified ftl offset parameter to specified buffer
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to load
    *     @arg  Min: 0
    *     @arg  Max: 1020
    * @param    size   size to load
    *     @arg  Min: 4
    *     @arg  Max: 1024
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no conflict
    */
uint32_t ftl_load(void *pdata, uint16_t offset, uint16_t size);

/** @} */ /* End of group FTL_Exported_Functions */


/** @} */ /* End of group FTL */

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // _FTL_H_
